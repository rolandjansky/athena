/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        TPileupReweighting

Author:      Will Buttinger
Created:     October 2011

Description: Tool to get the calculated MC pileup weight.
******************************************************************************/

// Preprocessor magic for debugging
#define XXX std::cout << " I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// This class' header
#include "PileupReweighting/TPileupReweighting.h"



// include math
#include <math.h>

// ROOT includes
#include <TROOT.h>
#include <TChain.h>
#include <TLeaf.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TAxis.h>
#include <TString.h>
#include <TRandom3.h>


ClassImp(CP::TPileupReweighting)


//=============================================================================
// Constructor
//=============================================================================
CP::TPileupReweighting::TPileupReweighting(const char* name) :
  TNamed(name,"notitle"), m_parentTool(this),
   m_SetWarnings(true),m_debugging(false),
   m_countingMode(true),m_unrepresentedDataAction(0),m_isInitialized(false),m_lumiVectorIsLoaded(false),
   m_dataScaleFactorX(1.),m_dataScaleFactorY(1.),
   m_mcScaleFactorX(1.),m_mcScaleFactorY(1.),
   m_nextPeriodNumber(1),m_ignoreFilePeriods(false),m_metadatatree(0),m_unrepDataTolerance(0.05),m_doGlobalDataWeight(false),m_lumicalcRunNumberOffset(0), m_emptyHistogram(0), m_random3(0), m_ignoreBadChannels(false)
{
   m_random3 = new TRandom3(0);
   m_random3->SetSeed(1);

   //create the global period 
   m_periods[-1] = new Period(-1,0,9999999,0/* the global default channel*/);

   //MC12ab merger by default
   //RemapPeriod(195848,195847);
}



//=============================================================================
// Destructor
//=============================================================================
CP::TPileupReweighting::~TPileupReweighting() {
   delete m_random3; 
}

void CP::TPileupReweighting::RemapPeriod(Int_t periodNumber1, Int_t periodNumber2) {
   //check if periodNumber2 exists
   if(m_periods.find(periodNumber2)==m_periods.end()) {
      m_periods[periodNumber2] = new Period(periodNumber2,0,0,GetDefaultChannel(-1)/*m_periods[-1]->defaultChannel*/);
   }
   m_periods[periodNumber1] = m_periods[periodNumber2];
}

void CP::TPileupReweighting::SetDefaultChannel(Int_t channel, Int_t periodNumber) {
   //check if periodNumber2 exists
   if(m_periods.find(periodNumber)==m_periods.end()) {
      m_periods[periodNumber] = new Period(periodNumber,0,0,channel);
   } else {
      m_periods[periodNumber]->SetDefaultChannel(channel); 
   }
}


Int_t CP::TPileupReweighting::SetBinning(Int_t nbinsx, Double_t* xbins, Int_t nbinsy, Double_t* ybins) {
   if(m_emptyHistogram) { delete m_emptyHistogram; }
   if(nbinsy>0) m_emptyHistogram = new TH2D("default","default",nbinsx,xbins,nbinsy,ybins);
   else m_emptyHistogram = new TH1D("default","default",nbinsx,xbins);
   m_emptyHistogram->SetDirectory(0);
   return 0;
}
Int_t CP::TPileupReweighting::SetUniformBinning(Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup) {
   std::vector<double> xbins(nbinsx+1);
   std::vector<double> ybins(nbinsy+1);
   for(int i=0;i<nbinsx+1;i++) xbins[i] = xlow + i*(xup-xlow)/nbinsx;
   if (nbinsy > 0)
     for(int i=0;i<nbinsy+1;i++) ybins[i] = ylow + i*(yup-ylow)/nbinsy;
   return SetBinning(nbinsx,&xbins[0],nbinsy,&ybins[0]);
}
Int_t CP::TPileupReweighting::SetBinning(TH1* hist) {
   if(!hist) return -1;
   if(m_emptyHistogram) { delete m_emptyHistogram; }
   m_emptyHistogram = dynamic_cast<TH1*>(hist->Clone("default"));
   if (m_emptyHistogram)
     m_emptyHistogram->SetDirectory(0);
   return 0;
}

Int_t CP::TPileupReweighting::GetDefaultChannel(Int_t mcRunNumber) {
   if(m_parentTool != this) return m_parentTool->GetDefaultChannel(mcRunNumber); 
   return m_periods[mcRunNumber]->defaultChannel;
}


Double_t CP::TPileupReweighting::GetIntegratedLumi(const TString& trigger) { 
   if(!m_isInitialized) { Info("GetIntegratedLumi","Initializing the subtool.."); Initialize(); }
   if(!m_lumiVectorIsLoaded) {
      Error("GetIntegratedLumi","No UNPRESCALED (Trigger=None) Lumicalc file loaded, so cannot get integrated lumi, returning 0");
      return 0;
   }
   if(trigger=="" || trigger=="None") return GetSumOfEventWeights(-1)/1E6; 
   //check the triggerPrimaryDistributions in the global period, if not present, build it
   //Period* global = m_periods[-1];
   
   if(m_triggerObjs.find(trigger)==m_triggerObjs.end()) {
      CalculatePrescaledLuminosityHistograms(trigger);
   }
   
   auto h = m_triggerObjs[trigger]->triggerHists[-1][m_triggerObjs[trigger]->getBits(this)];
   
   return h->Integral(0,h->GetNbinsX()+1)/1E6;
}



Double_t CP::TPileupReweighting::GetIntegratedLumi(Int_t periodNumber, UInt_t start, UInt_t end) {
   if(!m_isInitialized) { Info("GetIntegratedLumi","Initializing the subtool.."); Initialize(); }
   //look through dataPeriodRunTotals["pileup"][periodNumber] for runs inside the given period
   double total = 0; 
   for(auto run : m_periods[periodNumber]->runNumbers) {
      if(run >= start && run <= end) total += m_runs[run].inputHists["None"]->GetSumOfWeights();
   }
   return total*1E-6;
}

Double_t CP::TPileupReweighting::GetLumiBlockIntegratedLumi(Int_t runNumber, UInt_t lb) {
   if(!m_isInitialized) { Info("GetIntegratedLumi","Initializing the subtool.."); Initialize(); }
   if(m_runs.find(runNumber)==m_runs.end()) return 0.;
   auto& run = m_runs[runNumber];

   if(run.lumiByLbn.find(lb)==run.lumiByLbn.end()) return 0.;
   return run.lumiByLbn[lb].first*1E-6;
}

Float_t CP::TPileupReweighting::GetLumiBlockMu(Int_t runNumber, UInt_t lb) {
   if(!m_isInitialized) { Info("GetLumiBlockMu","Initializing the subtool.."); Initialize(); }
   if(m_runs.find(runNumber)==m_runs.end()) return -1.0;
   auto& run = m_runs[runNumber];

   if(run.lumiByLbn.find(lb)==run.lumiByLbn.end()) return -1.0;
   return run.lumiByLbn[lb].second;
}

Double_t CP::TPileupReweighting::GetIntegratedLumiFraction(Int_t periodNumber, UInt_t start, UInt_t end) {
   if(!m_isInitialized) { Info("GetIntegratedLumiFraction","Initializing the subtool.."); Initialize(); }

   if(!m_lumiVectorIsLoaded) {
      Error("GetIntegratedLumiFraction","No UNPRESCALED (Trigger=None) Lumicalc file loaded, so no lumi fraction possible, returning 0");
      return 0;
   }
   //total lumi in period is given by sumOfWeights[-1]
   double total = m_periods[periodNumber]->sumOfWeights[-1];
   //go through associated runs, adding up lumi from all within start and end (inclusive)
   double numer(0);
   for(auto run : m_periods[periodNumber]->runNumbers) {
      if(run >= start && run <= end) numer += m_runs[run].inputHists["None"]->GetSumOfWeights();
   }
   return numer / total;
}

Double_t CP::TPileupReweighting::GetIntegratedLumiFraction(Int_t periodNumber, Double_t mu, UInt_t start, UInt_t end) {
   if(!m_isInitialized) { Info("GetIntegratedLumiFraction","Initializing the subtool.."); Initialize(); }
   if(!m_lumiVectorIsLoaded) {
      Error("GetIntegratedLumiFraction","No UNPRESCALED (Trigger=None) Lumicalc file loaded, so no lumi fraction possible, returning 0");
      return 0;
   }
   //determine the mubin
   if(!m_emptyHistogram) { Error("GetIntegratedLumiFraction", "Cannot do this without a lumicalc file!"); return 0; }
   Int_t muBin = m_emptyHistogram->FindFixBin(mu);
   //the total mu in this bin in this period is given by the triggerHist = "None" hist
    
   
    
   double total = m_triggerObjs["None"]->triggerHists[periodNumber][0]->GetBinContent(muBin); //m_periods[periodNumber]->triggerHists["None"]->GetBinContent(muBin);
   //loop over assigned runs, if in range then include in numerator 
   double numer = 0;
   for(auto run : m_periods[periodNumber]->runNumbers) {
      if(run >= start && run <= end) numer += m_runs[run].muDist->GetBinContent(muBin);
   }

   return numer / total;
}



Int_t CP::TPileupReweighting::UsePeriodConfig(const TString& configName) {
   if(configName=="MC11a") {
      AddPeriod(180164, 177986,180481); //associates mc runnumber 180164 with data period 177986 to 180481 (period B-D)
      AddPeriod(183003, 180614,184169); //period E-H
      AddPeriod(185649, 185353,186934); //period I-K1. For I-K you would change the last number to 187815
      AddPeriod(185761, 186935,191933); //everything else. Thanks Ellie!
      SetUniformBinning(100,0,50);
      Info("UsePeriodConfig","Using MC11a Period configuration");
      return 0;
   } else if(configName=="MC11b" || configName=="MC11c") {
      AddPeriod(180164, 177986, 180481);
      AddPeriod(183003, 180614, 184169);
      AddPeriod(186169, 185353, 187815);
      AddPeriod(189751, 188902, 191933);
      SetUniformBinning(100,0,50);
      Info("UsePeriodConfig","Using MC11b/c Period configuration");
      return 0;
   } else if(configName=="MC12a") {
      AddPeriod(195847,200804,216432);
      //mc12a binning is in integer values of mu

      if(m_emptyHistogram && (m_emptyHistogram->GetNbinsX()!=50 || fabs(m_emptyHistogram->GetXaxis()->GetBinLowEdge(1)+0.5)>0.01|| fabs(m_emptyHistogram->GetXaxis()->GetBinUpEdge(50)-49.5)>0.01) ) {
         Error("UsePeriodConfig","Cannot use MC12a, an incompatible config has already been set up");
         throw std::runtime_error("Throwing 13: Cannot use MC14, an incompatible config has already been set up");
         return -1;
      }

      SetUniformBinning(50,-0.5,49.5);
      Info("UsePeriodConfig","Using MC12a Period configuration");
      return 0;
   } else if(configName=="MC12b") {
      AddPeriod(195848,200804,216432);
      //mc12b binning is in integer values of mu

      if(m_emptyHistogram && (m_emptyHistogram->GetNbinsX()!=50 || fabs(m_emptyHistogram->GetXaxis()->GetBinLowEdge(1)+0.5)>0.01|| fabs(m_emptyHistogram->GetXaxis()->GetBinUpEdge(50)-49.5)>0.01) ) {
         Error("UsePeriodConfig","Cannot use MC12b, an incompatible config has already been set up");
         throw std::runtime_error("Throwing 13: Cannot use MC14, an incompatible config has already been set up");
         return -1;
      }

      SetUniformBinning(50,-0.5,49.5);
      Info("UsePeriodConfig","Using MC12b Period configuration");
      return 0;
   } else if(configName=="MC12ab") {
      AddPeriod(195847,200804,216432);
      AddPeriod(195848,200804,216432);

      if(m_emptyHistogram && (m_emptyHistogram->GetNbinsX()!=50 || fabs(m_emptyHistogram->GetXaxis()->GetBinLowEdge(1)+0.5)>0.01|| fabs(m_emptyHistogram->GetXaxis()->GetBinUpEdge(50)-49.5)>0.01) ) {
         Error("UsePeriodConfig","Cannot use MC12ab, an incompatible config has already been set up");
         throw std::runtime_error("Throwing 13: Cannot use MC14, an incompatible config has already been set up");
         return -1;
      }

      //mc12a/b binning is in integer values of mu
      SetUniformBinning(50,-0.5,49.5);
      Info("UsePeriodConfig","Using MC12ab Period configuration");
      return 0;
   } else if(configName=="MC14_8TeV") {
      AddPeriod(212272,200804,216432);
      if(m_emptyHistogram && (m_emptyHistogram->GetNbinsX()!=50 || fabs(m_emptyHistogram->GetXaxis()->GetBinLowEdge(1)+0.5)>0.01 || fabs(m_emptyHistogram->GetXaxis()->GetBinUpEdge(50)-49.5)>0.01) ) {
         Error("UsePeriodConfig","Cannot use MC14_8TeV, an incompatible config has already been set up");
         throw std::runtime_error("Throwing 13: Cannot use MC14_8TeV, an incompatible config has already been set up");
         return -1;
      }
      SetUniformBinning(50,-0.5,49.5);
      Info("UsePeriodConfig","Using MC14_8TeV Period configuration");
      return 0;
   } else if(configName=="MC14_13TeV") {
      AddPeriod(222222,222222,999999);
      if(m_emptyHistogram && (m_emptyHistogram->GetNbinsX()!=100 || fabs(m_emptyHistogram->GetXaxis()->GetBinLowEdge(1)+0.5)>0.01|| fabs(m_emptyHistogram->GetXaxis()->GetBinUpEdge(100)-99.5)>0.01) ) {
         Error("UsePeriodConfig","Cannot use MC14_13TeV, an incompatible config has already been set up");
         throw std::runtime_error("Throwing 13: Cannot use MC14_13TeV, an incompatible config has already been set up");
         return -1;
      }
      SetUniformBinning(100,-0.5,99.5);
      Info("UsePeriodConfig","Using MC14_13TeV Period configuration");
      return 0;
   } else if(configName=="MC15") {
      AddPeriod(222510,222222,999999);
      AddPeriod(222525,222222,999999);
      AddPeriod(222526,222222,999999);
      AddPeriod(284500,222222,999999);
      AddPeriod(295000,222222,999999); //mc15c for pPb data in 2016
      if(m_emptyHistogram && (m_emptyHistogram->GetNbinsX()!=100 || fabs(m_emptyHistogram->GetXaxis()->GetBinLowEdge(1))>0.01 || fabs(m_emptyHistogram->GetXaxis()->GetBinUpEdge(100)-100.)>0.01) ) {
         Error("UsePeriodConfig","Cannot use MC15, an incompatible config has already been set up");
         throw std::runtime_error("Throwing 13: Cannot use MC15, an incompatible config has already been set up");
         return -1;
      }
      SetUniformBinning(100,0,100); //Thanks Eric </sarcasm>
      Info("UsePeriodConfig","Using MC15 Period configuration");
      return 0;
   } else if(configName=="Run2") {
     m_autoRunStart = 222222; m_autoRunEnd = 999999; //periods will be automatically added during Fill
     if(m_emptyHistogram && (m_emptyHistogram->GetNbinsX()!=100 || fabs(m_emptyHistogram->GetXaxis()->GetBinLowEdge(1))>0.01 || fabs(m_emptyHistogram->GetXaxis()->GetBinUpEdge(100)-100.)>0.01) ) {
         Error("UsePeriodConfig","Cannot use open Run2, an incompatible config has already been set up");
         throw std::runtime_error("Throwing 13: Cannot use Run2 config, an incompatible config has already been set up");
         return -1;
      }
      SetUniformBinning(100,0,100); //Thanks Eric </sarcasm>
      Info("UsePeriodConfig","Using Run2 Period configuration, which assumes period assignment of 222222 to 999999");
   }
   Error("UsePeriodConfig","Unrecognized period config");
   return -1;
}


