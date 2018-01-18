///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IsolationBuilder.cxx 
// Implementation file for class IsolationBuilder
/////////////////////////////////////////////////////////////////// 

// Isolation includes
#include "IsolationBuilder.h"



IsolationBuilder::IsolationBuilder( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

IsolationBuilder::~IsolationBuilder() {}

StatusCode IsolationBuilder::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  std::set<xAOD::Iso::IsolationFlavour> runIsoType;
  
  ATH_MSG_DEBUG("Initializing central electrons");
  ATH_CHECK(initializeIso<xAOD::ElectronContainer>(runIsoType, &m_elCaloIso, &m_elTrackIso, 
						   m_ElectronContainerName,
						   m_elisoInts, m_elcorInts,
						   m_customConfigEl, false));

  ATH_MSG_DEBUG("Initializing central photons");
  ATH_CHECK(initializeIso<xAOD::PhotonContainer>(runIsoType, &m_phCaloIso, &m_phTrackIso, 
						 m_PhotonContainerName,
						 m_phisoInts, m_phcorInts,
						 m_customConfigPh, false));

  ATH_MSG_DEBUG("Initializing forward electrons");
  ATH_CHECK(initializeIso<xAOD::ElectronContainer>(runIsoType, &m_feCaloIso, nullptr, 
						   m_FwdElectronContainerName,
						   m_feisoInts, m_fecorInts,
						   m_customConfigFwd, false));

  ATH_MSG_DEBUG("Initializing muons");
  ATH_CHECK(initializeIso<xAOD::MuonContainer>(runIsoType, &m_muCaloIso, &m_muTrackIso, 
					       m_MuonContainerName,
					       m_muisoInts, m_mucorInts,
					       m_customConfigMu, m_addCoreCorr));
			  

  // Retrieve the tools (there three Calo ones are the same in fact)
  if (!m_cellIsolationTool.empty() && runIsoType.find(xAOD::Iso::etcone) != runIsoType.end()) {
    ATH_CHECK(m_cellIsolationTool.retrieve());
  } else {
    m_cellIsolationTool.disable();
  }

  if (!m_topoIsolationTool.empty() && runIsoType.find(xAOD::Iso::topoetcone) != runIsoType.end()) {
    ATH_CHECK(m_topoIsolationTool.retrieve());
  } else {
    m_topoIsolationTool.disable();
  }

  if (!m_pflowIsolationTool.empty() && runIsoType.find(xAOD::Iso::neflowisol) != runIsoType.end()) {
    ATH_CHECK(m_pflowIsolationTool.retrieve());
  } else {
    m_pflowIsolationTool.disable();
  }

  if (!m_trackIsolationTool.empty() && runIsoType.find(xAOD::Iso::IsolationFlavour::ptcone) != runIsoType.end()) {
    ATH_CHECK(m_trackIsolationTool.retrieve());
  } else {
    m_trackIsolationTool.disable();
  }
  
  //initialise data handles
  ATH_CHECK(m_cellsKey.initialize(!m_cellIsolationTool.empty()));
	    
  // // Also can apply leakage correction if AODFix :
  // // be carefull ! There is a leakage in topoetcone, etcone, ... : do not run this !!!!!! (useless)
  // if (m_isAODFix && !m_leakTool.empty()) {
  //   ATH_MSG_INFO("Will run leakage corrections for photons and electrons");
  //   ATH_CHECK(m_leakTool.retrieve());
  // }


  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // For etcone, needs the cells
  if (!m_cellIsolationTool.empty()) {
    SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey);
    
    // check is only used for serial running; remove when MT scheduler used
    if(!cellcoll.isValid()) {
      ATH_MSG_ERROR("Failed to retrieve cell container: "<< m_cellsKey.key());
      return StatusCode::FAILURE;
    }
    
    m_cellColl = cellcoll.cptr();
  } else {
    m_cellColl = nullptr;
  }

  // If AODFix, first deep copy  -- 
  // JM: Let's leave this to later. From what I understand we will rename inputs
  //     and create new objects, but I don't know if it makes sense to do it here
  //     like this. Maybe there can be an external utility to copy renamed xAOD objects?
  // if (m_isAODFix) {
  //   if (m_ElectronContainerName.size()) {
  //     if (!evtStore()->tryRetrieve<xAOD::ElectronContainer>(m_ElectronContainerName)) {
  // 	if( deepCopy<xAOD::ElectronContainer,xAOD::ElectronAuxContainer>(m_ElectronContainerName).isFailure()) {
  // 	  ATH_MSG_FATAL( "Couldn't deep copy electrons" );
  // 	  return StatusCode::FAILURE;
  // 	}
  //     }
  //   }
  //   if (m_FwdElectronContainerName.size()) {
  //     if (!evtStore()->tryRetrieve<xAOD::ElectronContainer>(m_FwdElectronContainerName)) {
  // 	if( deepCopy<xAOD::ElectronContainer,xAOD::ElectronAuxContainer>(m_FwdElectronContainerName).isFailure()) {
  // 	  ATH_MSG_FATAL( "Couldn't deep copy forward electrons" );
  // 	  return StatusCode::FAILURE;
  // 	}
  //     }
  //   }
  //   if (m_PhotonContainerName.size()) {
  //     if (!evtStore()->tryRetrieve<xAOD::PhotonContainer>(m_PhotonContainerName)) {
  // 	if( deepCopy<xAOD::PhotonContainer,xAOD::PhotonAuxContainer>(m_PhotonContainerName).isFailure()) {
  // 	  ATH_MSG_FATAL( "Couldn't deep copy photons" );
  // 	  return StatusCode::FAILURE;
  // 	}
  //     }
  //   }
  //   SG::ReadHandle <xAOD::MuonContainer>h_muon(m_MuonContainerName);
  //   if (h_muon.isValid()) {
  //     if( deepCopy<xAOD::MuonContainer,xAOD::MuonAuxContainer>(m_MuonContainerName.key()).isFailure()) {
  // 	ATH_MSG_FATAL( "Couldn't deep copy muons" );
  // 	return StatusCode::FAILURE;
  //     }
  //   }
  // }

  // Compute isolations

  ATH_CHECK(executeCaloIso(m_elCaloIso));
  ATH_CHECK(executeCaloIso(m_phCaloIso));
  ATH_CHECK(executeCaloIso(m_feCaloIso));
  ATH_CHECK(executeCaloIso(m_muCaloIso));

  ATH_CHECK(executeTrackIso(m_elTrackIso));
  ATH_CHECK(executeTrackIso(m_phTrackIso));
  ATH_CHECK(executeTrackIso(m_muTrackIso));

  // if (m_isAODFix && !m_leakTool.empty())
  //   CHECK(runLeakage());

  
  return StatusCode::SUCCESS;
}
 
