/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cmath>

const unsigned int TileCalibUtils::MAX_ROS;
const unsigned int TileCalibUtils::MAX_DRAWER;
const unsigned int TileCalibUtils::MAX_DRAWR0;
const unsigned int TileCalibUtils::MAX_CHAN;
const unsigned int TileCalibUtils::MAX_GAIN;
const unsigned int TileCalibUtils::MAX_DRAWERIDX;
const unsigned int TileCalibUtils::LAS_PART_CHAN;
const unsigned int TileCalibUtils::TRIPS_DRAWERIDX;
const unsigned int TileCalibUtils::TRIPS_ROS;
const unsigned int TileCalibUtils::TRIPS_DRAWER;
const unsigned int TileCalibUtils::DEFINITIONS_DRAWERIDX;
const unsigned int TileCalibUtils::BAD_DEFINITION_CHAN;
const unsigned int TileCalibUtils::NOISY_DEFINITION_CHAN;
const unsigned int TileCalibUtils::NOGAINL1_DEFINITION_CHAN;
const unsigned int TileCalibUtils::BADTIMING_DEFINITION_CHAN;
const unsigned int TileCalibUtils::WRONGBCID_DEFINITION_CHAN;
const unsigned int TileCalibUtils::TIMINGDMUBCOFFSET_DEFINITION_CHAN;

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
  bool isLower(true);
  for (const char& c : folder) {
    if(c == '/'){
      isLower = false;
    } else if(isLower) {
      folderTag += char(std::tolower(c));
    } else {
      folderTag += char(std::toupper(c));
      isLower = true;
    }
  }

  return (folderTag + "-" + tag);
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getDrawerIdx(unsigned int ros, unsigned int drawer)
{
  if(ros >= MAX_ROS){                throw TileCalib::IndexOutOfRange("TileCalibUtils::getDrawerIdx", ros   , MAX_ROS   );}
  if(!ros){ if(drawer >= MAX_DRAWR0) throw TileCalib::IndexOutOfRange("TileCalibUtils::getDrawerIdx", drawer, MAX_DRAWR0);}
  else    { if(drawer >= MAX_DRAWER) throw TileCalib::IndexOutOfRange("TileCalibUtils::getDrawerIdx", drawer, MAX_DRAWER);}
  return (ros == 0 ? 0 : MAX_DRAWR0 + (ros - 1) * MAX_DRAWER) + drawer;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getDrawerIdxFromFragId(unsigned int fragId)
{
  unsigned int ros    = fragId >> 8;
  unsigned int drawer = fragId & 0xFF; 
  return getDrawerIdx(ros, drawer);
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getChanIdx(unsigned int ros, unsigned int drawer, unsigned int channel)
{
  if(channel >= MAX_CHAN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx", channel, MAX_CHAN);
  return getDrawerIdx(ros, drawer) * MAX_CHAN + channel;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getChanIdx(unsigned int drawerIdx, unsigned int channel)
{
  if(drawerIdx >= MAX_DRAWERIDX) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx(drawIdx)", channel, MAX_DRAWERIDX);
  if(channel  >= MAX_CHAN     ) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx(channel)", channel, MAX_CHAN);
  return drawerIdx * MAX_CHAN + channel;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getChanIdxFromFragId(unsigned int fragId, unsigned int channel)
{
  if(channel >= MAX_CHAN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx", channel, MAX_CHAN);
  return getDrawerIdxFromFragId(fragId) * MAX_CHAN + channel;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getAdcIdx(unsigned int ros, unsigned int drawer, unsigned int channel, unsigned int adc)
{
  if(adc >= MAX_GAIN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getAdcIdx", adc, MAX_GAIN);
  return getChanIdx(ros, drawer, channel) * MAX_GAIN + adc;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getAdcIdx(unsigned int drawerIdx, unsigned int channel, unsigned int adc)
{
  if(adc >= MAX_GAIN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getAdcIdx", adc, MAX_GAIN);
  return getChanIdx(drawerIdx, channel) * MAX_GAIN + adc;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getAdcIdxFromFragId(unsigned int fragId, unsigned int channel, unsigned int adc)
{
  if(adc >= MAX_GAIN) throw TileCalib::IndexOutOfRange("TileCalibUtils::getAdcIdx", adc, MAX_GAIN);
  return getChanIdxFromFragId(fragId, channel) * MAX_GAIN + adc;
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getMaxDrawer(unsigned int ros)
{
  if(ros >= MAX_ROS){throw TileCalib::IndexOutOfRange("TileCalibUtils::getMaxDrawer", ros, MAX_ROS);}
  return   ros == 0 ? MAX_DRAWR0 : MAX_DRAWER;
}

//
//_____________________________________________________________________________
std::string 
TileCalibUtils::getDrawerString(unsigned int ros, unsigned int drawer)
{
  //=== check for valid index
  TileCalibUtils::getDrawerIdx(ros, drawer);
  //=== build ros name
  std::ostringstream name;
  if     (ros == 1){ name << "LBA"; }
  else if(ros == 2){ name << "LBC"; }
  else if(ros == 3){ name << "EBA"; }
  else if(ros == 4){ name << "EBC"; }
  else{              name << "AUX"; }
  //=== add drawer number
  name << std::setw(2) << std::setfill('0') << drawer + 1;
  return name.str();
}

//
//_____________________________________________________________________________
unsigned int TileCalibUtils::getDefaultDrawerIdx(unsigned int drawerIdx) {

  //=== Global Detector defaults in  0 -  3
  //=== 
  //===  Defaults for LB         in  4 - 11
  //===  Defaults for EB         in 12 - 19
  //===
  //===
  //===  Defaults are further shifted in extended barrel: 
  //---   + 1 for Inner MBTS and special C10
  //---   + 2 for Outer MBTS
  //---   + 3 for Merged E1
  //---   + 4 for E4'
  //---   + 5 for Special D40 with missing D4: EBA15/EBC18
  //---   + 6 for Special D4 reduced in size
  //---   + 7 for D4 on the top of EB around special D4
  //
  //---  e.g.:

  //---   LB                                         in  4

  //---   EB                                         in 12 
  //---   EB (Inner MBTS + special C10)              in 13 
  //---   EB (Outer MBTS)                            in 14 
  //---   EB (Merged E1)                             in 15 
  //---   EB (E4')                                   in 16 
  //---   EB (Special D40: EBA15/EBC18)              in 17 
  //---   EB (Special D4 reduced in size)            in 18
  //---   EB (D4 on the top of EB around specail D4) in 19


    //=== defaults for defaults
  if(drawerIdx < MAX_DRAWR0) {
    if (drawerIdx < 4) return 0;
    unsigned int mod = (drawerIdx - 4) % 8;
    return mod > 0 ? drawerIdx - mod : 0;
  }
  
  
  //=== defaults for existing drawers
  drawerIdx -= MAX_DRAWR0;
  if (drawerIdx < 2 * MAX_DRAWER) return 4; // LB
    
  drawerIdx -= 2 * MAX_DRAWER;
  if (drawerIdx < MAX_DRAWER) { // EBA
    
    int  OffsetEBA[] = { 0, 0, 0, 0, 0, 0, 3, 2, // Merged E+1: EBA07; Outer MBTS: EBA08
                         0, 0, 0, 0, 7, 6, 5, 7, // D+4: EBA13, EBA16; Special D+4: EBA14; Special D+40: EBA15
                         7, 6, 6, 7, 0, 0, 0, 2, // D+4: EBA17, EBA20; Special D+4: EBA18, EBA19; Outer MBTS: EBA24
                         3, 0, 0, 0, 0, 0, 0, 0, // Merged E+1:  EBA25
                         0, 0, 0, 0, 0, 0, 1, 1, // Inner MBTS + special C+10: EBA39, EBA40
                         1, 1, 2, 3, 0, 0, 0, 0, // Inner MBTS + special C+10: EBA41, EBA42; Outer MBTS: EBA43; Merged E+1: EBA44
                         0, 0, 0, 0, 3, 2, 1, 1, // Merged E+1: EBA53; Outer MBTS: EBA54; Inner MBTS + special C+10: EBA55, EBA56
                         1, 1, 0, 0, 0, 0, 0, 0  // Inner MBTS + special C+10: EBA57, EBA58
                         
    };
    
    return 12 + OffsetEBA[drawerIdx];
    
  } 

  // EBC
  drawerIdx -= MAX_DRAWER;
  int  OffsetEBC[] = { 0, 0, 0, 0, 0, 0, 3, 2, // Merged E-1: EBC07; Outer MBTS: EBC08
                       0, 0, 0, 0, 7, 6, 6, 7, // D-4: EBC13, EBC16; Special D-4: EBC14, EBC15; 
                       7, 5, 6, 7, 0, 0, 0, 2, // D-4: EBC17, EBC20; Special D-40 EBC18; Special D-4: EBC19; Outer MBTS: EBC24
                       3, 0, 0, 3, 4, 0, 3, 4, // Merged E-1:  EBC25, EBC28, EBC31; E-4': EBC29, EBC32
                       0, 4, 3, 0, 4, 3, 1, 1, // E-4': EBC34, EBC37; Merged E-1: EBC35, EBC38; Inner MBTS + special C-10: EBC39, EBC40
                       1, 1, 2, 3, 0, 0, 0, 0, // Inner MBTS + special C-10: EBC41, EBC42; Outer MBTS: EBC43; Merged E-1: EBC44
                       0, 0, 0, 0, 3, 2, 1, 1, // Merged E-1: EBC53; Outer MBTS: EBC54; Inner MBTS + special C-10: EBC55, EBC56
                       1, 1, 0, 0, 0, 0, 0, 0  // Inner MBTS + special C-10: EBC57, EBC58
  };
  
  // Without this check we get a cppcheck false positive.
  if (drawerIdx >= std::size(OffsetEBC)) std::abort();
  return 12 + OffsetEBC[drawerIdx];
  
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getDefaultDrawerIdx(unsigned int ros, unsigned int drawer)
{
  return getDefaultDrawerIdx(getDrawerIdx(ros, drawer));
}

//
//_____________________________________________________________________________
unsigned int 
TileCalibUtils::getFirstDrawerInPartitionIdx(unsigned int drawerIdx)
{
  if(drawerIdx >= MAX_DRAWERIDX) throw TileCalib::IndexOutOfRange("TileCalibUtils::getChanIdx(drawIdx)", 0, MAX_DRAWERIDX);

  unsigned int drawer0 = 0;
  if(drawerIdx < MAX_DRAWR0) return drawer0;
  else if(drawerIdx < MAX_DRAWR0 + 1 * MAX_DRAWER) return MAX_DRAWR0;
  else if(drawerIdx < MAX_DRAWR0 + 2 * MAX_DRAWER) return MAX_DRAWR0 + 1 * MAX_DRAWER;
  else if(drawerIdx < MAX_DRAWR0 + 3 * MAX_DRAWER) return MAX_DRAWR0 + 2 * MAX_DRAWER;
  else return MAX_DRAWR0 + 3 * MAX_DRAWER;
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
  if(absVal != 0.){
    scale = static_cast<int>( ::truncf(std::log((std::pow(2., static_cast<int>(nBits)) - 1.)/absVal)*(1./std::log(2.))) );
  }
  //=== return input value with fixed point precision
  return ::roundf(val * std::pow(2., scale)) / std::pow(2., scale);
}