///returns a PeriodID. These count up from 1
Int_t CP::TPileupReweighting::AddPeriod(Int_t periodNumber, UInt_t start, UInt_t end) {
   //you can add more periods only in counting mode, but they must not be with subperiods!

   if(m_isInitialized && !m_countingMode) {
      Error("AddPeriod","You cannot AddPeriod after initializing the tool, except when in config file generating mode. Reorder your code!");
      throw std::runtime_error("Throwing 1: You cannot AddPeriod after initializing the tool, except when in config file generating mode. Reorder your code!");
   }

   if(m_periods.find(periodNumber)==m_periods.end()) {
      m_periods[periodNumber] = new Period(periodNumber,start,end,GetDefaultChannel(-1));
      return periodNumber;
   }

   //if period exists but both start and end are zero, then just assign that
   Period* p = m_periods[periodNumber];

   if(p->subPeriods.size() == 0 && p->start==0 && p->end==0) { p->start=start; p->end=end; return periodNumber; }

   //check if period already contained (i.e. same period or one of the existing subperiods) ... if it is, do nothing 
   if(p->start==start && p->end==end) return periodNumber;
   for(auto pp : p->subPeriods) {
      if(pp->start==start && pp->end==end) return pp->id;
   }

   //if get here, and in counting mode, this isn't allowed! .. i.e. we can't have subperiods
   if(m_isInitialized && m_countingMode) {
      Error("AddPeriod","You cannot have subperiods when in Config File Generating mode");
      throw std::runtime_error("Throwing 44: In Config File Generating mode, but detected subperiods in the period definition. This is not allowed.");
   }

   //if period is already defined, we create a new period with start and end, and assign our period to this one 
   //also create a new period for the existing period configuration of this period 

   if(p->subPeriods.size()==0) {
      while(m_periods.find(m_nextPeriodNumber) != m_periods.end()) m_nextPeriodNumber++;
      m_periods[m_nextPeriodNumber] = new Period(m_nextPeriodNumber,p->start,p->end,p->defaultChannel);
      p->subPeriods.push_back(m_periods[m_nextPeriodNumber]);
      p->start = 0; p->end=0;
   }

   while(m_periods.find(m_nextPeriodNumber) != m_periods.end()) m_nextPeriodNumber++;
   m_periods[m_nextPeriodNumber] = new Period(m_nextPeriodNumber,start,end, p->defaultChannel);
   p->subPeriods.push_back(m_periods[m_nextPeriodNumber]);

   return m_nextPeriodNumber;
}


Int_t CP::TPileupReweighting::GetFirstFoundPeriodNumber(UInt_t runNumber) {
   //go through periods, first period that includes this runNumber wins!
   //otherwise it returns the 'global period number': -1
   for(auto period : m_periods) {
      if(period.second->id==-1) continue;
      if(period.second->contains(runNumber)) return period.second->id;
   }
   return -1;

}


TH1* CP::TPileupReweighting::CloneEmptyHistogram(Int_t runNumber, Int_t channelNumber) {

   TString s("pileup");
   if(channelNumber>=0) {
      s += "_chan"; s += channelNumber;
   } else {
      s += "_data";
   }
   s+="_run"; s+= runNumber;
   //get the empty histogram
   if(!m_emptyHistogram) {
      Error("CloneEmptyHistogram","There is no binning info - use SetBinning/SetUniformBinning or load a prw config file (This usually means you need to call AddConfigFile BEFORE AddLumiCalcFile)");
      throw std::runtime_error("Throwing 47: There is no binning info - use SetBinning/SetUniformBinning or load a prw config file (This usually means you need to call AddConfigFile BEFORE AddLumiCalcFile)");
   }

   TH1* out = dynamic_cast<TH1*>(m_emptyHistogram->Clone(s));
   if (out) {
     out->SetTitle(s);
     out->SetDirectory(0); //take ownership
     out->SetEntries(0);
   }
   return out;

}

Int_t CP::TPileupReweighting::GenerateMetaDataFile(const TString& fileName,const TString& channelBranchName) {

   TTree inTree("in","in");
   inTree.ReadFile(fileName);
   TTree outTree("ChannelMetaData","ChannelMetaData");
   Int_t chanNum;
      std::map<TString, Double_t> data;
      TObjArray *leaves = inTree.GetListOfLeaves();
      if(leaves==0) {Error("GenerateMetaDataFile","No leaves"); return -1; }
      for(Int_t i=0;i<leaves->GetEntries();++i) {
         TLeaf *leaf = (TLeaf *)leaves->At(i);
         if(leaf) {
            TBranch *branch = leaf->GetBranch();
            if(strcmp(branch->GetName(),channelBranchName)==0) {
               //this is the channel branch
               if(strcmp(leaf->GetTypeName(),"Int_t")!=0) {
                  Error("GenerateMetaDataFile","Channel Branch must be type Int_t"); return -1;
               }
               branch->SetAddress(&chanNum);
               outTree.Branch(channelBranchName,&chanNum);
            } else if(strcmp(leaf->GetTypeName(),"Double_t")!=0) {
                  Warning("GenerateMetaDataFile","Cannot read non-double branch: %s",branch->GetName());
            } else {
               branch->SetAddress(&(data[branch->GetName()]));
               outTree.Branch(branch->GetName(),&(data[branch->GetName()]));
            }
         }
      }
      //loop over tree entries and read in the values according to property type 
      for(Int_t i=0;i<inTree.GetEntries();++i) {
         inTree.GetEntry(i);
         outTree.Fill();
      }

      //remove the file extension and replace with .root 
      TString outName = fileName(0,fileName.Last('.'));
      outName += ".prw.root";
      TFile f1(outName,"RECREATE");
      outTree.Write();
      f1.Close();
      Info("GenerateMetaDataFile","Succesfully Generated File %s",outName.Data()); 
      return 0;
}

Int_t CP::TPileupReweighting::AddMetaDataFile(const TString& fileName,const TString& channelBranchName) {
   TDirectory* origDir = gDirectory;
   TTree* tmp = 0;
   TFile* rootFile = 0;
   if(fileName.EndsWith(".root")) {
      //looks for ChannelMetaData ttree 
      // Load the data ROOT file
      rootFile = TFile::Open( fileName, "READ" );
      if ( rootFile->IsZombie() ) {
         Error("AddMetaDataFile","Could not open file: %s",fileName.Data());
         throw std::runtime_error("Throwing 6");
      } else {
         //try to get the the known TTrees 
         tmp = (TTree*)rootFile->Get( "ChannelMetaData" );
         if(!tmp) {
            Error("AddMetaDataFile","%s is not a valid metadata file. Should have a ChannelMetaData TTree",fileName.Data());
            throw std::runtime_error("Throwing 7");
         }
      }
   } else {
      //try to make the TTree by reading the file 
      tmp = new TTree("ChannelMetaData","ChannelMetaData");
      tmp->ReadFile(fileName);
   }
   Int_t chanNum;
   std::map<TString, Double_t> data;
   TObjArray *leaves = tmp->GetListOfLeaves();
   if(leaves==0) {Error("AddMetaDataFile","No leaves"); return -1; }
   for(Int_t i=0;i<leaves->GetEntries();++i) {
      TLeaf *leaf = (TLeaf *)leaves->At(i);
      if(leaf) {
         TBranch *branch = leaf->GetBranch();
         if(strcmp(branch->GetName(),channelBranchName)==0) {
            //this is the channel branch
            if(strcmp(leaf->GetTypeName(),"Int_t")!=0) {
               Error("AddMetaDataFile","Channel Branch must be type Int_t");
               throw std::runtime_error("Throwing 7");
            }
            branch->SetAddress(&chanNum);
         } else if(strcmp(leaf->GetTypeName(),"Double_t")!=0) {
               Warning("AddMetaDataFile","Cannot read non-double branch: %s",branch->GetName());
         } else {
            branch->SetAddress(&(data[branch->GetName()]));
         }
      }
   }
   //loop over tree entries and read in the values
   for(Int_t i=0;i<tmp->GetEntries();++i) {
      tmp->GetEntry(i);
      for(std::map<TString, Double_t>::iterator it = data.begin();it!=data.end();++it) {
         if(m_metadata.find(it->first)!=m_metadata.end()&&m_metadata[it->first].find(chanNum)!=m_metadata[it->first].end() && m_metadata[it->first][chanNum]!=it->second) {
            Warning("AddMetaDataFile", "Overriding metadata [%s,%d]. %f becomes %f",(it->first).Data(),chanNum,m_metadata[it->first][chanNum],it->second);
         }
         m_metadata[it->first][chanNum] = it->second;
      }
   }

   delete tmp;
   if(rootFile) { rootFile->Close();delete rootFile;}

   // Return to the original ROOT directory
   gDirectory = origDir;

   return 0;
}

