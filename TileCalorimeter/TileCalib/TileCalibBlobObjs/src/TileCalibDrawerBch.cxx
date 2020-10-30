/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibDrawerBch.h"
#include <iostream>

//
//_____________________________________________________________
TileCalibDrawerBch* 
TileCalibDrawerBch::getInstance(coral::Blob&              blob,
				const DefType&            def, 
				uint16_t                  nChans,
				TileBchDecoder::BitPatVer bitPatVer,
				const std::string&        author,
				const std::string&        comment,
				uint64_t                  timeStamp)
{
  TileCalibDrawerBch* calibDrawer = new TileCalibDrawerBch(blob);
  uint16_t objVers = static_cast<uint16_t>(bitPatVer);
  calibDrawer->init(def,nChans,objVers,author,comment,timeStamp);
  return calibDrawer;
}

//
//_____________________________________________________________
const TileCalibDrawerBch* 
TileCalibDrawerBch::getInstance(const coral::Blob& blob)
{
  //=== const Blob needs to be correctly formated
  if(blob.size()<static_cast<long>(m_hdrSize32)){
    throw TileCalib::InvalidBlob("TileCalibDrawerBch::getInstance");
  }
  return (new TileCalibDrawerBch(blob));
}

//
//_______________________________________________________________
TileCalibDrawerBch::TileCalibDrawerBch(const coral::Blob& blob) 
  : TileCalibDrawerDat<uint32_t>(blob)
{
  //=== check for correct blob type
  if(getBlobSize()){
    if(getObjType() != getType()){
      throw TileCalib::TypeConflict("TileCalibDrawerBch::Ctor",getObjType(),getType());
    }
  }
}

//
//_______________________________________________________________
TileCalibDrawerBch::TileCalibDrawerBch(coral::Blob& blob) 
  : TileCalibDrawerDat<uint32_t>(blob)
{
  //=== check for correct blob type
  if(getBlobSize()){
    if(getObjType() != getType()){
      throw TileCalib::TypeConflict("TileCalibDrawerBch::Ctor",getObjType(),getType());
    }
  }
}

