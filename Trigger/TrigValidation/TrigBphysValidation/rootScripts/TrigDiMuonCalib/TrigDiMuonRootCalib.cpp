/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// TrigDiMuonRootCalib class
///---------------------------------------------------
/// TrigDiMuon calibration and detailed
/// performance study
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of TrigDiMuonRootCalib
///===================================================

#include "TrigDiMuonRootCalib.h"
#include "TMath.h"
#include "TGraphAsymmErrors.h"
#include <limits>

///===================================================
void TrigDiMuonRootCalib::analyzeAllHitMatching(std::string chain, std::string settings, std::string cutStr) {
  Log log("TrigDiMuonRootCalib::analyzeAllHitMatching", ol());

  log.info("Settings: " + settings);
  log.info("cuts:     " + cutStr);

  double dRCut = 0.1;
  getValue(settings, "DRCut=", dRCut);

  std::string constantsFile = "constants.py";
  getValue(settings, "ConstantsFile=", constantsFile);

  int maxEvt = -1;
  getValue(settings, "MaxEvt=", maxEvt);

  bool noExt = settings.find("NoExtrapolation") != std::string::npos;

  // create selection algorithm
  CutsJpsi cuts(ol(), cutStr);

  // pT bins
  double ptBins[5] = {0, 4000,6000,8000, std::numeric_limits<double>::max()};
  double avgPt[4] = {0, 0, 0, 0};
  int    nPt[4] = {0, 0, 0, 0};

  // eta bins
  double etaBinsInn[4] = {0.9, 1.3, 1.6, std::numeric_limits<double>::max() };
  double etaBinsMid[6] = {0.9, 1.2, 1.35, 1.65, 2., std::numeric_limits<double>::max() };

  //histograms
  importHistFromFile("histograms_analyzeAllHitMatching.config");

  for(int j=0; j<5; ++j) {
    setValXVectPointer( "Hit_Endcap_mdt_inner_dEta"+iToStr(j) , &m_hit_Endcap_mdt_inner_dEta[j]  );
    setValXVectPointer( "Hit_Endcap_mdt_middle_dEta"+iToStr(j), &m_hit_Endcap_mdt_middle_dEta[j] );
    setValXVectPointer( "Hit_Barrel_mdt_inner_dEta"+iToStr(j) , &m_hit_Barrel_mdt_inner_dEta[j]  );
    setValXVectPointer( "Hit_Barrel_mdt_middle_dEta"+iToStr(j), &m_hit_Barrel_mdt_middle_dEta[j] );
    setValXVectPointer( "Hit_rpc_middle_dEta"+iToStr(j)       , &m_hit_rpc_middle_dEta[j]        );
    setValXVectPointer( "Hit_rpc_middle_dPhi"+iToStr(j)       , &m_hit_rpc_middle_dPhi[j]        );
    setValXVectPointer( "Hit_tgc_wire_inner_dEta"+iToStr(j)   , &m_hit_tgc_wire_inner_dEta[j]    );
    setValXVectPointer( "Hit_tgc_wire_middle_dEta"+iToStr(j)  , &m_hit_tgc_wire_middle_dEta[j]   );
    setValXVectPointer( "Hit_tgc_wire_inner_dPhi"+iToStr(j)   , &m_hit_tgc_wire_inner_dPhi[j]    );
    setValXVectPointer( "Hit_tgc_wire_middle_dPhi"+iToStr(j)  , &m_hit_tgc_wire_middle_dPhi[j]   );
    setValXVectPointer( "Hit_tgc_strip_inner_dPhi"+iToStr(j)  , &m_hit_tgc_strip_inner_dPhi[j]   );
    setValXVectPointer( "Hit_tgc_strip_middle_dPhi"+iToStr(j) , &m_hit_tgc_strip_middle_dPhi[j]  );
    setValXVectPointer( "Hit_tgc_strip_middle2_dPhi"+iToStr(j), &m_hit_tgc_strip_middle2_dPhi[j]  );
  }

  // old prpagator
  setValXVectPointer( "OldHit_Endcap_mdt_inner_dEta" , &m_hitOld_Endcap_mdt_inner_dEta  );
  setValXVectPointer( "OldHit_Endcap_mdt_middle_dEta", &m_hitOld_Endcap_mdt_middle_dEta );
  setValXVectPointer( "OldHit_Barrel_mdt_inner_dEta" , &m_hitOld_Barrel_mdt_inner_dEta  );
  setValXVectPointer( "OldHit_Barrel_mdt_middle_dEta", &m_hitOld_Barrel_mdt_middle_dEta );
  setValXVectPointer( "OldHit_rpc_middle_dEta"       , &m_hitOld_rpc_middle_dEta        );
  setValXVectPointer( "OldHit_rpc_middle_dPhi"       , &m_hitOld_rpc_middle_dPhi        );
  setValXVectPointer( "OldHit_tgc_wire_inner_dEta"   , &m_hitOld_tgc_wire_inner_dEta    );
  setValXVectPointer( "OldHit_tgc_wire_middle_dEta"  , &m_hitOld_tgc_wire_middle_dEta   );
  setValXVectPointer( "OldHit_tgc_wire_inner_dPhi"   , &m_hitOld_tgc_wire_inner_dPhi    );
  setValXVectPointer( "OldHit_tgc_wire_middle_dPhi"  , &m_hitOld_tgc_wire_middle_dPhi   );
  setValXVectPointer( "OldHit_tgc_strip_inner_dPhi"  , &m_hitOld_tgc_strip_inner_dPhi   );
  setValXVectPointer( "OldHit_tgc_strip_middle_dPhi" , &m_hitOld_tgc_strip_middle_dPhi  );
  setValXVectPointer( "OldHit_tgc_strip_middle2_dPhi", &m_hitOld_tgc_strip_middle2_dPhi  );

  // matching in eta bins
  for(int j=0; j<4; ++j) {
    setValXVectPointer( "Hit2_tgc_wire_inner_dPhi"+iToStr(j)   , &m_hit2_tgc_wire_inner_dPhi[j]    );
    setValXVectPointer( "Hit2_tgc_strip_inner_dPhi"+iToStr(j)  , &m_hit2_tgc_strip_inner_dPhi[j]   );
  }
  for(int j=0; j<6; ++j) {
    setValXVectPointer( "Hit2_Endcap_mdt_middle_dEta"+iToStr(j), &m_hit2_Endcap_mdt_middle_dEta[j] );
    setValXVectPointer( "Hit2_tgc_wire_middle_dEta"+iToStr(j)  , &m_hit2_tgc_wire_middle_dEta[j]   );
    setValXVectPointer( "Hit2_tgc_wire_middle_dPhi"+iToStr(j)  , &m_hit2_tgc_wire_middle_dPhi[j]   );
    setValXVectPointer( "Hit2_tgc_strip_middle_dPhi"+iToStr(j) , &m_hit2_tgc_strip_middle_dPhi[j]  );
  }

  // auxiliary plots
  std::vector<double> mdtInnerHits_r;
  std::vector<double> mdtInnerHits_z;
  std::vector<double> mdtMiddleHits_r;
  std::vector<double> mdtMiddleHits_z;

  std::vector<double> rpcMiddleHits_r;
  std::vector<double> rpcMiddleHits_z;

  std::vector<double> tgcInnerHits_r;
  std::vector<double> tgcInnerHits_z;
  std::vector<double> tgcInnerHits_phi;
  std::vector<double> tgcMiddleHits_r;
  std::vector<double> tgcMiddleHits_z;
  std::vector<double> tgcMiddleHits_phi;

  std::vector<double> muonPhi;
  std::vector<double> muonExtPhi;
  std::vector<std::vector<double> > hitPhi;
  std::vector<std::vector<double> > hitR;

  // extrapolator
  TrigDiMuonExtrapolator* propagator = new TrigDiMuonExtrapolator(ol(), settings, false);
  TrigDiMuonExtrapolator* propagatorOld = new TrigDiMuonExtrapolator(ol(), settings, false);

  // import propagator constants
  propagator->read(constantsFile);
  propagatorOld->read("constants_original.py");

  // declare trees
  TTree *tDetails;
  Details* details;

  // jpsi trees
  TTree** tJpsi = NULL;
  ParticleBag jpsi;

  TTree** tTracks = NULL;
  TrackBag tracks;

  TTree** tMuons = NULL;
  MuonBag muons;

  // do J/psi matching?
  std::string detailsTreeName = "All_"+chain+"_Details";
  bool doJpsiMatching = settings.find("DoJpsiMatching") != std::string::npos;
  if(doJpsiMatching) {
    detailsTreeName = chain+"_Details";
    tJpsi = new (TTree*);
    tTracks = new (TTree*);
    tMuons = new (TTree*);
  }

  bool doMuonMatching = !doJpsiMatching && settings.find("DoMuonMatching") != std::string::npos;
  if(doMuonMatching) {
    tMuons = new (TTree*);
  }

  //loop over all entries and all files
  log.info("******************************************************************");
  log.info("** BEGINNING OF THE LOOP                                        **");
  log.info("******************************************************************");
  m_nEntries = 0;
  for(resetIterator();
      iterateDetails(detailsTreeName, &tDetails, &details,
                     "Jpsi", tJpsi, &jpsi,
                     "MuidMuonCollectionTracks", tTracks, &tracks,
                     "MuidMuonCollection", tMuons, &muons) &&
          (maxEvt <0 || m_nEntries < maxEvt);
      ++m_nEntries)
  {

    if(m_nEntries % 10000 == 0) log.info("   ===> Entry " + iToStr(m_nEntries));

    /// check offline J/psi matching

    if(doJpsiMatching) {
      bool matchedJpsi = false;
      ParticleBag::iterator jpsiItr = jpsi.begin();
      for(; jpsiItr!=jpsi.end(); ++jpsiItr) {
        if( cuts.selectJpsi( m_ntupleReader, (*jpsiItr), tracks, muons) ) {
          // match to offline J/psi
          double dEta0 = fabs((*jpsiItr).getTrackMom(0).Eta() - details->Eta);
          double dPhi0 = dAngle((*jpsiItr).getTrackMom(0).Phi(), details->Phi);
          double dR0   = TMath::Sqrt(dEta0*dEta0 + dPhi0*dPhi0);

          double dEta1 = fabs((*jpsiItr).getTrackMom(1).Eta() - details->Eta);
          double dPhi1 = dAngle((*jpsiItr).getTrackMom(1).Phi(), details->Phi);
          double dR1   = TMath::Sqrt(dEta1*dEta1 + dPhi1*dPhi1);

          if(dR0 < dRCut || dR1 < dRCut) {
            matchedJpsi = true;
            break;
          }
        }
      }

      if(!matchedJpsi) continue;
    }

    /// check offline muons matching
    if(doMuonMatching) {

      bool matchedMuon = false;
      MuonBag::iterator muonItr = muons.begin();
      for(; muonItr!=muons.end(); ++muonItr) {

        if( !(*muonItr).isCombinedMuon() ) continue;

        // match to offline muon
        double dEta = fabs((*muonItr).eta() - details->Eta);
        double dPhi = dAngle((*muonItr).phi(), details->Phi);
        double dR   = TMath::Sqrt(dEta*dEta + dPhi*dPhi);

        if(dR < dRCut) {
          matchedMuon = true;
          break;
        }
      }

      if(!matchedMuon) continue;
    }


    /// fill hit matching histograms ===============================================

    double ptId   = details->Pt;
    double etaId  = details->Eta;
    double phiId  = details->Phi;
    double charge = details->Charge == 1 ? 1. : -1.; //TMath::IsNaN(details->Charge) ? -1. : 1.;

    // calculate average pT in each pT bin
    for(int j=0; j<4; ++j) {
      if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1]) {
        avgPt[j] += fabs(ptId);
        ++nPt[j];
      }
    }

    // MDT  -------------------------------------

    for(int i=0; i<(int)details->InnerMdt_r->size(); ++i) {
      mdtInnerHits_r.push_back(details->InnerMdt_r->at(i));
      mdtInnerHits_z.push_back(details->InnerMdt_z->at(i));
      if(fabs(details->Eta) > 1.) {
        // endcap
        double extEta = propagator->propagationInnerEndCap(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->InnerMdt_r->at(i), details->InnerMdt_z->at(i));
        if(noExt) {
          extEta = etaId;
        }

        for(int j=0; j<4; ++j)
          if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
            m_hit_Endcap_mdt_inner_dEta[j].push_back(hitEta - extEta );
        m_hit_Endcap_mdt_inner_dEta[4].push_back(hitEta - extEta );

      }else{
        // barrel
        double extEta = propagator->propagationInnerBarrel(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->InnerMdt_r->at(i), details->InnerMdt_z->at(i));
        if(noExt) {
          extEta = etaId;
        }

        for(int j=0; j<4; ++j)
          if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
            m_hit_Barrel_mdt_inner_dEta[j].push_back( hitEta - extEta );
        m_hit_Barrel_mdt_inner_dEta[4].push_back( hitEta - extEta );
      }
    }

    for(int i=0; i<(int)details->MiddleMdt_r->size(); ++i) {
      mdtMiddleHits_r.push_back(details->MiddleMdt_r->at(i));
      mdtMiddleHits_z.push_back(details->MiddleMdt_z->at(i));
      if(fabs(details->Eta) > 1.) {
        // endcap
        double extEta = propagator->propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->MiddleMdt_r->at(i), details->MiddleMdt_z->at(i));
        if(noExt) {
          extEta = etaId;
        }

        // pt bins
        for(int j=0; j<4; ++j)
          if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
            m_hit_Endcap_mdt_middle_dEta[j].push_back( hitEta - extEta );
        m_hit_Endcap_mdt_middle_dEta[4].push_back( hitEta - extEta );

        // eta bins
        if(fabs(ptId)>6000) {
          for(int j=0; j<5; ++j)
            if(fabs(etaId) >= etaBinsMid[j] && fabs(etaId) < etaBinsMid[j+1])
              m_hit2_Endcap_mdt_middle_dEta[j].push_back( ( hitEta - extEta ));
          m_hit2_Endcap_mdt_middle_dEta[5].push_back( (hitEta - extEta) );
        }
      }else{
        // barrel
        double extEta = propagator->propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->MiddleMdt_r->at(i), details->MiddleMdt_z->at(i));
        if(noExt) {
          extEta = etaId;
        }

        for(int j=0; j<4; ++j)
          if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
            m_hit_Barrel_mdt_middle_dEta[j].push_back( hitEta - extEta );
        m_hit_Barrel_mdt_middle_dEta[4].push_back( hitEta - extEta );
      }
    }


    // RPC ---------------------------------------

    for(int i=0; i<(int)details->MiddleRpc_etaPad_r->size(); ++i) {
      double extEta = propagator->propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
      double hitEta = getEta(details->MiddleRpc_etaPad_r->at(i), details->MiddleRpc_etaPad_z->at(i));
      if(noExt) {
        extEta = etaId;
      }

      for(int j=0; j<4; ++j)
        if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
          m_hit_rpc_middle_dEta[j].push_back( hitEta - extEta );
      m_hit_rpc_middle_dEta[4].push_back( hitEta - extEta );

      rpcMiddleHits_r.push_back( details->MiddleRpc_etaPad_r->at(i) );
      rpcMiddleHits_z.push_back( details->MiddleRpc_etaPad_z->at(i) );
    }

    for(int i=0; i<(int)details->MiddleRpc_phiPad_r->size(); ++i) {
      double extPhi = propagator->propagationMiddleBarrel(ptId, etaId, phiId, charge).Y();
      double hitPhi = atan2(details->MiddleRpc_phiPad_y->at(i),details->MiddleRpc_phiPad_x->at(i));
      if(noExt) {
        extPhi = phiId;
      }

      for(int j=0; j<4; ++j)
        if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
          m_hit_rpc_middle_dPhi[j].push_back( dAngle(hitPhi, extPhi) );
      m_hit_rpc_middle_dPhi[4].push_back( dAngle(hitPhi, extPhi) );

      rpcMiddleHits_r.push_back( details->MiddleRpc_phiPad_r->at(i) );
      rpcMiddleHits_z.push_back( details->MiddleRpc_phiPad_z->at(i) );
    }

    // TGC ---------------------------------------

    // wire
    for(int i=0; i<(int)details->InnerTgc_rhoHit_r->size(); ++i) {
      double extEta = propagator->propagationInnerEndCap(ptId, etaId, phiId, charge).X();
      double hitEta = getEta( details->InnerTgc_rhoHit_r->at(i), details->InnerTgc_rhoHit_z->at(i));
      double extPhi = propagator->propagationInnerEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->InnerTgc_rhoHit_phi->at(i);
      if(noExt) {
        extEta = etaId;
        extPhi = phiId;
      }

      // pt bins
      for(int j=0; j<4; ++j)
        if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1]) {
          m_hit_tgc_wire_inner_dEta[j].push_back( hitEta - extEta );
          m_hit_tgc_wire_inner_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
        }
      m_hit_tgc_wire_inner_dEta[4].push_back( hitEta - extEta );
      m_hit_tgc_wire_inner_dPhi[4].push_back( dAngle( hitPhi , extPhi ) );

      // eta bins
      if(fabs(ptId)>6000) {
        for(int j=0; j<3; ++j)
          if(fabs(etaId) >= etaBinsInn[j] && fabs(etaId) < etaBinsInn[j+1])
            m_hit2_tgc_wire_inner_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
        m_hit2_tgc_wire_inner_dPhi[3].push_back( dAngle( hitPhi , extPhi ) );
      }

      tgcInnerHits_r.push_back( details->InnerTgc_rhoHit_r->at(i) );
      tgcInnerHits_z.push_back( details->InnerTgc_rhoHit_z->at(i) );
      tgcInnerHits_phi.push_back( details->InnerTgc_rhoHit_phi->at(i) );
    }
    for(int i=0; i<(int)details->MiddleTgc_rhoHit_r->size(); ++i) {
      double extEta = propagator->propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
      double hitEta = getEta( details->MiddleTgc_rhoHit_r->at(i), details->MiddleTgc_rhoHit_z->at(i));
      double extPhi = propagator->propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->MiddleTgc_rhoHit_phi->at(i);
      if(noExt) {
        extEta = etaId;
        extPhi = phiId;
      }

      // pt bins
      for(int j=0; j<4; ++j) {
        if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1]){
          m_hit_tgc_wire_middle_dEta[j].push_back( hitEta - extEta );
          m_hit_tgc_wire_middle_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
        }
      }
      m_hit_tgc_wire_middle_dEta[4].push_back( hitEta - extEta );
      m_hit_tgc_wire_middle_dPhi[4].push_back( dAngle( hitPhi , extPhi ) );

      // eta bins
      if(fabs(ptId)>6000) {
        for(int j=0; j<5; ++j) {
          if(fabs(etaId) >= etaBinsMid[j] && fabs(etaId) < etaBinsMid[j+1]){
            m_hit2_tgc_wire_middle_dEta[j].push_back( (hitEta - extEta) );
            m_hit2_tgc_wire_middle_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
          }
        }
        m_hit2_tgc_wire_middle_dEta[5].push_back( (hitEta - extEta) );
        m_hit2_tgc_wire_middle_dPhi[5].push_back( dAngle( hitPhi , extPhi ) );
      }

      tgcMiddleHits_r.push_back( details->MiddleTgc_rhoHit_r->at(i) );
      tgcMiddleHits_z.push_back( details->MiddleTgc_rhoHit_z->at(i) );
      tgcMiddleHits_phi.push_back( details->MiddleTgc_rhoHit_phi->at(i) );
    }

    // strip
    if(details->InnerTgc_phiHit_r->size() > 0) {
      muonPhi.push_back(phiId);
      muonExtPhi.push_back(propagator->propagationInnerEndCap(ptId, etaId, phiId, charge).Y());
    }
    std::vector<double> tmpPhi;
    std::vector<double> tmpR;

    for(int i=0; i<(int)details->InnerTgc_phiHit_r->size(); ++i) {
      double extPhi = propagator->propagationInnerEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->InnerTgc_phiHit_phi->at(i);
      if(noExt) {
        extPhi = phiId;
      }

      // pt bins
      for(int j=0; j<4; ++j)
        if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
          m_hit_tgc_strip_inner_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
      m_hit_tgc_strip_inner_dPhi[4].push_back( dAngle( hitPhi , extPhi ) );

      // eta bins
      if(fabs(ptId)>6000) {
        for(int j=0; j<3; ++j)
          if(fabs(etaId) >= etaBinsInn[j] && fabs(etaId) < etaBinsInn[j+1])
            m_hit2_tgc_strip_inner_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
        m_hit2_tgc_strip_inner_dPhi[3].push_back( dAngle( hitPhi , extPhi ) );
      }

      tgcInnerHits_r.push_back( details->InnerTgc_phiHit_r->at(i) );
      tgcInnerHits_z.push_back( details->InnerTgc_phiHit_z->at(i) );
      tgcInnerHits_phi.push_back( details->InnerTgc_phiHit_phi->at(i) );

      tmpPhi.push_back(hitPhi);
      tmpR.push_back(details->InnerTgc_phiHit_r->at(i));

    }

    if(details->InnerTgc_phiHit_r->size() > 0) {
      hitPhi.push_back(tmpPhi);
      hitR.push_back(tmpR);
    }

    for(int i=0; i<(int)details->MiddleTgc_phiHit_r->size(); ++i) {
      double extEta = propagator->propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
      double extPhi = propagator->propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->MiddleTgc_phiHit_phi->at(i);
      if(noExt) {
        extPhi = phiId;
      }

      // pt bins
      if(fabs(extEta) <= 1.7) {
        for(int j=0; j<4; ++j)
          if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
            m_hit_tgc_strip_middle_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
        m_hit_tgc_strip_middle_dPhi[4].push_back( dAngle( hitPhi , extPhi ) );
      } else {
        for(int j=0; j<4; ++j)
          if(fabs(ptId) >= ptBins[j] && fabs(ptId) < ptBins[j+1])
            m_hit_tgc_strip_middle2_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
        m_hit_tgc_strip_middle2_dPhi[4].push_back( dAngle( hitPhi , extPhi ) );
      }

      // eta bins
      if(fabs(ptId)>6000) {
        for(int j=0; j<5; ++j)
          if(fabs(etaId) >= etaBinsMid[j] && fabs(etaId) < etaBinsMid[j+1])
            m_hit2_tgc_strip_middle_dPhi[j].push_back( dAngle( hitPhi , extPhi ) );
        m_hit2_tgc_strip_middle_dPhi[5].push_back( dAngle( hitPhi , extPhi ) );
      }

      tgcMiddleHits_r.push_back( details->MiddleTgc_phiHit_r->at(i) );
      tgcMiddleHits_z.push_back( details->MiddleTgc_phiHit_z->at(i) );
      tgcMiddleHits_phi.push_back( details->MiddleTgc_phiHit_phi->at(i) );
    }

    /// old propagator
    // MDT  -------------------------------------

    for(int i=0; i<(int)details->InnerMdt_r->size(); ++i) {
      if(fabs(details->Eta) > 1.) {
        // endcap
        double extEta = propagatorOld->propagationInnerEndCap(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->InnerMdt_r->at(i), details->InnerMdt_z->at(i));

        m_hitOld_Endcap_mdt_inner_dEta.push_back(hitEta - extEta );
      }else{
        // barrel
        double extEta = propagatorOld->propagationInnerBarrel(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->InnerMdt_r->at(i), details->InnerMdt_z->at(i));

        m_hitOld_Barrel_mdt_inner_dEta.push_back( hitEta - extEta );
      }
    }

    for(int i=0; i<(int)details->MiddleMdt_r->size(); ++i) {
      if(fabs(details->Eta) > 1.) {
        // endcap
        double extEta = propagatorOld->propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->MiddleMdt_r->at(i), details->MiddleMdt_z->at(i));

        m_hitOld_Endcap_mdt_middle_dEta.push_back( hitEta - extEta );
      }else{
        // barrel
        double extEta = propagatorOld->propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
        double hitEta = getEta(details->MiddleMdt_r->at(i), details->MiddleMdt_z->at(i));

        m_hitOld_Barrel_mdt_middle_dEta.push_back( hitEta - extEta );
      }
    }


    // RPC ---------------------------------------

    for(int i=0; i<(int)details->MiddleRpc_etaPad_r->size(); ++i) {
      double extEta = propagatorOld->propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
      double hitEta = getEta(details->MiddleRpc_etaPad_r->at(i), details->MiddleRpc_etaPad_z->at(i));

      m_hitOld_rpc_middle_dEta.push_back( hitEta - extEta );
    }

    for(int i=0; i<(int)details->MiddleRpc_phiPad_r->size(); ++i) {
      double extPhi = propagatorOld->propagationMiddleBarrel(ptId, etaId, phiId, charge).Y();
      double hitPhi = atan2(details->MiddleRpc_phiPad_y->at(i),details->MiddleRpc_phiPad_x->at(i));

      m_hitOld_rpc_middle_dPhi.push_back( dAngle(hitPhi, extPhi) );
    }

    // TGC ---------------------------------------

    // wire
    for(int i=0; i<(int)details->InnerTgc_rhoHit_r->size(); ++i) {
      double extEta = propagatorOld->propagationInnerEndCap(ptId, etaId, phiId, charge).X();
      double hitEta = getEta( details->InnerTgc_rhoHit_r->at(i), details->InnerTgc_rhoHit_z->at(i));
      double extPhi = propagatorOld->propagationInnerEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->InnerTgc_rhoHit_phi->at(i);

      m_hitOld_tgc_wire_inner_dEta.push_back( hitEta - extEta );
      m_hitOld_tgc_wire_inner_dPhi.push_back( dAngle( hitPhi , extPhi ) );

    }
    for(int i=0; i<(int)details->MiddleTgc_rhoHit_r->size(); ++i) {
      double extEta = propagatorOld->propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
      double hitEta = getEta( details->MiddleTgc_rhoHit_r->at(i), details->MiddleTgc_rhoHit_z->at(i));
      double extPhi = propagatorOld->propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->MiddleTgc_rhoHit_phi->at(i);

      m_hitOld_tgc_wire_middle_dEta.push_back( hitEta - extEta );
      m_hitOld_tgc_wire_middle_dPhi.push_back( dAngle( hitPhi , extPhi ) );

    }

    // strip
    for(int i=0; i<(int)details->InnerTgc_phiHit_r->size(); ++i) {
      double extPhi = propagatorOld->propagationInnerEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->InnerTgc_phiHit_phi->at(i);

      m_hitOld_tgc_strip_inner_dPhi.push_back( dAngle( hitPhi , extPhi ) );

    }
    for(int i=0; i<(int)details->MiddleTgc_phiHit_r->size(); ++i) {
      double extEta = propagatorOld->propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
      double extPhi = propagatorOld->propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();
      double hitPhi = details->MiddleTgc_phiHit_phi->at(i);

      if( fabs(extEta) <= 1.7 )
        m_hitOld_tgc_strip_middle_dPhi.push_back( dAngle( hitPhi , extPhi ) );
      else
        m_hitOld_tgc_strip_middle2_dPhi.push_back( dAngle( hitPhi , extPhi ) );

    }

    /// fill the histograms ========================================================

    fillAll();

  } // end of the loop
  log.info("******************************************************************");

  // calculate average pT in each pT bin
  for(int j=0; j<4; ++j) {
    avgPt[j] /= nPt[j];
  }

  //drawing ------------------------------

  std::vector<std::string> names;
  names.push_back("Hit_Endcap_mdt_inner_dEta");
  names.push_back("Hit_Endcap_mdt_middle_dEta");
  names.push_back("Hit_Barrel_mdt_inner_dEta");
  names.push_back("Hit_Barrel_mdt_middle_dEta");
  names.push_back("Hit_rpc_middle_dEta");
  names.push_back("Hit_rpc_middle_dPhi");
  names.push_back("Hit_tgc_wire_inner_dEta");
  names.push_back("Hit_tgc_wire_middle_dEta");
  names.push_back("Hit_tgc_wire_inner_dPhi");
  names.push_back("Hit_tgc_wire_middle_dPhi");
  names.push_back("Hit_tgc_strip_inner_dPhi");
  names.push_back("Hit_tgc_strip_middle_dPhi");
  names.push_back("Hit_tgc_strip_middle2_dPhi");

  ofstream* fout = NULL;
  if( settings.find("SaveCutCone") != std::string::npos ) {
    // open output file
    fout = new ofstream ( "cut_cone.py" );
    (*fout) << "## Hit-track matching cut-cone constants. Generated by TrigDiMuonRootCalib" << std::endl << std::endl;
  }

  TCanvas* c = new TCanvas(("c_"+names.at(0)).c_str(), ("c_"+names.at(0)).c_str());
  c->Print("fig/cutCone/plots.ps[");

  for(int i=0; i<(int)names.size(); ++i) {
    std::vector<std::string> hists;

    // old parametrization
    if(isDefined( "Old"+names.at(i) ) ) {
      setFillStyle( "Old"+names.at(i) , kYellow);
      hists.push_back("Old"+names.at(i));
    }

    // loop through pT bins
    for(int j=0;j<5;++j) {
      if( isDefined(names.at(i)+iToStr(j)) ) {
        if(j<4) setMarkerStyle( names.at(i)+iToStr(j) , 20 + 2*j);
        if(j<4) getHist( names.at(i)+iToStr(j))->SetLineColor(2+j);
        hists.push_back( names.at(i)+iToStr(j) );
      }
    }

    // "Cut-cone" plot
    TGraphErrors* gr = new TGraphErrors(4);

    // draw histogram
    if(hists.size()!=0) {
      c = new TCanvas(("c_"+names.at(i)).c_str(), ("c_"+names.at(i)).c_str());
      drawHist(hists);

      if(i!=8 && i!=9) {
        // fits
        for(int j=0; j<4; ++j) {
          if( isDefined(names.at(i)+iToStr(j)) ) {
            double range = .3;
            if(i==3 && j==1) range = .15;
            if(i==6)         range = .1;
            if(i==7 && j==2) range = .1;
            TF1* fitGaus = new TF1(("fitGaus_"+names.at(i)+iToStr(j)).c_str(), "gaus(0)", -range,range);
            fitGaus->SetParameters(getHist(names.at(i)+iToStr(j))->Integral(), getHist(names.at(i)+iToStr(j))->GetMean(), getHist(names.at(i)+iToStr(j))->GetRMS());
            fitGaus->SetParLimits(2,0.02,0.1);
            fitGaus->SetLineColor(2+j);
            getHist(names.at(i)+iToStr(j))->Fit(fitGaus, "0 R");
            if( settings.find("DrawGaus") != std::string::npos )
              fitGaus->Draw("same");

            // fill "cut-cone" plot
            gr->SetPoint(j, avgPt[j], fitGaus->GetParameter(2));
            gr->SetPointError(j, 0, fabs(fitGaus->GetParError(2)));
            gr->SetTitle( (std::string(getHist(names.at(i)+iToStr(j))->GetTitle())+";<p_{T}> (MeV);#sigma_{" + std::string(getHist(names.at(i)+iToStr(j))->GetXaxis()->GetTitle())+"}" ).c_str() );
          }
        }
      }

      // legend
      drawLegend(hists, 0.55,0.85,0.9,0.6);
      c->Print("fig/cutCone/plots.ps");

      if(i!=8 && i!=9) {
        // draw "cut-cone" plot
        c = new TCanvas(("c2_"+names.at(i)).c_str(), ("c2_"+names.at(i)).c_str());
        gr->Draw("pa");

        if( settings.find("SaveCutCone") != std::string::npos) {

          // fit "cut-cone" plot
          TF1* fitCone = new TF1(("fitCone_"+iToStr(i)).c_str(), "[0]/pow(x,[1])");
          fitCone->SetParameters(9.6, 0.7472);
          gr->Fit(fitCone);

          (*fout) << "Cone_"+names.at(i).substr(4)+"_c0=" << fitCone->GetParameter(0) << std::endl;
          (*fout) << "Cone_"+names.at(i).substr(4)+"_c1=" << fitCone->GetParameter(1) << std::endl << std::endl;
        }
        c->Print("fig/cutCone/plots.ps");
      }
    }
  }


  // hit matching in eta bins
  std::vector<std::string> names2;
  names2.push_back("Hit2_tgc_wire_inner_dPhi");
  names2.push_back("Hit2_tgc_strip_inner_dPhi");

  names2.push_back("Hit2_Endcap_mdt_middle_dEta");

  names2.push_back("Hit2_tgc_wire_middle_dEta");
  names2.push_back("Hit2_tgc_wire_middle_dPhi");
  names2.push_back("Hit2_tgc_strip_middle_dPhi");

  for(int i=0; i<(int)names2.size(); ++i) {
    std::vector<std::string> hists;

    // loop through eta bins
    int nBins = 6;
    if(i<2) nBins = 4;
    for(int j=0;j<nBins;++j) {
      if( isDefined(names2.at(i)+iToStr(j)) ) {
        if(j<nBins-1) setMarkerStyle( names2.at(i)+iToStr(j) , 20 + 2*j);
        if(j<nBins-1) getHist( names2.at(i)+iToStr(j))->SetLineColor(2+j);
        getHist(names2.at(i)+iToStr(j))->Scale(1./getHist(names2.at(i)+iToStr(j))->Integral());
        hists.push_back( names2.at(i)+iToStr(j) );
      }
    }

    // draw histogram
    if(hists.size()!=0) {
      c = new TCanvas(("c2_"+names2.at(i)).c_str(), ("c2_"+names2.at(i)).c_str());
      drawHist(hists);

      // fits
      if( settings.find("DrawGaus") != std::string::npos ) {
        for(int j=0; j<nBins; ++j) {
          if( isDefined(names2.at(i)+iToStr(j)) ) {
            double range = .3;
            if(i==2 && j==5) range = .2;
            TF1* fitGaus = new TF1(("fitGaus2_"+names2.at(i)+iToStr(j)).c_str(), "gaus(0)", -range,range);
            fitGaus->SetParameters(getHist(names2.at(i)+iToStr(j))->Integral(), getHist(names2.at(i)+iToStr(j))->GetMean(), getHist(names2.at(i)+iToStr(j))->GetRMS());
            fitGaus->SetParLimits(2,0.02,0.1);
            fitGaus->SetLineColor(2+j);
            getHist(names2.at(i)+iToStr(j))->Fit(fitGaus, "R", "same");
//             fitGaus->Draw("same");
          }
        }
      }

      // legend
      drawLegend(hists, 0.55,0.85,0.9,0.6);
      c->Print("fig/cutCone/plots.ps");
    }
  }


  // Hits possition
  c = new TCanvas("c_hitsPos","c_hitsPos");

  TH2F* axs = new TH2F("axs","Hits Position;Z (m); R (m)",100,-20,20,100,-3,13);
  axs->SetStats(0);
  axs->Draw();

  // TGC middle
  TGraph* g_tgcMiddleHits = new TGraph(tgcMiddleHits_r.size());
  for(int i=0; i<(int)tgcMiddleHits_r.size(); ++i) {
    g_tgcMiddleHits->SetPoint(i, tgcMiddleHits_z.at(i)/1000, tgcMiddleHits_r.at(i)/1000);
  }
  g_tgcMiddleHits->SetTitle("Hits Position;Z (m); R (m)");
  g_tgcMiddleHits->SetMarkerColor(kMagenta);
  g_tgcMiddleHits->SetFillColor(kMagenta);
  g_tgcMiddleHits->SetMarkerSize(0.2);
  g_tgcMiddleHits->Draw("p");

  // TGC inner
  TGraph* g_tgcInnerHits = new TGraph(tgcInnerHits_r.size());
  for(int i=0; i<(int)tgcInnerHits_r.size(); ++i) {
    g_tgcInnerHits->SetPoint(i, tgcInnerHits_z.at(i)/1000, tgcInnerHits_r.at(i)/1000);
  }
  g_tgcInnerHits->SetMarkerColor(kRed);
  g_tgcInnerHits->SetFillColor(kRed);
  g_tgcInnerHits->SetMarkerSize(0.2);
  g_tgcInnerHits->Draw("p");

  // MDT middle
  TGraph* g_mdtMiddleHits = new TGraph(mdtMiddleHits_r.size());
  for(int i=0; i<(int)mdtMiddleHits_r.size(); ++i) {
    g_mdtMiddleHits->SetPoint(i, mdtMiddleHits_z.at(i)/100, mdtMiddleHits_r.at(i)/100);
  }
  g_mdtMiddleHits->SetMarkerColor(kBlack);
  g_mdtMiddleHits->SetFillColor(kBlack);
  g_mdtMiddleHits->SetMarkerSize(0.2);
  g_mdtMiddleHits->Draw("p");

  // MDT inner
  TGraph* g_mdtInnerHits = new TGraph(mdtInnerHits_r.size());
  for(int i=0; i<(int)mdtInnerHits_r.size(); ++i) {
    g_mdtInnerHits->SetPoint(i, mdtInnerHits_z.at(i)/100, mdtInnerHits_r.at(i)/100);
  }
  g_mdtInnerHits->SetMarkerColor(kBlue);
  g_mdtInnerHits->SetFillColor(kBlue);
  g_mdtInnerHits->SetMarkerSize(0.2);
  g_mdtInnerHits->Draw("p");

  // RPC middle
  TGraph* g_rpcMiddleHits = new TGraph(rpcMiddleHits_r.size());
  for(int i=0; i<(int)rpcMiddleHits_r.size(); ++i) {
    g_rpcMiddleHits->SetPoint(i, rpcMiddleHits_z.at(i)/100, rpcMiddleHits_r.at(i)/100);
  }
  g_rpcMiddleHits->SetMarkerColor(kGreen);
  g_rpcMiddleHits->SetFillColor(kGreen);
  g_rpcMiddleHits->SetMarkerSize(0.2);
  g_rpcMiddleHits->Draw("p");

  TLegend* leg = new TLegend(0.3,0.35, 0.6, 0.2);
  leg->AddEntry(g_mdtInnerHits,  "MDT Inner Hits","f");
  leg->AddEntry(g_mdtMiddleHits, "MDT Middle Hits","f");
  leg->AddEntry(g_tgcInnerHits,  "TGC Inner Hits","f");
  leg->SetFillColor(kWhite);
  leg->Draw("same");

  TLegend* leg2 = new TLegend(0.6,0.35, 0.9, 0.25);
  leg2->AddEntry(g_tgcMiddleHits, "TGC Middle Hits","f");
  leg2->AddEntry(g_rpcMiddleHits, "RPC Middle Hits","f");
  leg2->SetFillColor(kWhite);
  leg2->Draw("same");

  c->Print("fig/cutCone/plots.ps");

  // Hits possition xy
  c = new TCanvas("c_hitsPos2","c_hitsPos2");
  TGraph* g_tgcInnerHits2 = new TGraph(tgcInnerHits_r.size());
  for(int i=0; i<(int)tgcInnerHits_r.size(); ++i) {
    g_tgcInnerHits2->SetPoint(i, tgcInnerHits_r.at(i)/1000*cos(tgcInnerHits_phi.at(i)), tgcInnerHits_r.at(i)/1000*sin(tgcInnerHits_phi.at(i)));
  }

  TH2F* axs3 = new TH2F("axs3","Hits Position: TGC Inner;X (m); Y (m)",100,-13,13,100,-13,13);
  axs3->SetStats(0);
  axs3->Draw();

  g_tgcInnerHits2->SetMarkerColor(kRed);
  g_tgcInnerHits2->SetFillColor(kRed);
  g_tgcInnerHits2->SetMarkerSize(0.2);
  g_tgcInnerHits2->Draw("p");

  new TCanvas("c_hitsPos3","c_hitsPos3");
  TGraph* g_tgcMiddleHits3 = new TGraph(tgcMiddleHits_r.size());
  for(int i=0; i<(int)tgcMiddleHits_r.size(); ++i) {
    g_tgcMiddleHits3->SetPoint(i, tgcMiddleHits_r.at(i)/1000*cos(tgcMiddleHits_phi.at(i)), tgcMiddleHits_r.at(i)/1000*sin(tgcMiddleHits_phi.at(i)));
  }

  TH2F* axs4 = new TH2F("axs4","Hits Position: TGC Middle;X (m); Y (m)",100,-13,13,100,-13,13);
  axs4->SetStats(0);
  axs4->Draw();

  g_tgcMiddleHits3->SetMarkerColor(kMagenta);
  g_tgcMiddleHits3->SetFillColor(kMagenta);
  g_tgcMiddleHits3->SetMarkerSize(0.2);
  g_tgcMiddleHits3->Draw("p");

  c->Print("fig/cutCone/plots.ps");


  c->Print("fig/cutCone/plots.ps]");