TTree* CP::TPileupReweighting::GetMetaDataTree() {
   if(m_metadatatree) delete m_metadatatree;
   m_metadatatree = new TTree(TString(this->GetName())+"MetaData",TString(this->GetName())+"MetaData");
   m_metadatatree->SetDirectory(0);
   Int_t channel=0;
   m_metadatatree->Branch("mc_channel_number",&channel);
   std::map<TString,Double_t> data;
   std::map<Int_t,bool> channels;
   //create a branch for each metadata item 
   for(std::map<TString,std::map<Int_t,Double_t> >::iterator it=m_metadata.begin();it!=m_metadata.end();++it) {
      for(std::map<Int_t,Double_t>::iterator it2=(it->second).begin();it2!=(it->second).end();++it2) {
         channels[it2->first]=true; //records which channels we have metadata for 
      }
      if(data.find(it->first)==data.end()) {
         //new branch
         data[it->first]=0.;
         m_metadatatree->Branch(it->first,&(data[it->first]));
      }
   }
   //also create branches for the NumberOfEvents and SumOfEventWeights 
   data["NumberOfEvents"]=0.;
   m_metadatatree->Branch("NumberOfEvents",&(data["NumberOfEvents"]));
   data["SumOfEventWeights"]=0.;
   m_metadatatree->Branch("SumOfEventWeights",&(data["SumOfEventWeights"]));
   //and add to channels list any channels that have events 
   auto& myMap = m_periods[-1]->sumOfWeights;
   for(std::map<Int_t,Double_t>::iterator it=myMap.begin();it!=myMap.end();++it) {
      if(it->first>=0 && it->second>0.) channels[it->first]=true;
   }

   //now loop over the channels and fill the TTree 
   for(std::map<Int_t,bool>::iterator it=channels.begin();it!=channels.end();++it) {
      channel=it->first;
      for(std::map<TString,Double_t>::iterator it2=data.begin();it2!=data.end();++it2) {
         if(it2->first=="NumberOfEvents") {
            //check for this in globalNumberOfEntries 
            if(myMap.find(channel)==myMap.end()) {
               data[it2->first]=0.;
               Warning("GetMetaDataTree","Channel %d does not have MetaData %s",it->first,(it2->first).Data());
            } else {
               data[it2->first]=m_periods[-1]->numberOfEntries[channel];
            }
         } else if(it2->first=="SumOfEventWeights") {
            //check for this in globalTotals 
            auto& myMap2 = m_periods[-1]->sumOfWeights;
            if(myMap2.find(channel)==myMap2.end()) {
               data[it2->first]=0.;
               Warning("GetMetaDataTree","Channel %d does not have MetaData %s",it->first,(it2->first).Data());
            } else {
               data[it2->first]=m_periods[-1]->sumOfWeights[channel];
            }
         } else {
            auto& myMap2 = m_metadata[it2->first];
            if(myMap2.find(channel)==myMap2.end()) {
               //this channel doesn't have this property
               data[it2->first]=0.;
               Warning("GetMetaDataTree","Channel %d does not have MetaData %s",it->first,(it2->first).Data());
            } else {
               data[it2->first]=myMap2[channel];
            }
         }
      }
      m_metadatatree->Fill();
   }
   
   m_metadatatree->BuildIndex("mc_channel_number");
   m_metadatatree->ResetBranchAddresses();

   return m_metadatatree;
}








void CP::TPileupReweighting::AddDistributionTree(TTree *tree, TFile *file) {

   bool isMC=true;
   //should have the standard structure 
   Int_t channel = 0; UInt_t runNbr = 0; 
   std::vector<UInt_t>* pStarts = 0;std::vector<UInt_t>* pEnds = 0;
   Char_t histName[150];
   Char_t customName[150];
   Bool_t isDefaultForRunNumber(false); bool hasDefaultsBranch=false;
   if(strcmp(tree->GetName(),"MCPileupReweighting")==0) {strcpy(customName,"pileup");isMC=true;}
   else {
      if(tree->SetBranchAddress("CustomName",&customName)!=0) {
         Error("AddDistributionTree","Could not find CustomName branch in TTree");throw std::runtime_error("Throwing 18");
      }
   }
   if(strcmp(tree->GetName(),"DataCustomReweighting")==0) {channel=-1;isMC=false;}
   else {
      if(tree->SetBranchAddress("Channel",&channel)!=0) {
         Error("AddDistributionTree","Could not find Channel branch in TTree");throw std::runtime_error("Throwing 18");
      }
   }
   if(tree->SetBranchAddress("RunNumber",&runNbr)!=0) {
      Error("AddDistributionTree","Could not find RunNumber branch in TTree");throw std::runtime_error("Throwing 18");
   }
   if(isMC) {
      if(tree->SetBranchAddress("PeriodStarts",&pStarts)!=0) {
         Error("AddDistributionTree","Could not find PeriodStarts branch in TTree");throw std::runtime_error("Throwing 18");
      }
      if(tree->SetBranchAddress("PeriodEnds",&pEnds)!=0) {
         Error("AddDistributionTree","Could not find PeriodEnds branch in TTree");throw std::runtime_error("Throwing 18");
      }
      if(tree->FindBranch("IsDefaultForRunNumber")!=0) {
         tree->SetBranchAddress("IsDefaultForRunNumber",&isDefaultForRunNumber);
         hasDefaultsBranch=true;
      }
   }

   if(tree->SetBranchAddress("HistName",&histName)!=0) {
      Error("AddDistributionTree","Could not find HistName branch in TTree");throw std::runtime_error("Throwing 18");
   }
   long n = tree->GetEntries();
   std::map<TString,bool> loadedHistos; //avoid double-loading from this file
   for(long i=0;i<n;i++) {
      tree->GetEntry(i);
      TString sHistName(histName);
      TString weightName(customName);
      if(loadedHistos.find(sHistName)==loadedHistos.end()) {
         loadedHistos[sHistName]=true;
         if(!m_ignoreFilePeriods && isMC) {
            for(unsigned int j=0;j<pStarts->size();j++) {
               unsigned int start = pStarts->at(j);unsigned int end = pEnds->at(j);
               AddPeriod(runNbr,start,end);
            }
         }
         //load the histogram
         TH1 *histo = (TH1*)file->Get( sHistName );
         if(!histo) histo = (TH1*)file->Get( TString::Format("%sPileupReweighting/%s",m_prwFilesPathPrefix.c_str(),sHistName.Data()) ); //try from subdir
         if(!histo) {
            Error("AddDistributionTree","Unable to find the histogram %s in the File %s",sHistName.Data(),file->GetName());
            throw std::runtime_error("Throwing 21");
         }
         //add it to the histograms 
         AddDistribution(histo, runNbr, channel);
         //we also add it to the global count, if this isn't data 
         if(channel>=0) AddDistribution(histo,-1,channel);
      }
      if(hasDefaultsBranch) {
         if(isDefaultForRunNumber && m_periods.find(runNbr)!=m_periods.end()) m_periods[runNbr]->defaultChannel=channel;
      }
   }

}

//data is signalled by a negative channel number
Int_t CP::TPileupReweighting::AddDistribution(TH1* hist,Int_t runNumber, Int_t channelNumber) {

  //NOTE: should probably just ignore histograms with 0 entries

   if(m_isInitialized) {
      Error("AddDistribution","You cannot AddDistribution after initializing the tool. Reorder your code!");
      throw std::runtime_error("Throwing 5: You cannot AddLumiCalcFile after initializing the tool. Reorder your code!");
   }

   if(channelNumber>=0 && !m_periods[runNumber]) {
      Error("AddDistribution","Unrecognised periodNumber: %d .. please use AddPeriod to define a period",runNumber);
      throw std::runtime_error("Throwing 6: Unrecognised periodNumber. Please use AddPeriod to define a period");
   }
   
   if(!m_useMultiPeriods && runNumber>=0) {
      //not allowing multi periods in a single channel, so before we go any further, require no periods other than this incoming one to have a hist defined for this channel
      //the global one can though
      for(auto& period : m_periods) {
        if(period.first<0) continue; //global sum period
        if(period.first==runNumber) continue; //ok to add to this
        if(period.second->inputHists.find(channelNumber)!=period.second->inputHists.end()) {
          TString myMsg = TString::Format("Attempt to add distribution for channel %d to period %d, but this channels already has period %d defined.\nThis is indicative of use of incompatible PRW config files, please check your config files for multiple periods/runNumbers for the same channel.",channelNumber,runNumber,period.first);
          Error("AddDistribution",myMsg);
          throw std::runtime_error(myMsg.Data());
        }
      }
   }
   

   TH1*& inputHist = (channelNumber<0) ? m_runs[runNumber].inputHists["None"] : m_periods[runNumber]->inputHists[channelNumber];

   if(!inputHist) {
      //if no emptyHistogram specified, we will use this histogram as the structure for the empty;
      if(!m_emptyHistogram) {
         //check if the input histogram is a TH1D or not. We like TH1D, not yucky TH1F
         if(strcmp(hist->IsA()->GetName(),"TH1D")==0 || strcmp(hist->IsA()->GetName(),"TH2D")==0) {
            m_emptyHistogram = dynamic_cast<TH1*>(hist->Clone("default"));
         } else {
            //check dimensionality
            if(hist->GetDimension()==1) {
               std::vector<Double_t> binsX;
               for(int i=0;i<=hist->GetNbinsX();i++) binsX.push_back(hist->GetXaxis()->GetBinLowEdge(i+1));
               TH1D tmpHist("tmpHist","tmpHist",binsX.size()-1,&binsX[0]);
               m_emptyHistogram = dynamic_cast<TH1*>(tmpHist.Clone("default"));
            } else if(hist->GetDimension()==2) {
               std::vector<Double_t> binsX;std::vector<Double_t> binsY;
               for(int i=0;i<=hist->GetNbinsX();i++) binsX.push_back(hist->GetXaxis()->GetBinLowEdge(i+1));
               for(int i=0;i<=hist->GetNbinsY();i++) binsY.push_back(hist->GetYaxis()->GetBinLowEdge(i+1));
               TH2D tmpHist("tmpHist","tmpHist",binsX.size()-1,&binsX[0],binsY.size()-1,&binsY[0]);
               m_emptyHistogram = dynamic_cast<TH1*>(tmpHist.Clone("default"));
            } else {
               Error("AddDistribution","Unknown input histogram dimensionality: %d",hist->GetDimension());
               throw std::runtime_error("Throwing 98");
            }
         }
         if (m_emptyHistogram)
         {
           m_emptyHistogram->Reset();m_emptyHistogram->SetEntries(0);m_emptyHistogram->SetDirectory(0);//clear the histogram
         }
      }
      inputHist = CloneEmptyHistogram(runNumber,channelNumber);

   }


   //iterator over bins of histogram, filling the TH1 stored in the data map
   Double_t numEntries = inputHist->GetEntries();
   Int_t bin,binx,biny;
   for(biny=1; biny<=hist->GetNbinsY(); biny++) {
      for(binx=1; binx<=hist->GetNbinsX(); binx++) {
         bin = hist->GetBin(binx,biny);
         Double_t value = hist->GetBinContent(bin);
         Double_t x = hist->GetXaxis()->GetBinCenter(binx);
         Double_t y = hist->GetYaxis()->GetBinCenter(biny);
         //shift x,y,z by the MCScaleFactors as appropriate 
         if(channelNumber>=0) {x *= m_mcScaleFactorX; y *= m_mcScaleFactorY;}
         else { x *= m_dataScaleFactorX; y *= m_dataScaleFactorY; }
         Int_t inBin = inputHist->FindFixBin(x,y);
         Double_t inValue = inputHist->GetBinContent(inBin);
         inputHist->SetBinContent(inBin,inValue+value);
      }
   }

   //also keep track of the number of entries 
   //SetBinContent screws with the entry count, so had to record it before the loops above
   inputHist->SetEntries(numEntries+hist->GetEntries());
   m_countingMode=false;
   return 0;

} 


