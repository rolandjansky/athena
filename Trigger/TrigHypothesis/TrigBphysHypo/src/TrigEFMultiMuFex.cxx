// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2MultiMuFex.cxx
 **
 **   Description: EF hypothesis algorithms for 3mu and 4mu signatures  
 **             
 **************************************************************************/ 
 
#include "TrigEFMultiMuFex.h"
#include "TrigBphysHelperUtilsTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"


TrigEFMultiMuFex::TrigEFMultiMuFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator)
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
,m_BmmHypTot(0),m_BmmHypVtx(0)
,m_expectNumberOfInputTE(3)
,m_oppositeCharge(true)
,m_NMassMuon(2)
,m_lowerMassCut(2000.)
,m_upperMassCut(10000.)
,m_ApplyupperMassCut(true)
,m_checkNinputTE(true)
,m_muonAlgo("TrigMuSuperEF")
,m_acceptAll(false)
//counters
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
,m_countPassedmumuPairsEv(0)
,m_countPassedBsMassEv(0)
,m_countPassedVtxFitEv(0)
,m_countPassedmumuPairs2R(0)
,m_countPassedBsMass2R(0)
,m_countPassedVtxFit2R(0)
,m_massMuon(105.6583715)
{

  // Read cuts

  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("NInputMuon"     , m_expectNumberOfInputTE    = 3 );
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("NMassMuon"     , m_NMassMuon    = 2 );
  declareProperty("LowerMassCut", m_lowerMassCut=2000.0);
  declareProperty("UpperMassCut", m_upperMassCut=10000.0);
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
  declareProperty("MuonAlgo", m_muonAlgo="TrigMuSuperEF");
  declareProperty("CheckNinputTE", m_checkNinputTE=true);
  
  declareMonitoredStdContainer("Errors"                 , m_mon_Errors                  , AutoClear);
  declareMonitoredStdContainer("Acceptance"             , m_mon_Acceptance              , AutoClear);
  declareMonitoredStdContainer("pTMu1",        m_mon_mu1pT       , AutoClear   );
  declareMonitoredStdContainer("pTMu2",        m_mon_mu2pT       , AutoClear  );
  declareMonitoredStdContainer("MuMumass",     m_mon_MuMumass    , AutoClear  );
  declareMonitoredStdContainer("FitMass",      m_mon_FitMass     , AutoClear );
  declareMonitoredStdContainer("VtxChi2",      m_mon_Chi2        , AutoClear  );
 
}

TrigEFMultiMuFex::~TrigEFMultiMuFex()
{ }

HLT::ErrorCode TrigEFMultiMuFex::hltInitialize()
{
    ATH_MSG_DEBUG("Initialization ..." );
    ATH_MSG_DEBUG("AcceptAll            = "  << (m_acceptAll==true ? "True" : "False") );
    ATH_MSG_DEBUG("Number of input muons expected   = " << m_expectNumberOfInputTE );
    ATH_MSG_DEBUG("Number of muons used for mass   = " << m_NMassMuon );
    ATH_MSG_DEBUG("OppositeCharge       = "  << (m_oppositeCharge==true ? "True" : "False") );
    ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut );
    ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut );
    ATH_MSG_DEBUG("check for number of input TEs is " << (m_checkNinputTE!=0 ? "ENABLED" : "DISABLED") );

    if ( timerSvc() ) {
        m_BmmHypTot = addTimer("EFBmmHypTot");
        m_BmmHypVtx = addTimer("EFBmmHypVtxFit");
    }
    if (m_muonAlgo != "TrigMuSuperEF" ) {
        ATH_MSG_INFO(" Expected algorithm name:  TrigMuSuperEF, but got: " << m_muonAlgo );
        return HLT::BAD_JOB_SETUP;
    }
    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't find TrigBphysHelperUtilsTool" );
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("TrigBphysHelperUtilsTool found" );
    }

    
  m_lastEvent=-1;
  m_lastEventPassed=-1;
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedmumuPairsEv=0;
  m_countPassedBsMassEv=0;
  m_countPassedVtxFitEv=0;
  m_countPassedmumuPairs2R=0;
  m_countPassedBsMass2R=0;
  m_countPassedVtxFit2R=0;

  return HLT::OK;
}

