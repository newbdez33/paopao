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
    
    if (kindValue==-1) {
        //invalid paopao
        return;
    }
    
    CCString *name = CCString::createWithFormat("%d0.png", kindValue);
    this->initWithSpriteFrameName(name->getCString());
    
    //选中效果
    _glow = CCSprite::createWithSpriteFrameName("glow.png");
    _glow->setPosition(ccp(this->boundingBox().size.width/2, this->boundingBox().size.height/2));
    _glow->setVisible(false);
    this->addChild(_glow);

    //glow action
    CCFiniteTimeAction* sequence = CCSequence::create(
                                                      CCScaleTo::create(0.2, 0.95),
                                                      CCScaleTo::create(0.2, 0.9),
                                                      NULL
                                                      );
    _glowAction = CCRepeatForever::create((CCActionInterval*)sequence);
    _glowAction->retain();
    
}

CCPoint PaopaoSprite::positionOnScreen(int offsetX, int offsetY) {
    CCPoint pos = ccp(offsetX + x * PP_SIZE + PP_SIZE*0.5f, offsetY + y * PP_SIZE + PP_SIZE*0.5f);
    CCLog("x:%f, ppx:%f y:%f", pos.x, x*PP_SIZE, pos.y);
    return pos;
}

PaopaoSprite::~PaopaoSprite(void) {
    CC_SAFE_RELEASE(_glowAction);
}

void PaopaoSprite::glow(bool flag) {

    _glow->stopAllActions();
    
    if (!flag) {
        _glow->setVisible(false);
        return;
    }
    
    _glow->setVisible(true);
    _glow->runAction(_glowAction);
    
}
