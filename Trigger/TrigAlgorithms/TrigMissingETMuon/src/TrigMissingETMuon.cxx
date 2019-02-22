/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigMissingETMuon.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigMissingETMuon
//
// AUTHOR:   Hongtao Yang
// CREATED:  Nov 18, 2011
//
// Xin Chen: Add muon pt cut (>5 GeV) to be consistent with offline 
//           and remove double counting of the same muComb muon (Apr. 24, 2012)
//
// Description: MissingET AllTEAlgo (FEX) algorithm. Adds
//              L2 or EF muon correction
//
// ********************************************************************

#include "TrigMissingETMuon/TrigMissingETMuon.h"

#include "xAODMuon/MuonContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigNavigation/Navigation.h"
#include "CxxUtils/sincos.h"
#include "CxxUtils/sincosf.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include <cmath>
#include <cstdio>

TrigMissingETMuon::TrigMissingETMuon(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_useCachedResult(false),
    m_cachedTE(0)
{

  declareProperty("METLabel", m_featureLabel = "T2MissingET", "label for the MET feature in the HLT Navigation");
  declareProperty("MuonPtCut", m_muonptcut = 5.0, "Muon Pt threshold");

  declareMonitoredVariable("Muon_Ex",     m_mu_ex);
  declareMonitoredVariable("Muon_Ey",     m_mu_ey);
  declareMonitoredVariable("Muon_SumEt",    m_mu_set);

  declareMonitoredVariable("Muon_Ex_log",      m_mu_ex_log);
  declareMonitoredVariable("Muon_Ey_log",      m_mu_ey_log);
  declareMonitoredVariable("Muon_SumEt_log",    m_mu_set_log);

  /** definition of the meaning for the component flag bits **/
  m_maskErrMuon            = 0x0004; // bit  2
  m_maskCompErrors         = 0x8000; // bit 15
  //   m_maskGlobErrors           = 0x80000000; // bit 31
  m_met=NULL;
}


//////////////////////////////////////////////////////////
HLT::ErrorCode TrigMissingETMuon::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{
  // CACHING
  // first check whether we executed this instance before:
  if (m_useCachedResult) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Executing this xAOD::TrigMissingETMuon " << name() << " in cached mode" << endmsg;
    }
    // Only count MET as an input TE (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );
    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);

    // save (cached) met feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;
  }

  // event status flag
  int flag=0;

  // start monitoring
  beforeExecMonitors().ignore();

  m_mu_ex = -9e9;
  m_mu_ey = -9e9;
  m_mu_set = -9e9;

  m_mu_ex_log = -9e9;
  m_mu_ey_log = -9e9;
  m_mu_set_log = -9e9;

  unsigned int tes_in_size=tes_in.size(); // = 2 (seeded) or 1 (unseeded). The TrigMissingET object is necessary.
  unsigned int muonIndex=0;               // = 1 if seeded
  unsigned int tes_in0_size=0;  // size of result (must be 1)
  unsigned int tes_in1_size=0;  // size of muon container

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing xAOD::TrigMissingETMuon::hltExecute()" << endmsg;
  }

  if(msgLvl() <= MSG::WARNING) {
    msg() << MSG::WARNING << "Executing xAOD::TrigMissingETMuon::hltExecute()" << endmsg;
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST: tes_in.size() = " << tes_in_size << endmsg;

    for (unsigned u=0; u<tes_in_size; ++u) {
      msg() << MSG::DEBUG << "REGTEST: tes_in[" << u << "].size() = " << tes_in[u].size() << endmsg;
    }
  }
  if(msgLvl() <= MSG::WARNING) {
    msg() << MSG::WARNING << "tes_in_size" << tes_in_size << endmsg;
  }
  //algorithm is muon seeded, therefore:
  //    algorithm has TrigMissingET as only seed if in unseeded mode
  //                  muon as second seed of in seeded mode  
  //so number of TEs must be 1 or 2 
  // ======================================================
  switch (tes_in_size) {

  case 1: // unseeded mode with TrigMissingET.

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Running in unseeded mode" << endmsg;
      msg() << MSG::DEBUG
	    << "No muon feature found: skipping muon correction" << endmsg;
    }
    tes_in0_size=tes_in[0].size();
    if (tes_in0_size != 1) {
      msg() << MSG::WARNING // ERROR
	    << "Configuration error: expecting exactly 1 TrigMissingET object.  Aborting chain" << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      std::vector<const xAOD::TrigMissingET*> vectorOfMET;
      for (HLT::TEVec::const_iterator met_it = tes_in[0].begin(); met_it != tes_in[0].end(); ++met_it) {
      	HLT::ErrorCode status = getFeatures( (*met_it) , vectorOfMET);
      	if(status != HLT::OK) {
      	  msg() << MSG::ERROR 
      		<< "no TrigMissingET object found for this TE ... ?!?"
      		<< endmsg;
      	  return HLT::NAV_ERROR;
      	}
      }
      if (msgLvl() <= MSG::WARNING) {
      	msg() << MSG::WARNING << "REGTEST: No muon input. Will save the input TE without updating it " << endmsg;
      }

      // Check if EDM is there
      if(vectorOfMET.size() == 0) 
        return HLT::NAV_ERROR;      
      
      const xAOD::TrigMissingET *met_temp=vectorOfMET[0];
      m_met = const_cast<xAOD::TrigMissingET *>(met_temp);      
      HLT::ErrorCode status = makeOutputTE(tes_in, type_out);
      return status;
    }
  case 2:			// Seeded mode
    muonIndex=1;

    tes_in0_size=tes_in[0].size();
    tes_in1_size=tes_in[1].size();

    if (tes_in0_size != 1) {
      msg() << MSG::WARNING // ERROR
	    << "Configuration error: expecting exactly 1 L2 or EF result.  Aborting chain" << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
    }
    if (tes_in1_size == 0) {
      if (msgLvl() <= MSG::DEBUG){
      	msg() << MSG::DEBUG
	      << "No muon feature found: skipping muon correction" << endmsg;
      }

      // Save ouput TE before leaving.
      std::vector<const xAOD::TrigMissingET*> vectorOfMET;
      for (HLT::TEVec::const_iterator met_it = tes_in[0].begin(); met_it != tes_in[0].end(); ++met_it) {
      	HLT::ErrorCode status = getFeatures( (*met_it) , vectorOfMET);
      	if(status != HLT::OK) {
      	  msg() << MSG::ERROR 
      		<< "no TrigMissingET object found for this TE ... ?!?"
      		<< endmsg;
      	  return HLT::NAV_ERROR;
      	}
      }

      // Check if EDM is there
      if(vectorOfMET.size() == 0) 
        return HLT::NAV_ERROR;
   	  
      const xAOD::TrigMissingET *met_temp=vectorOfMET[0];
      m_met = const_cast<xAOD::TrigMissingET *>(met_temp);      
      HLT::ErrorCode status = makeOutputTE(tes_in, type_out);
      return status;
    }
    break;

  default:
    msg() << MSG::WARNING	// ERROR
	  << "Configuration error: tes_in.size() is " << tes_in_size
	  << " but can only be 1 in seeded mode or 0 in unseeded mode.  Aborting chain" << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
  }

  // First we retrieve the TrigMissingET object
  std::vector<const xAOD::TrigMissingET*> vectorOfMET;
  for (HLT::TEVec::const_iterator met_it = tes_in[0].begin(); met_it != tes_in[0].end(); ++met_it) {
    HLT::ErrorCode status = getFeatures( (*met_it) , vectorOfMET);
    if(status != HLT::OK) {
      msg() << MSG::ERROR 
	    << "no TrigMissingET object found for this TE ... ?!?"
	    << endmsg;
      return HLT::NAV_ERROR;
    }
  }

  // Check if EDM is there
  if(vectorOfMET.size() == 0) 
    return HLT::NAV_ERROR;

  // ++++++++++++++++++++++++++++++++++++++++++
  const xAOD::TrigMissingET *met_temp=vectorOfMET[0];
  m_met = const_cast<xAOD::TrigMissingET *>(met_temp);
  // ++++++++++++++++++++++++++++++++++++++++++
  msg() << MSG::WARNING << "xAOD::TrigMissingET object retrieved." <<endmsg;

  flag=m_met->flag();
  msg() << MSG::WARNING << "Flag got." <<endmsg;

  // Now we loop over all the muons
  float muon_ex=0;
  float muon_ey=0;
  float muon_ez=0;
  float muon_sum_et=0;
  float muon_sum_e=0;
  //int muon_charge=0;
  unsigned int Nmuons=0;
  bool MuonError=false;

  msg() << MSG::WARNING << "REGTEST: Ready to access xAOD::MuonContainer  muons " << endmsg;

  std::vector<const xAOD::TrackParticle*> vecOfMuonTrk;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG
        << "REGTEST: Using muon pt threshold "<<m_muonptcut<<" GeV."<< endmsg;
  }

  for (HLT::TEVec::const_iterator muon_it = tes_in[muonIndex].begin();muon_it != tes_in[muonIndex].end(); ++muon_it) 
  {

    // Get the muon container linked to the TE
    const xAOD::MuonContainer* muonContainer(0);

    HLT::ErrorCode status = getFeature( (*muon_it) , muonContainer);
  
    if(status != HLT::OK || muonContainer==0) {
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG
              << "no MuonContainer found for this TE ... ?!?"
              << endmsg;
      }
      continue;
    }

    int NofMuons=muonContainer->size();

    // Check that there is at least one muon in TrigMuonEFInfoContainer
    if (NofMuons == 0){
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Size of vectorOfMuons is 0. Skipping muon correction" << endmsg;
      }
      MuonError=true;
      continue;
      //       return HLT::NAV_ERROR;
    }
    unsigned int kk=0;
    for(auto muon : *muonContainer) {
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Looking at muon " << kk << "pt = " << muon->pt() << "eta= " << 
                              muon->eta() << "phi= " << muon->phi() << endmsg;
      }
      const xAOD::Muon::MuonType muontype = muon->muonType();
      if(muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) { 
        // combined or segment tagged muon

        // double counting removal and minimum pt cut
        bool sel = true;
        float Et = fabs(muon->pt());
        if (Et/CLHEP::GeV < m_muonptcut) sel = false;
        if(!sel) continue;

        const xAOD::TrackParticle* idtrk = muon->trackParticle( xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle );

        if(idtrk==0) {sel = false;} // Reject muons without IDtracks

        if(!sel) continue;

        for(std::vector<const xAOD::TrackParticle*>::size_type idx=0; idx<vecOfMuonTrk.size(); idx++) {
          const xAOD::TrackParticle* idtrk_sel = vecOfMuonTrk[idx];
          //if((*idtrk) == (*idtrk_sel)) { //  == operator not defined for this class, so comment it out 
          if((idtrk) == (idtrk_sel)) {
            sel = false;
            break;
          }
        } //end of loop over stored muon tracks

        if(!sel) continue;

        vecOfMuonTrk.push_back(idtrk);  // store for testing future muon tracks for double counting prevention

        //int charge = muon->Charge()>0 ? 1 : -1;
        float eta = muon->eta();
        float phi = muon->phi();
        float cosPhi, sinPhi;
        sincosf(phi, &sinPhi, &cosPhi);
        float Ex = Et*cosPhi;
        float Ey = Et*sinPhi;
        float Ez = Et*sinhf(eta);
        float E = sqrtf(Et*Et + Ez*Ez);

        muon_ex     -= Ex;
        muon_ey     -= Ey;
        muon_ez     -= Ez;
        muon_sum_et += Et;
        muon_sum_e  += E;
        //muon_charge += charge;
        ++Nmuons;

        if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "REGTEST: Adding muon with pT = " << Et
                << " MeV, phi = " << phi << " rad, eta = " << eta << endmsg;
        }

      } // end muon-type if statement
    } // end loop over muons
  } // end loop over muons te's

  m_mu_ex = muon_ex;
  m_mu_ey = muon_ey;
  m_mu_set = muon_sum_et;
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Muon correction calculated." << endmsg;
  }

  if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Setting energies and flag" << endmsg;
  unsigned int muonComp = m_met->getNumberOfComponents() - 1;
  // Suggested by Diego: Checking the component name of the input
  msg() << MSG::WARNING << "MET component number: " << muonComp << endmsg;
  msg() << MSG::WARNING << "MET component name: " << m_met->nameOfComponent(muonComp) << endmsg;
  if( (m_met->nameOfComponent(muonComp)).substr(0,4)!="Muon") {
    msg() << MSG::ERROR << "Could not find MET container with muon information!!! Exit..." << endmsg;
    msg() << MSG::ERROR << "MET component number: " << muonComp << endmsg;
    msg() << MSG::ERROR << "MET component name: " << m_met->nameOfComponent(muonComp) << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);    
  }
  // Fetch Muon Components
  // Set muon variables
  m_met->setExComponent(muonComp,muon_ex);
  m_met->setEyComponent(muonComp,muon_ey);
  m_met->setEzComponent(muonComp,muon_ez);
  m_met->setSumEtComponent(muonComp,muon_sum_et);
  m_met->setSumEComponent(muonComp,muon_sum_e);
  m_met->setUsedChannelsComponent(muonComp, Nmuons);

  if (MuonError){ // set muon error bit 
    flag |= m_maskErrMuon; flag |= m_maskCompErrors;
  }
  m_met->setFlag(flag);
  if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Setting energies and flag done." << endmsg;

  if (msgLvl() <= MSG::DEBUG) {
    char buff[128];
    std::snprintf(buff,128,"REGTEST: (Muon FEX) Event status = 0x%08x", (unsigned)flag);
    msg() << MSG::DEBUG << buff << endmsg;
    msg() << MSG::DEBUG << "REGTEST: (Muon FEX) Nmuons = " << Nmuons << endmsg;
    if (Nmuons>0) {
      msg() << MSG::DEBUG << "REGTEST: (Muon FEX) muon_ex = " << muon_ex*1e-3 << " GeV" << endmsg;
      msg() << MSG::DEBUG << "REGTEST: (Muon FEX) muon_ey = " << muon_ey*1e-3 << " GeV" << endmsg;
      msg() << MSG::DEBUG << "REGTEST: (Muon FEX) muon_ez = " << muon_ez*1e-3 << " GeV" << endmsg;
      msg() << MSG::DEBUG << "REGTEST: (Muon FEX) muon_sum_et = " << muon_sum_et*1e-3 << " GeV" << endmsg;
      msg() << MSG::DEBUG << "REGTEST: (Muon FEX) muon_sum_e = " << muon_sum_e*1e-3 << " GeV" << endmsg;
    }
  }

  // monitoring: log-scale quantities
  float epsilon = 1e-6;  // 1 keV

  if (fabsf(m_mu_ex)>epsilon)
    m_mu_ex_log = copysign(log10(fabsf(m_mu_ex)), m_mu_ex);
  else
    m_mu_ex_log = 0;

  if (fabsf(m_mu_ey)>epsilon)
    m_mu_ey_log = copysign(log10(fabsf(m_mu_ey)), m_mu_ey);
  else
    m_mu_ey_log = 0;

  if (fabsf(m_mu_set)>epsilon)
    m_mu_set_log = copysign(log10(fabsf(m_mu_set)), m_mu_set);
  else
    m_mu_set_log = 0;

  HLT::ErrorCode status = makeOutputTE(tes_in, type_out);
  if(status != HLT::OK) return status;
  // stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;
}

//////////////////////////////////////////////////////////
HLT::ErrorCode TrigMissingETMuon::makeOutputTE(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out) {

  // Only count MET as an input TE (for seeding relation of navigation structure)
  HLT::TEVec allTEs;
  if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );

  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  HLT::ErrorCode status = HLT::OK;
  m_config->getNavigation()->copyAllFeatures(tes_in[0][0], outputTE);
  if (status != HLT::OK){
    msg() << MSG::ERROR
	  << "Write of xAOD::TrigMissingET feature into outputTE failed"
	  << endmsg;
    return status;
  }

  // Some debug output:
  if(msgLvl() <= MSG::DEBUG){
    msg() << MSG::DEBUG
	  << "We assume success, set TE with label "
	  << " active to signal positive result."
	  << endmsg;
  }

  // CACHING
  // if we got here, everything was okay. so, we cache the feature for further execution of this instance in e.g. other MET Sequences:
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Updated feature copied to output TE." << endmsg;
  }
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  return HLT::OK;
}

