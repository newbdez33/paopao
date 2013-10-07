#include "GameScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* GameScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene *layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

GameScene::GameScene() {
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("paopaos.plist");
    _gameBatchNode = CCSpriteBatchNode::create("paopaos.png", 49);
    this->addChild(_gameBatchNode, kMiddleground);
    
    _running = false;
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    //列
    _columns = CCArray::createWithCapacity(PP_BOX_COLUMNS);
    _columns->retain();
    
    //被消除的泡泡
    _matched = CCArray::createWithCapacity(PP_BOX_ROWS);    //capacity无所谓，变化的
    _matched->retain();
    
    _invalid = PaopaoSprite::create(-1, -1, -1);
    _invalid->retain();
    
    //消除效果粒子
    _eliminatePool = CCArray::createWithCapacity(10);
    _eliminatePool->retain();
    for (int i=0; i<10; i++) {
        CCParticleSystem *e = CCParticleSystemQuad::create("eliminate.plist");
        e->stopSystem();
        e->setVisible(false);
        _eliminatePool->addObject(e);
        this->addChild(e, kForeground);
    }
    _eliminateIdx = 0;
    
    //预载入纹理
    CCTextureCache::sharedTextureCache()->addImage("texture/1.png");
    CCTextureCache::sharedTextureCache()->addImage("texture/2.png");
    CCTextureCache::sharedTextureCache()->addImage("texture/3.png");
    CCTextureCache::sharedTextureCache()->addImage("texture/4.png");
    CCTextureCache::sharedTextureCache()->addImage("texture/5.png");
    CCTextureCache::sharedTextureCache()->addImage("texture/6.png");
    CCTextureCache::sharedTextureCache()->addImage("texture/7.png");
}

GameScene::~GameScene() {
    CC_SAFE_RELEASE(_columns);
    CC_SAFE_RELEASE(_matched);
    CC_SAFE_RELEASE(_invalid);
    CC_SAFE_RELEASE(_eliminatePool);
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    createGameScreen();
    
    resetGame();
    
    this->setTouchEnabled( true );
    this->scheduleUpdate();
    this->setUserInteractEnabled(true);
    
    return true;
}

#pragma mark - Game functions
void GameScene::createGameScreen() {
    
    //背景框
    CCSprite *backgroundBox = CCSprite::create("box2.png");
    backgroundBox->setPosition(ccp(_screenSize.width*0.5, _screenSize.height*0.5));
    this->addChild(backgroundBox);
    
    _boxOffsetX = (backgroundBox->getPositionX() - backgroundBox->boundingBox().size.width*0.5) +6;
    _boxOffsetY = (backgroundBox->getPositionY() - backgroundBox->boundingBox().size.height*0.5) +10;
    
}

void GameScene::resetGame() {
    
    _selected = NULL;
    _blinkTimer = 0;
    
    //1. 清空现在的box, 重新填充
    _columns->removeAllObjects();
    _gameBatchNode->removeAllChildren();

    //从左下角开始填充
    for (int y=0; y<PP_BOX_COLUMNS; y++) {
        CCArray *row = CCArray::createWithCapacity(PP_BOX_ROWS);
        for (int x=0; x<PP_BOX_ROWS; x++) {
            int kind = (arc4random()%PP_KIND_COUNT+1);
            PaopaoSprite *paopao = PaopaoSprite::create(x, y, kind);
            CCPoint pos = paopao->positionOnScreen(_boxOffsetX, _boxOffsetY);
            paopao->setPosition(pos);
            row->addObject(paopao);
            _gameBatchNode->addChild(paopao, kMiddleground);
        }
        _columns->addObject(row);
    }
    
    //检查是否已经存在3个以上的，并调整
    if(this->markAnyMatched()) {
        CCLog("调整");
        this->resetGame();
    }
    
    //检查是否可以继续游戏，如果不行，再次reset game
    if (!this->hasCandidate()) {
        this->resetGame();
    }
    
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bg.mp3", true);
}