Int_t CP::TPileupReweighting::AddLumiCalcFile(const TString& fileName, const TString& trigger) {

   if(m_isInitialized) {
      Error("AddLumiCalcFile","You cannot AddLumiCalcFile after initializing the tool. Reorder your code!");
      throw std::runtime_error("Throwing 5: You cannot AddLumiCalcFile after initializing the tool. Reorder your code!");
   }
   TDirectory* origDir = gDirectory;
   // Load the data ROOT file
   TFile* rootFile = TFile::Open( fileName, "READ" );
   if ( rootFile->IsZombie() ) {
      Error("AddConfigFile","Could not open file: %s",fileName.Data());
      std::string toThrow = "Throwing 6: Could not open file: "; toThrow += fileName;
      throw std::runtime_error(toThrow);
   } else {
      //try to get the the known TTrees 
      TTree *tmp = (TTree*)rootFile->Get( "LumiMetaData" );
      if(tmp) {
         m_lumicalcFiles[trigger].push_back(fileName);
         if(trigger=="None") {
            Info("AddLumiCalcFile","Adding LumiMetaData (scale factor=%f)...",m_dataScaleFactorX);
            //structure expected is as given by iLumiCalc:
            //   RunNbr, AvergeInteractionPerXing, IntLumi
            UInt_t runNbr=0;Float_t intLumi=0;UInt_t lbn=0;TBranch *b_runNbr=0;TBranch *b_intLumi=0;TBranch *b_lbn=0;
            Float_t mu=0.; TBranch *b_mu=0;
            if(tmp->SetBranchAddress("RunNbr",&runNbr,&b_runNbr)!=0) {
               Error("AddLumiCalcFile","Could not find RunNbr branch in Data TTree");throw std::runtime_error("Could not find RunNbr branch in Data TTree");
            }
            if(tmp->SetBranchAddress("AvergeInteractionPerXing",&mu,&b_mu)!=0) {
               Error("AddLumiCalcFile","Could not find AvergeInteractionPerXing branch in Data TTree");throw std::runtime_error("Could not find AvergeInteractionPerXing branch in Data TTree");
            }
            if(tmp->SetBranchAddress("IntLumi",&intLumi,&b_intLumi)!=0) {
               Error("AddLumiCalcFile","Could not find IntLumi branch in Data TTree");throw std::runtime_error("Could not find IntLumi branch in Data TTree");
            }
            if(tmp->SetBranchAddress("LBStart",&lbn,&b_lbn)!=0) {
               Error("AddLumiCalcFile","Could not find LBStart branch in Data TTree");throw std::runtime_error("Could not find LBStart branch in Data TTree");
            }
            long nEntries = tmp->GetEntries();
   
            for(long i=0;i<nEntries;i++) {
               b_runNbr->GetEntry(i);b_intLumi->GetEntry(i);b_mu->GetEntry(i);
               runNbr += m_lumicalcRunNumberOffset;
               //save the lumi by run and lbn 
               b_lbn->GetEntry(i);

               //before going any further, check the runnbr and lbn are ok
               if(!m_parentTool->runLbnOK(runNbr,lbn)) continue;

               Run& r = m_runs[runNbr];
               if(trigger=="None") {r.lumiByLbn[lbn].first += intLumi; r.lumiByLbn[lbn].second = mu;}
               //rescale the mu value  ... do this *after* having stored the mu value in the lumiByLbn map
               mu *= m_dataScaleFactorX; 
               //fill into input data histograms 
               //check if we need to create an empty histogram 
               
               if(r.inputHists.find(trigger) == r.inputHists.end()) {
                  r.inputHists[trigger] = CloneEmptyHistogram(runNbr,-1);
               }
               r.inputHists[trigger]->Fill(mu,intLumi);
            }
            m_countingMode=false;
            m_lumiVectorIsLoaded=true;
         } else {
            Info("AddLumiCalcFile","Adding LumiMetaData for DataWeight (trigger=%s) (scale factor=%f)...",trigger.Data(),m_dataScaleFactorX);
         }
         
      }
   }

   delete rootFile;

   // Return to the original ROOT directory
   gDirectory = origDir;

   return 0;
}

Int_t CP::TPileupReweighting::AddConfigFile(const TString& fileName) {

   //open the file and look for config TTrees 
   //recognised TTrees are: ChannelMetaData, MCPileupReweighting, MCCustomReweighting, DataCustomReweighting

   if(m_isInitialized) {
      Error("AddConfigFile","You cannot AddConfigFile after initializing the tool. Reorder your code!");
      throw std::runtime_error("Throwing 5: You cannot AddConfigFile after initializing the tool. Reorder your code!");
   }

   TDirectory* origDir = gDirectory;
   // Load the data ROOT file
   TFile* rootFile = TFile::Open( fileName, "READ" );
   if ( rootFile->IsZombie() ) {
      Error("AddConfigFile","Could not open file: %s",fileName.Data());
      std::string toThrow = "Throwing 6: Could not open file: "; toThrow += fileName;
      throw std::runtime_error(toThrow);
   } else {
      //try to get the the known TTrees 
      TTree *tmp = (TTree*)rootFile->Get( std::string(m_prwFilesPathPrefix+"MCPileupReweighting").c_str() );
      if(tmp) {
         //Info("AddConfigFile","Adding MCPileupReweighting...");
         AddDistributionTree(tmp,rootFile);
         m_countingMode=false;
      }
      tmp = 0;tmp = (TTree*)rootFile->Get( std::string(m_prwFilesPathPrefix+"MCCustomReweighting").c_str() );
      if(tmp) {
         //Info("AddConfigFile","Adding MCCustomReweighting...");
         AddDistributionTree(tmp,rootFile);
         m_countingMode=false;
      }
      tmp = 0;tmp = (TTree*)rootFile->Get( std::string(m_prwFilesPathPrefix+"DataCustomReweighting").c_str() );
      if(tmp) {
         //Info("AddConfigFile","Adding DataCustomReweighting...");
         AddDistributionTree(tmp,rootFile);
         m_countingMode=false;
      }
      tmp=0; tmp = (TTree*)rootFile->Get( std::string(m_prwFilesPathPrefix+"PileupReweighting/MCPileupReweighting").c_str() ); //allow trees in the PileupReweighting dir instead
      if(tmp) {
         //Info("AddConfigFile","Adding MCPileupReweighting...");
         AddDistributionTree(tmp,rootFile);
         m_countingMode=false;
      }
      tmp = 0;tmp = (TTree*)rootFile->Get( std::string(m_prwFilesPathPrefix+"PileupReweighting/MCCustomReweighting").c_str() );
      if(tmp) {
         //Info("AddConfigFile","Adding MCCustomReweighting...");
         AddDistributionTree(tmp,rootFile);
         m_countingMode=false;
      }
      tmp = 0;tmp = (TTree*)rootFile->Get( std::string(m_prwFilesPathPrefix+"PileupReweighting/DataCustomReweighting").c_str() );
      if(tmp) {
         //Info("AddConfigFile","Adding DataCustomReweighting...");
         AddDistributionTree(tmp,rootFile);
         m_countingMode=false;
      }
   }

   delete rootFile;

   // Return to the original ROOT directory
   gDirectory = origDir;

   return 0;
}

Bool_t CP::TPileupReweighting::RemoveChannel(int chanNum) {
   if(m_isInitialized) {
      Error("RemoveChannel","You cannot RemoveChannel after initializing the tool. Reorder your code!");
      throw std::runtime_error("Throwing 5: You cannot RemoveChannel after initializing the tool. Reorder your code!");
   }

   bool found(false);
   for(auto& period : m_periods) {
      auto itr = period.second->inputHists.begin();
      while(itr != period.second->inputHists.end()) {
         if(itr->first!=chanNum) ++itr;
         else {
            found=true;
            if(itr->second) delete itr->second; //delete the hist 
            itr = period.second->inputHists.erase(itr);
         }
      }
   }
   return found;
}

