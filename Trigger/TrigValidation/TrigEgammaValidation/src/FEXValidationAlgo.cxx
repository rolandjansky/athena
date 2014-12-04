/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/FEXValidationAlgo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "GeneratorObjects/McEventCollection.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"


//Conflict in public requirements, egammaPID resides in AsgTools now!
//#include "egammaEvent/egammaContainer.h"
//#include "egammaEvent/egamma.h"
//#include "egammaEvent/EMTrackMatch.h"

#include "TH1F.h"
#include "TProfile2D.h"
#include "TMath.h"

#include <string>
#include <vector>
#include <map>

FEXValidationAlgo::FEXValidationAlgo(const std::string & name, ISvcLocator * pSvcLocator, const std::string &file)
  : IValidationAlgo(name, pSvcLocator, file),
    m_nTotal(0), m_nL1(0), m_nL2(0), m_nL2IDScan(0), m_nL2IDSi(0), m_nEFScanCalo(0), m_nEFSiCalo(0), m_nEFScanTrack(0), m_nEFSiTrack(0)
{
  // e22i_tight
  
  // In CLHEP::MeV:
  declareProperty("L1EmClus", m_L1EmClus = 18.*CLHEP::GeV);
  declareProperty("L1EmIsol", m_L1EmIsol = 3.*CLHEP::GeV);
  declareProperty("L1HadCore", m_L1HadCore = 2.*CLHEP::GeV);
  declareProperty("L1HadIsol", m_L1HadIsol = 1.*CLHEP::GeV);

  declareProperty("L2Rcore", m_L2Rcore_cut = 0.895);
  declareProperty("L2Rstrip", m_L2Rstrip_cut = 0.730);
  // In CLHEP::MeV:
  declareProperty("L2Et", m_L2Et_cut = 22.*CLHEP::GeV);
  declareProperty("L2Ehad", m_L2Ehad_cut = 3*CLHEP::GeV);

  declareProperty("L2IDPt", m_L2IDPt = 3.*CLHEP::GeV);
  declareProperty("L2IDEP", m_L2IDEP = 100);
  declareProperty("L2IDDeta", m_L2IDDeta = 0.023);
  declareProperty("L2IDDphi", m_L2IDDphi = 0.091);

  declareProperty("EFCaloEt", m_EFCaloEt = 20.1*CLHEP::GeV);
  declareProperty("EFCaloDeta", m_EFCaloDeta = 0.05);
  declareProperty("EFCaloDphi", m_EFCaloDphi = 0.1);

  declareProperty("EFTrackeCalibFactor", m_EFTrackeCalibFactor = 1.0);
  declareProperty("EFTrackDeta", m_EFTrackDeta = 0.05);
  declareProperty("EFTrackDphi", m_EFTrackDphi = 0.1);
  declareProperty("EFTrackEtaCut", m_EFTrackEtaCut = 1.37);
  declareProperty("EFTrackLowEoPHighEta", m_EFTrackLowEoPHighEta = 0);
  declareProperty("EFTrackHighEoPHighEta", m_EFTrackHighEoPHighEta = 999);
  declareProperty("EFTrackLowEoPLowEta", m_EFTrackLowEoPLowEta = 0);
  declareProperty("EFTrackHighEoPLowEta", m_EFTrackHighEoPLowEta = 999);

  declareProperty("sL", m_sL = 0.191e6);

  declareProperty("lowEtCut", m_lowEtCut = 18.*CLHEP::GeV);
  declareProperty("maxEtaCut", m_maxEtaCut = 2.5);
}

