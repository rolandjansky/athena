/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  CombLinksDef.cxx
*/
#include "GaudiKernel/MsgStream.h"

#include "TriggerMenuAnalysis/CombLinksDef.h"
#include "TriggerMenuAnalysis/IndexFinderBase.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"

#include "TrkTrack/Track.h"
#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"

#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauDetailsContainer.h"

#include "JetEvent/JetCollection.h"

using namespace std;

std::map<ChainEntry::RoIType, CombLinksDef*> CombLinksDef::sCombLinksMap;
MsgStream* CombLinksDef::sLogger=0;
StoreGateSvc* CombLinksDef::sSGSvc=0;

bool CombLinksDef::addCombLinksDef(ChainEntry::RoIType type, CombLinksDef* x) {
  if (sCombLinksMap.find(type) == sCombLinksMap.end()) {
    sCombLinksMap[type] = x;
    return true;
  } else {
    return false;
  }
}

const CombLinksDef* CombLinksDef::getCombLinksDef(ChainEntry::RoIType type) {
  std::map<ChainEntry::RoIType, CombLinksDef*>::iterator p = 
    sCombLinksMap.find(type);
  if (p == sCombLinksMap.end()) {
    return 0;
  } else {
    return p->second;
  }
}

void CombLinksDef::buildKnownCombLinksDefs(StoreGateSvc* sgsvc, MsgStream* log) {
  sSGSvc = sgsvc;
  sLogger = log;
  CombLinksDef* x=0;

  //----------------------------------------------------------------
  // mu 
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_mu);
  x->addL1RoI("Muon_ROI");
  x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT", "forID");
  x->addFeature<MuonFeature, MuonFeatureContainer>("HLT");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Muon", 
							      "TrigSiTrack_Muon");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Muon", 
							      "TrigIDSCAN_Muon");
  x->addFeature<MuonFeatureDetails, MuonFeatureDetailsContainer>("HLT");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_Muon", 
							      "TrigL2SiTrackFinder_Muon");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_muonIso", 
							      "TrigL2SiTrackFinder_muonIso");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_muonIso", 
							      "TrigIDSCAN_muonIso");
  x->addFeature<CombinedMuonFeature, CombinedMuonFeatureContainer>("HLT");
  x->addFeature<IsoMuonFeature, IsoMuonFeatureContainer>("HLT");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_mu);
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrackCollection, Trk::Track>("HLT_InDetTrigTrackSlimmer_Muon_EFID", "InDetTrigTrackSlimmer_Muon_EFID");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_MuonEFInfo");
  x->addContFeature<TrigMuonEFIsolationContainer, TrigMuonEFIsolation>("HLT_MuonEFIsolation");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  // Tilemu
  x = new CombLinksDef(ChainEntry::kRoIType_L2_TileMu);
  x->addL1RoI("Muon_ROI");
  x->addFeature<TileMuFeature, TileMuFeatureContainer>("HLT");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Tile", 
							      "TrigSiTrack_Tile");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Tile", 
							      "TrigIDSCAN_Tile");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrack_Tile", 
							      "TriL2gSiTrack_Tile");
  x->addFeature<TileTrackMuFeature, TileTrackMuFeatureContainer>("HLT");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_TileMu);
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrigMuonEFContainer, TrigMuonEF>("HLT");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_MuonEFInfo");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_eMuonEFInfo", "MuGirl");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_MuTagIMO_EF", "MuTagIMO_EF");

  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  //----------------------------------------------------------------
  // e
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_e);
  x->addL1RoI("EmTau_ROI");
//   x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT", "initialRoI");
  x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT", "TrigT2CaloEgamma");
  x->addFeature<TrigEMCluster, TrigEMClusterContainer>("HLT_TrigT2CaloEgamma");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_eGamma", 
							      "TrigSiTrack_eGamma");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_eGamma", 
							      "TrigIDSCAN_eGamma");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_eGamma", 
							      "TrigL2SiTrackFinder_eGamma");
  x->addContFeature<TrigElectronContainer, TrigElectron>("HLT_L2IDCaloFex");
  x->addContFeature<TrigPhotonContainer, TrigPhoton>("HLT_L2PhotonFex");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_e);
  x->addL1RoI("EmTau_ROI");
  x->addContFeature<Rec::TrackParticleContainer, Rec::TrackParticle>("HLT", "HLT_InDetTrigParticleCreation_Electron_EFID");
  x->addContFeature<egammaContainer, egamma>("HLT", "egamma_Electrons");
  x->addContFeature<egammaContainer, egamma>("HLT", "egamma_Photons");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  //----------------------------------------------------------------
  // tau 
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_tau);
  x->addL1RoI("EmTau_ROI");
  x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT_initialRoI");
  x->addFeature<TrigTauCluster, TrigTauClusterContainer>("HLT_TrigT2CaloTau");
  x->addFeature<TrigTauClusterDetails, TrigTauClusterDetailsContainer>("HLT_TrigT2CaloTauDetails");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Tau", 
							      "TrigSiTrack_Tau");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Tau", 
							      "TrigIDSCAN_Tau");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_Tau",
							      "TrigL2SiTrackFinder_Tau");
  x->addFeature<TrigTauTracksInfo, TrigTauTracksInfoCollection>("HLT");
