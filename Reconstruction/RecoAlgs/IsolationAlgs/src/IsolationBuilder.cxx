///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IsolationBuilder.cxx 
// Implementation file for class IsolationBuilder
/////////////////////////////////////////////////////////////////// 

// Isolation includes
#include "IsolationBuilder.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "StoreGate/ReadHandle.h"

#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/Muon.h"
#include "CaloEvent/CaloCellContainer.h"

#include "boost/foreach.hpp"
#include "boost/format.hpp"

#include <set>
#include <utility>

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
  if (m_egCaloIso.size() != 0 || m_egTrackIso.size() != 0) {
    if (m_isolateEl && m_ElectronContainerName.size()) {
      if (m_customConfigEl == "")
	CHECK(IsolateEgamma("electron"));
      else
	CHECK(DecorateEgamma("electron"));
    }
    if (m_isolatePh && m_PhotonContainerName.size()) {
      if (m_customConfigPh == "")
	CHECK(IsolateEgamma("photon"));
      else
	CHECK(DecorateEgamma("photon"));
    }
  }
  if (m_feCaloIso.size() != 0 && m_FwdElectronContainerName.size()) {
    if (m_customConfigFwd == "")
      CHECK(IsolateEgamma("fwdelectron"));
    else
      CHECK(DecorateEgamma("fwdelectron"));
  }
  if ( (m_muCaloIso.size() != 0 || m_muTrackIso.size() != 0) ) {
    if (m_customConfigMu == "")
      CHECK(IsolateMuon());
    else
      CHECK(DecorateMuon());
  }

  // if (m_isAODFix && !m_leakTool.empty())
  //   CHECK(runLeakage());

  
  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::IsolateEgamma(std::string egType) {

  const xAOD::EgammaContainer *egC(0);
  if (egType == "electron") {
    if (evtStore()->contains<xAOD::ElectronContainer>(m_ElectronContainerName)) {
      if (evtStore()->retrieve(egC,m_ElectronContainerName).isFailure()) {
	ATH_MSG_FATAL("Cannot retrieve electron container " << m_ElectronContainerName);
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("ElectronContainer " << m_ElectronContainerName << " not available");
      return StatusCode::SUCCESS;
    }
  } else if (egType == "photon") {
    if (evtStore()->contains<xAOD::PhotonContainer>(m_PhotonContainerName)) {
      if (evtStore()->retrieve(egC,m_PhotonContainerName).isFailure()) {
	ATH_MSG_FATAL("Cannot retrieve photon container " << m_PhotonContainerName);
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("PhotonContainer " << m_PhotonContainerName << " not available");
      return StatusCode::SUCCESS;
    }
  } else if (egType == "fwdelectron") {
    if (evtStore()->contains<xAOD::ElectronContainer>(m_FwdElectronContainerName)) {
      if (evtStore()->retrieve(egC,m_FwdElectronContainerName).isFailure()) {
	ATH_MSG_FATAL("Cannot retrieve forward electron container " << m_FwdElectronContainerName);
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("Forward ElectronContainer " << m_FwdElectronContainerName << " not available");
      return StatusCode::SUCCESS;
    }
  } else {
    ATH_MSG_WARNING("Unknown egamma type " << egType);
    return StatusCode::SUCCESS;
  }
  auto it = egC->begin(), itE = egC->end();
  for (; it != itE; it++) {
    auto eg = *it; 
    //
    ATH_MSG_DEBUG(egType << " pt,eta,phi = " << eg->pt()/1e3 << " " << eg->eta() << " " << eg->phi());
    // 
    // Calo Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp>::iterator itc = m_egCaloIso.begin(), itcE = m_egCaloIso.end();
    if (egType == "fwdelectron") {
      itc  = m_feCaloIso.begin();
      itcE = m_feCaloIso.end();
    }
    for (; itc != itcE; itc++) {
      CaloIsoHelp isoH = itc->second;
      xAOD::Iso::IsolationFlavour flav = itc->first;
      bool bsc = false;
      if (flav == xAOD::Iso::etcone && m_cellColl) 
	bsc = m_cellIsolationTool->decorateParticle_caloCellIso(*eg, isoH.isoTypes, isoH.CorrList, m_cellColl);
      else if (flav == xAOD::Iso::topoetcone)
	bsc = m_topoIsolationTool->decorateParticle_topoClusterIso(*eg, isoH.isoTypes, isoH.CorrList);
      else if (flav == xAOD::Iso::neflowisol)
	bsc = m_pflowIsolationTool->decorateParticle_eflowIso(*eg, isoH.isoTypes, isoH.CorrList);
      if (bsc) {
	for (unsigned int i = 0; i < isoH.isoTypes.size(); i++) {
	  float iso = 0;
	  bool gotIso  = eg->isolationValue(iso,isoH.isoTypes[i]);
	  if (gotIso)
	    ATH_MSG_DEBUG("Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3);
	  else
	    ATH_MSG_WARNING("Missing isolation result for " << isoH.isoTypes[i]);
	}
      } else
	ATH_MSG_WARNING("Call to CaloIsolationTool failed for flavour " << xAOD::Iso::toString(flav));
    }
    if (egType == "fwdelectron")
      return StatusCode::SUCCESS;
    // 
    // Track Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp>::iterator itt = m_egTrackIso.begin(), ittE = m_egTrackIso.end();
    for (; itt != ittE; itt++) {
      TrackIsoHelp isoH = itt->second;
      //const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
      std::set<const xAOD::TrackParticle*> tracksToExclude;
      if (eg->type() == xAOD::Type::Electron)
	tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
      else {
	if (m_allTrackRemoval) //New (from ??/??/16) : now this gives all tracks
	  tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
	else { // this is just to be able to have the 2015+2016 default case (only tracks from first vertex)
	  const xAOD::Photon *gam = dynamic_cast<const xAOD::Photon *>(eg);
	  if (gam->nVertices() > 0) {
	    const xAOD::Vertex *phvtx = gam->vertex(0);
	    for (unsigned int itk = 0; itk < phvtx->nTrackParticles(); itk++)
	      tracksToExclude.insert(m_useBremAssoc ? xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvtx->trackParticle(itk)) : phvtx->trackParticle(itk));
	  }
	}
      }
      xAOD::Vertex *vx = 0;
      bool bsc = m_trackIsolationTool->decorateParticle(*eg, isoH.isoTypes, isoH.CorrList, vx, &tracksToExclude);
      if (bsc) {
	unsigned int nI = isoH.isoTypes.size();
	for (unsigned int i = 0; i < nI; i++) {
	  xAOD::Iso::IsolationConeSize coneSize = enumconeSize(isoH.isoTypes[i]);
	  xAOD::Iso::IsolationType varIsoType   = xAOD::Iso::isolationType(xAOD::Iso::ptvarcone, coneSize);
	  float iso = 0, isoV = 0;
	  bool gotIso  = eg->isolationValue(iso,isoH.isoTypes[i]);
	  bool gotIsoV = eg->isolationValue(isoV,varIsoType);
	  if (gotIso && gotIsoV)
	    ATH_MSG_DEBUG("Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3 << ", var cone = " << isoV/1e3);
	  else
	    ATH_MSG_WARNING("Missing isolation result " << gotIso << " " << gotIsoV);
	}
      } else
	ATH_MSG_WARNING("Call to TrackIsolationTool failed");
    }
  }

  return StatusCode::SUCCESS;
} 
StatusCode IsolationBuilder::IsolateMuon() {

  SG::ReadHandle<xAOD::MuonContainer> muonC(m_MuonContainerName);      
  if (!muonC.isValid()) {
    ATH_MSG_FATAL("Cannot retrieve muons container " << m_MuonContainerName.key());
    return StatusCode::FAILURE;
  }

  auto it = muonC->begin(), itE = muonC->end();
  for (; it != itE; it++) {
    auto mu = *it;

    ATH_MSG_DEBUG("Muon pt,eta,phi = " << mu->pt()/1e3 << " " << mu->eta() << " " << mu->phi());
    // 
    // Calo Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp>::iterator itc = m_muCaloIso.begin(), itcE = m_muCaloIso.end();
    for (; itc != itcE; itc++) {
      CaloIsoHelp isoH = itc->second;
      xAOD::Iso::IsolationFlavour flav = itc->first;
      bool bsc = false;
      if (flav == xAOD::Iso::IsolationFlavour::etcone && m_cellColl) 
	bsc = m_cellIsolationTool->decorateParticle_caloCellIso(*mu, isoH.isoTypes, isoH.CorrList, m_cellColl);
      else if (flav == xAOD::Iso::IsolationFlavour::topoetcone)
	bsc = m_topoIsolationTool->decorateParticle_topoClusterIso(*mu, isoH.isoTypes, isoH.CorrList);
      else if (flav == xAOD::Iso::IsolationFlavour::neflowisol)
	bsc = m_pflowIsolationTool->decorateParticle_eflowIso(*mu, isoH.isoTypes, isoH.CorrList);
      if (bsc) {
	for (unsigned int i = 0; i < isoH.isoTypes.size(); i++) {
	  float iso = 0;
	  bool gotIso  = mu->isolation(iso,isoH.isoTypes[i]);
	  if (gotIso)
	    ATH_MSG_DEBUG("Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3);
	  else
	    ATH_MSG_WARNING("Missing isolation result for " << xAOD::Iso::toString(isoH.isoTypes[i]));
	}
      } else
	ATH_MSG_WARNING("Call to CaloIsolationTool failed for flavour " << xAOD::Iso::toString(flav));
    }
    // 
    // Track Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp>::iterator itt = m_muTrackIso.begin(), ittE = m_muTrackIso.end();
    for (; itt != ittE; itt++) {
      TrackIsoHelp isoH = itt->second;
      bool bsc = m_trackIsolationTool->decorateParticle(*mu, isoH.isoTypes, isoH.CorrList);
      if (bsc) {
	unsigned int nI = isoH.isoTypes.size();
	for (unsigned int i = 0; i < nI; i++) {
	  xAOD::Iso::IsolationConeSize coneSize = enumconeSize(isoH.isoTypes[i]);
	  xAOD::Iso::IsolationType varIsoType   = xAOD::Iso::isolationType(xAOD::Iso::IsolationFlavour::ptvarcone, coneSize);
	  float iso = 0, isoV = 0;
	  bool gotIso  = mu->isolation(iso,isoH.isoTypes[i]);
	  bool gotIsoV = mu->isolation(isoV,varIsoType);
	  if (gotIso && gotIsoV)
	    ATH_MSG_DEBUG("Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3 << ", var cone = " << isoV/1e3);
	  else
	    ATH_MSG_WARNING("Missing isolation result " << gotIso << " " << gotIsoV);
	}
      } else
	ATH_MSG_WARNING("Call to TrackIsolationTool failed");
    }
  }

  return StatusCode::SUCCESS;
} 

////
//// Now, decorate, used for custom configurations, storing with decorators
////
StatusCode IsolationBuilder::DecorateEgamma(std::string egType) {

  xAOD::EgammaContainer *egC(0);
  if (egType == "electron") {
    if (evtStore()->contains<xAOD::ElectronContainer>(m_ElectronContainerName)) {
      if (evtStore()->retrieve(egC,m_ElectronContainerName).isFailure()) {
	ATH_MSG_FATAL("Cannot retrieve electron container " << m_ElectronContainerName);
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("ElectronContainer " << m_ElectronContainerName << " not available");
      return StatusCode::SUCCESS;
    }
  } else if (egType == "photon") {
    if (evtStore()->contains<xAOD::PhotonContainer>(m_PhotonContainerName)) {
      if (evtStore()->retrieve(egC,m_PhotonContainerName).isFailure()) {
	ATH_MSG_FATAL("Cannot retrieve photon container " << m_PhotonContainerName);
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("PhotonContainer " << m_PhotonContainerName << " not available");
      return StatusCode::SUCCESS;
    } 
  } else if (egType == "fwdelectron") {
    if (evtStore()->contains<xAOD::ElectronContainer>(m_FwdElectronContainerName)) {
      if (evtStore()->retrieve(egC,m_FwdElectronContainerName).isFailure()) {
	ATH_MSG_FATAL("Cannot retrieve forward electron container " << m_FwdElectronContainerName);
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("Forward ElectronContainer " << m_FwdElectronContainerName << " not available");
      return StatusCode::SUCCESS;
    }
  } else {
    ATH_MSG_WARNING("Unknown egamma type " << egType);
    return StatusCode::SUCCESS;
  }
  xAOD::EgammaContainer::iterator it = egC->begin(), itE = egC->end();
  for (; it != itE; it++) {
    xAOD::Egamma *eg = *it; 
    //
    ATH_MSG_DEBUG(egType << " pt,eta,phi = " << eg->pt()/1e3 << " " << eg->eta() << " " << eg->phi());
    // 
    // Calo Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp>::iterator itc = m_egCaloIso.begin(), itcE = m_egCaloIso.end();
    if (egType == "fwdelectron") {
      itc  = m_feCaloIso.begin();
      itcE = m_feCaloIso.end();
    }
    for (; itc != itcE; itc++) {
      xAOD::CaloIsolation CaloIsoResult;
      CaloIsoHelp isoH = itc->second;
      xAOD::Iso::IsolationFlavour flav = itc->first;
      bool bsc = false;
      if (flav == xAOD::Iso::etcone && m_cellColl) 
	bsc = m_cellIsolationTool->caloCellIsolation(CaloIsoResult, *eg, isoH.isoTypes, isoH.CorrList, m_cellColl);
      else if (flav == xAOD::Iso::topoetcone)
	bsc = m_topoIsolationTool->caloTopoClusterIsolation(CaloIsoResult, *eg, isoH.isoTypes, isoH.CorrList);
      else if (flav == xAOD::Iso::neflowisol)
	bsc = m_pflowIsolationTool->neutralEflowIsolation(CaloIsoResult, *eg, isoH.isoTypes, isoH.CorrList);
      if (bsc) {
	for (unsigned int i = 0; i < isoH.isoTypes.size(); i++) {
	  float iso = CaloIsoResult.etcones[i];
	  ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3);
	  (*isoH.isoDeco[i])(*eg) = iso;
	}
      } else
	ATH_MSG_WARNING("Call to CaloIsolationTool failed for custom flavour " << xAOD::Iso::toString(flav));
    }
    if (egType == "fwdelectron")
      return StatusCode::SUCCESS;
    // 
    // Track Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp>::iterator itt = m_egTrackIso.begin(), ittE = m_egTrackIso.end();
    for (; itt != ittE; itt++) {
      xAOD::TrackIsolation TrackIsoResult;
      TrackIsoHelp isoH = itt->second;
      const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
      xAOD::Vertex *vx = 0;
      bool bsc = m_trackIsolationTool->trackIsolation(TrackIsoResult, *eg, isoH.isoTypes, isoH.CorrList, vx, &tracksToExclude);
      if (bsc) {
	unsigned int nI = isoH.isoTypes.size();
	for (unsigned int i = 0; i < nI; i++) {
	  float iso = TrackIsoResult.ptcones[i], isoV = TrackIsoResult.ptvarcones_10GeVDivPt[i];
	  ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3 << ", var cone = " << isoV/1e3);
	  (*isoH.isoDeco[2*i])(*eg)   = iso;
	  (*isoH.isoDeco[2*i+1])(*eg) = isoV;
	}
      } else
	ATH_MSG_WARNING("Call to custom TrackIsolationTool failed");
    }
  }
  
  return StatusCode::SUCCESS;
} 
StatusCode IsolationBuilder::DecorateMuon() {

  SG::ReadHandle<xAOD::MuonContainer> muonC(m_MuonContainerName);
  if (!muonC.isValid()) {
    ATH_MSG_FATAL("Cannot retrieve muons container " << m_MuonContainerName.key());
    return StatusCode::FAILURE;
  }

  auto it = muonC->begin(), itE = muonC->end();
  for (; it != itE; it++) {
    auto mu = *it;
    //
    ATH_MSG_DEBUG("Muon pt,eta,phi = " << mu->pt()/1e3 << " " << mu->eta() << " " << mu->phi());
    // 
    // Calo Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp>::iterator itc = m_muCaloIso.begin(), itcE = m_muCaloIso.end();
    for (; itc != itcE; itc++) {
      xAOD::CaloIsolation CaloIsoResult;
      CaloIsoHelp isoH = itc->second;
      xAOD::Iso::IsolationFlavour flav = itc->first;
      bool bsc = false;
      if (flav == xAOD::Iso::IsolationFlavour::etcone && m_cellColl) 
	bsc = m_cellIsolationTool->caloCellIsolation(CaloIsoResult, *mu, isoH.isoTypes, isoH.CorrList, m_cellColl);
      else if (flav == xAOD::Iso::IsolationFlavour::topoetcone)
	bsc = m_topoIsolationTool->caloTopoClusterIsolation(CaloIsoResult, *mu, isoH.isoTypes, isoH.CorrList);
      else if (flav == xAOD::Iso::IsolationFlavour::neflowisol)
	bsc = m_pflowIsolationTool->neutralEflowIsolation(CaloIsoResult, *mu, isoH.isoTypes, isoH.CorrList);
      if (bsc) {
	for (unsigned int i = 0; i < isoH.isoTypes.size(); i++) {
	  float iso = CaloIsoResult.etcones[i];
	  ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3);
	  (*isoH.isoDeco[i])(*mu) = iso;
	}
	// not that nice. I expect a single core correction (e.g. for topoetcone, not coreMuon and coreCone together...)
	xAOD::Iso::IsolationCaloCorrection icc = xAOD::Iso::coreCone;
	bool ison = false;
	if (CaloIsoResult.corrlist.calobitset.test(static_cast<unsigned int>(xAOD::Iso::coreMuon))) {
	  icc  = xAOD::Iso::coreMuon;
	  ison = true;
	} else if (CaloIsoResult.corrlist.calobitset.test(static_cast<unsigned int>(xAOD::Iso::coreCone)))
	  ison = true;
	if (ison) {
	  if (CaloIsoResult.coreCorrections.find(icc) != CaloIsoResult.coreCorrections.end()) {
	    if (CaloIsoResult.coreCorrections[icc].find(xAOD::Iso::coreEnergy) != CaloIsoResult.coreCorrections[icc].end())
	      (*isoH.coreCorisoDeco)(*mu) = CaloIsoResult.coreCorrections[icc][xAOD::Iso::coreEnergy];
	    else
	      ATH_MSG_WARNING("Cannot find the core energy correction for custom flavour " << xAOD::Iso::toString(flav));
	  } else
	    ATH_MSG_WARNING("Cannot find the core correction for custom flavour " << xAOD::Iso::toString(flav));
	}
      } else
	ATH_MSG_WARNING("Call to CaloIsolationTool failed for custom flavour " << xAOD::Iso::toString(flav));
    }
    // 
    // Track Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp>::iterator itt = m_muTrackIso.begin(), ittE = m_muTrackIso.end();
    for (; itt != ittE; itt++) {
      xAOD::TrackIsolation TrackIsoResult;
      TrackIsoHelp isoH = itt->second;
      bool bsc = m_trackIsolationTool->trackIsolation(TrackIsoResult, *mu, isoH.isoTypes, isoH.CorrList);
      if (bsc) {
	unsigned int nI = isoH.isoTypes.size();
	for (unsigned int i = 0; i < nI; i++) {
	  float iso = TrackIsoResult.ptcones[i], isoV = TrackIsoResult.ptvarcones_10GeVDivPt[i];
	  ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toString(isoH.isoTypes[i]) << " = " << iso/1e3 << ", var cone = " << isoV/1e3);
	  (*isoH.isoDeco[2*i])(*mu)   = iso;
	  (*isoH.isoDeco[2*i+1])(*mu) = isoV;
	}
      } else
	ATH_MSG_WARNING("Call to custom TrackIsolationTool failed");
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
