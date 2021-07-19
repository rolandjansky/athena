/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/




#include <vector>
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TMath.h"
#include "TFile.h"

#include <map>
#include <iostream>
#include <algorithm>

#include "VtxNtuple.C"

using namespace std;

void load(const std::string & input);
void test(const std::vector<std::string> & _files);



std::map<std::string, TH1*> buildStandardPlots(const std::string & suffix) {

  int nBins  = 500;
  int nBinsx = 500;

  
  std::map<std::string, TH1*> output;
  
  TH1I* hI= new TH1I( std::string("hnTracks_" + suffix).c_str(), std::string( "Number of tracks " + suffix).c_str(), 101,-.5,100.5);
  output[hI->GetName()] = hI;
  TH1D* h= new TH1D( std::string("hsumpt_" + suffix).c_str(), std::string( "Sum pt2 " + suffix).c_str(), 200,0,200e3);
  output[h->GetName()] = h;
  
  h= new TH1D( std::string("hchi2_" + suffix).c_str(), std::string( "#chi^{2} " + suffix).c_str(), 200,0,200);
  output[h->GetName()] = h;
  h= new TH1D( std::string("hchi2ndf_" + suffix).c_str(), std::string( "#chi^{2}/ndf " + suffix).c_str(), 200,0,20);
  output[h->GetName()] = h;
  h= new TH1D( std::string("hprob_" + suffix).c_str(), std::string( "Prob(#chi^{2},ndf) " + suffix).c_str(), 200,0,1);
  output[h->GetName()] = h;
  
  h= new TH1D( std::string("hvType_" + suffix).c_str(), std::string( "Vertex Type " + suffix).c_str(), 10,0,10);
  output[h->GetName()] = h;


  h = new TH1D(std::string("hvx_" + suffix).c_str(),"Vertex x-position;x [mm]; Entries",1000,-35,35);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvy_" + suffix).c_str(),"Vertex y-position;y [mm]; Entries",1000,-35,35);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvz_" + suffix).c_str(),"Vertex z-position;z [mm]; Entries",1000,1000,1000);
  output[h->GetName()] = h;
  
  h = new TH1D(std::string("hvEx_" + suffix).c_str(),"Vertex x-position;Uncertainty(x) [mm]; Entries",2000,0,40);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvEy_" + suffix).c_str(),"Vertex y-position;Uncertainty(y) [mm]; Entries",2000,0,40);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvEz_" + suffix).c_str(),"Vertex z-position;Uncertainty(z) [mm]; Entries",2000,0,40);
  output[h->GetName()] = h;
  
  
  h = new TH1D(std::string("hvxPullMC_" + suffix).c_str(),"Vertex Pull x-position wrt MC ;Pull x; Entries",400,-20, 20);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvyPullMC_" + suffix).c_str(),"Vertex Pull y-position wrt MC ;Pull y; Entries",400,-20,20);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvzPullMC_" + suffix).c_str(),"Vertex Pull z-position wrt MC ;Pull z ; Entries",400,-20,20);
  output[h->GetName()] = h;
  
  h = new TH1D(std::string("hvDrhoMC_" + suffix).c_str(),"Vertex #Delta#rho(transverse)-position wrt MC ;Pull x; Entries",1000,0,10);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvDphiMC_" + suffix).c_str(),"Vertex #Delta##phi-position wrt MC ;; Entries",50,-TMath::Pi(),TMath::Pi());
  output[h->GetName()] = h;

  h = new TH1D(std::string("hvDdMC_" + suffix).c_str(),"Vertex #DeltaD-position wrt MC ;; Entries",1000,0, 10);
  output[h->GetName()] = h;

  h = new TH1D(std::string("hvDzMC_" + suffix).c_str(),"Vertex #Deltaz-position wrt MC ;; Entries",1000,-10, 10);
  output[h->GetName()] = h;

  
  
  
  return output;
}
std::map<std::string, TH1*> buildDeltaPlots(const std::string & suffix) {
  std::map<std::string, TH1*> output;
  
  TH1I* hI= new TH1I( std::string("hDnTracks_" + suffix).c_str(), std::string( "#Delta Number of tracks " + suffix).c_str(), 101,-50.5,50.5);
  output[hI->GetName()] = hI;
  TH1D* h= new TH1D( std::string("hDsumpt_" + suffix).c_str(), std::string( "Sum pt #Delta " + suffix).c_str(), 800,-200e3,200e3);
  output[h->GetName()] = h;
  
  
  h = new TH1D(std::string("hvDx_" + suffix).c_str(),"Vertex #Delta x-position;x [mm]; Entries",1000,-10,10);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvDy_" + suffix).c_str(),"Vertex #Delta y-position;y [mm]; Entries",1000,-10,10);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvDz_" + suffix).c_str(),"Vertex #Delta z-position;z [mm]; Entries",1000,-20,20);
  output[h->GetName()] = h;
  
  h = new TH1D(std::string("hvDEx_" + suffix).c_str(),"Vertex #Delta E x-position;Uncertainty(x) [mm]; Entries",1000,-10,10.);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvDEy_" + suffix).c_str(),"Vertex #Delta E y-position;Uncertainty(y) [mm]; Entries",1000,-10,10.);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvDEz_" + suffix).c_str(),"Vertex #Delta E z-position;Uncertainty(z) [mm]; Entries",1000,-15,15);
  output[h->GetName()] = h;
  
  h = new TH1D(std::string("hvDrho_" + suffix).c_str(),"Vertex #Delta rho;x [mm]; Entries",1000,-10,10.);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvDphi_" + suffix).c_str(),"Vertex #Delta #phi position;x [mm]; Entries",1000,-10,10.);
  output[h->GetName()] = h;
  h = new TH1D(std::string("hvDd_" + suffix).c_str(),"Vertex #Delta d-position;y [mm]; Entries",1000,-20,20.);
  output[h->GetName()] = h;

  
  return output;
}




