/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __TPILEUPREWEIGHTING__
#define __TPILEUPREWEIGHTING__

/**
   @class TPileupReweighting
   @brief Tool to get the calculated MC pileup weight. Also does custom weights and other useful stuff.

   @author  Will Buttinger
            Based on the original tool by Karsten Koeneke
            Thanks to Nick Edwards for testing and useful feedback

*/

/* Developer notes:
* Could have incorporated the pileup histos in to the generic custom maps
* but decided to keep them seperate for probable slight performance improvement
* Allow the user to change the histo binning through AddBinning with name="pileup"
* */

#include "TNamed.h"
#include <TFile.h>
#include <TString.h>
#include "TVectorD.h"
#include <map>
#include <vector>
#include <TRandom3.h>

// STL includes
#include <iostream>
#include <stdexcept>
#include <memory>
#include <list>

#include <TH1.h>
#include <TH2.h>


class TTree;
class TFile;



namespace CP {
  class TPileupReweighting : public TNamed {

  public: 
      /** Standard constructor */
      TPileupReweighting(const char* name="TPileupReweighting");

  public:

      /** use a hardcoded period configuration */
      Int_t UsePeriodConfig(const TString& configName);
      /** Add a histogram binning config. To modify the pileup histo binning, use "pileup" as name */
      Int_t SetBinning(Int_t nbinsx, Double_t* xbins, Int_t nbinsy=0, Double_t* ybins=0);
      Int_t SetUniformBinning(Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy=0, Double_t ylow=0, Double_t yup=0);
      Int_t SetBinning(TH1* hist);

      /** Set which channel should be used as a default when specific mc channel distributions cannot be found */
      /** default channels can now be assigned for each mc run number .. mcRunNumber=-1 is the global default */
      void SetDefaultChannel(Int_t channel, Int_t mcRunNumber=-1);
      Int_t GetDefaultChannel(Int_t mcRunNumber=-1);

      /** total luminosity loaded and accepted by the tool (in inverse pb) */ 
      Double_t GetIntegratedLumi(const TString& trigger="");
      /** totalMC maps should also hold the total number of entries for each channel */
      inline Double_t GetNumberOfEvents(Int_t channel) {
         Period* global = m_periods[-1];
         if(!global) return 0;
         if(global->numberOfEntries.find(channel)==global->numberOfEntries.end()) {
            Error("GetNumberOfEvents", "Unknown channel: %d",channel);
            return 0;
         }
         return global->numberOfEntries[channel];
      }
      inline Double_t GetSumOfEventWeights(Int_t channel) {
         Period* global = m_periods[-1];
         if(!global) return 0;
         if(global->sumOfWeights.find(channel)==global->sumOfWeights.end()) {
            Error("GetSumOfEventWeights", "Unknown channel: %d",channel);
            return 0;
         }
         return global->sumOfWeights[channel];
      }

      /** Combine two period numbers. Histos are merged and the first number will be redirected to the second (the second is created if it doesn't exist) */
      void RemapPeriod(Int_t periodNumber1, Int_t periodNumber2);

      
      /** return fraction of lumi assigned to periodNumber (or mcRunNumber) that is between start and end data run numbers*/
      Double_t GetIntegratedLumiFraction(Int_t periodNumber, UInt_t start, UInt_t end);
      /** return fraction of lumi assigned to periodNumber (or mcRunNumber) with given mu, that is between start and end data run numbers*/
      Double_t GetIntegratedLumiFraction(Int_t periodNumber, Double_t mu, UInt_t start, UInt_t end);

      /** get the amount of integrated lumi between the two given runs */
      inline Double_t GetIntegratedLumi(UInt_t start, UInt_t end) {    return GetIntegratedLumi(-1,start,end); }
      /** similar to above, but for only the given mcRunNumber/periodNumber */
      Double_t GetIntegratedLumi(Int_t periodNumber, UInt_t start, UInt_t end);

