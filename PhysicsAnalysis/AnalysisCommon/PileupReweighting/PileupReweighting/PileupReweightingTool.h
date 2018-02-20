/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PILEUPREWEIGHTING_PILEUPREWEIGHTINGTOOL_H
#define PILEUPREWEIGHTING_PILEUPREWEIGHTINGTOOL_H

// System include(s):
#include <memory>

// Framework include(s):
#include "AsgTools/AsgTool.h"

#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

// Local include(s):

#include "PileupReweighting/TPileupReweighting.h"

#include "AsgTools/ToolHandle.h"
#include "PATInterfaces/IWeightTool.h"

#include "TrigDecisionInterface/ITrigDecisionTool.h"

namespace CP {

   /// Implementation of the PileupReweighting tool
   ///
   /// @author Will Buttinger
   ///

   class PileupReweightingTool : 
                             public asg::AsgTool, protected virtual CP::TPileupReweighting,public virtual IPileupReweightingTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( PileupReweightingTool, CP::IPileupReweightingTool )

   public:
      using CP::TPileupReweighting::operator new; //must declare the new operator as public because everything else went protected
      using CP::TPileupReweighting::Merge;
      using CP::TPileupReweighting::operator delete;
      using CP::TPileupReweighting::operator delete[];


      /// Get a random lumiblock number for the given run number 
      virtual UInt_t GetRandomLumiBlockNumber(UInt_t runNumber) { return m_activeTool->GetRandomLumiBlockNumber(runNumber); }
      /// Get the integrated luminosity (in pb-1) between start and end run (inclusive)
      virtual Double_t GetIntegratedLumi(UInt_t start, UInt_t end)  { return m_activeTool->GetIntegratedLumi(start, end); }
      /// Total lumi (in pb-1) for a given trigger combination .. leave blank for the unprescaled
      virtual Double_t GetIntegratedLumi(const TString& trigger)  { return m_activeTool->GetIntegratedLumi(trigger); }
      /** similar to above, but for only the given mcRunNumber/periodNumber */
      virtual Double_t GetIntegratedLumi(Int_t periodNumber, UInt_t start, UInt_t end)  { return m_activeTool->GetIntegratedLumi(periodNumber, start, end); }
      /** return fraction of lumi assigned to periodNumber (or mcRunNumber) that is between start and end data run numbers*/
      virtual Double_t GetIntegratedLumiFraction(Int_t periodNumber, UInt_t start, UInt_t end)  { return m_activeTool->GetIntegratedLumiFraction(periodNumber, start, end); }
      /** return fraction of lumi assigned to periodNumber (or mcRunNumber) with given mu, that is between start and end data run numbers*/
      virtual Double_t GetIntegratedLumiFraction(Int_t periodNumber, Double_t mu, UInt_t start, UInt_t end) { return m_activeTool->GetIntegratedLumiFraction(periodNumber, mu, start, end); }
      /// use these methods when generating config files 
      virtual Int_t AddPeriod(Int_t periodNumber, UInt_t start, UInt_t end)  { return m_activeTool->AddPeriod(periodNumber, start, end); }
      virtual Int_t SetBinning(Int_t nbinsx, Double_t* xbins, Int_t nbinsy=0, Double_t* ybins=0)  { return m_activeTool->SetBinning(nbinsx, xbins, nbinsy, ybins); }
      virtual Int_t SetUniformBinning(Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy=0, Double_t ylow=0, Double_t yup=0)  { return m_activeTool->SetUniformBinning(nbinsx, xlow, xup, nbinsy, ylow, yup); }
      virtual Int_t SetBinning(TH1* hist)  { return m_activeTool->SetBinning(hist); }
      /// helpful alternative to using the EventBookkeepers info ... assuming you made your PRW Config file!!
      virtual Double_t GetSumOfEventWeights(Int_t channel)  { return m_activeTool->GetSumOfEventWeights(channel); }
      virtual Double_t GetNumberOfEvents(Int_t channel)  { return m_activeTool->GetNumberOfEvents(channel) ; }


      /// Constructor for standalone usage
      PileupReweightingTool( const std::string& name );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Finalize - can call the WriteToFile for us
      virtual StatusCode finalize();

      /// Decorates with: 
      /// MC: PileupWeight (CombinedWeight[*UnrepresentedDataWeight if action=2]), RandomRunNumber, RandomLumiBlockNumber 
      /// Data: corrected_averageInteractionsPerCrossing
      /// mu_dependent says if the mu_dependency should be used for random run numbers or the data weights. You will get random run numbers of 0 for events with zero pileup weight
      virtual StatusCode apply ( const xAOD::EventInfo& eventInfo, bool mu_dependent );

