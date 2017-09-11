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
  ::AthAlgorithm( name, pSvcLocator ),
  m_cellIsolationTool("", this),
  m_cellColl (nullptr),
  m_topoIsolationTool("", this),
  m_pflowIsolationTool("", this),
  m_trackIsolationTool("", this)
{
  //
  // Property declaration
  // 
  declareProperty("ElectronCollectionContainerName",    m_ElectronContainerName    = "Electrons");
  declareProperty("PhotonCollectionContainerName",      m_PhotonContainerName      = "Photons");
  declareProperty("MuonCollectionContainerName",        m_MuonContainerName        = "Muons");
  declareProperty("FwdElectronCollectionContainerName", m_FwdElectronContainerName = "ForwardElectrons");
  declareProperty("CellCollectionName",              m_cellsName             = "AllCalo", "Name of container which contain calo cells");
  declareProperty("CaloCellIsolationTool",           m_cellIsolationTool,    "Handle of the calo cell IsolationTool");
  declareProperty("CaloTopoIsolationTool",           m_topoIsolationTool,    "Handle of the calo topo IsolationTool");
  declareProperty("PFlowIsolationTool",              m_pflowIsolationTool,   "Handle of the pflow IsolationTool");
  declareProperty("TrackIsolationTool",              m_trackIsolationTool,   "Handle of the track IsolationTool");
  declareProperty("useBremAssoc",                    m_useBremAssoc          = true, "use track to track assoc after brem");
  declareProperty("FeIsoTypes",                      m_feisoInts, "The isolation types to do for forward electron: vector of vector of enum type Iso::IsolationType, stored as float");
  declareProperty("FeCorTypes",                      m_fecorInts, "The correction types to do for forward electron iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float");
  declareProperty("EgIsoTypes",                      m_egisoInts, "The isolation types to do for egamma: vector of vector of enum type Iso::IsolationType, stored as float");
  declareProperty("EgCorTypes",                      m_egcorInts, "The correction types to do for egamma iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float");
  declareProperty("MuIsoTypes",                      m_muisoInts, "The isolation types to do for Muons : vector of vector of enum type Iso::IsolationType, stored as float");
  declareProperty("MuCorTypes",                      m_mucorInts, "The correction types to do for Muon iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float");
  declareProperty("CustomConfigurationName",         m_customConfig          = "", "use a custom configuration");
  declareProperty("CustomConfigurationNameEl",       m_customConfigEl        = "", "use a custom configuration for electron");
  declareProperty("CustomConfigurationNamePh",       m_customConfigPh        = "", "use a custom configuration for photon");
  declareProperty("CustomConfigurationNameFwd",      m_customConfigFwd       = "", "use a custom configuration for forward electron");
  declareProperty("CustomConfigurationNameMu",       m_customConfigMu        = "", "use a custom configuration for muon");
  declareProperty("IsAODFix",                        m_isAODFix              = false);
  declareProperty("LeakageTool",                     m_leakTool,                   "Handle of the leakage Tool");
  declareProperty("IsolateEl",                       m_isolateEl             = true, "since egIsoTypes is common for el and ph, a new flag to control individually : electron");
  declareProperty("IsolatePh",                       m_isolatePh             = true, "since egIsoTypes is common for el and ph, a new flag to control individually : photon");
  declareProperty("AllTrackRemoval",                 m_allTrackRemoval       = true);
  
}

IsolationBuilder::~IsolationBuilder() {}

