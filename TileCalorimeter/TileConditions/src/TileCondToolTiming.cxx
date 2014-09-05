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

  //=== resize cache
  unsigned int maxAdcIdx = TileCalibUtils::getAdcIdx(TileCalibUtils::MAX_DRAWERIDX - 1,
						     TileCalibUtils::MAX_CHAN      - 1,
						     TileCalibUtils::MAX_GAIN      - 1);
  m_cache.resize( maxAdcIdx + 1 );
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

  //=== recache function needs to be bound to IOV updates only for COOL proxies
  const TileCondProxyCoolFlt* ptrPryAdcOffset = dynamic_cast<const TileCondProxyCoolFlt*>( &(*m_pryAdcOffset) );

  //=== call recache if all proxies are FILE proxies
  if(!ptrPryAdcOffset) {
    int idx = 0;
    std::list<std::string> keys;
    keys.push_back("ASCII files");
    CHECK( recache(idx, keys) );
  }  else {
  //=== if at least one COOL proxy is loaded, then the linked callback will take care of recaching
    CHECK( detStore()->regFcn(&TileCondProxyCoolFlt::callback, ptrPryAdcOffset, &TileCondToolTiming::recache, this, true) );
  }
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
StatusCode TileCondToolTiming::recache( IOVSVC_CALLBACK_ARGS_K(keys) ) 
{

  msg(MSG::INFO) << "TileCondToolTiming::recache() has been triggered by: ";
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    msg(MSG::INFO) << "\'" << *itr << "' ";
  }
  msg(MSG::INFO) << endmsg;

  //=== recache total time correction for all adcs
  for(unsigned int ros=0; ros<TileCalibUtils::MAX_ROS; ++ros){
    for(unsigned int mod=0; mod<TileCalibUtils::getMaxDrawer(ros); ++mod){
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros,mod);
      for(unsigned int chn=0; chn<TileCalibUtils::MAX_CHAN; ++chn){
	for(unsigned int adc=0; adc<TileCalibUtils::MAX_GAIN; ++adc){

	  float time = getChannelOffset(drawerIdx, chn, adc);
	  m_cache[TileCalibUtils::getAdcIdx(drawerIdx, chn, adc)] = time;

	}//end adc
      }//end chn
    }//end mod
  }//end ros

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