//   TCanvas* c = new TCanvas("c","c");
//   c->Print("hits.ps[");
//
//   for(int i=0; i<50 && i<(int)muonPhi.size(); ++i) {
//
//     std::vector<double> tmpPhi = hitPhi.at(i);
//     std::vector<double> tmpR   = hitR.at(i);
//
//     TH2F* axs2 = new TH2F(("axs2"+iToStr(i)).c_str(),"Hits Position;X (m); Y (m)",100,-13,13,100,-13,13);
//     axs2->SetStats(0);
//     axs2->Draw();
//
//     TGraph* g = new TGraph(tmpPhi.size());
//     for(int j=0; j<(int)tmpPhi.size(); ++j) {
//       g->SetPoint(j, tmpR.at(j)/1000*cos(tmpPhi.at(j)), tmpR.at(j)/1000*sin(tmpPhi.at(j)));
// //       log.info(fToStr(tmpR.at(j)));
//     }
//
//     g->SetMarkerColor(kRed);
//     g->SetFillColor(kRed);
//     g->SetMarkerSize(0.5);
//     g->Draw("p");
//
//     TLine* ll;
//     ll= new TLine(0,0,13*cos(muonPhi.at(i)), 13*sin(muonPhi.at(i)));
//     ll->Draw();
//
//     ll= new TLine(0,0,13*cos(muonExtPhi.at(i)), 13*sin(muonExtPhi.at(i)));
//     ll->SetLineStyle(2);
//     ll->Draw();
//
//     c->Print("hits.ps");
//
//   }
//
//   c->Print("hits.ps]");

}
///===================================================
void TrigDiMuonRootCalib::convert(std::string settings, std::string cutStr) {
  Log log("TrigDiMuonRootCalib::convert", ol());

  log.info("Settings: " + settings);
  log.info("cuts:     " + cutStr);

  std::string outFile = "TrigDiMuonCalibTree.root";
  getValue(settings, "OutFileName=", outFile);

  // create output tree
  TFile* f = TFile::Open(outFile.c_str(), "RECREATE");
  TTree* outTree = new TTree("TrigDiMuonCalibTree","TrigDiMuonCalib tree");

  // create branches
  Int_t           RunNumber;
  Int_t           LumiBlock;
  Int_t           EventNumber;
  double          pt;
  double          eta;
  double          phi;
  double          charge;
  vector<double>  *InnerMdt_eta   = new std::vector<double>;
  vector<double>  *InnerMdt_phi   = new std::vector<double>;
  vector<double>  *MiddleMdt_eta  = new std::vector<double>;
  vector<double>  *MiddleMdt_phi  = new std::vector<double>;
  vector<double>  *Csc_eta        = new std::vector<double>;
  vector<double>  *Csc_phi        = new std::vector<double>;
  vector<int>     *Csc_type       = new std::vector<int>;
  vector<double>  *MiddleRpc_eta  = new std::vector<double>;
  vector<double>  *MiddleRpc_phi  = new std::vector<double>;
  vector<int>     *MiddleRpc_type = new std::vector<int>;
  vector<double>  *InnerTgc_eta   = new std::vector<double>;
  vector<double>  *InnerTgc_phi   = new std::vector<double>;
  vector<int>     *InnerTgc_type  = new std::vector<int>;
  vector<double>  *MiddleTgc_eta  = new std::vector<double>;
  vector<double>  *MiddleTgc_phi  = new std::vector<double>;
  vector<int>     *MiddleTgc_type = new std::vector<int>;

  outTree->Branch("RunNumber", &RunNumber);
  outTree->Branch("LumiBlock", &LumiBlock);
  outTree->Branch("EventNumber", &EventNumber);
  outTree->Branch("Pt",     &pt);
  outTree->Branch("Eta",    &eta);
  outTree->Branch("Phi",    &phi);
  outTree->Branch("Charge", &charge);
  outTree->Branch("InnerMdt_eta", &InnerMdt_eta);
  outTree->Branch("InnerMdt_phi", &InnerMdt_phi);
  outTree->Branch("MiddleMdt_eta", &MiddleMdt_eta);
  outTree->Branch("MiddleMdt_phi", &MiddleMdt_phi);
  outTree->Branch("Csc_eta", &Csc_eta);
  outTree->Branch("Csc_phi", &Csc_phi);
  outTree->Branch("Csc_type", &Csc_type);
  outTree->Branch("MiddleRpc_eta", &MiddleRpc_eta);
  outTree->Branch("MiddleRpc_phi", &MiddleRpc_phi);
  outTree->Branch("MiddleRpc_type", &MiddleRpc_type);
  outTree->Branch("InnerTgc_eta", &InnerTgc_eta);
  outTree->Branch("InnerTgc_phi", &InnerTgc_phi);
  outTree->Branch("InnerTgc_type", &InnerTgc_type);
  outTree->Branch("MiddleTgc_eta", &MiddleTgc_eta);
  outTree->Branch("MiddleTgc_phi", &MiddleTgc_phi);
  outTree->Branch("MiddleTgc_type", &MiddleTgc_type);

  std::string muonCollection = "";
  getValue(settings, "MuonCollection=", muonCollection);
  if(muonCollection == "") {
    log.error("No muon collection specified");
    return;
  }

  //loop over files
  log.info("******************************************************************");
  log.info("** BEGINNING OF THE LOOP                                        **");
  log.info("******************************************************************");
  while( iterateFiles() ) {
    // trees
    TTree *tTracks, *tMuons, *tMuonHits;

    // load trees
    tTracks   = loadTree(muonCollection+"Tracks");
    tMuons    = loadTree(muonCollection);
    tMuonHits = loadTree(muonCollection+"Hits");

    if(tTracks == NULL || tMuons == NULL || tMuonHits == NULL) {
      log.error("Missing trees");
      continue;
    }

    if(tTracks->GetEntries() != tMuons->GetEntries()) {
      log.error("Wrong number of entries: "+iToStr(tTracks->GetEntries())+" != "+iToStr(tMuons->GetEntries()) );
      continue;
    }

    // prepare trees
    m_ntupleReader.prepareTree(muonCollection, tMuons);
    m_ntupleReader.prepareTree(muonCollection+"Tracks", tTracks);

    // muon hits
    Hits* muonHits  = new Hits(tMuonHits);

    // tracks and muons
    TrackBag tracks;
    MuonBag muons;

//     // FIXME temporary fix till I have new a ntuple
//     std::string oldFile = currentFileName().replace( currentFileName().find("2010-06-18"), 10, "2010-06-16" );
//     oldFile = oldFile.replace( oldFile.find(".10."), 4, ".09." );
//     log.info(oldFile);
//
//     TFile* fOld = TFile::Open(oldFile.c_str());
//     TTree* tMuonHitsOld = (TTree*)fOld->Get((muonCollection+"Hits").c_str());
//     if(tMuonHitsOld == NULL) {
//       log.error("Old Tree can't be loaded");
//       return;
//     }
//
//     // sanity check
//     if(tMuonHitsOld->GetEntries() != tMuonHits->GetEntries() ) {
//       log.error("Old and new ntuples have different number of entries");
//       continue;
//     }
//
//     Hits* muonHitsOld = new Hits(tMuonHitsOld, true);
//
//     //FIXME end of hack

    // event info
    int oldRun = -1;
    int oldEvt = -1;

    // loop over entries
    int step = tMuonHits->GetEntries() / 20;
    for(int entry=0; entry<tMuonHits->GetEntries(); ++entry) {
      if(step == 0 || entry % step == 0) log.info("   ===> Entry " + iToStr(entry));

      // load muon hits
      muonHits->GetEntry(entry);

//       // FIXME temporary fix till I have new a ntuple
//       muonHitsOld->GetEntry(entry);

      // get tracks and muons
      if(oldRun != muonHits->RunNumber || oldEvt != muonHits->EventNumber) {
        tracks = m_ntupleReader.getTracks(muonHits->RunNumber, muonHits->EventNumber, muonCollection+"Tracks", tTracks);
        muons  = m_ntupleReader.getMuons (muonHits->RunNumber, muonHits->EventNumber, muonCollection,          tMuons);
        // update event info
        oldRun = muonHits->RunNumber;
        oldEvt = muonHits->EventNumber;
      }

      // get muon
      Muon muon;
      if( !m_ntupleReader.findMuonByIndex(muons, muonHits->MuonIndex, muon) ) {
        log.error("Muon wasn't found");
        continue;
      }

      // get track
      Track track;
      if( !m_ntupleReader.findTrackByIndex(tracks, muon.inDetTrackIndex(), track) ) {
        log.error("Track wasn't found");
        continue;
      }

      // selection
      if( !muon.isCombinedMuon() ) continue;

      /// fill simple tree ************
      // cleanup
      log.debug("Cleanup");
      InnerMdt_eta->clear();
      InnerMdt_phi->clear();
      MiddleMdt_eta->clear();
      MiddleMdt_phi->clear();
      Csc_eta->clear();
      Csc_phi->clear();
      Csc_type->clear();
      MiddleRpc_eta->clear();
      MiddleRpc_phi->clear();
      MiddleRpc_type->clear();
      InnerTgc_eta->clear();
      InnerTgc_phi->clear();
      InnerTgc_type->clear();
      MiddleTgc_eta->clear();
      MiddleTgc_phi->clear();
      MiddleTgc_type->clear();

      // copy values
      log.debug("Fill values");
      RunNumber   = muonHits->RunNumber;
      LumiBlock   = muonHits->LumiBlock;
      EventNumber = muonHits->EventNumber;
      pt          = fabs(track.pt());
      eta         = track.eta();
      phi         = track.phi0();
      charge      = track.qOverP() > 0 ? 1. : -1.;
      InnerMdt_eta  ->assign(muonHits->InnerMdt_eta->begin() , muonHits->InnerMdt_eta->end() );
      InnerMdt_phi  ->assign(muonHits->InnerMdt_phi->begin() , muonHits->InnerMdt_phi->end() );
      MiddleMdt_eta ->assign(muonHits->MiddleMdt_eta->begin() , muonHits->MiddleMdt_eta->end() );
      MiddleMdt_phi ->assign(muonHits->MiddleMdt_phi->begin() , muonHits->MiddleMdt_phi->end() );
      Csc_eta       ->assign(muonHits->Csc_eta->begin() , muonHits->Csc_eta->end() );
      Csc_phi       ->assign(muonHits->Csc_phi->begin() , muonHits->Csc_phi->end() );
      Csc_type      ->assign(muonHits->Csc_type->begin() , muonHits->Csc_type->end() );
      MiddleRpc_eta ->assign(muonHits->MiddleRpc_eta->begin() , muonHits->MiddleRpc_eta->end() );
      MiddleRpc_phi ->assign(muonHits->MiddleRpc_phi->begin() , muonHits->MiddleRpc_phi->end() );
      MiddleRpc_type->assign(muonHits->MiddleRpc_type->begin() , muonHits->MiddleRpc_type->end() );
      InnerTgc_eta  ->assign(muonHits->InnerTgc_eta->begin() , muonHits->InnerTgc_eta->end() );
      InnerTgc_phi  ->assign(muonHits->InnerTgc_phi->begin() , muonHits->InnerTgc_phi->end() );
      InnerTgc_type ->assign(muonHits->InnerTgc_type->begin() , muonHits->InnerTgc_type->end() );
      MiddleTgc_eta ->assign(muonHits->MiddleTgc_eta->begin() , muonHits->MiddleTgc_eta->end() );
      MiddleTgc_phi ->assign(muonHits->MiddleTgc_phi->begin() , muonHits->MiddleTgc_phi->end() );


      MiddleTgc_type->assign(muonHits->MiddleTgc_type->begin() , muonHits->MiddleTgc_type->end() );

      // fill the tree
      log.debug("Fill tree");
      outTree->Fill();

    } // end of loop over entries

  } // end of loop over files

  // save output tree
  f->cd();
  outTree->Write();
  f->Close();

}
///===================================================
void TrigDiMuonRootCalib::calibrate(std::string settings, std::string cutStr) {
  Log log("TrigDiMuonRootCalib::calibrate", ol());

  log.info("Settings: " + settings);
  log.info("cuts:     " + cutStr);

  // extrapolator
  TrigDiMuonExtrapolator* propagator = new TrigDiMuonExtrapolator(ol(), settings);

  // import propagator constants
  propagator->read("constants.py");

  //loop over files
  log.info("******************************************************************");
  log.info("** BEGINNING OF THE LOOP                                        **");
  log.info("******************************************************************");
  while( iterateFiles() ) {
    // trees
    TTree *tMuonHits;

    // load trees
    tMuonHits   = loadTree("TrigDiMuonCalibTree");

    if(tMuonHits == NULL) {
      log.error("Missing tree");
      continue;
    }

    // muon hits
    Calib* muonHits  = new Calib(tMuonHits);

    // loop over entries
    int step = tMuonHits->GetEntries() / 20;
    for(int entry=0; entry<tMuonHits->GetEntries(); ++entry) {
      if(step == 0 || entry % step == 0) log.info("   ===> Entry " + iToStr(entry));

      // load muon hits
      muonHits->GetEntry(entry);

      // fill the histograms
      propagator->fillInner(muonHits);
      propagator->fillMiddle(muonHits);

    } // end of loop over entries

  } // end of loop over files

  // draw histograms
  propagator->drawInner(settings);
  propagator->drawMiddle(settings);

  // save fitted constants
  if(settings.find("fitInner") != std::string::npos || settings.find("fitMiddle") != std::string::npos)
    propagator->save("constants.py");

}
///===================================================
// void TrigDiMuonRootCalib::convertJpsi(std::string chain, std::string settings, std::string cutStr) {
//   Log log("TrigDiMuonRootCalib::convertJpsi", ol());
//
//   log.info("Settings: " + settings);
//   log.info("cuts:     " + cutStr);
//
//   // create selection algorithm
//   CutsJpsi cuts(ol(), cutStr);
//
//   std::string outFile = "TrigDiMuonJpsiTree.root";
//   getValue(settings, "OutFileName=", outFile);
//
//   // create output tree
//   TFile* f = TFile::Open(outFile.c_str(), "RECREATE");
//   TTree* outTree = new TTree("TrigDiMuonJpsiTree","TrigDiMuon Jpsi Tree");
//
//   //loop over files
//   log.info("******************************************************************");
//   log.info("** BEGINNING OF THE LOOP                                        **");
//   log.info("******************************************************************");
//   while( iterateFiles() ) {
//     // trees
//     TTree *tJpsi, *tDetails, *tTracks, *tMuons, *tMuonHits, *tTrig;
//
//     // load trees
//     tJpsi     = loadTree("Jpsi");
//     tChain    = loadTree(chain);
//     tDetails  = loadTree(chain+"_Details");
//     tTracks   = loadTree("MuidMuonCollectionTracks");
//     tMuons    = loadTree("MuidMuonCollection");
//     tTrig     = loadTree("TRIG");
//
//     if(tJpsi == NULL || tChain || tDetails == NULL || tTracks == NULL || tMuons == NULL || tTrig) {
//       log.error("Missing trees");
//       continue;
//     }
//
//     if(tTracks->GetEntries() != tMuons->GetEntries() ||
//        tJpsi->GetEntries() != tChain->GetEntries() ||
//        tJpsi->GetEntries() != tDetails->GetEntries())
//     {
//       log.error("Wrong number of entries" );
//       continue;
//     }
//
//     // prepare trees
//     m_ntupleReader.prepareTree(muonCollection, tMuons);
//     m_ntupleReader.prepareTree(muonCollection+"Tracks", tTracks);
//
//     Chain* trigger   = new Chain(tChain, chain, true, false);
//     Details* details = new Details(tDetails);
//
//     // loop over entries
//     int step = tJpsi->GetEntries() / 20;
//     for(int entry=0; entry<tJpsi->GetEntries(); ++entry) {
//       if(step == 0 || entry % step == 0) log.info("   ===> Entry " + iToStr(entry));
//
//       // load entries
//       BCompositeParticle jpsi = m_ntupleReader.readParticleTree(entry, "Jpsi", tJpsi);
//       trigger->GetEntry(entry);
//       details->GetEntry(entry);
//
//       // load muons and tracks
//       TrackBag tracks          = m_ntupleReader.getTracks  (jpsi.runNumber(), jpsi.eventNumber(), "MuidMuonCollectionTracks", tTracks       );
//       MuonBag muons            = m_ntupleReader.getMuons   (jpsi.runNumber(), jpsi.eventNumber(), "MuidMuonCollection",       tMuons        );
//       TriggerBag trigDecisions = m_ntupleReader.getTriggers(jpsi.runNumber(), jpsi.eventNumber(), "TRIG",                     trigDecisions );
//
//       /// level-1 jpsi =============================================================
//
//       if( !cuts.selectLevel1(trigDecisions) ) continue;
//
//       /// offline jpsi =============================================================
//
//       log.debug("offline jpsi analysis");
//
//       // J/psi selection
//       if( !cuts.selectJpsi(m_ntupleReader, jpsi, tracks,  muons) ) continue;
//
//
//     } // end of loop over entries
//
//
//     // tracks and muons
//     TrackBag tracks;
//     MuonBag muons;
//
//     // event info
//     int oldRun = -1;
//     int oldEvt = -1;
//
//     // loop over entries
//     int step = tMuonHits->GetEntries() / 20;
//     for(int entry=0; entry<tMuonHits->GetEntries(); ++entry) {
//       if(step == 0 || entry % step == 0) log.info("   ===> Entry " + iToStr(entry));
//
//       // load muon hits
//       muonHits->GetEntry(entry);
//
//       // get tracks and muons
//       if(oldRun != muonHits->RunNumber || oldEvt != muonHits->EventNumber) {
//         tracks = m_ntupleReader.getTracks(muonHits->RunNumber, muonHits->EventNumber, muonCollection+"Tracks", tTracks);
//         muons  = m_ntupleReader.getMuons (muonHits->RunNumber, muonHits->EventNumber, muonCollection,          tMuons);
//         // update event info
//         oldRun = muonHits->RunNumber;
//         oldEvt = muonHits->EventNumber;
//       }
//
//       // get muon
//       Muon muon;
//       if( !m_ntupleReader.findMuonByIndex(muons, muonHits->MuonIndex, muon) ) {
//         log.error("Muon wasn't found");
//         continue;
//       }
//
//       // get track
//       Track track;
//       if( !m_ntupleReader.findTrackByIndex(tracks, muon.inDetTrackIndex(), track) ) {
//         log.error("Track wasn't found");
//         continue;
//       }
//
//       // selection
//       if( !muon.isCombinedMuon() ) continue;
//
//       /// fill simple tree ************
//       // cleanup
//       log.debug("Cleanup");
//       InnerMdt_eta->clear();
//       InnerMdt_phi->clear();
//       MiddleMdt_eta->clear();
//       MiddleMdt_phi->clear();
//       Csc_eta->clear();
//       Csc_phi->clear();
//       MiddleRpc_eta->clear();
//       MiddleRpc_phi->clear();
//       InnerTgc_eta->clear();
//       InnerTgc_phi->clear();
//       MiddleTgc_eta->clear();
//       MiddleTgc_phi->clear();
//
//       // copy values
//       log.debug("Fill values");
//       RunNumber   = muonHits->RunNumber;
//       LumiBlock   = muonHits->LumiBlock;
//       EventNumber = muonHits->EventNumber;
//       pt          = fabs(track.pt());
//       eta         = track.eta();
//       phi         = track.phi0();
//       charge      = track.qOverP() > 0 ? 1. : -1.;
//       InnerMdt_eta  ->assign(muonHits->InnerMdt_eta->begin() , muonHits->InnerMdt_eta->end() );
//       InnerMdt_phi  ->assign(muonHits->InnerMdt_phi->begin() , muonHits->InnerMdt_phi->end() );
//       MiddleMdt_eta ->assign(muonHits->MiddleMdt_eta->begin() , muonHits->MiddleMdt_eta->end() );
//       MiddleMdt_phi ->assign(muonHits->MiddleMdt_phi->begin() , muonHits->MiddleMdt_phi->end() );
//       Csc_eta       ->assign(muonHits->Csc_eta->begin() , muonHits->Csc_eta->end() );
//       Csc_phi       ->assign(muonHits->Csc_phi->begin() , muonHits->Csc_phi->end() );
//       MiddleRpc_eta ->assign(muonHits->MiddleRpc_eta->begin() , muonHits->MiddleRpc_eta->end() );
//       MiddleRpc_phi ->assign(muonHits->MiddleRpc_phi->begin() , muonHits->MiddleRpc_phi->end() );
//       InnerTgc_eta  ->assign(muonHits->InnerTgc_eta->begin() , muonHits->InnerTgc_eta->end() );
//       InnerTgc_phi  ->assign(muonHits->InnerTgc_phi->begin() , muonHits->InnerTgc_phi->end() );
//       MiddleTgc_eta ->assign(muonHits->MiddleTgc_eta->begin() , muonHits->MiddleTgc_eta->end() );
//       MiddleTgc_phi ->assign(muonHits->MiddleTgc_phi->begin() , muonHits->MiddleTgc_phi->end() );
//
//       // fill the tree
//       log.debug("Fill tree");
//       outTree->Fill();
//
//     } // end of loop over entries
//
//   } // end of loop over files
//
//   // save output tree
//   f->cd();
//   outTree->Write();
//   f->Close();
//
// }
///===================================================
void TrigDiMuonRootCalib::testExtrapolator(std::string chain) {
  Log log("TrigDiMuonRootCalib::iterateDetails", ol());

  // histograms
  TH1F* h_ib_dPhi = new TH1F("h_ib_dPhi","Inner Barrel;#delta#phi",100,-1,1);
  TH1F* h_ie_dPhi = new TH1F("h_ie_dPhi","Inner Endcap;#delta#phi",100,-1,1);

  TH1F* h_mb_dPhi = new TH1F("h_mb_dPhi","Middle Barrel;#delta#phi",100,-1,1);
  TH1F* h_me_dPhi = new TH1F("h_me_dPhi","Middle Endcap;#delta#phi",100,-1,1);
  TH1F* h_mb_dEta = new TH1F("h_mb_dEta","Middle Barrel;#delta#phi",100,-1,1);
  TH1F* h_me_dEta = new TH1F("h_me_dEta","Middle Endcap;#delta#phi",100,-1,1);

  TH1F* h_match    = new TH1F("h_match","Missmatched hits",6,0,6);
  TH1F* h_mismatch = new TH1F("h_mismatch","Missmatched hits",6,0,6);

  TH1F* h_nDetails = new TH1F("h_nDetails","Details multiplicity",10,0,10);

  // extrapolator
  TrigDiMuonExtrapolator* propagator = new TrigDiMuonExtrapolator(ol(), "");

  // import propagator constants
  propagator->read("constants_original.py");
  propagator->readCutCone("cut_cone_original.py");

//   std::string detailsTreeName = "All_"+chain+"_Details";
//   std::string detailsTreeName = chain+"_Details";
  std::string detailsTreeName = "MuonFeatureDetails";

  // declare trees
  TTree *tDetails;
  Details* details;

  // event info
  int oldRun = -1;
  int oldEvt = -1;
  int nDetails = 0;

  //loop over all entries and all files
  log.info("******************************************************************");
  log.info("** BEGINNING OF THE LOOP                                        **");
  log.info("******************************************************************");
  m_nEntries = 0;
  for(resetIterator();
      iterateDetails(detailsTreeName, &tDetails, &details);
      ++m_nEntries)
  {

    if(m_nEntries % 10000 == 0) log.info("   ===> Entry " + iToStr(m_nEntries));


    if(oldRun!=details->RunNumber || oldEvt!=details->EvtNumber) {
      h_nDetails->Fill(nDetails);
      nDetails = 1;
      oldRun = details->RunNumber;
      oldEvt = details->EvtNumber;
    }else{
      ++nDetails;
    }

    double ptId   = details->Pt;
    double etaId  = details->Eta;
    double phiId  = details->Phi;
    double charge = details->Charge == 1 ? 1. : -1.; //TMath::IsNaN(details->Charge) ? -1. : 1.;
    log.debug("Details: pt = "+fToStr(ptId)+", eta = "+fToStr(etaId)+", phi = "+fToStr(phiId)+", charge = "+fToStr(charge));

    double extEta, extPhi;

    // inner station
    // barrel
    if(fabs(etaId)<1.1) {
      extPhi = propagator->propagationInnerBarrel(ptId,etaId ,phiId,charge).Y();
      h_ib_dPhi->Fill( dAngle(extPhi, details->InnerBarrel_extPhi) );

      extPhi = propagator->propagationMiddleBarrel(ptId,etaId,phiId,charge).Y();
      h_mb_dPhi->Fill( dAngle(extPhi, details->MiddleBarrel_extPhi) );

      extEta = propagator->propagationMiddleBarrel(ptId,etaId,phiId,charge).X();
      h_mb_dEta->Fill( dAngle(extEta, details->MiddleBarrel_extEta) );

    }

    // endcap
    if(fabs(etaId)>0.9) {
      extPhi = propagator->propagationInnerEndCap(ptId,etaId ,phiId,charge).Y();
      h_ie_dPhi->Fill( dAngle(extPhi, details->InnerEndcap_extPhi) );

      extPhi = propagator->propagationMiddleEndCap(ptId,etaId,phiId,charge).Y();
      h_me_dPhi->Fill( dAngle(extPhi, details->MiddleEndcap_extPhi) );

      extEta = propagator->propagationMiddleEndCap(ptId,etaId,phiId,charge).X();
      h_me_dEta->Fill( dAngle(extEta, details->MiddleEndcap_extEta) );

    }

    /// check hit-matching ********************
    int nMdt = 0;
    int nRpcPhi = 0;
    int nRpcEta = 0;
    int nTgcPhi = 0;
    int nTgcEta = 0;

    int nMdtOld = details->NAssociatedMdt;
    int nTgcEtaOld = details->NAssociatedRpcEta;
    int nTgcPhiOld = details->NAssociatedRpcPhi;
    int nRpcEtaOld = details->NAssociatedTgcRho;
    int nRpcPhiOld = details->NAssociatedTgcPhi;

    // details entry number
    int deInnMdt = 0;
    int deMidMdt = 0;
    int deRpcPhi = 0;
    int deRpcEta = 0;
    int deInnTgcPhi = 0;
    int deMidTgcPhi = 0;
    int deInnTgcEta = 0;
    int deMidTgcEta = 0;
    // MDT  -------------------------------------
    for(; deInnMdt<(int)details->InnerMdt_r->size(); ++deInnMdt) {
      if(propagator->matchMdtInner(details->InnerMdt_r->at(deInnMdt), details->InnerMdt_z->at(deInnMdt), ptId, etaId, phiId, charge))
        ++nMdt;
//       std::cout<<"Assoc: "<<details->InnerMdt_associated->at(deInnMdt)<<", "<<propagator->matchMdtInner(details->InnerMdt_r->at(deInnMdt), details->InnerMdt_z->at(deInnMdt), ptId, etaId, phiId, charge)<<std::endl;
//       if(details->InnerMdt_associated->at(deInnMdt))
//         ++nMdt;
    }

    for(; deMidMdt<(int)details->MiddleMdt_r->size(); ++deMidMdt) {
      if(propagator->matchMdtMiddle(details->MiddleMdt_r->at(deMidMdt), details->MiddleMdt_z->at(deMidMdt), ptId, etaId, phiId, charge))
        ++nMdt;
//       if(details->MiddleMdt_associated->at(deMidMdt))
//         ++nMdt;
    }

    // RPC ---------------------------------------
    for(; deRpcEta<(int)details->MiddleRpc_etaPad_r->size(); ++deRpcEta) {
      if(propagator->matchRpcEtaPad(details->MiddleRpc_etaPad_r->at(deRpcEta), details->MiddleRpc_etaPad_z->at(deRpcEta), ptId, etaId, phiId, charge))
        ++nRpcEta;
    }

    for(; deRpcPhi<(int)details->MiddleRpc_phiPad_x->size(); ++deRpcPhi) {
      if(propagator->matchRpcPhiPad(details->MiddleRpc_phiPad_x->at(deRpcPhi), details->MiddleRpc_phiPad_y->at(deRpcPhi), ptId, etaId, phiId, charge))
        ++nRpcPhi;
    }


    // TGC ---------------------------------------

    // wire
    for(; deInnTgcEta<(int)details->InnerTgc_rhoHit_r->size(); ++deInnTgcEta) {
      if(propagator->matchInnerTgcRhoHit(details->InnerTgc_rhoHit_r->at(deInnTgcEta), details->InnerTgc_rhoHit_z->at(deInnTgcEta), details->InnerTgc_rhoHit_phi->at(deInnTgcEta), ptId, etaId, phiId, charge))
        ++nTgcEta;
    }
    for(; deMidTgcEta<(int)details->MiddleTgc_rhoHit_r->size(); ++deMidTgcEta) {
      if(propagator->matchMiddleTgcRhoHit(details->MiddleTgc_rhoHit_r->at(deMidTgcEta), details->MiddleTgc_rhoHit_z->at(deMidTgcEta), details->MiddleTgc_rhoHit_phi->at(deMidTgcEta), ptId, etaId, phiId, charge))
        ++nTgcEta;
    }

    // strip
    for(; deInnTgcPhi<(int)details->InnerTgc_phiHit_r->size(); ++deInnTgcPhi) {
      if(propagator->matchInnerTgcPhiHit(details->InnerTgc_phiHit_phi->at(deInnTgcPhi), ptId, etaId, phiId, charge))
        ++nTgcPhi;
    }

    for(; deMidTgcPhi<(int)details->MiddleTgc_phiHit_r->size(); ++deMidTgcPhi) {
      if(propagator->matchMiddleTgcPhiHit(details->MiddleTgc_phiHit_phi->at(deMidTgcPhi), ptId, etaId, phiId, charge))
        ++nTgcPhi;
    }

    log.debug("MDT1   : "+iToStr(nMdt)+" x "+iToStr(nMdtOld));
    log.debug("RPC_eta: "+iToStr(nRpcEta)+" x "+iToStr(nRpcEtaOld)+"   RPC_phi: "+iToStr(nRpcPhi)+" x "+iToStr(nRpcPhiOld));
    log.debug("TGC_eta: "+iToStr(nTgcEta)+" x "+iToStr(nTgcEtaOld)+"   TGC_phi: "+iToStr(nTgcPhi)+" x "+iToStr(nTgcPhiOld));
    log.debug("");

  } // end of loop

  // draw
  new TCanvas("c1","c1");
  h_ib_dPhi->Draw();

  new TCanvas("c2","c2");
  h_mb_dPhi->Draw();

  new TCanvas("c3","c3");
  h_mb_dEta->Draw();

  new TCanvas("c4","c4");
  h_ie_dPhi->Draw();

  new TCanvas("c5","c5");
  h_me_dPhi->Draw();

  new TCanvas("c6","c6");
  h_me_dEta->Draw();

  new TCanvas("c7","c7");
  h_match->Draw();
  h_mismatch->SetFillColor(kYellow);
  h_mismatch->Draw("same");

  new TCanvas("c7","c7");
  h_nDetails->Draw();

}
///===================================================
void TrigDiMuonRootCalib::convertChain(std::string chain, std::string settings, std::string cutStr) {
  Log log("TrigDiMuonRootCalib::convertChain", ol());

  log.info("Settings: " + settings);
  log.info("cuts:     " + cutStr);

  // create selection algorithm
  CutsJpsi cuts(ol(), cutStr);

  std::string constantsFile = "constants.py";
  getValue(settings, "ConstantsFile=", constantsFile);

  std::string outFileName = chain+".root";
  getValue(settings, "OutputFile=", outFileName);

  // declare output ntuple branches
  // J/psi tree
  int runNumber, lumiBlock, eventNumber;
  double mass, pt, eta, phi;
  std::vector<int> *trigIndex;
  double trk_pt[2], trk_eta[2], trk_phi[2], trk_charge[2];
  double matchTrk_pt[2], matchTrk_eta[2], matchTrk_phi[2], matchTrk_charge[2];
  bool matchTrk_valid[2];

  trigIndex = new std::vector<int>;

  // trigger tree
  bool passTrigDec;
  double trigMass, chi2;
  double trkT_pt[2], trkT_eta[2], trkT_phi[2], trkT_charge[2];
  double trkT_nMdt[2], trkT_nTgc[2], trkT_nRpc[2];
  std::vector<double> *innerMdt_r[2], *innerMdt_z[2], *middleMdt_r[2], *middleMdt_z[2];
  std::vector<double> *etaRpc_r[2],*etaRpc_z[2], *phiRpc_x[2], *phiRpc_y[2];
  std::vector<double> *innerEtaTgc_r[2], *innerEtaTgc_z[2], *innerEtaTgc_phi[2],
                      *innerPhiTgc[2],
                      *middleEtaTgc_r[2], *middleEtaTgc_z[2], *middleEtaTgc_phi[2],
                      *middlePhiTgc[2];
  for(int i=0; i<2; ++i) {
    innerMdt_r[i]        = new std::vector<double>;
    innerMdt_z[i]        = new std::vector<double>;
    middleMdt_r[i]       = new std::vector<double>;
    middleMdt_z[i]       = new std::vector<double>;
    etaRpc_r[i]          = new std::vector<double>;
    etaRpc_z[i]          = new std::vector<double>;
    phiRpc_x[i]          = new std::vector<double>;
    phiRpc_y[i]          = new std::vector<double>;
    innerEtaTgc_r[i]     = new std::vector<double>;
    innerEtaTgc_z[i]     = new std::vector<double>;
    innerEtaTgc_phi[i]   = new std::vector<double>;
    innerPhiTgc[i]       = new std::vector<double>;
    middleEtaTgc_r[i]    = new std::vector<double>;
    middleEtaTgc_z[i]    = new std::vector<double>;
    middleEtaTgc_phi[i]  = new std::vector<double>;
    middlePhiTgc[i]      = new std::vector<double>;
  }

  std::vector<double>* muon_pt       = new std::vector<double>;
  std::vector<double>* muon_eta      = new std::vector<double>;
  std::vector<double>* muon_phi      = new std::vector<double>;
  std::vector<double>* muon_charge   = new std::vector<double>;
  std::vector<bool>*   muon_combined = new std::vector<bool>;

  // create branches
  TFile* fOut = TFile::Open(outFileName.c_str(),"RECREATE");

  // extrapolator
  TrigDiMuonExtrapolator* propagator = new TrigDiMuonExtrapolator(ol(), settings);
  propagator->read(constantsFile);

  // we pass throw the main loop two times. First time weprocess all the events,
  // second time only objects matched to the offline J/psi
  for(int doJpsi = 0; doJpsi < 2; ++doJpsi) {

    TTree* triggerTree = NULL;
    TTree* jpsiTree = NULL;
    TTree* muonTree = NULL;

    if(doJpsi) {
      jpsiTree = new TTree("Jpsi","Jpsi tree");
      jpsiTree->Branch("runNumber", &runNumber);
      jpsiTree->Branch("lumiBlock", &lumiBlock);
      jpsiTree->Branch("eventNumber", &eventNumber);
      jpsiTree->Branch("mass", &mass);
      jpsiTree->Branch("pt", &pt);
      jpsiTree->Branch("eta", &eta);
      jpsiTree->Branch("phi", &phi);
      jpsiTree->Branch("trigIndex", &trigIndex);
      jpsiTree->Branch("trk0_pt",      &(trk_pt[0]));
      jpsiTree->Branch("trk0_eta",     &(trk_eta[0]));
      jpsiTree->Branch("trk0_phi",     &(trk_phi[0]));
      jpsiTree->Branch("trk0_charge",  &(trk_charge[0]));
      jpsiTree->Branch("trk1_pt",      &(trk_pt[1]));
      jpsiTree->Branch("trk1_eta",     &(trk_eta[1]));
      jpsiTree->Branch("trk1_phi",     &(trk_phi[1]));
      jpsiTree->Branch("trk1_charge",  &(trk_charge[1]));
      jpsiTree->Branch("matchTrk0_pt",      &(matchTrk_pt[0]));
      jpsiTree->Branch("matchTrk0_eta",     &(matchTrk_eta[0]));
      jpsiTree->Branch("matchTrk0_phi",     &(matchTrk_phi[0]));
      jpsiTree->Branch("matchTrk0_charge",  &(matchTrk_charge[0]));
      jpsiTree->Branch("matchTrk0_valid",   &(matchTrk_valid[0]));
      jpsiTree->Branch("matchTrk1_pt",      &(matchTrk_pt[1]));
      jpsiTree->Branch("matchTrk1_eta",     &(matchTrk_eta[1]));
      jpsiTree->Branch("matchTrk1_phi",     &(matchTrk_phi[1]));
      jpsiTree->Branch("matchTrk1_charge",  &(matchTrk_charge[1]));
      jpsiTree->Branch("matchTrk1_valid",   &(matchTrk_valid[1]));

      triggerTree = new TTree("Trigger","Trigger tree");
    }else{
      muonTree    = new TTree("Muons","Muons");

      muonTree->Branch("runNumber", &runNumber);
      muonTree->Branch("lumiBlock", &lumiBlock);
      muonTree->Branch("eventNumber", &eventNumber);

      muonTree->Branch("muon_pt",       &muon_pt      );
      muonTree->Branch("muon_eta",      &muon_eta     );
      muonTree->Branch("muon_phi",      &muon_phi     );
      muonTree->Branch("muon_charge",   &muon_charge  );
      muonTree->Branch("muon_combined", &muon_combined);

      triggerTree = new TTree("AllTrigger","Trigger tree for all events");
    }

    // create trigger tree branches
    triggerTree->Branch("runNumber", &runNumber);
    triggerTree->Branch("lumiBlock", &lumiBlock);
    triggerTree->Branch("eventNumber", &eventNumber);
    triggerTree->Branch("passTrigDec", &passTrigDec);
    triggerTree->Branch("trigMass",     &trigMass);
    triggerTree->Branch("chi2",         &chi2);
    triggerTree->Branch("trk0_pt",      &(trkT_pt[0]));
    triggerTree->Branch("trk0_eta",     &(trkT_eta[0]));
    triggerTree->Branch("trk0_phi",     &(trkT_phi[0]));
    triggerTree->Branch("trk0_charge",  &(trkT_charge[0]));
    triggerTree->Branch("trk0_nMdt",    &(trkT_nMdt[0]));
    triggerTree->Branch("trk0_nTgc",    &(trkT_nTgc[0]));
    triggerTree->Branch("trk0_nRpc",    &(trkT_nRpc[0]));
    triggerTree->Branch("trk1_pt",      &(trkT_pt[1]));
    triggerTree->Branch("trk1_eta",     &(trkT_eta[1]));
    triggerTree->Branch("trk1_phi",     &(trkT_phi[1]));
    triggerTree->Branch("trk1_charge",  &(trkT_charge[1]));
    triggerTree->Branch("trk1_nMdt",    &(trkT_nMdt[1]));
    triggerTree->Branch("trk1_nTgc",    &(trkT_nTgc[1]));
    triggerTree->Branch("trk1_nRpc",    &(trkT_nRpc[1]));

    triggerTree->Branch("trk0_innerMdt_r",      &(innerMdt_r    [0]));
    triggerTree->Branch("trk0_innerMdt_z",      &(innerMdt_z    [0]));
    triggerTree->Branch("trk0_middleMdt_r",     &(middleMdt_r   [0]));
    triggerTree->Branch("trk0_middleMdt_z",     &(middleMdt_z   [0]));
    triggerTree->Branch("trk0_etaRpc_r",        &(etaRpc_r      [0]));
    triggerTree->Branch("trk0_etaRpc_z",        &(etaRpc_z      [0]));
    triggerTree->Branch("trk0_phiRpc_x",        &(phiRpc_x      [0]));
    triggerTree->Branch("trk0_phiRpc_y",        &(phiRpc_y      [0]));
    triggerTree->Branch("trk0_innerEtaTgc_r",   &(innerEtaTgc_r [0]));
    triggerTree->Branch("trk0_innerEtaTgc_z",   &(innerEtaTgc_z [0]));
    triggerTree->Branch("trk0_innerEtaTgc_phi", &(innerEtaTgc_phi[0]));
    triggerTree->Branch("trk0_innerPhiTgc",     &(innerPhiTgc    [0]));
    triggerTree->Branch("trk0_middleEtaTgc_r",  &(middleEtaTgc_r[0]));
    triggerTree->Branch("trk0_middleEtaTgc_z",  &(middleEtaTgc_z[0]));
    triggerTree->Branch("trk0_middleEtaTgc_phi",&(middleEtaTgc_phi[0]));
    triggerTree->Branch("trk0_middlePhiTgc",    &(middlePhiTgc    [0]));

    triggerTree->Branch("trk1_innerMdt_r",      &(innerMdt_r    [1]));
    triggerTree->Branch("trk1_innerMdt_z",      &(innerMdt_z    [1]));
    triggerTree->Branch("trk1_middleMdt_r",     &(middleMdt_r   [1]));
    triggerTree->Branch("trk1_middleMdt_z",     &(middleMdt_z   [1]));
    triggerTree->Branch("trk1_etaRpc_r",        &(etaRpc_r      [1]));
    triggerTree->Branch("trk1_etaRpc_z",        &(etaRpc_z      [1]));
    triggerTree->Branch("trk1_phiRpc_x",        &(phiRpc_x      [1]));
    triggerTree->Branch("trk1_phiRpc_y",        &(phiRpc_y      [1]));
    triggerTree->Branch("trk1_innerEtaTgc_r",   &(innerEtaTgc_r [1]));
    triggerTree->Branch("trk1_innerEtaTgc_z",   &(innerEtaTgc_z [1]));
    triggerTree->Branch("trk1_innerEtaTgc_phi", &(innerEtaTgc_phi[1]));
    triggerTree->Branch("trk1_innerPhiTgc",     &(innerPhiTgc    [1]));
    triggerTree->Branch("trk1_middleEtaTgc_r",  &(middleEtaTgc_r[1]));
    triggerTree->Branch("trk1_middleEtaTgc_z",  &(middleEtaTgc_z[1]));
    triggerTree->Branch("trk1_middleEtaTgc_phi",&(middleEtaTgc_phi[1]));
    triggerTree->Branch("trk1_middlePhiTgc",    &(middlePhiTgc    [1]));


    // declare trees
    TTree *tJpsi, *tTracks, *tMuons, *tTrig, *tChain, *tDetails;

    //declare b-physics objects
    BCompositeParticle jpsi;
    TrackBag    tracks;
    MuonBag     muons;
    TriggerBag  trigDecisions;

    //define Chain object
    Chain* trigChain = NULL;

    // define Details object
    Details* details = NULL;

    int currentTrigIndex = 0;

    //loop over all entries and all files
    log.info("******************************************************************");
    log.info("** BEGINNING OF THE LOOP "+iToStr(doJpsi)+"                                      **");
    log.info("******************************************************************");
    m_nEntries = 0;
    for(resetIterator();
        (doJpsi && iterateJpsi("Jpsi", &tJpsi, &jpsi,
                               "MuidMuonCollectionTracks", &tTracks, &tracks,
                               "MuidMuonCollection", &tMuons, &muons,
                               "TRIG", &tTrig, &trigDecisions,
                               chain, &tChain, &trigChain,
                               "MuonFeatureDetails", &tDetails, &details)) ||
        (!doJpsi && iterateChain("All_"+chain, &tChain, &trigChain,
                                 "TRIG", &tTrig, &trigDecisions,
                                 "MuonFeatureDetails", &tDetails, &details,
                                 "MuidMuonCollection", &tMuons, &muons,
                                 false, false));
        ++m_nEntries)
    {

      if(m_nEntries % 1000 == 0) log.info("   ===> Entry " + iToStr(m_nEntries));

      /// level-1 jpsi =============================================================

      if( !cuts.selectLevel1(trigDecisions) ) continue;
      passTrigDec = cuts.selectTrigDecision(trigDecisions, chain);

      /// offline jpsi =============================================================

      log.debug("offline jpsi analysis");

      // J/psi selection
      if( doJpsi && !cuts.selectJpsi(m_ntupleReader, jpsi, tracks,  muons) ) continue;

      /// Fill ntuple ================================================

      if(doJpsi) {
        // cleanup
        trigIndex->clear();

        // fill J/psi variables
        mass = jpsi.refittedMass();
        pt = jpsi.refittedPt();
        eta = jpsi.refittedEta();
        phi = jpsi.getRefMom().Phi();
        for(int i=0; i<2; ++i) {
          trk_pt[i]     = jpsi.getTrackMom(i).Pt();
          trk_eta[i]    = jpsi.getTrackMom(i).Eta();
          trk_phi[i]    = jpsi.getTrackMom(i).Phi();
          trk_charge[i] = i==0 ? 1. : -1.;

          // select trigger tracks
          matchTrk_valid[i]  = trigChain->MatchedTracks_valid->at(i);
          matchTrk_pt[i]     = trigChain->MatchedTracks_pt->at(i);
          matchTrk_eta[i]    = trigChain->MatchedTracks_eta->at(i);
          matchTrk_phi[i]    = trigChain->MatchedTracks_phi->at(i);
          matchTrk_charge[i] = trigChain->MatchedTracks_charge->at(i);
        }
      }else{
        // muon cleanup
        muon_pt      ->clear();
        muon_eta     ->clear();
        muon_phi     ->clear();
        muon_charge  ->clear();
        muon_combined->clear();

        // fill muon variables
        MuonBag::iterator muonItr = muons.begin();
        for(; muonItr!=muons.end(); ++muonItr) {
          muon_pt ->push_back((*muonItr).pt());
          muon_eta->push_back((*muonItr).eta());
          muon_phi->push_back((*muonItr).phi());
          muon_charge->push_back((*muonItr).charge());
          muon_combined->push_back((*muonItr).isCombinedMuon());
        }
      }

      // fill trigger variables
      for(int j=0; (doJpsi && j<(int)trigChain->Matched_mass->size()) || (!doJpsi && j<(int)trigChain->All_mass->size()); ++j) {
        // cleanup
        for(int i=0; i<2; ++i) {
          innerMdt_r[i]      ->clear();
          innerMdt_z[i]      ->clear();
          middleMdt_r[i]     ->clear();
          middleMdt_z[i]     ->clear();
          etaRpc_r[i]        ->clear();
          etaRpc_z[i]        ->clear();
          phiRpc_x[i]        ->clear();
          phiRpc_y[i]        ->clear();
          innerEtaTgc_r[i]   ->clear();
          innerEtaTgc_z[i]   ->clear();
          innerEtaTgc_phi[i] ->clear();
          innerPhiTgc[i]     ->clear();
          middleEtaTgc_r[i]  ->clear();
          middleEtaTgc_z[i]  ->clear();
          middleEtaTgc_phi[i]->clear();
          middlePhiTgc[i]    ->clear();
        }

        // di-muon trigger objects
        if(doJpsi) {
          runNumber   = jpsi.runNumber();
          lumiBlock   = jpsi.lumiBlock();
          eventNumber = jpsi.eventNumber();

          trigMass = trigChain->Matched_mass->at(j);
          chi2     = trigChain->Matched_chi2->at(j);
          trkT_pt[0]     = trigChain->Matched_trk1_pt->at(j);
          trkT_eta[0]    = trigChain->Matched_trk1_eta->at(j);
          trkT_phi[0]    = trigChain->Matched_trk1_phi->at(j);
          trkT_charge[0] = trigChain->Matched_trk1_charge->at(j);

          trkT_nMdt[0]  = trigChain->Matched_trk1_mdtHits->at(j);
          trkT_nRpc[0]  = trigChain->Matched_trk1_rpcHits->at(j);
          trkT_nTgc[0]  = trigChain->Matched_trk1_tgcHits->at(j);

          trkT_pt[1]     = trigChain->Matched_trk2_pt->at(j);
          trkT_eta[1]    = trigChain->Matched_trk2_eta->at(j);
          trkT_phi[1]    = trigChain->Matched_trk2_phi->at(j);
          trkT_charge[1] = trigChain->Matched_trk2_charge->at(j);

          trkT_nMdt[1]  = trigChain->Matched_trk2_mdtHits->at(j);
          trkT_nRpc[1]  = trigChain->Matched_trk2_rpcHits->at(j);
          trkT_nTgc[1]  = trigChain->Matched_trk2_tgcHits->at(j);
        } else {

          runNumber   = trigChain->RunNumber;
          lumiBlock   = trigChain->LumiBlock;
          eventNumber = trigChain->EventNumber;

          trigMass = trigChain->All_mass->at(j);
          chi2     = trigChain->All_chi2->at(j);
          trkT_pt[0]     = trigChain->All_trk1_pt->at(j);
          trkT_eta[0]    = trigChain->All_trk1_eta->at(j);
          trkT_phi[0]    = trigChain->All_trk1_phi->at(j);
          trkT_charge[0] = trigChain->All_trk1_charge->at(j);

          trkT_nMdt[0]  = trigChain->All_trk1_mdtHits->at(j);
          trkT_nRpc[0]  = trigChain->All_trk1_rpcHits->at(j);
          trkT_nTgc[0]  = trigChain->All_trk1_tgcHits->at(j);

          trkT_pt[1]     = trigChain->All_trk2_pt->at(j);
          trkT_eta[1]    = trigChain->All_trk2_eta->at(j);
          trkT_phi[1]    = trigChain->All_trk2_phi->at(j);
          trkT_charge[1] = trigChain->All_trk2_charge->at(j);

          trkT_nMdt[1]  = trigChain->All_trk2_mdtHits->at(j);
          trkT_nRpc[1]  = trigChain->All_trk2_rpcHits->at(j);
          trkT_nTgc[1]  = trigChain->All_trk2_tgcHits->at(j);

          //
        }

        // hits *************************************

        bool analyzed[2] = {false, false};

        log.debug("Loop over details");

        // loop over details
        int entry = m_detailsEntry;
        for(details->GetEntry(entry);
            details->RunNumber == trigChain->RunNumber && details->EvtNumber == trigChain->EventNumber && entry < details->fChain->GetEntries();
            details->GetEntry(++entry))
        {
          double ptId   = details->Pt;
          double etaId  = details->Eta;
          double phiId  = details->Phi;
          double charge = details->Charge == 1 ? 1. : -1.;
          log.debug("Details: pt = "+fToStr(ptId)+", eta = "+fToStr(etaId)+", phi = "+fToStr(phiId)+", charge = "+fToStr(charge));

          // determine whether Details correspond to the the trigger object
          int whichTrack = -1;
          double dEta1;
          double dPhi1;
          double dEta2;
          double dPhi2;

          if(doJpsi) {
            dEta1 = fabs(trigChain->Matched_trk1_eta->at(j) - etaId);
            dPhi1 = fabs(dAngle(trigChain->Matched_trk1_phi->at(j), phiId));
            dEta2 = fabs(trigChain->Matched_trk2_eta->at(j) - etaId);
            dPhi2 = fabs(dAngle(trigChain->Matched_trk2_phi->at(j), phiId));
          } else {
            dEta1 = fabs(trigChain->All_trk1_eta->at(j) - etaId);
            dPhi1 = fabs(dAngle(trigChain->All_trk1_phi->at(j), phiId));
            dEta2 = fabs(trigChain->All_trk2_eta->at(j) - etaId);
            dPhi2 = fabs(dAngle(trigChain->All_trk2_phi->at(j), phiId));
          }

          if(dEta1 < 0.000001 && dPhi1 < 0.000001) whichTrack = 0;
          if(dEta2 < 0.000001 && dPhi2 < 0.000001) {
            if(whichTrack == 0) {
              log.warning("Details matched to the both tracks");
              continue;
            }
            whichTrack = 1;
          }
          log.debug("  dEta1 = "+fToStr(dEta1)+", dPhi1 = "+fToStr(dPhi1));
          log.debug("  dEta2 = "+fToStr(dEta2)+", dPhi2 = "+fToStr(dPhi2));

          if(whichTrack == -1) continue;

          // iterators
          int deInnMdt = 0;
          int deMidMdt = 0;
          int deRpcPhi = 0;
          int deRpcEta = 0;
          int deInnTgcPhi = 0;
          int deMidTgcPhi = 0;
          int deInnTgcEta = 0;
          int deMidTgcEta = 0;

          // MDT  -------------------------------------
          log.debug("MDT inner");
          for(; deInnMdt<(int)details->InnerMdt_r->size(); ++deInnMdt) {
            if(whichTrack>=0 && !analyzed[whichTrack]) {
              innerMdt_r[whichTrack]->push_back( details->InnerMdt_r->at(deInnMdt) );
              innerMdt_z[whichTrack]->push_back( details->InnerMdt_z->at(deInnMdt) );
            }
          }

          log.debug("MDT middle");
          for(; deMidMdt<(int)details->MiddleMdt_r->size(); ++deMidMdt) {
            if(whichTrack>=0 && !analyzed[whichTrack]) {
              middleMdt_r[whichTrack]->push_back( details->MiddleMdt_r->at(deMidMdt) );
              middleMdt_z[whichTrack]->push_back( details->MiddleMdt_z->at(deMidMdt) );
            }
          }

          // RPC ---------------------------------------
          log.debug("RPC eta pads");
          for(; deRpcEta<(int)details->MiddleRpc_etaPad_r->size(); ++deRpcEta) {
            if(whichTrack>=0 && !analyzed[whichTrack]) {
              etaRpc_r[whichTrack]->push_back( details->MiddleRpc_etaPad_r->at(deRpcEta) );
              etaRpc_z[whichTrack]->push_back( details->MiddleRpc_etaPad_z->at(deRpcEta) );
            }
          }

          log.debug("RPC phi pads");
          for(; deRpcPhi<(int)details->MiddleRpc_phiPad_x->size(); ++deRpcPhi) {
            if(whichTrack>=0 && !analyzed[whichTrack]) {
              phiRpc_x[whichTrack]->push_back( details->MiddleRpc_phiPad_x->at(deRpcPhi) );
              phiRpc_y[whichTrack]->push_back( details->MiddleRpc_phiPad_y->at(deRpcPhi) );
            }
          }

          // TGC ---------------------------------------

          // wire
          log.debug("TGC inner wire");
          for(; deInnTgcEta<(int)details->InnerTgc_rhoHit_r->size(); ++deInnTgcEta) {
            if(whichTrack>=0 && !analyzed[whichTrack]) {
              innerEtaTgc_r[whichTrack]->push_back( details->InnerTgc_rhoHit_r->at(deInnTgcEta) );
              innerEtaTgc_z[whichTrack]->push_back( details->InnerTgc_rhoHit_z->at(deInnTgcEta) );
              innerEtaTgc_phi[whichTrack]->push_back( details->InnerTgc_rhoHit_phi->at(deInnTgcEta) );
            }
          }
          log.debug("TGC Middle wire");
          for(; deMidTgcEta<(int)details->MiddleTgc_rhoHit_r->size(); ++deMidTgcEta) {
            if(whichTrack>=0 && !analyzed[whichTrack]) {
              middleEtaTgc_r[whichTrack]->push_back( details->MiddleTgc_rhoHit_r->at(deMidTgcEta) );
              middleEtaTgc_z[whichTrack]->push_back( details->MiddleTgc_rhoHit_z->at(deMidTgcEta) );
              middleEtaTgc_phi[whichTrack]->push_back( details->MiddleTgc_rhoHit_phi->at(deMidTgcEta) );
            }
          }

          // strip
          log.debug("TGC inner strip");
          for(; deInnTgcPhi<(int)details->InnerTgc_phiHit_r->size(); ++deInnTgcPhi) {
            if(whichTrack>=0 && !analyzed[whichTrack])
              innerPhiTgc[whichTrack]->push_back( details->InnerTgc_phiHit_phi->at(deInnTgcPhi) );
          }

          log.debug("TGC middle strip");
          for(; deMidTgcPhi<(int)details->MiddleTgc_phiHit_r->size(); ++deMidTgcPhi) {
            if(whichTrack>=0 && !analyzed[whichTrack])
              middlePhiTgc[whichTrack]->push_back( details->MiddleTgc_phiHit_phi->at(deMidTgcPhi) );
          }

          analyzed[whichTrack] = true;

        } // end of loop over details

        log.debug("End of loop over details");



        // store entry number
        if(doJpsi) trigIndex->push_back(currentTrigIndex);

        ++currentTrigIndex;

        // fill the tree
//         log.info(iToStr(runNumber));
        triggerTree->Fill();
        if(!doJpsi) muonTree->Fill();
      }

      if(doJpsi) jpsiTree->Fill();

    } // end of the loop
    log.info("******************************************************************");

    // save trees
    fOut->cd();
    if(doJpsi) jpsiTree->Write();
    triggerTree->Write();
    if(!doJpsi) muonTree->Write();

  } // end of "doJpsi" loop

  // save
  fOut->Close();

}
///===================================================

