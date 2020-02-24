/*
   This file is part of ArduinoIoTCloud.

   Copyright 2020 ARDUINO SA (http://www.arduino.cc/)

   This software is released under the GNU General Public License version 3,
   which covers the main part of arduino-cli.
   The terms of this license can be found at:
   https://www.gnu.org/licenses/gpl-3.0.en.html

   You can be released from the requirements of the above licenses by purchasing
   a commercial license. Buying such a license is mandatory if you want to modify or
   otherwise use the software for commercial activities involving the Arduino
   software without disclosing the source code of your own applications. To purchase
   a commercial license, send an email to license@arduino.cc.
*/

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "../ArduinoIoTCloud_Defines.h"
#ifndef HAS_LORA

#include "TimeService.h"

#include <time.h>

#include "NTPUtils.h"

/**************************************************************************************
 * INTERNAL FUNCTION DECLARATION
 **************************************************************************************/

time_t cvt_time(char const * time);

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static time_t const EPOCH_AT_COMPILE_TIME = cvt_time(__DATE__);

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

TimeService::TimeService()
: _con_hdl(nullptr)
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

void TimeService::begin(ConnectionHandler * con_hdl)
{
  _con_hdl = con_hdl;
}

unsigned long TimeService::getTime()
{
  if(_con_hdl == nullptr) return 0;

  /* At first try to see if a valid time can be obtained
   * using the network time available via the connection
   * handler.
   */
  unsigned long const connection_time = _con_hdl->getTime();
  if(isTimeValid(connection_time)) {
    return connection_time;
  }

  /* If no valid network time is available try to obtain the
   * time via NTP next.
   */
  unsigned long const ntp_time = NTPUtils::getTime(_con_hdl->getUDP());
  if(isTimeValid(ntp_time)) {
    return ntp_time;
  }

  return 0;
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

bool TimeService::isTimeValid(unsigned long const time)
{
  return (time >= EPOCH_AT_COMPILE_TIME);
}

/**************************************************************************************
 * INTERNAL FUNCTION DEFINITION
 **************************************************************************************/

time_t cvt_time(char const * time)
{
  char s_month[5];
  int month, day, year;
  struct tm t =
  {
    0 /* tm_sec   */,
    0 /* tm_min   */,
    0 /* tm_hour  */,
    0 /* tm_mday  */,
    0 /* tm_mon   */,
    0 /* tm_year  */,
    0 /* tm_wday  */,
    0 /* tm_yday  */,
    0 /* tm_isdst */
  };
  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

  sscanf(time, "%s %d %d", s_month, &day, &year);

  month = (strstr(month_names, s_month) - month_names) / 3;

  t.tm_mon = month;
  t.tm_mday = day;
  t.tm_year = year - 1900;
  t.tm_isdst = -1;

  return mktime(&t);
}

#endif /* #ifndef HAS_LORA */
