/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// other packages
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/MdtIdHelper.h"

// this package
#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"

MdtCalibrationRegionSvc::MdtCalibrationRegionSvc(const std::string& name,ISvcLocator* sl)
  : AthService(name,sl),
    m_mdtIdHelper(0)
{

}

MdtCalibrationRegionSvc::~MdtCalibrationRegionSvc() {
}

// queryInterface 
StatusCode MdtCalibrationRegionSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if( interfaceID().versionMatch(riid) ) 
    {
      *ppvIF = dynamic_cast<MdtCalibrationRegionSvc*>(this);
    } else { 
    return AthService::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
} 


StatusCode MdtCalibrationRegionSvc::initialize()
{ 
  if( AthService::finalize().isFailure() ) return StatusCode::FAILURE;

  // initialize the MdtIdHelper
  ServiceHandle<StoreGateSvc> detStore("StoreGateSvc/DetectorStore", name());
  if (detStore.retrieve().isFailure())   {
    ATH_MSG_ERROR( "Can't locate the DetectorStore" ); 
    return StatusCode::FAILURE;
  }

  StatusCode sc = detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if ( sc.isFailure()) {
    ATH_MSG_ERROR( "Can't retrieve MdtIdHelper" );
    return sc;
  }
  
  initializeRegions();
  
  return sc;
}

StatusCode MdtCalibrationRegionSvc::finalize() { 
  return AthService::finalize();
}

void MdtCalibrationRegionSvc::initializeRegions()
{

  remapRtRegions("OneRt");

  MdtRegionSet corset;
  MdtRegionHash rhash_cor1(0);
  MdtRegion region_cor1(rhash_cor1);
  MdtRegionHash rhash_cor2(1);
  MdtRegion region_cor2(rhash_cor2);
  MdtRegionHash rhash_cor3(2);
  MdtRegion region_cor3(rhash_cor3);

  MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
  MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
  for( ; it!=it_end;++it ){
    if( m_mdtIdHelper->isBarrel(*it) ) region_cor1.addIdentifier( *it );
    else if( m_mdtIdHelper->isEndcap(*it) ) region_cor2.addIdentifier( *it );
    else region_cor3.addIdentifier( *it );
  }
  corset.push_back( region_cor1 );
  corset.push_back( region_cor2 );
  corset.push_back( region_cor3 );

  m_corRegionTool.initialize( corset, m_mdtIdHelper );
  
}

void MdtCalibrationRegionSvc::remapRtRegions(std::string mapName){
  MdtRegionSet rtset;

  if (mapName=="OnePerChamber"){

    MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
    MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
    int i=0;
    for( ; it!=it_end;++it ){
      // create one regione per module 
      MdtRegionHash rhash_rt(i++);
      MdtRegion region_rt(rhash_rt);
      region_rt.addIdentifier( *it );
      rtset.push_back( region_rt );
    }

  } else if (mapName=="OneRt") {

    MdtRegionHash rhash_rt(0);
    MdtRegion region_rt(rhash_rt);
    MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
    MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
    for( ; it!=it_end;++it ){
      // create one big region
      region_rt.addIdentifier( *it );
    }
    rtset.push_back( region_rt );

  } else {

    ATH_MSG_ERROR("Rt Regions Map "<<mapName <<" unknown. Keeping previous map ");
  }

  m_rtRegionTool.initialize( rtset, m_mdtIdHelper );
}