      /** get integrated lumi for specific run and lumiblock number .. comes from the 'unprescaled lumi', and is in pb*/
      Double_t GetLumiBlockIntegratedLumi(Int_t runNumber, UInt_t lb);

      /** get the lumiblock mu, useful for 'updating' the mu coming from data to account for new lumitags */
      Float_t GetLumiBlockMu(Int_t runNumber, UInt_t lb);

      //-----------------------------------------------------
      //General Tool Configuring methods
      //-----------------------------------------------------
      /** Indicate if warnings should be suppressed */
      inline void DisableWarnings(Bool_t in) { m_SetWarnings = !in;}
      /** Indicate if additional debugging information should be output */
      inline void EnableDebugging(Bool_t in) { m_debugging = in;}

      /** Set how to handle configurations where some of your data has no corresponding mc to describe it 
          0=Default (Throw exception), 1=Subtract lumi from normalizations (i.e. discard data), 2=keep lumi and continue*/
      inline void SetUnrepresentedDataAction(Int_t action, Double_t tolerance=0.05) {
         if(action<0 || action>3) {
            Fatal("SetUnrepresentedDataAction","Set action=%d - I'm afraid I can't let you do that, Dave",action);
            throw std::runtime_error("Throwing 2001");
         } 
         m_unrepresentedDataAction=action;
         m_unrepDataTolerance=tolerance; //applicable for action=2 mode. Default is 5%
      }
      /** return the unrepresented data fraction in a given channel .. when using action=2, you will want to scale up all MC events by 1/(1-unrepFraction) to account for missed data */
      Double_t GetUnrepresentedDataFraction(Int_t periodNumber,Int_t channel);
      inline Float_t GetUnrepresentedDataWeight(Int_t periodNumber,Int_t channel) { 
         if(m_unrepresentedDataAction!=2) {
            Warning("GetUnrepresentedDataWeight","You should not be applying this weight unless the UnrepresentedDataAction=2");
         }
         return 1./(1. - GetUnrepresentedDataFraction(periodNumber,channel));
      }
      //-----------------------------------------------------
      //Methods to veto data in Action=1 mode
      //-----------------------------------------------------
      Bool_t IsUnrepresentedData(Int_t runNumber, Float_t x, Float_t y=0.);

      /** Should the tool ignore period assignments encoded in config file */
      inline void IgnoreConfigFilePeriods(Bool_t in) { m_ignoreFilePeriods=in; }

      /** Assign an mc RunNumber to a data period */
      Int_t AddPeriod(Int_t periodNumber, UInt_t start, UInt_t end);
      /** Get the first period number with the data run number contained - assume all periods are disconnected for this to be useful */
      Int_t GetFirstFoundPeriodNumber(UInt_t runNumber);



      /** Scale the LumiMetaData mu values by this factor */
      inline void SetDataScaleFactors(Float_t x,Float_t y=1.) { m_dataScaleFactorX=x;m_dataScaleFactorY=y; }
      inline void SetMCScaleFactors(Float_t x,Float_t y=1.) { m_mcScaleFactorX=x;m_mcScaleFactorY=y;}



      //-----------------------------------------------------
      //Methods to load config files
      //-----------------------------------------------------
      Int_t AddConfigFile(const TString& fileName);
      Int_t AddLumiCalcFile(const TString& fileName, const TString& trigger="None");
      Int_t AddMetaDataFile(const TString& fileName,const TString& channelBranchName="mc_channel_number");

      /** Removes a channel from the inputs ... this is for experts only */
      Bool_t RemoveChannel(int chanNum);

      /** Initialize this class once before the event loop starts 
          If distribution information is provided, it is assumed to be 
          for the standard pileup reweighting */
      Int_t Initialize();


