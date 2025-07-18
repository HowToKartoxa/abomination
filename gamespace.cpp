#include "gamespace.h"
#include "ui_gamespace.h"
#include <QMouseEvent>

GameSpace::GameSpace(QWidget *parent, int difficulty, bool _showVectors, bool _useFullscreen, bool _playSounds)
    : QWidget(parent)
    , ui(new Ui::GameSpace)
    , nightmareMode(false)
    , showVectors(_showVectors)
    , useFullscreen(_useFullscreen)
    , playSounds(_playSounds)
    , zub(nullptr)
    , zubs()
    , updateTimer(nullptr)
    , cursorAnimationUpdateTimer(nullptr)
    , cursorMovementStatsUpdateTimer(nullptr)
    , gameGoing(false)
    , cursorFrames()
    , clicks(0)
    , hits(0)
    , cursorDistanceTravelled(0)
{
    srand(time(0));
    ui->setupUi(this);
    if(useFullscreen) showFullScreen();
    else show();
    if(difficulty == 0){
        zub = new zubzub(Vector2(width() / 2 // start pos
                         , height() / 2)     //
                         , 20                // mass
                         , 3    // hp
                         , 30   // forceMult
                         , 60   // borderForceMult
                         , 0.01 // dragCoeff
                         , 50   // maxSpeed
                         , width()           // xLimit
                         , height()          // yLimit
                         , 8);               // ambulanceSpeed
    }
    else if(difficulty == 1){
        zub = new zubzub(Vector2(width() / 2 // start pos
                                 , height() / 2)     //
                         , 25                // mass
                         , 6    // hp
                         , 70   // forceMult
                         , 120   // borderForceMult
                         , 0.01 // dragCoeff
                         , 50   // maxSpeed
                         , width()           // xLimit
                         , height()          // yLimit
                         , 8);
    }
    else if(difficulty == 2){
        zub = new zubzub(Vector2(width() / 2 // start pos
                                 , height() / 2)     //
                         , 40               // mass
                         , 10    // hp
                         , 230   // forceMult
                         , 400   // borderForceMult
                         , 0.01 // dragCoeff
                         , 90   // maxSpeed
                         , width()           // xLimit
                         , height()          // yLimit
                         , 8);
    }
    else{
        nightmareMode = true;
        for(int i = 0; i < 3; i ++){
            zubs.push_back(new zubzub(Vector2(width() / 2 + (rand() % 6) - 2// start pos
                                              , height() / 2 + (rand() % 6) - 2)     //
                                      , 40               // mass
                                      , 10    // hp
                                      , 230   // forceMult
                                      , 400   // borderForceMult
                                      , 0.01 // dragCoeff
                                      , 90   // maxSpeed
                                      , width()           // xLimit
                                      , height()          // yLimit
                                      , 8));
        }

    }
    updateTimer = new QTimer(this);
    cursorAnimationUpdateTimer = new QTimer(this);
    cursorMovementStatsUpdateTimer = new QTimer(this);
    updateTimer->setInterval(10);
    cursorAnimationUpdateTimer->setInterval(12);
    cursorMovementStatsUpdateTimer->setInterval(100);
    connect(updateTimer, &QTimer::timeout, this, &GameSpace::updateEvent);
    connect(cursorAnimationUpdateTimer, &QTimer::timeout, this, &GameSpace::cursorUpdateEvent);
    connect(cursorMovementStatsUpdateTimer, &QTimer::timeout, this, &GameSpace::cursorMovementEvent);
    updateTimer->start();

    QFileInfoList lst = QDir(":/bat/cursor").entryInfoList();
    unsigned int i = 0;
    unsigned int length = lst.length();
    QPixmap* temp;
    while(i != length){
        temp = new QPixmap(lst[i].absoluteFilePath());
        *temp = temp->scaledToHeight(50, Qt::SmoothTransformation);
        cursorFrames.push_back(temp);
        i++;
    }
    currentCursorFrame = 0;

    this->setCursor(QCursor(*cursorFrames[0], 46, 7));

    if(_playSounds){
        mainTheme = new QSoundEffect(this);
        mainTheme->setSource(QUrl(QString("qrc:///snd/sounds/mainTheme%1.wav").arg(rand()%3)));
        mainTheme->setVolume(0.2f);
        mainTheme->play();
        connect(mainTheme, &QSoundEffect::playingChanged, this, &GameSpace::changeTheme);
    }

    if(nightmareMode){
        connect(zubs[0], &zubzub::died, this, &GameSpace::onZubZubDied);
        connect(zubs[1], &zubzub::died, this, &GameSpace::onZubZubDied);
        connect(zubs[2], &zubzub::died, this, &GameSpace::onZubZubDied);
    }
    else{
        connect(zub, &zubzub::died, this, &GameSpace::onZubZubDied);
    }
    gameStartedAt = QTime::currentTime();
    cursorMovementStatsUpdateTimer->start();
    gameGoing = true;
}

