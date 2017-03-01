#ifndef ENHANCEDBIASWEIGHTER_ENHANCEDBIASWEIGHTER_H
#define ENHANCEDBIASWEIGHTER_ENHANCEDBIASWEIGHTER_H 1

#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "EnhancedBiasWeighter/IEnhancedBiasWeighter.h"
#include "EnhancedBiasWeighter/ReadLumiBlock.h"

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

#include <unordered_map>

struct ChainDetail {
  std::string m_name;
  std::string m_lowerName;
  std::string m_comment;
  uint32_t m_counter;
  double m_prescale;
  double m_eventsPassed;
  double m_eventsPassedWeighted;
  double m_rate;
  double m_rateErr;
  double m_passthroughPrescale;
  double m_rerunPrescale;
  double m_expressPrescale;
  double m_efficiency;
  double m_efficiencyErr;
  double m_prescaledEfficiency;
  double m_prescaledEfficiencyErr;
};

class EnhancedBiasWeighter: public asg::AsgTool, public virtual IEnhancedBiasWeighter, public virtual DerivationFramework::IAugmentationTool { 
 public: 
   //constructor for athena can be created using special macro
   //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
   ASG_TOOL_CLASS2( EnhancedBiasWeighter , IEnhancedBiasWeighter, DerivationFramework::IAugmentationTool)
   //add another constructor for non-athena use cases
   EnhancedBiasWeighter( const std::string& name );

   constexpr static double LHC_FREQUENCY = 11245.5; //<! LHC revolution frequency in Hz
   constexpr static uint32_t FULL_RING = 2738; //!< Number of bunches in a full ring

   /// Initialize is required by AsgTool base class
   virtual StatusCode  initialize() override;
   virtual StatusCode  finalize() override;
   // addBranches is required by DerivationFramework::AugmentationTool
   virtual StatusCode  addBranches() const override;

   virtual double   getEBWeight(const xAOD::EventInfo* eventInfo) const override;
   virtual double   getEBLiveTime(const xAOD::EventInfo* eventInfo) const override;
   virtual double   getLBLumi(const xAOD::EventInfo* eventInfo) const override;
   virtual double   getAverageLumi() const override;
   virtual double   getAverageMu() const override;
   virtual double   getDeadtime() const override;
   virtual uint32_t getPairedBunches() const override;
   virtual uint32_t getDistanceIntoTrain(const xAOD::EventInfo* eventInfo) const override;
   virtual bool     isUnbiasedEvent(const xAOD::EventInfo* eventInfo) const override; 
   virtual bool     isGoodLB(const xAOD::EventInfo* eventInfo) const override; 

   virtual std::unordered_map<std::string, ChainDetail> parsePrescaleXML(const std::string& prescaleXML) const override;

 private: 
    StatusCode loadWeights();
    StatusCode loadLumi();
    StatusCode trackAverages(const xAOD::EventInfo* eventInfo) const;

    int32_t getEventEBID(const xAOD::EventInfo* eventInfo) const; 

    ToolHandle<Trig::IBunchCrossingTool> m_bcTool;

    uint32_t m_runNumber; //!< Run we're processing - needed at initialize to locate and read in extra configuration.  
    double m_deadtime; //!< Online deadtime to correct for in rate prediction. Currently a constant for the EB period
    uint32_t m_pairedBunches; //!< Online number of paired bunches.
    bool m_calculateWeightingData; //!< True if data is to be loaded from COOL and CVMFS. False if it is to be fetched from a TRIG1 decorated dAOD
    bool m_enforceEBGRL; //!< Always return weight 0 if the event is vetoed due to enhanced bias 'good run list'
    bool m_useBunchCrossingTool; //!< Allow disabling of bunch crossing tool for situations when CONDBR2 is not accessible

    bool m_isMC; //!< True if supplying weights for MC
    double m_mcCrossSection; //!< If MC, the cross section in nb
    double m_mcFilterEfficiency; //!< If MC, the filter efficiency
    double m_mcKFactor; //!< If MC, any k factor
    double m_mcModifiedCrossSection; //!< Product of xsec, filter & kfactor. In units of cm
    double m_inelasticCrossSection; //!< Sigma_inel in units of cm^2. Default of 80 mb @ 13 TeV = 8e-26 cm^2

    std::unordered_map<uint64_t, int32_t> m_eventNumberToIdMap; //!< Map event number to a weighting ID

    mutable std::unordered_map<uint32_t, float> m_eventLivetime; //!< Cache of per-event livetime as a function of LB [LB -> effective walltime per event]
    mutable double                              m_lumiAverageNum; //!< The average instantaneous lumionosity over all events. Numerator
    mutable double                              m_muAverageNum; //!< The average mu over all events. Numerator
    mutable uint32_t                            m_averageDenom; //!< The average over all events. Denominator

    std::unordered_map<int32_t, double>     m_idToWeightMap; //!< Map a weighting ID to a Enhanced Bias event weight
    std::unordered_map<int32_t, uint8_t>    m_idToUnbiasedMap; //!< Map a weighting ID to a flag if this weight is from an unbiased (RD) trigger online
    std::unordered_map<uint32_t, uint32_t>  m_eventsPerLB; //!< Map of how many EnhancedBias events were recorded per LB 
    std::unordered_map<uint32_t, double>    m_lumiPerLB; //!< Map of instantaneous luminosity per LB 
    std::unordered_map<uint32_t, uint8_t>   m_goodLB;  //!< Like a Good Run List flag for EnhancedBias runs.

    ReadLumiBlock m_readLumiBlock; //!< Cache lumi block lengths. Get this from COOL.
}; 

#endif //> !ENHANCEDBIASWEIGHTER_ENHANCEDBIASWEIGHTER_H
