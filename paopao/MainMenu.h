//
//  MainMenu.h
//  paopao
//
//  Created by gavin on 14-2-12.
//
//

#ifndef __paopao__MainMenu__
#define __paopao__MainMenu__

#include <iostream>
#include "cocos2d.h"

class MainMenu : public cocos2d::CCLayer
{
public:
    virtual bool init();
    
    static cocos2d::CCScene *scene();
    
    void menu_game_start();
    void menu_game_seting();
    void menu_game_exit();
    
    CREATE_FUNC(MainMenu);
};

#endif /* defined(__paopao__MainMenu__) */