void TrigDiMuonRootCalib::analyzeChain(std::string chain, std::string settings, std::string cutStr) {
  Log log("TrigDiMuonRootCalib::analyzeChain", ol());

  log.info("Settings: " + settings);
  log.info("cuts:     " + cutStr);

  // create selection algorithm
  CutsJpsi cuts(ol(), cutStr);

  std::string constantsFile = "constants.py";
  getValue(settings, "ConstantsFile=", constantsFile);

  std::string cutConeFile = "cut_cone.py";
  getValue(settings, "CutConeFile=", cutConeFile);

  //histograms
  importHistFromFile("histograms_analyzeChain.config");

  setValXVectPointer( "jpsi"            , &m_jpsi          );
  setValXVectPointer( "jpsi_trk"        , &m_jpsi_trk      );
  setValXVectPointer( "jpsi_fex"        , &m_jpsi_fex      );
  setValXVectPointer( "jpsi_muID"       , &m_jpsi_muID     );
  setValXVectPointer( "jpsi_vtx"        , &m_jpsi_vtx      );

  setValXVectPointer( "jpsi_pt"         , &m_jpsi_pt          );
  setValXVectPointer( "jpsi_pt_trk"     , &m_jpsi_pt_trk      );
  setValXVectPointer( "jpsi_pt_fex"     , &m_jpsi_pt_fex      );
  setValXVectPointer( "jpsi_pt_muID"    , &m_jpsi_pt_muID     );
  setValXVectPointer( "jpsi_pt_vtx"     , &m_jpsi_pt_vtx      );

  setValXVectPointer( "jpsi_eta"         , &m_jpsi_eta          );
  setValXVectPointer( "jpsi_eta_trk"     , &m_jpsi_eta_trk      );
  setValXVectPointer( "jpsi_eta_fex"     , &m_jpsi_eta_fex      );
  setValXVectPointer( "jpsi_eta_muID"    , &m_jpsi_eta_muID     );
  setValXVectPointer( "jpsi_eta_vtx"     , &m_jpsi_eta_vtx      );

  setValXVectPointer( "jpsi_dR"         , &m_jpsi_dR          );
  setValXVectPointer( "jpsi_dR_trk"     , &m_jpsi_dR_trk      );
  setValXVectPointer( "jpsi_dR_fex"     , &m_jpsi_dR_fex      );
  setValXVectPointer( "jpsi_dR_muID"    , &m_jpsi_dR_muID     );
  setValXVectPointer( "jpsi_dR_vtx"     , &m_jpsi_dR_vtx      );

//   setValXVectPointer( "mu_eta"          , &m_mu_eta        );
//   setValXVectPointer( "mu_pt"           , &m_mu_pt         );
//   setValXVectPointer( "trigTrk_eta"     , &m_trigTrk_eta   );
//   setValXVectPointer( "trigTrk_pt"      , &m_trigTrk_pt    );
//
//   setValXVectPointer( "trig_chi2"       , &m_trig_chi2     );

  setValXVectPointer( "num_muID"        , &m_num_muID      );
  setValXVectPointer( "num_vtx"         , &m_num_vtx       );

  // check mandatory histograms
  if(!isDefined("jpsi")) {
    log.error("Histogram \"jpsi\" is mandatory. Leaving...");
    return;
  }

  //set custom histogram titles
  setTitle("jpsi", ("J/#psi: "+chain).c_str());

  // set pointer to the used cuts so they can be automaticaly drawn into the selected histograms
  setCutsForHist(&cuts);

  // extrapolator
  TrigDiMuonExtrapolator* propagator = new TrigDiMuonExtrapolator(ol(), settings);
  propagator->read(constantsFile);
  propagator->readCutCone(cutConeFile);

  // invariant mass range used to calculate efficiency
  int loBin = 1;
  int hiBin = getHist("jpsi")->GetNbinsX()+1;
  getValue(settings,"massBinLo=",loBin);
  getValue(settings,"massBinHi=",hiBin);

  // pointer to some histograms we will need further
  TH1F* h_jpsi      = getHist("jpsi");
  TH1F* h_num_muID = getHist("num_muID");
  TH1F* h_num_vtx   = getHist("num_vtx");

  //loop over all entries and all files
  log.info("******************************************************************");
  log.info("** BEGINNING OF THE LOOP                                        **");
  log.info("******************************************************************");
  while( iterateFiles() ) {

    // load trees
    TTree* tJpsi = loadTree("Jpsi");
    TTree* tTrig = loadTree("Trigger");

    SimpleJpsi* jpsi = new SimpleJpsi(tJpsi);
    TrigHits*   trig = new TrigHits(tTrig);

    // loop over entries
    for(int entry=0; entry<tJpsi->GetEntries(); ++entry) {
      if(entry % 100 == 0) log.info("   ===> Entry " + iToStr(entry));

      // load entry
      jpsi->GetEntry(entry);

      // select jpsi
      if( !cuts.selectSimpleJpsi(jpsi) ) continue;

      //fill J/psi histogram
      m_jpsi.push_back(jpsi->mass/1000);
      m_jpsi_pt.push_back(TMath::Min(jpsi->trk0_pt/1000, jpsi->trk1_pt/1000));
      m_jpsi_eta.push_back( fabs(jpsi->trk0_pt) < fabs(jpsi->trk1_pt) ? jpsi->trk0_eta : jpsi->trk1_eta );
      m_jpsi_dR.push_back( sqrt( pow( jpsi->trk0_eta-jpsi->trk1_eta, 2) + pow( dAngle( jpsi->trk0_phi, jpsi->trk1_phi ), 2 ) ) );

      /// matched trigger objects ==================================================

      //Trigger tracking
      if(jpsi->matchTrk0_valid && jpsi->matchTrk1_valid ) {
        m_jpsi_trk.push_back(jpsi->mass/1000);
        m_jpsi_pt_trk.push_back(TMath::Min(jpsi->trk0_pt/1000, jpsi->trk1_pt/1000));
        m_jpsi_eta_trk.push_back( fabs(jpsi->trk0_pt) < fabs(jpsi->trk1_pt) ? jpsi->trk0_eta : jpsi->trk1_eta );
        m_jpsi_dR_trk.push_back( sqrt( pow( jpsi->trk0_eta-jpsi->trk1_eta, 2) + pow( dAngle( jpsi->trk0_phi, jpsi->trk1_phi ), 2 ) ) );
      }

      log.debug("No of matched trigger objects " + iToStr(jpsi->trigIndex->size()));

      if(jpsi->trigIndex->size() > 0 ) {

        /// FEX
        m_jpsi_fex.push_back(jpsi->mass/1000);
        m_jpsi_pt_fex.push_back(TMath::Min(jpsi->trk0_pt/1000, jpsi->trk1_pt/1000));
        m_jpsi_eta_fex.push_back( fabs(jpsi->trk0_pt) < fabs(jpsi->trk1_pt) ? jpsi->trk0_eta : jpsi->trk1_eta );
        m_jpsi_dR_fex.push_back( sqrt( pow( jpsi->trk0_eta-jpsi->trk1_eta, 2) + pow( dAngle( jpsi->trk0_phi, jpsi->trk1_phi ), 2 ) ) );

        /// Hypo
        // apply hypo selection
        if( cuts.selectHypo(propagator,jpsi->trigIndex, trig) ) {
          m_jpsi_muID.push_back(jpsi->mass/1000);
          m_jpsi_pt_muID.push_back(TMath::Min(jpsi->trk0_pt/1000, jpsi->trk1_pt/1000));
          m_jpsi_eta_muID.push_back( fabs(jpsi->trk0_pt) < fabs(jpsi->trk1_pt) ? jpsi->trk0_eta : jpsi->trk1_eta );
          m_jpsi_dR_muID.push_back( sqrt( pow( jpsi->trk0_eta-jpsi->trk1_eta, 2) + pow( dAngle( jpsi->trk0_phi, jpsi->trk1_phi ), 2 ) ) );

          // if requested do chi2 scan
          if(h_num_vtx != NULL && h_jpsi!=NULL && cuts.cut_doChi2Scan.enabled &&
             jpsi->mass/1000 >= h_jpsi->GetBinLowEdge(loBin) &&
             jpsi->mass/1000 < h_jpsi->GetBinLowEdge(hiBin))
          {
            for(int j=1; j <= h_num_vtx->GetNbinsX() ; ++j) {
              double chi2Cut = h_num_vtx->GetBinLowEdge(j+1);

              if( cuts.selectHypo( propagator, jpsi->trigIndex, trig, -2, chi2Cut ) ) {
                m_num_vtx.push_back(h_num_vtx->GetBinCenter(j));  // efficiency scan histogram
              }
            } // end of loop over nHist
          }  // end of efficiency scan for different chi2 cuts
        }

        // if requested do no-of-hits scan
        if(h_num_muID != NULL && h_jpsi!=NULL && cuts.cut_doNMuHitsScan.enabled &&
           jpsi->mass/1000 >= h_jpsi->GetBinLowEdge(loBin) &&
           jpsi->mass/1000 < h_jpsi->GetBinLowEdge(hiBin))
        {
          for(int nHits=0; nHits < h_num_muID->GetNbinsX(); ++nHits) {
            if( cuts.selectHypo( propagator, jpsi->trigIndex, trig, nHits ) ) {
              m_num_muID.push_back(nHits);  // efficiency scan histogram
            }
          } // end of loop over nHist
        }  // end of efficiency scan for different number-of-hits cuts

        // apply additional chi2 selection
        if( cuts.selectHypo(propagator,jpsi->trigIndex, trig, -1, cuts.cut_extraChi2.val) ) {
          m_jpsi_vtx.push_back(jpsi->mass/1000);
          m_jpsi_pt_vtx.push_back(TMath::Min(jpsi->trk0_pt/1000, jpsi->trk1_pt/1000));
          m_jpsi_eta_vtx.push_back( fabs(jpsi->trk0_pt) < fabs(jpsi->trk1_pt) ? jpsi->trk0_eta : jpsi->trk1_eta );
          m_jpsi_dR_vtx.push_back( sqrt( pow( jpsi->trk0_eta-jpsi->trk1_eta, 2) + pow( dAngle( jpsi->trk0_phi, jpsi->trk1_phi ), 2 ) ) );
        }

      } // end of FEX

      /// update no. of selected jpsi's

      if(h_jpsi!=NULL &&
         jpsi->mass/1000 > h_jpsi->GetBinLowEdge(loBin) &&
         jpsi->mass/1000 < h_jpsi->GetBinLowEdge(hiBin))
      {
        ++m_nCandidates;
      }

      /// fill the histograms ========================================================

      fillAll();
    }

  } // end of the loop
  log.info("******************************************************************");


  // fitting ------------------------------
  TF1* fitFun = fitGaus("jpsi", settings);

  //drawing ------------------------------
  /// matched trigger object
  if(isDefined("jpsi") && isDefined("jpsi_trk")) {
    log.info("Drawing mass histogram");

    TCanvas* c = new TCanvas("c_jpsi","c_jpsi");

    // set histograms style
    setMarkerStyle("jpsi",20);
    setFillStyle("jpsi_trk",  kRed);
    setFillStyle("jpsi_fex",  kGreen);
    setFillStyle("jpsi_muID", kYellow);
    setFillStyle("jpsi_vtx",  kMagenta);

    // draw histogram
    drawHist(NameList("jpsi", "jpsi_trk", "jpsi_fex", "jpsi_muID", "jpsi_vtx"));

    // draw fitfun
    if(fitFun!=NULL) fitFun->Draw("same");

    // legend
    drawLegend(NameList("jpsi", "jpsi_trk", "jpsi_fex", "jpsi_muID", "jpsi_vtx"), 0.55,0.5,0.9,0.3);

    // indication of cuts
    if(getValue(settings,"massBinLo=",loBin) && getValue(settings,"massBinHi=",hiBin))
      drawCuts(getHist("jpsi")->GetBinLowEdge(loBin), getHist("jpsi")->GetBinLowEdge(hiBin), "jpsi");

    //calculate efficiency
    double denom = 0;
    std::vector<double> num;
    std::vector<double> eff;
    if( getEfficiencies( num, denom, eff, NameList("jpsi_trk","jpsi_fex","jpsi_muID").names, "jpsi", loBin, hiBin) ) {

      log.info("No of J/psi                 : " + fToStr(denom));
      log.info("No of J/psi after Tracking  : " + fToStr(num.at(0)));
      log.info("No of J/psi after FEX       : " + fToStr(num.at(1)));
      log.info("No of J/psi after Hypo.     : " + fToStr(num.at(2)));
//       log.info("No of J/psi after vertexing : " + fToStr(num.at(3)));

      log.info("Efficiency of Tracking  : " + fToStr(eff.at(0)));
      log.info("Efficiency of FEX       : " + fToStr(eff.at(1)));
      log.info("Efficiency of Hypo.     : " + fToStr(eff.at(2)));
//       log.info("Efficiency of vertexing : " + fToStr(eff.at(3)));

    }
    if( getEfficiencies( num, denom, eff, NameList("jpsi_vtx").names, "jpsi", loBin, hiBin) ) {

      log.info("No of J/psi after vertexing : " + fToStr(num.at(0)));
      log.info("Efficiency of vertexing : " + fToStr(eff.at(0)));

    }

    c->RedrawAxis();
  }

  // pt
  if(isDefined("jpsi_pt") && isDefined("jpsi_pt_trk")) {
    log.info("Drawing pT histogram");
    TCanvas* c = new TCanvas("c_jpsi_pt","c_jpsi_pt");

    // set histograms style
    setMarkerStyle("jpsi_pt",20);
    setFillStyle("jpsi_pt_trk",  kRed);
    setFillStyle("jpsi_pt_fex",  kGreen);
    setFillStyle("jpsi_pt_muID", kYellow);
    setFillStyle("jpsi_pt_vtx",  kMagenta);

    // draw histogram
    drawHist(NameList("jpsi_pt", "jpsi_pt_trk", "jpsi_pt_fex", "jpsi_pt_muID"));

    // legend
    drawLegend(NameList("jpsi_pt", "jpsi_pt_trk", "jpsi_pt_fex", "jpsi_pt_muID"), 0.55,0.5,0.9,0.3);

    c->RedrawAxis();

    // calculate efficiencies
    TLegend* leg = new TLegend(0.55,0.5,0.9,0.3);
    std::vector<TGraphAsymmErrors*> effPlots;
    if(isDefined("jpsi_pt_trk")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_pt_trk"), getHist("jpsi_pt"));
      g->SetMarkerStyle(20);
      leg->AddEntry(g, "Tracking eff.","pl");
      effPlots.push_back(g);
    }
    if(isDefined("jpsi_pt_fex")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_pt_fex"), getHist("jpsi_pt"));
      g->SetMarkerStyle(22);
      leg->AddEntry(g, "FEX eff.","pl");
      effPlots.push_back(g);
    }
    if(isDefined("jpsi_pt_muID")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_pt_muID"), getHist("jpsi_pt"));
      g->SetMarkerStyle(24);
      leg->AddEntry(g, "Hypo eff.","pl");
      effPlots.push_back(g);
    }

    // draw efficiency
    if(effPlots.size()>0) {
      new TCanvas("c_eff_jpsi_pt","c_eff_jpsi_pt");
      effPlots.at(0)->SetTitle(";sub-leading muon p_{T} (GeV); Efficiency");
      effPlots.at(0)->SetMinimum(0);
      effPlots.at(0)->Draw("ap");
      if(effPlots.size()>1) {
        std::vector<TGraphAsymmErrors*>::iterator effPlotsItr = effPlots.begin()+1;
        for(; effPlotsItr!=effPlots.end(); ++effPlotsItr) {
          (*effPlotsItr)->Draw("p");
        }
      }
    }
    leg->SetFillColor(kWhite);
    leg->Draw("same");
  }

  // eta
  if(isDefined("jpsi_eta") && isDefined("jpsi_eta_trk")) {
    log.info("Drawing eta histogram");
    TCanvas* c = new TCanvas("c_jpsi_eta","c_jpsi_eta");

    // set histograms style
    setMarkerStyle("jpsi_eta",20);
    setFillStyle("jpsi_eta_trk",  kRed);
    setFillStyle("jpsi_eta_fex",  kGreen);
    setFillStyle("jpsi_eta_muID", kYellow);
    setFillStyle("jpsi_eta_vtx",  kMagenta);

    // draw histogram
    drawHist(NameList("jpsi_eta", "jpsi_eta_trk", "jpsi_eta_fex", "jpsi_eta_muID"));

    // legend
    drawLegend(NameList("jpsi_eta", "jpsi_eta_trk", "jpsi_eta_fex", "jpsi_eta_muID"), 0.55,0.85,0.9,0.65);

    c->RedrawAxis();

    // calculate efficiencies
    TLegend* leg = new TLegend(0.55,0.5,0.9,0.3);
    std::vector<TGraphAsymmErrors*> effPlots;
    if(isDefined("jpsi_eta_trk")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_eta_trk"), getHist("jpsi_eta"));
      g->SetMarkerStyle(20);
      leg->AddEntry(g, "Tracking eff.","pl");
      effPlots.push_back(g);
    }
    if(isDefined("jpsi_eta_fex")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_eta_fex"), getHist("jpsi_eta"));
      g->SetMarkerStyle(22);
      leg->AddEntry(g, "FEX eff.","pl");
      effPlots.push_back(g);
    }
    if(isDefined("jpsi_eta_muID")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_eta_muID"), getHist("jpsi_eta"));
      g->SetMarkerStyle(24);
      leg->AddEntry(g, "Hypo eff.","pl");
      effPlots.push_back(g);
    }

    // draw efficiency
    if(effPlots.size()>0) {
      new TCanvas("c_eff_jpsi_eta","c_eff_jpsi_eta");
      effPlots.at(0)->SetTitle(";sub-leading muon #eta; Efficiency");
      effPlots.at(0)->SetMinimum(0);
      effPlots.at(0)->Draw("ap");
      if(effPlots.size()>1) {
        std::vector<TGraphAsymmErrors*>::iterator effPlotsItr = effPlots.begin()+1;
        for(; effPlotsItr!=effPlots.end(); ++effPlotsItr) {
          (*effPlotsItr)->Draw("p");
        }
      }
    }
    leg->SetFillColor(kWhite);
    leg->Draw("same");
  }

  // eta
  if(isDefined("jpsi_dR") && isDefined("jpsi_dR_trk")) {
    log.info("Drawing dR histogram");
    TCanvas* c = new TCanvas("c_jpsi_dR","c_jpsi_dR");

    // set histograms style
    setMarkerStyle("jpsi_dR",20);
    setFillStyle("jpsi_dR_trk",  kRed);
    setFillStyle("jpsi_dR_fex",  kGreen);
    setFillStyle("jpsi_dR_muID", kYellow);
    setFillStyle("jpsi_dR_vtx",  kMagenta);

    // draw histogram
    drawHist(NameList("jpsi_dR", "jpsi_dR_trk", "jpsi_dR_fex", "jpsi_dR_muID"));

    // legend
    drawLegend(NameList("jpsi_dR", "jpsi_dR_trk", "jpsi_dR_fex", "jpsi_dR_muID"), 0.55,0.85,0.9,0.65);

    c->RedrawAxis();

    // calculate efficiencies
    TLegend* leg = new TLegend(0.55,0.5,0.9,0.3);
    std::vector<TGraphAsymmErrors*> effPlots;
    if(isDefined("jpsi_dR_trk")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_dR_trk"), getHist("jpsi_dR"));
      g->SetMarkerStyle(20);
      leg->AddEntry(g, "Tracking eff.","pl");
      effPlots.push_back(g);
    }
    if(isDefined("jpsi_dR_fex")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_dR_fex"), getHist("jpsi_dR"));
      g->SetMarkerStyle(22);
      leg->AddEntry(g, "FEX eff.","pl");
      effPlots.push_back(g);
    }
    if(isDefined("jpsi_dR_muID")) {
      TGraphAsymmErrors* g = new TGraphAsymmErrors(getHist("jpsi_dR_muID"), getHist("jpsi_dR"));
      g->SetMarkerStyle(24);
      leg->AddEntry(g, "Hypo eff.","pl");
      effPlots.push_back(g);
    }

    // draw efficiency
    if(effPlots.size()>0) {
      new TCanvas("c_eff_jpsi_dR","c_eff_jpsi_dR");
      effPlots.at(0)->SetTitle(";#DeltaR; Efficiency");
      effPlots.at(0)->SetMinimum(0);
      effPlots.at(0)->Draw("ap");
      if(effPlots.size()>1) {
        std::vector<TGraphAsymmErrors*>::iterator effPlotsItr = effPlots.begin()+1;
        for(; effPlotsItr!=effPlots.end(); ++effPlotsItr) {
          (*effPlotsItr)->Draw("p");
        }
      }
    }
    leg->SetFillColor(kWhite);
    leg->Draw("same");
  }


  /// single tracks
  // eta
  if(isDefined("mu_eta") && isDefined("trigTrk_eta")) {
    new TCanvas("c_mu_eta","c_mu_eta");
    setMarkerStyle("mu_eta", 20);
    setFillStyle("trigTrk_eta", kYellow);

    // draw histogram
    drawHist(NameList("mu_eta", "trigTrk_eta"));

    // legend
    drawLegend(NameList("mu_eta", "trigTrk_eta"), 0.35,0.9,0.75,0.7);

    // draw efficiency
    new TCanvas("c_eff_mu_eta","c_eff_mu_eta");
    TGraphAsymmErrors* eff_mu_eta = new TGraphAsymmErrors(getHist("trigTrk_eta"), getHist("mu_eta"));
    eff_mu_eta->SetTitle("Trigger Tracking Efficiency;#eta");
    eff_mu_eta->Draw("ap");
  }

  // pt
  if(isDefined("mu_pt") && isDefined("trigTrk_pt")) {
    new TCanvas("c_mu_pt","c_mu_pt");
    setMarkerStyle("mu_pt", 20);
    setFillStyle("trigTrk_pt", kYellow);

    // draw histogram
    drawHist(NameList("mu_pt", "trigTrk_pt"));

    // legend
    drawLegend(NameList("mu_eta", "trigTrk_eta"), 0.35,0.9,0.75,0.7);

    // draw efficiency
    new TCanvas("c_eff_mu_pt","c_eff_mu_pt");
    TGraphAsymmErrors* eff_mu_pt = new TGraphAsymmErrors(getHist("trigTrk_pt"), getHist("mu_pt"));
    eff_mu_pt->SetTitle("Trigger Tracking Efficiency;p_{T} (MeV)");
    eff_mu_pt->Draw("ap");
  }

  /// efficiency scan of muon identification
  if(isDefined("num_muID") && isDefined("jpsi_fex")) {
    new TCanvas("c_num_muID","c_num_muID");
    setFillStyle("num_muID", kYellow);

    // draw histogram
    drawHist("num_muID");

    //efficiency
    new TCanvas("c_eff_muID","c_eff_muID");

    //denominator
    TH1F* h_denom_muID = new TH1F(*getHist("num_muID"));
    for(int i=1; i<=h_denom_muID->GetNbinsX(); ++i) {
      h_denom_muID->SetBinContent(i,getHist("jpsi_fex")->Integral(loBin,hiBin-1));
    }

    TGraphAsymmErrors* eff_muID = new TGraphAsymmErrors(getHist("num_muID"), h_denom_muID);
    eff_muID->SetTitle("Muon id. efficiency w.r.t. candidates passing FEX ;No. of associated hits;Efficiency");
    eff_muID->Draw("ap");

  }

  /// efficiency scan of vertexing
  if(isDefined("num_vtx") && isDefined("jpsi_muID")) {
    new TCanvas("c_num_vtx","c_num_vtx");
    setFillStyle("num_vtx", kYellow);

    // draw histogram
    drawHist("num_vtx");

    //efficiency
    new TCanvas("c_eff_vtx","c_eff_vtx");

    //denominator
    TH1F* h_denom_vtx = new TH1F(*getHist("num_vtx"));
    for(int i=1; i<=h_denom_vtx->GetNbinsX(); ++i) {
      h_denom_vtx->SetBinContent(i,getHist("jpsi_muID")->Integral(loBin,hiBin-1));
    }

    TGraphAsymmErrors* eff_vtx = new TGraphAsymmErrors(getHist("num_vtx"), h_denom_vtx);
    eff_vtx->SetTitle("Vertexing efficiency w.r.t. cadidates after muon id.;Trigger vertexing #chi^{2} cut;Efficiency");
    eff_vtx->Draw("ap");

  }

  /// more plots
  drawMoreHist(NameList("trig_chi2"));

}
///===================================================

