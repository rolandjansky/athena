/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Utility program for the ATLAS FTK [patternbankinfo]
 *
 *  give a sector-ordered pattern bank file as argument
 *  print some basic information and produce histograms
 *
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unistd.h>
//#include <TString.h>
//#include <TFile.h>
#include <TH1D.h>
//#include <inttypes.h>

#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKLogging.h"

using namespace std;

int main(int argc, char const *argv[]) {
   FTKLogging logging("main");
   int error=0;
   if(argc<2) {
      cout<<"usage: "<<argv[0]<<" sector-ordered-bank.root\n";
      return 1;
   }
   TDirectory *statFile=0;
   TDirectory *bankFile=FTKRootFile::Instance()->
      OpenRootFileReadonly(argv[1]);
   if(!bankFile) {
      logging.Fatal("openRootFile")<<"can not open: "<<argv[1]<<"\n";
      error++;
   } else {
      logging.Info("openRootFile")<<"file opened: "<<argv[1]<<"\n";
   }
   if(argc>2) {
      statFile=FTKRootFile::Instance()->
         CreateRootFile(argv[2]);
      if(!statFile) {
         logging.Error("createRootFile")<<"failed to create: "<<argv[2]<<"\n";
         error++;
      } else {
         logging.Info("createRootFile")<<"histogram file: "<<argv[2]<<"\n";   
      }
   }
   if(bankFile) {
      FTKPatternBySectorReader bankReader(*bankFile);
      int minSector=-1,maxSector=-1,nSector=0;
      std::map<int,int> s100sector,s100pattern;
      for(int sector=bankReader.GetFirstSector();sector>=0;
          sector=bankReader.GetNextSector(sector)) {
         if((minSector<0)||(sector<minSector)) minSector=sector;
         if((maxSector<0)||(sector>maxSector)) maxSector=sector;
         nSector++;
         int i100=sector/100;
         s100sector[i100]++;
         s100pattern[i100]+=bankReader.GetNPatterns(sector);
      }
      logging.Info("statistics")
         <<"total number of patterns: "<<bankReader.GetNPatterns()<<"\n";
      logging.Info("statistics")
         <<"total number of sectors: "<<nSector
         <<" ["<<minSector<<","<<maxSector<<"]\n";
      if(!s100sector.empty()) {
         int i0=(*s100sector.begin()).first;
         int i1=(*s100sector.rbegin()).first;
         int nBin=(i1-i0+1);
         if(statFile) {
            TH1D *h_s100_count=
               new TH1D("h_s100_count",";Sector;nSector",nBin,
                        i0*100,(i1+1)*100);
            TH1D *h_s100_pattern=
               new TH1D("h_s100_pattern",";Sector;nPattern",nBin,
                        i0*100,(i1+1)*100);
            TH1D *h_s100_patternPerSector=
               new TH1D("h_s100_patternPerSector",
                        ";Sector;nPattern/nSector",nBin,
                        i0*100,(i1+1)*100);
            for(int i100=0;i100<nBin;i100++) {
               int ns=s100sector[i100];
               int np=s100pattern[i100];
               h_s100_count->SetBinContent(i100+1,ns);
               h_s100_pattern->SetBinContent(i100+1,np);
               h_s100_patternPerSector->SetBinContent(i100+1,np/(double)ns);
            }
            h_s100_count->Write();
            h_s100_pattern->Write();
            h_s100_patternPerSector->Write();
            if(bankReader.GetContentType()==
               FTKPatternBySectorReader::CONTENT_NOTMERGED) {
               logging.Warning("contentType")<<"This bank is not merged, coverage details unknown\n";
            } else {
               TH1D *h_coverage100=
                  new TH1D("h_coverage100",";coverage;count",100,0.5,100.5);
               TH1D *h_coverage1000=
                  new TH1D("h_coverage1000",";coverage;count",100,0.5,1000.5);
               TH1D *h_s100_covPattern=
                  new TH1D("h_s100_covPattern",";sector;coverage*npattern",
                           nBin,i0*100,(i1+1)*100);
               std::map<int,std::map<int,int> > scMap;
               bankReader.GetNPatternsBySectorCoverage(scMap);
               uint64_t covPatt=0;
               uint64_t npattTotal=0;
               for(std::map<int,std::map<int,int> >::const_iterator
                      is=scMap.begin();is!=scMap.end();is++) {
                  int sector=(*is).first;
                  for(std::map<int,int>::const_iterator ic=(*is).second.begin();
                      ic!=(*is).second.end();ic++) {
                     int coverage=(*ic).first;
                     int npattern=(*ic).second;
                     h_coverage100->Fill(coverage,npattern);
                     h_coverage1000->Fill(coverage,npattern);
                     h_s100_covPattern->Fill(sector,coverage*npattern);
                     covPatt += coverage*npattern;
                     npattTotal+=npattern;
                  }
               }
               logging.Info("statistics")
                  <<"Total number of patterns*coverage: "<<covPatt
                  <<" average coverage: "<<covPatt/(double)npattTotal<<"\n";
               TH1D *h_s100_coverage=
                  new TH1D("h_s100_coverage",";sector;<coverage>",
                           nBin,i0*100,(i1+1)*100);
               h_s100_coverage->Divide(h_s100_covPattern,h_s100_pattern);
               h_coverage100->Write();
               h_coverage1000->Write();
               h_s100_covPattern->Write();
               h_s100_coverage->Write();
            }
         }
      } else {
         logging.Warning("statistics")
            <<"no pattern data was found (bank is empty)\n";
      }
   }
   if(statFile) delete statFile;
   if(bankFile) delete bankFile;
   return error;
}
