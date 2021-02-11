/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @class  : Lvl1Converter
 *
 * @brief this tool initializes the HLT from LVL1 objects
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 **********************************************************************************/

#ifndef TRIGSTEERING_LVL1CONVERTER_H
#define TRIGSTEERING_LVL1CONVERTER_H


#include <map>
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigSteering/Lvl1ConsistencyChecker.h"
#include "TrigDecisionMaker/ILvl1ResultAccessTool.h"

// forward declarations
class ITrigTimerSvc;
class TrigTimer;

namespace LVL1 {
  class RecMuonRoiSvc;
  class JEPRoIDecoder;
}

class Lvl1ConsistencyChecker;

namespace HLT {

   class SteeringChain;
   class TriggerElement;

   /**
      @class Lvl1Converter
      initialize HLT (typically L2) from L1 objects:
      1. activate Chains that have a matching L1 item
      2. create TEs for L1 all thresholds

      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */
   class Lvl1Converter : public LvlConverter
   {
   public:

      Lvl1Converter(const std::string& name, const std::string& type,
                    const IInterface* parent); //!< std Gaudi constructor
      ~Lvl1Converter() {} //!< destructor

      ErrorCode hltInitialize(); //!< HLT initialize -> get handle to services, read-in trigger configuration for L1 and HLT; prepare some lookup maps
      ErrorCode hltFinalize();   //!< HLT finalize   -> do nothing here
      ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& chains); //!< HLT execute -> see class description

      virtual const std::map<uint32_t, TriggerElement*> * roIWordToTE() const { return & m_RoI2TE; }
      
   private:

      void insertRoITEinMap(uint32_t roiWord, TriggerElement*);

      //properties:
      bool m_useL1Calo;      //!< consider L1 Calo part ?
      bool m_useL1Muon;      //!< consider L1 Muon part ?
      bool m_useL1JetEnergy; //!< consider L1 JetEnergy part ?
      bool m_updateCaloRoIs; //!< Use RoI converter for old format (default is false!)
      bool m_doTiming;       //!< do timing measurements ?
      bool m_gotL1Config;     //!< retrieve of Lvl1ConfigSvc pointer successful?
      bool m_ignoreL1Prescales; //!< ignores L1 prescales (takes But Before Prescale)
    

      unsigned m_overallRoIsLimit; //!< limit above which the event is considered busy
      unsigned m_jetRoIsLimit;     //!< limit above which the event is considered busy
      unsigned m_emtauRoIsLimit;   //!< limit above which the event is considered busy
      unsigned m_muonRoIsLimit;      //!< limit above which the event is considered busy

      ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool; //!< tool to handle L1 objects
      ToolHandle<Lvl1ConsistencyChecker> m_lvl1ConsistencyTool; //!< tool to check items -> thresolds 
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc; //!< access to trigger menu

      // muon specific svc
      const LVL1::RecMuonRoiSvc* m_recRPCRoiSvc; //!< Muon RoI svc
      const LVL1::RecMuonRoiSvc* m_recTGCRoiSvc; //!< Muon RoI svc

      ITrigTimerSvc* m_timerSvc;    //!< the service with all the timers
      TrigTimer* m_totalTime;       //!< total time of execution of this algo
      TrigTimer* m_sgTime;          //!< StoreGate time
      TrigTimer* m_lvl1ItemsTime;   //!< time for: extract Lvl1 items, activate matching L2 chains
      TrigTimer* m_caloTime;        //!< time for: do TEs for L1 Calo thresholds
      TrigTimer* m_muonTime;        //!< time for: do TEs for L1 Muon thresholds
      TrigTimer* m_jetTime;         //!< time for: do TEs for L1 Jet thresholds

      double fixphi(double);        //!< fix phi range to be within [-pi, +pi]

      std::map<uint32_t, TriggerElement*>  m_RoI2TE;  //!< maps roiWords to TE

   };
} // end namespace


#endif
