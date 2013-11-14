/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Staco.h"
#include "AANTMisc.h"
#include "TProfile.h"
#include "TF1.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TPostScript.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"

TH1F *chi2Fake, *chi2Muon;
TH2F *chi2FakeVSeta, *chi2VSeta,*chi2FakeVSphi, *chi2VSphi, *chi2FakeVSP, *chi2VSP;
TProfile *dZ0VSeta, *dA0VSeta, *dA0VSphi, *dtetVSeta, *dphiVSphi, *d1SPVSeta;
TProfile *d1SPVSetaCPZP, *d1SPVSetaCPZM, *d1SPVSetaCMZP, *d1SPVSetaCMZM;
TH1F *pulphMB, *pulphID, *pulthMB, *pulthID, *pulqpMB, *pulqpID;
TH1F *errphMB, *errphID, *errthMB, *errthID, *errqpMB, *errqpID;
TH1F *er1phMB, *er1phID, *er1thMB, *er1thID, *er1qpMB, *er1qpID;
TH1F *er2phMB, *er2phID, *er2thMB, *er2thID, *er2qpMB, *er2qpID;
TH1F *puld0, *pulz0, *pulph, *pulth, *pulqp;
TH2F *cord0, *corz0, *corph, *corth, *corqp;
TH1F *chiIDa0, *chiIDz0, *chiIDfi, *chiIDth, *chiIDqp;
TH1F *chiMSa0, *chiMSz0, *chiMSfi, *chiMSth, *chiMSqp;
TH1F *chiIDa0z0, *chiIDa0fi, *chiIDa0th, *chiIDa0qp;
TH1F *chiMSa0z0, *chiMSa0fi, *chiMSa0th, *chiMSa0qp;
TH1F *chiIDz0fi, *chiIDz0th, *chiIDz0qp;
TH1F *chiMSz0fi, *chiMSz0th, *chiMSz0qp;
TH1F *chiIDfith, *chiIDfiqp, *chiIDthqp ;
TH1F *chiMSfith, *chiMSfiqp, *chiMSthqp;
TH1F *dfiIDbon, *dfiIDbad, *efiIDbon, *efiIDbad, *fiIDbad, *thIDbad, *stacobon, *stacobad;
TH1F *phibadEndcap, *phibadBarrel;
TH2F *etaphibad;
TH1F *dptMuon, *dptFake;
TH1F *etaMuon, *etaFake;
TH1F *ptMbMuon, *ptMbFake, *ptStMuon, *ptStFake, *ptIdMuon, *ptIdFake;
TH1F *ppMbMuon, *ppMbFake, *ppStMuon, *ppStFake, *ppIdMuon, *ppIdFake;
TH1F *dphiMuon, *dphiFake, *detaMuon, *detaFake, *drrrMuon, *drrrFake;
TH2F *drVsPMuon, *drVsPFake, *detaVsPMuon, *detaVsPFake, *dtetaVsPMuon, *dtetaVsPFake, *petaVsPMuon, *petaVsPFake ;
TH2F *dd0VsPMuon, *dd0VsPFake, *dz0VsPMuon, *dz0VsPFake, *dphiVsPMuon, *dphiVsPFake;
TH2F *MboyVsMatchMuon, *MboyVsMatchFake;
TH2F *MstaVsMatchMuon, *MstaVsMatchFake;
TH2F *MstaVsNdofMuon,  *MstaVsNdofFake;
TH1F *NPIXMuon, *NPIXFake, *NSCTMuon, *NSCTFake, *NTRTMuon, *NTRTFake;
TH2F *pMSvspID;

const int ncut=8;
const float chi2cut[ncut] = { 20., 30., 40., 50., 80., 100., 200., 500. }; 
const int neta=3;
const float etacut[neta+1] = { 0., 1., 2., 2.5 };
float ngen[neta], nrec[neta][ncut];

Staco::Staco():Analysis() {

  m_Debug = kFALSE;

  m_nevt = 0;

}


Staco::~Staco() {}