StatusCode FEXValidationAlgo::prepare() {
  StatusCode sc;

  addTree(new TNtuple("EffTable", "Total Efficiencies", "sL:Total:L1:L2:L2IDScan:L2IDSi:EFScanCalo:EFSiCalo:EFScanTrack:EFSiTrack"));
  addTree(new TNtuple("CutTable", "Cuts", "L1EmClus:L1EmIsol:L1HadCore:L1HadIsol:L2Rcore:L2Rstrip:L2Et:L2Ehad:L2IDPt:L2IDDeta:L2IDDphi:L2IDEP:EFCaloEt:EFCaloDphi:EFCaloDeta:EFTrackeCalibFactor:EFTrackDeta:EFTrackDphi:EFTrackEtaCut:EFTrackLowEoPHighEta:EFTrackHighEoPHighEta:EFTrackLowEoPLowEta:EFTrackHighEoPLowEta"));

  addHistogram(new TH1F("L1EmClus", "Level 1 EM Cluster Energy ; L1 EM Cluster Energy [GeV] ; Probability", 80, 0., 80.));
  addHistogram(new TH1F("L1EmIsol", "Level 1 EM Isolation Energy ; L1 EM Isolation Energy [GeV] ; Probability", 15, 0., 15.));
  addHistogram(new TH1F("L1HadCore", "Level 1 Hadronic Core Energy ; L1 Had Core Energy [GeV] ; Probability", 15, 0., 15.));
  addHistogram(new TH1F("L1HadIsol", "Level 1 Hadronic Isolation Energy ; L1 Had Isolation Energy [GeV] ; Probability", 15, 0., 15.));

  addHistogram(new TH1F("L2CaloRcore", "R Core from L2 Fex ; Core Ratio ; Probability", 100, 0., 1.));
  addHistogram(new TH1F("L2CaloRstrip", "R Strip from L2 Fex ; Strip Ratio ; Probability", 100, 0., 1.));
  addHistogram(new TH1F("L2CaloEt", "Level 2 EM Fex E_{T} ; E_{T} [GeV] ; Probability", 160, 0., 80.));
  addHistogram(new TH1F("L2CaloHadEt", "Level 2 Hadronic Fex E_{T} ; E_{T} [GeV] ; Probability", 20, 0., 2.));
  addHistogram(new TH1F("L2Caloeta", "Level 2 Fex \\eta ; \\eta ; Probability", 100, 0., 2.5));
  addHistogram(new TH1F("L2Calophi", "Level 2 Fex \\phi ; \\phi ; Probability", 128, 0., 3.2));

  addHistogram(new TH1F("L2IDScanPt", "Level 2 ID (ID Scan Match) p_{T} ; p_{T} [GeV] ; Probability", 80, 0., 80.));
  addHistogram(new TH1F("L2IDScanDphi", "Level 2 ID (ID Scan Match) \\Delta\\phi ; \\Delta\\phi [rad] ; Probability", 128, 0., m_L2IDDphi*4));
  addHistogram(new TH1F("L2IDScanDeta", "Level 2 ID (ID Scan Match) \\Delta\\eta ; \\Delta\\eta ; Probability", 128, 0., m_L2IDDeta*4));
  addHistogram(new TH1F("L2IDScanA0", "Level 2 ID (ID Scan Match) A_{0} ; A_{0} ; Probability", 128, -3., 3.));
  addHistogram(new TH1F("L2IDScanZ0", "Level 2 ID (ID Scan Match) Z_{0} ; Z_{0} ; Probability", 128, -100., 100.));
  addHistogram(new TH1F("L2IDScanEP", "Level 2 ID (ID Scan Match) E_{T}/p_{T} ; E_{T}/p_{T} ; Probability", 100, 0., 6.));

  addHistogram(new TH1F("L2IDSiPt", "Level 2 ID (SiTrack Match) p_{T} ; p_{T} [GeV] ; Probability", 80, 0., 80.));
  addHistogram(new TH1F("L2IDSiDphi", "Level 2 ID (SiTrack Match) \\Delta\\phi ; \\Delta\\phi [rad] ; Probability", 128, 0., m_L2IDDphi*4));
  addHistogram(new TH1F("L2IDSiDeta", "Level 2 ID (SiTrack Match) \\Delta\\eta ; \\Delta\\eta ; Probability", 128, 0., m_L2IDDeta*4));
  addHistogram(new TH1F("L2IDSiA0", "Level 2 ID (SiTrack Match) A_{0} ; A_{0} ; Probability", 128, -3., 3.));
  addHistogram(new TH1F("L2IDSiZ0", "Level 2 ID (SiTrack Match) Z_{0} ; Z_{0} ; Probability", 128, -100., 100.));
  addHistogram(new TH1F("L2IDSiEP", "Level 2 ID (SiTrack Match) E_{T}/p_{T} ; E_{T}/p_{T} ; Probability", 100, 0., 6.));

  addHistogram(new TH1F("EFScanCaloEt", "E_{T} at EF Calo (and L2 ID Scan Match) ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("EFScanCaloDphi", "\\Delta\\phi at EF Calo (and L2 ID Scan Match) ; \\Delta\\phi [rad] ; Count", 128, 0., 2*m_EFCaloDphi));
  addHistogram(new TH1F("EFScanCaloDeta", "\\Delta\\eta at EF Calo (and L2 ID Scan Match) ; \\Delta\\eta ; Count", 128, 0., 2*m_EFCaloDeta));

  addHistogram(new TH1F("EFSiCaloEt", "E_{T} at EF Calo (and L2 ID SiTrack Match) ; E_{T} [GeV] ; Count", 80, 0, 80.));
  addHistogram(new TH1F("EFSiCaloDphi", "\\Delta\\phi at EF Calo (and L2 ID SiTrack Match) ; \\Delta\\phi [rad] ; Count", 128, 0., 2*m_EFCaloDphi));
  addHistogram(new TH1F("EFSiCaloDeta", "\\Delta\\eta at EF Calo (and L2 ID SiTrack Match) ; \\Delta\\eta ; Count", 128, 0., 2*m_EFCaloDeta));

  addHistogram(new TH1F("EFScanTrackPhi", "\\phi for EF Track (after L2 ID Scan Match) ; \\phi ; Probability", 100, 0, 3.2));
  addHistogram(new TH1F("EFScanTrackEta", "\\eta for EF Track (after L2 ID Scan Match) ; \\eta ; Probability", 100, 0, 3.2));
  addHistogram(new TH1F("EFScanTrackPt", "P_T for EF Track (after L2 ID Scan Match) ; p_{T} [GeV] ; Probability", 100, 0, 80.));

  addHistogram(new TH1F("EFScanTrackD0", "d0 for EF Track (after L2 ID Scan Match) ; d0 [mm] ; Count", 100, -0.5, 0.5));
  addHistogram(new TH1F("EFScanTrackZ0", "z0 for EF Track (after L2 ID Scan Match) ; z0 [mm] ; Count", 100, -200, 200));

  addHistogram(new TH1F("EFSiTrackPhi", "\\phi for EF Track (after L2 ID SiTrack Match) ; \\phi [rad] ; Probability", 100, 0, 3.2));
  addHistogram(new TH1F("EFSiTrackEta", "\\eta for EF Track (after L2 ID SiTrack Match) ; \\eta ; Probability", 100, 0, 3.2));
  addHistogram(new TH1F("EFSiTrackPt", "P_T for EF Track (after L2 ID SiTrack Match) ; p_{T} [GeV] ; Probability", 100, 0, 80.));

  addHistogram(new TH1F("EFSiTrackD0", "d0 for EF Track (after L2 ID SiTrack Match) ; d0 [mm] ; Probability", 100, -0.5, 0.5));
  addHistogram(new TH1F("EFSiTrackZ0", "z0 for EF Track (after L2 ID SiTrack Match) ; z0 [mm] ; Probability", 100, -200, 200));

  vect["EFScanTrackPhi"]  = new std::vector<float>;
  vect["EFScanTrackEta"]  = new std::vector<float>;
  vect["EFScanTrackPt"]  = new std::vector<float>;
  vect["EFScanTrackD0"]  = new std::vector<float>;
  vect["EFScanTrackZ0"]  = new std::vector<float>;
  
  vect["EFSiTrackPhi"]  = new std::vector<float>;
  vect["EFSiTrackEta"]  = new std::vector<float>;
  vect["EFSiTrackPt"]  = new std::vector<float>;
  vect["EFSiTrackD0"]  = new std::vector<float>;
  vect["EFSiTrackZ0"]  = new std::vector<float>;

  return StatusCode::SUCCESS;
}

float FEXValidationAlgo::delta(float a) {
  return fabsf(TMath::Pi() - fabsf(TMath::Pi() - fabsf(a)));
}

void FEXValidationAlgo::scaleError(float factor, TH1 *h) {
  if (h) {
    h->Sumw2();
    h->Scale(factor);
    /*
    for (int i = 0; i <= h->GetNbinsX(); i++) {
      if (h->GetBinContent(i) != 0)
        h->SetBinError(i, factor*sqrt(h->GetBinContent(i)));
      else
        h->SetBinError(i, 0.);
    }
    */
  }
}

bool FEXValidationAlgo::runTrack(const DataHandle<TrigInDetTrackCollection> lvl2t_first, const DataHandle<TrigInDetTrackCollection> lvl2t_last, float l2eta, float l2phi,\
                                 float &Pt, float &Eta, float &Phi, float &A0, float &Z0, TrigInDetTrack::AlgoId m)
{
  float DetaChosenID = 0.2, DphiChosenID = 0.2;
  double etac, phic;
  bool found = false;
  
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
       
       if ( (Deta < DetaChosenID) && (Dphi < DphiChosenID) && ((*lvl2t_it)->algorithmId() == m) ) {
         DetaChosenID = Deta;
         DphiChosenID = Dphi;
         Pt = (*lvl2t_it)->param()->pT();
         Eta = (float) etac;
         Phi = (float) phic;
         A0 = (*lvl2t_it)->param()->a0();
         Z0 = (*lvl2t_it)->param()->z0();
	 found = true;
      } // End if
    } // for all tracks
  } // for all track collections
  return found;
}

