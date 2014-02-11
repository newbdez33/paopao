//
//  MainMenuScene.cpp
//  paopao
//
//  Created by gavin on 14-2-11.
//
//

#include "Menu.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"


using namespace cocos2d;
using namespace CocosDenshion;

CCScene* Menu::scene()
{
    //'scene'是一个自动释放的对象
    CCScene *scene = CCScene::create();
    
    // 'layer' 是一个自动释放的对象
    Menu *layer = Menu::create();
    
    //添加图层作为一个孩子到现场
    scene->addChild(layer);
    
    // 返回 scene
    return scene;
}

Menu::~Menu()
{
    //CPP不需要调用super dealloc的
    //虚析构函数会这么做
}

Menu::Menu()
:isEnter(false)
{
}

// 初始化你需要的实例
bool Menu::init()
{
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCSprite* pSprite = CCSprite::create("box2.png");
    // 定位在屏幕的中心的子画面
    pSprite->setPosition( ccp(size.width/2, size.height/2) );
    //添加精灵作为一个孩子到这一层
    this->addChild(pSprite, 0);
    
    CCLabelTTF* label = CCLabelTTF::create("泡泡对对碰", "Mark~", 48);
	label->setPosition(  ccp( size.width /2 , size.height/2 ) );
    this->addChild(label, 1);
	
	CCSprite* button = CCSprite::create("restart_btn.png");
    button->setPosition(  ccp(size.width/2, size.height/2 - 80) );
    this->addChild(button, 2);
	this->setTouchEnabled(true);
    
    return true;
}

bool Menu::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    if(!this->isEnter){
        this->scheduleOnce(schedule_selector(Menu::playGame), 0);
        this->isEnter = true;
    }
    return true;
}
void Menu::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
}

void Menu::registerWithTouchDispatcher()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority + 1, true);
}

void Menu::playGame(CCObject* pSender)
{
    CCScene *pScene = GameScene::scene();
    CCTransitionFade* transitionScene = CCTransitionFade::create(1.0, pScene,ccWHITE);
    CCDirector::sharedDirector()->replaceScene(transitionScene);
}

void Menu::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}