Int_t CP::TPileupReweighting::Initialize() {

   //Need to calculate a period weight for each period
   //Need a reweighting histogram for each period 
   //for merged mc run numbers, we must generate or modify the existing mc distributions 



   //print out the period assignments if in debug mode 
   if(m_debugging) {
      PrintPeriods();
   }


   if(m_countingMode) {
      Info("Initialize","In Config File Generating mode. Remember to call WriteToFile!");
      //need to check no periods have subperiods, this is not allowed in counting mode 
      for(auto period : m_periods) {
         if(period.second->subPeriods.size()!=0) {
            Error("Initialize","You cannot have subperiods when in Config File Generating mode");
            throw std::runtime_error("Throwing 44: In Config File Generating mode, but detected subperiods in the period definition. This is not allowed.");
         }
      }
     //histograms are instantiated in the event loop as the channels occur
      //can delay period definition here
      //but since we are here, check that the user has at least defined some periods .. i.e. 
//       if(m_periods.size()==1) {
//          Error("Initialize", "In Config File Generating mode, but no periods have been defined. This makes no sense!? Define some periods please (with AddPeriod method)!");
//          throw std::runtime_error("Throwing 43: In Config File Generating mode, but no periods have been defined. This makes no sense!? Define some periods please (with AddPeriod method)!");
//       }
      m_isInitialized=true;
      return 0;
   }

   //find all channels that have too much unrepresented data (more than tolerance). We will remove these channels entirely 

   double totalData(0);
   //count up the unrepresented data, indexed by channel 
   for(auto& run : m_runs) {
      if(run.second.inputHists.find("None") == run.second.inputHists.end()) continue;
      TH1* hist = run.second.inputHists["None"];
      Int_t bin,binx,biny;
      for(biny=1; biny<=hist->GetNbinsY(); biny++) {
         for(binx=1; binx<=hist->GetNbinsX(); binx++) {
            bin = hist->GetBin(binx,biny);
            Double_t value = hist->GetBinContent(bin);
            totalData += value;
            if(value==0.) continue;
            bool isUnrep(false);
            std::map<Int_t, bool> doneChannels;
            for(auto& period : m_periods) {
               if(period.first != period.second->id) continue; //skip remappings
               if(!period.second->contains(run.first)) continue;
               for(auto& inHist : period.second->inputHists) {
                  if(inHist.first<0) continue; //skips any data hists (shouldn't happen really)
                  if((inHist.second)->GetBinContent(bin)==0) { 
                     period.second->unrepData[inHist.first] += value;  //store unrep data by period too, because will use in GetUnrepresentedFraction 
                     if(!isUnrep) {isUnrep=true; unrepDataByChannel[-1] += value;}    //store total unrep data... must be sure not to double count across the channels (if multiple channels have same unrep data
                  }
                  if(doneChannels[inHist.first]) continue; //dont doublecount the data if the channel was to blame across multiple periods
                  if((inHist.second)->GetBinContent(bin)==0) {unrepDataByChannel[inHist.first] += value;doneChannels[inHist.first]=true;  }
               }
            }
         }
      }
   }

   if(m_ignoreBadChannels && unrepDataByChannel[-1] && totalData && (unrepDataByChannel[-1]/totalData)>m_unrepDataTolerance) {
      Warning("Initialize","There is %f%% unrepresented data and 'IgnoreBadChannels' property is set to true. Will start ignoring channels until this is below the tolerance (%f%%)", 100.*(unrepDataByChannel[-1]/totalData),100.*m_unrepDataTolerance);
      //remove channels one-by-one until we are under tolerance 
      while( (unrepDataByChannel[-1]/totalData)>m_unrepDataTolerance ) {
         int worstChannel = -1;
         double worstFraction = 0;
         for(auto& channel : unrepDataByChannel) {
            if(channel.first<0) continue; //ignore the data 
            if(channel.second/totalData > worstFraction) { worstChannel=channel.first; worstFraction = channel.second/totalData; }
         }
         if(worstChannel==-1) {
            Error("Initialize","Run out of channels to remove. All your channels are bad ... oh dear oh dear oh dear. Please a better PRW config file");
            throw std::runtime_error("Throwing exception 333: Run out of channels to remove. All your channels are bad ... oh dear oh dear oh dear. Please a better PRW config file");
         }

         Warning("Initialize","Ignoring channel %d, which has %f%% unrepresented data (%f%% of the total unrep data)",worstChannel,100.*unrepDataByChannel[worstChannel]/totalData,100.*unrepDataByChannel[worstChannel]/unrepDataByChannel[-1]);
         //remove the bad channel 
         RemoveChannel(worstChannel);
         unrepDataByChannel.erase(unrepDataByChannel.find(worstChannel));

         //recalculate the total unrep data 
         unrepDataByChannel[-1] = 0;
         for(auto& run : m_runs) {
            if(run.second.inputHists.find("None") == run.second.inputHists.end()) continue;
            TH1* hist = run.second.inputHists["None"];
            Int_t bin,binx,biny;
            for(biny=1; biny<=hist->GetNbinsY(); biny++) {
               for(binx=1; binx<=hist->GetNbinsX(); binx++) {
                  bin = hist->GetBin(binx,biny);
                  Double_t value = hist->GetBinContent(bin);
                  if(value==0.) continue;
                  for(auto& period : m_periods) {
                     if(period.first != period.second->id) continue; //skip remappings
                     if(!period.second->contains(run.first)) continue;
                     bool done(false);
                     for(auto& inHist : period.second->inputHists) {
                        if(inHist.first<0) continue; //skips any data hists (shouldn't happen really)
                        if((inHist.second)->GetBinContent(bin)==0) { 
                           unrepDataByChannel[-1] += value; done=true; break;
                        }
                     }
                     if(done) break;
                  }
               }
            }
         }
      }
   }


   //need a CompositeTrigger to hold the unprescaled lumi hists ...
   m_triggerObjs["None"] = new CompositeTrigger;

   //go through all periods, use inputHists to populate the primary and secondary dists, and fill sumOfWeights and numberOfEntries
   for(auto period : m_periods) {
      if(period.first != period.second->id) continue; //skips remappings 
      for(auto& inputHist : period.second->inputHists) {
         int channel = inputHist.first;
         TH1* hist = inputHist.second;
         period.second->sumOfWeights[channel] += hist->GetSumOfWeights();
         period.second->numberOfEntries[channel] += hist->GetEntries();
         if(hist->GetDimension()==1) {
            period.second->primaryHists[channel] = dynamic_cast<TH1D*>(CloneEmptyHistogram(period.first,channel));
            period.second->primaryHists[channel]->Add(hist);
         }
      }
      //also add all runs that belong to us ... only do the "None" trigger
      for(auto& run : m_runs) {
         if(!period.second->contains(run.first)) continue;
         if(run.second.inputHists.find("None") == run.second.inputHists.end()) continue;
         //add to list of runNumbers for this period 
         period.second->runNumbers.push_back(run.first);
         TH1* hist = run.second.inputHists["None"];

         //sweep over bins, checking for unrepresented data and acting accordingly 
         Int_t bin,binx,biny;
         for(biny=1; biny<=hist->GetNbinsY(); biny++) {
            for(binx=1; binx<=hist->GetNbinsX(); binx++) {
               bin = hist->GetBin(binx,biny);
               Double_t value = hist->GetBinContent(bin);
               if(value==0.) continue;
               //loop over channels, check for zero in this bin  ... only need to do if we haven't checked ourselves 
               if(period.second->inputBinRedirect.find(bin) == period.second->inputBinRedirect.end()) {
                  for(auto& inputHist : period.second->inputHists) {
                     if(inputHist.first<0) continue; //skips data
                     Double_t mcValue = (inputHist.second)->GetBinContent(bin);
                     if(mcValue==0.) { 
                        if(m_unrepresentedDataAction!=0 && m_debugging) Info("Initialize","Unrepresented data at coords [%f,%f] caused by periodNumber %d in channel %d",hist->GetXaxis()->GetBinCenter(binx),hist->GetYaxis()->GetBinCenter(biny),period.first,inputHist.first);
                        //if we are doing unrepaction=2, just set to 'not the bin number'. if we are doing action=3, find the nearest good bin 
                        if(m_unrepresentedDataAction==0) {period.second->inputBinRedirect[bin] = bin-1;Error("Initialize","Unrepresented data at coords [%f,%f] caused by periodNumber %d in channel %d",hist->GetXaxis()->GetBinCenter(binx),hist->GetYaxis()->GetBinCenter(biny),period.first,inputHist.first);}
                        if(m_unrepresentedDataAction==1||m_unrepresentedDataAction==2) period.second->inputBinRedirect[bin] = bin-1;
                        else if(m_unrepresentedDataAction==3) period.second->inputBinRedirect[bin] = GetNearestGoodBin(period.first, bin);
                        break;
                     }
                  }
                  if(period.second->inputBinRedirect.find(bin) == period.second->inputBinRedirect.end()) period.second->inputBinRedirect[bin] = bin; //a good bin 
               }
               //if is a bad bin, act accordingly 
               if(period.second->inputBinRedirect[bin] != bin) {
                  run.second.badBins[bin]=value;
                  if(m_unrepresentedDataAction==1) {
                     //remove this data. we have to remember to do this for the trigger data too if this is the pileup weight
                     hist->SetBinContent(bin,0);
                     //also modify the m_inputTriggerHistograms ... 
                     for(auto& triggerDists : run.second.inputHists) triggerDists.second->SetBinContent(bin,0);
                  } else if(m_unrepresentedDataAction==3) {
                     //reassign the content to that bin 
                     hist->SetBinContent(period.second->inputBinRedirect[bin],hist->GetBinContent(period.second->inputBinRedirect[bin])+hist->GetBinContent(bin));
                     hist->SetBinContent(bin,0);
                     //also modify the m_inputTriggerHistograms ... 
                     for(auto& triggerDists : run.second.inputHists) {
                        triggerDists.second->SetBinContent(period.second->inputBinRedirect[bin],triggerDists.second->GetBinContent(period.second->inputBinRedirect[bin])+triggerDists.second->GetBinContent(bin));
                        triggerDists.second->SetBinContent(bin,0);
                     }
                   }
                }
            }
         }
         //create the period's 'data' hist if necessary 
         if( hist->GetDimension()==1 ) {
              if(!period.second->primaryHists[-1] )  {
                  period.second->primaryHists[-1] = dynamic_cast<TH1D*>(CloneEmptyHistogram(period.first,-1));
                  //period.second->triggerHists["None"] = dynamic_cast<TH1D*>(CloneEmptyHistogram(period.first,-1)); //this will remain unnormalized, unlike the 'primaryHist'
                  m_triggerObjs["None"]->triggerHists[period.second->id][0] = dynamic_cast<TH1D*>(CloneEmptyHistogram(period.first,-1)); //this will remain unnormalized, unlike the 'primaryHist'
               }
         }
         else if( hist->GetDimension()==2 ) {
            if(!period.second->secondaryHists[-1] ) {
               period.second->secondaryHists[-1] =  dynamic_cast<TH2D*>(CloneEmptyHistogram(period.first,-1));
               period.second->primaryHists[-1] = period.second->secondaryHists[-1]->ProjectionX(); period.second->primaryHists[-1]->SetDirectory(0);
               period.second->primaryHists[-1]->Reset();
               //period.second->triggerHists["None"] = static_cast<TH1D*>(period.second->primaryHists[-1]->Clone("triggerHist"));
               //period.second->triggerHists["None"]->SetDirectory(0);
               m_triggerObjs["None"]->triggerHists[period.second->id][0] = static_cast<TH1D*>(period.second->primaryHists[-1]->Clone("triggerHist"));
               m_triggerObjs["None"]->triggerHists[period.second->id][0]->SetDirectory(0);
            }
            
         }

      }

   }


   //now that all periods and runs have been checked, with redirects set up where necessary, we actually accumulate the data across the runs 
   //we should also check if there are any data runs that are not covered by the period assignments 

   double ignoredData(0);
   for(auto& run : m_runs) {
      bool covered(false);
      TH1* hist = run.second.inputHists["None"];
      for(auto& period : m_periods) {
         if(!period.second->contains(run.first)) continue;
         if(period.first!=-1) covered=true; //don't count the global period
         if(hist->GetDimension()==1) {
            period.second->primaryHists[-1]->Add(hist);
            m_triggerObjs["None"]->triggerHists[period.second->id][0]->Add(hist);//period.second->triggerHists["None"]->Add(hist);
         } else if(hist->GetDimension()==2) {
            period.second->secondaryHists[-1]->Add(hist);
            TH1* proj = ((TH2*)hist)->ProjectionX();
            period.second->primaryHists[-1]->Add(proj);
            m_triggerObjs["None"]->triggerHists[period.second->id][0]->Add(proj);//period.second->triggerHists["None"]->Add(proj);
            delete proj;
         }
         period.second->sumOfWeights[-1] += hist->GetSumOfWeights();
         period.second->numberOfEntries[-1] += hist->GetEntries();
      }
      if(!covered && m_periods.size()>1) {
         Warning("Initialize","loaded data in run %d that is not covered by period assignments",run.first);
         ignoredData += hist->GetSumOfWeights();
      } 
   }
   //double totalData =  (m_unrepresentedDataAction==1) ? (m_periods[-1]->sumOfWeights[-1]+unrepDataByChannel[-1]) : m_periods[-1]->sumOfWeights[-1];

   if(ignoredData>0.) Warning("Initialize", "Period Assignments missed %f%% data",100.*ignoredData/totalData);



   if(unrepDataByChannel[-1]) {
      double frac = unrepDataByChannel[-1]/totalData;
      if( frac  > m_unrepDataTolerance) {
               Error("Initialize", "%f%% unrepresented data, which suggests something is wrong with your prw config. Try EnableDebugging(true) to investigate",100.* (unrepDataByChannel[-1]/totalData));
      }
      if(m_unrepresentedDataAction==1) {
         Warning("Initialize","has %f%% unrepresented data. This was removed (UnrepresentedDataAction=1)",100.*frac);
      }  else if(m_unrepresentedDataAction==2) {
         Warning("Initialize","has %f%% unrepresented data. This was kept in (UnrepresentedDataAction=2)",100.*frac);
      } else if(m_unrepresentedDataAction==3) {
         Warning("Initialize","has %f%% unrepresented data. This was reassigned (UnrepresentedDataAction=3)",100.*frac);
      } else if(m_unrepresentedDataAction==0) {
         Error("Initialize","has %f%% unrepresented data:",100.*frac);
         //print the report of which channels caused it 
         for(auto& it : unrepDataByChannel) {
            if(it.first<0) continue; //ignore data
            Error("Initialize","   Channel %d caused %f%% of the unrepresented data (nEntries=%d)",it.first,100.*it.second/unrepDataByChannel[-1],m_periods[-1]->numberOfEntries[it.first]);
         }
         Error("Initialize","Exiting. You must decide how to proceed...");
         Error("Initialize","1) use AddPeriod or RemapPeriod to redefine the mc periods to include this data. You should not need to regenerate your mc config file");
         Error("Initialize","2) use SetUnrepresentedDataAction(1) to remove this data from the weight calculations. You should also veto such data events (using IsUnrepresentedData(..,..) method)");
         Error("Initialize","3) use SetUnrepresentedDataAction(2) to leave this data in the calculation. I hope you know what you're doing!!");
         Error("Initialize","4) use SetUnrepresentedDataAction(3) to reassign the data to the nearest representable bin");
         throw std::runtime_error("Throwing exception 22: Unrepresented data exists. Make a choice how to handle this. See err log for details");
      }
      //if get here, through the exception if frac is bad
      if( frac  > m_unrepDataTolerance) {
         throw std::runtime_error("Throwing exception 222: Some channel had too much unrepresented data. You should fix your prw file");
      }
   }


   if(m_debugging) Info("Initialize","Normalizing histograms and cleaning up...");
   //now that all the distributions are built. Normalize them all 
   for(auto period : m_periods) {
      if(period.first != period.second->id) continue;
      for(auto pHist : period.second->primaryHists) {
         normalizeHistogram(pHist.second);
      }
      for(auto pHist : period.second->secondaryHists) {
         normalizeHistogram(pHist.second);
      }
      for(auto pHist : period.second->inputHists) {
         delete pHist.second;
      }
      period.second->inputHists.clear();
   }

   //we keep the inputHists for data, because can use that to do random run numbers based on mu

   if(m_debugging) Info("Initialize","...Done");
   //if no input histograms were added, we are in counting mode
   

   m_isInitialized=true;
   
  return 0;
}

//loop over the mc histograms, find bin closest to given bin that is "ok" (has all nonzero)
//0 = ok bin, 1 = bad bin, 2 = out of range bin
Int_t CP::TPileupReweighting::IsBadBin(Int_t periodNumber, Int_t bin) {

   if(bin<0) return 2; //out of range 

   Period* p = m_periods[periodNumber];
   for(auto& inHist : p->inputHists) {
      int channelNumber = inHist.first;
      if(channelNumber<0) continue; //skip data hists 
      if(bin > (inHist.second->GetNbinsX()+2)*(inHist.second->GetNbinsY()+2)*(inHist.second->GetNbinsZ()+2)) return 2; //definitely out of range by this point!
      if(inHist.second->GetBinContent(bin)==0) return 1;
   }
   return 0;

}

//loop over the mc histograms, find bin closest to given bin that is "ok" (has all nonzero)
Int_t CP::TPileupReweighting::GetNearestGoodBin(Int_t thisMCRunNumber, Int_t bin) {

   int binDistance = 1;
   do {
      int res1 = IsBadBin(thisMCRunNumber,bin+binDistance);
      if(!res1) return bin+binDistance;
      int res2 = IsBadBin(thisMCRunNumber,bin-binDistance);
      if(!res2) return bin-binDistance;
      if(res1==2 && res2==2) { //run out of bins 
         Error("GetNearestGoodBin", "None of the bins are good!!??");
         return -1;
      }
      binDistance++;
   } while(true); //scary!

   return -1;

}

Double_t CP::TPileupReweighting::GetUnrepresentedDataFraction(Int_t periodNumber,Int_t channel) {

  //return unrepDataByChannel[channel]/GetSumOfEventWeights(-1); //the denominator is equivalent to asking for the total data!


   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetUnrepresentedDataFraction","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }

   //look for channel through sumOfWeights - because if the channel has no unrepData, it will have no entry in the unrepData map
   if(p->sumOfWeights.find(channel) == p->sumOfWeights.end()) channel = GetDefaultChannel(periodNumber);


   if(p->sumOfWeights.find(channel) == p->sumOfWeights.end()) {
      Error("GetUnrepresentedDataFraction","Unrecognised channel: %d",channel);
      throw std::runtime_error("GetUnrepresentedDataFraction: Unrecognised channel");
   }

   return p->unrepData[channel]/p->sumOfWeights[-1];

}



