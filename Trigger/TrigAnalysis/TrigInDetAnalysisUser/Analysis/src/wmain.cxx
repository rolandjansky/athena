
#include <iostream>

#include <TFile.h>
#include <TTree.h>

using namespace std;

#include "TrigInDetAnalysis/TrackEvent.h"

#include <map>



int main() 
{


  {
    TrackEvent* h = new TrackEvent();
    
    std::cout << "\n\nwriting ..." << std::endl;

    TFile f("tree.root", "recreate");
    TTree *tree = new TTree("tree","tree");
    
    //  tree->Branch("Track","Int",&t,6400);
    tree->Branch("TrackEvent", "TrackEvent",&h,6400, 1);
    
    std::string ars[3] = { "ars0", "ars1", "ars2" };
    
    for ( int i=0; i<3 ; i++ ) {
      
      h->clear();
      
      h->run_number(10);
      h->event_number(i);
      
      for ( int j=0 ; j<2 ; j++ ) { 
	h->addChain( ars[j] );
	TrackChain& tc = h->back();
	//	tc.addCombination();

	//	TrackCombination& c = h->back().back();

	tc.addRoi( TIDARoiDescriptor( j+1., j+2., j+3., j+1.1, j+2.1, j+3.1) );
	tc.addRoi( TIDARoiDescriptor( i+j+2., i+j+3., i+j+4., i+j+1.2, i+j+2.2, i+j+3.2) );
	
      }

      //    cout << "TrackEvent " << *h << endl;

      tree->Fill();
      
      cout << "TrackEvent " << *h << endl;
    
    }
    
    f.Write();
    f.Close();

  }


  {
    std::cout << "\n\nreading ..." << std::endl;

    TrackEvent* hh = new TrackEvent();

    TFile f("tree.root");
    TTree *tree = (TTree*)f.Get("tree");
    
    //  tree->Branch("Track","Int",&t,6400);
    tree->SetBranchAddress("TrackEvent", &hh );
    
    for ( int i=0; i<tree->GetEntries() ; i++ ) {
    
      tree->GetEntry(i);

      cout << "TrackEvent " << i << " " << *hh << endl;
      
    }
    
    f.Close();
    
  }

  return 0;
}

