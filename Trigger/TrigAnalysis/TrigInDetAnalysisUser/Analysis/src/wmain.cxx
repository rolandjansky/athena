/**
 **     @file    wmain.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <iostream>

#include <TFile.h>
#include <TTree.h>

using namespace std;

#include "TrigInDetAnalysis/TIDAEvent.h"

#include <map>



int main() 
{


  {
    TIDA::Event* h = new TIDA::Event();
    
    std::cout << "\n\nconstructing ..." << std::endl;

    TFile f("tree.root", "recreate");
    TTree *tree = new TTree("tree","tree");

    //  tree->Branch("Track","Int",&t,6400);
    tree->Branch("TIDA::Event", "TIDA::Event",&h,6400, 1);

    std::string ars[3] = { "ars0", "ars1", "ars2" };

    std::cout << "\n\nfilling ..." << std::endl;
    
    for ( int i=0; i<3 ; i++ ) {
      
      h->clear();
      
      h->run_number(10);
      h->event_number(i);

      std::cout << "\n\nEvent " << i << std::endl;

      for ( int j=0 ; j<2 ; j++ ) { 

	std::cout << "\n\tRoi " << j << std::endl;

	h->addChain( ars[j] );

	TIDA::Chain& tc = h->back();
	//	tc.addCombination();

	//	TrackCombination& c = h->back().back();

	TIDARoiDescriptor roi1( j+1.,  j+1-2.,  j+1+2.,
				j+3.,  j+3-1.1, j+3+1.1, 
				j+2.1, j+2.1-100, j+2.1+100 ); 

	for ( int ij=0 ; ij<j+1 ; ij++ ) roi1.push_back( TIDARoiDescriptor( ij, ij-0.2, ij+0.2,
									    ij, ij-0.2, ij+0.2,
									    ij, ij-0.2, ij+0.2 ) );
	
	std::cout << "Roi " << roi1 << std::endl;

	TIDARoiDescriptor roi2( j+1.,  j+1-2.,  j+1+2.,
				j+3.,  j+3-1.1, j+3+1.1, 
				j+2.1, j+2.1-100, j+2.1+100 ); 

	tc.addRoi( roi1 );
 
	tc.addRoi( roi2 );
	
      }

      cout << "TIDA::Event " << *h << endl;

      tree->Fill();
      
    }
    
    std::cout << "\n\nwriting ..." << std::endl;


    f.Write();
    f.Close();

  }


  {
    std::cout << "\n\nreading ..." << std::endl;

    TIDA::Event* hh = new TIDA::Event();

    TFile f("tree.root");
    TTree *tree = (TTree*)f.Get("tree");
    
    //  tree->Branch("Track","Int",&t,6400);
    tree->SetBranchAddress("TIDA::Event", &hh );
    
    for ( int i=0; i<tree->GetEntries() ; i++ ) {
    
      tree->GetEntry(i);

      cout << "TIDA::Event " << i << " " << *hh << endl;
      
    }
    
    f.Close();
    
  }

  return 0;
}