StatusCode IsolationBuilder::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  std::set<xAOD::Iso::IsolationFlavour> runIsoType;

  // Build helpers for all required isolations
  // For egamma (central)
  unsigned int nI = m_egisoInts.size();
  if (nI > 0 && m_egisoInts[0].size() > 0) 
    ATH_MSG_INFO("Will built egamma isolation types : ");
  else 
    nI = 0;
  for (unsigned int ii = 0; ii < nI; ii++) { // the various types : should be 4 by default (etcone, topoetcone, ptcone, neflowisol)
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav =
      xAOD::Iso::numIsolationFlavours;
    for (unsigned int jj = 0; jj < m_egisoInts[ii].size(); jj++) { // the size of the cone : should be 3 by default
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

  // For forward electrons
  nI = m_feisoInts.size();
  if (nI > 0 && m_feisoInts[0].size() > 0) 
    ATH_MSG_INFO("Will built forward electron isolation types : ");
  else 
    nI = 0;
  for (unsigned int ii = 0; ii < nI; ii++) { // here, should be at most three (no ptcone)
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav =
      xAOD::Iso::numIsolationFlavours;
    for (unsigned int jj = 0; jj < m_feisoInts[ii].size(); jj++) {
      int egIso = int(m_feisoInts[ii][jj]+0.5);
      xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(egIso);
      isoTypes.push_back(isoType);
      isoFlav = xAOD::Iso::isolationFlavour(isoType);
      std::string isoName = xAOD::Iso::toString(isoType);
      if (m_customConfig != "") {
	isoName += "_"; isoName += m_customConfig;
	Deco.push_back(new SG::AuxElement::Decorator<float>(isoName));
      }
      ATH_MSG_INFO(isoName);
    }
    if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
      CaloIsoHelp cisoH;
      cisoH.isoTypes = isoTypes;
      if (m_customConfig != "") cisoH.isoDeco  = Deco;
      xAOD::CaloCorrection corrlist;
      for (unsigned int jj = 0; jj < m_fecorInts[ii].size(); jj++) {
        int egCor = int(m_fecorInts[ii][jj]+0.5);
	corrlist.calobitset.set(static_cast<unsigned int>(egCor));
      }
      cisoH.CorrList = corrlist;
      m_feCaloIso.insert(std::make_pair(isoFlav,cisoH));
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
  for (unsigned int ii = 0; ii < nI; ii++) {   // the flavor (cell, topo, eflow, track
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav =
      xAOD::Iso::numIsolationFlavours;
    for (unsigned int jj = 0; jj < m_muisoInts[ii].size(); jj++) { // the cone size
      int muIso = int(m_muisoInts[ii][jj]+0.5);
      xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(muIso);
      isoTypes.push_back(isoType);
      isoFlav = xAOD::Iso::isolationFlavour(isoType);
      std::string isoName = xAOD::Iso::toString(isoType);
      if (m_customConfigMu != "") {
	isoName += "_"; isoName += m_customConfigMu;
	Deco.push_back(new SG::AuxElement::Decorator<float>(isoName));
      }
      ATH_MSG_INFO(isoName);
      if (isoFlav == xAOD::Iso::ptcone) {
	isoName = "ptvarcone";
	int is = 100*xAOD::Iso::coneSize(isoType);
	isoName += std::to_string(is);
	if (m_customConfigMu != "") {
	  isoName += "_"; isoName += m_customConfigMu;
	  Deco.push_back(new SG::AuxElement::Decorator<float>(isoName));
	}
	ATH_MSG_INFO(isoName);
      }
    }
    if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
      CaloIsoHelp cisoH;
      cisoH.isoTypes = isoTypes;
      cisoH.coreCorisoDeco = nullptr;
      if (m_customConfigMu != "") cisoH.isoDeco  = Deco;
      xAOD::CaloCorrection corrlist;
      for (unsigned int jj = 0; jj < m_mucorInts[ii].size(); jj++) {
	int muCor = int(m_mucorInts[ii][jj]+0.5);
	corrlist.calobitset.set(static_cast<unsigned int>(muCor));
	xAOD::Iso::IsolationCaloCorrection isoCor = static_cast<xAOD::Iso::IsolationCaloCorrection>(muCor);
	if (m_customConfigMu != "" && (isoCor == xAOD::Iso::coreCone || isoCor == xAOD::Iso::coreMuon) ) {
	  std::string isoCorName = "";
	  if (isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol)
	    isoCorName = xAOD::Iso::toString(isoFlav);
	  isoCorName += xAOD::Iso::toString(isoCor);
	  isoCorName += xAOD::Iso::toString(xAOD::Iso::coreEnergy); isoCorName += "Correction"; // hard coded since we never store the core area in fact
	  isoCorName += "_"; isoCorName += m_customConfigMu;
	  cisoH.coreCorisoDeco = new SG::AuxElement::Decorator<float>(isoCorName);
      }
      }
      cisoH.CorrList = corrlist;
      m_muCaloIso.insert(std::make_pair(isoFlav,cisoH));
    } else if (isoFlav == xAOD::Iso::ptcone) {
      TrackIsoHelp tisoH;
      tisoH.isoTypes = isoTypes;
      if (m_customConfigMu != "") tisoH.isoDeco  = Deco;
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


  // Also can apply leakage correction if AODFix :
  // be carefull ! There is a leakage in topoetcone, etcone, ... : do not run this !!!!!! (useless)
  if (m_isAODFix && !m_leakTool.empty()) {
    ATH_MSG_INFO("Will run leakage corrections for photons and electrons");
    ATH_CHECK(m_leakTool.retrieve());
  }

  //initialise data handles
  ATH_CHECK(m_MuonContainerName.initialize());

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
    if (itc->second.coreCorisoDeco != nullptr)      // it is only there for coreCone or coreMuon and if decoration
      delete itc->second.coreCorisoDeco;            // only implemented for muon calo iso for the time being...
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

  // // If AODFix, first deep copy  -- (JM: no longer necessary)
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
  /*
  if (m_customConfig == "") { 
    // Here, default configurations with standard names can be are computed
    if (m_egCaloIso.size() != 0 || m_egTrackIso.size() != 0) {
      if (m_ElectronContainerName.size()) CHECK(IsolateEgamma("electron"));
      if (m_PhotonContainerName.size())   CHECK(IsolateEgamma("photon"));
    }
    if (m_feCaloIso.size() != 0)
      if (m_FwdElectronContainerName.size()) CHECK(IsolateEgamma("fwdelectron"));
      
    if (m_muCaloIso.size() != 0 || m_muTrackIso.size() != 0)
      if (m_MuonContainerName.size()) CHECK(IsolateMuon());
  } else {
    // Here, custom configurations with custom names can be computed
    if (m_egCaloIso.size() != 0 || m_egTrackIso.size() != 0) {
      if (m_ElectronContainerName.size()) CHECK(DecorateEgamma("electron"));
      if (m_PhotonContainerName.size())   CHECK(DecorateEgamma("photon"));
    }
    if (m_feCaloIso.size() != 0)
      if (m_FwdElectronContainerName.size()) CHECK(DecorateEgamma("fwdelectron"));

    if (m_muCaloIso.size() != 0 || m_muTrackIso.size() != 0)
      if (m_MuonContainerName.size()) CHECK(DecorateMuon());
  }
  */
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

  if (m_isAODFix && !m_leakTool.empty())
    CHECK(runLeakage());

  
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

StatusCode IsolationBuilder::runLeakage() {
  
  // Retrieve data
  if (m_PhotonContainerName.size()) {
    xAOD::PhotonContainer* photons = evtStore()->retrieve< xAOD::PhotonContainer >(m_PhotonContainerName);
    if( !photons ) {
      ATH_MSG_ERROR("Couldn't retrieve photon container with key: " << m_PhotonContainerName);
      return StatusCode::FAILURE;
    }
    for (auto ph : *photons)           
      m_leakTool->applyCorrection(*ph);
  }
    
  if (m_ElectronContainerName.size()) {
    xAOD::ElectronContainer* electrons = evtStore()->retrieve< xAOD::ElectronContainer >(m_ElectronContainerName);
    if( !electrons ) {
      ATH_MSG_ERROR("Couldn't retrieve electron container with key: " << m_ElectronContainerName);
      return StatusCode::FAILURE;
    }
    for (auto el : *electrons)
      m_leakTool->applyCorrection(*el);
  }

  return StatusCode::SUCCESS;
}


template< class CONTAINER, class AUXSTORE > StatusCode IsolationBuilder::deepCopy( const std::string& key ) const {
    
  // Let the user know what's happening:
  ATH_MSG_VERBOSE( "Running deepCopy on container: " << key );
  
  // Decide which implementation to call:
  if( evtStore()->template contains< AUXSTORE >( key + "Aux." ) ) {
    if( deepCopyImp< CONTAINER, AUXSTORE >( key ).isFailure() ) {
      ATH_MSG_FATAL( "Couldn't call deepCopyImp with concrete "
		     "auxiliary store" );
      return StatusCode::FAILURE;
    }
  } else if( evtStore()->template contains< xAOD::AuxContainerBase >( key +
								      "Aux." ) ) {
    if( deepCopyImp< CONTAINER,
	xAOD::AuxContainerBase >( key ).isFailure() ) {
      ATH_MSG_FATAL( "Couldn't call deepCopyImp with generic "
		     "auxiliary store" );
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_FATAL( "Couldn't discover auxiliary store type for container \""
		   << key << "\"" );
    return StatusCode::FAILURE;
  }
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}
  
template< class CONTAINER, class AUXSTORE > StatusCode IsolationBuilder::deepCopyImp( const std::string& key ) const {
  
  // Retrieve the const container:
  ATH_MSG_VERBOSE( "Will retrieve " << key);
  const CONTAINER* c = 0;
  ATH_CHECK( evtStore()->retrieve( c, key ) );
  
  // Create the new container:
  ATH_MSG_VERBOSE( "Will create new containers" );
  CONTAINER* copy = new CONTAINER();
  AUXSTORE* copyAux = new AUXSTORE();
  copy->setStore( copyAux );
  
  // Do the deep copy:
  ATH_MSG_VERBOSE( "Will copy the object" );
  for( auto oldObj : *c ) {
    ATH_MSG_VERBOSE( "Now working on object " << oldObj);
    auto newObj = new typename CONTAINER::base_value_type();
    copy->push_back( newObj );
    *newObj = *oldObj;
  }
  
  // Do the overwrite:
  ATH_MSG_VERBOSE( "Will overwrite the container" );
  ATH_CHECK( evtStore()->overwrite( copy, key, true, false ) );
  ATH_MSG_VERBOSE( "Will overwrite the aux container" );
  ATH_CHECK( evtStore()->overwrite( copyAux, key + "Aux.", true, false ) );
  ATH_MSG_VERBOSE( "Done" );
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}