      /// Return combined pileup weight
      virtual float getCombinedWeight( const xAOD::EventInfo& eventInfo );

      /// Same as above, but for a 'custom weight' variable
      virtual float getCombinedWeight( const xAOD::EventInfo& eventInfo,Double_t x, Double_t y );

      virtual float getCombinedWeight( const xAOD::EventInfo& eventInfo , const TString& trigger, bool mu_dependent );

      /// return the prw hash used for fast updates of weights at the post-processing level ... see the share/makeWeightTree.C script for usage
      virtual ULong64_t getPRWHash( const xAOD::EventInfo& eventInfo );

      /// Get the mu of a lumiblock ... needed to 'correct' the number in datasets
      virtual float getCorrectedAverageInteractionsPerCrossing( const xAOD::EventInfo& eventInfo, bool includeDataScaleFactor );
      /// Get the corrected 'actual' interactions per crossing
      virtual float getCorrectedActualInteractionsPerCrossing( const xAOD::EventInfo& eventInfo, bool includeDataScaleFactor );
      
      /// Get the integrated lumi of a lumiblock (in pb-1)
      virtual double getLumiBlockIntegratedLumi(const xAOD::EventInfo& eventInfo);

      /// When using UnrepresentedDataAction=2, you may want to apply this additional weight to ensure sum of weights are preserved
      virtual float getUnrepresentedDataWeight( const xAOD::EventInfo& eventInfo );
   
      /// Get the dataWeight used to 'unprescale' data collected from a given trigger combination. mu_dependency is recommended to be true
      virtual float getDataWeight( const xAOD::EventInfo& eventInfo, const TString& trigger, bool mu_dependent );
   
      /// Get a random run number for this MC event, using mu-dependent randomization by default ... jetetmiss seem to like it muchly
      virtual int getRandomRunNumber( const xAOD::EventInfo& eventInfo , bool mu_dependent );
   
      /// Call this method once per event when in config file generating mode and you want standard mu reweighting
      virtual int fill( const xAOD::EventInfo& eventInfo );
   
      /// Use this method if you want to do a generic reweighting instead
      virtual int fill( const xAOD::EventInfo& eventInfo, Double_t x, Double_t y);

      /// Get pointer to the underlying tool - expert use only. Will require #include "PileupReweighting/TPileupReweighting.h"
      virtual CP::TPileupReweighting* expert() { return m_activeTool; }



      /// The ISystematicsTool methods 
      bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;
      CP::SystematicSet affectingSystematics() const;
      CP::SystematicSet recommendedSystematics() const;
      CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig );

#ifndef XAOD_STANDALONE
      void updateHandler(Property& /*p*/);
#endif

   protected:
      virtual bool runLbnOK(Int_t runNbr, Int_t lbn);
      virtual bool passTriggerBeforePrescale(const TString& trigger) const;

   private:
      std::string m_configStream;
      bool m_inConfigMode;

      // Systematic variation instances for the reweighting
      std::unique_ptr< CP::TPileupReweighting > m_upTool;
      std::unique_ptr< CP::TPileupReweighting > m_downTool;

      double m_upVariation; double m_downVariation;
      CP::SystematicVariation m_systUp, m_systDown;
      CP::TPileupReweighting* m_activeTool; //defaults to this

      std::vector<std::string> m_prwFiles;
      std::vector<std::string> m_lumicalcFiles;

      std::string m_prefix;

      int m_defaultChannel;
      std::string m_usePeriodConfig;
      std::map<int, bool> m_doneConfigs;

      bool m_noWeightsMode;
      
      bool m_varyRunNumber;
      bool m_useRunDependentPrescaleWeight;

      ///< Default weight tool in standalone mode
      std::unique_ptr< IWeightTool > m_defaultWeightTool;

      ToolHandle<IWeightTool> m_weightTool;
      ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
      ToolHandle<Trig::ITrigDecisionTool> m_tdt;

      // MN:  this prevents ROOT dict generator from complaining about lack of ClassDef()
      // Note: inheriting from TObject and not having ClassDef makes this class unsuitable for I/O
   public:
      static Version_t Class_Version() { return 0; }

   }; // class PileupReweightingTool

} // namespace CP

#endif // 
