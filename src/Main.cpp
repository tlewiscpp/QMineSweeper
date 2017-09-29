/***********************************************************************
*    main.cpp:                                                         *
*    Only exists to create the main window and set it along            *
*    Copyright (c) 2017 Tyler Lewis                                    *
************************************************************************
*    This is a source file for QMineSweeper:                           *
*    https://github.com/tlewiscpp/QMineSweeper                         *
*    The source code is released under the GNU LGPL                    *
*                                                                      *
*    You should have received a copy of the GNU Lesser General         *
*    Public license along with QMineSweeper                            *
*    If not, see <http://www.gnu.org/licenses/>                        *
***********************************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <algorithm>

#if defined(_WIN32)
    #include <Windows.h>
#else
    #include <signal.h>
    #include <unistd.h>

#endif

#define SIGNAL_STRING_BUFFER_SIZE 255

#include <QApplication>
#include <QString>
#include <QRect>
#include <QDateTime>

#include "MainWindow.h"
#include "QmsButton.h"
#include "QmsIcons.h"
#include "QmsStrings.h"
#include "QmsSoundEffects.h"
#include "QmsSettingsLoader.h"
#include "QmsUtilities.h"
#include "GameController.h"
#include "GlobalDefinitions.h"
#include "QmsApplicationSettings.h"

/*
 * The program is organized like this:
 *     MainWindow - Contains all UI elements, and handling of UI signals
 *     GameController - Single instance class that controls all of the game logic
 *     MineCoordinates - Representing the X,Y coordinates of a button
 *     MineCoordinatesHash - A functor hash function to be able to store the QMineSweeperButton in a std::unordered_map
 *     QMineSweeperButton - The actual button as it appears on the screen, inheriting from QPushButton
 *     QMineSweeperIcons - Single instance class that holds all of the common icons used
 *     QMineSweeperSounds - Single instance class that holds all of the common sounds used
 *     QMineSweeperStrings - Single instance class that holds all of the common strings used
 *     QMineSweeperUtilities - Collection of general utilities (random numbers, etc) used
 *     QMineSweeperSettingsLoader - Single instance class that holds all settings from a configuration file
 */

const std::list<const char *> HELP_SWITCHES{"-h", "--h", "-help", "--help"};
const std::list<const char *> VERSION_SWITCHES{"v", "-v", "--v", "-version", "--version"};
const std::list<const char *> DIMENSIONS_SWITCHES{"-d", "--d", "-dimensions", "--dimensions"};
const std::list<const char *> VERBOSE_LOGGING_SWITCHES{"-o", "--o", "-verbose", "--verbose"};

void displayHelp();
void displayVersion();
void interruptHandler(int signal);
void installSignalHandlers(void (*signalHandler)(int));
void globalLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

static bool verboseLogging{false};

using namespace QmsStrings;
using namespace QmsUtilities;
using namespace QmsGlobalSettings;

