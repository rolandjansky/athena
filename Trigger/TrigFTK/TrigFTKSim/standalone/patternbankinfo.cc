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

double HUFbytes(TH1D const *hist);

int main(int argc, char const *argv[]) {
   FTKLogging logging("main");
   FTKLogging::SetAbortLevel(1);
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

   // bank compression analysis
   vector<int> nDataMax;

   int ifilesRead=0;
   for(unsigned ifile=0;ifile<files.size();ifile++) {
      FTKRootFileChain fileChain;
      fileChain.Open(files[ifile].c_str());
      TDirectory *bankFile=fileChain.GetDirectory();
      if(!bankFile) {
         logging.Fatal("openInputFile")<<"can not open: "<<files[ifile]<<"\n";
      } else {
         logging.Info("openInputFile")<<"file opened: "<<files[ifile]<<"\n";
         FTKPatternBySectorReader *bankReader=
            FTKPatternBySectorReader::Factory(fileChain);
         int nSector=0;
         for(int sector=bankReader->GetFirstSector();sector>=0;
             sector=bankReader->GetNextSector(sector)) {
            nSector++;
            int i100=sector/100;
            s100sector[i100]++;
            uint64_t nPattern=bankReader->GetNPatterns(sector);
            s100pattern[i100]+=nPattern;
            s1pattern[sector]+=nPattern;
         }
         logging.Info("openInputFile")<<"nsector="<<nSector<<"\n";
         if(nSector) {
            logging.Info("sectorOrderedFile")
               <<"total number of patterns: "
               <<bankReader->GetNPatternsTotal()<<"\n";
            logging.Info("sectorOrderedFile")
               <<"total number of sectors: "<<nSector
               <<" ["<<(*s1pattern.begin()).first<<","
               <<(*s1pattern.rbegin()).first<<"]\n";
            if(bankReader->GetContentType()==
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
               FTKPatternBySectorReader::SectorByCoverage_t 
                  const &scMap=bankReader->GetSectorByCoverage();
               uint64_t covPatt=0;
               uint64_t npattTotal=0;
               for(FTKPatternBySectorReader::SectorByCoverage_t::const_iterator
                      iCov=scMap.begin();iCov!=scMap.end();iCov++) {
                  int coverage=(*iCov).first;
                  for(FTKPatternBySectorReader::SectorMultiplicity_t
                         ::const_iterator iSector=(*iCov).second.begin();
                      iSector!=(*iCov).second.end();iSector++) {
                     int sector=(*iSector).first;
                     int npattern=(*iSector).second;
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
               logging.Info("stat")<<"non-empty sectors: bank "
                                   <<sectorPatterns.size()
                                   <<" total "<<s1pattern.size()<<"\n";
            }
            // analyze lookup-tables
            for(int layer=0;layer<16;layer++) {
               TString treeName(TString::Format("Layer%d",layer));
               tree=0;
               bankFile->GetObject(treeName,tree);
               if(!tree) break;
               Int_t nData;
               tree->SetBranchAddress("nData",&nData);
               nDataMax.resize(layer+1);
               for(int i=0;i<tree->GetEntries();i++) {
                  tree->GetEntry(i);
                  if(nData>nDataMax[layer]) nDataMax[layer]=nData;
               }
               Int_t nPattern,firstPattern;
               vector<Int_t> data(nDataMax[layer]);
               tree->SetBranchAddress("data",data.data());
               tree->SetBranchAddress("nPattern",&nPattern);
               tree->SetBranchAddress("firstPattern",&firstPattern);
               TH1D *hist_nPattern=0,*hist_nData=0,*hist_iPatt=0;
               TH1D *hist_pattDensity=0, *hist_pattDensity20=0,
                  *hist_iPatt8=0,*hist_patt8Data=0,
                  *hist_pattData=0,*hist_nbits=0;
               int nPatternTotal=0;
               int nDataTotal=0;
               int iPattTotal=0;
               int iPatt8Total=0;
               if(statFile) {
                  statFile->cd();
                  hist_nPattern=new TH1D
                     (TString::Format("hist_nPattern%d",layer),";nPattern",
                      1000,0.5,10000.5);
                  hist_nData=new TH1D
                     (TString::Format("hist_nData%d",layer),";nData",
                      1000,0.5,10000.5);
                  hist_iPatt=new TH1D
                     (TString::Format("hist_iPatt%d",layer),";iPatt",
                      1000,0.5,100000.5);
                  hist_iPatt8=new TH1D
                     (TString::Format("hist_iPatt8%d",layer),";iPatt8",
                      1000,0.5,100000.5);
                  hist_pattDensity=new TH1D
                     (TString::Format("hist_pattDensity%d",layer),
                      ";nPattern/range",100,0.,1.);
                  hist_pattDensity20=new TH1D
                     (TString::Format("hist_pattDensity20_%d",layer),
                      ";nPattern/range (nPattern>=20)",100,0.,1.);
                  hist_pattData=new TH1D
                     (TString::Format("hist_pattData%d",layer),
                     ";data", 256,-0.5,255.5);
                  hist_patt8Data=new TH1D
                     (TString::Format("hist_patt8Data%d",layer),
                      ";data8", 256,-0.5,255.5);
                  hist_nbits=new TH1D
                     (TString::Format("hist_nbits%d",layer),
                      ";data8", 9,-0.5,8.5);
               }
               vector<uint8_t> patterns;
               patterns.reserve(1000000);
               for(int i=0;i<tree->GetEntries();i++) {
                  tree->GetEntry(i);
                  nPatternTotal += nPattern;
                  nDataTotal += nData;
                  // expand data
                  int ipatt=firstPattern & 0x7;
                  int ipatt0=ipatt;
                  patterns.resize(0);
                  patterns.push_back(1<<ipatt);
                  for(int iData=0;iData<nData;) {
                     int d=data[iData++];
                     if(d>=0xf0) {
                        for(int k=0;k<d-0xee;k++) {
                           ipatt ++;
                           size_t pos8=ipatt>>3;
                           if(patterns.size()<=pos8) {
                              patterns.resize(pos8+1);
                           }
                           patterns[pos8] |= 1<<(ipatt &7);
                        }
                     } else {
                        // process one pattern
                        uint32_t delta=1;
                        while(d & 0x80) {
                           // pattern with more than 7 bits
                           //  add non-zero high bits in groups of 4 bits
                           int shift=((d>>2)&0x3c)-25;
                           delta += ((uint32_t)(d&0xf))<<shift;
                           d=data[iData++];
                        }
                        delta += d;
                        ipatt += delta;
                        size_t pos8=ipatt>>3;
                        if(patterns.size()<=pos8) {
                           patterns.resize(pos8+1);
                        }
                        patterns[pos8] |= 1<<(ipatt &7);
                     }
                  }
                  // 
                  int iPatt8=patterns.size();
                  if(statFile) {
                     for(int iData=0;iData<nData;iData++) {
                        hist_pattData->Fill(data[iData]);
                     }
                     for(int k=0;k<iPatt8;k++) {
                        hist_patt8Data->Fill(patterns[k]);
                        int mult=patterns[k];
                        mult=(mult & 0x55)+((mult>>1)&0x55);
                        mult=(mult & 0x33)+((mult>>2)&0x33);
                        mult=(mult & 0xf)+(mult>>4);
                        hist_nbits->Fill(mult);
                     }

                     hist_nPattern->Fill(nPattern);
                     hist_nData->Fill(nData);
                     hist_iPatt->Fill(ipatt);
                     hist_iPatt8->Fill(iPatt8);
                     int range=ipatt-ipatt0+1;
                     hist_pattDensity->Fill((nPattern)/(double)range);
                     if(nPattern>=20) {
                        hist_pattDensity20->Fill((nPattern)/(double)range);
                     }
                  }
                  iPattTotal += ipatt-(firstPattern & 0x7);
                  iPatt8Total += iPatt8;
               }
               logging.Info("compress")
                  <<"layer="<<layer<<" ndataMax="<<nDataMax[layer]
                  <<" nPatternTotal="<<nPatternTotal
                  <<" nDataTotal="<<nDataTotal
                  <<" niPattTotal="<<iPattTotal
                  //<<" fractionNIpatt="<<((double)nPatternTotal)/iPattTotal
                  <<" fractionNpatt="<<((double)nDataTotal)/nPatternTotal
                  <<" fraction8Npatt="<<((double)iPatt8Total)/nPatternTotal
                  <<"\n";
               if(statFile) {
                  hist_nPattern->Write();
                  hist_nData->Write();
                  hist_nbits->Write();
                  hist_iPatt->Write();
                  hist_iPatt8->Write();
                  hist_pattData->Write();
                  hist_patt8Data->Write();
                  hist_pattDensity->Write();
                  hist_pattDensity20->Write();
               }
            }
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
            <<"no output file specified\n";
      }
   }
   if(statFile) delete statFile;
   return 0;
}
