#include "format.h"

#include <string>

using std::string;
using std::to_string;

string addLeadingZero(int value)
{
    if (value >= 0 && value < 10)
    {
        return "0" + to_string(value);
    }
    return to_string(value);
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long times)
{
    int seconds = times % 60;
    long hours = times / (60 * 60);
    int minutes = (times - hours * 60 * 60) / 60;

    return addLeadingZero(hours) + ":" + addLeadingZero(minutes) + ":" + addLeadingZero(seconds);
}