int main(int argc, char *argv[])
{
    installSignalHandlers(interruptHandler);
    qInstallMessageHandler(globalLogHandler);
    QCoreApplication::setOrganizationName(AUTHOR_NAME);
    QCoreApplication::setOrganizationDomain(REMOTE_URL);
    QCoreApplication::setApplicationName(LONG_PROGRAM_NAME);

    QmsUtilities::checkOrCreateProgramLogDirectory();
    QmsUtilities::checkOrCreateProgramSettingsDirectory();

     std::cout << std::endl;
     for (auto iter = argv + 1; iter != (argv + argc); iter++) {
         if (isSwitch(*iter, HELP_SWITCHES)) {
             displayHelp();
             return 0;
         } else if (isSwitch(*iter, VERSION_SWITCHES)) {
             displayVersion();
             return 0;
         }
     }
     displayVersion();
     int columnCount{-1};
     int rowCount{-1};
     bool columnsSetByCommandLine{false};

     for (auto iter = argv + 1; iter != (argv + argc); iter++) {
         if (isSwitch(*iter, VERBOSE_LOGGING_SWITCHES)) {
             verboseLogging = true;
         } else if (isSwitch(*iter, DIMENSIONS_SWITCHES)) {
            if (iter + 1) {
                if (rowCount != -1) {
                    std::cout << "Switch \"" << *iter << "\" accepted, but dimensions were already specified by command line option, skipping option" << std::endl;
                    continue;
                }
                std::pair<int, int> dimensions{tryParseDimensions(*(iter + 1))};

                if (dimensions.second == -1) {
                    std::cout << "Switch \"" << *iter << "\" accepted, but value \"" << *(iter + 1) << " is not a valid dimension specification, skipping option" << std::endl;
                    continue;
                } else {
                    columnCount = dimensions.first;
                    rowCount = dimensions.second;
                }
                iter++;
            } else {
                std::cout << "Switch \"" << *iter << "\" accepted, but no dimensions were specified after, skipping option" << std::endl;
            }
         } else if (isEqualsSwitch(*iter, DIMENSIONS_SWITCHES)) {
             if (rowCount != -1) {
                 std::cout << "Switch \"" << *iter << "\" accepted, but dimensions were already specified by command line option, skipping option" << std::endl;
                 continue;
             }
             std::string copyString{*iter};
             size_t foundPosition{copyString.find('=')};
             size_t foundEnd{copyString.substr(foundPosition).find(' ')};
             if (copyString.substr(foundPosition+1, (foundEnd - foundPosition)) == "") {
                 std::cout << "WARNING: Switch \"" << *iter << "\" accepted, but no dimensions we specified after, skipping option" << std::endl;
             } else {
                 std::pair<int, int> dimensions{tryParseDimensions(stripAllFromString(copyString.substr(foundPosition+1, (foundEnd - foundPosition)), "\""))};
                 if (dimensions.second == -1) {
                     std::cout << "Switch \"" << *iter << "\" accepted, but value \"" << stripAllFromString(copyString.substr(foundPosition+1, (foundEnd - foundPosition)), "\"") << "\" is not a valid dimension specification, skipping option" << std::endl;
                 } else {
                     columnCount = dimensions.first;
                     rowCount = dimensions.second;
                 }
             }
         } else if (containsSeparator(*iter)) {
             if (rowCount != -1) {
                 std::cout << "Switch \"" << *iter << "\" accepted, but dimensions were already specified by command line option, skipping option" << std::endl;
                 continue;
             }
             std::pair<int, int> dimensions{tryParseDimensions(*iter)};
             columnCount = dimensions.first;
             rowCount = dimensions.second;
         } else {
            std::cout << "Invalid switch : " << *iter << ", skipping" << std::endl;
         }
     }
     if (rowCount == -1) {
         columnCount = QmsSettingsLoader::DEFAULT_COLUMN_COUNT();
         rowCount = QmsSettingsLoader::DEFAULT_ROW_COUNT();
     } else {
        columnsSetByCommandLine = true;
    }
#if defined(__ANDROID__)
    (void)columnsSetByCommandLine;
#else
    QmsApplicationSettings settings{QmsSettingsLoader::loadApplicationSettings()};
    if (!columnsSetByCommandLine) {
        if ((columnCount <= 0) || (rowCount <= 0)) {
            columnCount = QmsSettingsLoader::DEFAULT_COLUMN_COUNT();
            rowCount = QmsSettingsLoader::DEFAULT_ROW_COUNT();
        } else {
            columnCount = settings.numberOfColumns();
            rowCount = settings.numberOfRows();
        }
    }
#endif
    LOG_INFO() << QString{"Using log file %1"}.arg(QmsUtilities::getLogFilePath());
    //LOG_DEBUG() << QString{"Beginning game with dimensions (%1x%2)"}.arg(QS_NUMBER(columnCount), QS_NUMBER(rowCount));
    //TODO: Load language from config file

    QApplication qApplication{argc, argv};
    QmsIcons::initializeInstance();
    QmsSettingsLoader::initializeInstance(nullptr);
    GameController::initializeInstance(columnCount, rowCount);
#if defined(__ANDROID__)
    QmsSoundEffects::initializeInstance();
#else
    QmsSoundEffects::initializeInstance(settings.audioVolume());
#endif
    std::shared_ptr<MainWindow> mainWindow{std::make_shared<MainWindow>(QmsSettingsLoader::DEFAULT_LANGUAGE())};
    gameController->bindMainWindow(mainWindow);
    mainWindow->setupNewGame();

    QMainWindow::connect(&qApplication, SIGNAL(aboutToQuit()), mainWindow.get(), SLOT(onApplicationExit()));
    mainWindow->setWindowIcon(applicationIcons->MINE_ICON_72);
    mainWindow->setWindowTitle(MainWindow::tr(MAIN_WINDOW_TITLE));
    mainWindow->show();
    mainWindow->centerAndFitWindow(true);
    mainWindow->resizeResetIcon();
    return qApplication.exec();
}