void GameSpace::updateEvent(){
    if(gameGoing){
        if(nightmareMode){
            zubs[0]->physicsProcess(Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), 25);
            zubs[1]->physicsProcess(Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), 25);
            zubs[2]->physicsProcess(Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), 25);
        }
        else zub->physicsProcess(Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), 25);
    }
    repaint();
}

void GameSpace::paintEvent(QPaintEvent* ev){
    if(gameGoing){
        QPainter painter(this);
        if(nightmareMode){
            zubs[0]->graphicsProcess(&painter, Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), showVectors);
            zubs[1]->graphicsProcess(&painter, Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), showVectors);
            zubs[2]->graphicsProcess(&painter, Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), showVectors);
        }
        else zub->graphicsProcess(&painter, Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()), showVectors);
    }
    QWidget::paintEvent(ev);
}

void GameSpace::mousePressEvent(QMouseEvent* ev){
    clicks++;
    if(!cursorAnimationUpdateTimer->isActive())cursorAnimationUpdateTimer->start();
    if(nightmareMode){
        unsigned int i = 0;
        bool played_sound = false;
        while(i < 3){
            if((Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()) - zubs[i]->position()).length() < 25){
                hits++;
                if(playSounds){
                    QSoundEffect* effect = new QSoundEffect(this);
                    effect->setVolume(0.5f);
                    effect->setSource(QUrl("qrc:///snd/sounds/hit.wav"));
                    effect->play();
                    played_sound = true;
                }
                if(!zubs[i]->isCurrentlyDying() && !zubs[i]->isCurrentlyDead()){
                    unsigned int eval = zubs[i]->hp();
                    eval--;
                    if(eval <= 0){
                        if(playSounds){
                            QSoundEffect* effect = new QSoundEffect(this);
                            effect->setVolume(0.5f);
                            effect->setSource(QUrl("qrc:///snd/sounds/scream.wav"));
                            effect->play();
                            QSoundEffect* effect1 = new QSoundEffect(this);
                            effect1->setVolume(0.5f);
                            effect1->setSource(QUrl("qrc:///snd/sounds/ambulance.wav"));
                            effect1->play();
                        }
                        zubs[i]->takeHit(true);
                    }
                    else zubs[i]->takeHit();
                    zubs[i]->setHp(eval);
                }
            }
            else{
                if(playSounds && !played_sound){
                    QSoundEffect* effect = new QSoundEffect(this);
                    effect->setVolume(0.2f);
                    effect->setSource(QUrl("qrc:///snd/sounds/miss.wav"));
                    effect->play();
                    played_sound = true;
                }
            }
            i++;
        }
    }
    else{
        if((Vector2(mapFromGlobal(this->cursor().pos()).rx(), mapFromGlobal(this->cursor().pos()).ry()) - zub->position()).length() < 25){
            if(playSounds){
                QSoundEffect* effect = new QSoundEffect(this);
                effect->setVolume(0.5f);
                effect->setSource(QUrl("qrc:///snd/sounds/hit.wav"));
                effect->play();
            }
            if(!zub->isCurrentlyDying()){
                unsigned int eval = zub->hp();
                eval--;
                if(eval <= 0){
                    if(playSounds){
                        QSoundEffect* effect = new QSoundEffect(this);
                        effect->setVolume(0.5f);
                        effect->setSource(QUrl("qrc:///snd/sounds/scream.wav"));
                        effect->play();
                        QSoundEffect* effect1 = new QSoundEffect(this);
                        effect1->setVolume(0.5f);
                        effect1->setSource(QUrl("qrc:///snd/sounds/ambulance.wav"));
                        effect1->play();
                    }
                    zub->takeHit(true);
                }
                else zub->takeHit();
                zub->setHp(eval);
            }
        }
        else{
            if(playSounds){
                QSoundEffect* effect = new QSoundEffect(this);
                effect->setVolume(0.2f);
                effect->setSource(QUrl("qrc:///snd/sounds/miss.wav"));
                effect->play();
            }
        }
    }
    QWidget::mousePressEvent(ev);
}

