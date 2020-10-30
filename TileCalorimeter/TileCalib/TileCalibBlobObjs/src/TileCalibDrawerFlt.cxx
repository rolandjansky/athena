/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include <iostream>
#include <algorithm>

//
//_____________________________________________________________
const TileCalibDrawerFlt* 
TileCalibDrawerFlt::getInstance(const coral::Blob& blob)
{
  //=== const Blob needs to be correctly formated
  if(blob.size()<static_cast<long>(m_hdrSize32)){
    throw TileCalib::InvalidBlob("TileCalibDrawerFlt::getInstance");
  }
  return (new TileCalibDrawerFlt(blob));
}

//
//_____________________________________________________________
TileCalibDrawerFlt* 
TileCalibDrawerFlt::getInstance(coral::Blob&        blob,
				const DefType&      def, 
				uint16_t            nChans,
				uint16_t            objVers,
				const std::string&  author,
				const std::string&  comment,
				uint64_t            timeStamp)
{
  TileCalibDrawerFlt* calibDrawer = new TileCalibDrawerFlt(blob);
  calibDrawer->init(def,nChans,objVers,author,comment,timeStamp);
  return calibDrawer;
}

//
//_______________________________________________________________
TileCalibDrawerFlt::TileCalibDrawerFlt(const coral::Blob& blob) : 
  TileCalibDrawerDat<float>(blob)
{
  if(getBlobSize()){
    if(getObjType() != getType()){
      throw TileCalib::TypeConflict("TileCalibDrawerFlt::Ctor",getObjType(),getType());
    }
  }
}

//
//_______________________________________________________________
TileCalibDrawerFlt::TileCalibDrawerFlt(coral::Blob& blob) : 
  TileCalibDrawerDat<float>(blob)
{
  if(getBlobSize()){
    if(getObjType() != getType()){
      throw TileCalib::TypeConflict("TileCalibDrawerFlt::Ctor",getObjType(),getType());
    }
  }
}

//
//______________________________________________________________
float 
TileCalibDrawerFlt::getCalib(unsigned int channel, unsigned int adc, float energy, bool invert) const
{
  //=== interpretation of data depends on the objVersion
  if(getObjVersion()==1){

    //=== Application of a single slope
    float slope = TileCalibDrawerFlt::getData(channel,adc,0);
    if(invert) 
      { 
	energy *= slope; 
      }
    else       
      { 
	if(slope==0.) throw TileCalib::InvalidValue("TileCalibDrawerFlt::getCalib(), SLOPE",slope);
	energy /= slope; 
      }
    return energy;
    
  }
  else if(getObjVersion()==100){
    
    //============================================================================
    //=== Look up table (LUT)
    //=== Values are stored in one x and one y column
    //=== Note: when filling the vector for the sqlite input file
    //=== first insert all x-values and then all y-values
    //=== Linear interpolation is done between two x-value entries
    //=== Under- and overflows return the input value (convenient for CIS LUT)
    //=== The return value is the LUT value!
    //============================================================================

    //NGO need to do something if "invert" is requested

    //NGO change LUT format to slope and residual? Makes it more consistent with rest
    //NGO and easier for visualization??

    //=== need at least 2 (x,y) pairs,
    //=== return unit function as default
    if(getObjSizeUint32()<4){
      return energy;
    }

    //=== Tile Default Policy has to be enforced here again, 
    //=== since getAddress() below assumes valid channel & adc

    if(channel >= getNChans()) {
      if (channel <= TileCalibUtils::MAX_CHAN) {
        channel = 0;
      } else {
        channel = channel % TileCalibUtils::MAX_CHAN;
        if (channel >= getNChans()) channel = 0;
      }
    }

    if(adc >= getNGains()) {adc = 0;}
    
    //=== determine position in table
    const float* xBeg   = getAddress(channel,adc);
    const float* xEnd   = xBeg + getObjSizeUint32()/2;
    const float* iX     = std::upper_bound(xBeg,xEnd,energy);
    unsigned int offset = iX-xBeg;
    float y(0.);
    //=== boundary values
    if(iX==xBeg || iX==xEnd){
      y = energy;
    }
    //=== linear interpolation
    else{
      float x1 = *(iX  );
      float x0 = *(iX-1);
      float y1 = *(xEnd+offset  );
      float y0 = *(xEnd+offset-1);
      y = y0+(energy-x0)*(y1-y0)/(x1-x0);  
    }
    return y;
  }

  //=== did not recognize the blob version 
  throw TileCalib::VersionConflict("TileCalibDrawerFlt::getCalib",getObjVersion());
}