void fillStandardPlots(std::map<std::string, TH1*> & hists,  const std::string & suffix, const MyVertex * vx, const MyMCVertex* mcvx =0) {
  if (!vx) return;
  hists["hnTracks_" + suffix]->Fill( vx->nTracks );
  hists["hsumpt_" + suffix]->Fill( vx->sumPt );

  hists["hvx_" + suffix]->Fill( vx->pos.X() );
  hists["hvy_" + suffix]->Fill( vx->pos.Y() );
  hists["hvz_" + suffix]->Fill( vx->pos.Z() );

  hists["hvEx_" + suffix]->Fill( sqrt(vx->cov[0][0]) );
  hists["hvEy_" + suffix]->Fill( sqrt(vx->cov[1][1]) );
  hists["hvEz_" + suffix]->Fill( sqrt(vx->cov[2][2]) );
  
  
  
  hists["hchi2_" + suffix]->Fill( vx->chi2 );
  hists["hchi2ndf_" + suffix]->Fill( vx->chi2 / vx->ndf );
  hists["hprob_" + suffix]->Fill( TMath::Prob(vx->chi2,vx->ndf) );

  hists["hvType_" + suffix]->Fill( vx->vType );
 
  if ( mcvx) {
    hists["hvxPullMC_" + suffix]->Fill( (vx->pos.X() - mcvx->pos.X())/sqrt(vx->cov[0][0]) );
    hists["hvyPullMC_" + suffix]->Fill( (vx->pos.Y() - mcvx->pos.Y())/sqrt(vx->cov[1][1]) );
    hists["hvzPullMC_" + suffix]->Fill( (vx->pos.Z() - mcvx->pos.Z())/sqrt(vx->cov[2][2]) );
    
    TVector3 diff = vx->pos - mcvx->pos;
    
    hists["hvDrhoMC_" + suffix]->Fill(  diff.Perp() ); 
    hists["hvDphiMC_" + suffix]->Fill(  diff.Phi() ); 
    hists["hvDdMC_"   + suffix]->Fill(  diff.Mag() ); 
    hists["hvDzMC_"   + suffix]->Fill(  diff.Z() ); 
  }
  
}


void fillDeltaPlots(std::map<std::string, TH1*> & hists,  const std::string & suffix, const MyVertex * vx1 , const MyVertex* vx2 ) {
  if (!vx1 || !vx2) return;
  hists["hDnTracks_" + suffix]->Fill( vx1->nTracks - vx2->nTracks );
  hists["hDsumpt_" + suffix]->Fill( vx1->sumPt - vx2->sumPt );
  
  hists["hvDx_" + suffix]->Fill( vx1->pos.X() - vx2->pos.X() );
  hists["hvDy_" + suffix]->Fill( vx1->pos.Y() - vx2->pos.Y() );
  hists["hvDz_" + suffix]->Fill( vx1->pos.Z() - vx2->pos.Z() );
  
  hists["hvDEx_" + suffix]->Fill( sqrt(vx1->cov[0][0]) - sqrt(vx2->cov[0][0]) );
  hists["hvDEy_" + suffix]->Fill( sqrt(vx1->cov[1][1]) - sqrt(vx2->cov[1][1]) );
  hists["hvDEz_" + suffix]->Fill( sqrt(vx1->cov[2][2]) - sqrt(vx2->cov[2][2]) );
  
  TVector3 diff = ( vx1 - vx2);
  
  hists["hvDrho_" + suffix]->Fill(  diff.Perp() ); 
  hists["hvDphi_" + suffix]->Fill(  diff.Phi() ); 
  hists["hvDd_"   + suffix]->Fill(  diff.Mag() ); 
  
  
}


