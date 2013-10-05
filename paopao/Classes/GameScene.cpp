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
}

GameScene::~GameScene() {
    CC_SAFE_RELEASE(_columns);
    CC_SAFE_RELEASE(_tobeRemoved);
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
    
    
    return true;
}

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
    for (int y=PP_BOX_COLUMNS-1; y>=0; y--) {
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

void GameScene::markAnyMatched() {
    //TODO
}

bool GameScene::hasCandidate() {
    //TODO
    return true;
}

void GameScene::update(float dt) {
    
}

void GameScene::ccTouchesBegan(CCSet* pTouches, CCEvent* event) {

    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    if (!touch) return;

    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL( location );
    //CCLog("touched x:%f, y:%f",  location.x, location.y);
    
    int x = (location.x - _boxOffsetX) / PP_SIZE;
	int y = (location.y - _boxOffsetY) / PP_SIZE;
    
    CCLog("touched x:%d, y:%d",  x, y);
    
    if (_selected && _selected->x==x && _selected->y==y) {
        //
    }
}

void GameScene::onEnterTransitionDidFinish() {
    CCLog("onEnterTransitionDidFinish进入");
}