//   x->addContFeature<TrigVertexCollection, TrigVertex>("HLT_T2HistoPrmVtx");
  x->addFeature<TrigTau, TrigTauContainer>("HLT");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_tau);
  x->addContFeature<Analysis::TauJetContainer, Analysis::TauJet>("HLT");
  x->addL1RoI("EmTau_ROI");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  //----------------------------------------------------------------
  // j
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_j);
  x->addL1RoI("Jet_ROI");
  x->addFeature<TrigT2Jet, TrigT2JetContainer>("HLT_TrigT2CaloJet");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_j);
  //  x->addL1RoI("Jet_ROI");
  x->addContFeature<JetCollection, Jet>("HLT_TrigJetRec");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  //----------------------------------------------------------------
  // b 
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_b);
  x->addL1RoI("Jet_ROI");
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrigL2BjetContainer, TrigL2Bjet>("HLT_L2BjetFex", "");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Jet", 
							      "TrigSiTrack_Jet");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Jet", 
							      "TrigIDSCAN_Jet");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder", 
							      "TrigL2SiTrackFinder");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_Jet", 
							      "TrigL2SiTrackFinder_Jet");
  x->addContFeature<TrigVertexCollection, TrigVertex>("HLT_T2HistoPrmVtx");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_b);
  x->addL1RoI("Jet_ROI");
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrigEFBjetContainer, TrigEFBjet>("HLT_EFBjetFex", "");
  x->addContFeature<Rec::TrackParticleContainer, Rec::TrackParticle>("HLT_InDetTrigParticleCreation_Bjet_EFID");
  x->addContFeature<TrigVertexCollection, TrigVertex>("HLT_EFHistoPrmVtx");
  x->addContFeature<VxContainer, Trk::VxCandidate>("HLT_PrimVx", "");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  //----------------------------------------------------------------
  // xe 
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_xe);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_xe);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  //----------------------------------------------------------------
  // DiMu 
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_DiMu);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_DiMu);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

}

CombLinksDef::CombLinksDef(ChainEntry::RoIType type) :
  mRoIType(type) {
}

CombLinksDef::~CombLinksDef() {
  std::vector<IndexFinderBase*>::iterator p1;
  std::vector<IndexFinderBase*>::iterator p2;

  for (p1=mSingleFeatures.begin(); p1!=mSingleFeatures.end(); ++p1) {
    if (*p1) delete *p1;
    *p1 = 0;
  }
  for (p2=mContainerFeatures.begin(); p2!=mContainerFeatures.end(); ++p2) {
    if (*p2) delete *p2;
    *p2 = 0;
  }
}

CombLinks CombLinksDef::createCombLinks() {
  CombLinks x(static_cast<int>(mRoIType));
  return x;
}

std::vector<std::string> CombLinksDef::typeLabelNamesInt() const {
  std::vector<std::string> s;
  std::string tmp;
  std::vector<IndexFinderBase*>::iterator p;

  for (p=mSingleFeatures.begin(); p!=mSingleFeatures.end(); ++p) {
    tmp = (*p)->typeLabel();
    s.push_back(tmp);
  }
  copy(mL1RoIs.begin(), mL1RoIs.end(), back_inserter(s));
  return s;
}

std::vector<std::string> CombLinksDef::typeLabelNamesVectorOfInt() const {
  std::vector<std::string> s;
  std::string tmp;
  std::vector<IndexFinderBase*>::iterator p;

  for (p=mContainerFeatures.begin(); p!=mContainerFeatures.end(); ++p) {
    tmp = (*p)->typeLabel();
    s.push_back(tmp);
  }
  return s;
}

void CombLinksDef::addL1RoI(const std::string& roi_feature) {
  if (find(mL1RoIs.begin(), mL1RoIs.end(), roi_feature) == mL1RoIs.end()) {
    mL1RoIs.push_back(roi_feature);
  }
}

int CombLinksDef::fillCombLinks(CombLinks& links, 
				const Trig::Combination& comb,
				Trig::TrigDecisionTool* tdt,
				long RoIWord) const {
  const std::vector<std::string>& l1rois = getL1RoIs();
  std::vector<std::string>::const_iterator p_roi;
  for (p_roi=l1rois.begin(); p_roi!=l1rois.end(); ++p_roi) {
    const FeatureIndex* tmp = links.index(*p_roi);
    if(!tmp) links.addIndex( *p_roi, FeatureIndex(-1,0) );
  }

  std::vector<IndexFinderBase*>::iterator p1;
  std::vector<IndexFinderBase*>::iterator p2;
  std::string te_label="";

  links.setTELabelString("");
  for (p1=mSingleFeatures.begin(); p1!=mSingleFeatures.end(); ++p1) {
    (*p1)->setTrigDecisionTool(tdt);
    (*p1)->setLogger(sLogger);
    (*p1)->setL1RoIName( mL1RoIs[0] );
    (*p1)->setL1ROIWord( RoIWord );
    FeatureIndex x = (*p1)->findIndex(comb, te_label);
    links.prependTELabel(te_label);
    links.addIndex( (*p1)->typeLabel(), x);
  }
  for (p2=mContainerFeatures.begin(); p2!=mContainerFeatures.end(); ++p2) {
    (*p2)->setTrigDecisionTool(tdt);
    (*p2)->setLogger(sLogger);
    (*p2)->setL1RoIName( mL1RoIs[0] );
    (*p2)->setL1ROIWord( RoIWord );
    std::vector<FeatureIndex> x = (*p2)->findIndexes(comb, te_label);
    links.prependTELabel(te_label);
    links.addIndexVec( (*p2)->typeLabel(), x);
  }
  return 0;
}

