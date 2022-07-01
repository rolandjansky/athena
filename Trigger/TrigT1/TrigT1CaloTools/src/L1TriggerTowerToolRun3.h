/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1TriggerTowerToolRun3.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef L1TRIGGERTOWERTOOLRUN3_H
#define L1TRIGGERTOWERTOOLRUN3_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerToolRun3.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrigConfData/L1Menu.h"
#include "StoreGate/ReadCondHandle.h"
#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefsContainer.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h" 


class CaloIdManager;
class CaloLVL1_ID;
class CaloTriggerTowerService;
class Incident;

namespace LVL1
{
  class IL1CaloMappingTool;
  class IL1CaloTTIdTools;
  class IL1DynamicPedestalProvider;

  /** @class L1TriggerTowerToolRun3

      This is a tool to reconstruct the L1 EM/tau trigger sums
      for a particular RoI location from the stored TriggerTowers.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1TriggerTowerToolRun3->process(digits, eta, phi, EmHad,  //inputs
                                        et, bcid, bcidresult)     //outputs
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1TriggerTowerToolRun3 : virtual public IL1TriggerTowerToolRun3, public IIncidentListener, public AthAlgTool
  {
    public:
      
      L1TriggerTowerToolRun3(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1TriggerTowerToolRun3 ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize() override; 
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize() override;
      /** catch begRun */
      virtual void handle(const Incident&) override;

      /** Take in vector of ADC digits, return PPrASIC results */
      virtual void process(const std::vector<int> &digits,
                           double eta, double phi, int layer,
                           std::vector<int> &et, std::vector<int> &bcidResults,
                           std::vector<int> &bcidDecisions, bool useJepLut = true) override;
       
      virtual void process(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId,
                           std::vector<int> &et, std::vector<int> &bcidResults,
                           std::vector<int> &bcidDecisions, bool useJepLut = true) override;

      virtual void simulateChannel(const xAOD::TriggerTower& tt, std::vector<int>& outCpLut,std::vector<int>& outJepLut, std::vector<int>& bcidResults, std::vector<int>& bcidDecisions) const override;
      