      //-----------------------------------------------------
      //Methods to get the various weights
      //-----------------------------------------------------
      Float_t GetCombinedWeight(Int_t periodNumber, Int_t channelNumber,Float_t x,Float_t y=0.);
      Float_t GetPeriodWeight(Int_t periodNumber, Int_t channelNumber);
      Float_t GetPrimaryWeight(Int_t periodNumber, Int_t channelNumber,Float_t x);
      Float_t GetSecondaryWeight(Int_t periodNumber, Int_t channelNumber,Float_t x,Float_t y);

      //-----------------------------------------------------
      //Methods to work with the metadata
      //-----------------------------------------------------
      Double_t GetMetaData(const TString& metadataName,Int_t channelNumber) {
         if(m_metadata.find(metadataName)==m_metadata.end()) {
            Error("GetMetaData","Metadata %s not known",metadataName.Data());
            return 0;
         }
         if(m_metadata[metadataName].find(channelNumber)==m_metadata[metadataName].end()) {
            Error("GetMetaData","Metadata %s not known in channel %d",metadataName.Data(), channelNumber);
            return 0;
         }
         return m_metadata[metadataName][channelNumber];
      }
      /** combines loaded metadata with channel sumsofweights and entry counts */
      TTree* GetMetaDataTree(); 
      Int_t GenerateMetaDataFile(const TString& fileName,const TString& channelBranchName="mc_channel_number");


      //-----------------------------------------------------
      //Methods to generate config files
      //-----------------------------------------------------
      Int_t Fill(Int_t runNumber,Int_t channelNumber,Float_t w,Float_t x, Float_t y=0.);
      Int_t WriteToFile(const TString& filename=""); //if no name given, will use tool name
      Int_t WriteToFile(TFile* outFile);


      //
      //Methods used to do the prwTree - for fast recalculation of prw weights when processing ntuples
      //
      ULong64_t GetPRWHash(Int_t periodNumber, Int_t channelNumber, Float_t x, Float_t y=0.);
      Bool_t MakeWeightTree(TString channelNumbers, TString outFile, TString hashBranch="PRWHash", TString weightBranch="PileupWeight");

      //-----------------------------------------------------
      //RandomDataPeriod functionality stuff
      //numbers generated seperately for each mc period
      //-----------------------------------------------------
      void SetRandomSeed(int seed) {m_random3->SetSeed(seed);}
      int GetRandomSeed() {return m_random3->GetSeed();}
      /** Gets a random data run number according to the integrated lumi distribution associated to this mcRunNumber */
      /** allows use of custom reweighting config files  */
      UInt_t GetRandomRunNumber(Int_t mcRunNumber);
      /** Get random run number according to integrated lumi distribution for the given mu value (uses the binning of the mu histogram) */
      UInt_t GetRandomRunNumber(Int_t mcRunNumber, Double_t mu);
      /** Get random period number from the sub-periods assigned to this run number */
      Int_t GetRandomPeriodNumber(Int_t mcRunNumber);
      /** Get a random lumi block from the run number given. Use GetRandomRunNumber to choose the run number */
      UInt_t GetRandomLumiBlockNumber(UInt_t runNumber);


      //-----------------------------------------------------
      //Methods for PROOF cluster merging of generated histos
      //-----------------------------------------------------
      Int_t Merge(TCollection *coll);
      //*std::map<Int_t,std::map<Int_t, TH1*> > & GetInputHistograms() { return m_inputHistograms;}


      std::vector<int> GetPeriodNumbers() const {
        std::vector<int> out;
        for(auto& p : m_periods) { out.push_back(p.first); }
        return out;
      }

      //-----------------------------------------------------
      //Methods to inspect the input and weighting histograms
      //-----------------------------------------------------
      TH1* GetInputHistogram(Int_t channelNumber, Int_t periodNumber) {
         if(m_periods.find(periodNumber)==m_periods.end()||
            m_periods[periodNumber]->inputHists.find(channelNumber)==m_periods[periodNumber]->inputHists.end()) {
            return 0;
         }
         return m_periods[periodNumber]->inputHists[channelNumber].get();
      }

