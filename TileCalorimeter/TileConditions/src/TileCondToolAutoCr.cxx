/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileConditions/TileCondToolAutoCr.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolAutoCr("TileCondToolAutoCr", 1, 0);

const InterfaceID& TileCondToolAutoCr::interfaceID() {
  return IID_TileCondToolAutoCr;
}

//
//____________________________________________________________________
TileCondToolAutoCr::TileCondToolAutoCr(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_pryNoiseAutoCr("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_NoiseAutoCr", this)
{
  //  declareInterface<ITileCondToolAutoCr>(this);
  declareInterface<TileCondToolAutoCr>(this);
  declareProperty("ProxyNoiseAutoCr", m_pryNoiseAutoCr);
}

//
//____________________________________________________________________
TileCondToolAutoCr::~TileCondToolAutoCr() {
}

//
//____________________________________________________________________
StatusCode TileCondToolAutoCr::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== Retrieve proxy
  CHECK( m_pryNoiseAutoCr.retrieve() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolAutoCr::finalize() {

  ATH_MSG_DEBUG( "finalize called" );
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________

void TileCondToolAutoCr::getAutoCorr(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
    std::vector<float>& vec) const {
  const TileCalibDrawerFlt* calibDrawer = m_pryNoiseAutoCr->getCalibDrawer(drawerIdx);
  unsigned int nNum = calibDrawer->getObjSizeUint32();

  if (vec.size() != nNum) {
    vec.resize(nNum);
  }

  for (unsigned int i = 0; i < nNum; ++i) {
    vec[i] = calibDrawer->getData(channel, adc, i);
  }
}

