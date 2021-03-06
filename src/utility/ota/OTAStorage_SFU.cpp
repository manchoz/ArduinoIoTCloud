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

/******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include <AIoTC_Config.h>
#if OTA_STORAGE_SFU

#include "OTAStorage_SFU.h"

#include <Arduino_DebugUtils.h>

/******************************************************************************
 * CONSTANTS
 ******************************************************************************/

static char const SFU_UPDATE_FILENAME[]      = "UPDATE.BIN";
static char const SFU_TEMP_UPDATE_FILENAME[] = "UPDATE.BIN.TMP";

/******************************************************************************
 * CTOR/DTOR
 ******************************************************************************/

OTAStorage_SFU::OTAStorage_SFU()
: _file{nullptr}
{

}

/******************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 ******************************************************************************/

bool OTAStorage_SFU::init()
{
  flash.begin();
  if(SPIFFS_OK != filesystem.mount()) {
    DBG_ERROR("OTAStorage_SFU::init - mount() failed with error code %d", filesystem.err());
    return false;
  }

  if(SPIFFS_OK != filesystem.check()) {
    DBG_ERROR("OTAStorage_SFU::init - check() failed with error code %d", filesystem.err());
    return false;
  }

  return true;
}

bool OTAStorage_SFU::open()
{
  filesystem.clearerr();
  _file = new File(filesystem.open(SFU_TEMP_UPDATE_FILENAME, CREATE | WRITE_ONLY| TRUNCATE));
  if(SPIFFS_OK != filesystem.err()) {
    DBG_ERROR("OTAStorage_SFU::open - open() failed with error code %d", filesystem.err());
    delete _file;
    return false;
  }
  return true;
}

size_t OTAStorage_SFU::write(uint8_t const * const buf, size_t const num_bytes)
{
  return _file->write(const_cast<uint8_t *>(buf), num_bytes);
}

void OTAStorage_SFU::close()
{
  _file->close();
  delete _file;
}

void OTAStorage_SFU::remove()
{
  filesystem.remove(SFU_TEMP_UPDATE_FILENAME);
}

bool OTAStorage_SFU::rename()
{
  return (SPIFFS_OK == filesystem.rename(SFU_TEMP_UPDATE_FILENAME, SFU_UPDATE_FILENAME));
}

void OTAStorage_SFU::deinit()
{
  filesystem.unmount();
}

#endif /* OTA_STORAGE_SFU */