void GameScene::findMatched() {
    
    bool h_done = false;    //横向
    bool v_done = false;    //纵向
    while (h_done == false || v_done == false) {
        
        //while block的逻辑执行两次，分别为横向和纵向
        Orientation o = OrientationHori;
        if (h_done==true) { //如果横向已经完成
            o = OrientationVert;
        }
        
        //循环全部
        for (int i=0; i<PP_BOX_COLUMNS; i++) {
            
            int matchCount = 0; //已经匹配的数量
            int kind = -1;      //
            PaopaoSprite *matched1 = NULL;
            PaopaoSprite *matched2 = NULL;
            PaopaoSprite *current = NULL;
            
            for (int j=0; j<PP_BOX_ROWS; j++) {
                //如果是水平方向检查
                if (o == OrientationHori) {
                    current = this->paopaoByXY(i, j);
                }else {
                    current = this->paopaoByXY(j, i);
                }
                
                //判断当前泡泡与上一个是否是一样的
                if (current->kindValue == kind) {
                    matchCount++;
                    if (matchCount > 3) {
                        _matched->addObject(current);
                    }else if( matchCount == 3 ) {
                        _matched->addObject(matched1);
                        _matched->addObject(matched2);
                        _matched->addObject(current);
                        matched1 = NULL;
                        matched2 = NULL;
                    }else if( matchCount == 2 ) {
                        matched2 = current;
                    }else {
                        //
                    }
                }else {
                    //如果不一样
                    matchCount = 1;
                    matched1 = current;
                    matched2 = NULL;
                    kind = current->kindValue;
                }
            }   //end for j
        }   //end for i
        
        //方向完成，标记
        if (o == OrientationHori) {
            h_done = true;
        }else {
            v_done = true;
        }
        
    }
    
    
}

bool GameScene::markAnyMatched() {

    //横纵检查
    this->findMatched();
    
    if (_matched->count() == 0) {
        //没有三消，返回false
        return false;
    }
    
    CCLog("找到%d个匹配", _matched->count() );
    for (int i=0; i<_matched->count(); i++) {
        PaopaoSprite *pp = (PaopaoSprite *)_matched->objectAtIndex(i);
        pp->setIsRemoved(true); //将该泡泡标记为移除状态
        //消除动画
        pp->runAction(
            CCRepeatForever::create(
                CCSequence::create(
                    CCMoveBy::create(0.03f, ccp(5,5)),
                    CCMoveBy::create(0.03f, ccp(-5,-5)),
                    NULL)));
        pp->runAction(CCSequence::create(CCScaleTo::create(0.2, 1.2), CCCallFunc::create(this, callfunc_selector(GameScene::removePaopaoFromScreen)), NULL));

    }
    
    //清空match数组
    _matched->removeAllObjects();
    
    this->print();
    CCLog("开始下降");
    
    //下降泡泡
    int maxFilledOnColumn = 0;
    for (int i=0; i<PP_BOX_COLUMNS; i++) {
        int filled = this->fillColumn(i);
        if (filled > maxFilledOnColumn) {
            maxFilledOnColumn = filled;
        }
    }
    
    this->print();
    
    //根据最大的下降泡泡数，做delay
    this->runAction(CCSequence::create(CCDelayTime::create(PP_MOVE_UNIT_TIME * maxFilledOnColumn + 0.03f + PP_ELIMINATE_TIME), CCCallFunc::create(this, callfunc_selector(GameScene::afterFillDone))));
    
    return true;
}