      TH1* GetPrimaryDistribution(Int_t channelNumber, Int_t periodNumber) {
         if(m_periods.find(periodNumber)==m_periods.end()||
            m_periods[periodNumber]->primaryHists.find(channelNumber)==m_periods[periodNumber]->primaryHists.end()) {
            return 0;
         }
         return m_periods[periodNumber]->primaryHists[channelNumber].get();
      }

      TH1* GetPrimaryTriggerDistribution(const TString& trigger, Int_t periodNumber, long triggerBits)  {
         if(m_triggerObjs.find(trigger)==m_triggerObjs.end() || m_triggerObjs[trigger]->triggerHists.find(periodNumber) ==m_triggerObjs[trigger]->triggerHists.end() ||
            m_triggerObjs[trigger]->triggerHists[periodNumber].find(triggerBits) == m_triggerObjs[trigger]->triggerHists[periodNumber].end()) return 0;
      /*
         if(m_triggerObjs.find(trigger)==m_triggerObjs.end() || m_triggerObjs.at(trigger).second->triggerHists.find(periodNumber)==m_triggerObjs.at(trigger).second->->triggerHists.end() ||
            m_triggerObjs.at(trigger).second->triggerHists.at(periodNumber).find(triggerBits) == m_triggerObjs.at(trigger).second.at(periodNumber).end()) return 0;
      */
         
         return m_triggerObjs[trigger]->triggerHists[periodNumber][triggerBits].get();
      }

      /** Method for weighting data to account for prescales and mu bias. Use by giving the tool multiple lumicalc files, one for each trigger */
      Double_t GetDataWeight(Int_t runNumber, const TString& trigger, Double_t x);
      Double_t GetDataWeight(Int_t runNumber, const TString& trigger);//version without mu dependence

    /** Method for prescaling MC to account for prescales in data */
      Double_t GetPrescaleWeight(Int_t runNumber, const TString& trigger, Double_t x);
      Double_t GetPrescaleWeight(Int_t runNumber, const TString& trigger);//version without mu dependence


      // other methods 
      Bool_t IsInitialized() { return m_isInitialized; }
// 
      Int_t AddDistribution(TH1* hist, Int_t runNumber, Int_t channelNumber);

      /** This method is DEFINITELY EXPERT USE ONLY. Used in the checkPRWConfigFile utitlity */
      void ResetCountingMode() { m_countingMode=true; }

      
      void SetParentTool(TPileupReweighting* tool) { m_parentTool = tool; }

      Float_t GetDataScaleFactor() const { return m_dataScaleFactorX; }

      void SetTriggerBit(const TString& trigger, bool in=true) { m_triggerPassBits[trigger]=in; }
      void ResetTriggerBits() { m_triggerPassBits.clear(); }

      double GetRunAverageMu(int run) { return m_runs[run].inputHists["None"]->GetMean(); }

  protected:
      virtual bool runLbnOK(Int_t /*runNbr*/, Int_t /*lbn*/) { return true; } //override in the ASG tool
      virtual bool passTriggerBeforePrescale(const TString& trigger) const { 
        if(m_triggerPassBits.size()==0) return true;
        try {
          return m_triggerPassBits.at(trigger); 
        } catch(...) { return true; }
      } //override in the ASG tool
      std::map<TString, bool> m_triggerPassBits;
      
      
      Int_t GetNearestGoodBin(Int_t thisMCRunNumber, Int_t bin);
      Int_t IsBadBin(Int_t thisMCRunNumber, Int_t bin);


      std::unique_ptr< TH1 > CloneEmptyHistogram(Int_t runNumber, Int_t channelNumber);
      /** Normalize histograms */
      void normalizeHistogram(TH1* histo);
      void AddDistributionTree(TTree *tree, TFile *file);
      //*Int_t FactorizeDistribution(TH1* hist, const TString weightName, Int_t channelNumber, Int_t periodNumber,bool includeInMCRun,bool includeInGlobal);

