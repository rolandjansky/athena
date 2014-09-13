/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibDrawerInt.h"
#include <iostream>

//
//_____________________________________________________________
TileCalibDrawerInt* 
TileCalibDrawerInt::getInstance(coral::Blob&              blob,
				const DefType&            def, 
				uint16_t                  nChans,
				uint16_t                  objVers,
				const std::string&        author,
				const std::string&        comment,
				uint64_t                  timeStamp)
{
  TileCalibDrawerInt* calibDrawer = new TileCalibDrawerInt(blob);
  calibDrawer->init(def,nChans,objVers,author,comment,timeStamp);
  return calibDrawer;
}

//
//_____________________________________________________________
const TileCalibDrawerInt* 
TileCalibDrawerInt::getInstance(const coral::Blob& blob)
{
  //=== const Blob needs to be correctly formated
  if(blob.size()<static_cast<long>(m_hdrSize32)){
    throw TileCalib::InvalidBlob("TileCalibDrawerBch::getInstance");
  }
  return (new TileCalibDrawerInt(blob));
}

//
//_______________________________________________________________
TileCalibDrawerInt::TileCalibDrawerInt(const coral::Blob& blob) 
  : TileCalibDrawerDat<uint32_t>(blob)
{
  //=== check for correct blob type
  if(getBlobSize()){
    if(getObjType() != getType()){
      throw TileCalib::TypeConflict("TileCalibDrawerInt::Ctor",getObjType(),getType());
    }
  }
}

