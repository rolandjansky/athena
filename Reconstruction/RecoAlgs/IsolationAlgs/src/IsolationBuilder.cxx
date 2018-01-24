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
  ATH_CHECK(initializeIso(runIsoType, &m_elCaloIso, &m_elTrackIso, 
			  m_ElectronContainerName,
			  m_elisoInts, m_elcorInts,
			  m_customConfigEl, false));
  
  ATH_MSG_DEBUG("Initializing central photons");
  ATH_CHECK(initializeIso(runIsoType, &m_phCaloIso, &m_phTrackIso, 
			  m_PhotonContainerName,
			  m_phisoInts, m_phcorInts,
			  m_customConfigPh, false));
  
  ATH_MSG_DEBUG("Initializing forward electrons");
  ATH_CHECK(initializeIso(runIsoType, &m_feCaloIso, nullptr, 
			  m_FwdElectronContainerName,
			  m_feisoInts, m_fecorInts,
			  m_customConfigFwd, false));

  ATH_MSG_DEBUG("Initializing muons");
  ATH_CHECK(initializeIso(runIsoType, &m_muCaloIso, &m_muTrackIso, 
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

IsolationBuilder::CaloIsoHelpHandles::CaloIsoHelpHandles(const IsolationBuilder::CaloIsoHelpKey& keys)
{
  for (const auto& key : keys.isoDeco) {
    isoDeco.emplace_back(key);
  }
}

IsolationBuilder::TrackIsoHelpHandles::TrackIsoHelpHandles(const IsolationBuilder::TrackIsoHelpKey& keys)
{
  for (const auto& key : keys.isoDeco) {
    isoDeco.emplace_back(key);
  }
  for (const auto& key : keys.isoDecoV) {
    isoDecoV.emplace_back(key);
  }
}

StatusCode IsolationBuilder::initializeIso(std::set<xAOD::Iso::IsolationFlavour>& runIsoType, // out
					   std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey> >*  caloIsoMap, // out
					   std::vector<std::pair<xAOD::Iso::IsolationFlavour,TrackIsoHelpKey> >* trackIsoMap, // out
					   const std::string& containerName,
					   const std::vector<std::vector<int> >& isoInts,
					   const std::vector<std::vector<int> >& corInts,
					   const std::string& customConfig,
					   bool addCoreCorr)
{
  
  std::string prefix = containerName + ".";
  
  for (size_t flavor = 0; flavor < isoInts.size(); flavor++) {
    // iterate over the flavor (cell, topo, eflow, track
    //   Note: it is a configuration error if different types
    //         are included in one inner vector

    CaloIsoHelpKey cisoH;
    TrackIsoHelpKey tisoH;
    
    //std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav =
      xAOD::Iso::numIsolationFlavours;
    xAOD::Iso::IsolationFlavour oldIsoFlav =
      xAOD::Iso::numIsolationFlavours;
    for (size_t type = 0; type < isoInts[flavor].size();type++) { // the cone size
      xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(isoInts[flavor][type]);
      isoFlav = xAOD::Iso::isolationFlavour(isoType);
      if (oldIsoFlav != xAOD::Iso::numIsolationFlavours && oldIsoFlav != isoFlav) {
	ATH_MSG_FATAL("Configuration error:  can only have one type of isolation in inner vector");
	return StatusCode::FAILURE;
      }
      oldIsoFlav = isoFlav;
      std::string isoName = prefix + xAOD::Iso::toString(isoType);
      if (customConfig != "") {
	isoName += "_" + customConfig;
      }
      if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
	cisoH.isoTypes.push_back(isoType);
	cisoH.isoDeco.emplace_back(isoName);
	ATH_CHECK(cisoH.isoDeco.back().initialize());
      } else if (isoFlav == xAOD::Iso::ptcone) {
	tisoH.isoTypes.push_back(isoType);
	tisoH.isoDeco.emplace_back(isoName);
	ATH_CHECK(tisoH.isoDeco.back().initialize());
	std::string isoNameV = prefix + "ptvarcone" + std::to_string(100*xAOD::Iso::coneSize(isoType));
	if (customConfig != "") {
	  isoNameV += "_" + customConfig;
	}
	tisoH.isoDecoV.emplace_back(isoNameV);
	ATH_CHECK(tisoH.isoDeco.back().initialize());
      } else {
	ATH_MSG_FATAL("Configuration error: Isolation flavor " << isoFlav << " not supported.");
	return StatusCode::FAILURE;
      }	
    }
    if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
      
      for (size_t corrType = 0; corrType < corInts[flavor].size(); corrType++) {
	const auto cor = static_cast<unsigned int>(corInts[flavor][corrType]);
	cisoH.CorrList.calobitset.set(cor);
	const xAOD::Iso::IsolationCaloCorrection isoCor = static_cast<xAOD::Iso::IsolationCaloCorrection>(cor);
	if (addCoreCorr && (isoCor == xAOD::Iso::coreCone || isoCor == xAOD::Iso::coreMuon) ) {
	  std::string isoCorName = prefix;
	  if (isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
	    isoCorName += xAOD::Iso::toString(isoFlav);
	  }
	  isoCorName += xAOD::Iso::toString(isoCor) + xAOD::Iso::toString(xAOD::Iso::coreEnergy)
	    + "Correction"; // hard coded since we never store the core area in fact
	  if (customConfig != "") {
	    isoCorName += "_" + customConfig;
	  }
	  cisoH.addCoreCorr = true;
	  cisoH.coreCorisoDeco = isoCorName;
	  ATH_CHECK(cisoH.coreCorisoDeco.initialize());
	}
      }
      if (caloIsoMap) {
	caloIsoMap->push_back(std::make_pair(isoFlav,cisoH));
      } else {
	ATH_MSG_FATAL("caloIsoMap was nullptr but the configuration attempted to use it");
	return StatusCode::FAILURE;
      }
    } else if (isoFlav == xAOD::Iso::ptcone) {
      tisoH.CorrList.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
      if (trackIsoMap) {
	trackIsoMap->push_back(std::make_pair(isoFlav,tisoH));
      } else {
	ATH_MSG_FATAL("trackIsoMap was nullptr but the configuration attempted to use it");
	return StatusCode::FAILURE;
      }	
    } else 
      ATH_MSG_WARNING("Isolation flavour " << xAOD::Iso::toCString(isoFlav) << " does not exist ! Check your inputs");
    runIsoType.insert(isoFlav);
  }
  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::executeCaloIso(const std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey> >& caloIsoMap)
{
  for (const auto& pr : caloIsoMap) {
    const xAOD::Iso::IsolationFlavour flav = pr.first;
    const auto& keys =  pr.second;
    CaloIsoHelpHandles handles(keys);

    if (handles.isoDeco.empty()) {
      ATH_MSG_FATAL("Have a CaloIsoHelpHandles with no actual isolations; something wrong happened");
      return StatusCode::FAILURE;
    }
    auto& readHandle = handles.isoDeco[0]; // can treat the writeDecorHandle as a read handle;
    if (!readHandle.isValid()) {
      ATH_MSG_FATAL("Could not retrieve read handle for " << keys.isoDeco[0].key());
      return StatusCode::FAILURE;
    }

    for (const auto& part : *readHandle) {
      xAOD::CaloIsolation CaloIsoResult;
      bool successfulCalc = false;
      if (flav == xAOD::Iso::IsolationFlavour::etcone && m_cellColl) {
	successfulCalc = m_cellIsolationTool->caloCellIsolation(CaloIsoResult, *part, keys.isoTypes, keys.CorrList, m_cellColl);
      } else if (flav == xAOD::Iso::IsolationFlavour::topoetcone) {
	successfulCalc = m_topoIsolationTool->caloTopoClusterIsolation(CaloIsoResult, *part, keys.isoTypes, keys.CorrList);
      } else if (flav == xAOD::Iso::IsolationFlavour::neflowisol) {
	successfulCalc = m_pflowIsolationTool->neutralEflowIsolation(CaloIsoResult, *part, keys.isoTypes, keys.CorrList);
      }

      if (successfulCalc) {
	for (unsigned int i = 0; i < keys.isoTypes.size(); i++) {
	  float iso = CaloIsoResult.etcones[i];
	  ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toCString(keys.isoTypes[i]) << " = " << iso/1e3);
	  (handles.isoDeco[i])(*part) = iso;
	}
	// not that nice. I expect a single core correction (e.g. for topoetcone, not coreMuon and coreCone together...)
	if (keys.addCoreCorr) {
	  SG::WriteDecorHandle<xAOD::IParticleContainer, float> coreCorisoDeco(keys.coreCorisoDeco);
	  xAOD::Iso::IsolationCaloCorrection icc = xAOD::Iso::coreCone;
	  bool ison = false;
	  if (CaloIsoResult.corrlist.calobitset.test(static_cast<unsigned int>(xAOD::Iso::coreMuon))) {
	    icc  = xAOD::Iso::coreMuon;
	    ison = true;
	  } else if (CaloIsoResult.corrlist.calobitset.test(static_cast<unsigned int>(xAOD::Iso::coreCone))) {
	    ison = true;
	  }
	  if (ison) {
	    if (CaloIsoResult.coreCorrections.find(icc) != CaloIsoResult.coreCorrections.end()) {
	      if (CaloIsoResult.coreCorrections[icc].find(xAOD::Iso::coreEnergy) != CaloIsoResult.coreCorrections[icc].end()) {
		coreCorisoDeco(*part) = CaloIsoResult.coreCorrections[icc][xAOD::Iso::coreEnergy];
	      } else {
		ATH_MSG_WARNING("Cannot find the core energy correction for custom flavour " << xAOD::Iso::toCString(flav));
	      }
	    } else {
	      ATH_MSG_WARNING("Cannot find the core correction for custom flavour " << xAOD::Iso::toCString(flav));
	    }
	  }
	}
      } else {
	ATH_MSG_ERROR("Call to CaloIsolationTool failed for custom flavour " << xAOD::Iso::toCString(flav));
	return StatusCode::RECOVERABLE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::executeTrackIso(const std::vector<std::pair<xAOD::Iso::IsolationFlavour,TrackIsoHelpKey > >& trackIsoMap)
{
  for (const auto& pr : trackIsoMap) {
    const xAOD::Iso::IsolationFlavour flav = pr.first;
    const auto& keys =  pr.second;
    TrackIsoHelpHandles handles(keys);

    if (handles.isoDeco.empty()) {
      ATH_MSG_FATAL("Have a TrackIsoHelpHandles with no actual isolations; something wrong happened");
      return StatusCode::FAILURE;
    }
    auto& readHandle = handles.isoDeco[0]; // can treat the writeDecorHandle as a read handle;
    if (!readHandle.isValid()) {
      ATH_MSG_FATAL("Could not retrieve read handle for " << keys.isoDeco[0].key());
      return StatusCode::FAILURE;
    }

    for (const auto& part : *readHandle) {
      xAOD::TrackIsolation TrackIsoResult;
      bool successfulCalc = m_trackIsolationTool->trackIsolation(TrackIsoResult, *part, keys.isoTypes, keys.CorrList);

      if (successfulCalc) {
	for (unsigned int i = 0; i < keys.isoTypes.size(); i++) {
	  float iso = TrackIsoResult.ptcones[i];
	  float isoV = TrackIsoResult.ptvarcones_10GeVDivPt[i];
	  ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toCString(keys.isoTypes[i]) << " = " << iso/1e3 << ", var cone = " << isoV/1e3);
	  (handles.isoDeco[i])(*part) = iso;
	  (handles.isoDecoV[i])(*part) = isoV;
	}
      } else {
	ATH_MSG_ERROR("Call to TrackIsolationTool failed for custom flavour " << xAOD::Iso::toCString(flav));
	return StatusCode::RECOVERABLE;
      }
    }
  }
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
