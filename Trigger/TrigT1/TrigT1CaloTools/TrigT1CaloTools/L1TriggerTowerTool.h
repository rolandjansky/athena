/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1TriggerTowerTool.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef L1TRIGGERTOWERTOOL_H
#define L1TRIGGERTOWERTOOL_H

#include <boost/any.hpp>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class CaloIdManager;
class CaloLVL1_ID;
class CaloTriggerTowerService;
class Incident;
class L1CaloCondSvc;
class L1CaloPpmFineTimeRefsContainer;
class L1CaloDerivedRunParsContainer;
class L1CaloRunParametersContainer;
class L1CaloPprChanStrategyContainer;

namespace TrigConf { class ILVL1ConfigSvc; }

namespace LVL1
{
  class IL1CaloMappingTool;
  class IL1CaloTTIdTools;
  class IL1DynamicPedestalProvider;

  /** @class L1TriggerTowerTool

      This is a tool to reconstruct the L1 EM/tau trigger sums
      for a particular RoI location from the stored TriggerTowers.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1TriggerTowerTool->process(digits, eta, phi, EmHad,  //inputs
                                        et, bcid, bcidresult)     //outputs
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1TriggerTowerTool : virtual public IL1TriggerTowerTool, public IIncidentListener, public AthAlgTool
  {
    public:
      
      L1TriggerTowerTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1TriggerTowerTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize();
      /** catch begRun */
      virtual void handle(const Incident&);
      /** Retrieve pointers to the L1Calo conditions containers */
      virtual StatusCode retrieveConditions();

      /** Take in vector of ADC digits, return PPrASIC results */
      virtual void process(const std::vector<int> &digits,
                           double eta, double phi, int layer,
                           std::vector<int> &et, std::vector<int> &bcidResults,
                           std::vector<int> &bcidDecisions, bool useJepLut = true);
       
      virtual void process(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId,
                           std::vector<int> &et, std::vector<int> &bcidResults,
                           std::vector<int> &bcidDecisions, bool useJepLut = true);

      virtual void simulateChannel(const xAOD::TriggerTower& tt, std::vector<int>& outCpLut, std::vector<int>& outJepLut, std::vector<int>& bcidResults, std::vector<int>& bcidDecisions);

      virtual void pedestalCorrection(std::vector<int>& firInOut, int firPed, int iElement, int layer,
                                      int bcid, float mu, std::vector<int_least16_t>& correctionOut);
      virtual void fir(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void fir(const std::vector<int> &digits, const std::vector<int> &firCoeffs, std::vector<int> &output);
      virtual void dropBits(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void dropBits(const std::vector<int> &fir, unsigned int first, std::vector<int> &output);
      virtual void etRange(const std::vector<int> &et, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void etRange(const std::vector<int> &et, int energyLow, int energyHigh, std::vector<int> &output);
      virtual void peakBcid(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void peakBcid(const std::vector<int> &fir, unsigned int strategy, std::vector<int> &output);
      virtual void satBcid(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void satBcid(const std::vector<int> &digits, int satLow, int satHigh, int satLevel, std::vector<int> &output);
      virtual void bcid(const std::vector<int> &fir, const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void bcid(const std::vector<int> &fir, const std::vector<int> &digits, unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &output);
      virtual void bcid(const std::vector<int> &fir, const std::vector<int> &lutInput, const std::vector<int> &digits, int energyLow, int energyHigh, int decisionSource, std::vector<unsigned int> &decisionConditions,
                        unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &result, std::vector<int> &decision);
      virtual void bcidDecisionRange(const std::vector<int>& lutInput, const std::vector<int>& digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const std::vector<unsigned int> &mask, std::vector<int> &output);
      virtual void lut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void cpLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void jepLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void lut(const std::vector<int> &fir, int slope, int offset, int cut, int ped, int strategy, bool disabled, std::vector<int> &output);
      virtual void nonLinearLut(const std::vector<int> &fir, int slope, int offset, int cut, int scale, short par1, short par2, short par3, short par4, bool disabled, std::vector<int> &output);
      virtual void applyEtRange(const std::vector<int>& lut, const std::vector<int>& range, const L1CaloCoolChannelId& channelId, std::vector<int> &output);
      virtual void firParams(const L1CaloCoolChannelId& channelId, std::vector<int> &firCoeffs);
      virtual void bcidParams(const L1CaloCoolChannelId& channelId, int &energyLow, int &energyHigh, int &decisionSource, std::vector<unsigned int> &decisionConditions,
                              unsigned int &peakFinderStrategy, int &satLow, int &satHigh, int &satLevel);
      virtual void lutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled);
      virtual void cpLutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled);
      virtual void jepLutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled);
      virtual Identifier identifier(double eta, double phi, int layer);
      virtual HWIdentifier hwIdentifier(const Identifier& id);
      virtual HWIdentifier hwIdentifier(double eta, double phi, int layer);
      virtual L1CaloCoolChannelId channelID(double eta, double phi, int layer);
      virtual L1CaloCoolChannelId channelID(const Identifier& id);
      virtual bool satOverride(int range, const L1CaloCoolChannelId& channelId);
      virtual bool disabledChannel(const L1CaloCoolChannelId& channelId);
      virtual double FCalTTeta(const L1CaloCoolChannelId& channelId);
      virtual double FCalTTeta(double nominalEta, double phi, int layer);
      virtual void setDebug(bool debug);
      virtual std::pair<double, double> refValues(const L1CaloCoolChannelId& channelId); 
      virtual StatusCode loadFTRefs();
    
    private:
  
      /** Print a vector to debug */
      template <typename T>
      void printVec(const std::vector<T>& vec);

      /** Get extra noise cut with disabled channel */
      bool disabledChannel(const L1CaloCoolChannelId& channelId, unsigned int& noiseCut);

      bool              m_debug;

      /// Id managers
      const CaloIdManager* m_caloMgr;
      
      /// and tools for computing identifiers
      const CaloLVL1_ID* m_lvl1Helper;
//       L1CaloTTIdTools* m_l1CaloTTIdTools;
      ToolHandle<LVL1::IL1CaloTTIdTools> m_l1CaloTTIdTools;
      ToolHandle<CaloTriggerTowerService> m_ttSvc;
      
      /// and mappings
      ToolHandle<LVL1::IL1CaloMappingTool> m_mappingTool;

      /// L1Calo conditions
      ServiceHandle<L1CaloCondSvc> m_l1CondSvc;

      /// Trig configurations
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

      // one of L1CaloPprConditionsContainer{,Run2}*
      bool m_isRun2;
      
      /// For Run2 strategy (LowMu, HighMu)
      L1CaloPprChanStrategyContainer* m_strategyContainer;
      L1CaloDerivedRunParsContainer* m_derivedRunParsContainer;
      L1CaloRunParametersContainer* m_runParametersContainer;
      
      boost::any m_conditionsContainer;
      // one of L1CaloPprDisabledChannelContainer{,Run2}*
      boost::any m_disabledChannelContainer;
      
      /// For the fine time monitoring
      L1CaloPpmFineTimeRefsContainer* m_dbFineTimeRefsTowers; 
      

      /// Mapping lookup table
      std::vector<unsigned int> m_idTable;

      /// Baseline correction Tool
      bool m_correctFir;
      ToolHandle<LVL1::IL1DynamicPedestalProvider> m_dynamicPedestalProvider;

      ///Parameters
      static const int s_saturationValue = 255;
      static const int s_maxTowers = 7168;     
      static const int m_nBcid = 350; // mc12: max 350 // all: 3564
  }; 
    
} // end of namespace

#endif 
