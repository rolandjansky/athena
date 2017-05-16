/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileConditions/TileCondToolTiming.h"
#include "TileCalibBlobObjs/Exception.h"
#include "TileConditions/TileCondProxyWrapper.h"

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
  , m_pryAdcOffset("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_AdcOffset", this)
{
  declareInterface<TileCondToolTiming>(this);

  declareProperty( "ProxyAdcOffset", m_pryAdcOffset );

}

//
//____________________________________________________________________
TileCondToolTiming::~TileCondToolTiming()
{
}

//
//____________________________________________________________________
StatusCode 
TileCondToolTiming::initialize()
{

  ATH_MSG_DEBUG( "In initialize()" );


  //=== always retrieve channel offset proxies
  CHECK( m_pryAdcOffset.retrieve() );


  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolTiming::finalize()
{
  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}


//
//____________________________________________________________________
float TileCondToolTiming::getChannelOffset(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const
{
  if(drawerIdx>=TileCalibUtils::MAX_DRAWERIDX){
    throw TileCalib::IndexOutOfRange("TileCondToolTiming::getChannelOffset", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }
  return m_pryAdcOffset->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
}