HLT::ErrorCode TrigEFMultiMuFex::hltFinalize()
{
  ATH_MSG_INFO("in finalize()" );
  MsgStream log(msgSvc(), name());
  
  ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigEFMultiMuFex -------------|" );
  ATH_MSG_INFO("Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI );
  ATH_MSG_INFO("Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs );
  ATH_MSG_INFO("Passed MuMu pairs:events/2xRoIs "  << m_countPassedmumuPairsEv<<"/"<<m_countPassedmumuPairs2R );
  ATH_MSG_INFO("Passed BsMass: events/2xRoIs "<< m_countPassedBsMassEv<<"/"<<m_countPassedBsMass2R);
  ATH_MSG_INFO("Passed Vtx Fit: events/2xRoIs "<<m_countPassedVtxFitEv<< "/"<<m_countPassedVtxFit2R);

  return HLT::OK;
}

// Define the bins for error-monitoring histogram
#define ERROR_No_EventInfo               0
#define ERROR_WrongNum_Input_TE          1
#define ERROR_GetMuonFailed              2
#define ERROR_AddTrack_Fails             3
#define ERROR_CalcInvMass_Fails          4
#define ERROR_BphysColl_Fails            5

// Define the bins for acceptance-monitoring histogram
#define ACCEPT_Input                  0
#define ACCEPT_GotMuons               1
#define ACCEPT_InvMass_Cut            2

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFMultiMuFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass) {
  
  ATH_MSG_DEBUG("Running TrigEFMultiMuFex::acceptInputs");
  m_mon_Acceptance.push_back( ACCEPT_Input );

  if ( timerSvc() )    m_BmmHypTot->start();
    
    // check the right number of inputTEs
    if ( m_checkNinputTE != 0 && m_expectNumberOfInputTE != inputTE.size()) {
        ATH_MSG_ERROR("Got different than " << m_expectNumberOfInputTE << " number of input TEs, found " << inputTE.size() );
        if ( timerSvc() )    m_BmmHypTot->stop();
        m_mon_Errors.push_back( ERROR_WrongNum_Input_TE );
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("Found Expected " << m_expectNumberOfInputTE << " inputTEs" );
    }
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        ATH_MSG_ERROR("Error retriving EventInfo" );
        m_mon_Errors.push_back( ERROR_No_EventInfo );
    }
  
    if (evtNumber !=  m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent=evtNumber;
    }
    m_countTotalRoI++;

  ATH_MSG_DEBUG(" Number of input TEs " << inputTE.size() );
  pass=true;
  return HLT::OK;

}


HLT::ErrorCode TrigEFMultiMuFex::hltExecute(HLT::TEConstVec& inputTE , HLT::TriggerElement* outputTE)
{
    ATH_MSG_DEBUG(" In TrigEFMultiMu hltExecute" );
    
    xAOD::TrigBphysContainer * xAODTrigBphysColl = new xAOD::TrigBphysContainer;
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl;
    xAODTrigBphysColl->setStore(&xAODTrigBphysAuxColl);
    
    if (m_expectNumberOfInputTE==3) { // call the main processing algorithm
        ATH_MSG_DEBUG(" Call processTriMuon " );
        processTriMuon(inputTE, *xAODTrigBphysColl);
    }
    
    /*
    if(vtxpass) m_countPassedVtxFit2R++;
    if(mumuIDpass) m_countPassedmumuPairs2R++;
    if (PassedBsMass) m_countPassedBsMass2R++;
    m_countPassedRoIs++;
    if(IdEvent != m_lastEventPassed) {
        m_countPassedEvents++;
        m_lastEventPassed = IdEvent;
        if(vtxpass) m_countPassedVtxFitEv++;
        if(mumuIDpass) m_countPassedmumuPairsEv++;
        if (PassedBsMass) m_countPassedBsMassEv++;
    }
     */
    if ( timerSvc() )  m_BmmHypTot->stop();

    
    if (xAODTrigBphysColl && xAODTrigBphysColl->size()) {
        ATH_MSG_DEBUG("REGTEST: Store Bphys Collection size: " << xAODTrigBphysColl->size() );
        
        HLT::ErrorCode sc = attachFeature(outputTE, xAODTrigBphysColl, "EFMultiMuFex" );
        if(sc != HLT::OK) {
            ATH_MSG_WARNING("Failed to store trigBphys Collection" );
            m_mon_Errors.push_back( ERROR_BphysColl_Fails );
            delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr; // assume deletion responsibility
            return HLT::ERROR;
        }
    } else {
        ATH_MSG_DEBUG("REGTEST: no bphys collection to store "  );
        delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr;
    }

    
    return HLT::OK;
}

