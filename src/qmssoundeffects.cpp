/***********************************************************************
*    qmssoundeffects.cpp:                                              *
*    Class for holding instances of all sounds used in QMineSweeper    *
*    Copyright (c) 2017 Tyler Lewis                                    *
************************************************************************
*    This is a source file for QMineSweeper:                           *
*    https://github.com/tlewiscpp/QMineSweeper                         *
*    This file holds the implementation of a QmsSoundEffects           *
*    class. QmsSoundEffects holds instances of all sound               *
*    effects used in the game, allowing members to be called to        *
*    play sounds, rather than having to use a raw filename string to   *
*    access the sound effect on disk, thus allowing low-latency effects*
*    The source code is released under the LGPL                        *
*                                                                      *
*    You should have received a copy of the GNU Lesser General         *
*    Public license along with QMineSweeper                            *
*    If not, see <http://www.gnu.org/licenses/>                        *
***********************************************************************/

#include "qmssoundeffects.h"

#include <QSoundEffect>
#include <QString>
#include <QUrl>
#include <QMediaPlayer>

using namespace QmsStrings;

const int QmsSoundEffects::s_EXPLOSION_EFFECT_VOLUME{25};

/* QmsSoundEffects() : Constructor, setting up the QSoundEffects used.
 * This thin wrapper class for the QSoundEffects is necessary because all QObjects
 * must be instantiated and set up after a QApplication is instantiated, so using them
 * as static objects is not usable. Thus, this late binding is necessary */
QmsSoundEffects::QmsSoundEffects() :
    explosionEffect{},
    m_explosionEffectSource{EXPLOSION_EFFECT_SOURCE_STRING}
{
    this->explosionEffect.setMedia(QUrl{this->m_explosionEffectSource});
    this->explosionEffect.setVolume(QmsSoundEffects::s_EXPLOSION_EFFECT_VOLUME);
}

/* ~QmsSoundEffects() : Destructor, empty by defaut */
QmsSoundEffects::~QmsSoundEffects()
{
    //Destructor
}