int GameScene::fillColumn(int idx) {
    
    int move_unit = 0;
    //从下往上，逐行循环
    for (int y=0; y<PP_BOX_ROWS; y++) {
        
        PaopaoSprite *p = this->paopaoByXY(idx, y);
        //这个泡泡是不是被消除了
        if (p->getIsRemoved()) {    //这是个已经消除的泡泡，不用下移
            move_unit++;
        }else if(move_unit==0) {    //之前没有消除的，所以这个泡泡不用下移
            //
        }else { //需要下移
            //向下移动动画
            p->runAction(CCSequence::create(CCDelayTime::create(PP_ELIMINATE_TIME), CCMoveBy::create(PP_MOVE_UNIT_TIME * move_unit, ccp(0, -PP_SIZE*move_unit)), CCCallFunc::create(p, callfunc_selector(PaopaoSprite::jump)), NULL));
            //在数组和自己的xy都要调整
            PaopaoSprite *replaced = this->paopaoByXY(idx, y - move_unit);  //这个位置是空的
            ((CCArray *)_columns->objectAtIndex(replaced->y))->replaceObjectAtIndex(idx, p);
            ((CCArray *)_columns->objectAtIndex(p->y))->replaceObjectAtIndex(idx, replaced);
            p->exchangedWith(replaced);
        }
    }
    
    for (int i=0; i<move_unit; i++) {
        //创建新泡泡，放到空位置上
        PaopaoSprite *p = this->paopaoByXY(idx, PP_BOX_ROWS-move_unit+i);
        int kind = (arc4random()%PP_KIND_COUNT+1);    //随机泡泡
        PaopaoSprite *np = PaopaoSprite::create(p->x, p->y, kind);
        np->setPosition(ccp(_boxOffsetX + idx * PP_SIZE + PP_SIZE/2, _boxOffsetY + (PP_BOX_ROWS + i) * PP_SIZE + PP_SIZE/2));
        _gameBatchNode->addChild(np);
        ((CCArray *)_columns->objectAtIndex(np->y))->replaceObjectAtIndex(np->x, np);
        np->runAction(CCSequence::create(CCDelayTime::create(PP_ELIMINATE_TIME), CCMoveBy::create(PP_MOVE_UNIT_TIME*move_unit, ccp(0, -PP_SIZE*move_unit)), CCCallFunc::create(np, callfunc_selector(PaopaoSprite::jump)), NULL));
    }
    
    return move_unit;
}

void GameScene::afterFillDone(cocos2d::CCNode *sender) {

    //还有匹配泡泡？
    if(this->markAnyMatched()) {
        return;
    }
    
    //游戏是否可以继续
    if (this->hasCandidate()) {
        this->setUserInteractEnabled(true);
    }else {
        CCLog("没有可以消除的了，游戏结束");
    }
}

void GameScene::removePaopaoFromScreen(PaopaoSprite *sender) {
    
    sender->stopAllActions();
    _gameBatchNode->removeChild(sender, true);
    
    if (_eliminateIdx >_eliminatePool->count()-1) {
        _eliminateIdx = 0;
    }
    CCString *name = CCString::createWithFormat("texture/%d.png", sender->kindValue);
    //CCLog("texture:%s", name->getCString());
    CCTexture2D* texture = CCTextureCache::sharedTextureCache()->textureForKey(name->getCString());
    CCParticleSystem *e = (CCParticleSystem *)_eliminatePool->objectAtIndex(_eliminateIdx);
    e->setTexture(texture);
    e->setPosition(sender->getPosition());
    e->resetSystem();
    e->setVisible(true);
    
    _eliminateIdx++;
}

