/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMonitoring/HLTEgammaFEXBaseTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

//for extrapolation
#include "CaloDetDescr/CaloDepthTool.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

#include "TH1F.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TTree.h"

HLTEgammaFEXBaseTool::HLTEgammaFEXBaseTool(const std::string & type, const std::string & name, const IInterface* parent)
  : IHLTMonTool(type,name,parent) {
  // In MeV:
  //declareProperty("L1EmClus", m_L1EmClus = 18.*CLHEP::GeV);
  //declareProperty("L1EmIsol", m_L1EmIsol = 3.*CLHEP::GeV);
  //declareProperty("L1HadCore", m_L1HadCore = 2.*CLHEP::GeV);
  //declareProperty("L1HadIsol", m_L1HadIsol = 2.*CLHEP::GeV);
  declareProperty("L1EmClus", m_L1EmClus = 3.*CLHEP::GeV);
  declareProperty("L1EmIsol", m_L1EmIsol = 99.*CLHEP::GeV);
  declareProperty("L1HadCore", m_L1HadCore = 99.*CLHEP::GeV);
  declareProperty("L1HadIsol", m_L1HadIsol = 99.*CLHEP::GeV);
  declareProperty("PassThroughL1", m_passThroughL1 = false);

  //declareProperty("L2Rcore", m_L2Rcore_cut = 0.895);
  //declareProperty("L2Rstrip", m_L2Rstrip_cut = 0.730);
  declareProperty("L2Rcore", m_L2Rcore_cut = 0.3);
  declareProperty("L2Rstrip", m_L2Rstrip_cut = 0.3);
  // In MeV:
  //declareProperty("L2Et", m_L2Et_cut = 20.*CLHEP::GeV);
  //declareProperty("L2Ehad", m_L2Ehad_cut = 3.8*CLHEP::GeV);
  declareProperty("L2Et", m_L2Et_cut = 3.*CLHEP::GeV);
  declareProperty("L2Ehad", m_L2Ehad_cut = 99.*CLHEP::GeV);

  declareProperty("L2IDPt", m_L2IDPt = 2.*CLHEP::GeV);
  declareProperty("L2IDEP", m_L2IDEP = 5.5);
  declareProperty("L2IDDeta", m_L2IDDeta = 0.2);
  declareProperty("L2IDDphi", m_L2IDDphi = 0.2);
/*
  declareProperty("L2IDPt", m_L2IDPt = 5.*CLHEP::GeV);
  declareProperty("L2IDEP", m_L2IDEP = 5.5);
  declareProperty("L2IDDeta", m_L2IDDeta = 0.018);
  declareProperty("L2IDDphi", m_L2IDDphi = 0.06);
*/

/*
  declareProperty("EFCaloEt", m_EFCaloEt = 22.*CLHEP::GeV);
  declareProperty("EFCaloDeta", m_EFCaloDeta = 0.1);
  declareProperty("EFCaloDphi", m_EFCaloDphi = 0.1);
*/
  declareProperty("EFCaloEt", m_EFCaloEt = 5.*CLHEP::GeV);
  declareProperty("EFCaloDeta", m_EFCaloDeta = 0.1);
  declareProperty("EFCaloDphi", m_EFCaloDphi = 0.1);

  declareProperty("L", m_L = 0.191*pow(10.0, 6.0));

  declareProperty("EFTrackeCalibFactor", m_EFTrackeCalibFactor = 1.0);
  declareProperty("EFTrackDeta", m_EFTrackDeta = 0.0036);
  declareProperty("EFTrackDphi", m_EFTrackDphi = 0.037);
  declareProperty("EFTrackEtaCut", m_EFTrackEtaCut = 1.37);
  declareProperty("EFTrackLowEoPHighEta", m_EFTrackLowEoPHighEta = 0.5);
  declareProperty("EFTrackHighEoPHighEta", m_EFTrackHighEoPHighEta = 3.86);
  declareProperty("EFTrackLowEoPLowEta", m_EFTrackLowEoPLowEta = 0.5);
  declareProperty("EFTrackHighEoPLowEta", m_EFTrackHighEoPLowEta = 2.08);

  declareProperty("lowEtCut", m_lowEtCut = 3.*CLHEP::GeV);
  declareProperty("maxEtaCut", m_maxEtaCut = 2.5);
  //declareProperty("lowEtCut", m_lowEtCut = 18.*CLHEP::GeV);
  //declareProperty("maxEtaCut", m_maxEtaCut = 2.5);

  declareProperty("doExtrapol", m_doExtrapol = true);

  declareProperty("TrigEMClusterKey", m_EMClusterKey = "HLT_TrigT2CaloEgamma");

  m_nTotal = 0;
  m_nL1 = 0;
  m_nL2 = 0;
  m_nL2IDScan = 0;
  m_nEFScanCalo = 0;
  m_nL2IDSi = 0;
  m_nEFSiCalo = 0;
  m_nEFScanTrack = 0;
  m_nEFSiTrack = 0;
}

