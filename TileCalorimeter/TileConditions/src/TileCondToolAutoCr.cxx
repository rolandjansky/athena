/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Tile includes
#include "TileConditions/TileCondToolAutoCr.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

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
{
  declareInterface<TileCondToolAutoCr>(this);

}

//
//____________________________________________________________________
TileCondToolAutoCr::~TileCondToolAutoCr() {
}

//
//____________________________________________________________________
StatusCode TileCondToolAutoCr::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== Initialize conditions data key with auto correlations
  ATH_CHECK( m_calibAutorCorrelationKey.initialize() );

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

void TileCondToolAutoCr::getAutoCorr(unsigned int drawerIdx,
                                     unsigned int channel,
                                     unsigned int adc,
                                     std::vector<float>& vec) const {


  SG::ReadCondHandle<TileCalibDataFlt> calibAutoCorrelation(m_calibAutorCorrelationKey);
  const TileCalibDrawerFlt* calibDrawer = calibAutoCorrelation->getCalibDrawer(drawerIdx);
  const unsigned int nElements(calibDrawer->getObjSizeUint32());

  if (vec.size() != nElements) {
    vec.resize(nElements);
  }

  for (unsigned int i = 0; i < nElements; ++i) {
    vec[i] = calibDrawer->getData(channel, adc, i);
  }
}
