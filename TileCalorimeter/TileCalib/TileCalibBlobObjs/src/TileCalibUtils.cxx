/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cmath>

const unsigned int TileCalibUtils::MAX_ROS       =   5;  /**< @brief Number of ROSs               */
const unsigned int TileCalibUtils::MAX_DRAWER    =  64;  /**< @brief Number of drawers in ROS 1-4 */
const unsigned int TileCalibUtils::MAX_DRAWR0    =  20;  /**< @brief Number of drawers in ROS 0   */
const unsigned int TileCalibUtils::MAX_CHAN      =  48;  /**< @brief Number of channels in drawer */
const unsigned int TileCalibUtils::MAX_GAIN      =   2;  /**< @brief Number of gains per channel  */
const unsigned int TileCalibUtils::MAX_DRAWERIDX = 276;  /**< @brief Maximal drawer index         */
const unsigned int TileCalibUtils::LAS_PART_CHAN =  43;  /**< @brief Empty channel number to store laser partition variation */
const unsigned int TileCalibUtils::TRIPS_DRAWERIDX = 2;  /**< @brief DrawerIdx used for storing trips probabilities */
const unsigned int TileCalibUtils::TRIPS_ROS     =   0;  /**< @brief Ros used for storing trips probabilities */
const unsigned int TileCalibUtils::TRIPS_DRAWER  =   2;  /**< @brief Drawer used for storing trips probabilities */
const unsigned int TileCalibUtils::DEFINITIONS_DRAWERIDX = 1; /**< @brief Drawer used for storing of bad and noisy channel definitions */
const unsigned int TileCalibUtils::BAD_DEFINITION_CHAN = 0;  /**< @brief Channel used for storing of bad channel definitions */
const unsigned int TileCalibUtils::NOISY_DEFINITION_CHAN = 1; /**< @brief Channel used for storing of noisy channel definitions */
const unsigned int TileCalibUtils::NOGAINL1_DEFINITION_CHAN = 2; /**< @brief Channel used for storing of NoGainLevel1 channel definitions */
const unsigned int TileCalibUtils::BADTIMING_DEFINITION_CHAN = 3; /**< @brief Channel used for storing of NoGainLevel1 channel definitions */

