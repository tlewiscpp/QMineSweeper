#ifndef QMINESWEEPER_QMSUTILITIES_HPP
#define QMINESWEEPER_QMSUTILITIES_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <cstdlib>

#include <ctime>
#include <vector>
#include <tuple>
#include <utility>
#include <regex>
#include <cstdio>
#include <random>
#include <QCryptographicHash>
#include <QString>
#include <sstream>

class QFile;

class QByteArray;

namespace QmsUtilities {

    template<typename T>
    inline std::string toStdString(const T &t) {
        return dynamic_cast<std::stringstream &>(std::stringstream{} << t).str();
    }


    bool clearDirectoryOfFiles(const QString &dir);
    void checkOrCreateProgramLogDirectory();
    void checkOrCreateProgramSettingsDirectory();
    QString getLogFilePath();
    QString getLogFileName();

    QString getOSVersion();
    QString getBuildArchitecture();
    QString getCurrentArchitecture();
    bool containsSeparator(const char *testString);
    std::pair<int, int> tryParseDimensions(const std::string &maybeDimensions);
    float tryParseMineRatio(const std::string &maybeMineRatio);
    bool isValidMineRatio(float mineRatio);
    std::string getSignalName(int signalNumber);

    QString getPID();
    QString getProcessUUID();

    //Does str start with target?
    template<typename CharT>
    inline bool startsWith(const std::basic_string<CharT> &str, const std::basic_string<CharT> &target) {
        return ((str.length() >= target.length()) && std::equal(target.begin(), target.end(), str.begin()));
    }

    //Does str end with target?
    template<typename CharT>
    inline bool endsWith(const std::basic_string<CharT> &str, const std::basic_string<CharT> &target) {
        return ((str.length() >= target.length()) && std::equal(target.rbegin(), target.rend(), str.rbegin()));
    }

    //Does str start with target?
    template<typename CharT>
    inline bool startsWith(const std::basic_string<CharT> &str, CharT target) {
        return (!str.empty() && (*(str.end() - 1) == target));
    }

    //Does str end with target?
    template<typename CharT>
    inline bool endsWith(const std::basic_string<CharT> &str, CharT target) {
        return (!str.empty() && (*str.begin() == target));
    }

    class Random {
    public:
        Random() = default;
        Random(std::mt19937::result_type seed);
        int drawNumber(int min, int max);

    private:
        std::mt19937 m_randomEngine{std::random_device{}()};
    };

    int randomBetween(int lowLimit, int highLimit, bool lowInclusive = true, bool highInclusive = true);

    void logString(const std::string &str);

    QString getProgramSettingsDirectory();

    std::string getPadding(size_t howMuch, char padChar);
    std::string getPadding(size_t howMuch, const char *padString);
    std::string getPadding(size_t howMuch, const std::string &padString);

    template<typename T>
    QString toQString(const T &convert) { return QString::fromStdString(toStdString(convert)); }

    QString toQString(const std::string &convert);
    QString toQString(const char *convert);
    QString toQString(const QString &convert);

    int roundIntuitively(double numberToRound);
    bool endsWith(const std::string &stringToCheck, const std::string &matchString);
    bool endsWith(const std::string &stringToCheck, char matchChar);

    template<typename Container>
    bool isSwitch(const std::string &switchToCheck, const Container &switches) {
        std::string copyString{switchToCheck};
        std::transform(copyString.begin(), copyString.end(), copyString.begin(), ::tolower);
        for (auto &it : switches) {
            if ((copyString == static_cast<std::string>(it)) &&
                (copyString.length() == static_cast<std::string>(it).length()) &&
                (copyString.find(static_cast<std::string>(it)) == 0)) {
                return true;
            }
        }
        return false;
    }

    template<typename Container>
    bool isSwitch(const char *switchToCheck, const Container &switches) {
        return isSwitch(static_cast<std::string>(switchToCheck), switches);
    }

    template<typename Container>
    bool isEqualsSwitch(const std::string &switchToCheck, const Container &switches) {
        std::string copyString{switchToCheck};
        std::transform(copyString.begin(), copyString.end(), copyString.begin(), ::tolower);
        for (auto &it : switches) {
            std::string copySwitch{static_cast<std::string>(it) + "="};
            if ((copyString.find(static_cast<std::string>(it) + "=") == 0)) {
                return true;
            }
        }
        return false;
    }

    template<typename Container>
    bool isEqualsSwitch(const char *switchToCheck, const Container &switches) {
        return isEqualsSwitch(static_cast<std::string>(switchToCheck), switches);
    }

    /*snprintf style*/
    template<typename ... Args>
    std::string PStringFormat(const char *format, Args &... args) {
#if defined(__ANDROID__)
        ssize_t size{snprintf(nullptr, 0, format, args ...) + 1};
#else
        ssize_t size{std::snprintf(nullptr, 0, format, args ...) + 1};
#endif
        std::unique_ptr<char[]> stringBuffer{new char[size]};
        snprintf(stringBuffer.get(), size, format, args ...);
        return std::string{stringBuffer.get(), stringBuffer.get() + size - 1};
    }

    int stringToInt(const std::string &str);
    int stringToInt(const char *str);


    /*Base case to break recursion*/
    inline std::string CSStringFormat(const char *formatting) { return formatting; }