HLTEgammaFEXBaseTool::~HLTEgammaFEXBaseTool() {
}

void HLTEgammaFEXBaseTool::FillEFTrack(const egamma *matchedEFTrk, std::map<std::string, std::vector<float> *> &vect) {
  const Trk::Perigee* aMeasPer = 0;
  if (matchedEFTrk) {
    if ( !(matchedEFTrk->trackParticle()) ) return;
    aMeasPer = matchedEFTrk->trackParticle()->perigee();
    if ( !aMeasPer  ) return;
    vect["Phi"]->push_back(aMeasPer->parameters()[Trk::phi]);
    vect["Eta"]->push_back(-std::log(tan(aMeasPer->parameters()[Trk::theta]/2.)));
    float pt2 = pow(aMeasPer->parameters()[Trk::px], 2.) + pow(aMeasPer->parameters()[Trk::py], 2.);
    vect["Pt"]->push_back(sqrt(pt2));
//    vect["D0"]->push_back(aMeasPer->parameters()[Trk::d0]);
//    vect["Z0"]->push_back(aMeasPer->parameters()[Trk::z0]);
    vect["Qual"]->push_back(matchedEFTrk->trackParticle()->fitQuality()->chiSquared());
    vect["Pixel"]->push_back(matchedEFTrk->trackParticle()->trackSummary()->get(Trk::numberOfPixelHits));
    vect["SCT"]->push_back(matchedEFTrk->trackParticle()->trackSummary()->get(Trk::numberOfSCTHits));
    vect["TRT"]->push_back(matchedEFTrk->trackParticle()->trackSummary()->get(Trk::numberOfTRTHits));
    vect["BLayer"]->push_back(matchedEFTrk->trackParticle()->trackSummary()->get(Trk::numberOfBLayerHits));
  }
}


void HLTEgammaFEXBaseTool::FillEFTrack(const xAOD::Electron *matchedEF, std::map<std::string, std::vector<float> *> &vect) {
  const xAOD::TrackParticle* matchedEFTrk = matchedEF->trackParticle();
  if (matchedEFTrk) {
    vect["Phi"]->push_back(matchedEFTrk->phi());
    vect["Eta"]->push_back(matchedEFTrk->eta());
    vect["Pt"] ->push_back(matchedEFTrk->pt()/1e3);
    vect["Qual"]  ->push_back(matchedEFTrk->chiSquared());
    uint8_t number=0;
    matchedEFTrk->summaryValue(number,xAOD::numberOfPixelHits);
    vect["Pixel"] ->push_back(number);
    number=0;
    matchedEFTrk->summaryValue(number,xAOD::numberOfSCTHits);
    vect["SCT"]   ->push_back(number);
    number=0;
    matchedEFTrk->summaryValue(number,xAOD::numberOfTRTHits);
    vect["TRT"]   ->push_back(number);
    number=0;
    matchedEFTrk->summaryValue(number,xAOD::numberOfBLayerHits);
    vect["BLayer"]   ->push_back(number);
/*
    vect["TRT"]   ->push_back(matchedEFTrk->trackSummary()->get(Trk::numberOfTRTHits));
    vect["BLayer"]->push_back(matchedEFTrk->trackSummary()->get(Trk::numberOfBLayerHits));
*/
 }
}

