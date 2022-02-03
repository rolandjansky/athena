/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// /***************************************************************************
//                           CPMSim.cxx  -  description
//                              -------------------
//     begin                : Wed Mar 05 2014
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// CPMSim class Implementation
// ================================================
//
//
//

// Utilities

// This algorithm includes
#include "CPMSim.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "TrigT1CaloEvent/CPMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CPMCMXData_ClassDEF.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"



namespace LVL1{

using namespace TrigConf;

//--------------------------------
// Constructors and destructors
//--------------------------------

CPMSim::CPMSim
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthReentrantAlgorithm( name, pSvcLocator ), 
      m_CPMTool("LVL1::L1CPMTools/L1CPMTools")
{
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode CPMSim::initialize()
{
  ATH_CHECK( m_CPMTool.retrieve() );
  ATH_CHECK( m_CPMTowerLocation.initialize() );
  ATH_CHECK( m_CPMCMXDataLocation.initialize() );
  ATH_CHECK( m_CPMTobRoILocation.initialize() );
  ATH_CHECK( m_L1MenuKey.initialize() );

  return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode CPMSim::execute(const EventContext& ctx) const
{
    
  /* Retrieve collection of CPMTowers
     Form TOBs from them
     Form backplane data from TOBs
     Store TOBs for BS simulation
     Form and store RoI SLink data for RoIB
  */

  ATH_MSG_DEBUG ( "starting CPMSim" ); 

  // Create containers for this event
  auto CMXData    = std::make_unique<DataVector<CPMCMXData> >();  //Container of backplane data objects 
  auto allTOBs    = std::make_unique<DataVector<CPMTobRoI> >();  // Container to hold all TOB RoIs in event

  SG::ReadHandle<xAOD::CPMTowerContainer> storedCPMTs
    (m_CPMTowerLocation, ctx);

  if (storedCPMTs->size() == 0)
    ATH_MSG_WARNING("Empty CPMTowerContainer - looks like a problem" );
	
  // Map the CPMTs
  xAOD::CPMTowerMap_t towerMap;
  m_CPMTool->mapTowers(storedCPMTs.cptr(), &towerMap);

  // get the L1Menu for the CPMTool
  auto l1Menu = SG::makeHandle( m_L1MenuKey, ctx );
  const TrigConf::L1Menu* l1Menu_ptr=l1Menu.cptr();

  // Loop over crates and modules
  for (int iCrate = 0; iCrate < 4; ++iCrate) {
    for (int iModule = 1; iModule <= 14; ++iModule) {
	    
      // For each module, find TOBs and backplane data
      std::vector<unsigned int> emCMXData;
      std::vector<unsigned int> tauCMXData;
      m_CPMTool->findCPMResults(l1Menu_ptr,&towerMap,iCrate,iModule,allTOBs.get(),emCMXData,tauCMXData);
      // Push backplane data into output DataVectors
      CMXData -> push_back(std::make_unique<CPMCMXData>(iCrate,iModule,0,emCMXData));
      CMXData -> push_back(std::make_unique<CPMCMXData>(iCrate,iModule,1,tauCMXData));

    } // loop over modules
  } // loop over crates
      
     
  // Store RoIB output, module readout and backplane results in the TES
  ATH_CHECK( SG::makeHandle(m_CPMCMXDataLocation, ctx).record (std::move (CMXData)) );
  ATH_CHECK( SG::makeHandle(m_CPMTobRoILocation, ctx).record (std::move (allTOBs)) );
  
  return StatusCode::SUCCESS ;
}


} // end of LVL1 namespace bracket


