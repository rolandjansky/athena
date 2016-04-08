/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ConvertData.cxx
*/
#include "TriggerMenuAnalysis/ConvertData.h"
#include "TriggerMenuAnalysis/ElectronObjectsInRoI.h"
#include "TriggerMenuAnalysis/MuonObjectsInRoI.h"
#include "TriggerMenuAnalysis/TauObjectsInRoI.h"
#include "TriggerMenuAnalysis/JetObjectsInRoI.h"
#include "TriggerMenuAnalysis/TileMuObjectsInRoI.h"
#include "TriggerMenuAnalysis/MissingETObjectsInRoI.h"

#include "TriggerMenuNtuple/RoIData_Muon.h"
#include "TriggerMenuNtuple/RoIData_TileMu.h"
#include "TriggerMenuNtuple/RoIData_Electron.h"
#include "TriggerMenuNtuple/RoIData_Tau.h"
#include "TriggerMenuNtuple/RoIData_Jet.h"
#include "TriggerMenuNtuple/RoIData_MissingET.h"
#include "TriggerMenuNtuple/MSMuon.h"
#include "TriggerMenuNtuple/CBMuon.h"
#include "TriggerMenuNtuple/TileMuon.h"
#include "TriggerMenuNtuple/TileL2Muon.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigParticle/TrigTau.h"
#include "tauEvent/TauJetContainer.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "JetEvent/JetCollection.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "egammaEvent/egammaContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "egammaEvent/EMShower.h"

#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"

#include "TrigMissingEtEvent/TrigMissingET.h"

using CLHEP::GeV;


void
setMSMuon(MSMuon& m, const Rec::TrackParticle* p)
{
  m.setMSMuon( p->pt(), 
               p->eta(), 
               p->phi(), 
               p->fitQuality()->chiSquared(), 
               p->fitQuality()->numberDoF(), 
               p->measuredPerigee()->parameters()[Trk::d0],
               p->measuredPerigee()->parameters()[Trk::z0],
               p->trackSummary()->get(Trk::numberOfMdtHits),
               p->trackSummary()->get(Trk::numberOfRpcPhiHits),
               p->trackSummary()->get(Trk::numberOfRpcEtaHits),
               p->trackSummary()->get(Trk::numberOfTgcPhiHits),
               p->trackSummary()->get(Trk::numberOfTgcEtaHits),
               p->trackSummary()->get(Trk::numberOfCscPhiHits),
               p->trackSummary()->get(Trk::numberOfCscEtaHits) );

  /*
  m.setMSMuon( p->pt(), 
               p->eta(), 
               p->phi(), 
               0,0,0,0,0,0,0,0,0);
  */
}

void 
setSAMuon(CBMuon& m, const Rec::TrackParticle* p)
{
  m.setSAMuon( p->pt(), 
               p->eta(), 
               p->phi(), 
               p->fitQuality()->chiSquared(), 
               p->fitQuality()->numberDoF(),
               p->measuredPerigee()->parameters()[Trk::d0],
               p->measuredPerigee()->parameters()[Trk::z0]);

  /*
  m.setSAMuon( p->pt(), 
               p->eta(), 
               p->phi(), 
               0,0);
  */

}

void 
setCBMuon(CBMuon& m, const Rec::TrackParticle* p)
{
  m.setCBMuon( p->pt(), 
               p->eta(), 
               p->phi(), 
               p->fitQuality()->chiSquared(), 
               p->fitQuality()->numberDoF(),
               p->measuredPerigee()->parameters()[Trk::d0],
               p->measuredPerigee()->parameters()[Trk::z0]);

  /*
  m.setCBMuon( p->pt(), 
               p->eta(), 
               p->phi(), 
               0,0);
  */
}