void HLTEgammaFEXBaseTool::FillLVL1(const EmTau_ROI &itEMTau, const std::string &grp) {
    hist("EmClus", grp)->Fill(itEMTau.getEMClus()/1.e3);
    hist("EmIsol", grp)->Fill(itEMTau.getEMIsol()/1.e3);
    hist("HadCore", grp)->Fill(itEMTau.getHadCore()/1.e3);
    hist("HadIsol", grp)->Fill(itEMTau.getHadIsol()/1.e3);
    hist("Eta", grp)->Fill(itEMTau.getEta());
    hist("Phi", grp)->Fill(itEMTau.getPhi());
}

void HLTEgammaFEXBaseTool::FillLVL1(const xAOD::EmTauRoI &itEMTau, const std::string &grp) {
    hist("EmClus", grp)->Fill(itEMTau.emClus()/1.e3);
    hist("EmIsol", grp)->Fill(itEMTau.emIsol()/1.e3);
    hist("HadCore", grp)->Fill(itEMTau.hadCore()/1.e3);
    hist("HadIsol", grp)->Fill(itEMTau.hadIsol()/1.e3);
    hist("Eta", grp)->Fill(itEMTau.eta());
    hist("Phi", grp)->Fill(itEMTau.phi());
}

void HLTEgammaFEXBaseTool::FillLVL2Calo(const TrigEMCluster *matchedL2, const std::string &grp) {
  if (matchedL2) {
      hist("Rcore", grp)->Fill(matchedL2->e237()/matchedL2->e277());
      hist("Rstrip", grp)->Fill((matchedL2->emaxs1()-matchedL2->e2tsts1())/(matchedL2->emaxs1()+matchedL2->e2tsts1()));
      hist("eta", grp)->Fill(matchedL2->eta());
      hist("phi", grp)->Fill(matchedL2->phi());
      hist("Et", grp)->Fill((matchedL2->energy()/cosh(matchedL2->eta()))/1.e3); 
      hist("HadEt", grp)->Fill((matchedL2->ehad1()/cosh(matchedL2->eta()))/1.e3);
  }
}

void HLTEgammaFEXBaseTool::FillLVL2Calo(const xAOD::TrigEMCluster *matchedL2, const std::string &grp) {
  if (matchedL2) {
      hist("Rcore", grp)->Fill(matchedL2->e237()/matchedL2->e277());
      hist("Rstrip", grp)->Fill((matchedL2->emaxs1()-matchedL2->e2tsts1())/(matchedL2->emaxs1()+matchedL2->e2tsts1()));
      hist("eta", grp)->Fill(matchedL2->eta());
      hist("phi", grp)->Fill(matchedL2->phi());
      hist("Et", grp)->Fill((matchedL2->energy()/cosh(matchedL2->eta()))/1.e3);
      hist("HadEt", grp)->Fill((matchedL2->ehad1()/cosh(matchedL2->eta()))/1.e3);
  }
}

void HLTEgammaFEXBaseTool::FillLVL2ID(float &Pt, float &Eta, float &Phi, float &HTOverAll, float &TRTHits, const TrigEMCluster *matchedL2, const std::string &grp) {
  if (matchedL2) {
    hist("Pt", grp)->Fill(fabsf(Pt)/1.e3);
    hist("Deta", grp)->Fill(fabsf(Eta - matchedL2->eta()));
    hist("Dphi", grp)->Fill(delta(Phi - matchedL2->phi()));
    hist("HTOverAll", grp)->Fill(HTOverAll);
    hist("TRTHits", grp)->Fill(TRTHits);
    hist("EP", grp)->Fill(fabsf((matchedL2->energy()/cosh(matchedL2->eta()))/Pt));
  }
}