      virtual void pedestalCorrection(std::vector<int>& firInOut, int firPed, int iElement, int layer,
                                      int bcid, float mu, std::vector<int_least16_t>& correctionOut) override;
      virtual void fir(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void fir(const std::vector<int> &digits, const std::vector<int> &firCoeffs, std::vector<int> &output) const override;
      virtual void dropBits(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void dropBits(const std::vector<int> &fir, unsigned int first, std::vector<int> &output) const override;
      virtual void etRange(const std::vector<int> &et, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void etRange(const std::vector<int> &et, int energyLow, int energyHigh, std::vector<int> &output) const override;
      virtual void peakBcid(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void peakBcid(const std::vector<int> &fir, unsigned int strategy,  std::vector<int> &output) const override;
      virtual void satBcid(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId,  std::vector<int> &output) const override;
      virtual void satBcid(const std::vector<int> &digits, int satLow, int satHigh, int satLevel, std::vector<int> &output) const override;
      virtual void bcid(const std::vector<int> &fir, const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void bcid(const std::vector<int> &fir, const std::vector<int> &digits, unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &output) const override;
      virtual void bcid(const std::vector<int> &fir, const std::vector<int> &lutInput, const std::vector<int> &digits, int energyLow, int energyHigh, int decisionSource, std::vector<unsigned int> &decisionConditions, unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &result, std::vector<int> &decision) const override;
      virtual void bcidDecisionRange(const std::vector<int>& lutInput, const std::vector<int>& digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, std::vector<int> &output) const override;
      virtual void bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const std::vector<unsigned int> &mask, std::vector<int> &output) const;
      virtual void cpLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void jepLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void lut(const std::vector<int> &fir, int slope, int offset, int cut, int strategy, bool disabled, std::vector<int> &output) const override;
      virtual void nonLinearLut(const std::vector<int> &fir, int slope, int offset, int cut, int scale, short par1, short par2, short par3, short par4, bool disabled, std::vector<int> &output) const;
      virtual void applyEtRange(const std::vector<int>& lut, const std::vector<int>& range, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const override;
      virtual void firParams(const L1CaloCoolChannelId& channelId, std::vector<int> &firCoeffs) const override;
      virtual void bcidParams(const L1CaloCoolChannelId& channelId, int &energyLow, int &energyHigh, int &decisionSource, std::vector<unsigned int> &decisionConditions,
                              unsigned int &peakFinderStrategy, int &satLow, int &satHigh, int &satLevel) const override;
      virtual void cpLutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled) override;
      virtual void jepLutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled) override;
      virtual Identifier identifier(double eta, double phi, int layer) override;
      virtual HWIdentifier hwIdentifier(const Identifier& id) override;
      virtual HWIdentifier hwIdentifier(double eta, double phi, int layer) override;
      virtual L1CaloCoolChannelId channelID(double eta, double phi, int layer) override;
      virtual L1CaloCoolChannelId channelID(const Identifier& id) override;
      virtual bool satOverride(int range) const override;
      virtual bool disabledChannel(const L1CaloCoolChannelId& channelId) const override;
      virtual double FCalTTeta(const L1CaloCoolChannelId& channelId) override;
      virtual double FCalTTeta(double nominalEta, double phi, int layer) override;
      virtual std::pair<double, double> refValues(const L1CaloCoolChannelId& channelId) override; 
      
    private:
      bool isRun2() const;
  
      /** Get extra noise cut with disabled channel */
      bool disabledChannel(const L1CaloCoolChannelId& channelId, unsigned int& noiseCut) const;


      /// Id managers
      const CaloIdManager* m_caloMgr;
      
      /// and tools for computing identifiers
      const CaloLVL1_ID* m_lvl1Helper;
      ToolHandle<LVL1::IL1CaloTTIdTools> m_l1CaloTTIdTools;
      ToolHandle<CaloTriggerTowerService> m_ttSvc;
      
      /// and mappings
      ToolHandle<LVL1::IL1CaloMappingTool> m_mappingTool;

            
      SG::ReadCondHandleKey<L1CaloPprConditionsContainerRun2> m_pprConditionsContainerRun2{ this, "InputKeyPprConditionsRun2", "L1CaloPprConditionsContainerRun2"};
      SG::ReadCondHandleKey<L1CaloPprDisabledChannelContainerRun2> m_pprDisabledChannelContainer{ this, "InputKeyDisabledChannel", "L1CaloPprDisabledChannelContainerRun2"};
      SG::ReadCondHandleKey<L1CaloPpmFineTimeRefsContainer>  m_ppmFineTimeRefsContainer{ this, "InputKeyTimeRefs", "L1CaloPpmFineTimeRefsContainer"}; 
      SG::ReadCondHandleKey<L1CaloRunParametersContainer>  m_runParametersContainer{ this, "InputKeyRunParameters", "L1CaloRunParametersContainer"};

      
      /// Mapping lookup table
      std::vector<unsigned int> m_idTable;

      /// Baseline correction Tool
      bool m_correctFir;
      ToolHandle<LVL1::IL1DynamicPedestalProvider> m_dynamicPedestalProvider;

      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{ this, "EventInfoKey", "EventInfo", "" };
      SG::ReadHandleKey<TrigConf::L1Menu>  m_L1MenuKey{ this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu" };
      
      // xAOD ConfigSvc is useable in both RAWtoALL jobs and from ESD, AOD.
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc{this, "TrigConfigSvc", "TrigConf::xAODConfigSvc/xAODConfigSvc"};  
      const TrigConf::L1Menu* getL1Menu(const EventContext& ctx) const;

      ///Parameters
      static const int s_saturationValue = 255;
      static const int s_maxTowers = 7168;     
      static const int m_nBcid = 350; // mc12: max 350 // all: 3564
  }; 
    
} // end of namespace

#endif 