void TrigDiMuonRootCalib::analyzeRate(std::string chain,
                                      std::string settings,
                                      std::string cutStr)
{
  Log log("TrigDiMuonRootCalib::analyzeRate", ol());

  log.info("Settings: " + settings);
  log.info("cuts:     " + cutStr);

  // extrapolator
  std::string constantsFile = "constants.py";
  getValue(settings, "ConstantsFile=", constantsFile);

  std::string cutConeFile = "cut_cone.py";
  getValue(settings, "CutConeFile=", cutConeFile);

  TrigDiMuonExtrapolator* propagator = new TrigDiMuonExtrapolator(ol(), settings);
  propagator->read(constantsFile);
  propagator->readCutCone(cutConeFile);

  // total number of events
  int extNEvents = 1;
  getValue(settings, "NEvents=", extNEvents);

  // create selection algorithm
  CutsJpsi cuts(ol(), cutStr);

  //histograms
  importHistFromFile("histograms_analyzeRate.config");

  setValXVectPointer( "rate_muID", &m_rate_muID);
  setValXVectPointer( "rate_mu_muID", &m_rate_mu_muID);
  setValXVectPointer( "rate_diMu_muID", &m_rate_diMu_muID);
  setValXVectPointer( "rate_vtx",  &m_rate_vtx);
  setValXVectPointer( "rate_mu_vtx", &m_rate_mu_vtx);
  setValXVectPointer( "rate_diMu_vtx", &m_rate_diMu_vtx);

  // get histograms pointers we will need further
  TH1F* h_rate_muID = getHist("rate_muID");
  TH1F* h_rate_vtx  = getHist("rate_vtx");

  //loop over all entries and all files
  log.info("******************************************************************");
  log.info("** BEGINNING OF THE LOOP                                        **");
  log.info("******************************************************************");
  int nEvents = 0;
  int nAcc = 0;
  int nAcc2 = 0;
  int nAcc_singleMu = 0;
  int nAcc_diMu = 0;

  while( iterateFiles() ) {

    // load trees
    TTree* tTrig  = loadTree("AllTrigger");
    TTree* tMuons = loadTree("Muons");

    TrigHits*    trig = new TrigHits(tTrig);
    SimpleMuon* muons = new SimpleMuon(tMuons);

    // loop over entries
    for(int entry=0; entry<tTrig->GetEntries(); entry = nextEvent(entry, trig), ++nEvents) {

      //if(entry > 5000) break; //FIXME

      if(nEvents % 100 == 0) log.info("   ===> Entry " + iToStr(entry));
      trig->GetEntry(entry);
      muons->GetEntry(entry);

      /// level-2 trigger rate analysis ============================================
      log.debug("level-2 trigger rate analysis");

      // ****************
//       int run   = trig->runNumber;
//       int event = trig->eventNumber;
//       bool dec = trig->passTrigDec;

//       for(int i=0; run == trig->runNumber && event == trig->eventNumber && i < trig->fChain->GetEntries(); trig->GetEntry(++i)) {
//         if( trig->passTrigDec != dec)
//           log.warning("Trigger decision is not consistently filled");
//       }

      bool passDec = trig->passTrigDec;
      bool passHypo = cuts.selectHypo(entry, propagator, trig);
      bool isMuon = muons->muon_pt->size()>0;
      bool isDiMuon = muons->muon_pt->size()>1;
      if(passDec)   ++nAcc;
      if(passHypo ) ++nAcc2;
      if(passHypo && isMuon) ++nAcc_singleMu;
      if(passHypo && isDiMuon) ++nAcc_diMu;

      // ****************

      // check if trigChain passed different number-of-hits cuts
      if(h_rate_muID != NULL) {
        for(int nHits=0; nHits < h_rate_muID->GetNbinsX(); ++nHits) {
          if( cuts.selectHypo( entry, propagator, trig, nHits ) ) {
            m_rate_muID.push_back(nHits);
            if(isMuon) m_rate_mu_muID.push_back(nHits);
            if(isDiMuon) m_rate_diMu_muID.push_back(nHits);
          }
        }
      }

      if( h_rate_vtx != NULL && passHypo ) {
        // check if trigChain passed different vertexing chi2 cuts
        for(int j=0; j < h_rate_vtx->GetNbinsX(); ++j) {
          double chi2Cut = h_rate_vtx->GetBinLowEdge(j+1);   // scan
          if( cuts.selectHypo( entry, propagator, trig, -2, chi2Cut ) ) {
            m_rate_vtx.push_back(h_rate_vtx->GetBinCenter(j+1));
            if(isMuon) m_rate_mu_vtx.push_back(h_rate_vtx->GetBinCenter(j+1));
            if(isDiMuon) m_rate_diMu_vtx.push_back(h_rate_vtx->GetBinCenter(j+1));
          }
        }
      }


      /// fill the histograms ========================================================

      fillAll();

    } // end of the loop
    log.info("******************************************************************");
  } // end of the file loop

  //drawing ------------------------------

  /// efficiency of muon Id
  if(isDefined("rate_muID")) {
    TCanvas* c = new TCanvas("c_rate_muID","c_rate_muID");

    setFillStyle("rate_muID",  kRed);
    setFillStyle("rate_mu_muID",  kGreen);
    setFillStyle("rate_diMu_muID", kYellow);

    // draw histogram
    drawHist(NameList("rate_muID","rate_mu_muID","rate_diMu_muID"));

    // draw legend
    drawLegend(NameList("rate_muID","rate_mu_muID","rate_diMu_muID"), 0.60,0.9,0.9,0.65);

    c->RedrawAxis();

    //efficiency
    new TCanvas("c_rateFraction_muID","c_rateFraction_muID");

    //denominator
    TH1F* h_denom_muID = new TH1F(*getHist("rate_muID"));
    for(int i=1; i<=h_denom_muID->GetNbinsX(); ++i) {
      h_denom_muID->SetBinContent(i,extNEvents);
    }

    TGraphAsymmErrors* rateFraction_muID = new TGraphAsymmErrors(getHist("rate_muID"), h_denom_muID);
    rateFraction_muID->SetTitle("Fraction of input rate after muon id.;No. of associated hits;Reduction factor");
    rateFraction_muID->SetMarkerStyle(20);
    rateFraction_muID->Draw("ap");

  }

  /// efficiency of vertexing
  if(isDefined("rate_vtx")) {
    TCanvas* c = new TCanvas("c_rate_vtx","c_rate_vtx");

    setFillStyle("rate_vtx",  kRed);
    setFillStyle("rate_mu_vtx",  kGreen);
    setFillStyle("rate_diMu_vtx", kYellow);

    // draw histogram
    drawHist(NameList("rate_vtx","rate_mu_vtx","rate_diMu_vtx"));

    // draw legend
    drawLegend(NameList("rate_vtx","rate_mu_vtx","rate_diMu_vtx"), 0.60,0.9,0.9,0.65);

    c->RedrawAxis();

    //efficiency
    new TCanvas("c_rateFraction_vtx","c_rateFraction_vtx");

    //denominator
    TH1F* h_denom_vtx = new TH1F(*getHist("rate_vtx"));
    for(int i=1; i<=h_denom_vtx->GetNbinsX(); ++i) {
      h_denom_vtx->SetBinContent(i,extNEvents);
    }

    TGraphAsymmErrors* rateFraction_vtx = new TGraphAsymmErrors(getHist("rate_vtx"), h_denom_vtx);
    rateFraction_vtx->SetTitle("Fraction of input rate after vertexing;Vertexing #chi^{2} cut;Reduction factor");
    rateFraction_vtx->SetMarkerStyle(20);
//     rateFraction_vtx->SetMarkerSize(0.1);
    rateFraction_vtx->Draw("ap");

  }

//   if(isDefined("rate_muID")) {
//     nAcc = getHist("rate_muID")->GetBinContent( getHist("rate_muID")->GetBin( cuts.cut_hypoNMuHits.val+1 ) );
//   }

  log.info("Accepted by trigDec: " + iToStr(nAcc));
  log.info("Accepted by Hypo   : " + fToStr(nAcc2));
  log.info("Single muon events Accepted by Hypo: " + fToStr(nAcc_singleMu));
  log.info("Di-muon events Accepted by Hypo    : " + fToStr(nAcc_diMu));

  //   log.info("Accepted by trigDec with no Hypo: " + iToStr(nDecNoHypo));
//   log.info("Accepted by Hypo with no trigDec: " + fToStr(nHypoNoDec));
//   log.info("Rejection wrt. Level-1: "+fToStr((double)nAcc/nEvents));

}
///===================================================
bool TrigDiMuonRootCalib::iterateDetails(std::string nameDetails, TTree** tDetails, Details**    details,
                                         std::string nameJpsi,    TTree** tJpsi,    ParticleBag* jpsi,
                                         std::string nameTracks,  TTree** tTracks,  TrackBag*    tracks,
                                         std::string nameMuons,   TTree** tMuons,   MuonBag*     muons    )
{
  Log log("TrigDiMuonRootCalib::iterateDetails", ol());

  //update current entry
  ++m_currentEntry;

  //end of file reached
  if(m_currentEntry >= m_currentNEntries) {
    m_currentEntry = 0;
    m_currentNEntries = -1;
  }

  // process first entry in the file
  if(m_currentEntry == 0) {
    // sanity check
    if(tDetails == NULL || details==NULL) {
      log.error("The tDetails tree is not defined. Leaving...");
      return false;
    }

    // sanity check
    if((tJpsi!=NULL && jpsi==NULL) ||
       (tTracks!=NULL && tracks==NULL) ||
       (tMuons!=NULL && muons==NULL))
    {
      log.error("Wrong setup");
      return false;
    }

    // loop over files until all the required trees are loaded
    bool allOK = false;
    while( iterateFiles() ) {
      allOK = false;

      // load trees
      *tDetails = loadTree(nameDetails);

      if(tJpsi!=NULL)   *tJpsi   = loadTree(nameJpsi);
      if(tTracks!=NULL) *tTracks = loadTree(nameTracks);
      if(tMuons!=NULL)  *tMuons  = loadTree(nameMuons);

      // check that trees were loaded correctly
      if(*tDetails == NULL ||
          (tJpsi!=NULL && *tJpsi == NULL) ||
          (tTracks!=NULL && *tTracks == NULL) ||
          (tMuons!=NULL && *tMuons == NULL)    )
      {
        log.info("Some trees were not loaded correctly");
        addCorruptedFile(currentFileName());
        continue;
      }

      //skip files with no entries
      if((*tDetails)->GetEntries() == 0) {
        log.info("No entries, skipping file");
        continue;
      }

      //create Details class
      *details = new Details(*tDetails);

      //prepare NtupleReader
      log.debug("Preparing NtupleReader");
      if(tJpsi!=NULL && (*tJpsi)->GetEntries() > 0) m_ntupleReader.prepareTree(nameJpsi, *tJpsi);
      if(tTracks!=NULL && (*tTracks)->GetEntries() > 0) m_ntupleReader.prepareTree(nameTracks, *tTracks);
      if(tMuons!=NULL && (*tMuons)->GetEntries() > 0) m_ntupleReader.prepareTree(nameMuons, *tMuons);


      // everything seems to be OK, we can use this file
      allOK = true;
      m_currentNEntries = (*tDetails)->GetEntries();
      break;

    } // end of loop over files

    if(!allOK) return false;
  }

  // read entries
  if(m_currentEntry<m_currentNEntries) {
    log.debug("Processing entry "+iToStr(m_currentEntry));

    // load Details object
    (*tDetails)->GetEntry(m_currentEntry);

    log.debug("Load Jpsi "+iToStr(m_currentEntry));
    // load other b-physics objects
    if(tJpsi!=NULL)   *jpsi   = m_ntupleReader.getParticles((*details)->RunNumber, (*details)->EvtNumber, nameJpsi,   *tJpsi);
    if(tTracks!=NULL) *tracks = m_ntupleReader.getTracks   ((*details)->RunNumber, (*details)->EvtNumber, nameTracks, *tTracks);
    if(tMuons!=NULL)  *muons  = m_ntupleReader.getMuons    ((*details)->RunNumber, (*details)->EvtNumber, nameMuons,  *tMuons);
    log.debug("done... ");
  }

  return true;
}
///===================================================
bool TrigDiMuonRootCalib::iterateJpsi(std::string nameJpsi,    TTree** tJpsi,   BCompositeParticle* jpsi,
                                       std::string nameTracks, TTree** tTracks, TrackBag* tracks,
                                       std::string nameMuons,  TTree** tMuons,  MuonBag* muons,
                                       std::string nameTrig,   TTree** tTrig,   TriggerBag* trigger,
                                       std::string nameChain,  TTree** tChain,  Chain** trigChain,
                                       std::string nameDetails,TTree** tDetails,Details** details )
{
  Log log("TrigDiMuonRootCalib::iterateJpsi", ol());

  //update current entry
  ++m_currentEntry;

  //end of file reached
  if(m_currentEntry >= m_currentNEntries) {
    m_currentEntry = 0;
    m_currentNEntries = -1;
  }

  // process first entry in the file
  if(m_currentEntry == 0) {
    // sanity check
    if(tJpsi == NULL) {
      log.error("The Jpsi tree is not defined. Leaving...");
      return false;
    }
    // sanity check
    if((tTracks!=NULL && tracks==NULL) ||
        (tMuons!=NULL  && muons==NULL) ||
        (tTrig!=NULL   && trigger==NULL) ||
        (tChain!=NULL  && trigChain==NULL) ||
        (tDetails!=NULL  && details==NULL))
    {
      log.error("Wrong setup");
      return false;
    }

    // loop over files until all the required trees are loaded
    bool allOK = false;
    while( iterateFiles() ) {
      allOK = false;

      // load trees
      *tJpsi = loadTree(nameJpsi);
      if(tTracks!=NULL) *tTracks = loadTree(nameTracks);
      if(tMuons!=NULL)  *tMuons = loadTree(nameMuons);
      if(tTrig!=NULL)   *tTrig = loadTree(nameTrig);
      if(tChain!=NULL)  *tChain = loadTree(nameChain);
      if(tDetails!=NULL)*tDetails = loadTree(nameDetails);

      // check that trees were loaded correctly
      if(*tJpsi == NULL ||
          (tTracks!=NULL && *tTracks == NULL) ||
          (tMuons!=NULL && *tMuons == NULL) ||
          (tTrig!=NULL && *tTrig == NULL) ||
          (tChain!=NULL && *tChain == NULL) ||
          (tDetails!=NULL && *tDetails == NULL))
      {
        log.info("Some trees were not loaded correctly");
        addCorruptedFile(currentFileName());
        continue;
      }

      //skip files with no entries
      if((*tJpsi)->GetEntries() == 0) {
        log.info("No entries, skipping file");
        continue;
      }

      //prepare NtupleReader
      log.debug("Preparing NtupleReader");

      if(tTracks!=NULL && (*tTracks)->GetEntries() > 0) m_ntupleReader.prepareTree(nameTracks, *tTracks);
      if(tMuons!=NULL  && (*tMuons)->GetEntries() > 0)  m_ntupleReader.prepareTree(nameMuons,  *tMuons);
      if(tTrig!=NULL   && (*tTrig)->GetEntries() > 0)   m_ntupleReader.prepareTree(nameTrig,   *tTrig);

      //prepare Chain class
      if(tChain !=NULL) *trigChain = new Chain(*tChain, nameChain);

      //prepare Details class
      if(tDetails !=NULL) *details = new Details(*tDetails);
      m_detailsEntry = 0;


      // everything seems to be OK, we can use this file
      allOK = true;
      m_currentNEntries = (*tJpsi)->GetEntries();
      break;

    } // end of loop over files

    if(!allOK) return false;
  }

  // read entries
  if(m_currentEntry<m_currentNEntries) {
    log.debug("Processing entry "+iToStr(m_currentEntry));
    //load jpsi
    *jpsi = m_ntupleReader.readParticleTree(m_currentEntry, nameJpsi,  *tJpsi );

    // load other b-physics objects
    if(tTracks!=NULL) *tracks  = m_ntupleReader.getTracks  (jpsi->runNumber(), jpsi->eventNumber(), nameTracks, *tTracks);
    if(tMuons!=NULL)  *muons   = m_ntupleReader.getMuons   (jpsi->runNumber(), jpsi->eventNumber(), nameMuons,  *tMuons);
    if(tTrig!=NULL)   *trigger = m_ntupleReader.getTriggers(jpsi->runNumber(), jpsi->eventNumber(), nameTrig,   *tTrig);

    // load trigger chain object
    if(tChain!=NULL)  (*trigChain)->GetEntry(m_currentEntry);

    // load Details
    if(tDetails!=NULL && tChain!=NULL && (*trigChain)->Matched_fitMass->size()!=0) {
      for((*details)->GetEntry(m_detailsEntry);
            ((*details)->RunNumber != (*jpsi).runNumber() || (*details)->EvtNumber != (*jpsi).eventNumber()) && m_detailsEntry<(*tDetails)->GetEntries();
            (*details)->GetEntry(++m_detailsEntry))
      {
//         log.info("run = "+iToStr((*jpsi).runNumber())+" ("+iToStr((*details)->RunNumber)+"), event = "+iToStr((*jpsi).eventNumber())+" ("+iToStr((*details)->EvtNumber)+")");
      }

      if(m_detailsEntry == (*tDetails)->GetEntries()) {
        log.error("End of Details tree reached");
        return false;
      }
    }
  }

  return true;

}
///===================================================
bool TrigDiMuonRootCalib::iterateChain( std::string nameChain,
                                        TTree** tChain,
                                        Chain** trigChain,
                                        std::string nameTrig,
                                        TTree** tTrig,
                                        TriggerBag* trigger,
                                        std::string nameDetails,
                                        TTree** tDetails,
                                        Details** details,
                                        std::string nameMuons,  TTree** tMuons,  MuonBag* muons,
                                        bool doAllTracks,
                                        bool doMatchedTracks)
{
  Log log("TrigDiMuonRootCalib::iterateChain", ol());

  //update current entry
  ++m_currentEntry;

  //end of file reached
  if(m_currentEntry >= m_currentNEntries) {
    m_currentEntry = 0;
    m_currentNEntries = -1;
  }

  // process first entry in the file
  if(m_currentEntry == 0) {
    // sanity check
    if(tChain == NULL || trigChain==NULL) {
      log.error("The Chain tree is not defined. Leaving...");
      return false;
    }
    // sanity check
    if( tChain==NULL ||
        (tTrig!=NULL  && trigger==NULL) ||
        (tDetails!=NULL  && details==NULL) ||
        (tMuons!=NULL  && muons==NULL) )
    {
      log.error("Wrong setup");
      return false;
    }

    // loop over files until all the required trees are loaded
    bool allOK = false;
    while( iterateFiles() ) {
      allOK = false;

      // load trees
      *tChain = loadTree(nameChain);
      if(tTrig!=NULL)    *tTrig    = loadTree(nameTrig);
      if(tDetails!=NULL) *tDetails = loadTree(nameDetails);
      if(tMuons!=NULL)   *tMuons   = loadTree(nameMuons);

      // check that trees were loaded correctly
      if(*tChain == NULL ||
          (tTrig!=NULL && *tTrig == NULL) ||
          (tDetails!=NULL && *tDetails == NULL) ||
          (tMuons!=NULL && *tMuons == NULL))
      {
        log.info("Some trees were not loaded correctly");
        addCorruptedFile(currentFileName());
        continue;
      }

      //skip files with no entries
      if((*tChain)->GetEntries() == 0) {
        log.info("No entries, skipping file");
        continue;
      }

      // check that tTrig and tChain trees have the same length
      if(tChain!=NULL && (*tTrig)->GetEntries() != (*tChain)->GetEntries()) {
        log.info("(*tTrig)->GetEntries() != (*tChain)->GetEntries(), skipping file");
        continue;
      }

      //create Chain class
      *trigChain = new Chain(*tChain, nameChain, doAllTracks, doMatchedTracks);

      // prepare Trig tree
      if(tTrig!=NULL && (*tTrig)->GetEntries() > 0)   m_ntupleReader.prepareTree(nameTrig,   *tTrig);

      //prepare Details class
      if(tDetails !=NULL) *details = new Details(*tDetails);
      m_detailsEntry = 0;

      // prepare muons
      if(tMuons!=NULL  && (*tMuons)->GetEntries() > 0)  m_ntupleReader.prepareTree(nameMuons,  *tMuons);


      // everything seems to be OK, we can use this file
      allOK = true;
      m_currentNEntries = (*tChain)->GetEntries();
      break;

    } // end of loop over files

    if(!allOK) return false;
  }

  // read entries
  if(m_currentEntry<m_currentNEntries) {
    log.debug("Processing entry "+iToStr(m_currentEntry)+" out of "+iToStr((*trigChain)->fChain->GetEntries()));

    (*trigChain)->GetEntry(m_currentEntry);

    if(tTrig!=NULL) *trigger = m_ntupleReader.readTriggerTree(m_currentEntry, nameTrig,   *tTrig);

    // load Details
    if(tDetails!=NULL && tChain!=NULL && (*trigChain)->All_fitMass->size()!=0) {
      for((*details)->GetEntry(m_detailsEntry);
            ((*details)->RunNumber != (*trigChain)->RunNumber || (*details)->EvtNumber != (*trigChain)->EventNumber) && m_detailsEntry<(*tDetails)->GetEntries();
            (*details)->GetEntry(++m_detailsEntry))
      {
        log.debug("run = "+iToStr((*trigChain)->RunNumber)+" ("+iToStr((*details)->RunNumber)+"), event = "+iToStr((*trigChain)->EventNumber)+" ("+iToStr((*details)->EvtNumber)+")");
      }

      if(m_detailsEntry == (*tDetails)->GetEntries()) {
        log.error("End of Details tree reached");
        return false;
      }
    }

    // load muons
    if(tMuons!=NULL) *muons = m_ntupleReader.getMuons((*trigChain)->RunNumber, (*trigChain)->EventNumber, nameMuons,  *tMuons);


//     //load Chains
//     for((*trigChain)->GetEntry(m_currentEntry);
//           (*trigChain)->All_fitMass->size()==0 && m_currentEntry < m_currentNEntries;
//           (*trigChain)->GetEntry(++m_currentEntry));
//
//     if(m_currentEntry != m_currentNEntries) {
//
//       //log.info("run = "+iToStr((*trigChain)->RunNumber)+", event = "+iToStr((*trigChain)->EventNumber));
//
//       *trigger = m_ntupleReader.getTriggers((*trigChain)->RunNumber, (*trigChain)->EventNumber, nameTrig,   *tTrig);
//
//       // load Details
//       if(tDetails!=NULL && tChain!=NULL && (*trigChain)->All_fitMass->size()!=0) {
//         for((*details)->GetEntry(m_detailsEntry);
//               ((*details)->RunNumber != (*trigChain)->RunNumber || (*details)->EvtNumber != (*trigChain)->EventNumber) && m_detailsEntry<(*tDetails)->GetEntries();
//               (*details)->GetEntry(++m_detailsEntry))
//         {
//           log.debug("run = "+iToStr((*trigChain)->RunNumber)+" ("+iToStr((*details)->RunNumber)+"), event = "+iToStr((*trigChain)->EventNumber)+" ("+iToStr((*details)->EvtNumber)+")");
//         }
//
//         if(m_detailsEntry == (*tDetails)->GetEntries()) {
//           log.error("End of Details tree reached");
//           return false;
//         }
//       }
//     }
  }

  return true;
}
///===================================================
TF1* TrigDiMuonRootCalib::fitGaus(std::string name, std::string settings) {

  TF1* fitFun = NULL;

  if(isDefined(name)) {
    double fitMin = 2500;
    double fitMax = 3700;

    getValue(settings, "fitMin=", fitMin);
    getValue(settings, "fitMax=", fitMax);

    if(settings.find("gaus+pol1")!=string::npos) {
      fitFun = new TF1("fitFun","gaus(0)+pol1(3)",fitMin,fitMax);
    } else if(settings.find("gaus+const")!=string::npos) {
      fitFun = new TF1("fitFun","gaus(0)+[3]",fitMin,fitMax);
    } else {
      fitFun = new TF1("fitFun","gaus(0)",fitMin,fitMax);
    }

    fitFun->SetParameters(0.5*getHist(name)->GetEntries(), 3.1, 0.07, getHist(name)->GetEntries() / (fitMax-fitMin));
    fitFun->SetParNames("Area", "#mu", "#sigma", "p_{0}", "p_{1}");
    fitFun->SetParLimits(2,0,1000);
//     fitFun->SetParLimits(3,0,50);

    getHist(name)->Fit(fitFun,"0 R");

    fitFun->SetLineColor(kBlue);
  }

  return fitFun;
}
///===================================================
bool TrigDiMuonRootCalib::getEfficiencies(std::vector<double>& num,
                                           double& denom,
                                           std::vector<double>& eff,
                                           std::vector<std::string> numHists,
                                           std::string denomHist,
                                           int loBin, int hiBin)
{
  //initialize
  denom = 0;
  num = std::vector<double>(numHists.size(), 0);
  eff = std::vector<double>(numHists.size(), 0);

  //sanity check
  if(numHists.empty() || !isDefined(denomHist))
    return false;

  //integrate histograms
  denom = getHist(denomHist)->Integral(loBin, hiBin-1);
  num = std::vector<double>(numHists.size(), 0);

  std::vector<std::string>::iterator numHistsItr = numHists.begin();
  std::vector<double>::iterator numItr = num.begin();
  for(; numHistsItr!=numHists.end(); ++numHistsItr, ++numItr) {
    if(isDefined(*numHistsItr)) (*numItr) = getHist(*numHistsItr)->Integral(loBin, hiBin-1);
  }

  //calculate efficiensies
  if(denom!=0) {
    std::vector<double>::iterator numItr = num.begin();
    std::vector<double>::iterator effItr = eff.begin();
    for(; numItr!=num.end(); ++numItr, ++effItr) {
      (*effItr) = (*numItr)/denom;
    }
  }

  return true;
}
///===================================================
int nextEvent(int entry, TrigHits* trig) {

  trig->GetEntry(entry);
  int run   = trig->runNumber;
  int event = trig->eventNumber;

  for(; run == trig->runNumber && event == trig->eventNumber && entry < trig->fChain->GetEntries(); trig->GetEntry(++entry));

  return entry;
}
///===================================================

///===================================================
///      . .
///====o==V==o========================================