void Staco::initialize(char* option) {
//
  printf("Starting Staco\n");
  if( strstr(option, "debug") != NULL )  m_Debug = kTRUE;

  chi2Fake = new TH1F("chi2Fake", "chi2Fake", 100, 0.,200. );
  chi2FakeVSeta = new TH2F("chi2FakeVSeta", "chi2FakeVSeta", 30,0.,3., 25,0.,200.);
  chi2FakeVSphi = new TH2F("chi2FakeVSphi", "chi2FakeVSphi", 30,-180.,180., 25,0.,200.);
  chi2FakeVSP   = new TH2F("chi2FakeVSP",   "chi2FakeVSP", 30,0.,100., 25,0.,200.);
  chi2Muon = new TH1F("chi2Muon", "chi2Muon", 100, 0.,200. );
  chi2VSeta = new TH2F("chi2VSeta", "chi2VSeta", 30,0.,3., 25,0.,200.);
  chi2VSphi = new TH2F("chi2VSphi", "chi2VSphi", 30,-180.,180., 25,0.,200.);
  chi2VSP   = new TH2F("chi2VSP","chi2VSP", 30,0.,100., 25,0.,200.);
  errphID = new TH1F("errphID", "errphID", 100, 0., 0.005);
  errphMB = new TH1F("errphMB", "errphMB", 100, 0., 0.050);
  errthID = new TH1F("errthID", "errthID", 100, 0., 0.005);
  errthMB = new TH1F("errthMB", "errthMB", 100, 0., 0.020);
  errqpID = new TH1F("errqpID", "errqpID", 100, 0., 0.01);
  errqpMB = new TH1F("errqpMB", "errqpMB", 100, 0., 0.01);
  er1phID = new TH1F("er1phID", "er1phID", 100, 0., 0.005);
  er1phMB = new TH1F("er1phMB", "er1phMB", 100, 0., 0.050);
  er1thID = new TH1F("er1thID", "er1thID", 100, 0., 0.005);
  er1thMB = new TH1F("er1thMB", "er1thMB", 100, 0., 0.020);
  er1qpID = new TH1F("er1qpID", "er1qpID", 100, 0., 0.01);
  er1qpMB = new TH1F("er1qpMB", "er1qpMB", 100, 0., 0.01);
  er2phID = new TH1F("er2phID", "er2phID", 100, 0., 0.005);
  er2phMB = new TH1F("er2phMB", "er2phMB", 100, 0., 0.050);
  er2thID = new TH1F("er2thID", "er2thID", 100, 0., 0.005);
  er2thMB = new TH1F("er2thMB", "er2thMB", 100, 0., 0.020);
  er2qpID = new TH1F("er2qpID", "er2qpID", 100, 0., 0.01);
  er2qpMB = new TH1F("er2qpMB", "er2qpMB", 100, 0., 0.01);
  pulphID = new TH1F("pulphID", "pulphID", 100, -50., 50.);
  pulphMB = new TH1F("pulphMB", "pulphMB", 100, -50., 50.);
  pulthID = new TH1F("pulthID", "pulthID", 100, -50., 50.);
  pulthMB = new TH1F("pulthMB", "pulthMB", 100, -50., 50.);
  pulqpID = new TH1F("pulqpID", "pulqpID", 100, -50., 50.);
  pulqpMB = new TH1F("pulqpMB", "pulqpMB", 100, -50., 50.);
  puld0 = new TH1F("puld0", "puld0", 100, -20., 20.);
  pulz0 = new TH1F("pulz0", "pulz0", 100, -20., 20.);
  pulph = new TH1F("pulph", "pulph", 100, -20., 20.);
  pulth = new TH1F("pulth", "pulth", 100, -20., 20.);
  pulqp = new TH1F("pulqp", "pulqp", 100, -20., 20.);
  cord0 = new TH2F("cord0", "cord0", 40, -1., 1., 40,-200.,200.);
  corz0 = new TH2F("corz0", "corz0", 40,-200.,200., 40,-200.,200.);
  corph = new TH2F("corph", "corph", 40,-3.2,3.2, 40,-3.2,3.2);
  corth = new TH2F("corth", "corth", 40,0.,3.2, 40,0.,3.2);
  corqp = new TH2F("corqp", "corqp", 40,-1.,1., 40,-1.,1.);

  dZ0VSeta = new TProfile("dZ0VSeta","dZ0VSeta",30,0.,3.," ");
  dA0VSeta = new TProfile("dA0VSeta","dA0VSeta",30,0.,3.," ");
  dA0VSphi = new TProfile("dA0VSphi","dA0VSphi",30,-180.,180.," ");
  dtetVSeta = new TProfile("dtetVSeta","dtetVSeta",30,0.,3.," ");
  dphiVSphi = new TProfile("dphiVSphi","dphiVSphi",30,-180.,180.," ");
  d1SPVSeta = new TProfile("d1SPVSeta","d1SPVSeta",30,0.,3.," ");
  d1SPVSetaCPZP = new TProfile("d1SPVSetaCPZP","d1SPVSetaCPZP",30,0.,3.," ");
  d1SPVSetaCPZM = new TProfile("d1SPVSetaCPZM","d1SPVSetaCPZM",30,0.,3.," ");
  d1SPVSetaCMZP = new TProfile("d1SPVSetaCMZP","d1SPVSetaCMZP",30,0.,3.," ");
  d1SPVSetaCMZM = new TProfile("d1SPVSetaCMZM","d1SPVSetaCMZM",30,0.,3.," ");

  chiIDa0 = new TH1F("chi2a0ID", "chi2a0ID", 100, 0., 100.);
  chiIDz0 = new TH1F("chi2z0ID", "chi2z0ID", 100, 0., 100.);
  chiIDfi = new TH1F("chi2fiID", "chi2fiID", 100, 0., 100.);
  chiIDth = new TH1F("chi2thID", "chi2thID", 100, 0., 100.);
  chiIDqp = new TH1F("chi2qpID", "chi2qpID", 100, 0., 100.);
  chiIDa0z0 = new TH1F("chi2a0z0ID", "chi2a0z0ID", 100, 0., 100.);
  chiIDa0fi = new TH1F("chi2a0fiID", "chi2a0fiID", 100, 0., 100.);
  chiIDa0th = new TH1F("chi2a0thID", "chi2a0thID", 100, 0., 100.);
  chiIDa0qp = new TH1F("chi2a0qpID", "chi2a0qpID", 100, 0., 100.);
  chiIDz0fi = new TH1F("chi2z0fiID", "chi2z0fiID", 100, 0., 100.);
  chiIDz0th = new TH1F("chi2z0thID", "chi2z0thID", 100, 0., 100.);
  chiIDz0qp = new TH1F("chi2z0qpID", "chi2z0qpID", 100, 0., 100.);
  chiIDfith = new TH1F("chi2fithID", "chi2fithID", 100, 0., 100.);
  chiIDfiqp = new TH1F("chi2fiqpID", "chi2fiqpID", 100, 0., 100.);
  chiIDthqp = new TH1F("chi2thqpID", "chi2thqpID", 100, 0., 100.);
  chiMSa0 = new TH1F("chi2a0MS", "chi2a0MS", 100, 0., 100.);
  chiMSz0 = new TH1F("chi2z0MS", "chi2z0MS", 100, 0., 100.);
  chiMSfi = new TH1F("chi2fiMS", "chi2fiMS", 100, 0., 100.);
  chiMSth = new TH1F("chi2thMS", "chi2thMS", 100, 0., 100.);
  chiMSqp = new TH1F("chi2qpMS", "chi2qpMS", 100, 0., 100.);
  chiMSa0z0 = new TH1F("chi2a0z0MS", "chi2a0z0MS", 100, 0., 100.);
  chiMSa0fi = new TH1F("chi2a0fiMS", "chi2a0fiMS", 100, 0., 100.);
  chiMSa0th = new TH1F("chi2a0thMS", "chi2a0thMS", 100, 0., 100.);
  chiMSa0qp = new TH1F("chi2a0qpMS", "chi2a0qpMS", 100, 0., 100.);
  chiMSz0fi = new TH1F("chi2z0fiMS", "chi2z0fiMS", 100, 0., 100.);
  chiMSz0th = new TH1F("chi2z0thMS", "chi2z0thMS", 100, 0., 100.);
  chiMSz0qp = new TH1F("chi2z0qpMS", "chi2z0qpMS", 100, 0., 100.);
  chiMSfith = new TH1F("chi2fithMS", "chi2fithMS", 100, 0., 100.);
  chiMSfiqp = new TH1F("chi2fiqpMS", "chi2fiqpMS", 100, 0., 100.);
  chiMSthqp = new TH1F("chi2thqpMS", "chi2thqpMS", 100, 0., 100.);

  dfiIDbon = new TH1F("dfiIDbon", "dfiIDbon", 100, -0.05, 0.05);
  efiIDbon = new TH1F("efiIDbon", "efiIDbon", 100,  0.0, 0.001);
  dfiIDbad = new TH1F("dfiIDbad", "dfiIDbad", 100, -0.05, 0.05);
  efiIDbad = new TH1F("efiIDbad", "efiIDbad", 100,  0.0, 0.001);
  fiIDbad  = new TH1F("fiIDbad" , "fiIDbad" , 90 , -3.15, 3.15);
  thIDbad  = new TH1F("thIDbad" , "thIDbad" , 90 ,  0.  , 3.15);
  stacobon = new TH1F("stacobon", "stacobon", 100,  0.  , 500.);
  stacobad = new TH1F("stacobad", "stacobad", 100,  0.  , 500.);

  dptMuon = new TH1F("dptMuon", "dptMuon", 100, -50., 50.);
  dptFake = new TH1F("dptFake", "dptFake", 100, -50., 50.);

  etaMuon = new TH1F("etaMuon", "etaMuon", 54, 0., 2.7);
  etaFake = new TH1F("etaFake", "etaFake", 54, 0., 2.7);

  dphiMuon = new TH1F("dphiMuon", "dphiMuon", 100, -0.5, 0.5);
  dphiFake = new TH1F("dphiFake", "dphiFake", 100, -0.5, 0.5);
  detaMuon = new TH1F("detaMuon", "detaMuon", 100, -0.5, 0.5);
  detaFake = new TH1F("detaFake", "detaFake", 100, -0.5, 0.5);
  drrrMuon = new TH1F("drrrMuon", "drrrMuon", 100,  0.0, 0.5);
  drrrFake = new TH1F("drrrFake", "drrrFake", 100,  0.0, 0.5);
  ptMbMuon = new TH1F("ptMbMuon", "ptMbMuon", 100, 0., 50.);
  ptMbFake = new TH1F("ptMbFake", "ptMbFake", 100, 0., 50.);
  ptIdMuon = new TH1F("ptIdMuon", "ptIdMuon", 100, 0., 50.);
  ptIdFake = new TH1F("ptIdFake", "ptIdFake", 100, 0., 50.);
  ptStMuon = new TH1F("ptStMuon", "ptStMuon", 100, 0., 50.);
  ptStFake = new TH1F("ptStFake", "ptStFake", 100, 0., 50.);
  ppMbMuon = new TH1F("ppMbMuon", "ppMbMuon", 100, 0., 50.);
  ppMbFake = new TH1F("ppMbFake", "ppMbFake", 100, 0., 50.);
  ppIdMuon = new TH1F("ppIdMuon", "ppIdMuon", 100, 0., 50.);
  ppIdFake = new TH1F("ppIdFake", "ppIdFake", 100, 0., 50.);
  ppStMuon = new TH1F("ppStMuon", "ppStMuon", 100, 0., 50.);
  ppStFake = new TH1F("ppStFake", "ppStFake", 100, 0., 50.);

  NPIXMuon = new TH1F("NPIXMuon", "NPIXMuon", 20, 0., 20.);
  NPIXFake = new TH1F("NPIXFake", "NPIXFake", 20, 0., 20.);
  NSCTMuon = new TH1F("NSCTMuon", "NSCTMuon", 30, 0., 30.);
  NSCTFake = new TH1F("NSCTFake", "NSCTFake", 30, 0., 30.);
  NTRTMuon = new TH1F("NTRTMuon", "NTRTMuon", 50, 0., 50.);
  NTRTFake = new TH1F("NTRTFake", "NTRTFake", 50, 0., 50.);

  pMSvspID = new TH2F("pMSvspID", "pMSvspID", 50,0.,100., 50,0.,100.);

  drVsPMuon = new TH2F("drVsPMuon", "drVsPMuon",          150, 0., 300., 50, 0., 0.5);
  drVsPFake = new TH2F("drVsPFake", "drVsPFake",          150, 0., 300., 50, 0., 0.5);
  detaVsPMuon = new TH2F("detaVsPMuon", "detaVsPMuon",    150, 0., 300., 50, 0., 0.5);
  detaVsPFake = new TH2F("detaVsPFake", "detaVsPFake",    150, 0., 300., 50, 0., 0.5);
  petaVsPMuon = new TH2F("petaVsPMuon", "petaVsPMuon",    150, 0., 300., 50, 0., 20.);
  petaVsPFake = new TH2F("petaVsPFake", "petaVsPFake",    150, 0., 300., 50, 0., 20.);
  dtetaVsPMuon = new TH2F("dtetaVsPMuon", "dtetaVsPMuon", 150, 0., 300., 50, 0., 0.5);
  dtetaVsPFake = new TH2F("dtetaVsPFake", "dtetaVsPFake", 150, 0., 300., 50, 0., 0.5);

  dd0VsPMuon = new TH2F("dd0VsPMuon", "dd0VsPMuon",          150, 0., 300., 50, 0., 900.);
  dd0VsPFake = new TH2F("dd0VsPFake", "dd0VsPFake",          150, 0., 300., 50, 0., 900.);
  dz0VsPMuon = new TH2F("dz0VsPMuon", "dz0VsPMuon",    150, 0., 300., 50, 0., 1200.);
  dz0VsPFake = new TH2F("dz0VsPFake", "dz0VsPFake",    150, 0., 300., 50, 0., 1200.);
  dphiVsPMuon = new TH2F("dphiVsPMuon", "dphiVsPMuon",    150, 0., 300., 50, 0., 0.4);
  dphiVsPFake = new TH2F("dphiVsPFake", "dphiVsPFake",    150, 0., 300., 50, 0., 0.4);

  MboyVsMatchFake = new TH2F("MboyVsMatchFake", "MboyVsMatchFake", 100, 0., 200., 100, 0., 60.);
  MboyVsMatchMuon = new TH2F("MboyVsMatchMuon", "MboyVsMatchMuon", 100, 0., 200., 100, 0., 60.);
  MstaVsMatchFake = new TH2F("MstaVsMatchFake", "MstaVsMatchFake", 100, 0., 200., 100, 0., 100.);
  MstaVsMatchMuon = new TH2F("MstaVsMatchMuon", "MstaVsMatchMuon", 100, 0., 200., 100, 0., 100.);
  MstaVsNdofFake = new TH2F("MstaVsNdofFake", "MstaVsNdofFake", 100, 0., 100., 100, 0., 100.);
  MstaVsNdofMuon = new TH2F("MstaVsNdofMuon", "MstaVsNdofMuon", 100, 0., 100., 100, 0., 100.);

  phibadEndcap = new TH1F("phibadEndcap", "phibadEndcap", 45, 0., 45.);
  phibadBarrel = new TH1F("phibadbarrel", "phibadBarrel", 45, 0., 45.);
  etaphibad = new TH2F("etaphibad", "etaphibad", 45,0.,45., 25,0.,2.5);

  for (int j=0; j<neta; j++) {
    ngen[j] = 0.;
    for (int i=0; i<ncut; i++)  nrec[j][i] = 0.;
  }
}