bool convertElectronData(RoIData_Electron& out, const ElectronObjectsInRoI& in) {
  out.setAssociatedChains(in.getAssociatedChains());
  out.setPassedChains(in.getPassedChains());

  const TrigEMCluster* x1 = in.getEMCluster();
  if (x1){
    const std::vector<TrigStatus> v1 = in.getChains( x1 );
    out.setEMCluster( x1->e(), x1->et(), x1->eta(),x1->phi(), v1);
  }

  const TrigInDetTrackCollection* x4 = in.getInDetTrackCollection();
  if(x4){
    const std::vector<TrigStatus> v4 = in.getChains( x4 );
      out.setL2ID( v4 );
  }

  const TrigElectronContainer* x3 = in.getTrigElectronContainer();
  if(x3){
    const std::vector<TrigStatus> v3 = in.getChains(x3);
    if(x3->size() == 0){
    } else if(x3->size() == 1){
      out.setL2Electron( (*x3)[0]->charge(), (*x3)[0]->EThad(), (*x3)[0]->Eratio(),(*x3)[0]->Rcore(),
			 (*x3)[0]->trkClusDeta(), (*x3)[0]->trkClusDphi(), (*x3)[0]->EtOverPt(),
			 (*x3)[0]->trkPt(), (*x3)[0]->trackAlgo(), v3);
    } else{
      out.setL2Electron( (*x3)[0]->charge(), (*x3)[0]->EThad(), (*x3)[0]->Eratio(),(*x3)[0]->Rcore(),
			 (*x3)[0]->trkClusDeta(), (*x3)[0]->trkClusDphi(), (*x3)[0]->EtOverPt(),
			 (*x3)[0]->trkPt(), (*x3)[0]->trackAlgo(), v3);
    }
  }

  const egammaContainer* x2 = in.getEFegamma();
  if (x2){
    const std::vector<TrigStatus> v2 = in.getChains(x2);
    if(x2->size() == 0){
    } else if (x2->size() == 1){
      const EMShower* y = (*x2)[0]->detail<EMShower>();
      out.setEFegamma( (*x2)[0]->pt(), (*x2)[0]->eta(), (*x2)[0]->phi(), v2, (*x2)[0]->e(),
		       (*x2)[0]->et(), y->r33over37allcalo(), y->e233(), y->e237(), y->e277(), y->f1(),
		       y->emins1(), y->fracs1(), y->e2tsts1(), y->weta1(), y->wtots1(),
		       y->weta2(), y->ethad(), y->ethad1(), static_cast<int>( (*x2)[0]->charge()) );
    } else {
      int highestPt=0;
      for(unsigned int i=0; i<x2->size(); i++){
	if( (*x2)[i]->pt() > (*x2)[highestPt]->pt() )
	  highestPt=i;
      }
      const EMShower* y2 = (*x2)[highestPt]->detail<EMShower>();
      out.setEFegamma( (*x2)[highestPt]->pt(), (*x2)[highestPt]->eta(), (*x2)[highestPt]->phi(), v2, (*x2)[highestPt]->e(),
		       (*x2)[highestPt]->et(), y2->r33over37allcalo(), y2->e233(), y2->e237(), y2->e277(), y2->f1(),
		       y2->emins1(), y2->fracs1(), y2->e2tsts1(), y2->weta1(), y2->wtots1(),
		       y2->weta2(), y2->ethad(), y2->ethad1(), static_cast<int>( (*x2)[highestPt]->charge()) );
      std::cout << x2->size() << "Electron in RoI" << std::endl;
    }

  }
  return true;
}

bool convertMuonData(RoIData_Muon& out, const MuonObjectsInRoI& in) {
  out.setAssociatedChains(in.getAssociatedChains());
  out.setPassedChains(in.getPassedChains());

  const Muon_ROI* l1 = in.getMuonRoI();
  if (l1){
    setL1(out, l1);
  }

  const MuonFeature* x1 = in.getMuonFeature();
  if (x1){
    const std::vector<TrigStatus> v1 = in.getChains( x1 );
    std::vector<std::string> v1s;
    std::vector<int>         v1i;
    setTrigStatus(v1, v1s, v1i);

    //setMuFast(out, x1, v1);
    setMuFast(out, x1, v1s, v1i);
  }

  const TrigInDetTrackCollection* x2t = in.getInDetTrackCollection();
  if (x2t){
    //const std::vector<TrigStatus> v2t = in.getChains( x2t );
    //std::vector<std::string> v2ts;
    //std::vector<int>         v2ti;
    //setTrigStatus(v2t, v2ts, v2ti);


    //setInDet(out, x2t, v2t);
    setInDet(out, x2t);
  }

  const CombinedMuonFeature* x2 = in.getCombinedMuonFeature();
  if (x2){
    const std::vector<TrigStatus> v2 = in.getChains( x2 );
    std::vector<std::string> v2s;
    std::vector<int>         v2i;
    setTrigStatus(v2, v2s, v2i);

    //setMuComb(out, x2, v2);
    setMuComb(out, x2, v2s, v2i);
  }

  const IsoMuonFeature* x2i = in.getIsoMuonFeature();
  if (x2i){
    const std::vector<TrigStatus> v2i = in.getChains( x2i );
    std::vector<std::string> v2is;
    std::vector<int>         v2ii;
    setTrigStatus(v2i, v2is, v2ii);

    //setMuIso(out, x2i, v2i);
    setMuIso(out, x2i, v2is, v2ii);
  }


  /*
  const TrigMuonEFContainer* x3 = in.getEFMuon();
  if (x3){
    const std::vector<TrigStatus> v3 = in.getChains( x3 );
    if (x3->size() == 0){
    } else if(x3->size() == 1){
      out.setEFMuon( (*x3)[0]->pt(), (*x3)[0]->eta(), (*x3)[0]->phi(), v3 );
    } else{
      int highestPt=0;
      for(unsigned int i=0; i<x3->size(); i++){
	if( (*x3)[i]->pt() > (*x3)[highestPt]->pt() )
	  highestPt=i;
      }
      out.setEFMuon( (*x3)[highestPt]->pt(), (*x3)[highestPt]->eta(), (*x3)[highestPt]->phi(), v3 );
      std::cout << x3->size() << " EFMuons found in RoI" << std::endl;
    }
  }
  */

  const TrigMuonEFInfoContainer* x3 = in.getEFMuonInfo();
  if(x3){
    const std::vector<TrigStatus> v3 = in.getChains( x3 );
    std::vector<std::string> v3s;
    std::vector<int>         v3i;
    setTrigStatus(v3, v3s, v3i);

    //setMuonEF(out, x3, v3);
    setMuonEF(out, x3, v3s, v3i);
  }

  return true;
}

