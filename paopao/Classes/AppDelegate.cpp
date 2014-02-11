//
//  paopaoAppDelegate.cpp
//  paopao
//
//  Created by Jack on 28/09/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#include "AppDelegate.h"

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "Menu.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    CCEGLView *pEGLView = CCEGLView::sharedOpenGLView();
    pDirector->setOpenGLView(pEGLView);
    
    //设置view size
    CCSize screenSize = pEGLView->getFrameSize();
    CCSize designSize = CCSize(320, 480);
    
    CCEGLView::sharedOpenGLView()->setDesignResolutionSize(designSize.width, designSize.height, kResolutionShowAll);
    
    //设置资源路径
    std::vector<std::string> searchPaths;
    if (screenSize.width > 320 ) {
        searchPaths.push_back("iphonehd");
        pDirector->setContentScaleFactor(640/designSize.width);
    }else {
        searchPaths.push_back("iphone");
        pDirector->setContentScaleFactor(320/designSize.width);
    }
    CCFileUtils::sharedFileUtils()->setSearchPaths(searchPaths);
    
    //预载入音效
    SimpleAudioEngine::sharedEngine()->preloadEffect( CCFileUtils::sharedFileUtils()->fullPathForFilename("start.wav").c_str() );

    // turn on display FPS
    pDirector->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
//    CCScene *pScene = GameScene::scene();
    
    //game menu scene
    CCScene *menu = Menu::scene();

    // run
    pDirector->runWithScene(menu);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->stopAnimation();
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
    
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->startAnimation();
    CCScene *pScene = pDirector->getRunningScene();
    GameScene *layer = (GameScene *)pScene->getChildren()->objectAtIndex(0);
    if (layer->isMuted()) {
        SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    }else {
        SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    }
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
    
}