void TrigEFMultiMuFex::processTriMuon(HLT::TEConstVec& inputTE, xAOD::TrigBphysContainer & outputContainer) {
    ATH_MSG_VERBOSE(" In processTriMuon " );
    typedef  ElementLinkVector<xAOD::MuonContainer>  ELVMuons;
    std::vector<ELVMuons>          vec_elv_muons; // for muons, size 3

    unsigned int nTEs = m_expectNumberOfInputTE;
    if( ! m_checkNinputTE ) nTEs = inputTE.size();


    for ( unsigned int i=0; i < nTEs; ++i) {
        ELVMuons elvmuon;
        ATH_MSG_DEBUG("Try to retrieve EFInfo container of muon " << i );
        if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(inputTE[i], elvmuon)!=HLT::OK ) {
            ATH_MSG_DEBUG("Failed to get EFInfo feature of muon " << i << ", exiting" );
            m_mon_Errors.push_back( ERROR_GetMuonFailed );
            return;
        }
        ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF " << i << " Feature, size = " << elvmuon.size() );
        vec_elv_muons.push_back(elvmuon);
    } // loop over each roi
    
    m_mon_Acceptance.push_back( ACCEPT_GotMuons );

    if(msgLvl() <= MSG::DEBUG) {
        int ic(0);
        for ( const auto muelv : vec_elv_muons) {
            // loop over the vector of muon containers
            msg() << MSG::DEBUG << "MuonContainer, Got MuonEF " << ic << " Feature, size = " << muelv.size() << endmsg;
            int i(0);
            for ( const auto muel: muelv) {
                msg() << MSG::DEBUG << "ELLink: " << i++
                    << " index: "  << muel.index()
                    << " sgkey: "  << muel.dataID()
                    << " hashkey: "<< muel.key()
                    << " valid: "  << muel.isValid()
                    << " ptr: "    << (muel.isValid() ? *muel : nullptr)
                    << endmsg;
                if (!muel.isValid()) { continue;}
                msg() << MSG::DEBUG << "Muon:   " << i
                    << " pt: " <<  (*muel)->pt()
                    << " eta: " << (*muel)->eta()
                    << " phi: " << (*muel)->phi()
                    << " q: "   << (*muel)->charge()
                    << " mutype: "   << (*muel)->muonType()
                    << " author: "   << (*muel)->author()
                    << " id/ms/cb: " << (*muel)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)
                    << " "           << (*muel)->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)
                    << " "           << (*muel)->trackParticle(xAOD::Muon::CombinedTrackParticle)
                    << endmsg;
            } // for
            // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODMuon/xAODMuon/versions/Muon_v1.h
            
            ++ic;
        } // for muons
    } // DEBUG
    
    std::vector<const xAOD::Muon*> uniqueMuons;
    // add all the unique muons into a single container
    for ( const auto muelv : vec_elv_muons) {
        for ( const auto muel : muelv ) {
            m_bphysHelperTool->addUnique( *muel, uniqueMuons, 0.005,0.005,10,xAOD::Muon::InnerDetectorTrackParticle);
        } // loop over muonEL in each roi
    } // loop over rois
    if ( msgLvl() <= MSG::DEBUG )ATH_MSG_DEBUG("Number of unique muons : " << uniqueMuons.size() );

    if (m_NMassMuon == 2 && uniqueMuons.size() > 1) {
        buildDiMu(uniqueMuons,outputContainer);
    } // di-muons
    
    if (m_NMassMuon == 3 && uniqueMuons.size() > 2) {
        buildTriMu(uniqueMuons,outputContainer);
    } // tri-muons
    
    return;
} //processTriMuon


void TrigEFMultiMuFex::buildDiMu (const std::vector<const xAOD::Muon*> &muons, xAOD::TrigBphysContainer & outputContainer) {
    ATH_MSG_DEBUG(" In TrigEFBMuMu buildDiMu" );
    if (muons.size() < 2) return;
    
    std::vector<const xAOD::Muon*>::const_iterator muit_end = muons.end();
    std::vector<const xAOD::Muon*>::const_iterator muit1(muons.begin()), muit2(muons.begin());

    for (;muit1 != muit_end; ++muit1) {
        double charge1 = (*muit1)->charge();
        const xAOD::TrackParticle * tp1 = (*muit1)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        if (!tp1) continue;
        for (muit2=muit1; muit2 != muit_end; ++muit2) {
            if (muit2 == muit1) continue;
            double charge2 = (*muit2)->charge();
            const xAOD::TrackParticle * tp2 = (*muit2)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
            if (!tp2) continue;
            
            if ( (charge1*charge2 > 0) ) {
                ATH_MSG_DEBUG("Reject All same charges: " << charge1 << " " << charge2 );
                continue;
            } // same sign
            double mass = m_bphysHelperTool->invariantMass( {tp1,tp2}, {m_massMuon,m_massMuon} );
            if ( mass < m_lowerMassCut || (m_ApplyupperMassCut && mass > m_upperMassCut)  ) {
                ATH_MSG_DEBUG("Mass buildDiMu: " << mass << " cut failed  "  );
                continue;
            } else {
                ATH_MSG_DEBUG("Mass buildDiMu: " << mass << " cut passed  "  );
                m_mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
            }

            // create output object
            xAOD::TrigBphys * xaodobj = new xAOD::TrigBphys;
            outputContainer.push_back(xaodobj);
            std::vector<ElementLink<xAOD::TrackParticleContainer> > trksEL =
            {(*muit1)->trackParticleLink(xAOD::Muon::InnerDetectorTrackParticle),
                (*muit2)->trackParticleLink(xAOD::Muon::InnerDetectorTrackParticle)
            };
            
            
            xaodobj->initialise(0, 0., 0., xAOD::TrigBphys::MULTIMU, mass,xAOD::TrigBphys::EF);
            std::vector<double> masses(trksEL.size(),m_massMuon);
            if (m_bphysHelperTool->vertexFit(xaodobj,trksEL,masses).isFailure()) {
                ATH_MSG_DEBUG("Problems with vertex fit in buildTriMu"  );
            }
            m_mon_mu1pT.push_back(tp1->p4().Pt()*0.001);
            m_mon_mu2pT.push_back(tp2->p4().Pt()*0.001);
            //m_mon_pTsum = ((m_mon_mu1pT + m_mon_mu2pT)); // but this isn't declared anywhere?
            //m_mon_dEtaMuMu.push_back(m_bphysHelperTool->absDeltaEta(tp1->p4().Eta(),tp2->p4().Eta()));
            //m_mon_dPhiMuMu.push_back(m_bphysHelperTool->absDeltaPhi(tp1->p4().Phi(),tp2->p4().Phi()));
            m_mon_MuMumass.push_back(mass*0.001);
            m_mon_FitMass.push_back(xaodobj->fitmass()*0.001);
            m_mon_Chi2.push_back   (xaodobj->fitchi2());


            
        } // muit2
    }//muit1
    
    
} // buildDiMu