void HLTEgammaFEXBaseTool::FillLVL2ID(float &Pt, float &Eta, float &Phi, float &HTOverAll, float &TRTHits, const xAOD::TrigEMCluster *matchedL2, const std::string &grp) {
  if (matchedL2) {
    hist("Pt", grp)->Fill(fabsf(Pt)/1.e3);
    hist("Deta", grp)->Fill(fabsf(Eta - matchedL2->eta()));
    hist("Dphi", grp)->Fill(delta(Phi - matchedL2->phi()));
    hist("HTOverAll", grp)->Fill(HTOverAll);
    hist("TRTHits", grp)->Fill(TRTHits);
    hist("EP", grp)->Fill(fabsf((matchedL2->energy()/cosh(matchedL2->eta()))/Pt));
  }
}

void HLTEgammaFEXBaseTool::FillEFCalo(const CaloCluster *matchedEFCalo, float DetaEFCalo, float DphiEFCalo, const std::string &grp) {
  if (matchedEFCalo) {
    hist("Et", grp)->Fill(matchedEFCalo->et()/1.e3);
    hist("Dphi", grp)->Fill(DphiEFCalo);
    hist("Deta", grp)->Fill(DetaEFCalo);
  }
}

void HLTEgammaFEXBaseTool::FillEFCalo(const xAOD::CaloCluster *matchedEFCalo, float DetaEFCalo, float DphiEFCalo, const std::string &grp) {
  if (matchedEFCalo) {
    hist("Et", grp)->Fill(matchedEFCalo->et()/1.e3);
    hist("Dphi", grp)->Fill(DphiEFCalo);
    hist("Deta", grp)->Fill(DetaEFCalo);
  }
}

void HLTEgammaFEXBaseTool::extrapolateToFirstSurface(const Rec::TrackParticle* m_track, double &caloeta, double &calophi ) {
  //bool got_a_track = false;

  if (!m_track) {
    (*m_log) << MSG::DEBUG << "m_track is NULL!!" << endreq;
    caloeta = -1111.;
    calophi = -1111.;
    return;
  }
  
//  HepGeom::Point3D<double> *pt_calo_ctb = new HepGeom::Point3D<double>;
//  HepGeom::Point3D<double> *pt_calo_local = new HepGeom::Point3D<double>;

  double trketa = -std::log(tan(((m_track)->perigee())->parameters()[Trk::theta]/2.));

  double distbar = 0.;
  double distec = 0.;
  double offset = 0.;
  bool result = false;
  CaloCell_ID::CaloSample sample;
  distbar = m_calodepth->deta(CaloCell_ID::EMB1,trketa);
  distec = m_calodepth->deta(CaloCell_ID::EME1,trketa);
  if (distbar < 0 ) sample = CaloCell_ID::EMB1;
  else if (distec < 0 ) sample = CaloCell_ID::EME1;
  else if ( distbar < distec) sample = CaloCell_ID::EMB1;
  else sample = CaloCell_ID::EME1;

  (*m_log) << MSG::DEBUG << "Calculating track seen by Calo ..." << endreq;
  const Trk::TrackParameters * SeenByCalo = m_toCalo->extrapolate(*(m_track->originalTrack()),sample,offset) ;
  result =  ( SeenByCalo != NULL );

  if (result) {
    //got_a_track=true;
    caloeta = SeenByCalo->eta();
    calophi = SeenByCalo->momentum().phi();
  } else {
    caloeta = -1111.;
    calophi = -1111.;
  }
  
  //delete pt_calo_ctb;
  //delete pt_calo_local;
}