UInt_t CP::TPileupReweighting::GetRandomRunNumber(Int_t periodNumber) {
   if(!m_isInitialized) { Info("GetRandomRunNumber","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) { return 0; } //do nothing when in counting mode

   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetRandomRunNumber","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }

   double lumi = p->sumOfWeights[-1] * m_random3->Rndm();

   //go through the runs assigned to this period ... 
   double lumiSum=0;
   for(auto runNum : p->runNumbers) {
      lumiSum += m_runs[runNum].inputHists["None"]->GetSumOfWeights();
      if(lumiSum >= lumi-0.00001) return runNum; //small difference is to catch rounding errors
   }
   Error("GetRandomRunNumber","overran integrated luminosity for periodNumber=%d (%f vs %f)",periodNumber,lumiSum,lumi);
   throw std::runtime_error("Throwing 46.1: overran integrated luminosity for GetRandomRunNumber");
   return 0;
}

UInt_t CP::TPileupReweighting::GetRandomRunNumber(Int_t periodNumber, Double_t x) {
   if(!m_isInitialized) { Info("GetRandomRunNumber","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) { return 0; } //do nothing when in counting mode

   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetRandomRunNumber","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }

   int bin = m_emptyHistogram->FindFixBin(x);

   double lumi = p->primaryHists[-1]->GetBinContent(bin) * p->sumOfWeights[-1] * m_random3->Rndm();

   if(!lumi) { return 0; } //no lumi with that mu


   //go through the runs assigned to this period ... 
   double lumiSum=0;
   for(auto runNum : p->runNumbers) {
      lumiSum += m_runs[runNum].inputHists["None"]->GetBinContent(bin);
      if(lumiSum >= lumi-0.00001) return runNum;  //small difference is to catch rounding errors
   }
   Error("GetRandomRunNumber","overran integrated luminosity for periodNumber=%d (%f vs %f)",periodNumber,lumiSum,lumi);
   throw std::runtime_error("Throwing 46.1: overran integrated luminosity for GetRandomRunNumber");
   return 0;
}

UInt_t CP::TPileupReweighting::GetRandomLumiBlockNumber(UInt_t runNumber) {
   if(!m_isInitialized) {Info("GetRandomLumiBlockNumber","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) { return 0; } //do nothing when in counting mode

   double lumi = GetIntegratedLumi(runNumber,runNumber) * m_random3->Rndm() * 1E6 /* dont forget the lumi was divided by a million to get to pb */; 
   double lumisum = 0.;

   for(auto& lbn : m_runs[runNumber].lumiByLbn) {
      if(m_unrepresentedDataAction==1 && IsUnrepresentedData(runNumber,lbn.second.second)) continue; //skip over bad lumi 
      lumisum += lbn.second.first;
      if(lumisum >= lumi) return lbn.first;
   }
   Error("GetRandomLumiBlockNumber","overran integrated luminosity for RunNumber=%d",runNumber);
   throw std::runtime_error("Throwing 46: overran integrated luminosity for runNumber");
   return 0;
}

//only considers periods assigned directly!
Int_t CP::TPileupReweighting::GetRandomPeriodNumber(Int_t periodNumber) {
   if(!m_isInitialized) { Info("GetRandomPeriodNumber","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) { return 0; } //do nothing when in counting mode

   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetRandomPeriodNumber","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }

   //if no subperiods, we just return ourselves 
   if(p->subPeriods.size()==0) return p->id;

   //need the total lumi of this period
   double lumi = p->sumOfWeights[-1] * m_random3->Rndm();

   //loop over subPeriods .. 
   double lumiSum=0;
   for(auto subp : p->subPeriods) {
      lumiSum += subp->sumOfWeights[-1];
      if(lumiSum >= lumi) return subp->id;
   }

   Error("GetRandomPeriodNumber","overran integrated luminosity for periodNumber=%d",periodNumber);
   throw std::runtime_error("Throwing 46.1: overran integrated luminosity for GetRandomPeriodNumber");
   return 0;

}

Bool_t CP::TPileupReweighting::IsUnrepresentedData(Int_t runNumber, Float_t x, Float_t y) {
   int bin = m_emptyHistogram->FindFixBin(x,y);
   return ( m_runs[runNumber].badBins[bin] );
}

//this method builds a file get can be friended to a TTree with a prw-hash branch
Bool_t CP::TPileupReweighting::MakeWeightTree(TString channelNumbers, TString outFile, TString hashBranch, TString weightBranch) {
   if(!m_isInitialized) { Info("MakeWeightTree","Initializing the subtool.."); Initialize(); }
   TH1* hist = m_emptyHistogram;
   if(!hist) {
      Error("MakeWeightTree","Tool not configured properly ... please report this!");
      throw std::runtime_error("Throwing 47: Tool not configured properly ... please report this!");
   }

   //loop over given channels, and loop over all periods (except the global period) and get the pileup weight in each case for each bin 
   TFile f1(outFile,"RECREATE");

   TTree* outTree = new TTree("prwTree","prwTree");
   ULong64_t prwHash(0);Float_t weight(0.);
   outTree->Branch(hashBranch,&prwHash);
   outTree->Branch(weightBranch,&weight);

   TObjArray *tx = channelNumbers.Tokenize(",");
   for (Int_t i = 0; i < tx->GetEntries(); i++) {
      int channelNumber = ((TObjString *)(tx->At(i)))->String().Atoi();
      if(channelNumber>999999 || channelNumber<0) { 
         Error("MakeWeightTree","ChannelNumber can not be bigger than 999999 or less than 0 ... got %d",channelNumber);
         f1.Close();
         return 0;
      }
      for(auto& period : m_periods) {
         if(period.first==-1) continue;
         int periodNumber = period.first;
         for(int i=1;i<=hist->GetNbinsX();i++) {
            double x = hist->GetXaxis()->GetBinCenter(i);
            for(int j=1;j<=hist->GetNbinsY();j++) {
               double y = hist->GetYaxis()->GetBinCenter(j);
               weight = GetCombinedWeight(periodNumber,channelNumber,x,y);
               prwHash = GetPRWHash(periodNumber,channelNumber,x,y);
               outTree->Fill();
            }
         }

      }
   }

   outTree->BuildIndex(hashBranch.Data());
   outTree->Write();
   f1.Close();

   Info("MakeWeightTree","Successfully wrote prwTree to %s",outFile.Data());

   return true;
}

ULong64_t CP::TPileupReweighting::GetPRWHash(Int_t periodNumber, Int_t channelNumber, Float_t x, Float_t y) {
   if(!m_isInitialized) { Info("GetPRWHash","Initializing the subtool.."); Initialize(); }
   TH1* hist = m_emptyHistogram;
   if(!hist) {
      Error("GetPRWHash","Tool not configured properly ... please report this!");
      throw std::runtime_error("Throwing 47: Tool not configured properly ... please report this!");
   }

   ULong64_t out = hist->FindFixBin(x,y);
   out += (unsigned long)(channelNumber)*10000000000;
   out += (unsigned long)(periodNumber)*10000;

   return out;

}

Float_t CP::TPileupReweighting::GetCombinedWeight(Int_t periodNumber, Int_t channelNumber, Float_t x, Float_t y) {
   if(!m_isInitialized) { Info("GetCombinedWeight","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) return 0.;
   if(m_countingMode) {
      Error("GetCombinedWeight","You cannot get a weight when in config file generating mode. Please use FillWeight method");
      throw std::runtime_error("Throwing 2: You cannot get a weight when in config file generating mode. Please use FillWeight method");
   }
   //decide how many dimensions this weight has - use the emptyHistogram to tell... 
   TH1* hist = m_emptyHistogram;
   if(!hist) {
      Error("GetCombinedWeight","Tool not configured properly ... please report this!");
      throw std::runtime_error("Throwing 47: Tool not configured properly ... please report this!");
   }

   Float_t out = GetPeriodWeight(periodNumber,channelNumber)*GetPrimaryWeight(periodNumber,channelNumber,x);
   if(hist->GetDimension()>1) out *= GetSecondaryWeight(periodNumber,channelNumber,x,y);
   
   return out;
}

Float_t CP::TPileupReweighting::GetPeriodWeight(Int_t periodNumber, Int_t channelNumber) {
   //= L_A/L / N_A/N  
   if(!m_isInitialized) { Info("GetPeriodWeight","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) return 0.;

   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetPeriodWeight","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }
   if(p->sumOfWeights.find(channelNumber) == p->sumOfWeights.end()) channelNumber = GetDefaultChannel(periodNumber);//p->defaultChannel;

   double n_a = p->sumOfWeights[channelNumber];
   double n = m_periods[-1]->sumOfWeights[channelNumber];

   double l_a = p->sumOfWeights[-1];
   double l = m_periods[-1]->sumOfWeights[-1];

   return (l_a/l) / (n_a/n);
}

Float_t CP::TPileupReweighting::GetPrimaryWeight(Int_t periodNumber, Int_t channelNumber,Float_t x) {
   //= L_i/L_A / N_i/N_A .. primaryHists have already been normalized
   if(!m_isInitialized) { Info("GetPrimaryWeight","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) return 0.;

   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetPrimaryWeight","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }
   int oChanNumber = channelNumber;
   if(p->sumOfWeights.find(channelNumber) == p->sumOfWeights.end()) channelNumber = GetDefaultChannel(periodNumber);//p->defaultChannel;

   if(!p->primaryHists[channelNumber]) {
      Error("GetPrimaryWeight","Unrecognised channelNumber: %d",oChanNumber);
      throw std::runtime_error("Throwing 2: Unrecognised channelNumber");
   }

   int bin = p->primaryHists[channelNumber]->FindFixBin(x);

   double n = p->primaryHists[channelNumber]->GetBinContent(bin);

   if(!p->primaryHists[-1]) {
      Error("GetPrimaryWeight","No data loaded for period %d. Did you forget to load a lumicalc file or data config file?",periodNumber);
      throw std::runtime_error("Throwing 3: No data loaded. Did you forget to load a lumicalc file or data config file?");
   }

   double l = p->primaryHists[-1]->GetBinContent(bin);

   return l/n;
}

Float_t CP::TPileupReweighting::GetSecondaryWeight(Int_t periodNumber, Int_t channelNumber,Float_t x,Float_t y) {
   //= L_j/L_i / N_j/N_i .. secondary hists have already been normalized
   if(!m_isInitialized) { Info("GetSecondaryWeight","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) return 0.;

   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetSecondaryWeight","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }
   if(p->sumOfWeights.find(channelNumber) == p->sumOfWeights.end()) channelNumber = GetDefaultChannel(periodNumber);//p->defaultChannel;
   int bin = p->secondaryHists[channelNumber]->FindFixBin(x,y);
   double n = p->secondaryHists[channelNumber]->GetBinContent(bin);
   double l = p->secondaryHists[-1]->GetBinContent(bin);

   return l/n;
}

Double_t CP::TPileupReweighting::GetDataWeight(Int_t runNumber, const TString& trigger) {
   //special mu-independent version of GetDataWeight. Will just use the global luminosity
   m_doGlobalDataWeight=true;
   double out = GetDataWeight(runNumber, trigger, 0);
   m_doGlobalDataWeight=false;
   return out;
}

Double_t CP::TPileupReweighting::GetDataWeight(Int_t runNumber, const TString& trigger, Double_t x) {

   if(!m_isInitialized) { Info("GetDataWeight","Initializing the subtool.."); Initialize(); }
   if(m_countingMode) return 0.;
   if(m_countingMode) {
      Error("GetDataWeight","You cannot get a weight when in config file generating mode. Please use Fill method");
      throw std::runtime_error("GetDataWeight: You cannot get a weight when in config file generating mode. Please use Fill method");
   }
   //determine which period this run number is in
   Int_t periodNumber = GetFirstFoundPeriodNumber(runNumber);

   Period* p = m_periods[periodNumber];
   if(!p) {
      Error("GetDataWeight","Unrecognised periodNumber: %d",periodNumber);
      throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
   }

   //see if we already made this trigger hist 
   auto tobj = m_triggerObjs.find(trigger);
   

   if(tobj==m_triggerObjs.end()) {
      //try to do a subtrigger calculation we need to reopen all the lumicalc files and do the fiddily prescaled luminosity calculation
      //will throw errors if can't
      CalculatePrescaledLuminosityHistograms(trigger);
      tobj = m_triggerObjs.find(trigger);
   }
   
   //check 

   
   if(tobj->second->triggerHists.find(p->id) == tobj->second->triggerHists.end()) {
    Error("GetDataWeight","Could not find trigger %s in period %d",trigger.Data(),p->id);
    throw std::runtime_error("GetDataWeight: Could not find trigger 1");
   }
   
      
   //now we need to evaluate the trigger bits, and if necessary calculate the new trigger hist for the new bits combination
   long tbits = tobj->second->getBits(this);
   
   if(tbits==0) return 1; //no trigger passed
   
   auto dItr = tobj->second->triggerHists[p->id].find(tbits);
   
   TH1D* denomHist = 0;
   if(dItr == tobj->second->triggerHists[p->id].end()) {
      //possible that the tbits for this event have not been encountered before, so just redo the calculation ...
      calculateHistograms(tobj->second);
   
      denomHist = tobj->second->triggerHists[p->id][tbits];
      if(denomHist==0) {
        Error("GetDataWeight","Could not find trigger %s in period %d with bits %ld",trigger.Data(),p->id, tbits);
        throw std::runtime_error("GetDataWeight: Could not find trigger 2");
      }
   } else {
    denomHist = dItr->second;
   }

   
   TH1D* numerHist = m_triggerObjs["None"]->triggerHists[p->id][0];
   if(numerHist==0) {
      Error("GetDataWeight","Could not find unprescaled trigger in period %d",p->id);
      throw std::runtime_error("GetDataWeight: Could not find unprescaled trigger. Please AddLumiCalc with a 'None' trigger");
   }
   
   if(m_doGlobalDataWeight) return numerHist->Integral(0,numerHist->GetNbinsX()+1)/denomHist->Integral(0,denomHist->GetNbinsX()+1);

   Int_t bin=numerHist->FindFixBin(x*m_dataScaleFactorX); //MUST SCALE BY THE DATA SCALE FACTOR!
   
   if(!denomHist->GetBinContent(bin)) {
     if(m_doPrescaleWeight) return -1; //happens if trigger was disabled/unavailable for that mu, even though that mu is in the dataset
      Error("GetDataWeight","Unrecognised mu value %f ... are you sure you included all lumicalc files",x);
      throw std::runtime_error("GetDataWeight: Unrecognised mu value. Please AddLumiCalc enough lumi with 'None' trigger");
   }
   
   return numerHist->GetBinContent(bin)/denomHist->GetBinContent(bin);
}


Double_t CP::TPileupReweighting::GetPrescaleWeight(Int_t runNumber, const TString& trigger) {
   //special mu-independent version of GetPrescaleWeight. Will just use the global luminosity
   m_doGlobalDataWeight=true;
   double out = GetPrescaleWeight(runNumber, trigger, 0);
   m_doGlobalDataWeight=false;
   return out;
}

Double_t CP::TPileupReweighting::GetPrescaleWeight(Int_t runNumber, const TString& trigger, Double_t x) {

  m_doPrescaleWeight = true;
  double out = GetDataWeight(runNumber,trigger,x);
  m_doPrescaleWeight = false;
  if(out<=0) return 0; //happens when triggers disabled/unavailable for a given mu ... therefore the prescale weight is 0
  return 1./out;

}


//fills the appropriate inputHistograms
Int_t CP::TPileupReweighting::Fill(Int_t runNumber,Int_t channelNumber,Float_t w,Float_t x, Float_t y) {
   if(!m_isInitialized) { Info("Fill","Initializing the subtool.."); Initialize(); }
   //should only be given genuine mcRunNumbers if mc (channel>=0). We don't fill periodNumber distributions 

   TH1* hist = 0;

   //get the period if mc, get the data run if data 
   if(channelNumber>=0) {
      if(m_periods.find(runNumber)==m_periods.end()) {
	//if assume default binning is true, will add binning from 0 to 100 for this run number
	if(m_autoRunStart || m_autoRunEnd) {
	  AddPeriod(runNumber,m_autoRunStart,m_autoRunEnd);
	}
	 if(m_periods.find(runNumber)==m_periods.end()) {
	   Error("Fill","Unrecognised runNumber: %d.  Check your period configuration (AddPeriod or UsePeriodConfig)",runNumber);
	   throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
	 }
      }
      Period* p = m_periods[runNumber];
      if(!p) {
         Error("Fill","Unrecognised runNumber: %d.  Check your period configuration (AddPeriod or UsePeriodConfig) ... but should never have got here so please report this!",runNumber);
         throw std::runtime_error("Throwing 1: Unrecognised periodNumber");
      }
      if(p->inputHists.find(channelNumber)==p->inputHists.end()) {
         //need to create my period histogram 
         p->inputHists[channelNumber] = CloneEmptyHistogram(runNumber,channelNumber);
      }
      hist = p->inputHists[channelNumber];
   } else {
      Run& r = m_runs[runNumber];
      if(r.inputHists["None"]==0) r.inputHists["None"]=CloneEmptyHistogram(runNumber,channelNumber);
      hist = r.inputHists["None"];
   }

   if(!hist) {
      Error("Fill","Unknown [run,channel] = [%d,%d]",runNumber,channelNumber);
      throw std::runtime_error("Throwing 45: Fill:: Unknown [run,channel] ");
   }

   if(hist->GetDimension()==1) {
      return hist->Fill(x,w);
   } else if(hist->GetDimension()==2) {
      return (static_cast<TH2*>(hist))->Fill(x,y,w);
   } 
   return -1;
}

Int_t CP::TPileupReweighting::WriteToFile(const TString& fname) {
   
   if(!m_countingMode) {Warning("WriteToFile","Not in counting mode, so no file will be written");return 0;}


   //build a TTree with the correct mc structure, and dump the mc histogram info in to it
   //also build aggregate histograms across all channels - shouldn't be used as input histograms unless for a single channel
   TString filename = fname;
   filename += (filename=="") ? TString(this->GetName()) + ".prw.root" : "";
   
   //loop over the weights. "pileup" gets it own MCPileupReweighting ttree. ... since we got rid of weightNames in this version, everything goes in MCPileupReweighting now!
   //data goes in to DataCustomReweighting. other goes in to MCCustomReweighting 
   TFile* outFile = TFile::Open(filename,"RECREATE");
   Int_t r = WriteToFile(outFile);
   outFile->Close();
   return r;
}


Int_t CP::TPileupReweighting::WriteToFile(TFile* outFile) {
   if(!m_countingMode) {Warning("WriteToFile","Not in counting mode, so no file will be written");return 0;}

   TDirectory* origDir = gDirectory;

   outFile->cd();

   TTree *outTreeMC=0;
   TTree *outTreeData=0;
   Int_t channel = 0;UInt_t runNumber = 0; 
   std::vector<UInt_t>* pStarts = 0;std::vector<UInt_t>* pEnds = 0;
   Char_t histName[150];


   //loop over periods ... periods only get entry in table if they have an input histogram 
   for(auto period : m_periods) {
      if(period.first != period.second->id) continue; //skips redirects 
      if(period.first<0) continue; //avoid the global run number
      runNumber = period.first;
      pStarts = new std::vector<UInt_t>;pEnds = new std::vector<UInt_t>;
      if(period.second->subPeriods.size()==0) { 
         pStarts->push_back(period.second->start); pEnds->push_back(period.second->end); 
      }
      else {
         for(auto subp : period.second->subPeriods) {
            pStarts->push_back(subp->start); pEnds->push_back(subp->end);
         }
      }
      for(auto inHist : period.second->inputHists) {
         channel = inHist.first;
         TH1* hist = inHist.second;
         strncpy(histName,hist->GetName(),sizeof(histName)-1);
         hist->Write();

         if(!outTreeMC) {
            outTreeMC = new TTree("MCPileupReweighting","MCPileupReweighting");
            outTreeMC->Branch("Channel",&channel);
            outTreeMC->Branch("RunNumber",&runNumber);
            outTreeMC->Branch("PeriodStarts",&pStarts);
            outTreeMC->Branch("PeriodEnds",&pEnds);
            outTreeMC->Branch("HistName",&histName,"HistName/C");
         }
         outTreeMC->Fill();
      }
      delete pStarts; delete pEnds;
   }

   //loop over data 
   for(auto& run : m_runs) {
      runNumber = run.first;
      if(run.second.inputHists.find("None")==run.second.inputHists.end()) continue;

      TH1* hist = run.second.inputHists["None"];
      strncpy(histName,hist->GetName(),sizeof(histName)-1);
      hist->Write();
      if(!outTreeData) {
         outTreeData = new TTree("DataPileupReweighting","DataPileupReweighting");
         outTreeData->Branch("RunNumber",&runNumber);
         outTreeData->Branch("HistName",&histName,"HistName/C");
      }
      outTreeData->Fill();
   }


   //write the non-zero ttrees 
   if(outTreeMC) {outTreeMC->Write();delete outTreeMC;outTreeMC=0;}
   if(outTreeData) {outTreeData->Write();delete outTreeData;outTreeData=0;}


   Info("WriteToFile", "Successfully generated config file: %s",outFile->GetName());
   Info("WriteToFile", "Happy Reweighting :-)");

   gDirectory = origDir;

   return 0;
}


//=============================================================================
// Method to calculate the ratio histogram
//=============================================================================
void CP::TPileupReweighting::normalizeHistogram(TH1* hist){
   // normalize the data histogram based on which sort of histogram it is
   if(hist){ 
      if(hist->InheritsFrom("TH3")) {
         Error("normalizeHistogram","3D reweighting not supported yet");
         throw std::runtime_error("Throwing 3: 3D reweighting not supported yet");
      }
      else if(hist->InheritsFrom("TH2")) {
         bool skipNorm=false;
         //normalize each bin according to the projection in x 
         TH1D* proj = ((TH2*)hist)->ProjectionX();
         Int_t bin,binx,biny,binz;
         for(binz=1; binz<=hist->GetNbinsZ(); binz++) {
            for(biny=1; biny<=hist->GetNbinsY(); biny++) {
               for(binx=1; binx<=hist->GetNbinsX(); binx++) {
                  bin = hist->GetBin(binx,biny,binz);
                  Double_t value = hist->GetBinContent(bin);
                  Double_t normalizer = proj->GetBinContent(binx);
                  if(fabs(normalizer)>0.00001) {
                     hist->SetBinContent(bin,value/normalizer);
                  } else {
                     skipNorm=true;
                  }
               }
            }
         }
         delete proj;
         if(skipNorm && m_debugging) Warning("normalizeHistogram","Skipped normalization in hist %s",hist->GetName());
      } else {
         //normalize to the sum of weights 
         if(fabs(hist->GetSumOfWeights())>0.00001) {
            hist->Scale(1.0/hist->GetSumOfWeights());
         } else {
            if (m_debugging) Warning("normalizeHistogram","Skipping Normalizing histogram %s to ~zero: %f",hist->GetName(),hist->GetSumOfWeights());
         }
      } 
   } else {
      Error("normalizeHistogram","Non existent histogram for normalizing");throw std::runtime_error("Throwing 56");
   }
}


//This allows PROOF to merge the generated histograms before the WriteToFile or WriteCustomToFile calls
Int_t CP::TPileupReweighting::Merge(TCollection *coll) {
   if(!coll) return 0;
   if(coll->IsEmpty()) return 0;

   // Iterate over the elements of the collection:
   TIter next( coll );
   TObject* obj = 0;
   while( ( obj = next() ) ) {

      // Check that the element is of the right type:
      CP::TPileupReweighting* vobj = dynamic_cast< CP::TPileupReweighting* >( obj );
      if( ! vobj ) {
         Error( "Merge", "Unknown object type encountered: %s",obj->ClassName() );
         return 0;
      }

      //merge the inputHistograms ... all the periods should be identical 
      for(auto period : vobj->m_periods) {
         if(period.first != period.second->id) continue;
         for(auto& iHist : period.second->inputHists) {
            if(GetInputHistogram(period.first,iHist.first)==0) {
               m_periods[period.first]->inputHists[iHist.first] = dynamic_cast<TH1*>(iHist.second->Clone(iHist.second->GetName()));
               m_periods[period.first]->inputHists[iHist.first]->SetDirectory(0);
            } else {
               GetInputHistogram(period.first,iHist.first)->Add(iHist.second);
            }
         }
      }
      //also must remember to merge the runs too (where the data is held)
      for(auto& run : vobj->m_runs) {
         for(auto& iHist : run.second.inputHists) {
            if(m_runs[run.first].inputHists[iHist.first]==0) {
               m_runs[run.first].inputHists[iHist.first] = dynamic_cast<TH1*>(iHist.second->Clone(iHist.second->GetName()));
               m_runs[run.first].inputHists[iHist.first]->SetDirectory(0);
            } else {
               m_runs[run.first].inputHists[iHist.first]->Add(iHist.second);
            }
         }
      }
   }

   return 1;
}


void CP::TPileupReweighting::CalculatePrescaledLuminosityHistograms(const TString& trigger) {
   //check if this is a composite trigger (user has provided OR of triggers that are passed before prescale)
   
   TString triggerCopy = trigger; triggerCopy.ReplaceAll(" ",""); triggerCopy.ReplaceAll("&&","&");triggerCopy.ReplaceAll("||","|");
   CompositeTrigger* t = makeTrigger(triggerCopy);
   if(t==0) {
      Error("GetDataWeight","Error parsing composite trigger set: %s",trigger.Data());
      throw std::runtime_error("Could not parse composite trigger");
   }

   std::vector<TString> subTriggers;

   t->getTriggers(subTriggers); //fills the vector

   t->subTriggers = subTriggers; //cache the vector of triggers
   
   calculateHistograms(t);
   
   //save trigger object
   m_triggerObjs[trigger] = t;

}

void CP::TPileupReweighting::calculateHistograms(CompositeTrigger* t) {
   
   //now we need the trigger bits for this trigger for this event
   long tbits = t->getBits(this);
   //1. Open all the lumicalc files
   //2. For each entry in the 'None' lumicalc file, lookup the run and lb in the subTriggers, and get the prescales (if not found, assume trigger not in, prescale=infinity)
   //3. perform calculation of lumi as: Lumi*(1 - Prod_triggers(1-1/prescale)) .. sum this up into the output histogram
   //    Note: Put each lumi into the period that contains the run ( use  )

   TDirectory* origDir = gDirectory;

   std::map<TString, std::map<Int_t, std::map<Int_t, Float_t> > > prescaleByRunAndLbn;

   //Load the prescales for each lumicalc subTrigger 
   for(std::vector<TString>::iterator it = t->subTriggers.begin();it!=t->subTriggers.end();++it) {
      //Info("aa","subtrigger %s Int lumi is %f", it->Data(), m_triggerPrimaryDistributions[*it][1]->Integral());
      for(auto& fileName : m_lumicalcFiles[*it]) {
        TFile* rootFile = TFile::Open( fileName, "READ" );
        if ( rootFile->IsZombie() ) {
          Error("CalculatePrescaledLuminosityHistograms","Could not open file: %s",fileName.Data());
          std::string toThrow = "Throwing 6: Could not open file: "; toThrow += fileName;
          throw std::runtime_error(toThrow);
        } else {
          //try to get the the known TTrees 
          TTree *tmp = (TTree*)rootFile->Get( "LumiMetaData" );
          if(tmp) {
              //structure expected is as given by iLumiCalc:
              //   RunNbr, AvergeInteractionPerXing, IntLumi
              UInt_t runNbr=0;Float_t ps1=0;Float_t ps2=0; Float_t ps3=0;UInt_t lbn=0;TBranch *b_runNbr=0;TBranch *b_L1Presc=0;TBranch *b_L2Presc=0;TBranch *b_L3Presc=0;TBranch *b_lbn=0;
              if(tmp->SetBranchAddress("RunNbr",&runNbr,&b_runNbr)!=0) {
                Error("CalculatePrescaledLuminosityHistograms","Could not find RunNbr branch in Data TTree");throw std::runtime_error("Could not find RunNbr branch in Data TTree");
              }
              if(tmp->SetBranchAddress("L1Presc",&ps1,&b_L1Presc)!=0) {
                Error("CalculatePrescaledLuminosityHistograms","Could not find L1Presc branch in Data TTree");throw std::runtime_error("Could not find L1Presc branch in Data TTree");
              }
              if(tmp->SetBranchAddress("L2Presc",&ps2,&b_L2Presc)!=0) {
                Error("CalculatePrescaledLuminosityHistograms","Could not find L2Presc branch in Data TTree");throw std::runtime_error("Could not find L2Presc branch in Data TTree");
              }
              if(tmp->SetBranchAddress("L3Presc",&ps3,&b_L3Presc)!=0) {
                Error("CalculatePrescaledLuminosityHistograms","Could not find L3Presc branch in Data TTree");throw std::runtime_error("Could not find L3Presc branch in Data TTree");
              }
              if(tmp->SetBranchAddress("LBStart",&lbn,&b_lbn)!=0) {
                Error("CalculatePrescaledLuminosityHistograms","Could not find LBStart branch in Data TTree");throw std::runtime_error("Could not find LBStart branch in Data TTree");
              }
              long nEntries = tmp->GetEntries();
              for(long i=0;i<nEntries;i++) {
                b_runNbr->GetEntry(i);b_L1Presc->GetEntry(i);b_L2Presc->GetEntry(i);b_L3Presc->GetEntry(i);b_lbn->GetEntry(i);
                runNbr += m_lumicalcRunNumberOffset;
                //save the prescale by run and lbn 
                //if(runNbr==215643) 
                if(m_debugging) Info("...","prescale in [%d,%d] = %f %f %f", runNbr,lbn,ps1,ps2,ps3);
                if(ps1>0&&ps2>0&&ps3>0) prescaleByRunAndLbn[*it][runNbr][lbn] = ps1*ps2*ps3;
              }
          }
        }
       } //end of loop over lumicalc files for this trigger
   }

   for(auto& fileName : m_lumicalcFiles["None"]) {
    // Load the data ROOT file for the 'None' lumicalc file
    TFile* rootFile = TFile::Open( fileName, "READ" );
    if ( rootFile->IsZombie() ) {
        Error("CalculatePrescaledLuminosityHistograms","Could not open file: %s",fileName.Data());
        std::string toThrow = "Throwing 6: Could not open file: "; toThrow += fileName;
        throw std::runtime_error(toThrow);
    } else {
        //try to get the the known TTrees 
        TTree *tmp = (TTree*)rootFile->Get( "LumiMetaData" );
        if(tmp) {
          //structure expected is as given by iLumiCalc:
          //   RunNbr, AvergeInteractionPerXing, IntLumi
          UInt_t runNbr=0;Float_t intLumi=0;UInt_t lbn=0;TBranch *b_runNbr=0;TBranch *b_intLumi=0;TBranch *b_lbn=0;
          Float_t mu=0.; TBranch *b_mu=0;
          if(tmp->SetBranchAddress("RunNbr",&runNbr,&b_runNbr)!=0) {
              Error("CalculatePrescaledLuminosityHistograms","Could not find RunNbr branch in Data TTree");throw std::runtime_error("Could not find RunNbr branch in Data TTree");
          }
          if(tmp->SetBranchAddress("AvergeInteractionPerXing",&mu,&b_mu)!=0) {
              Error("CalculatePrescaledLuminosityHistograms","Could not find AvergeInteractionPerXing branch in Data TTree");throw std::runtime_error("Could not find AvergeInteractionPerXing branch in Data TTree");
          }
          if(tmp->SetBranchAddress("IntLumi",&intLumi,&b_intLumi)!=0) {
              Error("CalculatePrescaledLuminosityHistograms","Could not find IntLumi branch in Data TTree");throw std::runtime_error("Could not find IntLumi branch in Data TTree");
          }
          if(tmp->SetBranchAddress("LBStart",&lbn,&b_lbn)!=0) {
              Error("CalculatePrescaledLuminosityHistograms","Could not find LBStart branch in Data TTree");throw std::runtime_error("Could not find LBStart branch in Data TTree");
          }
          long nEntries = tmp->GetEntries();
          //this loop ends up calling the triggerBeforePrescale method each time, but this should hopefully stop once the loading is done
          for(long i=0;i<nEntries;i++) {
              b_runNbr->GetEntry(i);b_intLumi->GetEntry(i);b_mu->GetEntry(i);
              b_lbn->GetEntry(i);
  
              runNbr += m_lumicalcRunNumberOffset;
              //for each subtrigger, lookup prescale, and calculate pFactor 
  /*
              double pFactor(1);
              for(std::vector<TString>::iterator it = subTriggers.begin();it!=subTriggers.end();++it) {
                if(prescaleByRunAndLbn[*it][runNbr].find(lbn) != prescaleByRunAndLbn[*it][runNbr].end()) {
                    pFactor *= (1. - 1./prescaleByRunAndLbn[*it][runNbr][lbn]);
                }
              }
              pFactor = 1. - pFactor;
  */
              
              double pFactor = t->eval(prescaleByRunAndLbn,runNbr,lbn,this);
  
              //Info("...","prescale in [%d,%d] = %f", runNbr,lbn,1./pFactor);
  
              int bin = m_emptyHistogram->FindFixBin(mu*m_dataScaleFactorX);
  
              //add into all periods that contain this runNbr 
              for(auto p : m_periods) {
                if(p.first != p.second->id) continue; //skips remappings 
                if(!p.second->contains(runNbr)) continue;
                
                
                
                
                std::map<long,TH1D*>& triggerHists = t->triggerHists[p.second->id];
                if(triggerHists.find(tbits) == triggerHists.end()) {
                    triggerHists[tbits] = dynamic_cast<TH1D*>(CloneEmptyHistogram(p.first,-1));
                    if(m_debugging) Info("CalculatePrescaledLuminosityHistograms","Created Data Weight Histogram for [%s,%d,%ld]",t->val.Data(),p.first, tbits);
                }
                  //check if we were about to fill a bad bin ... if we are, we either skipp the fill (unrep action=1) or redirect (unrep action=3)
                if( (m_unrepresentedDataAction==1) && p.second->inputBinRedirect[bin]!=bin) { } //do nothing
                else if( m_unrepresentedDataAction==3 ) {triggerHists[tbits]->Fill(triggerHists[tbits]->GetBinCenter(p.second->inputBinRedirect[bin]), intLumi*pFactor);}
                else triggerHists[tbits]->Fill(mu*m_dataScaleFactorX,intLumi*pFactor);
                
              }
          }
        }
    }
    delete rootFile;
   } //end loop over unprescaled lumicalc files
   

   //Info("aa","Int lumi is %f", m_triggerPrimaryDistributions[trigger][1]->Integral());



   // Return to the original ROOT directory
   gDirectory = origDir;


}

CP::TPileupReweighting::CompositeTrigger* CP::TPileupReweighting::makeTrigger(const TString& s) {
   if(m_debugging) Info("makeTrigger","Doing %s",s.Data());
   //find the first operand
   TString oper1; CompositeTrigger* cOper1 = 0;
   if(s.BeginsWith("(")) {
      //find closing bracket
      int bCounter=1; int i=1;
      while(bCounter!=0 && i<=s.Length()) {
         if(s(i)=='(') bCounter++;
         else if(s(i)==')') bCounter--;
         i++;
      }
      if(bCounter!=0) { Error("makeTrigger","Missing closing bracket"); return 0; }
      oper1 = s(1,i-2);
      if(i==s.Length()+1) return makeTrigger(oper1); //meainingless brackets, just return evaluation
      else {cOper1 = makeTrigger(oper1); if(cOper1==0) return 0; }
   }
   //find the second operand. Do this by finding the first operation at this level. if cannot find, then just set val
   int i=0;int bCounter=0; int op=0;
   while(i<=s.Length()) {
      if(s(i)=='(') bCounter++;
      else if(s(i)==')') bCounter--;
      else if(s(i)=='&' && bCounter==0) { op=2; break;}
      else if(s(i)=='|' && bCounter==0) { op=1; break;}
      i++;
   }


   if(op==0) {
      if(m_lumicalcFiles.find(s)==m_lumicalcFiles.end()) {
         Error("GetDataWeight","Could not find subTrigger %s",s.Data());
         return 0;
      }
      if(cOper1) return cOper1; 
      CompositeTrigger* out = new CompositeTrigger;
      out->op=op;out->val = s; return out; //just a value trigger
   }

   CompositeTrigger* out = new CompositeTrigger;
   out->op=op;

   if(op==1) { //an OR 
      if(cOper1) out->trig1=cOper1;  
      else {oper1=s(0,i); out->trig1=makeTrigger(oper1); if(out->trig1==0) { delete out; return 0; } }
      TString oper2 = s(i+1,s.Length()); out->trig2 = makeTrigger(oper2); if(out->trig2==0) { delete out; return 0; } 
      return out;
   }

   //got here, must be an AND keep going until we hit an OR at this level, then make all that the first operand (delete cOper if necessary)
   int j=i;bCounter=0;
   while(j<=s.Length()) {
      if(s(j)=='(') bCounter++;
      else if(s(j)==')') bCounter--;
      else if(s(j)=='|' && bCounter==0) { op=1; break;}
      j++;
   }
   if(j==s.Length()+1 && op==2) { //no more OR found, set oper2 to remainder 
      if(cOper1) out->trig1=cOper1;  
      else {oper1=s(0,i); out->trig1=makeTrigger(oper1); }
      TString oper2 = s(i+1,s.Length()); if(m_debugging) Info("makeTrigger","Found & %s %s",oper1.Data(),oper2.Data());
      out->trig2 = makeTrigger(oper2); if(out->trig2==0) { delete out; return 0; } 
      return out;
   } else if(op==1) {   //found an OR, set oper1 to everything up to this 
      
      oper1 = s(0,j); TString oper2 = s(j+1,s.Length()); if(m_debugging) Info("makeTrigger","Found & then | %s %s",oper1.Data(),oper2.Data());
      if(cOper1) delete cOper1;
      out->op=op; //updates to an OR
      out->trig1=makeTrigger(oper1); if(out->trig1==0) { delete out; return 0; }
      out->trig2=makeTrigger(oper2); if(out->trig2==0) { delete out; return 0; } 
      return out;
   }
   Error("makeTrigger","Should never have got here!, but did with %s", s.Data());
   delete out;
   return 0;
}