bool convertTauData(RoIData_Tau& out, const TauObjectsInRoI& in) {
  out.setAssociatedChains(in.getAssociatedChains());
  out.setPassedChains(in.getPassedChains());

  const TrigTauCluster* x1 = in.getTauCluster();
  if (x1){
    const std::vector<TrigStatus> v1 = in.getChains( x1 );
    out.setTauCluster(x1->et(), x1->eta(), x1->phi(), v1);
  }
  const TrigTau* x2 = in.getTau();
  if (x2){
    const std::vector<TrigStatus> v2 = in.getChains( x2 );
    out.setL2Tau(x2->pt(), x2->eta(), x2->phi(), v2);
  }
  const Analysis::TauJetContainer* x3 = in.getEFTau();
  if (x3){
    const std::vector<TrigStatus> v3 = in.getChains( x3 );
    if(x3->size() == 0){
    } else if(x3->size() == 1){
      out.setEFTau( (*x3)[0]->pt(), (*x3)[0]->eta(), (*x3)[0]->phi(), v3 );
    } else{
      int highestPt=0;
      for(unsigned int i=0; i<x3->size(); i++){
	if( (*x3)[i]->pt() > (*x3)[highestPt]->pt() )
	  highestPt=i;
      }
      out.setEFTau( (*x3)[highestPt]->pt(), (*x3)[highestPt]->eta(), (*x3)[highestPt]->phi(), v3 );
    }
  }
  return true;
}


bool convertJetData(RoIData_Jet& out, const JetObjectsInRoI& in) {
  out.setAssociatedChains(in.getAssociatedChains());
  out.setPassedChains(in.getPassedChains());

  const TrigT2Jet* x1 = in.getL2Jet();
  if (x1){
    const std::vector<TrigStatus> v1 = in.getChains( x1 );
    out.setL2Jet(x1->et(), x1->eta(),x1->phi(), v1);
  }
  const JetCollection* x2 = in.getEFJet();
  if (x2){
    const std::vector<TrigStatus> v2 = in.getChains( x2 );
    if(x2->size() == 0){
    } else if (x2->size() == 1){
      out.setEFJet( (*x2)[0]->et(), (*x2)[0]->eta(), (*x2)[0]->phi(), v2 );
    } else {
      int highestPt=0;
      for(unsigned int i=0; i<x2->size(); i++){
	if( (*x2)[i]->pt() > (*x2)[highestPt]->pt() )
	  highestPt=i;
      }
      out.setEFJet( (*x2)[highestPt]->et(), (*x2)[highestPt]->eta(), (*x2)[highestPt]->phi(), v2 );
      std::cout << x2->size() << "Jet in RoI" << std::endl;
    }
  }
  return true;
}


bool convertTileMuData(RoIData_TileMu& out, const TileMuObjectsInRoI& in) {
  out.setAssociatedChains(in.getAssociatedChains());
  out.setPassedChains(in.getPassedChains());

  const Muon_ROI* l1 = in.getMuonRoI();
  if (l1){
    setL1(out, l1);
  }
  const TileMuFeature* x1 = in.getTileMuFeature();
  if (x1){
    const std::vector<TrigStatus> v1 = in.getChains( x1 );
    std::vector<std::string> v1s;
    std::vector<int>         v1i;
    setTrigStatus(v1, v1s, v1i);

    //out.setTileMu(x1->eta(), x1->phi(), x1->enedep(), x1->quality(), v1 );
    out.setTileMu(x1->eta(), x1->phi(), x1->enedep(), x1->quality(), v1s, v1i );
  }

  const TileTrackMuFeature* x2 = in.getTileTrackMuFeature();
  if (x2){
    const std::vector<TrigStatus> v2 = in.getChains( x2 );
    std::vector<std::string> v2s;
    std::vector<int>         v2i;
    setTrigStatus(v2, v2s, v2i);

    //out.setTileTrackMu(x2->pt(), x2->eta(), x2->phi(), v2);
    out.setTileTrackMu(x2->pt(), x2->eta(), x2->phi(), v2s, v2i);
  }

  /*
  const TrigMuonEFContainer* x3 = in.getEFMuon();
  if (x3){
    const std::vector<TrigStatus> v3 = in.getChains( x3 );
    if (x3->size() == 0){
    } else if(x3->size() == 1){
      out.setEFMuon( (*x3)[0]->pt(), (*x3)[0]->eta(), (*x3)[0]->phi(), v3 );
    } else{
      int highestPt=0;
      for(unsigned int i=0; i<x3->size(); i++){
	if( (*x3)[i]->pt() > (*x3)[highestPt]->pt() )
	  highestPt=i;
      }
      out.setEFMuon( (*x3)[highestPt]->pt(), (*x3)[highestPt]->eta(), (*x3)[highestPt]->phi(), v3 );
      std::cout << x3->size() << " EFMuons found in RoI" << std::endl;
    }
  }
  */

  const TrigMuonEFInfoContainer* x3 = in.getEFMuonInfo();
  if(x3){
    const std::vector<TrigStatus> v3 = in.getChains( x3 );
    std::vector<std::string> v3s;
    std::vector<int>         v3i;
    setTrigStatus(v3, v3s, v3i);

    //setMuonEF(out, x3, v3);
    setMuonEF(out, x3, v3s, v3i);
  }

  return true;
}