void FEXValidationAlgo::extrapolateToFirstSurface(const Rec::TrackParticle* m_track, double &caloeta, double &calophi ) {
  //bool got_a_track = false;

  if (!m_track) {
    (*m_log) << MSG::INFO << "m_track is NULL!!" << endreq;
    caloeta = -1111.;
    calophi = -1111.;
    return;
  }

  //HepGeom::Point3D<double> *pt_calo_ctb = new HepGeom::Point3D<double>;
  //HepGeom::Point3D<double> *pt_calo_local = new HepGeom::Point3D<double>;

  double trketa = -std::log(tan(((m_track)->perigee())->parameters()[Trk::theta]/2.));

  double distbar = 0.;
  double distec = 0.;
  double offset = 0.;
//  bool result = false;
//  Removing extrapolation
//  See TrigL2ElectronFex for new extrapolation
//  CaloCell_ID::CaloSample sample;
  caloeta = -1111.;
  calophi = -1111.;
  //delete pt_calo_ctb;
  //delete pt_calo_local;
}

void FEXValidationAlgo::FillLVL1(LVL1_ROI::emtaus_type::const_iterator itEMTau) {
  hist1("L1EmClus")->Fill((itEMTau)->getEMClus()/1.e3);
  hist1("L1EmIsol")->Fill((itEMTau)->getEMIsol()/1.e3);
  hist1("L1HadCore")->Fill((itEMTau)->getHadCore()/1.e3);
  hist1("L1HadIsol")->Fill((itEMTau)->getHadIsol()/1.e3);
}