      void CalculatePrescaledLuminosityHistograms(const TString& trigger);

      //********** Private members*************************
      TPileupReweighting* m_parentTool; //points to self if not a 'systematic varion' tool instance
      Bool_t m_SetWarnings;
      Bool_t m_debugging;
      Bool_t m_countingMode;
      Int_t m_unrepresentedDataAction;                         //used as property in wrapper tool
      Bool_t m_isInitialized;Bool_t m_lumiVectorIsLoaded;
      Float_t m_dataScaleFactorX;Float_t m_dataScaleFactorY;   //first is used as property in wrapper tool
      Float_t m_mcScaleFactorX;Float_t m_mcScaleFactorY;
      Int_t m_nextPeriodNumber;
      Bool_t m_ignoreFilePeriods;
      TTree *m_metadatatree;
      Double_t m_unrepDataTolerance;
      Bool_t m_doGlobalDataWeight; //used in GetDataWeight to flag mu-independent version
      Bool_t m_doPrescaleWeight = false;
      Int_t m_lumicalcRunNumberOffset; //used for 'faking' a lumicalc file for run2

      /** map storing the lumicalc file locations - used when building DataPileupWeights */
      std::map<TString,std::vector<TString> > m_lumicalcFiles;

      //-----------------------------------------------------
      //Shared private data members
      //-----------------------------------------------------



      /** the empty histogram used for this weight... effectively holds the configuration of the binning */
      std::unique_ptr< TH1 > m_emptyHistogram; 



      /** channel metadata map */
      std::map<TString, std::map<Int_t, Double_t> > m_metadata; 

public:
      struct CompositeTrigger {
         int op;
         std::unique_ptr< CompositeTrigger > trig1;
         std::unique_ptr< CompositeTrigger > trig2;
         TString val;
         std::vector<TString> subTriggers; //only set for top-level object
         CompositeTrigger() : op(0),trig1(),trig2(),val("") { }
         double eval(std::map<TString, std::map<Int_t, std::map<Int_t, Float_t> > >& m, int run, int lbn, const TPileupReweighting* tool) {
            switch(op) {
               case 0: if(m[val][run].find(lbn)==m[val][run].end() || !m[val][run][lbn] || !tool->passTriggerBeforePrescale(val)) return 0; /*trigger/failed disabled, so cannot contribute*/   
                        return 1./m[val][run][lbn];
               case 1: return 1. - (1. - trig1->eval(m,run,lbn,tool))*(1.-trig2->eval(m,run,lbn,tool)); //OR
               case 2: return trig1->eval(m,run,lbn,tool)*trig2->eval(m,run,lbn,tool);
               default: return 1;
            }
         }
         void getTriggers(std::vector<TString>& s) {
            if(trig1==0&&trig2==0&&val.Length()>0) s.push_back(val);
            else { trig1->getTriggers(s); trig2->getTriggers(s); }
         }
         
         long getBits(const TPileupReweighting* tool) {
          long out(0);
          for(uint i=0;i<subTriggers.size();i++) out += (tool->passTriggerBeforePrescale(subTriggers[i]) << i);
          return out;
         }
         
         // unnormalized ... i.e. integral should be equal to the lumi!
         // ... indexed by PeriodID,tbits
         std::map<int, std::map<long, std::unique_ptr< TH1 > > > triggerHists;

      };
protected:

      std::map<TString, std::unique_ptr<CompositeTrigger> > m_triggerObjs; //map from trigger string to composite trigger object

