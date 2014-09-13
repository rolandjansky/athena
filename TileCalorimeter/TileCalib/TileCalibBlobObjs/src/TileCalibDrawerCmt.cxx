/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibDrawerCmt.h"
#include "TileCalibBlobObjs/TileCalibType.h"

//
//_____________________________________________________________
TileCalibDrawerCmt* 
TileCalibDrawerCmt::getInstance(coral::Blob&       blob,					 
				const std::string& author, 
				const std::string& comment, 
				uint64_t           timeStamp)
{
  TileCalibDrawerCmt* calibDrawer = new TileCalibDrawerCmt(blob);
  calibDrawer->init(author,comment,timeStamp);
  return calibDrawer;
}

//
//_____________________________________________________________
const TileCalibDrawerCmt* 
TileCalibDrawerCmt::getInstance(const coral::Blob& blob)
{
  //=== const Blob needs to be correctly formated
  if(blob.size()<static_cast<long>(m_hdrSize32)){
    throw TileCalib::InvalidBlob("TileCalibDrawerCmt::getInstance");
  }
  return (new TileCalibDrawerCmt(blob));
}

//
//__________________________________________________________________
TileCalibDrawerCmt::TileCalibDrawerCmt(const coral::Blob& blob)
  
  : TileCalibDrawerBase(blob)
{
  //=== no type check here
}

//
//__________________________________________________________________
void 
TileCalibDrawerCmt::init(const std::string& author, 
			 const std::string& comment, 
			 uint64_t timeStamp)
{
  createBlob(TileCalibType::CMT,0,0,0,0,0,author,comment,timeStamp);
}

