/***********************************************************************
*    customdialog.h:                                                   *
*    Custom QDialog for sending additional signals                     *
*    Copyright (c) 2016 Tyler Lewis                                    *
************************************************************************
*    This is a header file for QMineSweeper:                           *
*    https://github.com/Pinguinsan/QMineSweeper                        *
*    The source code is released under the GNU LGPL                    *
*    This file holds the implementation of a CustomDialog class        *
*    The CustomDialog is used to override QDialog signals and slots    *
*    to inform the MainWindow when it is closing, to unpause the game  *
*                                                                      *
*    You should have received a copy of the GNU Lesser General         *
*    Public license along with QMineSweeper                            *
*    If not, see <http://www.gnu.org/licenses/>                        *
***********************************************************************/


#ifndef QMINESWEEPER_CUSTOMDIALOG_H
#define QMINESWEEPER_CUSTOMDIALOG_H

#include <QDialog>

class CustomDialog : public QDialog
{
    Q_OBJECT
public:

signals:
    void aboutToClose();

public:
    void closeEvent(QCloseEvent *ce)
    {
        emit (aboutToClose());
        QDialog::closeEvent(ce);
    }
};

#endif //QMINESWEEPER_CUSTOMDIALOG_H