      std::unique_ptr<CompositeTrigger> makeTrigger(const TString& s);
      void calculateHistograms(CompositeTrigger* trigger);

public:
      inline void PrintPeriods() { for(auto p : m_periods) {std::cout << p.first << " -> "; p.second->print("");} }
      struct Period {
         Period(Int_t _id, UInt_t _start, UInt_t _end, Int_t _defaultChannel) : id(_id),start(_start),end(_end),defaultChannel(_defaultChannel) {};
         Int_t id;
         UInt_t start;
         UInt_t end;
         Int_t defaultChannel;
         std::map<Int_t, Int_t> inputBinRedirect;
         std::map<Int_t, Double_t> unrepData; //indexed by channel number
         std::vector<Period*> subPeriods; 
         std::vector<UInt_t> runNumbers; //populated with runNumbers that actually had some data available
         std::map<Int_t, std::unique_ptr< TH1 > > inputHists;
         std::map<Int_t, Double_t> sumOfWeights; 
         std::map<Int_t, Int_t> numberOfEntries;
         std::map<Int_t, std::unique_ptr< TH1 > > primaryHists; //normalized histograms, indexed by channelNumber. -1 holds the data
         std::map<Int_t, std::unique_ptr< TH2 > > secondaryHists; //semi-normalized histograms, only used in 2D reweighting
         //std::map<TString, TH1D*> triggerHists; //unnormalized ... i.e. integral should be equal to the lumi! ... should really only be filled in the channel=-1 case (i.e. data)
         bool contains(unsigned int runNumber) {
            if(runNumber >= start && runNumber <= end) return true;
            for(auto p : subPeriods) if(p->contains(runNumber)) return true;
            return false;
         };
         void SetDefaultChannel(Int_t channel) {
            defaultChannel=channel;
            for(auto p : subPeriods) p->SetDefaultChannel(channel);
         };
         void print(const char* prefix) { 
            std::cout << prefix << id << "[" << start << "," << end << "] : ";
            for(auto& hist : inputHists) std::cout << hist.first << " , ";
            std::cout << std::endl;
            for(auto p : subPeriods) p->print(TString::Format(" %s",prefix).Data()); };
      };
      struct Run {
         std::map< TString, std::unique_ptr< TH1 > > inputHists; //key is the 'trigger' 
         std::map<Int_t,Double_t> badBins; 
         Double_t lumi; //total data in run
         std::map<UInt_t, std::pair<Double_t,Double_t> > lumiByLbn; //key=lbn, value = <lumi,mu>
         std::unique_ptr< TH1 > muDist; //mu distribution for this run
      };
protected:
      /// List physically holding (owning) period objects
      std::list< Period > m_periodList;
      std::map<Int_t, Period*> m_periods; //periods mapped by id. -1 = the "global" period (0->9999999). Uses a pointer so can easily implement remap as two entries pointing at same period
      std::map<UInt_t, Run> m_runs; //runs mapped by runNumber

      std::map<Int_t, Double_t> unrepDataByChannel; // -1 holds the total unrepData!

      std::string m_prwFilesPathPrefix;

      //-----------------------------------------------------
      //RandomDataPeriod functionality stuff
      //numbers generated seperately for each mc period
      //-----------------------------------------------------
      std::unique_ptr< TRandom3 > m_random3;

      Bool_t m_ignoreBadChannels; //if true, will print a warning about any channels with too much unrepresented data, but will then just ignore them
      Bool_t m_useMultiPeriods = true; //if true, will allow for runDependentMC 

    //these two numbers are used in the auto-configurations, e.g. "Run2" .. fills additional periods
    int m_autoRunStart = 0;
    int m_autoRunEnd = 0;
      
public:
      //this method is a convenience for copying over properties to the tools that are used for systematic variations
      inline void CopyProperties(const TPileupReweighting* in) {
         m_prwFilesPathPrefix = in->m_prwFilesPathPrefix;
         m_ignoreBadChannels = in->m_ignoreBadChannels;
         m_unrepresentedDataAction = in->m_unrepresentedDataAction;
         m_unrepDataTolerance= in->m_unrepDataTolerance;
      }

      ClassDef(TPileupReweighting,0)


  }; // End: class definition

} // End: namespace Root

#endif