bool HLTEgammaFEXBaseTool::runTrack(const DataHandle<TrigInDetTrackCollection> lvl2t_first, const DataHandle<TrigInDetTrackCollection> lvl2t_last, float l2eta, float l2phi, \
                                    float &Pt, float &Eta, float &Phi, float &HTOverAll, float &TRTHits, TrigInDetTrack::AlgoId m) {
  bool ret = false;
  
  float DetaChosen = 0.2, DphiChosen = 0.2;
  double etac, phic;

  const DataHandle<TrigInDetTrackCollection> lvl2t_init = lvl2t_first;

  (*m_log) << MSG::DEBUG << "On runTrack()." << endreq;
  for(; lvl2t_init != lvl2t_last; lvl2t_init++) {
    TrigInDetTrackCollection::const_iterator lvl2t_it = lvl2t_init->begin();
    TrigInDetTrackCollection::const_iterator lvl2t_end = lvl2t_init->end();

    for( ; lvl2t_it != lvl2t_end; lvl2t_it++) {
      (*m_log) << MSG::DEBUG << "Preparing to extrapolate." << endreq;

      TrigInDetTrackHelper trackHelper((*lvl2t_it)->param());
      trackHelper.extrapolate(1470.0*CLHEP::mm,3800.0*CLHEP::mm, phic, etac);

      float Deta = fabsf(l2eta - (float) etac);
      float Dphi = delta(l2phi - (float) phic);

      if ( (Deta < DetaChosen) && (Dphi < DphiChosen) && ((*lvl2t_it)->algorithmId() == m) ) {
        DetaChosen = Deta;
        DphiChosen = Dphi;
        Pt = (*lvl2t_it)->param()->pT();
        Eta = (float) etac;
        Phi = (float) phic;
//        A0 = (*lvl2t_it)->param()->a0();
//        Z0 = (*lvl2t_it)->param()->z0();
	if ( (*lvl2t_it)->NStrawHits() == 0 )
	  HTOverAll = 0;
	else
	  HTOverAll = ( (*lvl2t_it)-> NTRHits() )/( (*lvl2t_it)->NStrawHits() );
	TRTHits = (*lvl2t_it)->NStrawHits();
        ret = true;
      }
    } // for all tracks
  } // for all track collections
  return ret;
}

float HLTEgammaFEXBaseTool::delta(float a) {
  return fabsf(TMath::Pi() - fabsf(TMath::Pi() - fabsf(a)));
}