bool convertMissingETData(RoIData_MissingET& out, const MissingETObjectsInRoI& in) {
  out.setAssociatedChains(in.getAssociatedChains());
  out.setPassedChains(in.getPassedChains());

  const TrigMissingET* x1 = in.getL2MissingET();
  if (x1){
    const std::vector<TrigStatus> v1 = in.getChains( x1 );
    out.setL2MissingET(x1->sumEt(), x1->et(), x1->ex(), x1->ey(), x1->getFlag(), v1);
  }
  const TrigMissingET* x2 = in.getEFMissingET();
  if (x2){
    const std::vector<TrigStatus> v2 = in.getChains( x2 );
    out.setEFMissingET(x2->sumEt(), x2->et(), x2->ex(), x2->ey(), v2 );
  }
  return true;
}

//for set 

void
setL1(RoIData_Muon& m, const Muon_ROI* f)
{

  if( !f )return;

  if( f->getSource() == Muon_ROI::Barrel ){
    m.L1_RoISource  = 0;
  }else if( f->getSource() == Muon_ROI::Endcap ){
    m.L1_RoISource  = 1;
  }else if( f->getSource() == Muon_ROI::Forward ){
    m.L1_RoISource  = 2;
  }else{
    m.L1_RoISource  = -1;
  }

  m.L1_ThrNumber  = f->getThrNumber();
  m.L1_RoINumber  = f->getRoI();
  m.L1_ThrNumber  = f->getThrNumber();
  m.L1_SectorAddress  = f->getSectorAddress();

  if( f->getHemisphere() == Muon_ROI::Positive ){
    m.L1_isAside = true;
  }else{
    m.L1_isAside = false;
  }

  m.L1_isFirstCandidate    = f->isFirstCandidate();
  m.L1_isMoreCandInRoI     = f->isMoreCandInRoI();
  m.L1_isMoreCandInSector  = f->isMoreCandInSector();


  m.L1_pt  = f->pt();
  m.L1_eta = f->eta();
  m.L1_phi = f->phi();

}

void
setL1(RoIData_TileMu& m, const Muon_ROI* f)
{

  if( !f ) return;

  if( f->getSource() == Muon_ROI::Barrel ){
    m.L1_RoISource  = 0;
  }else if( f->getSource() == Muon_ROI::Endcap ){
    m.L1_RoISource  = 1;
  }else if( f->getSource() == Muon_ROI::Forward ){
    m.L1_RoISource  = 2;
  }else{
    m.L1_RoISource  = -1;
  }

  m.L1_ThrNumber  = f->getThrNumber();
  m.L1_RoINumber  = f->getRoI();
  m.L1_ThrNumber  = f->getThrNumber();
  m.L1_SectorAddress  = f->getSectorAddress();

  if( f->getHemisphere() == Muon_ROI::Positive ){
    m.L1_isAside = true;
  }else{
    m.L1_isAside = false;
  }

  m.L1_isFirstCandidate    = f->isFirstCandidate();
  m.L1_isMoreCandInRoI     = f->isMoreCandInRoI();
  m.L1_isMoreCandInSector  = f->isMoreCandInSector();


  m.L1_pt  = f->pt();
  m.L1_eta = f->eta();
  m.L1_phi = f->phi();

}

void 
//setMuFast(RoIData_Muon& m, const MuonFeature* f, std::vector<TrigStatus> status)
setMuFast(RoIData_Muon& m, const MuonFeature* f, std::vector<std::string>& name, std::vector<int>& status)
{

  if( !f ) return;

  //original variables
  m.setMuFast( f->pt(), f->eta(), f->phi(), name, status);

  //extended variables
  m.MuFast_algoID = (int) f->algoId();

  m.MuFast_RoIID   = f->roiId();
  m.MuFast_sAddress = f->saddress();

  m.MuFast_radius  = f->radius();
  m.MuFast_dirPhi  = f->dir_phi();
  m.MuFast_zeta    = f->zeta();
  m.MuFast_dirZeta = f->dir_zeta();
  m.MuFast_beta    = f->beta();   

  m.MuFast_sp1R     = f->sp1_r();
  m.MuFast_sp1Z     = f->sp1_z();
  m.MuFast_sp1Slope = f->sp1_slope();
  m.MuFast_sp2R     = f->sp2_r();
  m.MuFast_sp2Z     = f->sp2_z();
  m.MuFast_sp2Slope = f->sp2_slope();
  m.MuFast_sp3R     = f->sp3_r();
  m.MuFast_sp3Z     = f->sp3_z();
  m.MuFast_sp3Slope = f->sp3_slope();


  m.MuFast_brRadius  = f->br_radius();
  m.MuFast_brSagitta = f->br_sagitta();

  m.MuFast_ECAlpha = f->ec_alpha();
  m.MuFast_ECBeta = f->ec_beta();

}
void 
setMuFastDetails(RoIData_Muon& m, const MuonFeatureDetails* f)
{

  if( !f ) return;

  //muFast MuonFeatureDetails variables
  m.MuFast_RoIThreshold = f->roi_threshold();
  m.MuFast_RoIEta = f->roi_eta();
  m.MuFast_RoIPhi = f->roi_phi();

  m.MuFast_RPC1x = f->rpc1_x();
  m.MuFast_RPC1y = f->rpc1_y();
  m.MuFast_RPC1z = f->rpc1_z();
  m.MuFast_RPC2x = f->rpc2_x();
  m.MuFast_RPC2y = f->rpc2_y();
  m.MuFast_RPC2z = f->rpc2_z();
  m.MuFast_RPC3x = f->rpc3_x();
  m.MuFast_RPC3y = f->rpc3_y();
  m.MuFast_RPC3z = f->rpc3_z();

  m.MuFast_TGCMid1Eta = f->tgc_Mid1_eta();
  m.MuFast_TGCMid1Phi = f->tgc_Mid1_phi();
  m.MuFast_TGCMid1R = f->tgc_Mid1_r();
  m.MuFast_TGCMid1Z = f->tgc_Mid1_z();
  m.MuFast_TGCMid2Eta = f->tgc_Mid2_eta();
  m.MuFast_TGCMid2Phi = f->tgc_Mid2_phi();
  m.MuFast_TGCMid2R = f->tgc_Mid2_r();
  m.MuFast_TGCMid2Z = f->tgc_Mid2_z();
  m.MuFast_TGCMidRhoChi2 = f->tgc_Mid_rho_chi2();
  m.MuFast_TGCMidRhoN = f->tgc_Mid_rho_N();
  m.MuFast_TGCMidPhiChi2 = f->tgc_Mid_phi_chi2();
  m.MuFast_TGCMidPhiN = f->tgc_Mid_phi_N();
  m.MuFast_TGCInnEta = f->tgc_Inn_eta();
  m.MuFast_TGCInnPhi = f->tgc_Inn_phi();
  m.MuFast_TGCInnR = f->tgc_Inn_r();
  m.MuFast_TGCInnZ = f->tgc_Inn_z();
  m.MuFast_TGCInnRhoStd = f->tgc_Inn_rho_std();
  m.MuFast_TGCInnRhoN = f->tgc_Inn_rho_N();
  m.MuFast_TGCInnPhiStd = f->tgc_Inn_phi_std();
  m.MuFast_TGCInnPhiN = f->tgc_Inn_phi_N();
  m.MuFast_TGCPt = f->tgc_PT();
  m.MuFast_ExtrInnerEta = f->extrInnerEta();
  m.MuFast_ExtrInnerPhi = f->extrInnerPhi();

  m.MuFast_MDTInnerFitChi = f->mdt_Inner_fit_chi();
  m.MuFast_MDTMiddleFitChi = f->mdt_Middle_fit_chi();
  m.MuFast_MDTOuterFitChi = f->mdt_Outer_fit_chi();

  m.MuFast_PhiMap = f->PhiMap();
}

