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
    
public:
    
    int kindValue;
    int x;
    int y;
    
    ~PaopaoSprite(void);
    PaopaoSprite(int ax, int ay, int akind);

	static PaopaoSprite* create(int ax, int ay, int akind);
    void initPaopao();
    
    CCPoint positionOnScreen(int offsetX, int offsetY);
    bool isNextTo(PaopaoSprite *other);
    
    //选中动画开关
    void glow(bool flag);
    
    void exchangedWith(PaopaoSprite *other);
};

#endif /* defined(__paopao__PaopaoSprite__) */
