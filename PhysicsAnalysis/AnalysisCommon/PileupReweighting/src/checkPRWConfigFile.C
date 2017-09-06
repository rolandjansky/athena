/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "PileupReweighting/TPileupReweighting.h"
#include "TFile.h"
#include "TSystem.h"
#include "TH1D.h"

#include <fstream>

using namespace std;

int main ( int argc, char *argv[] )
{
   if ( argc > 4 || argc < 3 ) { // argc should be 3 or 4 for correct execution
      // We print argv[0] assuming it is the program name
      cout<<"usage: "<< argv[0] <<" <prwConfigFile> <TextFileWithListOfDatasets> [<outputPRWConfigFile>]\n";
      cout<<"For this program to work, you *must* have done localSetupPyAMI and obtained a valid grid certificate\n";
      cout<<"The text file should contain the datasets you made the prw config file from. This program will check your prw file is complete.\n";
      cout<<"If <outputPRWConfigFile> is specified, a prw config file with only the complete channels is output. The channels must be in the list of datasets file too, so this can be used to filter.\n";
      return -1;
   }
   

   CP::TPileupReweighting* out = 0;
   if(argc==4) {
      out = new CP::TPileupReweighting("out");
      out->AddConfigFile(argv[1]);
      out->ResetCountingMode(); //trick tool into going into counting mode 
   }


   std::map<int,TString> datasetNames;

   std::map<int,bool> goodDatasets;

   std::ifstream infile(argv[2]);
   
   std::string str; 
    while (std::getline(infile, str)) {
       TString s(str);
       datasetNames[ TString(s(s.First('.')+1,6)).Atoi() ] = s.ReplaceAll("\r","");
    }

   TFile f2(argv[1]); //"/afs/cern.ch/user/f/fmeloni/workarea/public/PRW25ns/merged_prw.root"
   f2.GetDirectory("PileupReweighting")->cd(); //changes diretory if it exists

   int nComplete(0); int nSkipped(0); int nIncomplete(0); int nSuspect(0);

   TList* l = gDirectory->GetListOfKeys();
   for(int i=0;i<l->GetSize();i++) {
      //find channels and check for completeness 
      TString k = l->At(i)->GetName();
      if(!k.BeginsWith("pileup")) continue;
      TH1D* h = (TH1D*)gDirectory->Get(k);
      //extract channel number..always 6 digits 
      TString chanNum(k(11,6));
      if(datasetNames.find(chanNum.Atoi()) == datasetNames.end()) {
         std::cout << "Skipping channel " << chanNum << " because not in dataset list" << std::endl;
         if(out) out->RemoveChannel(chanNum.Atoi());
         nSkipped++;
         continue;
      }

      std::cout << datasetNames[chanNum.Atoi()] << " : " << std::flush;

      TString result = gSystem->GetFromPipe(TString::Format("ami show dataset info %s | awk '/totalEvents/{ print $3}'",datasetNames[chanNum.Atoi()].Data()));
      int nAmiEvents = result.Atoi();


      if(nAmiEvents == int(h->GetEntries())) {
         std::cout << " Complete (" << nAmiEvents << ")" << std::endl;
         goodDatasets[chanNum.Atoi()] = true;
         nComplete++;
      } else if(nAmiEvents > h->GetEntries()) {
         std::cout << " Incomplete (" << int(h->GetEntries()) << " vs " << nAmiEvents << ")" << std::endl;
         if(out) out->RemoveChannel(chanNum.Atoi());
         nIncomplete++;
      } else if(nAmiEvents < h->GetEntries()) {
         std::cout << " Suspect (" << int(h->GetEntries()) << " vs " << nAmiEvents << ")" << std::endl;
         if(out) out->RemoveChannel(chanNum.Atoi());
         nSuspect++;
      }
   }

   if(argc==4) {
      out->Initialize();
      out->WriteToFile(argv[3]);
      delete out;
   }

   //print a summary 
   std::cout << (nComplete+nSkipped+nIncomplete+nSuspect) << " channels processed: " << std::endl;
   if(nSkipped) std::cout << "  " << nSkipped << " skipped." << std::endl;
   if(nIncomplete) std::cout << "  " << nIncomplete << " incomplete." << std::endl;
   if(nSuspect) std::cout << "  " << nSuspect << " suspect." << std::endl;
   if(nComplete) std::cout << "  " << nComplete << " complete." << std::endl;

   return 0;
}

