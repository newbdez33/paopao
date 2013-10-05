#include "GameScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* GameScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene *layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

GameScene::GameScene() {
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("paopaos.plist");
    _gameBatchNode = CCSpriteBatchNode::create("paopaos.png", 49);
    this->addChild(_gameBatchNode, kMiddleground);
    
    _running = false;
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    //列
    _columns = CCArray::createWithCapacity(PP_BOX_COLUMNS);
    _columns->retain();
    
    //被消除的泡泡
    _tobeRemoved = CCArray::createWithCapacity(PP_BOX_ROWS);    //capacity无所谓，变化的
    _tobeRemoved->retain();
    
    _invalid = PaopaoSprite::create(-1, -1, -1);
    _invalid->retain();
}

GameScene::~GameScene() {
    CC_SAFE_RELEASE(_columns);
    CC_SAFE_RELEASE(_tobeRemoved);
    CC_SAFE_RELEASE(_invalid);
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    createGameScreen();
    
    resetGame();
    
    this->setTouchEnabled( true );
    this->scheduleUpdate();
    this->setUserInteractEnabled(true);
    
    return true;
}

#pragma mark - Game functions
void GameScene::createGameScreen() {
    
    //背景框
    CCSprite *backgroundBox = CCSprite::create("box2.png");
    backgroundBox->setPosition(ccp(_screenSize.width*0.5, _screenSize.height*0.5));
    this->addChild(backgroundBox);
    
    _boxOffsetX = (backgroundBox->getPositionX() - backgroundBox->boundingBox().size.width*0.5) +6;
    _boxOffsetY = (backgroundBox->getPositionY() - backgroundBox->boundingBox().size.height*0.5) +10;
    
}

void GameScene::resetGame() {
    
    _selected = NULL;
    
    //1. 清空现在的box, 重新填充
    _columns->removeAllObjects();
    
    //从左上角开始填充
    for (int y=0; y<PP_BOX_COLUMNS; y++) {
        CCArray *row = CCArray::createWithCapacity(PP_BOX_ROWS);
        for (int x=0; x<PP_BOX_ROWS; x++) {
            int kind = (arc4random()%PP_KIND_COUNT+1);
            PaopaoSprite *paopao = PaopaoSprite::create(x, y, kind);
            CCPoint pos = paopao->positionOnScreen(_boxOffsetX, _boxOffsetY);
            paopao->setPosition(pos);
            row->addObject(paopao);
            _gameBatchNode->addChild(paopao, kMiddleground);
        }
        _columns->addObject(row);
    }
    
    //检查是否已经存在3个以上的，并调整
    this->markAnyMatched();
    
    //检查是否可以继续游戏，如果不行，再次reset game
    if (!this->hasCandidate()) {
        this->resetGame();
    }
}

bool GameScene::markAnyMatched() {
    //TODO
    return false;
}

bool GameScene::hasCandidate() {
    //TODO
    return true;
}

PaopaoSprite *GameScene::paopaoByXY(int x, int y) {
    if (x < 0 || x >= PP_BOX_COLUMNS || y < 0 || y >= PP_BOX_ROWS)
		return _invalid;
    
    return (PaopaoSprite* )((CCArray*)_columns->objectAtIndex(y))->objectAtIndex(x);
}

void GameScene::exchange(PaopaoSprite *selected1, PaopaoSprite *selected2, SEL_CallFuncND sel) {
    
    selected1->glow(false);
    selected2->glow(false);
    
    CCFiniteTimeAction* move1 = CCSequence::create(CCMoveTo::create(PP_MOVE_UNIT_TIME, selected2->positionOnScreen(_boxOffsetX, _boxOffsetY)),
                                                     CCCallFuncND::create(this, sel, selected1),
                                                     NULL);
    CCFiniteTimeAction* move2 = CCSequence::create(CCMoveTo::create(PP_MOVE_UNIT_TIME, selected1->positionOnScreen(_boxOffsetX, _boxOffsetY)),
                                                     CCCallFuncND::create(this, sel, selected2),
                                                     NULL);
    
    CCLog("开始动画交换");
    selected1->runAction(move1);
    selected2->runAction(move2);
    CCLog("开始业务交换");
    
    //然后在数组中也换位置
    CCArray *row = (CCArray *)_columns->objectAtIndex( selected1->y );
    row->replaceObjectAtIndex(selected1->x, selected2);
    row = (CCArray *)_columns->objectAtIndex( selected2->y );
    row->replaceObjectAtIndex(selected2->x, selected1);
    
    //交换一下泡泡的xy
    selected1->exchangedWith(selected2);
    
}

void GameScene::afterExchange(cocos2d::CCNode *sender, PaopaoSprite *paopao) {
    if (_exchanged1==NULL) {
        _exchanged1 = paopao;
        CCLog("第一个交换动画完成");
        return;
    }
    
    if (this->markAnyMatched()) {
        CCLog("交换成功");
        this->setUserInteractEnabled(true);
    }else {
        CCLog("非法交换，REVERT");
        //this->setUserInteractEnabled(true); //临时测试
        this->exchange(paopao, _exchanged1, callfuncND_selector(GameScene::revertExchange));
    }
    _exchanged1 = NULL;
}

void GameScene::revertExchange(cocos2d::CCNode *sender, PaopaoSprite *paopao) {
    if (_exchanged1==NULL) {
        _exchanged1 = paopao;
        return;
    }
    _exchanged1 = NULL;
    this->setUserInteractEnabled(true);
}


#pragma mark - Cocos2d Events
void GameScene::update(float dt) {
    
}

void GameScene::ccTouchesBegan(CCSet* pTouches, CCEvent* event) {
    
    if (!this->getUserInteractEnabled()) {
        CCLog("动画中，不允许操作");
        return;
    }
    
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    if (!touch) return;
    
    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL( location );
    //CCLog("touched x:%f, y:%f",  location.x, location.y);
    
    int x = (location.x - _boxOffsetX) / PP_SIZE;
	int y = (location.y - _boxOffsetY) / PP_SIZE;
    
    CCLog("touched x:%d, y:%d",  x, y);
    
    if (_selected && _selected->x==x && _selected->y==y) {
        return;
    }

    CCLog("getting x:%d, y:%d", x, y);
    PaopaoSprite *pp = this->paopaoByXY(x, y);
    CCLog("got kind:%d, x:%d, y:%d", pp->kindValue, pp->x, pp->y);
    if (_selected && _selected->isNextTo(pp)) {
        
        this->setUserInteractEnabled(false);
        //选中了第二个泡泡
        this->exchange(_selected, pp, callfuncND_selector(GameScene::afterExchange));
        _selected = NULL;
    }else {
        //第一个泡泡被选中
        CCLog("ppx:%f, ppy:%f", pp->getPosition().x, pp->getPosition().y);
        pp->glow(true);
        _selected = pp;
    }
}

void GameScene::onEnterTransitionDidFinish() {
    CCLog("onEnterTransitionDidFinish进入");
}
