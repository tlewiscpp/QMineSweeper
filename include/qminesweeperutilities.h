/***********************************************************************
*    qminesweeperutilities.h:                                          *
*    Namespace for general utilities used by classes in QMineSweeper   *
*    Copyright (c) 2016 Tyler Lewis                                    *
************************************************************************
*    This is a header file for QMineSweeper:                           *
*    https://github.com/Pinguinsan/QMineSweeper                        *
*    This file holds the declarations for a QMineSweeperUtilities      *
*    namespace. QMineSweeperUtilities holds utilities shared by all    *
*    classes, such a delay functions, random functions, etc            *
*    The source code is released under the LGPL                        *
*                                                                      *
*    You should have received a copy of the GNU Lesser General         *
*    Public license along with QMineSweeper                            *
*    If not, see <http://www.gnu.org/licenses/>                        *
***********************************************************************/

#ifndef QMINESWEEPER_QMINESWEEPERUTILITIES_H
#define QMINESWEEPER_QMINESWEEPERUTILITIES_H

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <tuple>
#include <utility>
#include <regex>
#include <cstdio>
#include <random>

#include <QString>

namespace QMineSweeperUtilities
{

    class Random
    {
    public:
        Random() = default;
        Random(std::mt19937::result_type seed);
        int drawNumber(int min, int max);

    private:
        std::mt19937 m_randomEngine{std::random_device{}()};
    };

    int randomBetween(int lowLimit, int highLimit, bool lowInclusive = false, bool highInclusive = false);

    void logString(const std::string &str);

    template<typename T>
    std::string toString(const T &convert)
    {
        std::string returnString{""};
        std::stringstream transfer;
        transfer << convert;
        transfer >> returnString;
        return returnString;
    }


    std::string toString(const std::string &str);
    std::string toString(const char *str);
    std::string getPadding(size_t howMuch, char padChar);
    std::string getPadding(size_t howMuch, const char *padString);
    std::string getPadding(size_t howMuch, const std::string &padString);

    template <typename T>
    QString toQString(const T &convert) { return QString::fromStdString(toString(convert)); }
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


    template <typename T>
    std::string toStdString(const T &rhs)
    {
        std::stringstream stringStream{};
        stringStream << rhs;
        return stringStream.str();
    }

    /*snprintf style*/
    template<typename ... Args>
    std::string PStringFormat(const char *format, Args& ... args)
    {
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
    std::string TStringFormat(const char *formatting);

    /*C# style String.Format()*/
    template <typename First, typename ... Args>
    std::string TStringFormat(const char *formatting, First& first, Args& ... args)
    {
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

        /*Iterate through string, finding position and lengths of all matches {x}*/
        while(std::regex_search(copyString, match, targetRegex)) {
            /*Get the absolute position of the match in the original return string*/
            size_t foundPosition{match.position() + (returnString.length() - copyString.length())};
            int regexMatchNumericValue{0};
            try {
                /*Convert the integer value between the opening and closing braces to an int to compare */
                regexMatchNumericValue = stringToInt(returnString.substr(foundPosition + 1, (foundPosition + match.str().length())));
                
                /*Do not allow negative numbers, although this should never get picked up the regex anyway*/
                if (regexMatchNumericValue < 0) {
                    throw std::runtime_error(TStringFormat("ERROR: In TStringFormat() - Formatted string is invalid (formatting = {0})", formatting));
                }
                /* If the numeric value in the curly brace token is smaller than
                * the current smallest (or if the smallest value has not yet been set,
                * ie it is the first match), set the corresponding smallestX variables
                * and wrap them up into a TokenInformation and add it to the std::vector */
                int smallestValue{std::get<0>(smallestValueInformation.at(0))};
                if ((smallestValue == -1) || (regexMatchNumericValue < smallestValue)) {
                    smallestValueInformation.clear();
                    smallestValueInformation.push_back(std::make_tuple(regexMatchNumericValue,
                                                                    foundPosition,
                                                                    match.str().length()));
                } else if (regexMatchNumericValue == smallestValue) {
                    smallestValueInformation.push_back(std::make_tuple(regexMatchNumericValue,
                                                                    foundPosition,
                                                                    match.str().length()));
                }
            } catch (std::exception e) {
                //TODO: Throw instead of just output exception 
                std::cout << e.what() << std::endl;
            }
            copyString = match.suffix();
        }
        int smallestValue{std::get<0>(smallestValueInformation.at(0))};
        if (smallestValue == -1) {
            throw std::runtime_error(TStringFormat("ERROR: In TStringFormat() - Formatted string is invalid (formatting = {0})", formatting));
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
            returnString = returnString.substr(0, smallestValueAdjustedPosition)
                        + firstString
                        + returnString.substr(smallestValueAdjustedPosition + smallestValueLength);
            index++;
        }
        return TStringFormat(returnString.c_str(), args...);
    }

    template<typename ... Args>
    QString QStringFormat(const char *format, Args ... args)
    {
        ssize_t size = snprintf(nullptr, 0, format, args ...) + 1;
        std::unique_ptr<char[]> stringBuffer{new char[size]};
        snprintf(stringBuffer.get(), size, format, args ...);
        return QString::fromStdString(std::string{stringBuffer.get(), stringBuffer.get() + size - 1});
    }
}

#endif //QMINESWEEPER_QMINESWEEPERUTILITIES_H
