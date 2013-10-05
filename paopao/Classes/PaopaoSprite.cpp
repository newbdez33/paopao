//
//  PaopaoSprite.cpp
//  paopao
//
//  Created by Jack on 1/10/13.
//
//

#include "PaopaoSprite.h"

using namespace cocos2d;

PaopaoSprite::PaopaoSprite(int ax, int ay, int akind) {
    
    kindValue = akind;
    x = ax;
    y = ay;
}

PaopaoSprite * PaopaoSprite::create(int ax, int ay, int akind) {
    PaopaoSprite *sprite = new PaopaoSprite(ax, ay, akind);
    if (sprite) {
        sprite->initPaopao();
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_RELEASE(sprite);
    return NULL;
}

void PaopaoSprite::initPaopao() {
    
    CCString *name = CCString::createWithFormat("%d0.png", kindValue);
    this->initWithSpriteFrameName(name->getCString());
}

CCPoint PaopaoSprite::positionOnScreen(int offsetX, int offsetY) {
    CCPoint pos = ccp(offsetX + x * PP_SIZE + PP_SIZE*0.5f, offsetY + y * PP_SIZE + PP_SIZE*0.5f);
    CCLog("x:%f, ppx:%f y:%f", pos.x, x*PP_SIZE, pos.y);
    return pos;
}

PaopaoSprite::~PaopaoSprite(void) {}
