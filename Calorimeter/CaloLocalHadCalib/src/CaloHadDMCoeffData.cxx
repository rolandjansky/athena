/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloHadDMCoeffData.cxx,v 1.2 2009-05-18 20:31:52 pospelov Exp $
//
// Description: see CaloHadDMCoeffData.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------
#include "CaloLocalHadCalib/CaloHadDMCoeffData.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "CaloLocalHadCalib/CaloHadDMCoeffData.h"
#include "CaloLocalHadCalib/CaloLocalHadCoeffHelper.h"


CaloHadDMCoeffData::CaloHadDMCoeffData(TTree *tree) :
  fChain(nullptr),
  fCurrent(0),
  m_ncls(0),
  m_mc_pdg(0),
  m_mc_ener(0),
  m_mc_eta(0),
  m_mc_phi(0),
  m_cls_ener(nullptr),
  m_cls_ener_unw(nullptr),
  m_cls_lambda(nullptr),
  m_cls_eta(nullptr),
  m_cls_phi(nullptr),
  m_cls_smpener(nullptr),
  m_cls_smpener_unw(nullptr),
  m_narea(0),
  m_cls_eprep(nullptr),
  m_cls_dmener(nullptr),
  m_engClusSumCalib(0),
  m_cls_engcalib(nullptr),
  m_cls_recostat(nullptr),
  m_cls_pi0prob(nullptr),
  m_cls_isol(nullptr),
  m_cls_oocener(nullptr),
  m_cls_calib_emfrac(nullptr),
  m_cls_engcalibpres(nullptr),
  b_ncls(nullptr),
  b_mc_pdg(nullptr),
  b_mc_ener(nullptr),
  b_mc_eta(nullptr),
  b_mc_phi(nullptr),
  b_cls_ener(nullptr),
  b_cls_ener_unw(nullptr),
  b_cls_lambda(nullptr),
  b_cls_eta(nullptr),
  b_cls_phi(nullptr),
  b_cls_smpener_unw(nullptr),
  b_m_narea(nullptr),
  b_cls_eprep(nullptr),
  b_cls_dmener(nullptr),
  b_engClusSumCalib(nullptr),
  b_cls_engcalib(nullptr),
  b_cls_recostat(nullptr),
  b_cls_pi0prob(nullptr),
  b_cls_isol(nullptr),
  b_cls_oocener(nullptr),
  b_cls_calib_emfrac(nullptr),
  b_cls_engcalibpres(nullptr),
  m_MaxEventsPerFile(0),
  m_classify_type(kCLASSIFY_USE_PDG)
{
  if (tree == nullptr) {
    //std::cout << "CaloHadDMCoeffData::CaloHadDMCoeffData() -> Error. No pointer to the tree!" << std::endl;
    return;
  }
  Init(tree);
}


CaloHadDMCoeffData::~CaloHadDMCoeffData()
{
//   if (!fChain) return;
//   delete fChain->GetCurrentFile();
}


// Int_t CaloHadDMCoeffData::GetEntry(Long64_t entry)
// {
// // Read contents of entry.
//   if (!fChain) return 0;
//   return fChain->GetEntry(entry);
// }


Int_t CaloHadDMCoeffData::GetEntry(Long64_t entry)
{
  static int offset = 0;
  // Read contents of entry.
  if (!fChain) return 0;
  if(!m_MaxEventsPerFile) {
    return fChain->GetEntry(entry);
  }else{
    // read only m_MaxEventsPerFile
    if(entry==0) offset = 0;
    Int_t nbytes = fChain->GetEntry(offset++);
    TTree *loc = ((TChain *)fChain)->GetTree();
    //std::cout << "entry:" << entry << " offset:" << offset << " loc->GetReadEntry():" << loc->GetReadEntry() << std::endl;
    if( loc->GetReadEntry() >= m_MaxEventsPerFile) {
      offset = ((TChain *)fChain)->GetTreeOffset()[ fChain->GetTreeNumber()] + loc->GetEntries();
      //std::cout << "changing offset "<< ((TChain *)fChain)->GetTreeOffset()[ fChain->GetTreeNumber()] << " " << loc->GetEntries() << " " << offset << std::endl;
      nbytes = fChain->GetEntry(offset++);
    }
   return nbytes;
  }
}


Int_t CaloHadDMCoeffData::GetEntries()
{
// Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntries();
}


