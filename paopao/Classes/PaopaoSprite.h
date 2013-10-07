//
//  PaopaoSprite.h
//  paopao
//
//  Created by Jack on 1/10/13.
//
//

#ifndef __paopao__PaopaoSprite__
#define __paopao__PaopaoSprite__

#include "cocos2d.h"

using namespace cocos2d;

class PaopaoSprite : public CCSprite {
private:
    
    CCSprite *_glow;
    CCAction *_glowAction;
    CCAction *_blinkAction;
    
    bool _isBlinking;
    
public:
    
    int kindValue;
    int x;
    int y;
    
    CC_SYNTHESIZE(bool, isRemoved, IsRemoved);
    
    ~PaopaoSprite(void);
    PaopaoSprite(int ax, int ay, int akind);

	static PaopaoSprite* create(int ax, int ay, int akind);
    void initPaopao();
    
    CCPoint positionOnScreen(int offsetX, int offsetY);
    bool isNextTo(PaopaoSprite *other);
    
    //选中动画开关
    void glow(bool flag);
    void blink();
    void afterBlink();
    
    void exchangedWith(PaopaoSprite *other);
    void print(const char *prefix);
};

#endif /* defined(__paopao__PaopaoSprite__) */
