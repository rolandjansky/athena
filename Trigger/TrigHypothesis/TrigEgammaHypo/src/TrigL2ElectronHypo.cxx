// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronHypo.cxx
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after 
 **                tracking algorithms: creates TrigElectrons
 **
 **   Author: R.Goncalo <r.goncalo@rhul.ac.uk> Thu Jan 20 22:02:43 BST 2005
 **           P.Urquijo <Phillip.Urquijo@cern.ch>
 **
 **   Created:      Sat Mar  1 19:55:56 GMT 2005
 **   Modified:     RG 18 Mar 06 - fix to always generate TrigElectrons
 **                              - use key2keyStore for collection names
 **                 N.Berger Dec.06 - migrate to new steering (RG)
 **   Modified:     R. White Feb 17 - cleanup and remove eta dependence
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2ElectronHypo.h"
#include "xAODTrigger/TrigPassBits.h"

class ISvcLocator;

template <class SRC>
inline const DataVector<xAOD::TrigElectron>** dvec_cast(SRC** ptr) { 
  return (const DataVector<xAOD::TrigElectron>**)(ptr); 
} 

TrigL2ElectronHypo::TrigL2ElectronHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty( "AcceptAll",            m_acceptAll  = true );
  declareProperty( "TrackPt",              m_trackPtthr = 5.0*CLHEP::GeV );
  declareProperty( "CaloTrackdETA",        m_calotrackdeta ); //loose cut
  declareProperty( "CaloTrackdPHI",        m_calotrackdphi ); //loose cut
  declareProperty( "CaloTrackdEoverPLow",  m_calotrackdeoverp_low );
  declareProperty( "CaloTrackdEoverPHigh", m_calotrackdeoverp_high );
  declareProperty( "TRTRatio", m_trtratio);

  declareMonitoredVariable("CutCounter",m_cutCounter);
  declareMonitoredCollection("PtCalo",*dvec_cast(&m_egamma_container),&TrigL2ElectronHypo::getCaloPt);
  declareMonitoredCollection("PtTrack",*dvec_cast(&m_egamma_container),&TrigL2ElectronHypo::getTkPt);
  declareMonitoredCollection("CaloTrackdEta",*dvec_cast(&m_egamma_container),&xAOD::TrigElectron::trkClusDeta);
  declareMonitoredCollection("CaloTrackdPhi",*dvec_cast(&m_egamma_container),&xAOD::TrigElectron::trkClusDphi);
  declareMonitoredCollection("CaloTrackEoverP",*dvec_cast(&m_egamma_container),&xAOD::TrigElectron::etOverPt);
  declareMonitoredCollection("CaloEta",*dvec_cast(&m_egamma_container),&xAOD::TrigElectron::caloEta);
  declareMonitoredCollection("CaloPhi",*dvec_cast(&m_egamma_container),&xAOD::TrigElectron::caloPhi);
  m_egamma_container = nullptr;
}

TrigL2ElectronHypo::~TrigL2ElectronHypo()
{ }

HLT::ErrorCode TrigL2ElectronHypo::hltInitialize()
{
    ATH_MSG_DEBUG("Initialization:" );

    ATH_MSG_DEBUG("Initialization completed successfully:" );
    ATH_MSG_DEBUG("AcceptAll            = " 
	<< (m_acceptAll==true ? "True" : "False") ); 
    ATH_MSG_DEBUG("TrackPt              = " << m_trackPtthr           ); 
    ATH_MSG_DEBUG("CaloTrackdETA        = " << m_calotrackdeta        );
    ATH_MSG_DEBUG("CaloTrackdPHI        = " << m_calotrackdphi        );
    ATH_MSG_DEBUG("CaloTrackdEoverPLow  = " << m_calotrackdeoverp_low );
    ATH_MSG_DEBUG("CaloTrackdEoverPHigh = " << m_calotrackdeoverp_high);
    ATH_MSG_DEBUG("TRTRatio = " << m_trtratio);
  
  return HLT::OK;
}

HLT::ErrorCode TrigL2ElectronHypo::hltFinalize()
{
    ATH_MSG_INFO("in finalize()" );
    return HLT::OK;
}

