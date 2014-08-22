/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define SCTTimeHists_cxx
#include "SCTTimeHists.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <sstream>
#include <ctime>

void SCTTimeHists::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L SCTTimeHists.C
//      Root > SCTTimeHists t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
  if (fChain == 0) return;

   std::vector<TH1F *> m_phitmapHistoVector;
   

   //The CBNT is not sorted by event number
   //We need to loop over it and get the highest event number
   //Get max and min time
   int theMaximumEventNumber=0;
   double theMaximumTime=0;
   double theMinimumTime=3000000000; 
   Long64_t nentries = fChain->GetEntriesFast();
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      b_event_number->GetEntry(jentry);  
      b_Time->GetEntry(jentry);
      if(event_number>theMaximumEventNumber)theMaximumEventNumber=event_number;
      if((double)Time>theMaximumTime)theMaximumTime=(double)Time;
      if((double)Time<theMinimumTime)theMinimumTime=(double)Time;  
   }
   
   //Here book histograms for each wafer (use an int that runs over wafer hashes)
   int maxWafer=6216;
   for(int theWaferHash=0;theWaferHash<maxWafer ;theWaferHash++){
    
     ostringstream histoname;
     ostringstream histotitle;
     histoname << "Hist_" << theWaferHash;
     histotitle << "Histogram for wafer with hash: " << theWaferHash;
     int nbins = (int)(theMaximumTime-theMinimumTime);
     int firstBin=0;
     int lastBin=(int)(theMaximumTime-theMinimumTime);
     
     double toff=theMinimumTime;
     
     
     TH1F* hitmapHisto_tmp = new TH1F(histoname.str().c_str(),histotitle.str().c_str(),nbins,firstBin,lastBin);    
     hitmapHisto_tmp->GetXaxis()->SetTimeDisplay(1);
     //We need to set the time offset- this chooses the start time on the x axis
     hitmapHisto_tmp->GetXaxis()->SetTimeOffset(toff,"gmt"); 
     //hitmapHisto_tmp->GetXaxis()->SetTimeOffset(0,"gmt");
     //hitmapHisto_tmp->GetXaxis()->SetTimeFormat("#splitline{%H:%M:%S}{%d\/%m\/%y}");
     m_phitmapHistoVector.push_back(hitmapHisto_tmp);
   }
  
   //nentries = fChain->GetEntriesFast();
   //Now loop over the events and fill the histograms
   //Long64_t nbytes = 0, nb = 0;
   std::vector<int> vectorOfDodgyWaferHashes;
   std::vector<TH1F *> m_pstripLevelHistoVector;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      fChain->GetEntry(jentry); 
      int theCurrentEventNumber=event_number;
      Time_t theCurrentTime=Time;
      int counterMax = sct_numHitsInWafer->size();
        
      for(int counter=0;counter!=counterMax;++counter) {
	int theWaferHash = (*sct_waferHash)[counter];
	int theNumberOfHits = (*sct_numHitsInWafer)[counter];
	m_phitmapHistoVector[theWaferHash]->Fill(theCurrentTime-theMinimumTime,theNumberOfHits);
	if(theNumberOfHits>10){
	  //Is this wafer hash of the current module already in the dodgy vector?
	  bool waferHasAlreadyBeenFound=false;
	  for (vector<int>::iterator it = vectorOfDodgyWaferHashes.begin(); it!=vectorOfDodgyWaferHashes.end(); ++it) {
	    if((*it)==theWaferHash){
	      waferHasAlreadyBeenFound=true;
	      continue;
	    }
	  }
	  if(!waferHasAlreadyBeenFound){
	    vectorOfDodgyWaferHashes.push_back(theWaferHash);
	    //Push strip level plot back into strip vector
	    ostringstream stripLevelHistName;
	    stripLevelHistName << "StripHits/" << (*sct_barrelec)[counter] << "_" << (*sct_layer)[counter] << "_" << (*sct_phi)[counter] << "_" << (*sct_eta)[counter] << "_" << (*sct_side)[counter];
	    TH1F* Hist = (TH1F*)gDirectory->Get(stripLevelHistName.str().c_str());
	    m_pstripLevelHistoVector.push_back(Hist);
	  }
	  //cout <<"Found more than 200 hits! In fact, I found " << theNumberOfHits << " hits." << endl;
	  //cout <<"Event number: " << theCurrentEventNumber << "(barrel_ec,layer,side,eta,phi) = (" << (*sct_barrelec)[counter] << "," << (*sct_layer)[counter] << "," << (*sct_side)[counter] << "," << (*sct_eta)[counter] << "," << (*sct_phi)[counter]<<")" << " Wafer Hash: " << theWaferHash<<endl;
	}
	//Set title of histogram to something more intuitive if we've not met this histogram before
	ostringstream theDefaultTitle;
	theDefaultTitle << "Hist_" << theWaferHash;
	if(m_phitmapHistoVector[theWaferHash]->GetTitle()!=theDefaultTitle.str().c_str()){
	  ostringstream theNewTitle;
	  theNewTitle << "(barrel_ec,layer,side,eta,phi) = (" << (*sct_barrelec)[counter] << "," << (*sct_layer)[counter] << "," << (*sct_side)[counter] << "," << (*sct_eta)[counter] << "," << (*sct_phi)[counter]<<")"; 
	  m_phitmapHistoVector[theWaferHash]->SetTitle(theNewTitle.str().c_str());	 
	}
      }
      
      // if (Cut(ientry) < 0) continue;
      
   }//End of loop on events
   //Print out dodgy module hashes
   cout <<"The following wafers had problems:" << endl;
   int i=0;
   for (vector<int>::iterator it = vectorOfDodgyWaferHashes.begin(); it!=vectorOfDodgyWaferHashes.end(); ++it) {
     cout << "Wafer hash: " << (*it) << endl;
     cout <<"Layer vector size: " << (*sct_layer).size() << endl;
     //Get the histogram and save it as a gif
     TCanvas * c1 = new TCanvas;
     m_phitmapHistoVector[(*it)]->Draw();
     ostringstream hist;
     hist << "Hist_" << (*it) << ".gif";
     c1->SaveAs(hist.str().c_str());
     //Get the strip level histogram for this wafer
     m_pstripLevelHistoVector[i]->Draw();
     ostringstream stripOutFile;
     stripOutFile << m_pstripLevelHistoVector[i]->GetName() << ".gif";
     c1->SaveAs(stripOutFile.str().c_str());
     i++;
   }
}
