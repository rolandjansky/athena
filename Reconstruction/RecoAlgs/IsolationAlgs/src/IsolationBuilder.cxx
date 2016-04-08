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

#include "RecoToolInterfaces/ICaloCellIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/INeutralEFlowIsolationTool.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "CaloEvent/CaloCellContainer.h"

#include "boost/foreach.hpp"
#include "boost/format.hpp"

#include <set>
#include <utility>

IsolationBuilder::IsolationBuilder( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_cellColl (nullptr)
{
  //
  // Property declaration
  // 
  declareProperty("ElectronCollectionContainerName", m_ElectronContainerName = "Electrons");
  declareProperty("PhotonCollectionContainerName",   m_PhotonContainerName   = "Photons");
  declareProperty("MuonCollectionContainerName",     m_MuonContainerName     = "Muons");
  declareProperty("CellCollectionName",              m_cellsName             ="AllCalo", "Name of container which contain calo cells");
  declareProperty("CaloCellIsolationTool",           m_cellIsolationTool,    "Handle of the calo cell IsolationTool");
  declareProperty("CaloTopoIsolationTool",           m_topoIsolationTool,    "Handle of the calo topo IsolationTool");
  declareProperty("PFlowIsolationTool",              m_pflowIsolationTool,   "Handle of the pflow IsolationTool");
  declareProperty("TrackIsolationTool",              m_trackIsolationTool,   "Handle of the track IsolationTool");
  declareProperty("useBremAssoc",                    m_useBremAssoc          = true, "use track to track assoc after brem");
  declareProperty("EgIsoTypes",                      m_egisoInts, "The isolation types to do for egamma: vector of vector of enum type Iso::IsolationType, stored as float");
  declareProperty("EgCorTypes",                      m_egcorInts, "The correction types to do for egamma iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float");
  declareProperty("MuIsoTypes",                      m_muisoInts, "The isolation types to do for Muons : vector of vector of enum type Iso::IsolationType, stored as float");
  declareProperty("MuCorTypes",                      m_mucorInts, "The correction types to do for Muon iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float");
  declareProperty("CustomConfgurationName",          m_customConfig          = "", "use a custom configuration");
}

IsolationBuilder::~IsolationBuilder() {}

StatusCode IsolationBuilder::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  std::set<xAOD::Iso::IsolationFlavour> runIsoType;

  // Build helpers for all required isolations
  // For egamma
  unsigned int nI = m_egisoInts.size();
  if (nI > 0 && m_egisoInts[0].size() > 0) 
    ATH_MSG_INFO("Will built egamma isolation types : ");
  else 
    nI = 0;
  for (unsigned int ii = 0; ii < nI; ii++) {
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav =
      xAOD::Iso::numIsolationFlavours;
    for (unsigned int jj = 0; jj < m_egisoInts[ii].size(); jj++) {
      int egIso = int(m_egisoInts[ii][jj]+0.5);
      xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(egIso);
      isoTypes.push_back(isoType);
      isoFlav = xAOD::Iso::isolationFlavour(isoType);
      std::string isoName = xAOD::Iso::toString(isoType);
      if (m_customConfig != "") {
	isoName += "_"; isoName += m_customConfig;
	Deco.push_back(new SG::AuxElement::Decorator<float>(isoName));
      }
      ATH_MSG_INFO(isoName);
      if (isoFlav == xAOD::Iso::ptcone) {
	isoName = "ptvarcone";
	int is = 100*xAOD::Iso::coneSize(isoType);
	isoName += std::to_string(is);
	if (m_customConfig != "") {
	  isoName += "_"; isoName += m_customConfig;
	  Deco.push_back(new SG::AuxElement::Decorator<float>(isoName));
	}
	ATH_MSG_INFO(isoName);
      }
    }
    if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
      CaloIsoHelp cisoH;
      cisoH.isoTypes = isoTypes;
      if (m_customConfig != "") cisoH.isoDeco  = Deco;
      xAOD::CaloCorrection corrlist;
      for (unsigned int jj = 0; jj < m_egcorInts[ii].size(); jj++) {
        int egCor = int(m_egcorInts[ii][jj]+0.5);
	corrlist.calobitset.set(static_cast<unsigned int>(egCor));
      }
      cisoH.CorrList = corrlist;
      m_egCaloIso.insert(std::make_pair(isoFlav,cisoH));
    } else if (isoFlav == xAOD::Iso::ptcone) {
      TrackIsoHelp tisoH;
      tisoH.isoTypes = isoTypes;
      if (m_customConfig != "") tisoH.isoDeco  = Deco;
      xAOD::TrackCorrection corrlist;
      corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
      tisoH.CorrList = corrlist;
      m_egTrackIso.insert(std::make_pair(isoFlav,tisoH));
    } else 
      ATH_MSG_WARNING("Isolation flavour " << xAOD::Iso::toString(isoFlav) << " does not exist ! Check your inputs");
    if (runIsoType.find(isoFlav) == runIsoType.end()) runIsoType.insert(isoFlav);
  }

  // For muons
  nI = m_muisoInts.size();
  if (nI > 0 && m_muisoInts[0].size() > 0) 
    ATH_MSG_INFO("Will built muon isolation types : ");
  else 
    nI = 0;
  for (unsigned int ii = 0; ii < nI; ii++) {
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav =
      xAOD::Iso::numIsolationFlavours;
    for (unsigned int jj = 0; jj < m_muisoInts[ii].size(); jj++) {
      int muIso = int(m_muisoInts[ii][jj]+0.5);
      xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(muIso);
      isoTypes.push_back(isoType);
      isoFlav = xAOD::Iso::isolationFlavour(isoType);
      std::string isoName = xAOD::Iso::toString(isoType);
      if (m_customConfig != "") {
	isoName += "_"; isoName += m_customConfig;
	Deco.push_back(new SG::AuxElement::Decorator<float>(isoName));
      }
      ATH_MSG_INFO(isoName);
      if (isoFlav == xAOD::Iso::ptcone) {
	isoName = "ptvarcone";
	int is = 100*xAOD::Iso::coneSize(isoType);
	isoName += std::to_string(is);
	if (m_customConfig != "") {
	  isoName += "_"; isoName += m_customConfig;
	  Deco.push_back(new SG::AuxElement::Decorator<float>(isoName));
	}
	ATH_MSG_INFO(isoName);
      }
    }
    if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
      CaloIsoHelp cisoH;
      cisoH.isoTypes = isoTypes;
      if (m_customConfig != "") cisoH.isoDeco  = Deco;
      xAOD::CaloCorrection corrlist;
      for (unsigned int jj = 0; jj < m_mucorInts[ii].size(); jj++) {
	int muCor = int(m_mucorInts[ii][jj]+0.5);
	corrlist.calobitset.set(static_cast<unsigned int>(muCor));
      }
      cisoH.CorrList = corrlist;
      m_muCaloIso.insert(std::make_pair(isoFlav,cisoH));
    } else if (isoFlav == xAOD::Iso::ptcone) {
      TrackIsoHelp tisoH;
      tisoH.isoTypes = isoTypes;
      if (m_customConfig != "") tisoH.isoDeco  = Deco;
      xAOD::TrackCorrection corrlist;
      corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
      tisoH.CorrList = corrlist;
      m_muTrackIso.insert(std::make_pair(isoFlav,tisoH));
    } else 
      ATH_MSG_WARNING("Isolation flavour " << xAOD::Iso::toString(isoFlav) << " does not exist ! Check your inputs");
    if (runIsoType.find(isoFlav) == runIsoType.end()) runIsoType.insert(isoFlav);
  }

  // Retrieve the tools (there three Calo ones are the same in fact)
  if (!m_cellIsolationTool.empty() && runIsoType.find(xAOD::Iso::etcone) != runIsoType.end())
    ATH_CHECK(m_cellIsolationTool.retrieve());

  if (!m_topoIsolationTool.empty() && runIsoType.find(xAOD::Iso::topoetcone) != runIsoType.end())
    ATH_CHECK(m_topoIsolationTool.retrieve());

  if (!m_pflowIsolationTool.empty() && runIsoType.find(xAOD::Iso::neflowisol) != runIsoType.end())
    ATH_CHECK(m_pflowIsolationTool.retrieve());

  if (!m_trackIsolationTool.empty() && runIsoType.find(xAOD::Iso::IsolationFlavour::ptcone) != runIsoType.end())
    ATH_CHECK(m_trackIsolationTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

 // Delete pointers to decorators
  std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp>::iterator itc = m_egCaloIso.begin(), itcE = m_egCaloIso.end();
  for (; itc != itcE; itc++) {
    std::vector<SG::AuxElement::Decorator<float>*> vec = itc->second.isoDeco;
    for (unsigned int ii = 0; ii < vec.size(); ii++)
      delete vec[ii];
  }
  std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp>::iterator itt = m_egTrackIso.begin(), ittE = m_egTrackIso.end();
  for (; itt != ittE; itt++) {
    std::vector<SG::AuxElement::Decorator<float>*> vec = itt->second.isoDeco;
    for (unsigned int ii = 0; ii < vec.size(); ii++)
      delete vec[ii];
  }
  itc = m_muCaloIso.begin(), itcE = m_muCaloIso.end();
  for (; itc != itcE; itc++) {
    std::vector<SG::AuxElement::Decorator<float>*> vec = itc->second.isoDeco;
    for (unsigned int ii = 0; ii < vec.size(); ii++)
      delete vec[ii];
  }
  itt = m_muTrackIso.begin(), ittE = m_muTrackIso.end();
  for (; itt != ittE; itt++) {
    std::vector<SG::AuxElement::Decorator<float>*> vec = itt->second.isoDeco;
    for (unsigned int ii = 0; ii < vec.size(); ii++)
      delete vec[ii];
  }

  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // For etcone, needs the cells
  if (!m_cellIsolationTool.empty()) {
    if (evtStore()->retrieve(m_cellColl, m_cellsName).isFailure() || !m_cellColl) {
      ATH_MSG_WARNING("Cannot retrieve Calo Cell Container " << m_cellsName << ". Will not compute etcone");
      m_cellColl = 0;
    }
  }

  // Compute isolations
  if (m_customConfig == "") { 
    // Here, default configurations with standard names can be are computed
    if (m_egCaloIso.size() != 0 || m_egTrackIso.size() != 0) {
      CHECK(IsolateEgamma("electron"));
      CHECK(IsolateEgamma("photon"));
    }
    if (m_muCaloIso.size() != 0 || m_muTrackIso.size() != 0)
      CHECK(IsolateMuon());
  } else {
    // Here, custom configurations with custom names can be computed
    if (m_egCaloIso.size() != 0 || m_egTrackIso.size() != 0) {
      CHECK(DecorateEgamma("electron"));
      CHECK(DecorateEgamma("photon"));
    }
    if (m_muCaloIso.size() != 0 || m_muTrackIso.size() != 0)
      CHECK(DecorateMuon());
  }

  return StatusCode::SUCCESS;
}