HLT::ErrorCode TrigL2ElectronHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
    // initialize monitoring counter
    m_cutCounter=-1;
    m_egamma_container = 0;

    bool hasInput=false;
    bool hasContainer=false;
    // Accept-All mode
    // Allows algorithm to run
    if (m_acceptAll) {
        pass = true;
        ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
    } else {
        pass = false;
        ATH_MSG_DEBUG("AcceptAll property not set: applying selection");
    }

    // get RoI descriptor
    const TrigRoiDescriptor* roiDescriptor = 0;
    if (getFeature(outputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

    if ( !roiDescriptor ) {
        ATH_MSG_WARNING("No RoI for this Trigger Element! " );
        return HLT::NAV_ERROR;
    }

    ATH_MSG_DEBUG("Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId() 
            << "; RoI = "   << *roiDescriptor);

    // get TrigElectrons from the steering
    const xAOD::TrigElectronContainer* trigElecColl = 0;
    HLT::ErrorCode stat = getFeature(outputTE, trigElecColl, "L2ElectronFex");

    if ( stat != HLT::OK || trigElecColl == 0) {
        if ( msgLvl() <= MSG::DEBUG) {
            ATH_MSG_DEBUG("Failed to get TrigElectron collection" );
        }
        return HLT::OK;
    }

    hasInput=true;
    ATH_MSG_DEBUG("Got collection with " << trigElecColl->size() 
            << " TrigElectrons" );

    // if no electrons were found, just leave TrigElectronColl. empty and leave
    if ( trigElecColl->size() == 0 ) {
        ATH_MSG_DEBUG("No electrons to analyse, leaving!" );
        hasContainer=false; 
    }
    else { 
        hasContainer=true;
    }
    // initialize counter after all error conditions checked
    m_egamma_container = trigElecColl;
    m_cutCounter=0;
    bool pTcaloCut=false;
    bool dEtaCaloCut=false;
    bool dPhiCaloCut=false;
    bool eTOverPtCut_lo=false;
    bool eTOverPtCut_hi=false;
    bool TRTRatioCut=false;

    // generate TrigPassBits mask to flag which TrigElectrons pass hypo cuts
    std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::TrigElectronContainer>(trigElecColl);

    // Now loop over electrons, see if at least one passes all cuts
    xAOD::TrigElectronContainer::const_iterator elecIter, elecEnd=trigElecColl->end();
    for (elecIter = trigElecColl->begin(); elecIter != elecEnd; ++elecIter) {    

        if(m_acceptAll){
            xBits->markPassing((*elecIter),trigElecColl,true);
            continue;
        }
        const xAOD::TrackParticle* trkIter = (*elecIter)-> trackParticle();
        if (trkIter==NULL) continue; // disconsider candidates without track

        // Retrieve all quantities
        float dPhiCalo    = (*elecIter)->trkClusDphi();
        float dEtaCalo    = (*elecIter)->trkClusDeta();
        float pTcalo      = (*elecIter)->pt();	  
        float eTOverPt    = (*elecIter)->etOverPt();	  
        //    int   trackIndx   = (*elecIter)->trackIndx();
        float NTRHits     = (float)((*elecIter)->nTRTHits());
        float NStrawHits  = (float)((*elecIter)->nTRTHiThresholdHits());
        float TRTHitRatio = NStrawHits==0 ? 1e10 : NTRHits/NStrawHits;

        // apply cuts 

        if (pTcalo < m_trackPtthr){ 
            ATH_MSG_VERBOSE("Fails pt cut");
            continue;
        }
        pTcaloCut=true;
        if (dEtaCalo > m_calotrackdeta) {
            ATH_MSG_VERBOSE("Fails dEta cut");
            continue;
        }
        dEtaCaloCut=true;
        if (dPhiCalo > m_calotrackdphi) {
            ATH_MSG_VERBOSE("Fails dPhi cut");
            continue;
        }
        dPhiCaloCut=true;
        if(eTOverPt <  m_calotrackdeoverp_low ) {
            ATH_MSG_VERBOSE("Fails eoverp low cut");
            continue;
        }
        eTOverPtCut_lo = true;  
        if ( eTOverPt > m_calotrackdeoverp_high ) {
            ATH_MSG_VERBOSE("Fails eoverp high cut");
            continue;
        }
        eTOverPtCut_hi = true;   
        if (TRTHitRatio < m_trtratio){
            ATH_MSG_VERBOSE("Fails TRT cut");
            continue;
        }
        TRTRatioCut = true;

        // TrigElectron passed all cuts: set flags
        pass = true;  
        xBits->markPassing((*elecIter),trigElecColl,true);

        ATH_MSG_DEBUG("Event accepted !" );	      
    } // end of loop over electrons

    m_cutCounter=hasInput+hasContainer+pTcaloCut+dEtaCaloCut+dPhiCaloCut+eTOverPtCut_lo+eTOverPtCut_hi+TRTRatioCut;
    // store TrigPassBits result
    if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
        ATH_MSG_ERROR("Could not store TrigPassBits! ");

    return HLT::OK;
}

