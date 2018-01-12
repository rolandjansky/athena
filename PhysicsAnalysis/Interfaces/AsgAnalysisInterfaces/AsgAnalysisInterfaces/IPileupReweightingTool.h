/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///ASG PileupReweightingTool interface
///author: will@cern.ch
#ifndef PILEUPREWEIGHTING_IPILEUPREWEIGHTINGTOOL_H
#define PILEUPREWEIGHTING_IPILEUPREWEIGHTINGTOOL_H

#include "AsgTools/IAsgTool.h"
#include "TString.h"
#include "xAODEventInfo/EventInfo.h"

#include "PATInterfaces/ISystematicsTool.h"
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

///Main tools implementing this interface:
///  CP::PileupReweightingTool   (source code: PhysicsAnalysis/AnalysisCommon/PileupReweighting)



class TH1;

namespace CP {
class TPileupReweighting;

   class IPileupReweightingTool : virtual public CP::ISystematicsTool
{

      ASG_TOOL_INTERFACE( CP::IPileupReweightingTool )

      public:


         /// Return combined pileup weight
         virtual float getCombinedWeight( const xAOD::EventInfo& eventInfo ) = 0;

         /// Same as above, but for a 'custom weight' variable
         virtual float getCombinedWeight( const xAOD::EventInfo& eventInfo,Double_t x, Double_t y=0. ) = 0;

         /// get combined weight (i.e. pileup weight) but with a 1./dataWeight factor applied, this if used for 'prescaling' MC
         virtual float getCombinedWeight( const xAOD::EventInfo& eventInfo , const TString& trigger, bool mu_dependent=true ) = 0;

         /// When using UnrepresentedDataAction=2, you may want to apply this additional weight to ensure sum of weights are preserved
         virtual float getUnrepresentedDataWeight( const xAOD::EventInfo& eventInfo ) = 0;

         /// Get the dataWeight used to 'unprescale' data collected from a given trigger combination. mu_dependency is recommended to be true
         virtual float getDataWeight( const xAOD::EventInfo& eventInfo, const TString& trigger, bool mu_dependent=true ) = 0;

         /// Get a random run number for this MC event, mu_dependency is recommended ... jetetmiss seem to like it muchly
         virtual int getRandomRunNumber( const xAOD::EventInfo& eventInfo , bool mu_dependent=true) = 0;

         /// Get the corrected average mu of a lumiblock ... (averaging is over all filled BCID in the fill)
         /// The additional flag indicates if the returned value should include the data scale factor or not.
         /// Note: if the lumiblock is not present in the input lumicalc file, then -1.0 will be returned
         /// This method will soon be deprecated in favour of the new naming: getCorrectedAverageInteractionsPerCrossing
         virtual float getCorrectedMu( const xAOD::EventInfo& eventInfo, bool includedDataScaleFactor=false ) {
            return getCorrectedAverageInteractionsPerCrossing( eventInfo, includedDataScaleFactor );
         }
         virtual float getCorrectedAverageInteractionsPerCrossing( const xAOD::EventInfo& eventInfo, bool includedDataScaleFactor=false ) = 0;
         
         /// Get the actual mu of a lumiblock ... (BCID-specific mu)
         /// The additional flag indicates if the returned value should include the data scale factor or not.
         /// Note: if the lumiblock is not present in the input lumicalc file, then -1.0 will be returned
         virtual float getCorrectedActualInteractionsPerCrossing( const xAOD::EventInfo& eventInfo, bool includedDataScaleFactor=false ) = 0;
         
         /// Get the integrated lumi of a lumiblock (in pb-1)
         virtual double getLumiBlockIntegratedLumi( const xAOD::EventInfo& eventInfo ) = 0;

         /// Decorates with: 
         /// MC: PileupWeight (CombinedWeight[*UnrepresentedDataWeight if action=2]), RandomRunNumber, RandomLumiBlockNumber, PRWHash
         /// Data: corrected_averageInteractionsPerCrossing
         /// mu_dependent says if the mu_dependency should be used for random run numbers or the data weights. You will get random run numbers of 0 for events with zero pileup weight
         virtual StatusCode apply ( const xAOD::EventInfo& eventInfo, bool mu_dependent=true ) = 0;


         /// return the prw hash used for fast updates of weights at the post-processing level ... see the share/makeWeightTree.C script for usage
         virtual ULong64_t getPRWHash( const xAOD::EventInfo& eventInfo ) = 0;

         // methods that go straight to the underlying pileup tool 

         /// Get a random lumiblock number for the given run number 
         virtual UInt_t GetRandomLumiBlockNumber(UInt_t runNumber) = 0;

         /// possible alternative to using the EventBookkeepers info ... assuming you made your PRW Config file!!
         virtual Double_t GetSumOfEventWeights(Int_t channel) = 0;
         virtual Double_t GetNumberOfEvents(Int_t channel) = 0;

         /// Get the integrated luminosity (in pb-1) between start and end run (inclusive)
         virtual Double_t GetIntegratedLumi(UInt_t start, UInt_t end) = 0;
         /// Total lumi (in pb-1) for a given trigger combination .. leave blank for the unprescaled
         virtual Double_t GetIntegratedLumi(const TString& trigger="") = 0;
         /** similar to above, but for only the given mcRunNumber/periodNumber */
         virtual Double_t GetIntegratedLumi(Int_t periodNumber, UInt_t start, UInt_t end) = 0;
         /** return fraction of lumi assigned to periodNumber (or mcRunNumber) that is between start and end data run numbers*/
         virtual Double_t GetIntegratedLumiFraction(Int_t periodNumber, UInt_t start, UInt_t end) = 0;
         /** return fraction of lumi assigned to periodNumber (or mcRunNumber) with given mu, that is between start and end data run numbers*/
         virtual Double_t GetIntegratedLumiFraction(Int_t periodNumber, Double_t mu, UInt_t start, UInt_t end) = 0;


         /// use these methods when generating config files 
         virtual Int_t AddPeriod(Int_t periodNumber, UInt_t start, UInt_t end) = 0;
         virtual Int_t SetBinning(Int_t nbinsx, Double_t* xbins, Int_t nbinsy=0, Double_t* ybins=0) = 0;
         virtual Int_t SetUniformBinning(Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy=0, Double_t ylow=0, Double_t yup=0) = 0;
         virtual Int_t SetBinning(TH1* hist) = 0;

         /// Call this method once per event when in config file generating mode and you want standard mu reweighting
         virtual int fill( const xAOD::EventInfo& eventInfo ) = 0;

         /// Use this method if you want to do a generic reweighting instead
         virtual int fill( const xAOD::EventInfo& eventInfo, Double_t x, Double_t y=0.) = 0;


         /// Get pointer to the underlying tool - expert use only:  Will require #include "PileupReweighting/TPileupReweighting.h"
         virtual CP::TPileupReweighting* expert() = 0;


   };

}



#endif
