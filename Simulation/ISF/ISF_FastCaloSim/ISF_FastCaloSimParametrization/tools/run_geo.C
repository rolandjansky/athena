/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TChain.h"
#include <iostream>

void run_geo();

void run_geo()
{
  
 //how to run in root 6 on lxplus:
 //.x init_geo.C+
 //.x run_geo.C
 
 // Warning: cell lookup in the FCal is not working yet!
 CaloGeometryFromFile* geo=new CaloGeometryFromFile();
 geo->SetDoGraphs(1);
 geo->LoadGeometryFromFile("/afs/cern.ch/atlas/groups/Simulation/FastCaloSim/ATLAS-GEO-20-00-01.root","ATLAS-GEO-20-00-01");
 //CaloGeometry::m_debug_identity=3179554531063103488;
 //geo->Validate();
 
 
  const CaloDetDescrElement* cell;
  cell=geo->getDDE(2,0.24,0.24); //This is not working yet for the FCal!!!
  cout<<"Found cell id="<<cell->identify()<<" sample="<<cell->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<endl;
  
  Long64_t cellid64(3179554531063103488);
  Identifier cellid(cellid64);
  cell=geo->getDDE(cellid); //This is working also for the FCal
  cout<<"Found cell id="<<cell->identify()<<" sample="<<cell->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<endl;
  
  new TCanvas("Calo_layout","Calo layout");
  TH2D* hcalolayout=new TH2D("hcalolayout","hcalolayout",50,-7000,7000,50,0,4000);
  hcalolayout->Draw();
  hcalolayout->SetStats(0);
  
  for(int i=0;i<24;++i) {
    double eta=0.2;
    double mineta,maxeta;
    geo->minmaxeta(i,eta,mineta,maxeta);
    cout<<geo->SamplingName(i)<<" : mineta="<<mineta<<" maxeta="<<maxeta<<endl;
    if(mineta<eta && maxeta>eta) {
      double avgeta=eta;
      cout<<"  r="<<geo->rent(i,avgeta)<<" -> "<<geo->rmid(i,avgeta)<<" -> "<<geo->rext(i,avgeta)<<endl;
    }  
    geo->GetGraph(i)->Draw("Psame");
  }
  
  geo->DrawGeoForPhi0();
 
 
}

 