void load(const std::string & input) {
  std::vector<std::string> s;
  s.push_back(input);
  test(s);
  
}

void test(const std::vector<std::string> & _files) {
  
  TChain * ch = new TChain("MinBiasTree"); // to be filled
  for ( std::vector<std::string>::const_iterator it = _files.begin();
       it != _files.end(); ++it) {
    ch->Add(it->c_str());
  }
  VtxNtuple * t = new VtxNtuple(ch);  
  
  
  // disable track branches
  bool disableTracks(true);
  if (disableTracks) {
    int n = t->fChain->GetListOfBranches()->GetSize();
    for ( int i =0; i < n; ++i) {
      std::string name = t->fChain->GetListOfBranches()->At(i)->GetName();
      if (name.find_first_of("trk_") == 0 ) {
        // cout << "Found: " << name.find_first_of("trk_")  << endl;
        t->fChain->SetBranchStatus(name.c_str(), 0);
      }
    }
  } // disable tracks
  
  
  Event evt;
  MCVertices mcvx;
  Vertices v; // standard vertices
  Vertices vRes, vPix, vSCT;
  
  std::vector< std::map<std::string, TH1*> > toWrite;
  
  std::map<std::string, TH1*> usual         = buildStandardPlots("AllDefault");
  std::map<std::string, TH1*> allPix        = buildStandardPlots("AllPix");
  std::map<std::string, TH1*> allSCT        = buildStandardPlots("AllSCT");
  std::map<std::string, TH1*> allRes        = buildStandardPlots("AllRes");
  toWrite.push_back( usual );
  toWrite.push_back( allPix );
  toWrite.push_back( allSCT );
  toWrite.push_back( allRes );
  
  std::map<std::string, TH1*> usualMatched = buildStandardPlots("MatchedDefault");
  std::map<std::string, TH1*> resMatched = buildStandardPlots("resMatched");
  std::map<std::string, TH1*> pixMatched = buildStandardPlots("pixMatched");
  std::map<std::string, TH1*> sctMatched = buildStandardPlots("sctMatched");
  toWrite.push_back( usualMatched );
  toWrite.push_back( resMatched );
  toWrite.push_back( pixMatched );
  toWrite.push_back( sctMatched );
  
  
  std::map<std::string, TH1*> resMatchedComb = buildDeltaPlots("resMatchedComb");
  std::map<std::string, TH1*> pixMatchedComb = buildDeltaPlots("pixMatchedComb");
  std::map<std::string, TH1*> sctMatchedComb = buildDeltaPlots("sctMatchedComb");
  std::map<std::string, TH1*> sctMatchedPix  = buildDeltaPlots("sctMatchedPix");
  toWrite.push_back( resMatchedComb );
  toWrite.push_back( pixMatchedComb );
  toWrite.push_back( sctMatchedComb );
  toWrite.push_back( sctMatchedPix );
  
  
  /*
  std::map<std::string, TH1*> deltaCombResMatched = buildDeltaPlots("DeltaCombRes");
  std::map<std::string, TH1*> deltaCombPixMatched = buildDeltaPlots("deltaCombPixMatched");
  std::map<std::string, TH1*> deltaCombSCTMatched = buildDeltaPlots("deltaCombSCTMatched");
  std::map<std::string, TH1*> deltaPixSCTMatched  = buildDeltaPlots("deltaPixSCTMatched");
*/
  
  Long64_t nentries = t->fChain->GetEntriesFast();
  
  Long64_t nbytes = 0, nb = 0;
  Long64_t jentry=0;
  for (; jentry<nentries;jentry++) {
    Long64_t ientry = t->LoadTree(jentry);
    if (ientry < 0) break;
    nb = t->fChain->GetEntry(jentry);   nbytes += nb;
    if ( jentry %10000 == 0 ) cout << jentry << endl;
    
    // if (Cut(ientry) < 0) continue;
    evt.fill((SEvent*)&t->ei_RunNumber);
    mcvx.fill((SMCVertex*)&t->mcVx_n);
    v.fill((SVertex*)&t->vx_n);
    vRes.fill((SVertex*)&t->vxSi_n);
    vPix.fill((SVertex*)&t->vxPix_n);
    vSCT.fill((SVertex*)&t->vxSCT_n);
    
    
    
    std::map<const MyVertex*,const MyVertex*>    matchedCombPix  = matchVertices( v,    vPix);
    std::map<const MyVertex*,const MyVertex*>    matchedCombSCT  = matchVertices( v,    vSCT);
    std::map<const MyVertex*,const MyVertex*>    matchedPixSCT   = matchVertices( vPix, vSCT);
    std::map<const MyVertex*,const MyVertex*>    matchedCombRes      = matchVertices( v,    vRes);

    
    std::map<const MyMCVertex*,const MyVertex*>  matched        = matchVertices( mcvx, v);
    std::map<const MyMCVertex*,const MyVertex*>  matchedResMC   = matchVertices( mcvx, vRes);
    std::map<const MyMCVertex*,const MyVertex*>  matchedPixMC   = matchVertices( mcvx, vPix);
    std::map<const MyMCVertex*,const MyVertex*>  matchedSCTMC   = matchVertices( mcvx, vSCT);

    
    
    // fill the standard histograms
    for (std::vector<MyVertex>::const_iterator it = v.vertices.begin(); it != v.vertices.end(); ++it) {
      fillStandardPlots(usual, "AllDefault", &(*it), 0);
    }
    for (std::vector<MyVertex>::const_iterator it = vPix.vertices.begin(); it != vPix.vertices.end(); ++it) {
      fillStandardPlots(allPix, "AllPix", &(*it), 0);
    }
    for (std::vector<MyVertex>::const_iterator it = vSCT.vertices.begin(); it != vSCT.vertices.end(); ++it) {
      fillStandardPlots(allSCT, "AllSCT", &(*it), 0);
    }
    for (std::vector<MyVertex>::const_iterator it = vRes.vertices.begin(); it != vRes.vertices.end(); ++it) {
      fillStandardPlots(allRes, "AllRes", &(*it), 0);
    }
    
    
    for (std::map<const MyMCVertex*,const MyVertex*>::const_iterator it =  matched.begin(); it != matched.end(); ++it) {
      fillStandardPlots(usualMatched, "MatchedDefault", it->second, it->first);
    }
    for (std::map<const MyMCVertex*,const MyVertex*>::const_iterator it =  matchedResMC.begin(); it != matchedResMC.end(); ++it) {
      fillStandardPlots(resMatched, "resMatched", it->second, it->first);
    }
    for (std::map<const MyMCVertex*,const MyVertex*>::const_iterator it =  matchedPixMC.begin(); it != matchedPixMC.end(); ++it) {
      fillStandardPlots(pixMatched, "pixMatched", it->second, it->first);
    }
    for (std::map<const MyMCVertex*,const MyVertex*>::const_iterator it =  matchedSCTMC.begin(); it != matchedSCTMC.end(); ++it) {
      fillStandardPlots(sctMatched, "sctMatched", it->second, it->first);
    }
    
    
    for (std::map<const MyVertex*,const MyVertex*>::const_iterator it =  matchedCombPix.begin(); it != matchedCombPix.end(); ++it) {
      fillDeltaPlots(pixMatchedComb, "pixMatchedComb", it->first, it->second);
    }
    for (std::map<const MyVertex*,const MyVertex*>::const_iterator it =  matchedCombSCT.begin(); it != matchedCombSCT.end(); ++it) {
      fillDeltaPlots(sctMatchedComb, "sctMatchedComb", it->first, it->second);
    }
    for (std::map<const MyVertex*,const MyVertex*>::const_iterator it =  matchedPixSCT.begin(); it != matchedPixSCT.end(); ++it) {
      fillDeltaPlots(sctMatchedPix, "sctMatchedPix", it->first, it->second);
    }
    for (std::map<const MyVertex*,const MyVertex*>::const_iterator it =  matchedCombRes.begin(); it != matchedCombRes.end(); ++it) {
      fillDeltaPlots(resMatchedComb, "resMatchedComb", it->first, it->second);
    }
    
    
    /*
    // delta plots
    for (std::map<const MyVertex*,const MyVertex*>::const_iterator it =  matchedRes.begin(); it != matchedRes.end(); ++it) {
      fillDeltaPlots(deltaCombResMatched,  "DeltaCombRes", it->first , it->second  );
    }
    */
    
    
  }// loop over events
  cout << "Finished loop over: " << jentry << "  entries" << endl;
  
  //output the hists
  
  TFile * f = new TFile("vtxOuput.root","recreate");
  f->cd();
  
  for ( std::vector< std::map<std::string, TH1*> >::iterator it =  toWrite.begin(); it != toWrite.end(); ++it) {
    for ( std::map<std::string, TH1*>::iterator jt = it->begin(); jt != it->end(); ++jt) {
      if (!jt->second) continue;
      jt->second->Write();
    }
    
  }

  
}

int main(int argc, char* argv[] ) {
  // simple input a list of files
  
  std::vector<std::string> files;
  
  for ( int i=1; i < argc; ++i) {
    std::string n(argv[i]);
    cout << "Adding file: " << n << endl;
    files.push_back(n);
  }
  cout << endl;
  
  test(files);
  
  
  return 0;
}


