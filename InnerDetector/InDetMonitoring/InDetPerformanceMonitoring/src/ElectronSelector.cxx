/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//==================================================================================
//
//  ElectronSelector.cxx :       Class designed to reconstruct di-electrons events
//                        in particular Z0 -> e+ e- events.
//==================================================================================

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/ElectronSelector.h"
// Package Headers
#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include <sstream>
// ATLAS headers
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Random/RandFlat.h"

#include "GaudiKernel/IToolSvc.h"


// Static declarations
std::atomic<unsigned int> ElectronSelector::s_uNumInstances;

//==================================================================================
// Public Methods
//==================================================================================
ElectronSelector::ElectronSelector():
  m_doDebug ( false ),
  m_ptCut ( 10. ),
  m_etaCut ( 2.47 ) // 2.47 is the official acceptance for central electrons. Forward electrons is another story...
{
  ++s_uNumInstances;
  
  std::stringstream xTmp;  xTmp << s_uNumInstances;
  m_xSampleName     = "ElectronSelector_" + xTmp.str();
  
  m_pxElectron = nullptr;
  
  m_msgStream =  new MsgStream(PerfMonServices::getMessagingService(), "InDetPerformanceMonitoring" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ElectronSelector::~ElectronSelector()
{
  --s_uNumInstances;
  delete m_msgStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ElectronSelector::Init()
{
  (*m_msgStream) << MSG::INFO << " -- ElectronSelector::Init -- START -- " << endmsg;
  ISvcLocator* serviceLocator = Gaudi::svcLocator();
  IToolSvc* toolSvc;
  StatusCode sc = serviceLocator->service("ToolSvc", toolSvc, true);
  
  if ( sc.isFailure() || toolSvc == 0 ) {
    (*m_msgStream) << MSG::ERROR << "  * ElectronSelector::Init * Unable to retrieve ToolSvc " << endmsg;
    return;
  }
  
  // PARENT::Init();

  //---Electron Likelihood tool---
  // m_doIDCuts = true;
  (*m_msgStream) << MSG::INFO << "ElectronSelector::Init -- Setting up electron LH tool." << endmsg;
  m_LHTool2015 = new AsgElectronLikelihoodTool ("m_LHTool2015");

  const std::string elecWorkingPoint = "LooseLHElectron"; // "MediumLHElectron" "TightLHElectron"

  if((m_LHTool2015->setProperty("WorkingPoint",elecWorkingPoint.c_str())).isFailure()) {
    (*m_msgStream) << MSG::WARNING << "Failure loading ConfigFile for electron likelihood tool with working point: " << elecWorkingPoint.c_str()  << endmsg;
  } 
  else  {
    (*m_msgStream) << MSG::INFO << "Loading ConfigFile for electron likelihood tool with working point: " << elecWorkingPoint << ". SUCCESS " << endmsg;    
  } 

  // check config files at: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/EGammaIdentificationRun2
  std::string confDir = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodVeryLooseOfflineConfig2017_Smooth.conf";
  if ( (m_LHTool2015->setProperty("ConfigFile", confDir)).isSuccess()) {
    (*m_msgStream) << MSG::INFO << "Electron likelihood config ("<< confDir.c_str() << ") setting SUCCESS!" << endmsg;
  }
  else {
    (*m_msgStream) << MSG::WARNING << "Electron likelihood config ("<< confDir.c_str() << ") setting FAILURE" << endmsg;
  }

  if (m_LHTool2015->initialize().isSuccess()) {
    (*m_msgStream) << MSG::INFO << "Electron likelihood tool initialize() SUCCESS!" << endmsg;
  }
  else {
    (*m_msgStream) << MSG::WARNING << "Electron likelihood tool initialize() FAILURE!" << endmsg;
  }

  (*m_msgStream) << MSG::INFO << " --ElectronSelector::Init -- COMPLETED -- " << endmsg;
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ElectronSelector::PrepareElectronList(const xAOD::ElectronContainer* pxElecContainer)
{
  (*m_msgStream) << MSG::DEBUG << " --ElectronSelector::PrepareElectronList -- START  -- " << endmsg;
  Clear(); // clear current list records

  typedef xAOD::ElectronContainer::const_iterator electron_iterator;
  electron_iterator iter    = pxElecContainer->begin();
  electron_iterator iterEnd = pxElecContainer->end();
  
  // Loop over the Electrons                                                                                                                                                       
  int electroncount = 0;
  for(; iter != iterEnd ; iter++) {
    electroncount++;
    (*m_msgStream) << MSG::DEBUG  << " -- ElectronSelector::PrepareElectronList -- candiate electron " << electroncount 
		   << " has author " << (*iter)->author(xAOD::EgammaParameters::AuthorElectron)
		   << endmsg;
    const xAOD::Electron * ELE = (*iter);
    if ( RecordElectron(ELE) ) {
      (*m_msgStream) << MSG::DEBUG  << " -- ElectronSelector::PrepareElectronList -- candiate electron " << electroncount 
		     << " is good " 
		     << endmsg;
    }
  }
  bool progressingwell = true;
  
  (*m_msgStream) << MSG::DEBUG  << " -- ElectronSelector::PrepareElectronList -- finished recording electrons. "
		 << "  recorded electrons: " << m_pxElTrackList.size()
		 << "  out of tested electron candidates:" << electroncount  << endmsg;
  if (m_pxElTrackList.size() < 2) progressingwell = false;
  if (progressingwell) progressingwell = OrderElectronList ();
  if (progressingwell) progressingwell = RetrieveVertices ();

  if (!progressingwell) {
    (*m_msgStream) << MSG::DEBUG  << " -- ElectronSelector::PrepareElectronList -- FAILED -- this event has not even a good e+e- pair "  << endmsg;
    this->Clear(); // reset the content as it is not going to be used
  }

  (*m_msgStream) << MSG::DEBUG << " -- ElectronSelector::PrepareElectronList -- COMPLETED -- electroncount -- m_pxElTrackList.size() / all = " 
		 << m_pxElTrackList.size() << " / " << electroncount 
		 << endmsg;
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ElectronSelector::RecordElectron (const xAOD::Electron * thisElec)
{
  // start assuming electron candidate is good 
  bool electronisgood = true;

  // check the electron satisfies the working point
  if (!m_LHTool2015->accept(thisElec) ) {
    electronisgood = false;
    (*m_msgStream) << MSG::DEBUG << " -- electron fails workingpoint selection  -- " << endmsg;
  }

  //Get the track particle                                                                                                                                                        
  const xAOD::TrackParticle* theTrackParticle = thisElec->trackParticle();
  
  if (!theTrackParticle) {
    electronisgood = false;
    (*m_msgStream) << MSG::DEBUG << " -- electron fails trackparticle  -- " << endmsg;
  }

  if (electronisgood && thisElec->author(xAOD::EgammaParameters::AuthorElectron) != 1) {
    electronisgood = false;
    (*m_msgStream) << MSG::DEBUG << "   -- electron fails author  -- " << thisElec->author(xAOD::EgammaParameters::AuthorElectron) << endmsg;
  }

  if (electronisgood && theTrackParticle->pt() * m_CGeV < m_ptCut ) { // pt cut given in GeV
    electronisgood = false;
    (*m_msgStream) << MSG::DEBUG << "   -- electron fails pt cut  -- pt= " << theTrackParticle->pt()
		   << " < " << m_ptCut << " (cut value) "
		   << endmsg;
  }

  const xAOD::CaloCluster* cluster = thisElec->caloCluster();
  if(!cluster) {
    electronisgood = false;
    (*m_msgStream) << MSG::DEBUG << "   -- electron candidate has no CaloCluster  " << endmsg; 
  }

  if (electronisgood && (cluster->e() * sin(theTrackParticle->theta())) * m_CGeV < m_ptCut) { // cut on et of the cluster
    electronisgood = false;
    (*m_msgStream) << MSG::DEBUG << "   -- electron fails cluster Et cut  -- Et= " << (cluster->e() * cos(theTrackParticle->theta()))* m_CGeV 
		   << " < " << m_ptCut << " (cut value) "
		   << endmsg;
  }

  if (electronisgood && (fabs(cluster->eta())> m_etaCut || fabs(theTrackParticle->eta())> m_etaCut) ) { // cut in eta for the cluster and the track 
    electronisgood = false;
    (*m_msgStream) << MSG::DEBUG << "   -- electron fails eta cut  -- cluster_eta= " << cluster->eta() << endmsg;
  }

  if (electronisgood) {
    // store this electron
    m_pxElTrackList.push_back(theTrackParticle);

    (*m_msgStream) << MSG::DEBUG << " * RecordElectron * good electron found -> store this electron with pt " << theTrackParticle->pt() 
		   << "  --> current m_pxElTrackList.size(): " << m_pxElTrackList.size()
		   << std::endl;
  }

  return electronisgood;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                      
void ElectronSelector::Clear()
{
  m_pxElTrackList.clear();
  m_goodElecNegTrackParticleList.clear();
  m_goodElecPosTrackParticleList.clear();
 
  // -1 means not assigned
  m_elecneg1 = -1;
  m_elecneg2 = -1;
  m_elecpos1 = -1;
  m_elecpos2 = -1;

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                      
bool ElectronSelector::OrderElectronList()
{
  (*m_msgStream) << MSG::DEBUG << " -- ElectronSelector::OrderElectronList -- START  -- list size: " << m_pxElTrackList.size( ) << endmsg;
  
  bool goodlist = true;

  if (m_pxElTrackList.size() >= 2) { // we need at least 2 electrons
    double ptMinus1 = 0.;
    double ptMinus2 = 0.;
    double ptPlus1  = 0.;
    double ptPlus2  = 0.;

    int elecnegcount = 0;
    int elecposcount = 0;

    for (int ielec=0; ielec < (int) m_pxElTrackList.size(); ielec++) {
      std::cout <<" -- ElectronSelector::OrderElectronList -- elec--> pt "<< m_pxElTrackList.at(ielec)->pt() << std::endl;
      std::cout <<"                                                   d0 "<< m_pxElTrackList.at(ielec)->d0() << std::endl;
      std::cout <<"                                             sigma_d0 "<< m_pxElTrackList.at(ielec)->definingParametersCovMatrixVec()[0] << std::endl;

      // negative electrons
      if (m_pxElTrackList.at(ielec)->charge()== -1) { // positive electron
	elecnegcount++;
	if (m_pxElTrackList.at(ielec)->pt()> ptMinus1) {
	  // store 1st in 2nd
	  ptMinus2   =  ptMinus1;
	  m_elecneg2 = m_elecneg1;
	  // now store the new one in 1st place
	  ptMinus1   = m_pxElTrackList.at(ielec)->pt();
	  m_elecneg1 = ielec;	
	} 
	else if (m_pxElTrackList.at(ielec)->pt()> ptMinus2) {
	  // store the new one in 2nd place
	  ptMinus2   = m_pxElTrackList.at(ielec)->pt();
	  m_elecneg2 = ielec;
	}
      }
      // positive electrons
      if (m_pxElTrackList.at(ielec)->charge()==  1) { // positive electron
	elecposcount++;
	if (m_pxElTrackList.at(ielec)->pt()> ptPlus1) {
	  // store 1st in 2nd
	  ptPlus2   =  ptPlus1;
	  m_elecpos2 = m_elecpos1;
	  // now store the new one in 1st place
	  ptPlus1   = m_pxElTrackList.at(ielec)->pt();
	  m_elecpos1 = ielec;	
	} 
	else if (m_pxElTrackList.at(ielec)->pt()> ptPlus2) {
	  // store the new one in 2nd place
	  ptPlus2   = m_pxElTrackList.at(ielec)->pt();
	  m_elecpos2 = ielec;
	}
      }
    }

    if (elecposcount == 0 || elecnegcount == 0) {
      // We need at least one e- and one e+
      if (m_doDebug) std::cout << " -- ElectronSelector::OrderElectronList -- No opposite charge electrons --> DISCARD ALL ELECTRONS -- " << std::endl;
      elecposcount = 0;
      elecnegcount = 0;
      this->Clear();
      goodlist = false;
    }

    if (m_doDebug && elecposcount + elecnegcount >= 2 ){ 
      std::cout << " -- ElectronSelector::OrderElectronList -- electron summary list taking " << elecposcount + elecnegcount 
		<< "  electrons from the input list of " << m_pxElTrackList.size() << " electrons: " << std::endl;
      if (m_elecneg1 >= 0) std::cout << "                                leading e-: " << m_elecneg1 << "   Pt = " << ptMinus1 << std::endl;
      if (m_elecneg2 >= 0) std::cout << "                                second  e-: " << m_elecneg2 << "   Pt = " << ptMinus2 << std::endl;
      if (m_elecpos1 >= 0) std::cout << "                                leading e+: " << m_elecpos1 << "   Pt = " << ptPlus1 << std::endl;
      if (m_elecpos2 >= 0) std::cout << "                                second  e+: " << m_elecpos2 << "   Pt = " << ptPlus2 << std::endl;
    }

    if (elecposcount + elecnegcount >= 2){ // fill the final list of electrons
      if (m_elecneg1 >= 0) m_goodElecNegTrackParticleList.push_back(m_pxElTrackList.at(m_elecneg1));
      if (m_elecneg2 >= 0) m_goodElecNegTrackParticleList.push_back(m_pxElTrackList.at(m_elecneg2));
      if (m_elecpos1 >= 0) m_goodElecPosTrackParticleList.push_back(m_pxElTrackList.at(m_elecpos1));
      if (m_elecpos2 >= 0) m_goodElecPosTrackParticleList.push_back(m_pxElTrackList.at(m_elecpos2));
    }
  }

  (*m_msgStream) << MSG::DEBUG << " -- ElectronSelector::OrderElectronList -- COMPLETED  -- status: "<< goodlist << std::endl;
  return goodlist;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                      
bool ElectronSelector::RetrieveVertices ()
{
  if (m_doDebug) std::cout << " -- ElectronSelector::RetrieveVertices -- START  -- list size: " 
			   << m_goodElecNegTrackParticleList.size() + m_goodElecPosTrackParticleList.size() 
			   << std::endl;
  bool goodvertices = false; 
  int nverticesfound = 1; // WARNING default must be 0 --> set to 1 for R22 --> needs to be fixed

  if (m_goodElecNegTrackParticleList.size() >= 1 && m_goodElecPosTrackParticleList.size() >= 1) { // we need at least 1 e- and 1 e+
    // then, check the distances between the e- and e+ vertices, and make sure at least 1 pair comes from same vertex
    for (size_t ielec = 0; ielec < m_goodElecNegTrackParticleList.size(); ielec++) {
      // loop on e-
      // R21 -> R22 SALVA // TrkParticles have no vertex in R22 --> THIS NEEDS A FIX
      /*
      if (m_goodElecNegTrackParticleList.at(ielec)->vertex()) {
	if (m_doDebug) std::cout << "     e-(" << ielec <<")->vertex()->v= (" << m_goodElecNegTrackParticleList.at(ielec)->vertex()->x()
				 << ", " << m_goodElecNegTrackParticleList.at(ielec)->vertex()->y()
				 << ", " << m_goodElecNegTrackParticleList.at(ielec)->vertex()->z()
				 << ") " << std::endl;

	// for (unsigned int iposi = 0; iposi < m_goodElecPosTrackParticleList.size(); iposi++) {
	for (size_t iposi = 0; iposi < m_goodElecPosTrackParticleList.size(); iposi++) {
	  if (m_goodElecPosTrackParticleList.at(iposi)->vertex()) {
	    if (m_doDebug) std::cout << "     e+(" << iposi <<")->vertex()->v= (" << m_goodElecPosTrackParticleList.at(iposi)->vertex()->x()
				     << ", " << m_goodElecPosTrackParticleList.at(iposi)->vertex()->y()
				     << ", " << m_goodElecPosTrackParticleList.at(iposi)->vertex()->z()
				     << ") " << std::endl;
	    float delta_x = fabs( m_goodElecNegTrackParticleList.at(ielec)->vertex()->x()-m_goodElecPosTrackParticleList.at(iposi)->vertex()->x() );
	    float delta_y = fabs( m_goodElecNegTrackParticleList.at(ielec)->vertex()->y()-m_goodElecPosTrackParticleList.at(iposi)->vertex()->y() );
	    float delta_z = fabs( m_goodElecNegTrackParticleList.at(ielec)->vertex()->z()-m_goodElecPosTrackParticleList.at(iposi)->vertex()->z() );

	    if (delta_x < m_deltaXYcut && delta_y < m_deltaXYcut && delta_z < m_deltaZcut) {
	      nverticesfound++;
	      if (m_doDebug) std::cout << "     ELEC-BINGO !!! e+e- pair in same vertex !!! e-[" << ielec 
				       << "]  e+[" << iposi<< "]   count: " << nverticesfound << std::endl;
	    } // vertex is the same
	  } // positron has vertex
	} // loop on positrons
      } // electron has vertex
      */
    } // loop on electrons (e-) 
  } // at least one e+e- pair

  if (nverticesfound >= 1) goodvertices = true;

  if (m_doDebug) std::cout << " -- ElectronSelector::RetrieveVertices -- COMPLETED -- status: " << goodvertices << std::endl; 
  return goodvertices;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                      
const xAOD::TrackParticle* ElectronSelector::GetElecNegTrackParticle (size_t i) 
{
  if (i >= m_goodElecNegTrackParticleList.size()) { // requesting out of range electron
    return nullptr;
  } 
  return m_goodElecNegTrackParticleList.at(i);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                      
const xAOD::TrackParticle* ElectronSelector::GetElecPosTrackParticle (size_t i) 
{
  if (i >=  m_goodElecPosTrackParticleList.size()) { // requesting out of range electron
    return nullptr;
  } 
  return m_goodElecPosTrackParticleList.at(i);
}