void 
setMuComb(RoIData_Muon& m, const CombinedMuonFeature* f, std::vector<std::string>& name, std::vector<int>& status)
{

  if( !f ) return;

  //original variables
  m.setMuComb( f->pt(), f->eta(), f->phi(), name, status );

  //extended variables
  m.MuComb_sigmaPt = f->sigma_pt();

  //associated ID track variables
  const TrigInDetTrack *t = 0;
  const ElementLink<TrigInDetTrackCollection> eLink_trkCollection = f->IDTrackLink();
  if( eLink_trkCollection.isValid() ) t = *eLink_trkCollection;

  if(!t) return;

  m.MuComb_ID_algo       = t->algorithmId();
  m.MuComb_ID_pt         = t->param()->pT();
  m.MuComb_ID_ept        = t->param()->epT();
  m.MuComb_ID_eta        = t->param()->eta();
  m.MuComb_ID_eeta       = t->param()->eeta();
  m.MuComb_ID_phi0       = t->param()->phi0();
  m.MuComb_ID_ephi0      = t->param()->ephi0();
  m.MuComb_ID_z0         = t->param()->z0();
  m.MuComb_ID_ez0        = t->param()->ez0();
  m.MuComb_ID_a0         = t->param()->a0();
  m.MuComb_ID_ea0        = t->param()->ea0();
  m.MuComb_ID_chi2       = t->chi2();
  m.MuComb_ID_nPIX       = t->NPixelSpacePoints();
  m.MuComb_ID_nSCT       = t->NSCT_SpacePoints();
  m.MuComb_ID_hitPattern = t->HitPattern();
  m.MuComb_ID_nStrawHits = t->NStrawHits();
  m.MuComb_ID_nStraw     = t->NStraw();
  m.MuComb_ID_nStrawTime = t->NStrawTime();
  m.MuComb_ID_nTRHits    = t->NTRHits();


  //Retrieve ID track collections
  /*
   * this is filled to other classes
   *
  const DataHandle<TrigInDetTrackCollection> trackCollection;
  const DataHandle<TrigInDetTrackCollection> lastTrackCollection;
  StatusCode sc_idtrk = m_storeGate->retrieve(trackCollection,lastTrackCollection);
  if ( sc_idtrk.isFailure() ) {
    *m_log << MSG::VERBOSE << "Failed to retrieve ID tracks collections" << endreq;
    return StatusCode::SUCCESS;   
  }
	
  // Loop over tracks collections
  for (int iTrackColl=0; trackCollection != lastTrackCollection;++trackCollection, ++iTrackColl) {

    TrigInDetTrackCollection::const_iterator track= trackCollection->begin();
    TrigInDetTrackCollection::const_iterator lastTrack= trackCollection->end();

    // Loop over tracks
    for (; trk != lastTrack; trk++) {

      if( (*trk)==0 ) continue;

      //only SiTrack(1) and IDScan(2)
      int algo_trk = trk->algorithmId();
      if( algo_trk != 1 && algo_trk != 2 ) continue;

      // pt > 1GeV
      float pt_trk = trk->param()->pT();
      if( pt_trk < 1000. ) continue;

      // deltaR < 0.3
      float eta_trk = trk->param()->eta();
      float phi_trk = trk->param()->phi();
      float eta_trk = trk->param()->eta();
      float phi_trk = trk->param()->phi();
      float dR = calc_dR(eta, phi, eta_trk, phi_trk);
      if( dR > 0.3 ) continue;

      m.ID_algo       .push_back( trk->algorithmId() ) ;
      m.ID_pt         .push_back( trk->param()->pT() ) ;
      m.ID_ept        .push_back( trk->param()->epT() ) ;
      m.ID_eta        .push_back( trk->param()->eta() ) ;
      m.ID_eeta       .push_back( trk->param()->eeta() ) ;
      m.ID_phi0       .push_back( trk->param()->phi0() ) ;
      m.ID_ephi0      .push_back( trk->param()->ephi0() ) ;
      m.ID_z0         .push_back( trk->param()->z0() ) ;
      m.ID_ez0        .push_back( trk->param()->ez0() ) ;
      m.ID_a0         .push_back( trk->param()->a0() ) ;
      m.ID_ea0        .push_back( trk->param()->ea0() ) ;
      m.ID_chi2       .push_back( trk->chi2() ) ;
      m.ID_nPIX       .push_back( trk->NPixelSpacePoints() ) ;
      m.ID_nSCT       .push_back( trk->NSCT_SpacePoints() ) ;
      m.ID_hitPattern .push_back( trk->HitPattern() ) ;
      m.ID_nStrawHits .push_back( trk->NStrawHits() ) ;
      m.ID_nStraw     .push_back( trk->NStraw() ) ;
      m.ID_nStrawTime .push_back( trk->NStrawTime() ) ;
      m.ID_nTRHits    .push_back( trk->NTRHits() ) ;

    }//loop over tracks
  }//loop over collection
  */
}

