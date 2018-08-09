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
 * in the partition file, each partition corresponds to a list of sectors
 *   together with a maximum pattern count
 *   
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
#include <TH2D.h>
#include <TH1D.h>
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
         fSteering->AddIntPar("rebinSlices",1,0);

         fSteering->AddDoublePar("absetarange",2,0.0);

         fSteering->AddDoublePar("etabinOffset",1,0.0);

         fSteering->AddDoublePar("epsilonLimit",1,1.0);
         fSteering->AddDoublePar("weightLimit",1,0.0);

         fSteering->AddStringPar("efficiency_file");
         fSteering->AddStringPar("efficiency_plot");

         fSteering->AddStringPar("pattern_bank_path");
         fSteering->AddStringPar("slices_file_path");

         fSteering->AddStringPar("maximum_input_file");
         fSteering->AddStringPar("maximum_output_file");

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
   const char *GetMaximumInputFile(void) const {
      return *(*this)["maximum_input_file"];
   }
   const char *GetMaximumOutputFile(void) const {
      return *(*this)["maximum_output_file"];
   }
   double GetAbsEtaRangePar(int i) const {
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
   int GetRebinSlices(void) const {
      return (*this)["rebinSlices"][0];
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
   // here one can restrict the range in eta
   //  sectors outside the eta range are discarded
   //  -> they will not be written to the partition file
   double absetamin=PartitionSteering::Instance()->GetAbsEtaRangePar(0);
   double absetamax=PartitionSteering::Instance()->GetAbsEtaRangePar(1);
   // this offset is there to adjust the slices numbering
   //  to the bins of the eta efficiency plot
   double etabinOffset=PartitionSteering::Instance()->GetEtaBinOffset();
   // this is a limit on the maximum allowed sector ID
   //   -> sectorIDs >= max will not be written to the partition file
   int maxSectorId=PartitionSteering::Instance()->GetMaxSectorId();
   //
   // this combines several half-slices into one bin
   int rebinSlices=PartitionSteering::Instance()->GetRebinSlices();

   cout<<"abs(eta) range: "<<absetamin<<"..."<<absetamax<<"\n";
   cout<<"etabinOffset: "<<etabinOffset<<"\n";

   // loop over all regions
   int totalMaxSector=0;
   int totalMaxSectorUsed=0;
   int totalNsectorTotal=0;
   int totalNsectorUsed=0;
   int totalNsectorUsedBank=0;

   // write output file 
   TFile *outputRoot=new TFile("bank_statistics.root","RECREATE");
   TH2D *originalPerSlice2(0),*partitionedPerSlice2(0);
   TH1D *etaPerSlice2(0),*cosThetaPerSlice2(0);

   vector<map<int,int> > knownMax(nreg);
   char const *maximumInputFile=PartitionSteering::Instance()->GetMaximumInputFile();
   if(maximumInputFile) {
      ifstream in(maximumInputFile);
      int line=1;
      while(!in.eof()) {
         int ireg,sector,maximum;
         in>>ireg>>sector>>maximum;
         if(in.fail()) break;
         if((ireg<0)||(ireg>=(int)knownMax.size())) {
            logging.Error("maximumFromFile")<<"bad region="<<ireg<<" at line="<<line<<"\n";
            break;
         }
         if(knownMax[ireg].find(sector)!=knownMax[ireg].end()) {
            logging.Error("maximumFromFile")<<"duplicate sector="<<sector<<" in region="<<ireg<<" at line="<<line<<"\n";
            break;
         }
         if(maximum<=0) {
            logging.Error("maximumFromFile")<<"max maximum="<<maximum<<" in sector="<<sector<<" region="<<ireg<<" at line="<<line<<"\n";
            break;
         }
         knownMax[ireg][sector]=maximum;
         line++;
      }
   }

   ofstream *dumpMaximum=0;
   char const *maximumOutputFile= PartitionSteering::Instance()->GetMaximumOutputFile();
   if(maximumOutputFile) {
      dumpMaximum=new ofstream(maximumOutputFile);
   }

   for(int reg=0;reg<nreg;reg++) {

      struct SectorPatternData {
         int fNPattern;
         int fNMax;
      };

      logging.Info("loop")<<"Processing region "<<reg<<" known max for "<<knownMax[reg].size()<<" sector(s)\n";
      // read slices
      TString sliceFileName=
         PartitionSteering::Instance()->GetSlicesFilePath()+
         TString::Format("/slices_8L_reg%d.root",reg);
      TDirectory *slices=FTKRootFile::Instance()->OpenRootFileReadonly
         (sliceFileName);
      if(slices) {
         // ctheta contains for each slice in cos(theta) a bitvector
         //  the bits correspond to sector IDs
         //  if the bit is on, the sector may have patterns
         //  in the given cos(theta) bin
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

         // this variable will contain the number of patterns per sector
         //   first : patterns per sector
         //   second :  1 if at limit, -1 or 0 otherwise
         map<int,SectorPatternData> patternsPerSector;

         // this variable will contain the total number of patterns
         int nPatternRead=0;
         for(int isubRead=0;isubRead<nsub;isubRead++) {
            // read pattern bank(s) and determine number of patterns
            //   per sector
            int nPatternSub=0;
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
            // for the given bank, read the sectorID of all patterns
            // and count the (original) number of patterns per sector ID
            TTree *tree;
            patternBank->GetObject("Sector",tree);
            if(tree) {
               int sector,first,last,allPatternsUsed;
               tree->SetBranchAddress("sector",&sector);
               tree->SetBranchAddress("first",&first);
               tree->SetBranchAddress("last",&last);
               tree->SetBranchAddress("allPatternsUsed",&allPatternsUsed);
               for(int i=0;i<tree->GetEntries();i++) {
                  allPatternsUsed=0;
                  tree->GetEntry(i);
                  int n=last+1-first;
                  for(int special=((first-1) & 0xfffe0000)+0x20000;special<=last;special += 0x20000) {
                     n--; // special patterns are not counted
                  }
                  SectorPatternData &pps=patternsPerSector[sector];
                  pps.fNPattern = n;
                  pps.fNMax = (allPatternsUsed==1) ? n : PartitionSteering::Instance()->GetNPattern();
                  nPatternSub+=n;                  
               }
            } else {
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
                  // access branches and set addresses
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
                  patternsPerSector[sectorID].fNPattern++;
                  patternsPerSector[sectorID].fNMax=PartitionSteering::Instance()->GetNPattern();
                  nPatternSub++;
               }
            }
            globfree(&globStruct);
            logging.Info("subregion")
               <<"read subregion="<<isubRead<<" nPattern="<<nPatternSub<<"\n";
            nPatternRead += nPatternSub;
         }
         logging.Info("readpattern")
            <<"total sectors (bank): "<<patternsPerSector.size()
            <<" total patterns: "<<nPatternRead<<"\n";

         // add information abount known maxima
         for(map<int,int>::const_iterator i=knownMax[reg].begin();
             i!=knownMax[reg].end();i++) {
            map<int,SectorPatternData>::iterator
               i2=patternsPerSector.find((*i).first);
            if(i2==patternsPerSector.end()) {
               SectorPatternData &pps=patternsPerSector[(*i).first];
               pps.fNPattern=0;
               pps.fNMax=(*i).second;
            } else if((*i2).second.fNMax==
                      PartitionSteering::Instance()->GetNPattern()) {
               logging.Info("setMaximum")
                  <<"reg="<<reg<<" sector="<<(*i2).first
                  <<" fMax "<<(*i2).second.fNMax
                  <<" -> "<<(*i).second<<"\n";
               (*i2).second.fNMax=(*i).second;
            } else if((*i2).second.fNMax!=(*i).second) {
               logging.Error("setMaximum")
                  <<"reg="<<reg<<" sector="<<(*i2).first
                  <<" fMax="<<(*i2).second.fNMax
                  <<" fMax(iteration-1)="<<(*i).second<<"\n";
            }
         }

         // dump information about maximum per sector
         if(dumpMaximum) {
            for(map<int,SectorPatternData>::const_iterator i=patternsPerSector.begin();
                i!=patternsPerSector.end();i++) {
               if((*i).second.fNMax<
                  PartitionSteering::Instance()->GetNPattern()) {
                  (*dumpMaximum)<<reg<<" "<<(*i).first
                                <<" "<<(*i).second.fNMax<<"\n";
               }
            }
         }

         // here we have:
         //  ctheta[] : sectors per cos(theta) slice
         //                        .
         //  patternsPerSector[] : number of patterns per sector
         //                        .size() is the number of sectors
         //  nPatternRead : total number of patterns
         
         //
         // statistics for one subregion
         int regionMaxSector=0;
         int regionMaxSectorUsed=0;
         int regionNsectorTotal=0;
         int regionNsectorUsed=0;
         int regionNsectorUsedBank=0;

         // there will be nsub groups of partitions
         // (corresponding to the LAMB boards)
         for(int isub=0;isub<nsub;isub++) {
            // analyse patterns per sector in slices of eta
            // rebalance patterns as a function of the efficiency
            // (1) for each sector, determine the eta slice number 
            int nSlice1=ctheta->GetEntries();
            map<int,double> sum0,sum1;
            for(int i=0;i<nSlice1;i++) {
               TBits *bits=(TBits *)ctheta->At(i);
               for(size_t sector=0;sector<bits->GetNbits();sector++) {
		 if((int)sector %nsub !=isub) continue;
                  // skip sector if outside range
		 if(maxSectorId && ((int)sector>=maxSectorId)) continue;
                  if((*bits)[sector]) {
                     sum0[sector]+=1.0;
                     sum1[sector]+=i;
                  }
               }
            }
            // here:
            //    sum0[sector] -> number of slices for this sector
            //    sum1[sector] -> weighted sum of slices (weight=slice number)
            // -> the average position of a sector is given by:
            //       sum1[sector]/sum0[sector]
            // This position is measured in units of slice number
            // it can take half-integer numbers: 0 0.5 1 1.5 2 2.5 ...

            // This sets the number of partitions along eta
            //   there is one partition for each possible half-integer number
            //   empty partitions will be skipped.

            int nSliceTimes2=2*nSlice1;
            int nSliceRebin=nSliceTimes2/(rebinSlices>0 ? rebinSlices : 1);
            if((rebinSlices>0)&&(nSliceTimes2 % rebinSlices)) {
               nSliceRebin++;
            }

            if(!originalPerSlice2) {
               outputRoot->cd();
               originalPerSlice2=new TH2D("originalPerSlice2",
                                           ";slice;tower",
                                           nSliceRebin,-0.5,nSliceRebin-0.5,
                                           nreg,-0.5,nreg-0.5);
               partitionedPerSlice2=new TH2D("partitionedPerSlice2",
                                              ";slice;tower",
                                              nSliceRebin,-0.5,nSliceRebin-0.5,
                                              nreg,-0.5,nreg-0.5);
               etaPerSlice2=new TH1D("etaPerSlice2",";slice;eta",
                                     nSliceRebin,-0.5,nSliceRebin-0.5);
               cosThetaPerSlice2=new TH1D("cosThetaPerSlice2",
                                          ";slice;cos(theta)",
                                          nSliceRebin,-0.5,nSliceRebin-0.5);
            }
            // next, each sector is assigned to one partition "nslice2"
            // first : number of patterns
            // second : if zero, all sectors are at limit
            vector<SectorPatternData > patternPerSlice(nSliceRebin);
            vector<vector<int> > sectorList(nSliceRebin);
            for(map<int,double>::const_iterator is=sum0.begin();
              is!=sum0.end();is++) {
               int sector=(*is).first;
               int sliceRebin=
                  ((int)(2.*sum1[sector]/(*is).second))/
                  (rebinSlices>0 ? rebinSlices : 0);
               if(sliceRebin>=nSliceRebin) sliceRebin=nSliceRebin-1;
               //sliceNumber[sector]=slice;
               sectorList[sliceRebin].push_back(sector);
               if(patternsPerSector.find(sector)!=patternsPerSector.end()) {
                  SectorPatternData &ppSlice=patternPerSlice[sliceRebin];
                  SectorPatternData &ppSector=patternsPerSector[sector];
                  ppSlice.fNPattern += ppSector.fNPattern;
                  ppSlice.fNMax += ppSector.fNMax;
                  if(ppSlice.fNMax>PartitionSteering::Instance()->GetNPattern()) {
                     ppSlice.fNMax=PartitionSteering::Instance()->GetNPattern();
                  }
               }
            }
            for(int i=0;i<nSliceRebin;i++) {
               if((patternPerSlice[i].fNMax>0)&&(patternPerSlice[i].fNMax<PartitionSteering::Instance()->GetNPattern())) {
                  logging.Info("Truncation")<<"isub="<<isub<<" islice="<<i<<" nmax="<<patternPerSlice[i].fNMax
                                            <<" npattern="<<patternPerSlice[i].fNPattern<<"\n";
               }
            }
            // here:
            //   sectorList[partition][] :  the sectors in this partition 
            //   patternPerSlice[partition]
            //     : number of patterns and maximum in partition

            // (2) for each slice determine the efficiency and add up
            //   the target number of patterns
            vector<double> targetPatterns(nSliceRebin);
            double ctheta0=TMath::SinH(-3.),ctheta1=TMath::SinH(3.);
            double total=0.;
            for(int i2=0;i2<nSliceTimes2;i2++) {
               // get cos(theta) from slice number
               double ctheta=(i2+etabinOffset)*(ctheta1-ctheta0)/
                  nSliceTimes2+ctheta0;
               // get eta coordinate (for efficiency plot)
               double eta=TMath::ASinH(ctheta);
               double abseta=TMath::Abs(eta);
               // interpolated efficiency from plot
               double epsilon=efficiencyPlot->Eval(eta);
               double epsilon0=
                  PartitionSteering::Instance()->GetEpsilonLimit();
               int i=i2/(rebinSlices>0 ? rebinSlices : 1);
               if(i>=nSliceRebin) i=nSliceRebin-1;
               cosThetaPerSlice2->SetBinContent(i+1,ctheta);
               etaPerSlice2->SetBinContent(i+1,eta);
               if(((absetamin>=absetamax)||
                   ((abseta>=absetamin)&&(abseta<=absetamax)))&&
                  (patternPerSlice[i].fNPattern>0)) {
                  double weight=PartitionSteering::Instance()
                     ->GetWeightLimit();
                  if(epsilon<epsilon0) {
                     weight=-1./TMath::Log(1.-epsilon/epsilon0);
                     if(!(weight>PartitionSteering::Instance()
                          ->GetWeightLimit())) {
                        weight=PartitionSteering::Instance()->GetWeightLimit();
                     }
                  }
                  // weight: desired weighting factor for which the predicted
                  //   efficiency reaches the target efficiency

                  targetPatterns[i]+= patternPerSlice[i].fNPattern*weight;
                  total+= targetPatterns[i];
               }
               originalPerSlice2->SetBinContent
                  (i+1,reg+1,patternPerSlice[i].fNPattern);
            }
            // here:
            //   targetPatterns[partition] : patterns needed to reach target
            //                                 efficiency
            //   total : total number of patterns needed to reach target

            // (3) normalize
            //   targetPatterns[] should be scaled down by the factor
            //     nPattern/total 
            // target number of patterns in this subregion
            int nPattern=nPatternRead/nsub;
            if(PartitionSteering::Instance()->GetNPattern())
               nPattern=PartitionSteering::Instance()->GetNPattern();
            if(total==0) {
               logging.Warning("subregion")
                  <<"no sectors accepted, store one pattern per partition\n";
               nPattern=nSliceTimes2;
            }
            for(int i=0;i<nSliceRebin;i++) {
               targetPatterns[i]*=((double)nPattern)/total;
            }
            // (4) take into account truncation
            double truncate=0.;
            do {
               total=0.;
               double free=0.;
               truncate=0.;
               for(int i=0;i<nSliceRebin;i++) {
                  if(targetPatterns[i]>=patternPerSlice[i].fNMax) {
                     truncate += targetPatterns[i]-patternPerSlice[i].fNMax;
                     if(targetPatterns[i]>0.0) {
                        logging.Info("target")<<" slice="<<i<<" change from "<<targetPatterns[i]<<" to "<<patternPerSlice[i].fNMax<<"\n";
                     }
                     targetPatterns[i]=patternPerSlice[i].fNMax;
                  } else {
                     free+=targetPatterns[i];
                  }
                  total+=targetPatterns[i];
               }
               logging.Info("target")<<" total="<<total<<" nPattern="<<nPattern<<" free="<<free<<" truncate="<<truncate<<"\n";
               if(free<=0.) break;
               // truncate: excess patterns to be distributed
               double scale=truncate/free;
               for(int i=0;i<nSliceRebin;i++) {
                  if(targetPatterns[i]<patternPerSlice[i].fNMax-0.5) {
                     double n=targetPatterns[i]*scale;
                     targetPatterns[i] += n;
                     truncate -= n;
                  }
               }
            } while(truncate>1.0);

            //  (5) finally, all numbers have to be integer
            //    fractional patterns have to be rounded up or down
            //    also, take into account truncation

            // round to integer
            int tot=0;
            for(int i=0;i<nSliceRebin;i++) {
               // skip empty partitions
               if(patternPerSlice[i].fNPattern==0) continue;
               if(total>0) {
                  int target=(int)(targetPatterns[i]+0.5);
                  if(patternPerSlice[i].fNMax>target) {
                     // set new normalized target number of patterns
                     // (only for those partitions which are not at limit)
                     patternPerSlice[i].fNPattern=target;
                  } else {
                     patternPerSlice[i].fNPattern=patternPerSlice[i].fNMax;
                  }
                  tot+= patternPerSlice[i].fNPattern;
               } else {
                  patternPerSlice[i].fNPattern=1;
               }
            }
            logging.Info("adjust")<<"isub="<<isub<<" tot="<<tot<<" nPattern="<<nPattern<<"\n";
            // execute this loop to adjust the number of patterns
            //   until the integer number: tot
            //   equals the target number: nPattern
            int nAdjustTotal=0;
            int nLimitTotal=0;
            while(tot-nPattern) {
               // try all partitions in sequence
               int nAdjust=0;
               int nLimit=0;
               for(int i=0;i<nSliceRebin;i++) {
                  // do not adjust empty partitions
                  if(patternPerSlice[i].fNPattern<=0) continue;
                  // do not adjust partitions which are at limit
                  int delta=tot-nPattern;
                  if(!delta) break; // done
                  else if(delta>0) {
                     // too many patterns, subtract one in this partition
                     tot--;
                     patternPerSlice[i].fNPattern--;
                     nAdjust++;
                  } else if(delta<0) {
                     // too few patterns, add one in this partition
                     if(patternPerSlice[i].fNPattern<patternPerSlice[i].fNMax) {
                        tot++;
                        patternPerSlice[i].fNPattern++;
                        nAdjust++;
                     } else {
                        nLimit++;
                     }
                  }
               }
               nAdjustTotal+=nAdjust;
               nLimitTotal+=nLimit;
               if(nAdjust==0) break;
            }
            logging.Info("adjust")<<"total="<<tot<<" nPattern="<<nPattern<<" adjustTotal="<<nAdjustTotal<<" limit="<<nLimitTotal<<"\n";
            for(int i=0;i<nSliceRebin;i++) {
               partitionedPerSlice2->SetBinContent
                  (i+1,reg+1,patternPerSlice[i].fNPattern);
            }
            // done
            // write out partition file
            int sum=0;
            int nSectorUsed=0;
            int nSectorUsedBank=0;
            int nSliceUsed=0;
            for(int i=0;i<nSliceRebin;i++) {
               if(sectorList[i].size() && patternPerSlice[i].fNPattern) {
                  sum += patternPerSlice[i].fNPattern;
                  (*outputFile)<<"PARTITION "<<iPart<<" "
                               <<patternPerSlice[i].fNPattern
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
                  logging.Info("result")
                     <<"ipart="<<iPart<<" isub="<<isub <<" islice="<<i
                     <<" npattern="<<patternPerSlice[i].fNPattern
                     <<" nmax="<<patternPerSlice[i].fNMax
                     <<"\n";
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
   outputRoot->cd();
   originalPerSlice2->Write();
   partitionedPerSlice2->Write();
   etaPerSlice2->Write();
   cosThetaPerSlice2->Write();
   delete outputRoot;

   if(dumpMaximum) delete dumpMaximum;

   logging.Info("output")
      <<"maxSector="<<totalMaxSector<<" maxSectorUsed="<<totalMaxSectorUsed
      <<" nSectorTotal="<<totalNsectorTotal<<" nSectorUsed="<<totalNsectorUsed
      <<" nSectorUsedBank="<<totalNsectorUsedBank<<"\n";
   return 0;
}