void FEXValidationAlgo::FillLVL2Calo(TrigEMCluster *matchedL2) {
  if (matchedL2) {
    hist1("L2CaloRcore")->Fill(matchedL2->e237()/matchedL2->e277());
    hist1("L2CaloRstrip")->Fill((matchedL2->emaxs1()-matchedL2->e2tsts1())/(matchedL2->emaxs1()+matchedL2->e2tsts1()));
    hist1("L2Caloeta")->Fill(fabsf(matchedL2->eta()));
    hist1("L2Calophi")->Fill(fabsf(matchedL2->phi()));
    hist1("L2CaloEt")->Fill((matchedL2->energy()/cosh(matchedL2->eta()))/1.e3); 
    hist1("L2CaloHadEt")->Fill((matchedL2->ehad1()/cosh(matchedL2->eta()))/1.e3);
  }
}

void FEXValidationAlgo::FillLVL2ID(float Pt, float Eta, float Phi, float A0, float Z0, TrigEMCluster *matchedL2, const std::string &ScanSi) {
  if (matchedL2) {
    hist1("L2ID"+ScanSi+"Pt")->Fill(fabsf(Pt)/1.e3);
    hist1("L2ID"+ScanSi+"Deta")->Fill(fabsf(Eta - matchedL2->eta()));
    hist1("L2ID"+ScanSi+"Dphi")->Fill(delta(Phi - matchedL2->phi()));
    hist1("L2ID"+ScanSi+"A0")->Fill(A0);
    hist1("L2ID"+ScanSi+"Z0")->Fill(Z0);
    hist1("L2ID"+ScanSi+"EP")->Fill(fabsf((matchedL2->energy()/cosh(matchedL2->eta()))/Pt));
  }
}

void FEXValidationAlgo::FillEFCalo(const CaloCluster *matchedEFCalo, float DetaEFCalo, float DphiEFCalo, const std::string &ScanSi) {
  if (matchedEFCalo) {
    hist1("EF"+ScanSi+"CaloEt")->Fill(matchedEFCalo->et()/1.e3);
    hist1("EF"+ScanSi+"CaloDphi")->Fill(DphiEFCalo);
    hist1("EF"+ScanSi+"CaloDeta")->Fill(DetaEFCalo);
  }
}

/*void FEXValidationAlgo::FillEFTrack(const egamma *matchedEFTrk, const std::string & ScanSi) {
  const Trk::Perigee* aMeasPer;
  if (matchedEFTrk) {
    aMeasPer = matchedEFTrk->trackParticle()->perigee();
    vect["EF"+ScanSi+"TrackPhi"]->push_back(aMeasPer->parameters()[Trk::phi]);
    vect["EF"+ScanSi+"TrackEta"]->push_back(-std::log(tan(aMeasPer->parameters()[Trk::theta]/2.)));
    float pt2 = pow(aMeasPer->parameters()[Trk::px], 2.) + pow(aMeasPer->parameters()[Trk::py], 2.);
    vect["EF"+ScanSi+"TrackPt"]->push_back(sqrt(pt2));
    vect["EF"+ScanSi+"TrackD0"]->push_back(aMeasPer->parameters()[Trk::d0]);
    vect["EF"+ScanSi+"TrackZ0"]->push_back(aMeasPer->parameters()[Trk::z0]);
  }
}*/

