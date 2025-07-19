#ifndef GAMESPACE_H
#define GAMESPACE_H

#include <QWidget>
#include "zubzub.h"
#include <QTimer>
#include <QSoundEffect>

namespace Ui {
class GameSpace;
}

class GameSpace : public QWidget
{
    Q_OBJECT

public:
    explicit GameSpace(QWidget *parent, int difficulty, bool _showVectors, bool _useFullscreen, bool _playSounds);
    ~GameSpace();

private:
    Ui::GameSpace *ui;

    bool nightmareMode;
    bool showVectors;
    bool useFullscreen;
    bool playSounds;

    zubzub* zub;
    QVector<zubzub*> zubs;

    QTimer* updateTimer;
    QTimer* cursorAnimationUpdateTimer;
    QTimer* cursorMovementStatsUpdateTimer;

    Vector2 lastCursorPos;

    bool gameGoing;

    QList<QPixmap*> cursorFrames;
    unsigned int currentCursorFrame;

    QSoundEffect* mainTheme;

    // Game statistics
    QTime gameStartedAt;
    QTime gameFinishedAt;

    unsigned int clicks;
    unsigned short hits;

    unsigned long long cursorDistanceTravelled;


private slots:
    void paintEvent(QPaintEvent* ev);
    void resizeEvent(QResizeEvent* ev);
    void updateEvent();
    void mousePressEvent(QMouseEvent* ev);
    void onZubZubDied(zubzub*);

    void cursorUpdateEvent();

    void cursorMovementEvent();

    void changeTheme();

signals:
    void finishedGame(QTime _gameStartedAt, QTime _gameFinishedAt, unsigned int _clicks, unsigned short _hits, unsigned long long _cusorDistanceTravelled);
};

#endif // GAMESPACE_H