void CaloHadDMCoeffData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

  m_ncls = 0;
  m_mc_pdg = 0;
  m_mc_ener = 0.0;
  m_mc_eta = 0.0;
  m_mc_phi = 0.0;
  m_narea = 0;
  m_engClusSumCalib = 0.0;

  // Set object pointer
  m_cls_ener = nullptr;
  m_cls_ener_unw = nullptr;
  m_cls_lambda = nullptr;
  m_cls_eta = nullptr;
  m_cls_phi = nullptr;
  //m_cls_emfrac = 0;
  //m_cls_smpener = 0;
  m_cls_smpener_unw = nullptr;
  //m_cls_ibin = 0;
  m_cls_eprep = nullptr;
  m_cls_dmener = nullptr;
  m_cls_engcalib = nullptr;
  m_cls_recostat = nullptr;
  m_cls_pi0prob = nullptr;
  m_cls_isol = nullptr;
  m_cls_oocener = nullptr;
  m_cls_calib_emfrac = nullptr;
  m_cls_engcalibpres = nullptr;
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("ncls", &m_ncls, &b_ncls);
  fChain->SetBranchAddress("mc_pdg", &m_mc_pdg, &b_mc_pdg);
  fChain->SetBranchAddress("mc_ener", &m_mc_ener, &b_mc_ener);
  fChain->SetBranchAddress("mc_eta", &m_mc_eta, &b_mc_eta);
  fChain->SetBranchAddress("mc_phi", &m_mc_phi, &b_mc_phi);
  fChain->SetBranchAddress("cls_ener", &m_cls_ener, &b_cls_ener);
  fChain->SetBranchAddress("cls_ener_unw", &m_cls_ener_unw, &b_cls_ener_unw);
  fChain->SetBranchAddress("cls_lambda", &m_cls_lambda, &b_cls_lambda);
  fChain->SetBranchAddress("cls_eta", &m_cls_eta, &b_cls_eta);
  fChain->SetBranchAddress("cls_phi", &m_cls_phi, &b_cls_phi);
  //fChain->SetBranchAddress("cls_emfrac", &m_cls_emfrac, &b_cls_emfrac);
  //fChain->SetBranchAddress("cls_smpener", &m_cls_smpener, &b_cls_smpener);
  fChain->SetBranchAddress("cls_smpener_unw", &m_cls_smpener_unw, &b_cls_smpener_unw);
  //fChain->SetBranchAddress("cls_ibin", &m_cls_ibin, &b_cls_ibin);
  fChain->SetBranchAddress("narea", &m_narea, &b_m_narea);
  fChain->SetBranchAddress("cls_eprep", &m_cls_eprep, &b_cls_eprep);
  fChain->SetBranchAddress("cls_dmener", &m_cls_dmener, &b_cls_dmener);
  fChain->SetBranchAddress("cls_engcalib", &m_cls_engcalib, &b_cls_engcalib);
  fChain->SetBranchAddress("cls_recostat", &m_cls_recostat, &b_cls_recostat);
  fChain->SetBranchAddress("cls_pi0prob", &m_cls_pi0prob, &b_cls_pi0prob);
  fChain->SetBranchAddress("cls_isol", &m_cls_isol, &b_cls_isol);
  fChain->SetBranchAddress("cls_oocener", &m_cls_oocener, &b_cls_oocener);
  fChain->SetBranchAddress("engClusSumCalib", &m_engClusSumCalib, &b_engClusSumCalib);
  fChain->SetBranchAddress("cls_calib_emfrac", &m_cls_calib_emfrac, &b_cls_calib_emfrac);
  fChain->SetBranchAddress("cls_engcalibpres", &m_cls_engcalibpres, &b_cls_engcalibpres);
}



/* ****************************************************************************
create tree which points to the variables of given class
**************************************************************************** */
TTree *CaloHadDMCoeffData::MakeTree(const char *treename)
{
  TTree *tree = new TTree(treename, treename);

  tree->Branch("ncls",&m_ncls,"ncls/I");
  tree->Branch("mc_pdg",&m_mc_pdg,"mc_pdg/I");
  tree->Branch("mc_ener",&m_mc_ener,"mc_ener/D");
  tree->Branch("mc_eta",&m_mc_eta,"mc_eta/D");
  tree->Branch("mc_phi",&m_mc_phi,"mc_phi/D");
  m_cls_ener = new std::vector<double >;
  tree->Branch("cls_ener","vector<double >",&m_cls_ener);
  m_cls_ener_unw = new std::vector<double >;
  tree->Branch("cls_ener_unw","vector<double >",&m_cls_ener_unw);

  m_cls_lambda = new std::vector<double >;
  tree->Branch("cls_lambda","vector<double >",&m_cls_lambda);
  m_cls_eta = new std::vector<double >;
  tree->Branch("cls_eta","vector<double >",&m_cls_eta);
  m_cls_phi = new std::vector<double >;
  tree->Branch("cls_phi","vector<double >",&m_cls_phi);
//   m_cls_emfrac = new std::vector<double >;
//   tree->Branch("cls_emfrac","vector<double >",&m_cls_emfrac);

  m_cls_smpener = new std::vector<std::vector<double > >;
//  tree->Branch("cls_smpener","vector<vector<double > >",&m_cls_smpener);
  m_cls_smpener_unw = new std::vector<std::vector<double > >;
  tree->Branch("cls_smpener_unw","vector<vector<double > >",&m_cls_smpener_unw);
  //m_cls_ibin = new std::vector<std::vector<int > >;
  //tree->Branch("cls_ibin","vector<vector<int > >",&m_cls_ibin);
  tree->Branch("narea",&m_narea,"narea/I");
  m_cls_eprep = new std::vector<std::vector<double > >;
  tree->Branch("cls_eprep","vector<vector<double > >",&m_cls_eprep);
  m_cls_dmener = new std::vector<std::vector<double > >;
  tree->Branch("cls_dmener","vector<vector<double > >",&m_cls_dmener);
  tree->Branch("engClusSumCalib",&m_engClusSumCalib,"engClusSumCalib/D");
  m_cls_engcalib = new std::vector<double >;
  tree->Branch("cls_engcalib","vector<double >",&m_cls_engcalib);
  m_cls_recostat = new std::vector<int >;
  tree->Branch("cls_recostat","vector<int >",&m_cls_recostat);
  m_cls_pi0prob = new std::vector<double >;
  tree->Branch("cls_pi0prob","vector<double >",&m_cls_pi0prob);
  m_cls_isol = new std::vector<double >;
  tree->Branch("cls_isol","vector<double >",&m_cls_isol);
  m_cls_oocener = new std::vector<double >;
  tree->Branch("cls_oocener","vector<double >",&m_cls_oocener);
  m_cls_calib_emfrac = new std::vector<double >;
  tree->Branch("cls_calib_emfrac","vector<double >",&m_cls_calib_emfrac);
  m_cls_engcalibpres = new std::vector<double >;
  tree->Branch("cls_engcalibpres","vector<double >",&m_cls_engcalibpres);

  return tree;
}