void Staco::execute() {
  m_nevt = m_nevt + 1;

  if( p_evt->staco_nmuon == 0 && p_evt-> MboyESD_nmuon > 0 ) {
    if( p_evt->MuonEntRec_Num > 0 && fabs((p_evt->MuonEntRec_Eta)->at(0)) < 2.5 ) {
      cout << " nostaco run " << p_evt->Run << " evt " << p_evt->Event << " spectro " << p_evt-> MboyESD_nmuon
           << " teta " << (p_evt->MuonEntRec_Eta)->at(0) << endl; 
    }
  }

  for (int irec=0; irec<p_assocST->getNrec(); irec++) {
    float stachi2 = getMatchedChi2(irec);
    float teta    = p_assocST->getThetaRec(irec);
    float eta     = fabs(p_assocST->getEtaRec(irec));
    float phi     = p_assocST->getPhiRec(irec)*180./3.14159;
    float P       = fabs(0.001/p_assocST->getChargeSurPRec(irec));
    float Pt      = P * sin(teta);
    int imb = getMboy(irec);
    int iid = getInDet(irec);
    P = fabs(0.001/((p_evt->Trk_qOverP)->at(iid)));
    float erqp = sqrt( (p_evt->Trk_sigqOverP)->at(iid)*(p_evt->Trk_sigqOverP)->at(iid) + (p_evt->MboyESD_covr55)->at(imb) );
    float dphi = (p_evt->Trk_phi)->at(iid) - (p_evt->MboyESD_Phi)->at(imb);
    if( dphi >  m_PI )  dphi = dphi - 2.*m_PI;
    if( dphi < -m_PI )  dphi = dphi + 2.*m_PI;
    float deta = TMath::ASinH(1./tan((p_evt->Trk_theta)->at(iid))) - TMath::ASinH(1./tan((p_evt->MboyESD_Theta)->at(imb)));
    float dteta = (p_evt->Trk_theta)->at(iid) - (p_evt->MboyESD_Theta)->at(imb);
    float dd0 = (p_evt->Trk_d0)->at(iid) - (p_evt->MboyESD_A0)->at(imb);
    float dz0 = (p_evt->Trk_z0)->at(iid) - (p_evt->MboyESD_Z )->at(imb);
    float chi2 = (p_evt->MboyTrkMS_Chistat)->at(imb);
    int ndof = (p_evt->MboyTrkMS_Ndof)->at(imb);
    float covfifi = (p_evt->MboyESD_covr33)->at(imb) + ((p_evt->Trk_sigphi)->at(iid))*((p_evt->Trk_sigphi)->at(iid));
    float covfith = (p_evt->MboyESD_covr43)->at(imb);
    float covthth = (p_evt->MboyESD_covr44)->at(imb) + ((p_evt->Trk_sigtheta)->at(iid))*((p_evt->Trk_sigtheta)->at(iid));
    float peta = dteta / sqrt(covthth);
//    float covz0z0 = (p_evt->MboyESD_covr22)->at(imb) + ((p_evt->Trk_sigz0)->at(iid))*((p_evt->Trk_sigz0)->at(iid));
//    float covz0th = (p_evt->MboyESD_covr42)->at(imb);
//    float covthth = (p_evt->MboyESD_covr44)->at(imb) + ((p_evt->Trk_sigtheta)->at(iid))*((p_evt->Trk_sigtheta)->at(iid));
//    float verchi2 =      ((p_evt->MboyESD_Z    )->at(imb)-(p_evt->Trk_z0   )->at(iid)) * ((p_evt->MboyESD_Z    )->at(imb)-(p_evt->Trk_z0   )->at(iid)) * covthth
//                  - 2. * ((p_evt->MboyESD_Z    )->at(imb)-(p_evt->Trk_z0   )->at(iid)) * ((p_evt->MboyESD_Theta)->at(imb)-(p_evt->Trk_theta)->at(iid)) * covz0th
//                  +      ((p_evt->MboyESD_Theta)->at(imb)-(p_evt->Trk_theta)->at(iid)) * ((p_evt->MboyESD_Theta)->at(imb)-(p_evt->Trk_theta)->at(iid)) * covz0z0;
//    verchi2 = verchi2 / (covz0z0*covthth-covz0th*covz0th);
    float verchi2 = ((p_evt->MboyESD_Theta)->at(imb)-(p_evt->Trk_theta)->at(iid)) * ((p_evt->MboyESD_Theta)->at(imb)-(p_evt->Trk_theta)->at(iid)) / covthth;
    float drr = sqrt( dphi*dphi + deta*deta );
    float mboychi2 = (p_evt->MboyTrkMS_Chifit)->at(imb);
    float mstachi2 = (p_evt->MboyTrkMS_Chistat)->at(imb) / ndof ;
    int npix = (p_evt->Trk_numberOfPixelHits)->at(iid);
    int nsct = (p_evt->Trk_numberOfSCTHits)->at(iid);
    int ntrt = (p_evt->Trk_numberOfTRTHits)->at(iid);
    if(p_assocST->isFake(irec)) {
      chi2Fake->Fill( stachi2 );
      chi2FakeVSeta->Fill( eta, stachi2 );
      chi2FakeVSphi->Fill( phi, stachi2 );
      chi2FakeVSP->Fill( P, stachi2);
      dptFake->Fill( ((p_evt->Trk_qOverP)->at(iid)-(p_evt->MboyESD_qOverP)->at(imb))/erqp );
      etaFake->Fill( eta );
      ppMbFake->Fill( fabs(1./(p_evt->MboyESD_qOverP)->at(imb))/1000. );
      ppIdFake->Fill( fabs(1./(p_evt->Trk_qOverP)->at(iid))/1000. );
      ppStFake->Fill( fabs(P) );
      ptMbFake->Fill( fabs(sin((p_evt->MboyESD_Theta)->at(imb))/(p_evt->MboyESD_qOverP)->at(imb))/1000. );
      ptIdFake->Fill( fabs(sin((p_evt->Trk_theta)->at(iid))/(p_evt->Trk_qOverP)->at(iid))/1000. );
      ptStFake->Fill( fabs(sin(teta)*P) );
      dphiFake->Fill( dphi ); 
      detaFake->Fill( deta );
      drrrFake->Fill( sqrt( dphi*dphi + deta*deta ) );
      drVsPFake->Fill( 1000./P , drr );
      detaVsPFake->Fill( 1000./P , fabs(deta) );
      petaVsPFake->Fill( 1000./P , fabs(peta) );
      dtetaVsPFake->Fill( 1000./P , fabs(dteta) );
      dd0VsPFake->Fill( 1000./P , fabs(dd0) );
      dz0VsPFake->Fill( 1000./P , fabs(dz0) );
      dphiVsPFake->Fill( 1000./P , fabs(dphi) );
      MboyVsMatchFake->Fill( stachi2 , mboychi2 );
      MstaVsMatchFake->Fill( stachi2 , mstachi2 );
      MstaVsNdofFake->Fill( ndof     , mstachi2 );
      NPIXFake->Fill( npix );
      NSCTFake->Fill( nsct );
      NTRTFake->Fill( ntrt );
    } else {
      chi2Muon->Fill( stachi2 );
      chi2VSeta->Fill( eta, stachi2 );
      chi2VSphi->Fill( phi, stachi2 );
      chi2VSP->Fill( P, stachi2);
      dptMuon->Fill( ((p_evt->Trk_qOverP)->at(iid)-(p_evt->MboyESD_qOverP)->at(imb))/erqp );
      etaMuon->Fill( eta );
      ppMbMuon->Fill( fabs(1./(p_evt->MboyESD_qOverP)->at(imb))/1000. );
      ppIdMuon->Fill( fabs(1./(p_evt->Trk_qOverP)->at(iid))/1000. );
      ppStMuon->Fill( fabs(P) );
      ptMbMuon->Fill( fabs(sin((p_evt->MboyESD_Theta)->at(imb))/(p_evt->MboyESD_qOverP)->at(imb))/1000. );
      ptIdMuon->Fill( fabs(sin((p_evt->Trk_theta)->at(iid))/(p_evt->Trk_qOverP)->at(iid))/1000. );
      ptStMuon->Fill( fabs(sin(teta)*P) );
      dphiMuon->Fill( dphi );
      detaMuon->Fill( deta );
      drrrMuon->Fill( sqrt( dphi*dphi + deta*deta ) );
      drVsPMuon->Fill( 1000./P , drr );
      detaVsPMuon->Fill( 1000./P , fabs(deta) );
      petaVsPMuon->Fill( 1000./P , fabs(peta) );
      dtetaVsPMuon->Fill( 1000./P , fabs(dteta) );
      dd0VsPMuon->Fill( 1000./P , fabs(dd0) );
      dz0VsPMuon->Fill( 1000./P , fabs(dz0) );
      dphiVsPMuon->Fill( 1000./P , fabs(dphi) );
      MboyVsMatchMuon->Fill( stachi2 , mboychi2 );
      MstaVsMatchMuon->Fill( stachi2 , mstachi2 );
      MstaVsNdofMuon->Fill( ndof     , mstachi2 );
      NPIXMuon->Fill( npix );
      NSCTMuon->Fill( nsct );
      NTRTMuon->Fill( ntrt );
    }
    pMSvspID->Fill( fabs(1./(p_evt->Trk_qOverP)->at(iid))/1000. , fabs(1./(p_evt->MboyESD_qOverP)->at(imb))/1000. ); 

    if( stachi2 > -1. ) {
      int imb = getMboy(irec);
      int iid = getInDet(irec);
      if( imb >= 0 && iid >= 0 ) {
        float erd0 = sqrt(     (p_evt->Trk_sigd0)->at(iid)*(p_evt->Trk_sigd0)->at(iid)     + (p_evt->MboyESD_covr11)->at(imb) );
        float erz0 = sqrt(     (p_evt->Trk_sigz0)->at(iid)*(p_evt->Trk_sigz0)->at(iid)     + (p_evt->MboyESD_covr22)->at(imb) );
        float erph = sqrt(    (p_evt->Trk_sigphi)->at(iid)*(p_evt->Trk_sigphi)->at(iid)    + (p_evt->MboyESD_covr33)->at(imb) );
        float erth = sqrt(  (p_evt->Trk_sigtheta)->at(iid)*(p_evt->Trk_sigtheta)->at(iid)  + (p_evt->MboyESD_covr44)->at(imb) );
        float erqp = sqrt( (p_evt->Trk_sigqOverP)->at(iid)*(p_evt->Trk_sigqOverP)->at(iid) + (p_evt->MboyESD_covr55)->at(imb) );
        puld0->Fill( (    (p_evt->Trk_d0)->at(iid)-(p_evt->MboyESD_A0)->at(imb)    )/erd0 );
        pulz0->Fill( (    (p_evt->Trk_z0)->at(iid)-(p_evt->MboyESD_Z)->at(imb)     )/erz0 );
        pulph->Fill( (   (p_evt->Trk_phi)->at(iid)-(p_evt->MboyESD_Phi)->at(imb)   )/erph );
        pulth->Fill( ( (p_evt->Trk_theta)->at(iid)-(p_evt->MboyESD_Theta)->at(imb) )/erth );
        pulqp->Fill( ((p_evt->Trk_qOverP)->at(iid)-(p_evt->MboyESD_qOverP)->at(imb))/erqp ); 
        cord0->Fill(     (p_evt->Trk_d0)->at(iid), (p_evt->MboyESD_A0)->at(imb)     );
        corz0->Fill(     (p_evt->Trk_z0)->at(iid), (p_evt->MboyESD_Z)->at(imb)      );
        corph->Fill(    (p_evt->Trk_phi)->at(iid), (p_evt->MboyESD_Phi)->at(imb)    );
	corth->Fill(  (p_evt->Trk_theta)->at(iid), (p_evt->MboyESD_Theta)->at(imb)  );
        corqp->Fill( (p_evt->Trk_qOverP)->at(iid), (p_evt->MboyESD_qOverP)->at(imb) );

        dZ0VSeta->Fill( eta, (p_evt->Trk_z0)->at(iid)-(p_evt->MboyESD_Z)->at(imb));
        dA0VSeta->Fill( eta, (p_evt->Trk_d0)->at(iid)-(p_evt->MboyESD_A0)->at(imb));
        dA0VSphi->Fill( phi, (p_evt->Trk_d0)->at(iid)-(p_evt->MboyESD_A0)->at(imb));
        dtetVSeta->Fill( eta, (p_evt->Trk_theta)->at(iid)-(p_evt->MboyESD_Theta)->at(imb));
        float dphi = ((p_evt->Trk_phi)->at(iid)-(p_evt->MboyESD_Phi)->at(imb))*180./3.14159;
        if( dphi > 180. )  dphi = dphi - 360.;
        if( dphi <-180. )  dphi = dphi + 360.;
        dphiVSphi->Fill( phi, dphi);
        float d1SP = 1000.*((p_evt->Trk_qOverP)->at(iid)-(p_evt->MboyESD_qOverP)->at(imb));
        d1SPVSeta->Fill( eta, d1SP);
        if( p_assocST->getEtaRec(irec) > 0. ) {
          if( p_assocST->getChargeSurPRec(irec) > 0. ) d1SPVSetaCPZP->Fill( eta, d1SP);
          else                                         d1SPVSetaCMZP->Fill( eta, d1SP);
        } else {
          if( p_assocST->getChargeSurPRec(irec) > 0. ) d1SPVSetaCPZM->Fill( eta, d1SP);
          else                                         d1SPVSetaCMZM->Fill( eta, d1SP);
        }
//
        int isim;
        float a0ref, z0ref, firef, thref, qpref;

	/*        isim = 1
        a0ref = (p_evt->staco_A0)->at(irec);
        z0ref = (p_evt->staco_Z)->at(irec);
        firef = (p_evt->staco_Phi)->at(irec);
        thref = (p_evt->staco_Theta)->at(irec);
        qpref = (p_evt->staco_qOverP)->at(irec); */

        isim = p_assocST->getSim(irec);
        if( isim >= 0 ) {
          a0ref = p_assocST->getD0Sim(isim);
	  z0ref = p_assocST->getZ0Sim(isim);
	  firef = p_assocST->getPhiSim(isim);
	  thref = p_assocST->getThetaSim(isim);
	  qpref = p_assocST->getChargeSurPSim(isim);
	}
	
/*         if( isim >= 0 ) { */
/*           if( (p_evt->Trk_truthNt)->at(iid) != isim ) { */
/*             cout << " PROBLEME ID " << (p_evt->Trk_truthNt)->at(iid) << "  " << isim << endl; */
/*             dumpSimul(); */
/* 	  } */
/*           float di1 = (p_evt->Trk_d0)->at(iid)     - a0ref; */
/*           float di2 = (p_evt->Trk_z0)->at(iid)     - z0ref; */
/*           float di3 = (p_evt->Trk_phi)->at(iid)    - firef; */
/*           float di4 = (p_evt->Trk_theta)->at(iid)  - thref; */
/*           float di5 = (p_evt->Trk_qOverP)->at(iid) - qpref; */
/*           float ci11 = (p_evt->Trk_sigd0)->at(iid) * (p_evt->Trk_sigd0)->at(iid); */
/*           float ci21 = (p_evt->Trk_covVert21)->at(iid); */
/*           float ci22 = (p_evt->Trk_sigz0)->at(iid) * (p_evt->Trk_sigz0)->at(iid); */
/*           float ci31 = (p_evt->Trk_covVert31)->at(iid); */
/*           float ci32 = (p_evt->Trk_covVert32)->at(iid); */
/*           float ci33 = (p_evt->Trk_sigphi)->at(iid) * (p_evt->Trk_sigphi)->at(iid); */
/*           float ci41 = (p_evt->Trk_covVert41)->at(iid); */
/*           float ci42 = (p_evt->Trk_covVert42)->at(iid); */
/*           float ci43 = (p_evt->Trk_covVert43)->at(iid); */
/*           float ci44 = (p_evt->Trk_sigtheta)->at(iid) * (p_evt->Trk_sigtheta)->at(iid); */
/*           float ci51 = (p_evt->Trk_covVert51)->at(iid); */
/*           float ci52 = (p_evt->Trk_covVert52)->at(iid); */
/*           float ci53 = (p_evt->Trk_covVert53)->at(iid); */
/*           float ci54 = (p_evt->Trk_covVert54)->at(iid); */
/*           float ci55 = (p_evt->Trk_sigqOverP)->at(iid) * (p_evt->Trk_sigqOverP)->at(iid); */
/*           if( di3 > 3.14159 )  di3 = di3 - 2.*3.14159; */
/*           if( di3 <-3.14159 )  di3 = di3 + 2.*3.14159; */
/*           chiIDa0->Fill( di1*di1/ci11 ); */
/*           chiIDz0->Fill( di2*di2/ci22 ); */
/*           chiIDfi->Fill( di3*di3/ci33 ); */
/*           chiIDth->Fill( di4*di4/ci44 ); */
/*           chiIDqp->Fill( di5*di5/ci55 ); */
/*           chiIDa0z0->Fill( (ci22*di1*di1-2.*ci21*di1*di2+ci11*di2*di2)/(ci11*ci22-ci21*ci21) ); */
/*           chiIDa0fi->Fill( (ci33*di1*di1-2.*ci31*di1*di3+ci11*di3*di3)/(ci11*ci33-ci31*ci31) ); */
/*           chiIDa0th->Fill( (ci44*di1*di1-2.*ci41*di1*di4+ci11*di4*di4)/(ci11*ci44-ci41*ci41) ); */
/*           chiIDa0qp->Fill( (ci55*di1*di1-2.*ci51*di1*di5+ci11*di5*di5)/(ci11*ci55-ci51*ci51) ); */
/*           chiIDz0fi->Fill( (ci33*di2*di2-2.*ci32*di2*di3+ci22*di3*di3)/(ci22*ci33-ci32*ci32) ); */
/*           chiIDz0th->Fill( (ci44*di2*di2-2.*ci42*di2*di4+ci22*di4*di4)/(ci22*ci44-ci42*ci42) ); */
/*           chiIDz0qp->Fill( (ci55*di2*di2-2.*ci52*di2*di5+ci22*di5*di5)/(ci22*ci55-ci52*ci52) ); */
/*           chiIDfith->Fill( (ci44*di3*di3-2.*ci43*di3*di4+ci33*di4*di4)/(ci33*ci44-ci43*ci43) ); */
/*           chiIDfiqp->Fill( (ci55*di3*di3-2.*ci53*di3*di5+ci33*di5*di5)/(ci33*ci55-ci53*ci53) ); */
/*           chiIDthqp->Fill( (ci55*di4*di4-2.*ci54*di4*di5+ci44*di5*di5)/(ci44*ci55-ci54*ci54) ); */
/*           if( di3*di3/ci33 < 10. ) { */
/*             dfiIDbon->Fill( di3 ); */
/*             efiIDbon->Fill( sqrt(ci33) ); */
/*             stacobon->Fill( stachi2 ); */
/*           } else if( di3*di3/ci33 > 100. ) { */
/*             dfiIDbad->Fill( di3 ); */
/*             efiIDbad->Fill( sqrt(ci33) ); */
/*             fiIDbad->Fill( firef ); */
/*             thIDbad->Fill( thref ); */
/*             stacobad->Fill( stachi2 ); */
/*           } */

/*           float dm1 = (p_evt->Mboy_A0)->at(imb)     - a0ref; */
/*           float dm2 = (p_evt->MboyESD_Z)->at(imb)      - z0ref; */
/*           float dm3 = (p_evt->MboyESD_Phi)->at(imb)    - firef; */
/*           float dm4 = (p_evt->MboyESD_Theta)->at(imb)  - thref; */
/*           float dm5 = (p_evt->MboyESD_qOverP)->at(imb) - qpref; */
/*           float cm11 = (p_evt->MboyESD_covr11)->at(imb); */
/*           float cm21 = (p_evt->MboyESD_covr21)->at(imb); */
/*           float cm22 = (p_evt->MboyESD_covr22)->at(imb); */
/*           float cm31 = (p_evt->MboyESD_covr31)->at(imb); */
/*           float cm32 = (p_evt->MboyESD_covr32)->at(imb); */
/*           float cm33 = (p_evt->MboyESD_covr33)->at(imb); */
/*           float cm41 = (p_evt->MboyESD_covr41)->at(imb); */
/*           float cm42 = (p_evt->MboyESD_covr42)->at(imb); */
/*           float cm43 = (p_evt->MboyESD_covr43)->at(imb); */
/*           float cm44 = (p_evt->MboyESD_covr44)->at(imb); */
/*           float cm51 = (p_evt->MboyESD_covr51)->at(imb); */
/*           float cm52 = (p_evt->MboyESD_covr52)->at(imb); */
/*           float cm53 = (p_evt->MboyESD_covr53)->at(imb); */
/*           float cm54 = (p_evt->MboyESD_covr54)->at(imb); */
/*           float cm55 = (p_evt->MboyESD_covr55)->at(imb); */
/*           if( dm3 > 3.14159 )  dm3 = dm3 - 2.*3.14159; */
/*           if( dm3 <-3.14159 )  dm3 = dm3 + 2.*3.14159; */
/*           chiMSa0->Fill( dm1*dm1/cm11 ); */
/*           chiMSz0->Fill( dm2*dm2/cm22 ); */
/*           chiMSfi->Fill( dm3*dm3/cm33 ); */
/*           chiMSth->Fill( dm4*dm4/cm44 ); */
/*           chiMSqp->Fill( dm5*dm5/cm55 ); */
/*           chiMSa0z0->Fill( (cm22*dm1*dm1-2.*cm21*dm1*dm2+cm11*dm2*dm2)/(cm11*cm22-cm21*cm21) ); */
/*           chiMSa0fi->Fill( (cm33*dm1*dm1-2.*cm31*dm1*dm3+cm11*dm3*dm3)/(cm11*cm33-cm31*cm31) ); */
/*           chiMSa0th->Fill( (cm44*dm1*dm1-2.*cm41*dm1*dm4+cm11*dm4*dm4)/(cm11*cm44-cm41*cm41) ); */
/*           chiMSa0qp->Fill( (cm55*dm1*dm1-2.*cm51*dm1*dm5+cm11*dm5*di5)/(cm11*cm55-cm51*cm51) ); */
/*           chiMSz0fi->Fill( (cm33*dm2*dm2-2.*cm32*dm2*dm3+cm22*dm3*dm3)/(cm22*cm33-cm32*cm32) ); */
/*           chiMSz0th->Fill( (cm44*dm2*dm2-2.*cm42*dm2*dm4+cm22*dm4*dm4)/(cm22*cm44-cm42*cm42) ); */
/*           chiMSz0qp->Fill( (cm55*dm2*dm2-2.*cm52*dm2*dm5+cm22*dm5*dm5)/(cm22*cm55-cm52*cm52) ); */
/*           chiMSfith->Fill( (cm44*dm3*dm3-2.*cm43*dm3*dm4+cm33*dm4*dm4)/(cm33*cm44-cm43*cm43) ); */
/*           chiMSfiqp->Fill( (cm55*dm3*dm3-2.*cm53*dm3*dm5+cm33*dm5*dm5)/(cm33*cm55-cm53*cm53) ); */
/*           chiMSthqp->Fill( (cm55*dm4*dm4-2.*cm54*dm4*dm5+cm44*dm5*dm5)/(cm44*cm55-cm54*cm54) ); */
/*         } */
//
        isim = p_assocST->getSim(irec);
        float cut = 5.;
        if( isim >= 0 ) {
          float errph = (p_evt->Trk_sigphi)->at(iid);
          float errth = (p_evt->Trk_sigtheta)->at(iid);
          float errqp = (p_evt->Trk_sigqOverP)->at(iid);
          dphi = (p_evt->Trk_phi)->at(iid) - p_assocST->getPhiSim(isim);
          if( dphi > 3.14159 )  dphi = dphi - 2.*3.14159;
          if( dphi <-3.14159 )  dphi = dphi + 2.*3.14159;
          float pulph = dphi / errph;
          float pulth = ( (p_evt->Trk_theta)->at(iid) - p_assocST->getThetaSim(isim) ) / errth;
	  float pulqp = ( (p_evt->Trk_qOverP)->at(iid) - p_assocST->getChargeSurPSim(isim) ) / errqp;
          pulphID->Fill( pulph );
          pulthID->Fill( pulth );
          pulqpID->Fill( pulqp );
          errphID->Fill( errph );
          errthID->Fill( errth );
          errqpID->Fill( errqp*1000. );
          if( fabs( pulph) < cut )  er1phID->Fill( errph );
          else                      er2phID->Fill( errph );
          if( fabs( pulth) < cut )  er1thID->Fill( errth );
          else                      er2thID->Fill( errth );
          if( fabs( pulqp) < cut )  er1qpID->Fill( errqp*1000. );         
          else                      er2qpID->Fill( errqp*1000. );
          errph = sqrt( (p_evt->MboyESD_covr33)->at(imb) );
          errth = sqrt( (p_evt->MboyESD_covr44)->at(imb) );
          errqp = sqrt( (p_evt->MboyESD_covr55)->at(imb) );
	  dphi = (p_evt->MboyESD_Phi)->at(imb) - p_assocST->getPhiSim(isim);
          if( dphi > 3.14159 )  dphi = dphi - 2.*3.14159;
          if( dphi <-3.14159 )  dphi = dphi + 2.*3.14159;
          pulph = dphi / errph;
          pulth = ( (p_evt->MboyESD_Theta)->at(imb) - p_assocST->getThetaSim(isim) ) / errth;
          pulqp = ( (p_evt->MboyESD_qOverP)->at(imb) - p_assocST->getChargeSurPSim(isim) ) / errqp;
          pulphMB->Fill( pulph );
          pulthMB->Fill( pulth );
          pulqpMB->Fill( pulqp );
          errphMB->Fill( errph );
          errthMB->Fill( errth );
          errqpMB->Fill( errqp*1000. );
          if( fabs( pulph) < cut )  er1phMB->Fill( errph );
          else                      er2phMB->Fill( errph );
          if( fabs( pulth) < cut )  er1thMB->Fill( errth );
          else                      er2thMB->Fill( errth );
          if( fabs( pulqp) < cut )  er1qpMB->Fill( errqp*1000. );         
          else                      er2qpMB->Fill( errqp*1000. );
        }
      }
    }
  }

// efficiency vs chi2 cut
  for (int isim=0; isim<p_assocST->getNsim(); isim++) {
    if(p_assocST->isMuonDirect(isim)) {
      float eta = p_assocST->getEtaSim(isim);
      if( fabs(eta) < 2.5 ) {
        int ieta = 0;
        for (int j=0; j<neta; j++) { if( fabs(eta) > etacut[j] ) ieta = j; }
        ngen[ieta] = ngen[ieta] + 1;
        int irec = p_assocST->getRec(isim);
        if( irec >= 0 ) {
//        float stachi2 = (p_evt->staco_chi2)->at(irec);
          float stachi2 = getMatchedChi2(irec);
          if( stachi2 > 50. ) {
            float phiphi = p_assocST->getPhiSim(isim)*180./3.14159;
            while( phiphi > 45. ) phiphi = phiphi - 45.;
            while( phiphi <  0. ) phiphi = phiphi + 45.;
            if( fabs(eta) < 1.2 ) phibadBarrel->Fill( phiphi );
            else                  phibadEndcap->Fill( phiphi );
            etaphibad->Fill( phiphi, fabs(eta) );
          }
          for (int i=0; i<ncut; i++) { if( stachi2 <  chi2cut[i] ) nrec[ieta][i] = nrec[ieta][i] + 1.; }
          if( stachi2 > 100. ) {
            cout << " probleme chi2 " << stachi2 << " evt run " << p_evt->Event << " " << p_evt->Run << endl;
            int imb = getMboy(irec);
            int iid = getInDet(irec);
            cout << " q/p phi theta d0 z0 ID " << (p_evt->Trk_qOverP)->at(iid)      << " " << (p_evt->Trk_phi)->at(iid)     << " " << (p_evt->Trk_theta)->at(iid)     
                                        << " " << (p_evt->Trk_d0)->at(iid)          << " " << (p_evt->Trk_z0)->at(iid)      << endl; 
            cout << "                        " << (p_evt->Trk_sigqOverP)->at(iid)   << " " << (p_evt->Trk_sigphi)->at(iid)  << " " << (p_evt->Trk_sigtheta)->at(iid) << endl;
            cout << "                     MS " << (p_evt->MboyESD_qOverP)->at(imb)  << " " << (p_evt->MboyESD_Phi)->at(imb) << " " << (p_evt->MboyESD_Theta)->at(imb)
	                                << " " << (p_evt->MboyESD_A0)->at(imb)      << " " << (p_evt->MboyESD_Z)->at(imb)   << endl;
            cout << "                        " << sqrt((p_evt->MboyESD_covr55)->at(imb)) << " " << sqrt((p_evt->MboyESD_covr33)->at(imb)) << " " << sqrt((p_evt->MboyESD_covr44)->at(imb)) << endl;
            cout << "                    sim " << p_assocST->getChargeSurPSim(isim) << " " << p_assocST->getPhiSim(isim)    << " " << p_assocST->getThetaSim(isim)    << endl;
            if( p_assocMS->getSim(imb) >= 0 ) {
	      cout << " q/P spectro mes " << p_assocMS->getChargeSurPRec(imb) << " sim " << p_assocMS->getChargeSurPSim( p_assocMS->getSim(imb) ) << endl;
	    }
            if( p_evt->Trk_totalNumTracks < 10 ) {
              for (int k=0; k<p_evt->Trk_totalNumTracks; k++) {
		cout << " track ID " << (p_evt->Trk_qOverP)->at(k) << " " << (p_evt->Trk_phi)->at(k) << " " << (p_evt->Trk_theta)->at(k) << endl;
              }
            }

          }
        }
      }
    }
  }
}