void 
setMuIso(RoIData_Muon& m, const IsoMuonFeature* f, std::vector<std::string>& name, std::vector<int>& status)
{

  if( !f )return;

  //original variables
  m.setMuIso( f->pt(), f->eta(), f->phi(), name, status );

  //extended variables
  m.MuIso_EtICEC = f->getEtInnerConeEC();
  m.MuIso_EtOCEC = f->getEtOuterConeEC();
  m.MuIso_EtICHC = f->getEtInnerConeHC();
  m.MuIso_EtOCHC = f->getEtOuterConeHC();

  m.MuIso_nTracks = f->getNTracksCone();
  m.MuIso_sumPtTracks = f->getSumPtTracksCone();
  m.MuIso_ptMuTracks = f->getPtMuTracksCone();

  m.MuIso_RoIId  = f->getRoiIdMu();
  m.MuIso_LArWeight  = f->getLArWeight();
  m.MuIso_TileWeight  = f->getTileWeight();

}


void
setInDet(RoIData_Muon& m, const TrigInDetTrackCollection* f)
{

  if( !f )return;

  TrigInDetTrackCollection::const_iterator track= f->begin();
  TrigInDetTrackCollection::const_iterator lastTrack= f->end();

  for (; track != lastTrack; track++) {

    const TrigInDetTrack* trk = (*track);
    if( trk==0 ) continue;

    //only SiTrack(1) and IDScan(2)
    int algo_trk = trk->algorithmId();
    if( algo_trk != 1 && algo_trk != 2 ) continue;

    // pt > 1GeV
    float pt_trk = trk->param()->pT();
    if( pt_trk < 1000. ) continue;

    // deltaR < 0.3
    float eta = m.MuFast_eta;
    float phi = m.MuFast_phi;
    float eta_trk = trk->param()->eta();
    float phi_trk = trk->param()->phi0();
    float dR = calc_dR(eta, phi, eta_trk, phi_trk);
    if( dR > 0.3 ) continue;

    m.ID_algo       .push_back( trk->algorithmId() ) ;
    m.ID_pt         .push_back( trk->param()->pT() ) ;
    m.ID_ept        .push_back( trk->param()->epT() ) ;
    m.ID_eta        .push_back( trk->param()->eta() ) ;
    m.ID_eeta       .push_back( trk->param()->eeta() ) ;
    m.ID_phi0       .push_back( trk->param()->phi0() ) ;
    m.ID_ephi0      .push_back( trk->param()->ephi0() ) ;
    m.ID_z0         .push_back( trk->param()->z0() ) ;
    m.ID_ez0        .push_back( trk->param()->ez0() ) ;
    m.ID_a0         .push_back( trk->param()->a0() ) ;
    m.ID_ea0        .push_back( trk->param()->ea0() ) ;
    m.ID_chi2       .push_back( trk->chi2() ) ;
    m.ID_nPIX       .push_back( trk->NPixelSpacePoints() ) ;
    m.ID_nSCT       .push_back( trk->NSCT_SpacePoints() ) ;
    m.ID_hitPattern .push_back( trk->HitPattern() ) ;
    m.ID_nStrawHits .push_back( trk->NStrawHits() ) ;
    m.ID_nStraw     .push_back( trk->NStraw() ) ;
    m.ID_nStrawTime .push_back( trk->NStrawTime() ) ;
    m.ID_nTRHits    .push_back( trk->NTRHits() ) ;
    
  }//loop over tracks

}


