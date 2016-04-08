/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "readBTag.h"
#include "readParticleJetTree.h"
#include <TTree.h>
#include <iostream>
#include <TMath.h>

using namespace std;

int writeNtuple(void) {

  cout << "starting" << endl;

  readParticleJetTree* t=new readParticleJetTree();
  readBTag* myBTag=new readBTag();
  

  Int_t nVTX;
  Int_t nTracksAtVtx;
  Int_t nSingleTracks;
  Float_t energyFraction;
  Float_t mass;
  Float_t significance3d;
  Int_t cat_nVTX;
  Int_t cat_nTracksAtVtx;
  Int_t cat_nSingleTracks;
  Int_t cat_flavour;
  Float_t IP3dlike;
  Float_t IPlikecombold;
  Int_t cat_oneVTX;
  Float_t pT;
  Float_t eta;
  
//  Int_t cat_twoTracks;
  


  const double s_pion=139.57018;

  TFile *file=new TFile("simpleTree.root","recreate");
  
  TTree* myTree=new TTree("SVTree","SVTree");
  
  myTree->Branch("nVTX",&nVTX,"nVTX/I");
  myTree->Branch("nTracksAtVtx",&nTracksAtVtx,"nTracksAtVtx/I");
  myTree->Branch("nSingleTracks",&nSingleTracks,"nSingleTracks/I");
  myTree->Branch("energyFraction",&energyFraction,"energyFraction/F");
  myTree->Branch("mass",&mass,"mass/F");
  myTree->Branch("significance3d",&significance3d,"significance3d/F");
  myTree->Branch("cat_nVTX",&cat_nVTX,"cat_nVTX/I");
  myTree->Branch("cat_nTracksAtVtx",&cat_nTracksAtVtx,"cat_nTracksAtVtx/I");
  myTree->Branch("cat_nSingleTracks",&cat_nSingleTracks,"cat_nSingleTracks/I");
  myTree->Branch("cat_flavour",&cat_flavour,"cat_flavour/I");
  myTree->Branch("IP3dlike",&IP3dlike,"IP3dlike/F");
  myTree->Branch("IPlikecombold",&IPlikecombold,"IPlikecombold/F");
  myTree->Branch("cat_oneVTX",&cat_oneVTX,"cat_oneVTX/I");
  myTree->Branch("pT",&pT,"pT/F");
  myTree->Branch("eta",&eta,"eta/F");
//  myTree->Branch("cat_twoTracks",&cat_twoTracks,"cat_twoTracks/I");
  


  Int_t num_entries=t->fChain->GetEntries();
  
  if (num_entries!=myBTag->fChain->GetEntries()) {
    throw;
  }

  cout << "Total entries are: " << num_entries << endl;
  for (Int_t i=0;i<num_entries;i++) {

//    std::cout << " get entry " << i << std::endl;
    myBTag->GetEntry(i);
    IP3dlike=myBTag->WeightIP3D;
    IPlikecombold=myBTag->Discriminator;
//    std::cout << " finished get entry" << IP3dlike << " " << IPlikecombold << std::endl;
   
      
    //    cout << " IP3dlike " << IP3dlike << endl;
    
    if (i % 10000 == 0 ) {
      std::cout << " processing event number " << i << std::endl;
    }

    t->GetEntry(i);
    cat_flavour=(Int_t)t->m_jet_flavour;
    
    // energy from primary vertex

    if (fabs(t->m_jet_momentum_eta)<2.5&&t->m_jet_momentum_pt>15000.) {

      eta=t->m_jet_momentum_eta;
      pT=t->m_jet_momentum_pt;
      
    double primaryVertexEnergy=0;
    
    for (int j=0;j<t->number_primary;++j) {
      
      if (fabs(t->primary_IPd0[j])<3.5&&
          fabs(t->primary_IPz0[j])<5.&&
          t->primary_pT[j]>500.) 
      {
        primaryVertexEnergy+=TMath::Sqrt(t->primary_p[j]*t->primary_p[j]+s_pion*s_pion);
      }
      
    }
    
    for (int o=0;o<t->neutral_particle_number;++o) 
    {
      if (t->neutral_particle_forIP[o]>0
          &&
          TMath::Sqrt(fabs(t->neutral_particle_IPchi2[o]))<3)
      {
        primaryVertexEnergy+=TMath::Sqrt(t->neutral_particle_mass[o]*
                                         t->neutral_particle_mass[o]+
                                         1./t->neutral_particle_qOverP[o]*
                                         1./t->neutral_particle_qOverP[o]);
      }
    }
    
    primaryVertexEnergy+=t->jetVTXfit_energyFromPrimaryVtx;


      
 
      nVTX=0;
      nSingleTracks=0;

     for (int q=0;q<t->jetVTXfit_clusternum;q++) 
    {
      if (t->jetVTXfit_dist[q]>0)
      {
        if (t->jetVTXfit_ntracks[q]>1)
        {
          nVTX+=1;
        }
        else
        {
          nSingleTracks+=1;
        }
      }
    }


      cat_nVTX=nVTX;
      cat_nSingleTracks=nSingleTracks;

      nTracksAtVtx=0;
      energyFraction=0.;

      for (int u=0;u<t->jetVTXfit_clusternum;++u) {
        if (t->jetVTXfit_dist[u]>0) 
        {
          
          if (t->jetVTXfit_ntracks[u]>1) {
            nTracksAtVtx+=t->jetVTXfit_ntracks[u];

          }

          if (nVTX>0)
          {
            nTracksAtVtx+=t->jetVTXfit_nNeutracks[u];
          }
          //    if (t->jetVTXfit_ntracks[u]>1||nVTX==0) {
//          if (t->jetVTXfit_ntracks[u]>1||nVTX==0) {
          energyFraction+=TMath::Sqrt(t->jetVTXfit_p[u]*t->jetVTXfit_p[u]+
                                      t->jetVTXfit_mass[u]*t->jetVTXfit_mass[u]);
//                                      t->jetVTXfit_massWithNeu[u]*t->jetVTXfit_massWithNeu[u]);
//            std::cout << " energy from sec: " << energyFraction << std::endl;
            //}
        }
      }
      
      if (energyFraction+primaryVertexEnergy>0) {
        energyFraction=energyFraction/(energyFraction+primaryVertexEnergy);
      }
      
      cat_nTracksAtVtx=nTracksAtVtx;
      mass=0.;
      significance3d=0.;
      if (nVTX>0) {
	Float_t px(0.);
	Float_t py(0.);
	Float_t pz(0.);
	Float_t dist(0.);
	Float_t inverror(0.);
	Float_t e(0.);
	for (int u=0;u<t->jetVTXfit_clusternum;++u) {
          if (t->jetVTXfit_dist[u]>0) 
          {
	  if (t->jetVTXfit_ntracks[u]>1) {
	    dist+=t->jetVTXfit_dist[u]/t->jetVTXfit_errdist[u]/t->jetVTXfit_errdist[u];
	    inverror+=1./t->jetVTXfit_errdist[u]/t->jetVTXfit_errdist[u];
          }
          px+=t->jetVTXfit_px[u];
          py+=t->jetVTXfit_py[u];
          pz+=t->jetVTXfit_pz[u];
          e+=TMath::Sqrt(t->jetVTXfit_p[u]*t->jetVTXfit_p[u]+t->jetVTXfit_mass[u]*t->jetVTXfit_mass[u]);
          }
	}
	if (e*e-px*px-py*py-pz*pz>0.) {
	  mass=TMath::Sqrt(e*e-px*px-py*py-pz*pz);
	  if (mass>5000.) {
	    Double_t touse=mass-5000.;
	    Double_t pi=TMath::Pi();
	    mass = 5000.+5000./pi*2.*TMath::ATan(pi/2./5000.*touse);
	  }
	}
        if (inverror>0) 
        {
          significance3d=dist/TMath::Sqrt(inverror);
          significance3d=100./(TMath::Pi()/2.)*TMath::ATan(TMath::Pi()/2./100.*significance3d);
        }
        
	//	}
      } else {
	Float_t px(0.);
	Float_t py(0.);
	Float_t pz(0.);
	Float_t e(0.);
	Float_t dist(0.);
	Float_t inverror(0.);
	for (int u=0;u<t->jetVTXfit_clusternum;++u) {
          if (t->jetVTXfit_dist[u]>0) 
          {
            
	  if (t->jetVTXfit_ntracks[u]==1) {
	    dist+=t->jetVTXfit_dist[u]/t->jetVTXfit_errdist[u]/t->jetVTXfit_errdist[u];
	    inverror+=1./t->jetVTXfit_errdist[u]/t->jetVTXfit_errdist[u];
	    px+=t->jetVTXfit_px[u];
	    py+=t->jetVTXfit_py[u];
	    pz+=t->jetVTXfit_pz[u];
	    e+=TMath::Sqrt(t->jetVTXfit_p[u]*t->jetVTXfit_p[u]+t->jetVTXfit_mass[u]*t->jetVTXfit_mass[u]);
	  } else {
	    //	    throw;
	    std::cout << "ERRORE " << std::endl;
	  }
          }
          
	}
	if (t->jetVTXfit_clusternum>0) {
	  significance3d=dist/TMath::Sqrt(inverror);
	  significance3d=100./(TMath::Pi()/2.)*TMath::ATan(TMath::Pi()/2./100.*significance3d);
	}
	if (e*e-px*px-py*py-pz*pz>0.) {
	  mass=TMath::Sqrt(e*e-px*px-py*py-pz*pz);
	  if (mass>5000.) {
	    Double_t touse=mass-5000.;
	    Double_t pi=TMath::Pi();
	    mass = 5000.+5000./pi*2.*TMath::ATan(pi/2./5000.*touse);
	  }
	}
      }
      
      //      if (nVTX>0) {

      if (nVTX==0) 
      {
        if (nSingleTracks<2) 
        {
          cat_oneVTX=0;
        }
        else
        {
          cat_oneVTX=1;
        }
      }
      else
      {
        if (nSingleTracks+nTracksAtVtx==2) 
        {
          cat_oneVTX=2;
        }
        else if (nSingleTracks+nTracksAtVtx==3)
        {
          cat_oneVTX=3;
        }
        else if (nSingleTracks+nTracksAtVtx>3)
        {
          cat_oneVTX=4;
        }
        else
        {
          std::cout <<" WRONG " << std::endl;
        }
        
      }
      
        


      myTree->Fill();
      //      }
    }
  }
  myTree->Write();
  file->Write();
}
  
