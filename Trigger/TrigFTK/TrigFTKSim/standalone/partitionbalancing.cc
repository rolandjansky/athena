/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Utility program for the ATLAS FTK [partitionbalancing]
 *
 *  read (cached) pattern bank, slices, efficiency plot
 *
 *  write out partition file which can be used as a steering
 *    to equalize the efficiency across eta bins
 *
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <glob.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TMath.h>
#include <TBits.h>

#include "TrigFTKSim/FTK_CompressedAMBank.h"
#include "TrigFTKSim/FTKSteering.h"
#include "TrigFTKSim/FTKLogging.h"
#include "TrigFTKSim/FTKRootFile.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

using namespace std;

class PartitionSteering : public FTKSteering {
public:
   static PartitionSteering *Instance(void) {
      if(!fSteering) {
         fSteering=new PartitionSteering();

         fSteering->AddIntPar("NREG",1,0);
         fSteering->AddIntPar("NSUB",1,0);
         fSteering->AddIntPar("NPATTERN",1,0);

         fSteering->AddIntPar("maxSectorId",1,0);

         fSteering->AddDoublePar("absetarange",2,0.0);

         fSteering->AddDoublePar("etabinOffset",1,0.0);

         fSteering->AddDoublePar("epsilonLimit",1,1.0);
         fSteering->AddDoublePar("weightLimit",1,0.0);

         fSteering->AddStringPar("efficiency_file");
         fSteering->AddStringPar("efficiency_plot");

         fSteering->AddStringPar("pattern_bank_path");
         fSteering->AddStringPar("slices_file_path");

      }
      return fSteering;
   }
   const char *GetEfficiencyFileName(void) const {
      return *(*this)["efficiency_file"];
   }
   const char *GetEfficiencyPlotName(void) const {
      return *(*this)["efficiency_plot"];
   }
   const char *GetPatternBankPath(void) const {
      return *(*this)["pattern_bank_path"];
   }
   const char *GetSlicesFilePath(void) const {
      return *(*this)["slices_file_path"];
   }
   double const GetAbsEtaRangePar(int i) const {
      return (*(*this)["absetarange"])[i];
   }
   double GetEpsilonLimit(void) const {
      return (*this)["epsilonLimit"][0];
   }
   double GetWeightLimit(void) const {
      return (*this)["weightLimit"][0];
   }
   double GetEtaBinOffset(void) const {
       return (*this)["etabinOffset"][0];
   }
   int GetMaxSectorId(void) const {
      return (*this)["maxSectorId"][0];
   }
   int GetNPattern(void) const {
      return (*this)["NPATTERN"][0];
   }
   int GetNSub(void) const {
      return (*this)["NSUB"][0];
   }
   int GetNReg(void) const {
      return (*this)["NREG"][0];
   }
protected:
   static PartitionSteering *fSteering;
};

PartitionSteering *PartitionSteering::fSteering=0;