bool GameScene::hasCandidate() {
    
    for (int y=0; y<PP_BOX_ROWS; y++)
    {
		for (int x=0; x<PP_BOX_COLUMNS; x++)
        {
            PaopaoSprite* aTile = this->paopaoByXY(x, y);   //当前tile
			//v 1 2
            /*
             向下相邻的情况
             X
             O  //y
             O
             */
			if (aTile->y-1 >= 0) {  //如果当前tile不在最底部
                PaopaoSprite* bTile = this->paopaoByXY(x, y-1);    //得到纵向的向下一个相邻tile
				if (aTile->kindValue == bTile->kindValue) {         //如果纵向花色一样，
					{
                        PaopaoSprite* cTile = this->paopaoByXY(x, y+2);    //得到纵向的向上两个相邻tile
						if (cTile->kindValue == aTile->kindValue)   //如果花色一样，则说明对调y+2到y+1就可以消除，返回true，可以消除
							return true;
					}
					{
                        PaopaoSprite* cTile = this->paopaoByXY(x-1, y+1);  //左一
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        PaopaoSprite* cTile = this->paopaoByXY(x+1, y+1);  //右一
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
                    
                    //开始 当前tile肯定是和下面一个相同，这就说明当前下2肯定不相同（不然就消除了）。那就再往下找3个，看是否相同
					{
                        PaopaoSprite* cTile = this->paopaoByXY(x,y-3);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        PaopaoSprite* cTile = this->paopaoByXY(x-1, y-2);  //或者下2的左右是否有相同
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        PaopaoSprite* cTile = this->paopaoByXY(x+1, y-2);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
                    //结束
                }
			}
            
            //上面处理的是当前tile下面相邻。
            
            //下面是找向下两个，即
            /*
             O  //y
             X
             O
             */
            //的情况
			//v 1 3
			if (aTile->y-2 >= 0) {
				PaopaoSprite *bTile = this->paopaoByXY(x, y-2);
				if (aTile->kindValue == bTile->kindValue)
                {
					{
                        /*
                         O      //y+1
                         O      //y
                         X
                         O
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x, y+1);    //覆盖了第一组逻辑中缺少的向上相邻判断
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         O      //y
                         X
                         O
                         O      //y-3
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x,y-3);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         O        //y
                         O X        //x-1, y-1
                         O
                         O
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x-1, y-1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         O      //y
                         X O    //y-1, x+1
                         O
                         O
                         */
                        
                        PaopaoSprite* cTile = this->paopaoByXY(x+1, y-1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
				}
			}
            
			// h 1 2
			if (aTile->x+1 < PP_BOX_COLUMNS) {
                PaopaoSprite* bTile = this->paopaoByXY(x+1, y);
                //右相邻组判断
				if (aTile->kindValue == bTile->kindValue) {
					{
                        /*
                         O X O O    //x-2, x, x+1
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x-2, y);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         X O O    //x, x+1
                         O        //x-1 & y-1
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x-1, y-1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         O        //x-1 & y+1
                         X O O    //x, x+1
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x-1, y+1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         X O O X O    //x, x+1, x+3
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x+3, y);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         X O O X    //x, x+1
                         O    //x+2 & y-1
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x+2, y-1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         O    //x+2 & y+1
                         X O O X    //x, x+1
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x+2, y+1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
				}
			}
            
			//h 1 3
			if (aTile->x+2 >= PP_BOX_COLUMNS) {
                PaopaoSprite* bTile = this->paopaoByXY(x+2, y);
                //右间邻组  X O X O   //x, x+2
				if (aTile->kindValue == bTile->kindValue)
                {
					{
                        //X O X O O   //x, x+2, x+3
                        PaopaoSprite* cTile = this->paopaoByXY(x+3, y);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        //O O X O   //x-1, x, x+2
                        PaopaoSprite* cTile = this->paopaoByXY(x-1, y);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         O     //x+1 & y-1
                         O X O   //x, x+2
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x+1, y-1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
					{
                        /*
                         O X O //x, x+2
                         O   //x+1 & y+1
                         */
                        PaopaoSprite* cTile = this->paopaoByXY(x+1, y+1);
						if (cTile->kindValue == aTile->kindValue)
							return true;
					}
				}
			}
		}
	}
	return false;
}

PaopaoSprite *GameScene::paopaoByXY(int x, int y) {
    if (x < 0 || x >= PP_BOX_COLUMNS || y < 0 || y >= PP_BOX_ROWS)
		return _invalid;
    
    return (PaopaoSprite* )((CCArray*)_columns->objectAtIndex(y))->objectAtIndex(x);
}

void GameScene::exchange(PaopaoSprite *selected1, PaopaoSprite *selected2, SEL_CallFuncND sel) {
    
    selected1->glow(false);
    selected2->glow(false);
    
    CCFiniteTimeAction* move1 = CCSequence::create(CCMoveTo::create(PP_MOVE_UNIT_TIME, selected2->positionOnScreen(_boxOffsetX, _boxOffsetY)),
                                                     CCCallFuncND::create(this, sel, selected1),
                                                     NULL);
    CCFiniteTimeAction* move2 = CCSequence::create(CCMoveTo::create(PP_MOVE_UNIT_TIME, selected1->positionOnScreen(_boxOffsetX, _boxOffsetY)),
                                                     CCCallFuncND::create(this, sel, selected2),
                                                     NULL);
    
    CCLog("开始动画交换");
    selected1->runAction(move1);
    selected2->runAction(move2);
    CCLog("开始业务交换");
    
    //然后在数组中也换位置
    CCArray *row = (CCArray *)_columns->objectAtIndex( selected1->y );
    row->replaceObjectAtIndex(selected1->x, selected2);
    row = (CCArray *)_columns->objectAtIndex( selected2->y );
    row->replaceObjectAtIndex(selected2->x, selected1);
    
    //交换一下泡泡的xy
    selected1->exchangedWith(selected2);
    
}

void GameScene::afterExchange(cocos2d::CCNode *sender, PaopaoSprite *paopao) {
    if (_exchanged1==NULL) {
        _exchanged1 = paopao;
        CCLog("第一个交换动画完成");
        return;
    }
    
    if (this->markAnyMatched()) {
        CCLog("交换成功");
        this->setUserInteractEnabled(true);
    }else {
        CCLog("非法交换，REVERT");
        //this->setUserInteractEnabled(true); //临时测试
        this->exchange(paopao, _exchanged1, callfuncND_selector(GameScene::revertExchange));
    }
    _exchanged1 = NULL;
}

void GameScene::revertExchange(cocos2d::CCNode *sender, PaopaoSprite *paopao) {
    if (_exchanged1==NULL) {
        _exchanged1 = paopao;
        return;
    }
    _exchanged1 = NULL;
    this->setUserInteractEnabled(true);
}

void GameScene::print() {
    for (int y=0; y<_columns->count(); y++) {
        CCArray *row = (CCArray *)_columns->objectAtIndex(y);
        for (int x=0; x<row->count(); x++) {
            PaopaoSprite *p = (PaopaoSprite *)row->objectAtIndex(x);
            p->print(CCString::createWithFormat("@[%d,%d] ===", x, y)->getCString());
        }
    }
}


#pragma mark - Cocos2d Events
void GameScene::update(float dt) {

    float r = (arc4random()%400+1)/100.0f + 0.5;
    if (_blinkTimer>r) {
        
        _blinkTimer = 0;
        
        int rx  = arc4random()%PP_BOX_ROWS;
        int ry = arc4random()%PP_BOX_COLUMNS;
        PaopaoSprite *p = this->paopaoByXY(rx, ry);
        
        p->blink();
    }
    _blinkTimer+=dt;
    
}

void GameScene::ccTouchesBegan(CCSet* pTouches, CCEvent* event) {
    
    if (!this->getUserInteractEnabled()) {
        CCLog("动画中，不允许操作");
        return;
    }
    
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    if (!touch) return;
    
    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL( location );
    //CCLog("touched x:%f, y:%f",  location.x, location.y);
    
    int x = (location.x - _boxOffsetX) / PP_SIZE;
	int y = (location.y - _boxOffsetY) / PP_SIZE;
    
    CCLog("touched x:%d, y:%d",  x, y);
    
    if (_selected && _selected->x==x && _selected->y==y) {
        return;
    }

    CCLog("getting x:%d, y:%d", x, y);
    PaopaoSprite *pp = this->paopaoByXY(x, y);
    CCLog("got kind:%d, x:%d, y:%d", pp->kindValue, pp->x, pp->y);
    if (_selected && _selected->isNextTo(pp)) {
        
        this->setUserInteractEnabled(false);
        //选中了第二个泡泡
        this->exchange(_selected, pp, callfuncND_selector(GameScene::afterExchange));
        _selected = NULL;
    }else {
        //第一个泡泡被选中
        if (_selected!=NULL) {
            _selected->glow(false);
        }
        if (pp->kindValue!=-1) {
            pp->glow(true);
            _selected = pp;
        }
    }
}

void GameScene::onEnterTransitionDidFinish() {
    CCLog("onEnterTransitionDidFinish进入");
}
