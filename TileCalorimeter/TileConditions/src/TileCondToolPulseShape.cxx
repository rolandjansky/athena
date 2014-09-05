/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileCalibBlobObjs/Exception.h"
#include "TileConditions/TileCondProxyWrapper.h"

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
    , m_pryPulseShape("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_PulseShape", this)
{
  declareInterface<TileCondToolPulseShape>(this);
  declareProperty("ProxyPulseShape", m_pryPulseShape);
}

//
//____________________________________________________________________
TileCondToolPulseShape::~TileCondToolPulseShape() {
}

//
//____________________________________________________________________
StatusCode TileCondToolPulseShape::initialize() {
  ATH_MSG_DEBUG( "In initialize()" );

  //=== Retrieve offline proxies
  CHECK( m_pryPulseShape.retrieve() );

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
bool TileCondToolPulseShape::getPulseShapeYDY(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                              , float time, float &y, float &dy) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolPulseShape::getPulseShapeYDY", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryPulseShape->getCalibDrawer(drawerIdx)->getYDY(channel, adc, time, y, dy);
}

//
//____________________________________________________________________