int main(int argc, char const *argv[]) {
   // ============ read steering ==================
   int error=0;
   istream *steering= &cin;
   FTKLogging logging("main");
   if(argc>1) {
      steering=ftk_dcap::open_for_read(argv[1]);
      if(!steering) {
         logging.Fatal("parse")<<"failed to open steering file \""
                               <<argv[1]<<"\"\n";
         error=3;
	 return error;
      }
   }
   if(PartitionSteering::Instance()->Parse(*steering)<0) {
      logging.Error("parse")<<"errors found in steering\n";
      error=2;
      return error;
   }
   if(steering!= &cin) delete steering;

   // determine number of regions and subregions
   int nreg=PartitionSteering::Instance()->GetNReg();
   int nsub=PartitionSteering::Instance()->GetNSub();

   if(nreg<=0) {
      logging.Error("init")<<"number of regions has to be specified\n";
   }
   if(nsub<=0) {
      logging.Error("init")<<"number of subregions has to be specified\n";
   }
   // read efficiency plot
   TDirectory *efficiencyFile=
      FTKRootFile::Instance()->OpenRootFileReadonly
      (PartitionSteering::Instance()->GetEfficiencyFileName());
   TGraph *efficiencyPlot=0;
   if(efficiencyFile) {
      efficiencyFile->GetObject
         (PartitionSteering::Instance()->GetEfficiencyPlotName(),
          efficiencyPlot);
   } else {
      logging.Fatal("init")
         <<"failed to open efficiency file "
         <<PartitionSteering::Instance()->GetEfficiencyFileName()<<"\n";
   }
   if(!efficiencyPlot) {
      logging.Fatal("init")
         <<"efficiency plot \""
         <<PartitionSteering::Instance()->GetEfficiencyPlotName()
         <<"\"not found\n";
   }
   double absetamin=PartitionSteering::Instance()->GetAbsEtaRangePar(0);
   double absetamax=PartitionSteering::Instance()->GetAbsEtaRangePar(1);
   double etabinOffset=PartitionSteering::Instance()->GetEtaBinOffset();
   int maxSectorId=PartitionSteering::Instance()->GetMaxSectorId();
   cout<<"abs(eta) range: "<<absetamin<<"..."<<absetamax<<"\n";
   cout<<"etabinOffset: "<<etabinOffset<<"\n";
   // loop over all regions
   int totalMaxSector=0;
   int totalMaxSectorUsed=0;
   int totalNsectorTotal=0;
   int totalNsectorUsed=0;
   int totalNsectorUsedBank=0;
   for(int reg=0;reg<nreg;reg++) {
      logging.Info("loop")<<"Processing region "<<reg<<"\n";
      // read slices
      TString sliceFileName=
         PartitionSteering::Instance()->GetSlicesFilePath()+
         TString::Format("/slices_8L_reg%d.root",reg);
      TDirectory *slices=FTKRootFile::Instance()->OpenRootFileReadonly
         (sliceFileName);
      if(slices) {
         TClonesArray *ctheta;
         TString name("c_bits_ctheta");
         slices->GetObject(name,ctheta);
         if(!ctheta) {
            logging.Fatal("loop")
               <<"failed to read cos(theta) slices\n";
         }
         int iPart=0;
         // output file
         TString outputFileName=TString::Format("partition_reg%d.out",reg);
         ostream *outputFile=new ofstream(outputFileName);
         // read pattern bank(s)
         // either read one subregion or read nSub subregions
         map<int,int> patternsPerSector;
         int nPatternRead=0;
         for(int isubRead=0;isubRead<nsub;isubRead++) {
            // read pattern bank(s) and determine number of patterns
            //   per sector
            TString bankName=
               PartitionSteering::Instance()->GetPatternBankPath()+
               TString::Format("/*_reg%d_sub%d.p*.root",reg,isubRead);
            // get proper filename from glob
            glob_t globStruct;
            int r=glob(bankName,0,0,&globStruct);
            if(r) {
               if(isubRead==0) {
                  logging.Fatal("subregion")
                     <<"glob() is not able to resolve the pattern bank name "
                     <<bankName<<"\n";
               } else {
                  logging.Warning("subregion")
                     <<"glob() is not able to resolve the pattern bank name "
                     <<bankName<<", skipping\n";
                  continue;
               }
            }
            if(globStruct.gl_pathc<1) {
               if(isubRead==0) {
                  logging.Fatal("subregion")
                     <<"pattern bank "<<bankName<<" not found\n";
               } else {
                  logging.Warning("subregion")
                     <<"pattern bank "<<bankName<<" not found, skipping\n";
                  continue;
               }
            } else if(globStruct.gl_pathc!=1) {
               logging.Error("loop")
                  <<"pattern bank "<<bankName<<" not unique\n";
            }
            logging.Info("subregion")
               <<"Processing pattern bank "<<globStruct.gl_pathv[0]<<"\n";
            TDirectory *patternBank=
               FTKRootFile::Instance()->OpenRootFileReadonly
               (globStruct.gl_pathv[0]);
            if(!patternBank) {
               logging.Fatal("subregion")<<"can not open pattern bank root file\n";
            }
            //
            // count sector multiplicities
            TTree *tree;
            patternBank->GetObject("Bank",tree);
            if(!tree) {
               logging.Fatal("subregion")<<"can not find TTree \"Bank\"\n";
            }
            int nPattern=tree->GetEntriesFast();
            FTKPattern *pattern=new FTKPattern();
            if(tree->SetBranchAddress("Pattern",&pattern)!=0) {
               // no match, assume this is a flat file
               logging.Info("subregion")<<"try to read flat format\n";
               delete pattern;
               pattern=0;
            }
            TBranch *branch_sectorID=0;
            int sectorID;
            if(!pattern) {
               // flat format
               // acces branches and set addresses
               branch_sectorID=tree->GetBranch("sectorID");
               branch_sectorID->SetAddress(&sectorID);
            } else {
               branch_sectorID=tree->GetBranch("m_sectorID");
            }
            for(int iPattern=0;iPattern<nPattern;++iPattern) {
               branch_sectorID->GetEntry(iPattern);
               if(pattern) {
                  sectorID=pattern->getSectorID();
               }
               patternsPerSector[sectorID]++;
               nPatternRead++;
            }
            globfree(&globStruct);
            logging.Info("subregion")
               <<"read subregion="<<isubRead<<" nPattern="<<nPattern<<"\n";
         }
         logging.Info("readpattern")
            <<"total sectors (bank): "<<patternsPerSector.size()
            <<" total patterns: "<<nPatternRead<<"\n";

         //
         // statistics for one subregion
         int regionMaxSector=0;
         int regionMaxSectorUsed=0;
         int regionNsectorTotal=0;
         int regionNsectorUsed=0;
         int regionNsectorUsedBank=0;
         // analyse patterns per sector in slices of eta
         for(int isub=0;isub<nsub;isub++) {
            // rebalance patterns as a function of the efficiency
            // (1) for each sector, determine the eta slice number 
            int nSlice1=ctheta->GetEntries();
            map<int,double> sum0,sum1;
            for(int i=0;i<nSlice1;i++) {
               TBits *bits=(TBits *)ctheta->At(i);
               //for(map<int,int>::const_iterator is=
               //       patternsPerSector.begin();is!= patternsPerSector.end();
               //    is++) {
               //   int sector=(*is).first;
               for(int sector=0;sector<bits->GetNbits();sector++) {
                  if(sector %nsub !=isub) continue;
                  // skip sector if outside range
                  if(maxSectorId && (sector>=maxSectorId)) continue;
                  if((*bits)[sector]) {
                     sum0[sector]+=1.0;
                     sum1[sector]+=i;
                  }
               }
            }
            //map<int,int> sliceNumber;
            int nSlice2=2*nSlice1;
            vector<int> patternPerSlice(nSlice2);
            vector<vector<int> > sectorList(nSlice2);
            for(map<int,double>::const_iterator is=sum0.begin();
              is!=sum0.end();is++) {
               int sector=(*is).first;
               int slice2=(int)(2.*sum1[sector]/(*is).second);
               //sliceNumber[sector]=slice;
               sectorList[slice2].push_back(sector);
               if(patternsPerSector.find(sector)!=patternsPerSector.end()) {
                  patternPerSlice[slice2]+=patternsPerSector[sector];
               }
            }
            // (2) for each slice determine the efficiency and add up
            //   target number of patterns
            vector<double> targetPatterns(nSlice2);
            double ctheta0=TMath::SinH(-3.),ctheta1=TMath::SinH(3.);
            double total=0.;
            for(int i=0;i<nSlice2;i++) {
               double ctheta=(i+etabinOffset)*(ctheta1-ctheta0)/nSlice2+ctheta0;
               double eta=TMath::ASinH(ctheta);
               double abseta=TMath::Abs(eta);
               double epsilon=efficiencyPlot->Eval(eta);
               double epsilon0=
                  PartitionSteering::Instance()->GetEpsilonLimit();
               if(((absetamin>=absetamax)||
                   ((abseta>=absetamin)&&(abseta<=absetamax)))&&
                  (patternPerSlice[i]>0)) {
                  double weight=PartitionSteering::Instance()
                     ->GetWeightLimit();
                  if(epsilon<epsilon0) {
                     weight=-1./TMath::Log(1.-epsilon/epsilon0);
                     if(!(weight>PartitionSteering::Instance()
                          ->GetWeightLimit())) {
                        weight=PartitionSteering::Instance()->GetWeightLimit();
                     }
                  }
                  targetPatterns[i]+= patternPerSlice[i]*weight;
                  total+= targetPatterns[i];
               }
            }
            // (3) normalize and round
            int tot=0;
            int nPattern=nPatternRead/nsub;
            if(PartitionSteering::Instance()->GetNPattern())
               nPattern=PartitionSteering::Instance()->GetNPattern();
            if(total==0) {
               logging.Warning("subregion")
                  <<"no sectors accepted, store one pattern per partition\n";
               nPattern=nSlice2;
            }
            for(int i=0;i<nSlice2;i++) {
               if(patternPerSlice[i]==0) continue;
               if(total>0) {
                  patternPerSlice[i]=
                     (int)((targetPatterns[i]*nPattern)/total+0.5);
               } else {
                  patternPerSlice[i]=1;
               }
               tot+= patternPerSlice[i];
           }
            while(tot-nPattern) {
               for(int i=0;i<nSlice2;i++) {
                  if(patternPerSlice[i]<=0) continue;
                  int delta=tot-nPattern;
                  if(!delta) break;
                  else if(delta>0) {
                     tot--;
                     patternPerSlice[i]--;
                  } else if(delta<0) {
                     if(patternPerSlice[i]>0) {
                        tot++;
                        patternPerSlice[i]++;
                     }
                  }
               }
            }
            // write out partition file
            int sum=0;
            int nSectorUsed=0;
            int nSectorUsedBank=0;
            int nSliceUsed=0;
            for(int i=0;i<nSlice2;i++) {
               if(sectorList[i].size() && patternPerSlice[i]) {
                  sum += patternPerSlice[i];
                  (*outputFile)<<"PARTITION "<<iPart<<" "<<patternPerSlice[i]
                               <<" "<<sectorList[i].size();
                  for(unsigned j=0;j<sectorList[i].size();j++) {
                     if(!j) (*outputFile)<<"\n";
                     else (*outputFile)<<" ";
                     (*outputFile)<<sectorList[i][j];
                     if(sectorList[i][j]>regionMaxSectorUsed) {
                        regionMaxSectorUsed=sectorList[i][j];
                     }
                     if(patternsPerSector.find(sectorList[i][j])!=patternsPerSector.end()) {
                        nSectorUsedBank++;
                     }
                  }
                  nSectorUsed+=sectorList[i].size();
                  (*outputFile)<<"\n";
                  iPart++;
                  nSliceUsed++;
               }
            }
            int maxSector=(*sum0.rbegin()).first;
            logging.Info("output")
               <<"isub="<<isub<<" #partitions="<<nSliceUsed
               <<" #sectors="<<sum0.size()
               <<" maxSector="<<maxSector
               <<" #selectedSector="<<nSectorUsed
               <<" #patterns="<<sum<<"\n";
            if(sum!=nPattern) {
               logging.Fatal("subregions")<<"this is a bug\n";
            }
            if(maxSector>regionMaxSector) regionMaxSector=maxSector;
            regionNsectorTotal+=sum0.size();
            regionNsectorUsed+=nSectorUsed;
            regionNsectorUsedBank+=nSectorUsedBank;
         }
         logging.Info("output")
            <<"region="<<reg<<" maxSector="<<regionMaxSector<<" maxSectorUsed="<<regionMaxSectorUsed
            <<" nSectorTotal="<<regionNsectorTotal<<" nSectorUsed="<<regionNsectorUsed
            <<" nSectorUsedBank="<<regionNsectorUsedBank<<"\n";
         delete outputFile;
         delete slices;
         if(totalMaxSector<regionMaxSector) totalMaxSector=regionMaxSector;
         if(totalMaxSectorUsed<regionMaxSectorUsed) totalMaxSectorUsed=regionMaxSectorUsed;
         if(totalNsectorTotal<regionNsectorTotal) totalNsectorTotal=regionNsectorTotal;
         if(totalNsectorUsed<regionNsectorUsed) totalNsectorUsed=regionNsectorUsed;
         if(totalNsectorUsedBank<regionNsectorUsedBank) totalNsectorUsedBank=regionNsectorUsedBank;
      } else {
         logging.Fatal("loop")
            <<"failed to open slices file \""<<sliceFileName<<"\"\n";
      }
   }
   logging.Info("output")
      <<"maxSector="<<totalMaxSector<<" maxSectorUsed="<<totalMaxSectorUsed
      <<" nSectorTotal="<<totalNsectorTotal<<" nSectorUsed="<<totalNsectorUsed
      <<" nSectorUsedBank="<<totalNsectorUsedBank<<"\n";
   return 0;
}