StatusCode IsolationBuilder::IsolateEgamma(std::string egType) {

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
    // 
    // Track Isolation types
    //
    std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp>::iterator itt = m_egTrackIso.begin(), ittE = m_egTrackIso.end();
    for (; itt != ittE; itt++) {
      TrackIsoHelp isoH = itt->second;
      const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
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

  xAOD::MuonContainer *muonC(0);
  if (evtStore()->contains<xAOD::MuonContainer>(m_MuonContainerName)) {
    if (evtStore()->retrieve(muonC,m_MuonContainerName).isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve muons container " << m_MuonContainerName);
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG("MuonContainer " << m_MuonContainerName << " not available");
    return StatusCode::SUCCESS;
  }

  xAOD::MuonContainer::iterator it = muonC->begin(), itE = muonC->end();
  for (; it != itE; it++) {
    xAOD::Muon *mu = *it;

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

  xAOD::MuonContainer *muonC(0);
  if (evtStore()->contains<xAOD::MuonContainer>(m_MuonContainerName)) {
    if (evtStore()->retrieve(muonC,m_MuonContainerName).isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve muons container " << m_MuonContainerName);
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG("MuonContainer " << m_MuonContainerName << " not available");
    return StatusCode::SUCCESS;
  }

  xAOD::MuonContainer::iterator it = muonC->begin(), itE = muonC->end();
  for (; it != itE; it++) {
    xAOD::Muon *mu = *it;
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
