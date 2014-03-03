//
//  MainMenu.cpp
//  paopao
//
//  Created by gavin on 14-2-12.
//
//

#include "MainMenu.h"
#include "GameScene.h"
#include "CCTransition.h"


using namespace cocos2d;

CCScene *MainMenu::scene()
{
    CCScene *scene = CCScene::create();
    
    MainMenu *layer = MainMenu::create();
    
    scene->addChild(layer);
    
    return scene;
}

bool MainMenu::init()
{
    //super init
    if (!CCLayer::init()) {
        return false;
    }

//    CCSize size = CCDirector::sharedDirector()->getWinSize();
    // CCMenuItem
    CCMenuItemImage *imageItemStart=CCMenuItemImage::create("restart_btn.png", "restart_btn.png",this,menu_selector(MainMenu::menu_game_start));
    imageItemStart->setPosition(CCPointZero);
    CCMenuItemImage *imageItemSet=CCMenuItemImage::create("restart_btn.png", "restart_btn.png",this,menu_selector(MainMenu::menu_game_seting));
    CCMenuItemImage *imageItemExit=CCMenuItemImage::create("restart_btn.png", "restart_btn.png",this,menu_selector(MainMenu::menu_game_exit));
    CCMenu *menu = CCMenu::create(imageItemStart,imageItemSet,imageItemExit,NULL);
    menu->alignItemsVerticallyWithPadding(40.0);
    this->addChild(menu,2);

    return true;
}


void MainMenu::menu_game_start()
{

    CCScene *pScene = GameScene::scene();
    CCTransitionFade* transitionScene = CCTransitionFade::create(0.5, pScene);
    //瓦片翻转效果
    //CCTransitionZoomFlipX * transitionScene = CCTransitionZoomFlipX::create(0.7, pScene);
    CCDirector::sharedDirector()->replaceScene(transitionScene);
    
    //淡入淡出
    //CCTransitionFade
    //瓦片翻转效果
    //CCTransitionFadeTR
    //旧场景弹跳缩小，新场景弹跳放大
    //CCTransitionZoomFlipX
    //旧场景缩小，新场景在其基础上放大
    //CCTransitionShrinkGrow
    //新场景呈瓦片替代旧场景
    //CCTransitionTurnOffTiles
}

void MainMenu::menu_game_seting()
{
    CCLog("seting ~~ ");
}

void MainMenu::menu_game_exit()
{
    CCLog("exit ~~");
}

