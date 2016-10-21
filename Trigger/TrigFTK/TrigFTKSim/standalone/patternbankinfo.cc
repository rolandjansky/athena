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
#include "TrigFTKSim/FTKPattern.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

using namespace std;

int main(int argc, char const *argv[]) {
   FTKLogging logging("main");
   std::string output;
   std::vector<std::string> files;
   try {
    std::string appName = boost::filesystem::basename(argv[0]);
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
       ("help,h", "Print this help message")
       ("output,o",po::value<std::string>(&output),
        "Name of output root file for histograms")
       ("files", po::value<std::vector<std::string> >(),
        "Input files with pattern banks (sector,pcache,ccache)")
       ;
    po::positional_options_description p;
    p.add("files", -1);

    po::variables_map vm;

    po::store(po::command_line_parser(argc, argv).options(desc)
              .positional(p).run(), vm);
    if (vm.count("help")) {
       cout << desc << "\n";
       return 1;
    }
    po::notify(vm);

    files=vm["files"].as< vector<string> >();

   } catch (std::exception& e) {
      logging.Error("parse_arguments")<< e.what()<<"\n";
      return 1;
   }
   TDirectory *statFile=FTKRootFile::Instance()->
      CreateRootFile(output.c_str());
   std::map<int,uint64_t> s100sector,s100pattern,s1pattern;
   int ifilesRead=0;
   for(unsigned ifile=0;ifile<files.size();ifile++) {
      TDirectory *bankFile=FTKRootFile::Instance()->
         OpenRootFileReadonly(files[ifile].c_str());
      if(!bankFile) {
         logging.Fatal("openInputFile")<<"can not open: "<<files[ifile]<<"\n";
      } else {
         logging.Info("openInputFile")<<"file opened: "<<files[ifile]<<"\n";
         FTKPatternBySectorReader bankReader(*bankFile);
         int nSector=0;
         for(int sector=bankReader.GetFirstSector();sector>=0;
             sector=bankReader.GetNextSector(sector)) {
            nSector++;
            int i100=sector/100;
            s100sector[i100]++;
            uint64_t nPattern=bankReader.GetNPatterns(sector);
            s100pattern[i100]+=nPattern;
            s1pattern[sector]+=nPattern;
         }
         if(nSector) {
            logging.Info("sectorOrderedFile")
               <<"total number of patterns: "<<bankReader.GetNPatterns()<<"\n";
            logging.Info("sectorOrderedFile")
               <<"total number of sectors: "<<nSector
               <<" ["<<(*s1pattern.begin()).first<<","
               <<(*s1pattern.rbegin()).first<<"]\n";
            if(bankReader.GetContentType()==
               FTKPatternBySectorReader::CONTENT_NOTMERGED) {
               logging.Warning("sectorOrderedFile")
                  <<"This bank is not merged, coverage details unknown\n";
            } else if(ifilesRead>0) {
               if(ifilesRead==1)
                  logging.Warning("sectorOrderedFile")
                     <<"Coverage histograms are determined "
                     "from the first input file alone\n";
            } else {
               statFile->cd();
               // not a sector ordered bank, read cached bank
               int i100e=(*s100sector.rbegin()).first;
               int nBin100=(i100e+1);
               TH1D *h_coverage100=
                  new TH1D("h_coverage100",";coverage;count",100,0.5,100.5);
               TH1D *h_coverage1000=
                  new TH1D("h_coverage1000",";coverage;count",100,0.5,1000.5);
               TH1D *h_s100_covPattern=
                  new TH1D("h_s100_covPattern",";sector;coverage*npattern",
                           nBin100,0,nBin100*100.);
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
                           nBin100,0.,nBin100*100.);
               for(int i100=0;i100<nBin100;i100++) {
                  int cn=h_s100_covPattern->GetBinContent(i100);
                  double n=s100pattern[i100];
                  if(n>0.) {
                     h_s100_coverage->SetBinContent(i100,cn/n);
                  }
               }
               h_coverage100->Write();
               h_coverage1000->Write();
               h_s100_covPattern->Write();
               h_s100_coverage->Write();
            }
         } else {
            TTree *tree;
            // try to read pcached bank:
            // "Bank" tree is a list of FTKPattern objects
            bankFile->GetObject("Bank",tree);
            if(tree) {
               // pcached bank
               int nPattern=tree->GetEntriesFast();
               logging.Info("readPCachedBank")
                  <<"\""<<bankFile->GetName()<<"\" number of patterns: "
                  <<nPattern<<"\n";
               FTKPattern *pattern=new FTKPattern();
               if(tree->SetBranchAddress("Pattern",&pattern)!=0) {
                  // no match, assume this is a flat file
                  logging.Info("readPCachedBank")<<"try to read flat format\n";
                  delete pattern;
                  pattern=0;
               }
               TBranch *branch_sectorID;
               int sectorID;
               if(!pattern) {
                  branch_sectorID=tree->GetBranch("sectorID");
                  branch_sectorID->SetAddress(&sectorID);
               } else {
                  branch_sectorID=tree->GetBranch("m_sectorID");
               }
               map<int,int>sectorPatterns;
               for(int iPattern=0;iPattern<nPattern;++iPattern) {
                  branch_sectorID->GetEntry(iPattern);
                  if(pattern) sectorID=pattern->getSectorID();
                  int i100=sectorID/100;
                  s100pattern[i100]++;
                  uint64_t &npatt=s1pattern[sectorID];
                  if(!npatt) s100sector[i100]++; // count sector only once
                  sectorPatterns[sectorID]++;
                  npatt++;
               }
               logging.Info("stat")<<"non-empty sectors: "<<sectorPatterns.size()<<"\n";
            }
            delete bankFile;
         }
         ifilesRead++;
      }
   }
   if(!s1pattern.empty()) {
      if(statFile) {
         statFile->cd();
         int i100e=(*s100sector.rbegin()).first;
         int nBin100=(i100e+1);
         TH1D *h_s100_count=
            new TH1D("h_s100_count",";Sector;nSector",nBin100,
                     0.,nBin100*100);
         TH1D *h_s100_pattern=
            new TH1D("h_s100_pattern",";Sector;nPattern",nBin100,
                     0.,nBin100*100);
         TH1D *h_s100_patternPerSector=
            new TH1D("h_s100_patternPerSector",
                     ";Sector;nPattern/nSector",nBin100,
                     0.,nBin100*100);
         for(int i100=0;i100<nBin100;i100++) {
            int ns=s100sector[i100];
            int np=s100pattern[i100];
            h_s100_count->SetBinContent(i100+1,ns);
            h_s100_pattern->SetBinContent(i100+1,np);
            h_s100_patternPerSector->SetBinContent(i100+1,np/(double)ns);
         }
         h_s100_count->Write();
         h_s100_pattern->Write();
         h_s100_patternPerSector->Write();
      } else {
         logging.Warning("statistics")
            <<"no pattern data was found (bank is empty)\n";
      }
   }
   if(statFile) delete statFile;
   return 0;
}