void HLTEgammaFEXBaseTool::PrintTable(bool withcut) {
if ( withcut ) {
  (*m_log) << MSG::INFO << "BEGIN Cuts" << endreq;
  (*m_log) << MSG::INFO << "L1EmClus              " << m_L1EmClus/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "L1EmIsol              " << m_L1EmIsol/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "L1HadCore             " << m_L1HadCore/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "L1HadIsol             " << m_L1HadIsol/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "PassThroughL1         " << m_passThroughL1 << endreq;
  (*m_log) << MSG::INFO << "L2CaloRcore           " << m_L2Rcore_cut << endreq;
  (*m_log) << MSG::INFO << "L2CaloRstrip          " << m_L2Rstrip_cut << endreq;
  (*m_log) << MSG::INFO << "L2CaloEt              " << m_L2Et_cut/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "L2CaloHadEt           " << m_L2Ehad_cut/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "L2IDScanPt            " << m_L2IDPt/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "L2IDScanDeta          " << m_L2IDDeta << endreq;
  (*m_log) << MSG::INFO << "L2IDScanDphi          " << m_L2IDDphi << endreq;
  (*m_log) << MSG::INFO << "L2IDScanEP            " << m_L2IDEP << endreq;
  (*m_log) << MSG::INFO << "L2IDSiPt              " << m_L2IDPt/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "L2IDSiDeta            " << m_L2IDDeta << endreq;
  (*m_log) << MSG::INFO << "L2IDSiDphi            " << m_L2IDDphi << endreq;
  (*m_log) << MSG::INFO << "L2IDSiEP              " << m_L2IDEP << endreq;
  (*m_log) << MSG::INFO << "EFScanCaloEt          " << m_EFCaloEt/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "EFScanCaloDphi        " << m_EFCaloDphi << endreq;
  (*m_log) << MSG::INFO << "EFScanCaloDeta        " << m_EFCaloDeta << endreq;
  (*m_log) << MSG::INFO << "EFSiCaloEt            " << m_EFCaloEt/CLHEP::GeV << endreq;
  (*m_log) << MSG::INFO << "EFSiCaloDphi          " << m_EFCaloDphi << endreq;
  (*m_log) << MSG::INFO << "EFSiCaloDeta          " << m_EFCaloDeta << endreq;
  // BELOW: These must be updated!
  (*m_log) << MSG::INFO << "EFTrackeCalibFactor   " << m_EFTrackeCalibFactor << endreq;
  (*m_log) << MSG::INFO << "EFTrackDeta           " << m_EFTrackDeta << endreq;
  (*m_log) << MSG::INFO << "EFTrackDphi           " << m_EFTrackDphi << endreq;
  (*m_log) << MSG::INFO << "EFTrackEtaCut         " << m_EFTrackEtaCut << endreq;
  (*m_log) << MSG::INFO << "EFTrackLowEoPHighEta  " << m_EFTrackLowEoPHighEta << endreq;
  (*m_log) << MSG::INFO << "EFTrackHighEoPHighEta " << m_EFTrackHighEoPHighEta << endreq;
  (*m_log) << MSG::INFO << "EFTrackLowEoPLowEta   " << m_EFTrackLowEoPLowEta << endreq;
  (*m_log) << MSG::INFO << "EFTrackHighEoPLowEta  " << m_EFTrackHighEoPLowEta << endreq;
  (*m_log) << MSG::INFO << "END Cuts" << endreq;
}

  (*m_log) << MSG::INFO << "BEGIN Rates" << endreq;
  (*m_log) << MSG::INFO << "L                     " << m_L << endreq;
  (*m_log) << MSG::INFO << "Total                 " << m_nTotal << endreq;
  (*m_log) << MSG::INFO << "L1                    " << m_nL1 << endreq;
  (*m_log) << MSG::INFO << "L2                    " << m_nL2 << endreq;
  (*m_log) << MSG::INFO << "L2IDScan              " << m_nL2IDScan << endreq;
  (*m_log) << MSG::INFO << "L2IDSi                " << m_nL2IDSi << endreq;
  (*m_log) << MSG::INFO << "EFScanCalo            " << m_nEFScanCalo << endreq;
  (*m_log) << MSG::INFO << "EFSiCalo              " << m_nEFSiCalo << endreq;
  (*m_log) << MSG::INFO << "EFScanTrack           " << m_nEFScanTrack << endreq;
  (*m_log) << MSG::INFO << "EFSiTrack             " << m_nEFSiTrack << endreq;
  (*m_log) << MSG::INFO << "END Rates" << endreq;
}

void HLTEgammaFEXBaseTool::FillHisto(const std::string & name, std::map<std::string, std::vector<float> *> &vect, const std::string &grp) {
  for (std::vector<float>::const_iterator i = vect[name]->begin();
       i != vect[name]->end();
       i++) {
    hist(name, grp)->Fill(*i);
  }
  vect[name]->clear();
}

void HLTEgammaFEXBaseTool::FillEFIDHistos(std::map<std::string, std::vector<float> *> &vect, const std::string &grp) {
  FillHisto("Phi", vect, grp);
  FillHisto("Eta", vect, grp);
  FillHisto("Pt", vect, grp);
  FillHisto("Qual", vect, grp);
  FillHisto("Pixel", vect, grp);
  FillHisto("SCT", vect, grp);
  FillHisto("TRT", vect, grp);
  FillHisto("BLayer", vect, grp);
}

void HLTEgammaFEXBaseTool::scaleError(float factor, TH1 *h) {
  if (h) {
    h->Scale(factor);
    for (int i = 0; i <= h->GetNbinsX(); i++) {
      if (h->GetBinContent(i) != 0)
        h->SetBinError(i, factor*sqrt(h->GetBinContent(i)));
      else
        h->SetBinError(i, 0.);
    }
  }
}

void HLTEgammaFEXBaseTool::divide(TH1 *num, TH1 *den, TH1 *quo) {
  quo->Divide(num, den, 1., 1., "b");
}

