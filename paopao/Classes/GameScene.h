#ifndef __GameScene_SCENE_H__
#define __GameScene_SCENE_H__

#include "cocos2d.h"
#include "PaopaoSprite.h"

using namespace cocos2d;

class GameScene : public cocos2d::CCLayer
{
    
    CCSpriteBatchNode * _gameBatchNode;
    
    CCArray *_columns;
    CCArray *_tobeRemoved;
    float _boxOffsetX;
    float _boxOffsetY;
    
    PaopaoSprite *_selected;
    
    bool _running;
    CCSize _screenSize;
    
    void createGameScreen();
    void resetGame();
    
    //检查并将消除的泡泡打标记
    void markAnyMatched();
    //检查是否还存在进一步消除的可能
    bool hasCandidate();
    
public:
    GameScene();
    ~GameScene();
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();

    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(GameScene);
    void update (float dt);
    virtual void ccTouchesBegan(CCSet* pTouches, CCEvent* event);
    
    //////
    void onEnterTransitionDidFinish();
};

#endif // __GameScene_SCENE_H__
