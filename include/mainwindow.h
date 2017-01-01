/***********************************************************************
*    mainwindow.h:                                                     *
*    Main window for displaying the game                               *
*    Copyright (c) 2016 Tyler Lewis                                    *
************************************************************************
*    This is a header file for QMineSweeper:                           *
*    https://github.com/Pinguinsan/QMineSweeper                        *
*    The source code is released under the GNU LGPL                    *
*    This file holds the declarations of a custom QMainWindow class    *
*    MainWindow is the default container for a Qt Desktop Application  *
*                                                                      *
*    You should have received a copy of the GNU Lesser General         *
*    Public license along with QMineSweeper                            *
*    If not, see <http://www.gnu.org/licenses/>                        *
***********************************************************************/

#ifndef QMINESWEEPER_MAINWINDOW_H
#define QMINESWEEPER_MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QWindow>
#include <QDesktopWidget>
#include <QString>
#include <QGridLayout>
#include <QLCDNumber>
#include <QRect>
#include <QTimer>
#include <QDialog>
#include <QSound>
#include <QMediaPlayer>

#include <iostream>
#include <memory>

#include "eventtimer.h"
#include "qminesweeperbutton.h"
#include "qminesweepericons.h"
#include "gamecontroller.h"
#include "boardresizewindow.h"
#include "qminesweepersoundeffects.h"
#include "qminesweeperutilities.h"
#include "qminesweeperstrings.h"

#if (__cplusplus < 201402L) && !defined(_MSC_VER)
    namespace std {
    template<typename T, typename... Args>
        unique_ptr<T> make_unique(Args&&... args)
        {
            return unique_ptr<T>(new T(forward<Args>(args)...));
        }
    }
#endif

namespace Ui
{
    class MainWindow;
    class BoardResizeWindow;
}

class QMineSweeperButton;
class GameController;
class BoardResizeWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::shared_ptr<QMineSweeperIcons> qmsiPtr,
                        std::shared_ptr<QMineSweeperSoundEffects> qmssePtr,
                        std::shared_ptr<GameController> gcPtr,
                        std::shared_ptr<QDesktopWidget> qdwPtr,
                        QWidget *parent = 0);
    ~MainWindow();

    void populateMineField();
    void displayAllMines();
    void resizeResetIcon();
    void displayMine(std::shared_ptr<QMineSweeperButton> msb);
    void bindGameController(std::shared_ptr<GameController> gc);
    void bindQMineSweeperIcons(std::shared_ptr<QMineSweeperIcons> qmsiPtr);
    void bindQDesktopWidget(std::shared_ptr<QDesktopWidget> qdw);
    void bindQMineSweeperSoundEffects(std::shared_ptr<QMineSweeperSoundEffects> qmssePtr);
    void setResetButtonIcon(const QIcon &icon);
    void drawNumberOfSurroundingMines(std::shared_ptr<QMineSweeperButton> msb);
    int xPlacement() const;
    int yPlacement() const;
    void centerAndFitWindow();
    bool boardResizeDialogVisible();
    std::shared_ptr<QMineSweeperIcons> qmsiPtr() const;
    std::shared_ptr<QMineSweeperSoundEffects> qmssePtr() const;
    QString saveStyleSheet() const;

private:
    std::unique_ptr<QTimer> m_eventTimer;
    std::unique_ptr<EventTimer> m_playTimer;
    std::unique_ptr<EventTimer> m_userIdleTimer;
    std::unique_ptr<Ui::BoardResizeWindow> m_bsui;
    std::unique_ptr<Ui::MainWindow> m_ui;
    std::unique_ptr<BoardResizeWindow>  m_boardSizeWindow;
    std::unique_ptr<QMediaPlayer> m_explosionEffect;
    std::shared_ptr<QMineSweeperIcons> m_qmsiPtr;
    std::shared_ptr<QMineSweeperSoundEffects> m_qmssePtr;
    std::shared_ptr<GameController> m_gameController;
    std::shared_ptr<QDesktopWidget> m_qDesktopWidget;

    static const int s_TASKBAR_HEIGHT;
    static const int s_GAME_TIMER_INTERVAL;
    static const double s_DEFAULT_NUMBER_OF_MINES;
    static const int s_HEIGHT_SCALE_FACTOR;
    static const int s_WIDTH_SCALE_FACTOR;
    static const int s_NUMBER_OF_HORIZONTAL_MARGINS;
    static const int s_NUMBER_OF_VERTIAL_MARGINS;
    static const int s_DEFAULT_MINE_SIZE_SCALE_FACTOR;
    static const double s_MINE_ICON_REDUCTION_SCALE_FACTOR;

    double m_reductionSizeScaleFactor;
    QString m_saveStyleSheet;
    int m_xPlacement;
    int m_yPlacement;
    QSize m_currentDefaultMineSize;
    QSize m_currentMaxMineSize;
    QSize m_currentIconReductionSize;
    bool m_maxMineSizeCacheIsValid;
    bool m_iconReductionSizeCacheIsValid;
    bool m_tempPauseFlag;
    bool m_boardSizeGeometrySet;

    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

    void invalidateSizeCaches();
    void doGameReset();
    QSize getMaxMineSize();
    QSize getIconReductionSize();
    void calculateXYPlacement();
    std::string getLCDDisplayPadding(int howMuch);

signals:
    void resetButtonClicked();
    void resetGame();
    void mineExplosionEvent();
    void boardResize(int columns, int rows);
    void gamePaused();
    void gameResumed();
    void mineDisplayed();
    void winEvent();
    void mineSweeperButtonCreated(std::shared_ptr<QMineSweeperButton> msb);

public slots:
    void resetResetButtonIcon();
    void onGameStarted();
    void onGamePaused();
    void onGameResumed();
    void onMineExplosionEventTriggered();
    void setupNewGame();
    void onGameWon();
    void onCustomDialogClosed();
    void startUserIdleTimer();

private slots:
    void eventLoop();
    void onResetButtonClicked();
    void onChangeBoardSizeActionTriggered();
    void onAboutQtActionTriggered();
    void onApplicationExit();
    void onBsuiOkayButtonClicked();
    void onBsuiCancelButtonClicked();
    void updateVisibleGameTimer();
    void updateLCDDisplays();
    void updateGeometry();
    void updateUserIdleTimer();
    void startGameTimer();

};

#endif // QMINESWEEPER_MAINWINDOW_H