//
//______________________________________________________________
bool  
TileCalibDrawerFlt::getYDY(unsigned int channel, unsigned int adc, float x, 
			   float& y, float& dy) const
{
  //=== need to have a "function" blob version
  if(getObjVersion()!=200){
    throw TileCalib::VersionConflict("TileCalibDrawerFlt::getYDY",getObjVersion());
  }
    
  //=== require minimum pulse shape
  if(getObjSizeUint32()<4){
    return false;
  }
  
  //=== Tile Default Policy has to be enforced here again, 
  //=== since getAddress() below assumes valid channel & adc

  if(channel >= getNChans()) {
    if (channel <= TileCalibUtils::MAX_CHAN) {
      channel = 0;
    } else {
      channel = channel % TileCalibUtils::MAX_CHAN;
      if (channel >= getNChans()) channel = 0;
    }
  }

  if( adc >= getNGains()) {adc = 0;}
  
  //=== determine position in table
  const float* xBeg   = getAddress(channel,adc);
  const float* xEnd   = xBeg + getObjSizeUint32()/2;
  const float* iX     = std::upper_bound(xBeg,xEnd,x);
  unsigned int offset = iX-xBeg;

  //=== under-/overflow: return boundary y and dy==0
  //=== offset delta is to avoid float comparison troubles
  const float delta = 1.0e-3;
  if(iX==xBeg) {
    y  = *(xEnd+offset);
    dy = 0.;
    if (x < (*(xBeg) - delta)) return false;
  }
  else if(iX==xEnd) {
    y  = *(xEnd+offset-1);
    dy = 0.;
    if  (x > (*(xEnd-1) + delta)) return false;
  }

  //=== linear interpolation to determine y
  else{
    float x1 = *(iX  );
    float x0 = *(iX-1);
    float y1 = *(xEnd+offset  );
    float y0 = *(xEnd+offset-1);
    float slope = (y1-y0)/(x1-x0);
    y  = y0+(x-x0)*slope;  
    
    //=== determine dy from secant near [x,y]
    float dx = x1 - x0;
    float xs1 = x + 0.5 * dx;
    float xs0 = x - 0.5 * dx;
    dy = (getY(channel, adc, xs1) - getY(channel, adc, xs0)) / dx;
  }
  return true;
}

//
//______________________________________________________________
float  
TileCalibDrawerFlt::getY(unsigned int channel, unsigned int adc, float x) const 
{
  float y(-9999.);
  //=== need to have a "function" blob version
  if(getObjVersion()!=200){
    throw TileCalib::VersionConflict("TileCalibDrawerFlt::getY",getObjVersion());
  }
    
  //=== require minimum pulse shape
  if(getObjSizeUint32()<4){
    return false;
  }
  
  //=== Tile Default Policy has to be enforced here again, 
  //=== since getAddress() below assumes valid channel & adc
  
  if(channel >= getNChans()) {
    if (channel <= TileCalibUtils::MAX_CHAN) {
      channel = 0;
    } else {
      channel = channel % TileCalibUtils::MAX_CHAN;
      if (channel >= getNChans()) channel = 0;
    }
  }
  
  if(adc >= getNGains()) {adc = 0;}
  
  //=== determine position in table
  const float* xBeg   = getAddress(channel,adc);
  const float* xEnd   = xBeg + getObjSizeUint32()/2;
  const float* iX     = std::upper_bound(xBeg,xEnd,x);
  unsigned int offset = iX-xBeg;

  //=== under-/overflow: return boundary y
  if(iX==xBeg){
    y  = *(xEnd+offset);
    return y;
  }
  else if(iX==xEnd){
    y  = *(xEnd+offset-1);
    return y;
  }

  //=== linear interpolation to determine y
  else{
    float x1 = *(iX  );
    float x0 = *(iX-1);
    float y1 = *(xEnd+offset  );
    float y0 = *(xEnd+offset-1);
    float slope = (y1-y0)/(x1-x0);
    y  = y0+(x-x0)*slope;  
  }
  return y;
}

//
//______________________________________________________________
float  
TileCalibDrawerFlt::getDY(unsigned int channel, unsigned int adc, float x) const 
{
  float y(-9999.), dy(-9999.);
  (void)getYDY(channel,adc,x,y,dy);
  return dy;
}

