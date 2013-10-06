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
    _matched = CCArray::createWithCapacity(PP_BOX_ROWS);    //capacity无所谓，变化的
    _matched->retain();
    
    _invalid = PaopaoSprite::create(-1, -1, -1);
    _invalid->retain();
    
    //消除效果粒子
//    _eliminate = CCParticleSystemQuad::create("eliminate.plist");
//    _eliminate->stopSystem();
//    _eliminate->setVisible(false);
//    this->addChild(_eliminate, kForeground);
}

GameScene::~GameScene() {
    CC_SAFE_RELEASE(_columns);
    CC_SAFE_RELEASE(_matched);
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

    //从左下角开始填充
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
    //this->markAnyMatched();
    
    //检查是否可以继续游戏，如果不行，再次reset game
    if (!this->hasCandidate()) {
        this->resetGame();
    }
}

void GameScene::findMatched() {
    
    bool h_done = false;    //横向
    bool v_done = false;    //纵向
    while (h_done == false || v_done == false) {
        
        //while block的逻辑执行两次，分别为横向和纵向
        Orientation o = OrientationHori;
        if (h_done==true) { //如果横向已经完成
            o = OrientationVert;
        }
        
        //循环全部
        for (int i=0; i<PP_BOX_COLUMNS; i++) {
            
            int matchCount = 0; //已经匹配的数量
            int kind = -1;      //
            PaopaoSprite *matched1 = NULL;
            PaopaoSprite *matched2 = NULL;
            PaopaoSprite *current = NULL;
            
            for (int j=0; j<PP_BOX_ROWS; j++) {
                //如果是水平方向检查
                if (o == OrientationHori) {
                    current = this->paopaoByXY(i, j);
                }else {
                    current = this->paopaoByXY(j, i);
                }
                
                //判断当前泡泡与上一个是否是一样的
                if (current->kindValue == kind) {
                    matchCount++;
                    if (matchCount > 3) {
                        _matched->addObject(current);
                    }else if( matchCount == 3 ) {
                        _matched->addObject(matched1);
                        _matched->addObject(matched2);
                        _matched->addObject(current);
                        matched1 = NULL;
                        matched2 = NULL;
                    }else if( matchCount == 2 ) {
                        matched2 = current;
                    }else {
                        //
                    }
                }else {
                    //如果不一样
                    matchCount = 1;
                    matched1 = current;
                    matched2 = NULL;
                    kind = current->kindValue;
                }
            }   //end for j
        }   //end for i
        
        //方向完成，标记
        if (o == OrientationHori) {
            h_done = true;
        }else {
            v_done = true;
        }
        
    }
    
    
}

bool GameScene::markAnyMatched() {

    //横纵检查
    this->findMatched();
    
    if (_matched->count() == 0) {
        //没有三消，返回false
        return false;
    }
    
    CCLog("找到%d个匹配", _matched->count() );
    for (int i=0; i<_matched->count(); i++) {
        PaopaoSprite *pp = (PaopaoSprite *)_matched->objectAtIndex(i);
        pp->setIsRemoved(true); //将该泡泡标记为移除状态
        pp->runAction(
            CCRepeatForever::create(
                CCSequence::create(
                    CCMoveBy::create(0.03f, ccp(5,5)),
                    CCMoveBy::create(0.03f, ccp(-5,-5)),
                    NULL)));
        pp->runAction(CCSequence::create(CCScaleTo::create(0.3, 1.2), CCCallFunc::create(this, callfunc_selector(GameScene::removePaopaoFromScreen)), NULL));

    }
    return true;
}

void GameScene::removePaopaoFromScreen(PaopaoSprite *sender) {
    _gameBatchNode->removeChild(sender, true);
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
        if (_selected!=NULL) {
            _selected->glow(false);
        }
        CCLog("ppx:%f, ppy:%f", pp->getPosition().x, pp->getPosition().y);
        pp->glow(true);
        _selected = pp;
    }
}

void GameScene::onEnterTransitionDidFinish() {
    CCLog("onEnterTransitionDidFinish进入");
}
