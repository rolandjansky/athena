/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolTiming.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolTiming("TileCondToolTiming", 1, 0);
const InterfaceID& TileCondToolTiming::interfaceID()
{
  return IID_TileCondToolTiming;
}

//
//____________________________________________________________________
TileCondToolTiming::TileCondToolTiming(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool( type, name, parent)
{

  declareInterface<TileCondToolTiming>(this);

}

//
//____________________________________________________________________
TileCondToolTiming::~TileCondToolTiming() {
}

//
//____________________________________________________________________
StatusCode TileCondToolTiming::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );


  //=== Initialize condition data key with timings
  ATH_CHECK( m_calibTimingKey.initialize() );


  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolTiming::finalize() {

  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}


//
//____________________________________________________________________
float TileCondToolTiming::getChannelOffset(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibTiming(m_calibTimingKey);
  return calibTiming->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);

}
