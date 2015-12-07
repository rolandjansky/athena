/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloGeometryFromFile.h"
#include <TTree.h>
#include <TFile.h>
#include "CaloDetDescr/CaloDetDescrElement.h"

using namespace std;

CaloGeometryFromFile::CaloGeometryFromFile() : CaloGeometry()
{
}

CaloGeometryFromFile::~CaloGeometryFromFile()
{
}

bool CaloGeometryFromFile::LoadGeometryFromFile(TString filename,TString treename)
{
  TTree *tree;
  TFile *f = TFile::Open(filename);
  if(!f) return false;
  f->GetObject(treename,tree);
  if(!tree) return false;

  TTree* fChain = tree;
  
  CaloDetDescrElement cell;
  
  // List of branches
  TBranch        *b_identifier;   //!
  TBranch        *b_calosample;   //!
  TBranch        *b_eta;   //!
  TBranch        *b_phi;   //!
  TBranch        *b_r;   //!
  TBranch        *b_eta_raw;   //!
  TBranch        *b_phi_raw;   //!
  TBranch        *b_r_raw;   //!
  TBranch        *b_x;   //!
  TBranch        *b_y;   //!
  TBranch        *b_z;   //!
  TBranch        *b_x_raw;   //!
  TBranch        *b_y_raw;   //!
  TBranch        *b_z_raw;   //!
  TBranch        *b_deta;   //!
  TBranch        *b_dphi;   //!
  TBranch        *b_dr;   //!
  TBranch        *b_dx;   //!
  TBranch        *b_dy;   //!
  TBranch        *b_dz;   //!
  
  fChain->SetMakeClass(1);
  fChain->SetBranchAddress("identifier", &cell.m_identify, &b_identifier);
  fChain->SetBranchAddress("calosample", &cell.m_calosample, &b_calosample);
  fChain->SetBranchAddress("eta", &cell.m_eta, &b_eta);
  fChain->SetBranchAddress("phi", &cell.m_phi, &b_phi);
  fChain->SetBranchAddress("r", &cell.m_r, &b_r);
  fChain->SetBranchAddress("eta_raw", &cell.m_eta_raw, &b_eta_raw);
  fChain->SetBranchAddress("phi_raw", &cell.m_phi_raw, &b_phi_raw);
  fChain->SetBranchAddress("r_raw", &cell.m_r_raw, &b_r_raw);
  fChain->SetBranchAddress("x", &cell.m_x, &b_x);
  fChain->SetBranchAddress("y", &cell.m_y, &b_y);
  fChain->SetBranchAddress("z", &cell.m_z, &b_z);
  fChain->SetBranchAddress("x_raw", &cell.m_x_raw, &b_x_raw);
  fChain->SetBranchAddress("y_raw", &cell.m_y_raw, &b_y_raw);
  fChain->SetBranchAddress("z_raw", &cell.m_z_raw, &b_z_raw);
  fChain->SetBranchAddress("deta", &cell.m_deta, &b_deta);
  fChain->SetBranchAddress("dphi", &cell.m_dphi, &b_dphi);
  fChain->SetBranchAddress("dr", &cell.m_dr, &b_dr);
  fChain->SetBranchAddress("dx", &cell.m_dx, &b_dx);
  fChain->SetBranchAddress("dy", &cell.m_dy, &b_dy);
  fChain->SetBranchAddress("dz", &cell.m_dz, &b_dz);
  
  Long64_t nentries = fChain->GetEntriesFast();
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = fChain->LoadTree(jentry);
    if (ientry < 0) break;
    fChain->GetEntry(jentry);
    
    CaloDetDescrElement* pcell=new CaloDetDescrElement(cell);
    addcell(pcell);

    if(jentry%25000==0) {
      //cout<<jentry<<" : "<<cell.getSampling()<<", "<<cell.identify()<<endl;
/*
      cout<<"all : "<<m_cells.size()<<endl;
      for(int i=0;i<MAX_SAMPLING;++i) {
        cout<<"  cells sampling "<<i<<" : "<<m_cells_in_sampling[i].size()<<" cells";
        cout<<", "<<m_cells_in_regions[i].size()<<" lookup map(s)"<<endl;
      } 
*/      
      
      //if(jentry>5) break;
    }
  }

  delete f;

  return PostProcessGeometry();
}