//by Andrea Ventura
void 
setMuonEF(RoIData_Muon& m, const TrigMuonEFInfoContainer* c, std::vector<std::string>& name, std::vector<int>& status)
{
  //std::cout<<"inside setMuonEF"<<std::endl;
  if( !c ||
      (*c).size() == 0 )
    return;
  const TrigMuonEFInfo* muonInfo = (*c)[0];

  //extended info
  if (muonInfo->hasTrack()) {
    m.MuonEF_nSegments = muonInfo->NSegments();
    // nTracks can be obtained counting # of tracks in the RoI
    m.MuonEF_nTracks = 0;
    m.MuonEF_nMDT = muonInfo->NMdtHits();
    m.MuonEF_nRPC = muonInfo->NRpcHits();
    m.MuonEF_nTGC = muonInfo->NTgcHits();
    m.MuonEF_nCSC = muonInfo->NCscHits();

    const TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();

    for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin() ; TrackItr!=tc->end(); TrackItr++) {

      const TrigMuonEFInfoTrack* t = (*TrackItr);
      if( !t )continue;

      TrigMuonEFTrack* muonTrack = t->SpectrometerTrack();

      if (muonTrack) {
        m.MuonEF_nTracks++;
        m.MuonEFMS_pt = 1./fabs(muonTrack->iPt())/GeV ;
        m.MuonEFMS_eta = muonTrack->eta() ;
        m.MuonEFMS_phi = muonTrack->phi() ;
        m.MuonEFMS_d0 = muonTrack->d0() ;
        m.MuonEFMS_z0 = muonTrack->z0() ;
        m.MuonEFMS_chi2 = muonTrack->chi2() ;
        m.MuonEFMS_chi2prob = muonTrack->chi2prob() ;
        //m.MuonEFMS_nMDT = muonTrack->NMdtHitsPhi() + muonTrack->NMdtHitsEta() ; // not valid
        m.MuonEFMS_nRPCphi = muonTrack->NRpcHitsPhi() ;
        m.MuonEFMS_nRPCeta = muonTrack->NRpcHitsEta() ;
        m.MuonEFMS_nTGCphi = muonTrack->NTgcHitsPhi() ;
        m.MuonEFMS_nTGCeta = muonTrack->NTgcHitsEta() ;
        m.MuonEFMS_nCSCphi = muonTrack->NCscHitsPhi() ;
        m.MuonEFMS_nCSCeta = muonTrack->NCscHitsEta() ;
      }

      muonTrack = t->ExtrapolatedTrack();

      if (muonTrack) {
        m.MuonEFSA_pt = 1./fabs(muonTrack->iPt())/GeV ;
        m.MuonEFSA_eta = muonTrack->eta() ;
        m.MuonEFSA_phi = muonTrack->phi() ;
        m.MuonEFSA_d0 = muonTrack->d0() ;
        m.MuonEFSA_z0 = muonTrack->z0() ;
        m.MuonEFSA_chi2 = muonTrack->chi2() ;
        m.MuonEFSA_chi2prob = muonTrack->chi2prob() ;
      }

      TrigMuonEFCbTrack* muonCbTrack = t->CombinedTrack();

      if (muonCbTrack) {
        m.MuonEFCB_pt = 1./fabs(muonCbTrack->iPt())/GeV ;
        m.MuonEFCB_eta = muonCbTrack->eta() ;
        m.MuonEFCB_phi = muonCbTrack->phi() ;
        m.MuonEFCB_d0 = muonCbTrack->d0() ;
        m.MuonEFCB_z0 = muonCbTrack->z0() ;
        m.MuonEFCB_chi2 = muonCbTrack->chi2() ;
        m.MuonEFCB_chi2prob = muonCbTrack->chi2prob() ;
        m.MuonEFCB_matchChi2 = muonCbTrack->matchChi2() ;
        m.MuonEFCB_nSCT = muonCbTrack->NIdSctHits() ;
        m.MuonEFCB_nPIX = muonCbTrack->NIdPixelHits() ;
        m.MuonEFCB_nTRT = muonCbTrack->NTrtHits() ;
						
      }
    }

  }

  float ptmax=0.;
  float etamax=0.;
  float phimax=0.;

  // take CB (else SA or MS)  
  if( m.MuonEFCB_pt > 0 ){
    ptmax  = m.MuonEFCB_pt;
    etamax = m.MuonEFCB_eta;
    phimax = m.MuonEFCB_phi;
  }else if( m.MuonEFSA_pt > 0 ){
    ptmax  = m.MuonEFSA_pt;
    etamax = m.MuonEFSA_eta;
    phimax = m.MuonEFSA_phi;
  }else if( m.MuonEFMS_pt > 0 ){
    ptmax  = m.MuonEFMS_pt;
    etamax = m.MuonEFMS_eta;
    phimax = m.MuonEFMS_phi;
  }

  //original info
  m.setMuonEF( ptmax, etamax, phimax, name, status);

}

