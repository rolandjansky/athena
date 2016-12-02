/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define HITAGprinter_cxx
#include "HITAGprinter.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <map>

std::map< int , std::map< int, int> > runeventmap;

void HITAGprinter::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L HITAGprinter.C
//      Root > HITAGprinter t
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

   FILE *vfile=fopen("HI_vtx.txt","w");
   FILE *ffile=fopen("HI_filter.txt","w");

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0, passed=0,total=0,duplicate=0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;

      MBTSTimeDiff=0;//default in case it's not there
      L1PassedTrigMaskTBP5 = 1<<29;//otherwise no events pass

      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      
      //run event vx vy vz
      if (NVtx<1) {
	//printf("Warning: %d %d NVtx=%d, setting to 0 VtxX,VtxY,VtxZ = %f %f %f\n",RunNumber,EventNumber,NVtx,VtxX,VtxY,VtxZ);
	VtxX=0;VtxY=0;VtxZ=0;
      }

      //check if we already have this event
      if (runeventmap[RunNumber][EventNumber]==7744) {
	++duplicate;
	continue;
      }
      runeventmap[RunNumber][EventNumber]=7744;

      fprintf(vfile,"%d %d %f %f %f\n",RunNumber,EventNumber,VtxX,VtxY,VtxZ);
      fprintf(ffile,"%d %d %d %d %f\n",RunNumber,EventNumber,(L1PassedTrigMaskTBP5>>29)&0x1,NVtx,MBTSTimeDiff);

      ++total;
      if ( (L1PassedTrigMaskTBP5>>29)&0x1 ==1 && NVtx==1 && fabs(MBTSTimeDiff)<3 ) ++passed;
      
   }//Loop over events

   printf("passed %d out of %d events, with %d duplicates skipped\n",passed,total,duplicate);

   fclose(vfile);
   fclose(ffile);

}//Loop()