    /*C# style String.Format()*/
    template <typename First, typename ... Args>
    std::string CSStringFormat(const char *formatting, First&& first, Args&& ... args) {
        /* Match exactly one opening brace, one or more numeric digit,
        * then exactly one closing brace, identifying a token */
        static const std::regex targetRegex{"\\{[0-9]+\\}"};
        std::smatch match;

        /* Copy the formatting string to a std::string, to
        * make for easier processing, which will eventually
        * be used (the .c_str() method) to pass the remainder
        * of the formatting recursively */
        std::string returnString{formatting};

        /* Copy the formatting string to another std::string, which
        * will get modified in the regex matching loop, to remove the
        * current match from the string and find the next match */
        std::string copyString{formatting};

        /* std::tuple to hold the current smallest valued brace token,
        * wrapped in a std::vector because there can be multiple brace
        * tokens with the same value. For example, in the following format string:
        * "There were {0} books found matching the title {1}, {0}/{2}",
        * this pass will save the locations of the first and second {0} */
        using TokenInformation = std::tuple<int, size_t, size_t>;
        std::vector<TokenInformation> smallestValueInformation{std::make_tuple(-1, 0, 0)};

        try {
            /*Iterate through string, finding position and lengths of all matches {x}*/
            while(std::regex_search(copyString, match, targetRegex)) {
                /*Get the absolute position of the match in the original return string*/
                size_t foundPosition{match.position() + (returnString.length() - copyString.length())};
                int regexMatchNumericValue{0};
                /*Convert the integer value between the opening and closing braces to an int to compare */
                auto regexMatchNumericString = returnString.substr(foundPosition + 1, match.str().length() - 2);
                regexMatchNumericValue = stringToInt(regexMatchNumericString);

                /*Do not allow negative numbers, although this should never get picked up the regex anyway*/
                if (regexMatchNumericValue < 0) {
                    std::stringstream message{};
                    message << "ERROR: In CSStringFormat() - Formatted string is invalid (formatting = " << formatting << ")";
                    throw std::runtime_error(message.str());
                }
                /* If the numeric value in the curly brace token is smaller than
                * the current smallest (or if the smallest value has not yet been set,
                * ie it is the first match), set the corresponding smallestX variables
                * and wrap them up into a TokenInformation and add it to the std::vector */
                int smallestValue{std::get<0>(smallestValueInformation.at(0))};
                if ((smallestValue == -1) || (regexMatchNumericValue < smallestValue)) {
                    smallestValueInformation.clear();
                    smallestValueInformation.emplace_back(std::make_tuple(regexMatchNumericValue, foundPosition, match.str().length()));
                } else if (regexMatchNumericValue == smallestValue) {
                    smallestValueInformation.emplace_back(std::make_tuple(regexMatchNumericValue, foundPosition, match.str().length()));
                }
                copyString = match.suffix();
            }
            int smallestValue{std::get<0>(smallestValueInformation.at(0))};
            if (smallestValue == -1) {
                return returnString;
            }
            /* Set the returnString to be up to the brace token, then the string
            * representation of current argument in line (first), then the remainder
            * of the format string, effectively removing the token and replacing it
            * with the requested item in the final string, then pass it off recursively */

            std::string firstString{toStdString(first)};
            int index{0};
            for (const auto &it : smallestValueInformation) {
                size_t smallestValueLength{std::get<2>(it)};


                /* Since the original string will be modified, the adjusted position must be
                calculated for any repeated brace tokens, kept track of by index.
                The length of string representation of first mutiplied by which the iterationn count
                is added, and the length of the brace token multiplied by the iteration count is
                subtracted, resulting in the correct starting position of the current brace token */
                size_t lengthOfTokenBracesRemoved{index * smallestValueLength};
                size_t lengthOfStringAdded{index * firstString.length()};
                size_t smallestValueAdjustedPosition{std::get<1>(it) + lengthOfStringAdded - lengthOfTokenBracesRemoved};
                returnString = returnString.substr(0, smallestValueAdjustedPosition) + firstString + returnString.substr(smallestValueAdjustedPosition + smallestValueLength);
                index++;
            }
            return CSStringFormat(returnString.c_str(), args...);
        } catch (const std::exception &e) {
            //If we fail something, just return invalid formatting
            (void)e;
            return returnString;
        }
    }


    template<typename ... Args>
    QString QStringFormat(const char *format, Args ... args) {
        auto size = snprintf(nullptr, 0, format, args ...) + 1;
        std::unique_ptr<char[]> stringBuffer{new char[size]};
        snprintf(stringBuffer.get(), size, format, args ...);
        return QString::fromStdString(std::string{stringBuffer.get(), stringBuffer.get() + size - 1});
    }

    bool toBool(const QString &str, bool *ok = nullptr);
    bool toBool(const std::string &str, bool *ok = nullptr);
    QString boolToQString(bool state);
    std::string boolToString(bool state);

    QByteArray getFileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);
    QByteArray getFileChecksum(QIODevice *inputDevice, QCryptographicHash::Algorithm hashAlgorithm);

    QString getFileDirectoryPath(const QFile &file);
    QString getFileName(const QFile &file);
    bool looksLikeWindowsFilePath(const QString &path);
    std::string stripFromString(const std::string &stringToStrip, const std::string &whatToStrip);
    std::string stripFromString(const std::string &stringToStrip, char whatToStrip);
    std::string stripAllFromString(const std::string &stringToStrip, const std::string &whatToStrip);
    std::string stripAllFromString(const std::string &stringToStrip, char whatToStrip);
}

#endif //QMINESWEEPER_QMSUTILITIES_HPP