void Staco::finalize() {
  char nom[256];

  sprintf(nom, "dump.coupureStaco%s", m_comment);
  FILE *txtFile = fopen(nom, "w");
  float nr, ng;
  float eff, err, fak;
  for (int j=0; j<neta; j++) {
    fprintf( txtFile," zone eta : %3.1f < abs(eta) < %3.1f \n", etacut[j], etacut[j+1] );
    for (int i=0; i<ncut; i++) {
      eff = nrec[j][i]/ngen[j]; err = sqrt(eff*(1.-eff)/ngen[j]);
      fprintf( txtFile," effic coupure à  %3.0f : %.4f +/- %.4f \n", chi2cut[i], eff, err );
    }
  }
  fprintf( txtFile," =============== all eta ==================== \n" );
  for (int i=0; i<ncut; i++) {
    ng = 0.;
    nr = 0.;
    for (int j=0; j<neta; j++) {
      ng = ng + ngen[j];
      nr = nr + nrec[j][i];
    }
    eff = nr/ng; err = sqrt(eff*(1.-eff)/ng);
    fprintf( txtFile," effic coupure à  %3.0f : %.4f +/- %.4f \n", chi2cut[i], eff, err );
  }
  fclose(txtFile);

  TCanvas *chi2Staco = new TCanvas("chi2", "chi2", 60, 10, m_LARGPS, m_HAUTPS); chi2Staco->Divide(2,4);
  chi2Staco->cd(1); gPad->SetLogy(1); chi2Muon->Draw();
  chi2Staco->cd(2); gPad->SetLogy(1); chi2Fake->Draw();
  chi2Staco->cd(3); chi2VSeta->SetStats(kFALSE);     chi2VSeta->SetMarkerSize(0.6);     chi2VSeta->Draw();
  chi2Staco->cd(4); chi2FakeVSeta->SetStats(kFALSE); chi2FakeVSeta->SetMarkerSize(0.6); chi2FakeVSeta->Draw();
//  chi2Staco->cd(5); chi2VSphi->SetStats(kFALSE);     chi2VSphi->SetMarkerSize(0.6);     chi2VSphi->Draw();
//  chi2Staco->cd(6); chi2FakeVSphi->SetStats(kFALSE); chi2FakeVSphi->SetMarkerSize(0.6); chi2FakeVSphi->Draw();
  chi2Staco->cd(5); gPad->SetLogy(1); dptMuon->Draw();
  chi2Staco->cd(6); gPad->SetLogy(1); dptFake->Draw();
  chi2Staco->cd(7); chi2VSP->SetStats(kFALSE);    chi2VSP->SetMarkerSize(0.6);    chi2VSP->Draw();
  chi2Staco->cd(8); chi2FakeVSP->SetStats(kFALSE); chi2FakeVSP->SetMarkerSize(0.6); chi2FakeVSP->Draw();
  chi2Staco->Update(); 
  sprintf(nom, "Chi2Staco%s.ps", m_comment);  chi2Staco->Print(nom);

  TCanvas *dangStaco;
  dangStaco = new TCanvas("dang", "dang", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,3);
  dangStaco->cd(1); etaMuon->Draw();
  dangStaco->cd(2); etaFake->Draw();
  dangStaco->cd(3); dphiMuon->Draw();
  dangStaco->cd(4); dphiFake->Draw();
  dangStaco->cd(5); detaMuon->Draw();
  dangStaco->cd(6); detaFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps(", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("danb", "danb", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,2);
  dangStaco->cd(1); drrrMuon->Draw();
  dangStaco->cd(2); drrrFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("danc", "danc", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,3);
  dangStaco->cd(1); ppMbMuon->Draw();
  dangStaco->cd(2); ppMbFake->Draw();
  dangStaco->cd(3); ppIdMuon->Draw();
  dangStaco->cd(4); ppIdFake->Draw();
  dangStaco->cd(5); ppStMuon->Draw();
  dangStaco->cd(6); ppStFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("dan2", "dan2", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,3);
  dangStaco->cd(1); ptMbMuon->Draw();
  dangStaco->cd(2); ptMbFake->Draw();
  dangStaco->cd(3); ptIdMuon->Draw();
  dangStaco->cd(4); ptIdFake->Draw();
  dangStaco->cd(5); ptStMuon->Draw();
  dangStaco->cd(6); ptStFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("dan3", "dan3", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,3);
  dangStaco->cd(1); NPIXMuon->Draw();
  dangStaco->cd(2); NPIXFake->Draw();
  dangStaco->cd(3); NSCTMuon->Draw();
  dangStaco->cd(4); NSCTFake->Draw();
  dangStaco->cd(5); NTRTMuon->Draw();
  dangStaco->cd(6); NTRTFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("dan4", "dan4", 60, 10, m_LARGPS, m_HAUTPS);
  dangStaco->cd(1); pMSvspID->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("dan5", "dan5", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,3);
  dangStaco->cd(1); MboyVsMatchMuon->SetMarkerSize(0.6); MboyVsMatchMuon->Draw();
  dangStaco->cd(2); MboyVsMatchFake->SetMarkerSize(0.6); MboyVsMatchFake->Draw();
  dangStaco->cd(3); MstaVsMatchMuon->SetMarkerSize(0.6); MstaVsMatchMuon->Draw();
  dangStaco->cd(4); MstaVsMatchFake->SetMarkerSize(0.6); MstaVsMatchFake->Draw();
  dangStaco->cd(5); MstaVsNdofMuon->SetMarkerSize(0.6);  MstaVsNdofMuon->Draw();
  dangStaco->cd(6); MstaVsNdofFake->SetMarkerSize(0.6);  MstaVsNdofFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("dan6", "dan6", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,3);
  dangStaco->cd(1); dd0VsPMuon->SetMarkerSize(0.6);  dd0VsPMuon->Draw();
  dangStaco->cd(2); dd0VsPFake->SetMarkerSize(0.6);  dd0VsPFake->Draw();
  dangStaco->cd(3); dz0VsPMuon->SetMarkerSize(0.6);  dz0VsPMuon->Draw();
  dangStaco->cd(4); dz0VsPFake->SetMarkerSize(0.6);  dz0VsPFake->Draw();
  dangStaco->cd(5); dphiVsPMuon->SetMarkerSize(0.6); dphiVsPMuon->Draw();
  dangStaco->cd(6); dphiVsPFake->SetMarkerSize(0.6); dphiVsPFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps", m_comment);  dangStaco->Print(nom);
  delete dangStaco;
  dangStaco = new TCanvas("dan7", "dan7", 60, 10, m_LARGPS, m_HAUTPS); dangStaco->Divide(2,4);
  dangStaco->cd(1); drVsPMuon->SetMarkerSize(0.6);    drVsPMuon->Draw();
  dangStaco->cd(2); drVsPFake->SetMarkerSize(0.6);    drVsPFake->Draw();
  dangStaco->cd(3); detaVsPMuon->SetMarkerSize(0.6);  detaVsPMuon->Draw();
  dangStaco->cd(4); detaVsPFake->SetMarkerSize(0.6);  detaVsPFake->Draw();
  dangStaco->cd(5); petaVsPMuon->SetMarkerSize(0.6);  petaVsPMuon->Draw();
  dangStaco->cd(6); petaVsPFake->SetMarkerSize(0.6);  petaVsPFake->Draw();
  dangStaco->cd(7); dtetaVsPMuon->SetMarkerSize(0.6); dtetaVsPMuon->Draw();
  dangStaco->cd(8); dtetaVsPFake->SetMarkerSize(0.6); dtetaVsPFake->Draw();
  dangStaco->Update();
  sprintf(nom, "dangStaco%s.ps)", m_comment);  dangStaco->Print(nom);
  delete dangStaco;

  TCanvas *biasStaco = new TCanvas("bias", "bias", 60, 10, m_LARGPS, m_HAUTPS); biasStaco->Divide(2,3);
  biasStaco->cd(1); dZ0VSeta->Draw();
  biasStaco->cd(2); dA0VSeta->Draw();
  biasStaco->cd(3); dA0VSphi->Draw();
  biasStaco->cd(4); dtetVSeta->Draw();
  biasStaco->cd(5); dphiVSphi->Draw();
  biasStaco->cd(6); d1SPVSeta->Draw();
  biasStaco->Update();
  sprintf(nom, "BiasStaco%s.ps(", m_comment);  biasStaco->Print(nom);
  TCanvas *bias2Staco = new TCanvas("bias2", "bias2", 60, 10, m_LARGPS, m_HAUTPS); bias2Staco->Divide(2,2);
  bias2Staco->cd(1); d1SPVSetaCPZP->Draw();
  bias2Staco->cd(2); d1SPVSetaCPZM->Draw();
  bias2Staco->cd(3); d1SPVSetaCMZP->Draw();
  bias2Staco->cd(4); d1SPVSetaCMZM->Draw();
  bias2Staco->Update();
  sprintf(nom, "BiasStaco%s.ps)", m_comment);  bias2Staco->Print(nom);

  TCanvas *deltaStaco = new TCanvas("delta", "delta", 60, 10, m_LARGPS, m_HAUTPS); deltaStaco->Divide(2,3);
  gStyle->SetOptStat(111110);
  deltaStaco->cd(1); gPad->SetLogy(1); pulphID->Draw();
  deltaStaco->cd(2); gPad->SetLogy(1); pulphMB->Draw();
  deltaStaco->cd(3); gPad->SetLogy(1); pulthID->Draw();
  deltaStaco->cd(4); gPad->SetLogy(1); pulthMB->Draw();
  deltaStaco->cd(5); gPad->SetLogy(1); pulqpID->Draw();
  deltaStaco->cd(6); gPad->SetLogy(1); pulqpMB->Draw();
  deltaStaco->Update(); 
  sprintf(nom, "DeltaStaco%s.ps(", m_comment);  deltaStaco->Print(nom);
  TCanvas *errorStaco = new TCanvas("error", "error", 60, 10, m_LARGPS, m_HAUTPS); errorStaco->Divide(2,3);
  gStyle->SetOptStat(111110);
  errorStaco->cd(1); gPad->SetLogy(1); errphID->Draw();
  errorStaco->cd(2); gPad->SetLogy(1); errphMB->Draw();
  errorStaco->cd(3); gPad->SetLogy(1); errthID->Draw();
  errorStaco->cd(4); gPad->SetLogy(1); errthMB->Draw();
  errorStaco->cd(5); gPad->SetLogy(1); errqpID->Draw();
  errorStaco->cd(6); gPad->SetLogy(1); errqpMB->Draw();
  errorStaco->Update(); 
  sprintf(nom, "DeltaStaco%s.ps", m_comment);  errorStaco->Print(nom);
  TCanvas *erro1Staco = new TCanvas("erro1", "erro1", 60, 10, m_LARGPS, m_HAUTPS); erro1Staco->Divide(2,3);
  gStyle->SetOptStat(111110);
  erro1Staco->cd(1); gPad->SetLogy(1); er1phID->Draw();
  erro1Staco->cd(2); gPad->SetLogy(1); er1phMB->Draw();
  erro1Staco->cd(3); gPad->SetLogy(1); er1thID->Draw();
  erro1Staco->cd(4); gPad->SetLogy(1); er1thMB->Draw();
  erro1Staco->cd(5); gPad->SetLogy(1); er1qpID->Draw();
  erro1Staco->cd(6); gPad->SetLogy(1); er1qpMB->Draw();
  erro1Staco->Update(); 
  sprintf(nom, "DeltaStaco%s.ps", m_comment);  erro1Staco->Print(nom);
  TCanvas *erro2Staco = new TCanvas("erro2", "erro2", 60, 10, m_LARGPS, m_HAUTPS); erro2Staco->Divide(2,3);
  gStyle->SetOptStat(111110);
  erro2Staco->cd(1); gPad->SetLogy(1); er2phID->Draw();
  erro2Staco->cd(2); gPad->SetLogy(1); er2phMB->Draw();
  erro2Staco->cd(3); gPad->SetLogy(1); er2thID->Draw();
  erro2Staco->cd(4); gPad->SetLogy(1); er2thMB->Draw();
  erro2Staco->cd(5); gPad->SetLogy(1); er2qpID->Draw();
  erro2Staco->cd(6); gPad->SetLogy(1); er2qpMB->Draw();
  erro2Staco->Update(); 
  sprintf(nom, "DeltaStaco%s.ps)", m_comment);  erro2Staco->Print(nom);

  TCanvas *pullStaco = new TCanvas("pull", "pull", 60, 10, m_LARGPS, m_HAUTPS); pullStaco->Divide(2,3);
  gStyle->SetOptStat(111110);
  pullStaco->cd(1); puld0->SetStats(kTRUE); puld0->Draw();
  pullStaco->cd(2); pulz0->SetStats(kTRUE); pulz0->Draw();
  pullStaco->cd(3); pulph->SetStats(kTRUE); pulph->Draw();
  pullStaco->cd(4); pulth->SetStats(kTRUE); pulth->Draw();
  pullStaco->cd(5); pulqp->SetStats(kTRUE); pulqp->Draw();
  pullStaco->Update();
  TCanvas *corrStaco = new TCanvas("corr", "corr", 60, 10, m_LARGPS, m_HAUTPS); corrStaco->Divide(2,3);
  corrStaco->cd(1); cord0->Draw();
  corrStaco->cd(2); corz0->Draw();
  corrStaco->cd(3); corph->Draw();
  corrStaco->cd(4); corth->Draw();
  corrStaco->cd(5); corqp->Draw();
  corrStaco->Update();
  /*  sprintf(nom, "PullStaco%s.png", m_comment); pullStaco->Print(nom); */
  sprintf(nom, "PullStaco%s.ps(", m_comment);  pullStaco->Print(nom);
  sprintf(nom, "PullStaco%s.ps)", m_comment);  corrStaco->Print(nom);

  gStyle->SetOptStat(111110);
  TCanvas *canva1 = new TCanvas("canva1", "canva1", 60, 10, m_LARGPS, m_HAUTPS); canva1->Divide(2,3);
  canva1->cd(1); gPad->SetLogy(1); chiIDa0->Draw();
  canva1->cd(2); gPad->SetLogy(1); chiIDz0->Draw();
  canva1->cd(3); gPad->SetLogy(1); chiIDfi->Draw();
  canva1->cd(4); gPad->SetLogy(1); chiIDth->Draw();
  canva1->cd(5); gPad->SetLogy(1); chiIDqp->Draw();
  canva1->Update();
  TCanvas *canva2 = new TCanvas("canva2", "canva2", 60, 10, m_LARGPS, m_HAUTPS); canva2->Divide(2,3);
  canva2->cd(1); gPad->SetLogy(1); chiMSa0->Draw();
  canva2->cd(2); gPad->SetLogy(1); chiMSz0->Draw();
  canva2->cd(3); gPad->SetLogy(1); chiMSfi->Draw();
  canva2->cd(4); gPad->SetLogy(1); chiMSth->Draw();
  canva2->cd(5); gPad->SetLogy(1); chiMSqp->Draw();
  canva2->Update();
  TCanvas *canva3 = new TCanvas("canva3", "canva3", 60, 10, m_LARGPS, m_HAUTPS); canva3->Divide(2,2);
  canva3->cd(1); gPad->SetLogy(1); chiIDa0z0->Draw();
  canva3->cd(2); gPad->SetLogy(1); chiIDa0fi->Draw();
  canva3->cd(3); gPad->SetLogy(1); chiIDa0th->Draw();
  canva3->cd(4); gPad->SetLogy(1); chiIDa0qp->Draw();
  canva3->Update();
  TCanvas *canva4 = new TCanvas("canva4", "canva4", 60, 10, m_LARGPS, m_HAUTPS); canva4->Divide(2,2);
  canva4->cd(1); gPad->SetLogy(1); chiMSa0z0->Draw();
  canva4->cd(2); gPad->SetLogy(1); chiMSa0fi->Draw();
  canva4->cd(3); gPad->SetLogy(1); chiMSa0th->Draw();
  canva4->cd(4); gPad->SetLogy(1); chiMSa0qp->Draw();
  canva4->Update();
  TCanvas *canva5 = new TCanvas("canva5", "canva5", 60, 10, m_LARGPS, m_HAUTPS); canva5->Divide(2,3);
  canva5->cd(1); gPad->SetLogy(1); chiIDz0fi->Draw();
  canva5->cd(2); gPad->SetLogy(1); chiIDz0th->Draw();
  canva5->cd(3); gPad->SetLogy(1); chiIDz0qp->Draw();
  canva5->cd(4); gPad->SetLogy(1); chiIDfith->Draw();
  canva5->cd(5); gPad->SetLogy(1); chiIDfiqp->Draw();
  canva5->cd(6); gPad->SetLogy(1); chiIDthqp->Draw();
  canva5->Update();
  TCanvas *canva6 = new TCanvas("canva6", "canva6", 60, 10, m_LARGPS, m_HAUTPS); canva6->Divide(2,3);
  canva6->cd(1); gPad->SetLogy(1); chiMSz0fi->Draw();
  canva6->cd(2); gPad->SetLogy(1); chiMSz0th->Draw();
  canva6->cd(3); gPad->SetLogy(1); chiMSz0qp->Draw();
  canva6->cd(4); gPad->SetLogy(1); chiMSfith->Draw();
  canva6->cd(5); gPad->SetLogy(1); chiMSfiqp->Draw();
  canva6->cd(6); gPad->SetLogy(1); chiMSthqp->Draw();
  canva6->Update();
  TCanvas *canva7 = new TCanvas("canva7", "canva7", 60, 10, m_LARGPS, m_HAUTPS);
// canva7->Divide(1,2);
//  canva7->cd(1); dfiIDbon->Draw();
//  canva7->cd(2); efiIDbon->Draw();
//  canva7->cd(3); dfiIDbad->Draw();
//  canva7->cd(4); efiIDbad->Draw();
//  canva7->cd(5);  fiIDbad->Draw();
//  canva7->cd(6);  thIDbad->Draw();
//  canva7->cd(7);  stacobon->Draw();
//  canva7->cd(8);  stacobad->Draw();
//  canva7->cd(1); phibadBarrel->Draw();
//  canva7->cd(2); phibadEndcap->Draw();
  etaphibad->SetStats(kFALSE); etaphibad->SetMarkerSize(0.6); etaphibad->Draw();
  canva7->Update();

  sprintf(nom, "Chi2PartielStaco%s.ps(", m_comment); canva1->Print(nom);
  sprintf(nom, "Chi2PartielStaco%s.ps",  m_comment); canva2->Print(nom); canva3->Print(nom); canva4->Print(nom);
                                                     canva5->Print(nom); canva6->Print(nom);
  sprintf(nom, "Chi2PartielStaco%s.ps)", m_comment); canva7->Print(nom);

  gStyle->SetOptStat(10);

}

int Staco::getMboy(int i) {
  for (int imb=0; imb<p_assocMB->getNrec(); imb++) {
    if(fabs(p_assocMB->getThetaRec(imb)-(p_evt->staco_ThetaMS)->at(i))<0.00001 &&
       fabs(p_assocMB->getPhiRec(imb)-(p_evt->staco_PhiMS)->at(i))<0.00001) {
      return imb;
    }
  }
  return -1;
}
  
int Staco::getInDet(int i) {
  for (int iid=0; iid<p_evt->Trk_totalNumTracks && iid<500; iid++) {
    if(fabs((p_evt->Trk_theta)->at(iid)-(p_evt->staco_ThetaID)->at(i))<0.00001 &&
       fabs((p_evt->Trk_phi)->at(iid)-(p_evt->staco_PhiID)->at(i))<0.00001) {
      return iid;
    }
  }
  return -1;
}

float Staco::getMatchedChi2(int i) {
  int imb = getMboy(i);
  int iid = getInDet(i);
  if( imb < 0 || iid < 0 ) {
    cout << " problem in Staco::getMatchedChi2  imb iid " << imb << "  " << iid << endl;
    return (p_evt->staco_chi2)->at(i);
  } else {
    float chi2 = (p_evt->staco_chi2)->at(i) - (p_evt->Trk_Chi2)->at(iid) - (p_evt->MboyESD_Chi2)->at(imb) ;
    if( chi2 < 0. )  chi2 = 0.;
    return chi2;
  }
}

void Staco::dumpSimul() {
  for (int i=0; i<p_evt->NPar; i++) {
    cout << " Rv " << (p_evt->RVGen)->at(i) << " Zv " << (p_evt->ZVGen)->at(i) << " code " << (p_evt->Type)->at(i)
         << " fille " << (p_evt->KLDauNt)->at(i)
         << " vertOri " << (p_evt->KOriVNt)->at(i)
         << " vertEnd " << (p_evt->KEndVNt)->at(i)
         << endl;
  }
}