//by Andrea Ventura
void 
setMuonEF(RoIData_TileMu& m, const TrigMuonEFInfoContainer* c, std::vector<std::string>& name, std::vector<int>& status)
{
  if( !c ||
      (*c).size() == 0 )
    return;
  const TrigMuonEFInfo* muonInfo = (*c)[0];

  //extended info
  if (muonInfo->hasTrack()) {
    m.MuonEF_nSegments = muonInfo->NSegments();
    // nTracks can be obtained counting # of tracks in the RoI
    m.MuonEF_nTracks = 0;
    m.MuonEF_nMDT = muonInfo->NMdtHits();
    m.MuonEF_nRPC = muonInfo->NRpcHits();
    m.MuonEF_nTGC = muonInfo->NTgcHits();
    m.MuonEF_nCSC = muonInfo->NCscHits();

    const TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();

    for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin() ; TrackItr!=tc->end(); TrackItr++) {

      const TrigMuonEFInfoTrack* t = (*TrackItr);
      if( !t )continue;

      TrigMuonEFTrack* muonTrack = t->SpectrometerTrack();

      if (muonTrack) {
        m.MuonEF_nTracks++;
        m.MuonEFMS_pt = 1./fabs(muonTrack->iPt())/GeV ;
        m.MuonEFMS_eta = muonTrack->eta() ;
        m.MuonEFMS_phi = muonTrack->phi() ;
        m.MuonEFMS_d0 = muonTrack->d0() ;
        m.MuonEFMS_z0 = muonTrack->z0() ;
        m.MuonEFMS_chi2 = muonTrack->chi2() ;
        m.MuonEFMS_chi2prob = muonTrack->chi2prob() ;
        //m.MuonEFMS_nMDT = muonTrack->NMdtHitsPhi() + muonTrack->NMdtHitsEta() ; // not valid
        m.MuonEFMS_nRPCphi = muonTrack->NRpcHitsPhi() ;
        m.MuonEFMS_nRPCeta = muonTrack->NRpcHitsEta() ;
        m.MuonEFMS_nTGCphi = muonTrack->NTgcHitsPhi() ;
        m.MuonEFMS_nTGCeta = muonTrack->NTgcHitsEta() ;
        m.MuonEFMS_nCSCphi = muonTrack->NCscHitsPhi() ;
        m.MuonEFMS_nCSCeta = muonTrack->NCscHitsEta() ;
      }

      muonTrack = t->ExtrapolatedTrack();

      if (muonTrack) {
        m.MuonEFSA_pt = 1./fabs(muonTrack->iPt())/GeV ;
        m.MuonEFSA_eta = muonTrack->eta() ;
        m.MuonEFSA_phi = muonTrack->phi() ;
        m.MuonEFSA_d0 = muonTrack->d0() ;
        m.MuonEFSA_z0 = muonTrack->z0() ;
        m.MuonEFSA_chi2 = muonTrack->chi2() ;
        m.MuonEFSA_chi2prob = muonTrack->chi2prob() ;
      }

      TrigMuonEFCbTrack* muonCbTrack = t->CombinedTrack();

      if (muonCbTrack) {
        m.MuonEFCB_pt = 1./fabs(muonCbTrack->iPt())/GeV ;
        m.MuonEFCB_eta = muonCbTrack->eta() ;
        m.MuonEFCB_phi = muonCbTrack->phi() ;
        m.MuonEFCB_d0 = muonCbTrack->d0() ;
        m.MuonEFCB_z0 = muonCbTrack->z0() ;
        m.MuonEFCB_chi2 = muonCbTrack->chi2() ;
        m.MuonEFCB_chi2prob = muonCbTrack->chi2prob() ;
        m.MuonEFCB_matchChi2 = muonCbTrack->matchChi2() ;
        m.MuonEFCB_nSCT = muonCbTrack->NIdSctHits() ;
        m.MuonEFCB_nPIX = muonCbTrack->NIdPixelHits() ;
        m.MuonEFCB_nTRT = muonCbTrack->NTrtHits() ;
	
      }
    }

  }

  float ptmax=0.;
  float etamax=0.;
  float phimax=0.;

  // take CB (else SA or MS)  
  if( m.MuonEFCB_pt > 0 ){
    ptmax  = m.MuonEFCB_pt;
    etamax = m.MuonEFCB_eta;
    phimax = m.MuonEFCB_phi;
  }else if( m.MuonEFSA_pt > 0 ){
    ptmax  = m.MuonEFSA_pt;
    etamax = m.MuonEFSA_eta;
    phimax = m.MuonEFSA_phi;
  }else if( m.MuonEFMS_pt > 0 ){
    ptmax  = m.MuonEFMS_pt;
    etamax = m.MuonEFMS_eta;
    phimax = m.MuonEFMS_phi;
  }

  //original info
  m.setMuonEF( ptmax, etamax, phimax, name, status);

}

double
calc_dphi(double phi_i, double phi_j)
{
  double dphi = fabs(phi_i - phi_j);
  if( dphi > CLHEP::pi ) dphi = CLHEP::pi*2 - dphi;
  return dphi;
}

double
calc_dR(double eta_i, double phi_i, double eta_j, double phi_j)
{
  double deta = eta_i - eta_j;
  double dphi = calc_dphi(phi_i, phi_j);
  double dR   = sqrt( deta*deta + dphi*dphi );
  return dR;
}

void
setTrigStatus(const std::vector<TrigStatus>& v,
              std::vector<std::string>& vs,
              std::vector<int>&         vi)
{

  std::vector<TrigStatus>::const_iterator ite=v.end();

  for( std::vector<TrigStatus>::const_iterator it=v.begin(); it != ite; it++ ){
    vs.push_back( it->name() );
    vi.push_back( it->status() );
  }

}