// JM:  of AODFix, so commenting out for now

// StatusCode IsolationBuilder::runLeakage() {
  
//   // Retrieve data
//   if (m_PhotonContainerName.size()) {
//     xAOD::PhotonContainer* photons = evtStore()->retrieve< xAOD::PhotonContainer >(m_PhotonContainerName);
//     if( !photons ) {
//       ATH_MSG_ERROR("Couldn't retrieve photon container with key: " << m_PhotonContainerName);
//       return StatusCode::FAILURE;
//     }
//     for (auto ph : *photons)           
//       m_leakTool->applyCorrection(*ph);
//   }
    
//   if (m_ElectronContainerName.size()) {
//     xAOD::ElectronContainer* electrons = evtStore()->retrieve< xAOD::ElectronContainer >(m_ElectronContainerName);
//     if( !electrons ) {
//       ATH_MSG_ERROR("Couldn't retrieve electron container with key: " << m_ElectronContainerName);
//       return StatusCode::FAILURE;
//     }
//     for (auto el : *electrons)
//       m_leakTool->applyCorrection(*el);
//   }

//   return StatusCode::SUCCESS;
// }
 

// template< class CONTAINER, class AUXSTORE > StatusCode IsolationBuilder::deepCopy( const std::string& key ) const {
    
//   // Let the user know what's happening:
//   ATH_MSG_VERBOSE( "Running deepCopy on container: " << key );
  
//   // Decide which implementation to call:
//   if( evtStore()->template contains< AUXSTORE >( key + "Aux." ) ) {
//     if( deepCopyImp< CONTAINER, AUXSTORE >( key ).isFailure() ) {
//       ATH_MSG_FATAL( "Couldn't call deepCopyImp with concrete "
// 		     "auxiliary store" );
//       return StatusCode::FAILURE;
//     }
//   } else if( evtStore()->template contains< xAOD::AuxContainerBase >( key +
// 								      "Aux." ) ) {
//     if( deepCopyImp< CONTAINER,
// 	xAOD::AuxContainerBase >( key ).isFailure() ) {
//       ATH_MSG_FATAL( "Couldn't call deepCopyImp with generic "
// 		     "auxiliary store" );
//       return StatusCode::FAILURE;
//     }
//   } else {
//     ATH_MSG_FATAL( "Couldn't discover auxiliary store type for container \""
// 		   << key << "\"" );
//     return StatusCode::FAILURE;
//   }
  
//   // Return gracefully:
//   return StatusCode::SUCCESS;
// }
  
// template< class CONTAINER, class AUXSTORE > StatusCode IsolationBuilder::deepCopyImp( const std::string& key ) const {
  
//   // Retrieve the const container:
//   ATH_MSG_VERBOSE( "Will retrieve " << key);
//   const CONTAINER* c = 0;
//   ATH_CHECK( evtStore()->retrieve( c, key ) );
  
//   // Create the new container:
//   ATH_MSG_VERBOSE( "Will create new containers" );
//   CONTAINER* copy = new CONTAINER();
//   AUXSTORE* copyAux = new AUXSTORE();
//   copy->setStore( copyAux );
  
//   // Do the deep copy:
//   ATH_MSG_VERBOSE( "Will copy the object" );
//   for( auto oldObj : *c ) {
//     ATH_MSG_VERBOSE( "Now working on object " << oldObj);
//     auto newObj = new typename CONTAINER::base_value_type();
//     copy->push_back( newObj );
//     *newObj = *oldObj;
//   }
  
//   // Do the overwrite:
//   ATH_MSG_VERBOSE( "Will overwrite the container" );
//   ATH_CHECK( evtStore()->overwrite( copy, key, true, false ) );
//   ATH_MSG_VERBOSE( "Will overwrite the aux container" );
//   ATH_CHECK( evtStore()->overwrite( copyAux, key + "Aux.", true, false ) );
//   ATH_MSG_VERBOSE( "Done" );
  
//   // Return gracefully:
//   return StatusCode::SUCCESS;
// }
