/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolPulseShape("TileCondToolPulseShape", 1, 0);

const InterfaceID& TileCondToolPulseShape::interfaceID() {
  return IID_TileCondToolPulseShape;
}

//
//____________________________________________________________________
TileCondToolPulseShape::TileCondToolPulseShape(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
{
  declareInterface<TileCondToolPulseShape>(this);
}

//
//____________________________________________________________________
TileCondToolPulseShape::~TileCondToolPulseShape() {
}

//
//____________________________________________________________________
StatusCode TileCondToolPulseShape::initialize() {
  ATH_MSG_DEBUG( "In initialize()" );

  //=== Initialize conditions data key with pulse shape
  ATH_CHECK( m_calibPulseShapeKey.initialize() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolPulseShape::finalize() {
  ATH_MSG_DEBUG( "finalize called" );
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
bool TileCondToolPulseShape::getPulseShapeYDY(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                              float time, float &y, float &dy,
                                              const EventContext &ctx) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibPulseShape(m_calibPulseShapeKey, ctx);
  return calibPulseShape->getCalibDrawer(drawerIdx)->getYDY(channel, adc, time, y, dy);
}

//
//____________________________________________________________________