//
//_____________________________________________________________________________
std::string 
TileCalibUtils::getFullTag(const std::string& folder, const std::string& tag)
  //=== 
  //=== build a unique cool tag based on the tag conventions outline here:
  //=== https://twiki.cern.ch/twiki/bin/view/Atlas/CoolTagging
  //=== "tag" should be of the form "xxx-yy" for real data
  //===
{
  std::string folderTag("");
  for (std::string::const_iterator i = folder.begin(); i != folder.end(); i++) {
    if(*i=='/'){
      i++;
      folderTag+=char(std::toupper(*i));
    }
    else{
      folderTag+=char(std::tolower(*i));
    }
  }
  return (folderTag+"-"+tag);
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getDrawerIdx(unsigned int ros, unsigned int drawer)
{
  if(ros>=MAX_ROS){                throw TileCalib::IndexOutOfRange("TileCalibUtils::getDrawerIdx",ros   ,MAX_ROS   );}
  if(!ros){ if(drawer>=MAX_DRAWR0) throw TileCalib::IndexOutOfRange("TileCalibUtils::getDrawerIdx",drawer,MAX_DRAWR0);}
  else    { if(drawer>=MAX_DRAWER) throw TileCalib::IndexOutOfRange("TileCalibUtils::getDrawerIdx",drawer,MAX_DRAWER);}
  return (ros==0 ? 0 : MAX_DRAWR0+(ros-1)*MAX_DRAWER ) + drawer;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getDrawerIdxFromFragId(unsigned int fragId)
{
  unsigned int ros    = fragId >> 8;
  unsigned int drawer = fragId & 0xFF; 
  return getDrawerIdx(ros,drawer);
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getChanIdx(unsigned int ros, unsigned int drawer, unsigned int channel)
{
  if(channel>=MAX_CHAN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx",channel,MAX_CHAN);
  return getDrawerIdx(ros,drawer) * MAX_CHAN + channel;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getChanIdx(unsigned int drawerIdx, unsigned int channel)
{
  if(drawerIdx>=MAX_DRAWERIDX) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx(drawIdx)",channel,MAX_DRAWERIDX);
  if(channel  >=MAX_CHAN     ) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx(channel)",channel,MAX_CHAN);
  return drawerIdx * MAX_CHAN + channel;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getChanIdxFromFragId(unsigned int fragId, unsigned int channel)
{
  if(channel>=MAX_CHAN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx",channel,MAX_CHAN);
  return getDrawerIdxFromFragId(fragId) * MAX_CHAN + channel;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getAdcIdx(unsigned int ros, unsigned int drawer, unsigned int channel, unsigned int adc)
{
  if(adc>=MAX_GAIN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getAdcIdx",adc,MAX_GAIN);
  return getChanIdx(ros,drawer,channel) * MAX_GAIN + adc;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getAdcIdx(unsigned int drawerIdx, unsigned int channel, unsigned int adc)
{
  if(adc>=MAX_GAIN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getAdcIdx",adc,MAX_GAIN);
  return getChanIdx(drawerIdx,channel) * MAX_GAIN + adc;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getAdcIdxFromFragId(unsigned int fragId, unsigned int channel, unsigned int adc)
{
  if(adc>=MAX_GAIN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getAdcIdx",adc,MAX_GAIN);
  return getChanIdxFromFragId(fragId,channel) * MAX_GAIN + adc;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getMaxDrawer(unsigned int ros)
{
  if(ros>=MAX_ROS){throw TileCalib::IndexOutOfRange("TileCalibUtils::getMaxDrawer",ros,MAX_ROS);}
  return   ros==0 ? MAX_DRAWR0 : MAX_DRAWER;
}

//
//_____________________________________________________________________________
std::string 
TileCalibUtils::getDrawerString(unsigned int ros, unsigned int drawer)
{
  //=== check for valid index
  TileCalibUtils::getDrawerIdx(ros,drawer);
  //=== build ros name
  std::ostringstream name;
  if     (ros==1){ name<<"LBA"; }
  else if(ros==2){ name<<"LBC"; }
  else if(ros==3){ name<<"EBA"; }
  else if(ros==4){ name<<"EBC"; }
  else{            name<<"AUX"; }
  //=== add drawer number
  name << std::setw(2) << std::setfill('0') << drawer+1;
  return name.str();
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getDefaultDrawerIdx(unsigned int drawerIdx)
{
  //=== Global Detector defaults in  0 -  3
  //=== Defaults for LBA         in  4 -  7
  //=== Defaults for LBC         in  8 - 11
  //=== Defaults for EBA         in 12 - 15
  //=== Defaults for EBC         in 16 - 19
  //===
  //=== Defaults are further subsegmented per sector (8 modules)
  //--- i.e. defaults for LBA01,LBA02, LBA09,LBA10,... in 4
  //---      defaults for LBA03,LBA04, LBA11,LBA12,... in 5
  //--- etc... 
  //=== default for eg. 5 is 4 (NOT 1 !)
  //=== default for eg. 4 is 0

  //=== defaults for defaults
  if(drawerIdx < MAX_DRAWR0){
    unsigned int mod = drawerIdx % 4;
    return mod>0 ? drawerIdx-mod : 0;
  }

  //=== defaults for existing drawers
  drawerIdx -= MAX_DRAWR0;
  unsigned int mod = (drawerIdx % 8) / 2;
  return  4*(drawerIdx/TileCalibUtils::MAX_DRAWER+1) + mod;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getDefaultDrawerIdx(unsigned int ros, unsigned int drawer)
{
  return getDefaultDrawerIdx(getDrawerIdx(ros,drawer));
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getFirstDrawerInPartitionIdx(unsigned int drawerIdx)
{
  if(drawerIdx>=MAX_DRAWERIDX) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx(drawIdx)",0,MAX_DRAWERIDX);

  unsigned int drawer0 = 0;
  if(drawerIdx < MAX_DRAWR0) return drawer0;
  else if(drawerIdx < MAX_DRAWR0 + 1*MAX_DRAWER) return MAX_DRAWR0;
  else if(drawerIdx < MAX_DRAWR0 + 2*MAX_DRAWER) return MAX_DRAWR0 + 1*MAX_DRAWER;
  else if(drawerIdx < MAX_DRAWR0 + 3*MAX_DRAWER) return MAX_DRAWR0 + 2*MAX_DRAWER;
  else return MAX_DRAWR0 + 3*MAX_DRAWER;
}

//
//____________________________________________________________________
float 
TileCalibUtils::fixedPointPrecision(float val, unsigned int nBits)
{
  nBits -= 1;
  
  //=== get scale
  int   scale  = 0;
  float absVal = std::fabs(val);
  if(absVal!=0.){
    scale = static_cast<int>( ::truncf(std::log((std::pow(2.,static_cast<int>(nBits))-1.)/absVal)/std::log(2.)) );
  }
  //=== return input value with fixed point precision
  return ::roundf(val * std::pow(2.,scale)) / std::pow(2.,scale);
}