void GameSpace::onZubZubDied(zubzub* _zub){
    if(nightmareMode){
        bool indicator = true;
        unsigned int i = 0;
        while(indicator){
            if(zubs[i] == _zub){
                disconnect(zubs[i], &zubzub::died, this, &GameSpace::onZubZubDied);
                indicator = false;
            }
            i++;
        }
        if(zubs[0]->isCurrentlyDead() && zubs[1]->isCurrentlyDead() && zubs[2]->isCurrentlyDead()){
            gameFinishedAt = QTime::currentTime();
            gameGoing = false;
            emit finishedGame(gameStartedAt, gameFinishedAt, clicks, hits, cursorDistanceTravelled);
            hide();
            disconnect(mainTheme, &QSoundEffect::playingChanged, this, &GameSpace::changeTheme);
            mainTheme->stop();
        }
    }
    else{
        gameFinishedAt = QTime::currentTime();
        gameGoing = false;
        disconnect(zub, &zubzub::died, this, &GameSpace::onZubZubDied);
        emit finishedGame(gameStartedAt, gameFinishedAt, clicks, hits, cursorDistanceTravelled);
        hide();
        disconnect(mainTheme, &QSoundEffect::playingChanged, this, &GameSpace::changeTheme);
        mainTheme->stop();
    }
}

void GameSpace::resizeEvent(QResizeEvent* ev){
    if(gameGoing){
        if(nightmareMode){
            zubs[0]->changeLimits(ev);
            zubs[1]->changeLimits(ev);
            zubs[2]->changeLimits(ev);
        }
        else zub->changeLimits(ev);
    }
}

void GameSpace::cursorUpdateEvent(){
    if(currentCursorFrame == cursorFrames.length()){
        cursorAnimationUpdateTimer->stop();
        currentCursorFrame = 0;
        setCursor(QCursor(*cursorFrames[currentCursorFrame], 46, 7));
    }
    else{
        setCursor(QCursor(*cursorFrames[currentCursorFrame], 46, 7));
        currentCursorFrame++;
    }
}

void GameSpace::cursorMovementEvent(){

}

GameSpace::~GameSpace()
{
    unsigned int iter = 0;
    while(iter != zubs.length()){
        delete zubs[iter];
        iter++;
    }
    iter = 0;
    while(iter != cursorFrames.length()){
        delete cursorFrames[iter];
        iter++;
    }
    updateTimer->stop();
    cursorAnimationUpdateTimer->stop();
    delete updateTimer;
    delete cursorAnimationUpdateTimer;
    mainTheme->stop();
    delete mainTheme;
    delete ui;
}

void GameSpace::changeTheme(){
    if(playSounds && mainTheme->isPlaying() == false){
        mainTheme->setSource(QUrl(QString("qrc:///snd/sounds/mainTheme%1.wav").arg(rand()%3)));
        mainTheme->setVolume(0.2f);
        mainTheme->play();
    }
}