/* ****************************************************************************
create tree which points to the variables of given class
**************************************************************************** */
void CaloHadDMCoeffData::clear()
{
  m_ncls = 0;
  m_mc_pdg = 0;
  m_mc_ener = 0.0;
  m_mc_eta = 0.0;
  m_mc_phi = 0.0;
  m_cls_ener->clear();
  m_cls_smpener->clear();
  m_cls_ener_unw->clear();
  m_cls_lambda->clear();
  m_cls_eta->clear();
  m_cls_phi->clear();
  //m_cls_emfrac->clear();
  m_cls_smpener_unw->clear();
  m_narea = 0;
  //m_cls_ibin->clear();
  m_cls_eprep->clear();
  m_cls_dmener->clear();
  m_engClusSumCalib = 0.0;
  m_cls_engcalib->clear();
  m_cls_recostat->clear();
  m_cls_pi0prob->clear();
  m_cls_isol->clear();
  m_cls_oocener->clear();
  m_cls_calib_emfrac->clear();
  m_cls_engcalibpres->clear();
}



/* ****************************************************************************
pack cluster variables into vector of float for later calculation of global bin number
**************************************************************************** */
int CaloHadDMCoeffData::PackClusterVars(int i_cls, std::vector<float> &vars)
{

  float clusEmFrac = -1.0;
  if(m_classify_type == kCLASSIFY_USE_PARTICLEID) {
    clusEmFrac = (*m_cls_calib_emfrac)[i_cls];
  }else{
    // if sample contains only pi+- or pi0
    if( m_mc_pdg == 111) {
      clusEmFrac = 0.99; // pi0
    }else if( abs(m_mc_pdg) == 211) {
      clusEmFrac = 0.25; // pi+, pi-
    }else{
      std::cout << "CaloHadDMCoeffData::PackClusterVars() -> Warning! Wrong particle " << m_mc_pdg << std::endl;
    }
  }
  float clusSide = ( (*m_cls_eta)[i_cls]<0?-1.0:1.0);
  float clusEta = (*m_cls_eta)[i_cls];
  float clusPhi = (*m_cls_phi)[i_cls];
  float clusEner = (*m_cls_ener_unw)[i_cls];
  if(clusEner > 0.0) {
    clusEner = log10( clusEner );
  } else {
    clusEner = 0.0;
  }
  float clusLambda = (*m_cls_lambda)[i_cls];
  if(clusLambda > 0.0) {
    clusLambda = log10(clusLambda);
  } else{
    clusLambda = 0.0;
  }
  if(clusLambda > 4.0) clusLambda =  3.9999;
  vars.resize(CaloLocalHadCoeffHelper::DIM_UNKNOWN, 0.0);
  vars[CaloLocalHadCoeffHelper::DIM_EMFRAC] = clusEmFrac;
  vars[CaloLocalHadCoeffHelper::DIM_SIDE] = clusSide;
  vars[CaloLocalHadCoeffHelper::DIM_ETA] = fabs(clusEta);
  vars[CaloLocalHadCoeffHelper::DIM_PHI] = clusPhi;
  vars[CaloLocalHadCoeffHelper::DIM_ENER] = clusEner;
  vars[CaloLocalHadCoeffHelper::DIM_LAMBDA] = clusLambda;
  return 0;
}