void FEXValidationAlgo::FillHisto(const std::string & name) {
  if (vect.find(name) == vect.end()) {
    *m_log << MSG::ERROR << "Searching vector with name " << name << " failed on FillHisto." << endreq;
  } else if (vect[name] == 0) {
    *m_log << MSG::ERROR << "Found vector with name " << name << " but it is a zero pointer." << endreq;
  } else {
    for (std::vector<float>::const_iterator i = vect[name]->begin();
         i != vect[name]->end();
         i++) {
      hist1(name)->Fill(*i);
    }
    vect[name]->clear();
  }
}

void FEXValidationAlgo::FillEFScanTrackHistos() {
  FillHisto("EFScanTrackPhi");
  FillHisto("EFScanTrackEta");
  FillHisto("EFScanTrackPt");
  FillHisto("EFScanTrackD0");
  FillHisto("EFScanTrackZ0");
}

void FEXValidationAlgo::FillEFSiTrackHistos() {
  FillHisto("EFSiTrackPhi");
  FillHisto("EFSiTrackEta");
  FillHisto("EFSiTrackPt");
  FillHisto("EFSiTrackD0");
  FillHisto("EFSiTrackZ0");
}

void FEXValidationAlgo::FillTable()
{
  float cutsTmp[] = {
    (float)(m_L1EmClus/1.e3),
    (float)(m_L1EmIsol/1.e3),
    (float)(m_L1HadCore/1.e3),
    (float)(m_L1HadIsol/1.e3),
    (float)(m_L2Rcore_cut),
    (float)(m_L2Rstrip_cut),
    (float)(m_L2Et_cut/1.e3),
    (float)(m_L2Ehad_cut/1.e3),
    (float)(m_L2IDPt/1.e3),
    (float)(m_L2IDDeta),
    (float)(m_L2IDDphi),
    (float)(m_L2IDEP),
    (float)(m_EFCaloEt/1.e3),
    (float)(m_EFCaloDphi),
    (float)(m_EFCaloDeta),
    (float)(m_EFTrackeCalibFactor),
    (float)(m_EFTrackDeta),
    (float)(m_EFTrackDphi),
    (float)(m_EFTrackEtaCut),
    (float)(m_EFTrackLowEoPHighEta),
    (float)(m_EFTrackHighEoPHighEta),
    (float)(m_EFTrackLowEoPLowEta),
    (float)(m_EFTrackHighEoPLowEta)
};

  ((TNtuple *) tree("EffTable"))->Fill(m_sL, m_nTotal, m_nL1, m_nL2, m_nL2IDScan, m_nL2IDSi, m_nEFScanCalo, m_nEFSiCalo, m_nEFScanTrack, m_nEFSiTrack);
  ((TNtuple *) tree("CutTable"))->Fill((Float_t *) cutsTmp);

  // Normalize dump histograms to get Probabilities
  std::vector<std::string> DumpPattern;
  std::vector<std::string> DumpNotPattern;
  DumpPattern.push_back("/Dump/");
  std::map<std::string, TH1 *>  DumpHistograms;
  getHistsFromPath(DumpPattern, DumpNotPattern, DumpHistograms);
  addDirectory(name()+"/DumpNotScaled");
  for (std::map<std::string, TH1 *>::iterator i = DumpHistograms.begin(); i != DumpHistograms.end(); ++i) {
    TH1F *newhist = new TH1F(*( (TH1F *) i->second));
    addHistogram(newhist);
    double integral = i->second->Integral();
    if (integral != 0) {
      // Recalculate errors and normalize
      //for (int k = 0; k <= i->second->GetNbinsX(); ++k) {
      //  i->second->SetBinError(k, sqrt(i->second->GetBinContent(k))/integral);
      //}
      i->second->Sumw2();
      i->second->Scale(1.0/integral);
    }
  }
}

FEXValidationAlgo::~FEXValidationAlgo()
{
  for (std::map<std::string, std::vector<float> *>::iterator i = vect.begin(); i != vect.end(); i++) {
    if (i->second)
      delete i->second;
  }
}

