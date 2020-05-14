/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// other packages
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/MdtIdHelper.h"

// this package
#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"

MdtCalibrationRegionSvc::MdtCalibrationRegionSvc(const std::string &name,ISvcLocator *sl)
  : AthService(name,sl), m_mdtIdHelper(0), m_regionType(ONERT), m_numberOfRegions(1) {
}

// queryInterface 
StatusCode MdtCalibrationRegionSvc::queryInterface(const InterfaceID &riid, void **ppvIF) { 
  if( interfaceID().versionMatch(riid) ) {
    *ppvIF = dynamic_cast<MdtCalibrationRegionSvc*>(this);
  } else { 
    return AthService::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
} 

StatusCode MdtCalibrationRegionSvc::initialize() { 
  ATH_CHECK(AthService::initialize());

  // initialize the MdtIdHelper
  ServiceHandle<StoreGateSvc> detStore("StoreGateSvc/DetectorStore", name());
  ATH_CHECK(detStore.retrieve());

  ATH_CHECK(detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER"));
  
  // Initialize RT and correction function regions
  remapRtRegions("OneRt");
  
  return StatusCode::SUCCESS;
}  //end MdtCalibrationRegionSvc::initialize

void MdtCalibrationRegionSvc::remapRtRegions(std::string mapName) {
  m_regionHash.resize(0);
    
// One RT for all chambers
  if (mapName=="OneRt") {
    m_regionType = ONERT;
    m_numberOfRegions = 1;
    MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
    MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
    for( int i=0; it!=it_end; ++it, i++ ) {
      m_regionHash.push_back(0);    //all chambers use RT 0 
    }

// One RT per chamber
// RT/correction functions are stored using chamber hash as index. 
// Chamber hash is available from
//      m_mdtIdHelper->get_module_hash( *it, hash );
// or   m_mdtIdHelper->get_hash( *it, hash, &idCont );
  } else if (mapName=="OnePerChamber") {
    m_regionType = ONEPERCHAMBER;
    m_numberOfRegions = m_mdtIdHelper->module_hash_max();
    MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
    MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
    for( int i=0; it!=it_end; ++it, i++ ) {
      m_regionHash.push_back(i);   //Ok, silly since rt region = chamber hash
    }

// One RT per multilayer
// This is starts out the same as ONEPERCHAMBER except that m_regionHash is used
// to store the index for m_rtData for each ML.  m_regionHash is initialized here
// for ML2 to use the ML1 RT.   In MdtCalibDBStrTool if an ML2 RT is read from COOL 
// then it is added to m_rtData and the hash (index) to for the ML2 is stored in m_regionHash. 
  } else if (mapName=="OnePerMultilayer") {
    m_regionType = ONEPERMULTILAYER;
    m_numberOfRegions = m_mdtIdHelper->module_hash_max();  //initial value; may increase of ML2 RTs are read from COOL
    //initialize m_regionHash so ML2 uses ML1 hash (i.e. ML2 uses ML1 RT function)
    //If ML2 RT is read from COOL this is overwritten in MdtCalibDBStrTool
    //The index, i, of the loop below is effectively a loop over (chamber) hashes.
    //m_regionHash is filled in the order of ML hashes
    MdtIdHelper::const_id_iterator it     = m_mdtIdHelper->module_begin();
    MdtIdHelper::const_id_iterator it_end = m_mdtIdHelper->module_end();
    for( int i=0; it!=it_end; ++it, i++ ) {
      m_regionHash.push_back(i);  
      if( m_mdtIdHelper->numberOfMultilayers(*it) == 2 ) {
	m_regionHash.push_back(i);
      }
    }

  } else {
    ATH_MSG_ERROR("Rt Regions Map "<<mapName <<" unknown. Keeping previous map ");
  }

}  //end MdtCalibrationRegionSvc::remapRtRegions
