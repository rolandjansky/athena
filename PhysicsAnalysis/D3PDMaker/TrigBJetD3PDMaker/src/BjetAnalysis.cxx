/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <cmath>
#include <iomanip>

// ROOT include(s):
#include <TH1.h>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

// Local include(s):
#include "BjetAnalysis.h"

/*
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigL2Bjet.h"

#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigParticle/TrigEFBjet.h"
*/

BjetAnalysis::BjetAnalysis( const std::string &name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
     m_histSvc( "THistSvc", name ),
     m_trigDec( "TrigDec::TrigDecisionTool/TrigDecisionTool" )
{

   declareProperty( "TrigDecisionTool", m_trigDec,
                    "The tool to access TrigDecision" );
   declareProperty( "File", m_file,
                    "The file string used to dump the histograms" );
   declareProperty( "L2Chain", m_L2chain = "L2_b20",
                    "L2 chain to be used.");
   declareProperty( "EFChain", m_EFchain = "EF_b20",
                    "EF chain to be used.");
}

StatusCode BjetAnalysis::initialize() {

   // get HistSvc
   CHECK( m_histSvc.retrieve() );

   // get TrigDecisionTool
   //   CHECK( m_trigDec.retrieve() );

/*
  m_hL2_zp = makeTH1F("L2_zp", "L2 z of the Primary Vertex (mm)", 100, -200, 200);
  m_hL2_zp_pass = makeTH1F("L2_zp_pass", "L2 (accepted) z of the Primary Vertex (mm)", 100, -200, 200);
  m_hEF_zp = makeTH1F("EF_zp", "EF z of the Primary Vertex (mm)", 100, -200, 200);
  m_hEF_zp_pass = makeTH1F("EF_zp_pass", "EF (accepted) z of the Primary Vertex (mm)", 100, -200, 200);
  m_hL2_XIP3D_all = makeTH1F("L2_XIP3D_all", "L2 XIP3D", 50, 0, 1);
  m_hL2_XIP3D_pass = makeTH1F("L2_XIP3D_pass", "L2 (accepted) XIP3D", 50, 0, 1);
  m_hEF_XIP3D_all = makeTH1F("EF_XIP3D_all", "EF XIP3D", 50, 0, 1);
  m_hEF_XIP3D_pass = makeTH1F("EF_XIP3D_pass", "EF (accepted) XIP3D", 50, 0, 1);

  m_hL1_eta = makeTH1F("L1_eta", "L1 #eta ; #eta ; Entries", 32, -3.2, 3.2);
  m_hL1_phi = makeTH1F("L1_phi", "L1 #phi ; #phi ; Entries", 32, -3.2, 3.2);

  m_hL2_trkN = makeTH1F("L2_trkN", "L2 Tracks number ; N ; Entries", 100, 0, 100);
  m_hL2_trkPt = makeTH1F("L2_trkPt", "L2 Tracks p_{T} ; p_{T} ; Entries", 10, 0, 10e3);
  m_hL2_trkEta = makeTH1F("L2_trkEta", "L2 Tracks #eta ; #eta ; Entries", 32, -3.2, 3.2);
  m_hL2_trkPhi = makeTH1F("L2_trkPhi", "L2 Tracks #phi ; #phi ; Entries", 32, -3.2, 3.2);
  m_hL2_trkZ0 = makeTH1F("L2_trkZ0", "L2 Tracks z_{0}; z_{0} ; Entries", 100, -200, 200);

  m_hEF_trkN = makeTH1F("EF_trkN", "EF Tracks number ; N ; Entries", 100, 0, 100);
  m_hEF_trkPt = makeTH1F("EF_trkPt", "EF Tracks p_{T} ; p_{T} ; Entries", 10, 0, 10e3);
  m_hEF_trkEta = makeTH1F("EF_trkEta", "EF Tracks #eta ; #eta ; Entries", 32, -3.2, 3.2);
  m_hEF_trkPhi = makeTH1F("EF_trkPhi", "EF Tracks #phi ; #phi ; Entries", 32, -3.2, 3.2);
  m_hEF_trkZ0 = makeTH1F("EF_trkZ0", "EF Tracks z_{0} ; z_{0} ; Entries", 100, -200, 200);
*/
  return StatusCode::SUCCESS;
}

StatusCode BjetAnalysis::execute() {

   const Rec::TrackParticleContainer *trkPC = 0;
   StatusCode scsg = evtStore()->retrieve( trkPC, "HLT_InDetTrigParticleCreation_Bjet_EFID" );
   if( ( ! scsg.isFailure() ) && ( trkPC != 0 ) ) {
      ATH_MSG_INFO( "TPC: Got " << trkPC->size() << " TrackParticles." );
      for( size_t i = 0; i < trkPC->size(); ++i ) {
         ATH_MSG_INFO( "TPC: TrackParticle #" << i << ": pT = " << std::setprecision( 40 )
                       << trkPC->at(i)->pt()
                       << ", eta = " << trkPC->at(i)->eta() << ", phi = "
                       << trkPC->at(i)->phi() );
      }
   }
/*
  Trig::FeatureContainer fx;
  Trig::FeatureContainer::combination_const_iterator cx, cxEnd;

  fx = m_trigDec->features(m_L2chain, TrigDefs::alsoDeactivateTEs);
  cxEnd = fx.getCombinations().end();
  for(cx=fx.getCombinations().begin(); cx!=cxEnd ; ++cx) {
    std::vector<Trig::Feature<TrigL2Bjet> > trigL2Bjet;
    trigL2Bjet = cx->get<TrigL2Bjet>("L2BjetFex",TrigDefs::alsoDeactivateTEs);
    for(std::vector<Trig::Feature<TrigL2Bjet> >::const_iterator trkL2bI = trigL2Bjet.begin(); trkL2bI != trigL2Bjet.end(); trkL2bI++) {
      const TrigL2Bjet* f = trkL2bI->cptr();
      if (!f) continue;

      (*m_log) << MSG::DEBUG << "TrigL2Bjet: zPrmVtx = " << f->prmVtx() << ", XIP3D = " << f->xIP3D() << ", eta = " << f->eta() << ", phi = " << f->phi() << endreq;

      m_hL2_XIP3D_all->Fill(f->xIP3D());
      m_hL2_zp->Fill(f->prmVtx());
 
      // L2 copies L1 eta and phi:
      m_hL1_eta->Fill(f->eta());
      m_hL1_phi->Fill(f->phi());
    }

    trigL2Bjet = cx->get<TrigL2Bjet>("L2BjetFex"); // only passed
    for(std::vector<Trig::Feature<TrigL2Bjet> >::const_iterator trkL2bI = trigL2Bjet.begin(); trkL2bI != trigL2Bjet.end(); trkL2bI++) {
      const TrigL2Bjet* f = trkL2bI->cptr();
      if (!f) continue;

      m_hL2_XIP3D_pass->Fill(f->xIP3D());
      m_hL2_zp_pass->Fill(f->prmVtx());
 
    }
    std::vector<Trig::Feature<TrigInDetTrackCollection> > inDetTrackFeature;
    std::vector<Trig::Feature<TrigInDetTrackCollection> >::const_iterator trkFeatureI;

    inDetTrackFeature = cx->get<TrigInDetTrackCollection>("TrigSiTrack_Jet", TrigDefs::alsoDeactivateTEs);
    for(trkFeatureI = inDetTrackFeature.begin(); trkFeatureI != inDetTrackFeature.end(); trkFeatureI ++) {
      const TrigInDetTrackCollection* L2tr = trkFeatureI->cptr();
      (*m_log) << MSG::DEBUG << "SiTrack Track number = " << L2tr->size() << endreq;

      TrigInDetTrackCollection::const_iterator aa = L2tr->begin();
      TrigInDetTrackCollection::const_iterator bb = L2tr->end();
      int N = 0;
      for (; aa != bb; ++aa) {
        if (*aa) {
          if ((*aa)->param()) {
            N++;
            (*m_log) << MSG::INFO << "   * SiTrack Track - (" << (*aa)->param()->eta() << ", " << (*aa)->param()->phi0() << ") - Pt = " << (*aa)->param()->pT() << endreq;
            m_hL2_trkPt->Fill((*aa)->param()->pT());
            m_hL2_trkEta->Fill((*aa)->param()->eta());
            m_hL2_trkPhi->Fill((*aa)->param()->phi0());
            m_hL2_trkZ0->Fill((*aa)->param()->z0());
          }
        }
      }
      m_hL2_trkN->Fill(N);
    }

    inDetTrackFeature = cx->get<TrigInDetTrackCollection>("TrigIDSCAN_Jet", TrigDefs::alsoDeactivateTEs);
    for(trkFeatureI = inDetTrackFeature.begin(); trkFeatureI != inDetTrackFeature.end(); trkFeatureI ++) {
      const TrigInDetTrackCollection* L2tr = trkFeatureI->cptr();
      (*m_log) << MSG::INFO << "IDSCAN  Track number = " << L2tr->size() << endreq;
      m_htrkN->Fill(L2tr->size());

      TrigInDetTrackCollection::const_iterator aa = L2tr->begin();
      TrigInDetTrackCollection::const_iterator bb = L2tr->end();
      for (; aa != bb; ++aa) {
        if (*aa) {
          if ((*aa)->param()) {
            (*m_log) << MSG::INFO << "   * IDSCAN  Track - (" << (*aa)->param()->eta() << ", " << (*aa)->param()->phi0() << ") - Pt = " << (*aa)->param()->pT() << endreq;
            m_htrkPt->Fill((*aa)->param()->pT());
            m_htrkEta->Fill((*aa)->param()->eta());
            m_htrkPhi->Fill((*aa)->param()->phi0());
          }
        }
      }
    }
  }

  fx = m_trigDec->features(m_EFchain, TrigDefs::alsoDeactivateTEs);
  cxEnd = fx.getCombinations().end();
  for(cx = fx.getCombinations().begin(); cx != cxEnd ; ++cx) {
    std::vector<Trig::Feature<TrigEFBjet> > trigEFBjet;
    trigEFBjet = cx->get<TrigEFBjet>("EFBjetFex",TrigDefs::alsoDeactivateTEs);
    for(std::vector<Trig::Feature<TrigEFBjet> >::const_iterator itr = trigEFBjet.begin(); itr != trigEFBjet.end(); itr++) {
      const TrigEFBjet* f = itr->cptr();
      if (!f) continue;

      (*m_log) << MSG::DEBUG << "TrigEFBjet: zPrmVtx = " << f->prmVtx() << ", XIP3D = " << f->xIP3D() << ", eta = " << f->eta() << ", phi = " << f->phi() << endreq;

      m_hEF_XIP3D_all->Fill(f->xIP3D());
      m_hEF_zp->Fill(f->prmVtx());
    }

    trigEFBjet = cx->get<TrigEFBjet>("EFBjetFex"); // only passed
    for(std::vector<Trig::Feature<TrigEFBjet> >::const_iterator itr = trigEFBjet.begin(); itr != trigEFBjet.end(); itr++) {
      const TrigEFBjet* f = itr->cptr();
      if (!f) continue;

      m_hEF_XIP3D_pass->Fill(f->xIP3D());
      m_hEF_zp_pass->Fill(f->prmVtx());
    }
    std::vector<Trig::Feature<Rec::TrackParticleContainer> > efTrackFeature;
    std::vector<Trig::Feature<Rec::TrackParticleContainer> >::const_iterator efTrkFeatureI;

    efTrackFeature = cx->get<Rec::TrackParticleContainer>("InDetTrigParticleCreation_Bjet_EFID", TrigDefs::alsoDeactivateTEs);
    for(efTrkFeatureI = efTrackFeature.begin(); efTrkFeatureI != efTrackFeature.end(); efTrkFeatureI++) {
      const Rec::TrackParticleContainer* EFtr = efTrkFeatureI->cptr();
      (*m_log) << MSG::DEBUG << "Track number = " << EFtr->size() << endreq;

      Rec::TrackParticleContainer::const_iterator aa = EFtr->begin();
      Rec::TrackParticleContainer::const_iterator bb = EFtr->end();
      int N = 0;
      for (; aa != bb; ++aa) {
        if (*aa) {
          if ((*aa)->perigee()) {
            N++;
            (*m_log) << MSG::INFO << "   * EF Track - (" << (*aa)->perigee()->eta() << ", " << (*aa)->perigee()->parameters()[Trk::phi0] << ") - Pt = " << (*aa)->perigee()->pT() << endreq;
            m_hEF_trkPt->Fill((*aa)->perigee()->pT());
            m_hEF_trkEta->Fill((*aa)->perigee()->eta());
            m_hEF_trkPhi->Fill((*aa)->perigee()->parameters()[Trk::phi0]);
            m_hEF_trkZ0->Fill((*aa)->perigee()->parameters()[Trk::z0]);
          }
        }
      }
      m_hEF_trkN->Fill(N);
    }
  }
*/
  return StatusCode::SUCCESS;
}

TH1F *BjetAnalysis::makeTH1F( const std::string &name, const std::string &title,
                              Int_t nbinsx, Double_t xlow, Double_t xhigh ) {

   TH1F *h = new TH1F( name.c_str(), title.c_str(), nbinsx, xlow, xhigh );
   std::string path = "/";
   path += m_file;
   path += "/";
   path += name;

   if( m_histSvc->regHist( path, h ).isFailure() ) {
      REPORT_MESSAGE( MSG::ERROR )
         << "Couldn't register histogram with name: " << path;
      throw 0;
   }

   return h;
}

Float_t BjetAnalysis::deltaPhi( Float_t dphi ) {

   return std::abs( M_PI - std::abs( M_PI - std::abs( dphi ) ) );
}
