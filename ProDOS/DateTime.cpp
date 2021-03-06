

#include <cstdio>
#include <ctime>
#include <cstring>

#include <ProDOS/DateTime.h>

using namespace ProDOS;

/**
 * Create a new timestamp, set to the epoch.
 */
DateTime::DateTime() :
    _yymmdd(0), _hhmm(0)
{
    init(std::time(NULL));
}

/**
 * Create a new timestamp from the native ProDOS format (date in the high 16-bit word,
 * time in the low 16-bit word).
 *
 * Where the date is a 16 bit value:
 *
 * <pre>
 *  7 6 5 4 3 2 1 0   7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
 * |    Year     |  Month  |   Day   |
 * +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
 * </pre>
 *
 * And the time is a 16 bit value:
 *
 * <pre>
 *  7 6 5 4 3 2 1 0   7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
 * |0 0 0|  Hour   | |0 0|  Minute   |
 * +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
 * </pre>
 *
 * ProDOS technote 28
 *
 * The following definition allows the same range of years that the Apple IIgs
 * Control Panel CDA currently does:
 *
 * <ul>
 * <li> A seven-bit ProDOS year value is in the range 0 to 99
 *      (100 through 127 are invalid)</li>
 * <li> Year values from 40 to 99 represent 1940 through 1999</li>
 * <li> Year values from 0 to 39 represent 2000 through 2039K/li>
 * </ul>
 *
 * @param dtm the combined words
 */
DateTime::DateTime(uint32_t dtm) :
    _yymmdd((dtm >> 16) & 0xff), _hhmm(dtm & 0xff)
{
}

/**
 * Create a new timestamp.
 *
 * @param yymmdd the date portion of the timestamp
 * @param hhmm the time portion of the timestamp
 * @see DateTime(uint32_t)
 */
DateTime::DateTime(unsigned yymmdd, unsigned hhmm) :
    _yymmdd(yymmdd), _hhmm(hhmm)
{
}


/**
 * Create a new timestamp from UNIX standard time
 * @param time the UNIX time
 */
DateTime::DateTime(std::time_t time) :
    _yymmdd(0), _hhmm(0)
{
    init(time);
}


DateTime::DateTime(unsigned year, unsigned month, unsigned day,
        unsigned hour, unsigned minute) :
    _yymmdd(0), _hhmm(0)
{
    init(year, month, day, hour, minute);
}

void DateTime::init(std::time_t time)
{
    tm t;
    ::localtime_r(&time, &t);
    init(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min);
}

void DateTime::init(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute)
{

    //printf("%d %d %d %d %d\n", year, month, day, hour, minute);

    // 1940 is the earliest year, so clamp to 1940-01-01 00:00
    if (year < 1940)
    {
        _yymmdd = (40 << 9) | (1 << 5) | 1;
        _hhmm = 0;
        return;
    }
    // 2039 is the latest year, so clamp to 2039-12-31 23:59
    if (year > 2039)
    {
        _yymmdd = (39 << 9) | (12 << 5) | 31;
        _hhmm = (23 << 8) | 59;
        return;
    }

    if (year >= 2000) year -= 2000;
    else year -= 1900;

    _hhmm = minute | (hour << 8);

    _yymmdd = day | (month << 5) | (year << 9);
}

unsigned DateTime::year() const
{
    unsigned tmp = _yymmdd >> 9;
    //if (tmp > 100) return 0;
  
    if (tmp <= 39) tmp += 100;

    return tmp + 1900;
}

/*
 * A positive or 0 value for tm_isdst causes mktime() to presume initially
 * that Daylight Savings Time, respectively, is or is not in effect for
 * the specified time. A negative value for tm_isdst causes mktime() to
 * attempt to determine whether Daylight Saving Time is in effect for the
 * specified time.
 */

/**
 * Convert to to UNIX time
 * @return the UNIX time
 */
std::time_t DateTime::toUnix() const
{
    tm t;

    if (_yymmdd == 0) return 0;

    std::memset(&t, 0, sizeof(tm));

    t.tm_min = minute();
    t.tm_hour = hour();
    t.tm_isdst = -1;

    t.tm_mday = day();
    t.tm_mon = month() - 1;
    t.tm_year = year() - 1900;

    return std::mktime(&t);
    // convert back via locatime & fudge for dst?
}

