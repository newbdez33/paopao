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
    this->setIsRemoved(false);
    _isBlinking = false;
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
    
    CCAnimation* animation;
    animation = CCAnimation::create();
    CCSpriteFrame * frame;
    int i;
    //animation for ground hit
    for(i = 1; i <= 2; i++) {
        CCString *name = CCString::createWithFormat("%d%d.png", this->kindValue, i);
        frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name->getCString());
        animation->addSpriteFrame(frame);
    }
    animation->setDelayPerUnit(0.05);
    animation->setRestoreOriginalFrame(true);
    _blinkAction = CCSequence::create(CCDelayTime::create(0.1), CCAnimate::create(animation), CCCallFunc::create(this, callfunc_selector(PaopaoSprite::afterBlink)), NULL);
    _blinkAction->retain();
    
    animation = CCAnimation::create();
    for(i = 3; i <= 4; i++) {
        CCString *name = CCString::createWithFormat("%d%d.png", this->kindValue, i);
        frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name->getCString());
        animation->addSpriteFrame(frame);
    }
    animation->setDelayPerUnit(0.05);
    animation->setRestoreOriginalFrame(true);
    _jumpAction = CCAnimate::create(animation);
    _jumpAction->retain();
}

void PaopaoSprite::afterBlink() {
    _isBlinking = false;
}

CCPoint PaopaoSprite::positionOnScreen(int offsetX, int offsetY) {
    CCPoint pos = ccp(offsetX + x * PP_SIZE + PP_SIZE*0.5f, offsetY + y * PP_SIZE + PP_SIZE*0.5f);
    //CCLog("x:%f, ppx:%f y:%f", pos.x, x*PP_SIZE, pos.y);
    return pos;
}

PaopaoSprite::~PaopaoSprite(void) {
    CC_SAFE_RELEASE(_glowAction);
    CC_SAFE_RELEASE(_blinkAction);
    CC_SAFE_RELEASE(_jumpAction);
}

bool PaopaoSprite::isNextTo(PaopaoSprite *other) {
    return
	(x == other->x && abs(y - other->y)==1)||
	(y == other->y && abs(x - other->x)==1);
}

void PaopaoSprite::glow(bool flag) {

    if (_glow) {
        _glow->stopAllActions();
    }
    
    if (!flag) {
        _glow->setVisible(false);
        return;
    }
    
    _glow->setVisible(true);
    _glow->runAction(_glowAction);
    
}

void PaopaoSprite::exchangedWith(PaopaoSprite *other) {
    
    int tmpX = other->x;
    int tmpY = other->y;
    //int tmpKind = other->kindValue;
    
    other->x = this->x;
    other->y = this->y;
    //other->kindValue = this->kindValue;
    //CCString *name = CCString::createWithFormat("%d0.png", other->kindValue);
    //other->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name->getCString()));
    
    this->x = tmpX;
    this->y = tmpY;
    //this->kindValue = tmpKind;
    //name = CCString::createWithFormat("%d0.png", this->kindValue);
    //this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name->getCString()));
}

void PaopaoSprite::blink() {

    if (_blinkAction && _isBlinking==false) {
        _isBlinking = true;
        this->runAction(_blinkAction);
    }
}

void PaopaoSprite::jump() {
    if (_jumpAction) {
        this->runAction(_jumpAction);
    }
}

void PaopaoSprite::print(const char *prefix) {
    CCLog("%s == ptr:%p x:%d, y:%d, kind:%d, isRemoved:%d, isVisiable:%d", prefix, this, this->x, this->y, this->kindValue, this->getIsRemoved()?1:0, this->isVisible()?1:0);
}