void interruptHandler(int signalNumber)
{
#if defined(_WIN32)
    Q_UNUSED(signalNumber);
#else
    if ((signalNumber == SIGUSR1) || (signalNumber == SIGUSR2) || (signalNumber == SIGCHLD)) {
        return;
    }
    std::cout << std::endl << "Caught signal " << signalNumber << " (" << strsignal(signalNumber) << "), exiting " << PROGRAM_NAME << std::endl;
    exit (signalNumber);
#endif
}

void installSignalHandlers(void (*signalHandler)(int))
{
#if defined(_WIN32)
    Q_UNUSED(signalHandler);
#else
    static struct sigaction signalInterruptHandler;
    signalInterruptHandler.sa_handler = signalHandler;
    sigemptyset(&signalInterruptHandler.sa_mask);
    signalInterruptHandler.sa_flags = 0;
    sigaction(SIGHUP, &signalInterruptHandler, NULL);
    sigaction(SIGINT, &signalInterruptHandler, NULL);
    sigaction(SIGQUIT, &signalInterruptHandler, NULL);
    sigaction(SIGILL, &signalInterruptHandler, NULL);
    sigaction(SIGABRT, &signalInterruptHandler, NULL);
    sigaction(SIGFPE, &signalInterruptHandler, NULL);
    sigaction(SIGPIPE, &signalInterruptHandler, NULL);
    sigaction(SIGALRM, &signalInterruptHandler, NULL);
    sigaction(SIGTERM, &signalInterruptHandler, NULL);
    sigaction(SIGUSR1, &signalInterruptHandler, NULL);
    sigaction(SIGUSR2, &signalInterruptHandler, NULL);
    sigaction(SIGCHLD, &signalInterruptHandler, NULL);
    sigaction(SIGCONT, &signalInterruptHandler, NULL);
    sigaction(SIGTSTP, &signalInterruptHandler, NULL);
    sigaction(SIGTTIN, &signalInterruptHandler, NULL);
    sigaction(SIGTTOU, &signalInterruptHandler, NULL);
#endif
}

void displayVersion()
{
    using namespace QmsUtilities;
    LOG_INFO() << QString{"%1, v%2.%3.%4"}.arg(PROGRAM_NAME, QS_NUMBER(SOFTWARE_MAJOR_VERSION), QS_NUMBER(SOFTWARE_MINOR_VERSION), QS_NUMBER(SOFTWARE_PATCH_VERSION));
    LOG_INFO() << QString{"Written by %1"}.arg(AUTHOR_NAME);
    LOG_INFO() << QString{"Built with %1 v%2.%3.%4, %5"}.arg(COMPILER_NAME, QS_NUMBER(COMPILER_MAJOR_VERSION), QS_NUMBER(COMPILER_MINOR_VERSION), QS_NUMBER(COMPILER_PATCH_VERSION), __DATE__);
    LOG_INFO() << QString{"Build architecture: %1"}.arg(getBuildArchitecture());
    LOG_INFO() << QString{"Current detected architecture: %1"}.arg(getCurrentArchitecture());
    LOG_INFO() << QString{"Detected OS version: %1"}.arg(getOSVersion());
}