void TrigEFMultiMuFex::buildTriMu(const std::vector<const xAOD::Muon*> &muons, xAOD::TrigBphysContainer & outputContainer) {
    ATH_MSG_DEBUG(" In TrigEFBMuMu buildTriMu" );
    if (muons.size() < 3) return;

    std::vector<const xAOD::Muon*>::const_iterator muit_end = muons.end();
    std::vector<const xAOD::Muon*>::const_iterator muit1(muons.begin()), muit2(muons.begin()), muit3(muons.begin());
    
    for (;muit1 != muit_end; ++muit1) {
        double charge1 = (*muit1)->charge();
        const xAOD::TrackParticle * tp1 = (*muit1)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        if (!tp1) continue;
        for (muit2=muit1; muit2 != muit_end; ++muit2) {
            if (muit2 == muit1) continue;
            double charge2 = (*muit2)->charge();
            const xAOD::TrackParticle * tp2 = (*muit2)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
            if (!tp2) continue;

            for (muit3=muit2; muit3 != muit_end; ++muit3) {
                if (muit3 == muit2 || muit3 == muit1) continue;
                double charge3 = (*muit3)->charge();
                const xAOD::TrackParticle * tp3 = (*muit3)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
                if (!tp3) continue;

                if ( (charge1*charge2 > 0) && (charge1*charge3 > 0) && (charge2*charge3 > 0) ) {
                    ATH_MSG_DEBUG("Reject All same charges: " << charge1 << " " << charge2 << " " << charge3 );
                    continue;
                } // same sign
                
                double mass = m_bphysHelperTool->invariantMass( {tp1,tp2,tp3}, {m_massMuon,m_massMuon,m_massMuon} );
                if ( mass < m_lowerMassCut || (m_ApplyupperMassCut && mass > m_upperMassCut)  ) {
                    ATH_MSG_DEBUG("Mass buildTriMu: " << mass << " cut failed  "  );
                    continue;
                } else {
                    ATH_MSG_DEBUG("Mass buildTriMu: " << mass << " cut passed  "  );
                    m_mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
                }
                
                // create output object
                xAOD::TrigBphys * xaodobj = new xAOD::TrigBphys;
                outputContainer.push_back(xaodobj);
                std::vector<ElementLink<xAOD::TrackParticleContainer> > trksEL =
                         {(*muit1)->trackParticleLink(xAOD::Muon::InnerDetectorTrackParticle),
                          (*muit2)->trackParticleLink(xAOD::Muon::InnerDetectorTrackParticle),
                          (*muit3)->trackParticleLink(xAOD::Muon::InnerDetectorTrackParticle)
                         };
                    
                
                xaodobj->initialise(0, 0., 0., xAOD::TrigBphys::MULTIMU, mass,xAOD::TrigBphys::EF);
                std::vector<double> masses(trksEL.size(),m_massMuon);
                if (m_bphysHelperTool->vertexFit(xaodobj,trksEL,masses).isFailure()) {
                    ATH_MSG_DEBUG("Problems with vertex fit in buildTriMu"  );
                }
                m_bphysHelperTool->fillTrigObjectKinematics(xaodobj,{tp1,tp2,tp3});
                
            } // muit3
        } // muit2
    } // muit1
    
    
} // buildTriMu