void displayHelp()
{
    LOG_INFO() << QString{"Usage: %1 [options]=(ColumnsxRows)"}.arg(PROGRAM_NAME);
    LOG_INFO() << "Options: ";
    LOG_INFO() << "    -h, --h, -help, --help: Display this help text";
    LOG_INFO() << "    -v, --v, -version, --version: Display the version";
    LOG_INFO() << "    -d, --d, -dimensions, --dimensions: Set the number of columns";
    LOG_INFO() << "    -o, --o, -verbose, --verbose: Log debug output";
    LOG_INFO() << "Example: ";
    LOG_INFO() << "    To start a 14x9 game, any of the following command line options would work:";
    LOG_INFO() << "    QMineSweeper --dimensions 14x9";
    LOG_INFO() << "    QMineSweeper --dimensions=14x9";
    LOG_INFO() << "    QMineSweeper 14x9";
}

template <typename StringType, typename FileStringType>
void logToFile(const StringType &str, const FileStringType &filePath)
{
    QFile qFile{filePath};
    QString stringCopy{toQString(str)};
    if (qFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        if (qFile.write(toStdString(str).c_str(), toStdString(str).length()) == -1) {
            throw std::runtime_error(QString{"Failed to log data \"%1\" to file \"%2\" (file was opened, but not writable, permission problem?)"}.arg(toQString(str), toQString(filePath)).toStdString());
        }
    } else {
        throw std::runtime_error(QString{"Failed to log data \"%1\" to file \"%2\" (could not open file)"}.arg(toQString(str), toQString(filePath)).toStdString());
    }

}

void globalLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg{msg.toLocal8Bit()};
    QString logContext{""};
    auto *outputStream = &std::cout;
    switch (type) {
    case QtDebugMsg:
        if (!verboseLogging) {
            return;
        }
        logContext = "{  Debug }: ";
        outputStream = &std::cout;
        break;
    case QtInfoMsg:
        logContext = "{  Info  }: ";
        outputStream = &std::clog;
        break;
    case QtWarningMsg:
        logContext = "{  Warn  }: ";
        outputStream = &std::cout;
        break;
    case QtCriticalMsg:
        logContext = "{Critical}: ";
        outputStream = &std::cerr;
        break;
    case QtFatalMsg:
        logContext = "{  Fatal }: ";
        outputStream = &std::cerr;
        abort();
    }
    QString logMessage{""};
    std::string coreLogMessage{QString{localMsg}.toStdString()};
    if (coreLogMessage.find('\"') == 0) {
        coreLogMessage = coreLogMessage.substr(1);
    }
    if (coreLogMessage.find_last_of('\"') == coreLogMessage.length() - 1) {
        coreLogMessage = coreLogMessage.substr(0, coreLogMessage.length() - 1);
    }
    //coreLogMessage.erase(std::remove_if(coreLogMessage.begin(), coreLogMessage.end(),[](char c) { return c == '\"'; }), coreLogMessage.end());
    if ((type == QtCriticalMsg) || (type == QtFatalMsg)) {
        logMessage = QString{"[%1] - %2 %3 (%4:%5, %6)"}.arg(QDateTime::currentDateTime().time().toString(), logContext, coreLogMessage.c_str(), context.file, QS_NUMBER(context.line), context.function);
    } else {
        logMessage = QString{"[%1] - %2 %3"}.arg(QDateTime::currentDateTime().time().toString(), logContext, coreLogMessage.c_str());
    }
    bool addLineEnding{true};
    static const QList<const char *> LINE_ENDINGS{"\r\n", "\r", "\n", "\n\r"};
    for (const auto &it : LINE_ENDINGS) {
        if (logMessage.endsWith(it)) {
            addLineEnding = false;
        }
    }
    if (addLineEnding) {
        logMessage.append("\n");
    }
    if (outputStream) {
        *outputStream << logMessage.toStdString();
    }
    logToFile(logMessage.toStdString(), QmsUtilities::getLogFilePath());
    outputStream->flush();
}
