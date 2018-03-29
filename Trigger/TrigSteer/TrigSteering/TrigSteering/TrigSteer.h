/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : TrigSteer
 *
 * @brief  TopAlgorithm for HLT Steering
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: TrigSteer.h,v 1.56 2009-05-05 20:29:23 tbold Exp $

 **********************************************************************************/

#ifndef TRIGSTEERING_TRIGSTEER_H
#define TRIGSTEERING_TRIGSTEER_H
#include <string>
#include <map>
#include <vector>

#include "TrigConfBase/MsgStream.h"

#include "TrigSteering/ISequenceProvider.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigTimeAlgs/CookTimer.h"

#include "TrigConfHLTData/HLTSequence.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "AthenaMonitoring/IMonitorToolBase.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "EventInfo/TriggerInfo.h"
#include "AthenaKernel/Timeout.h"

class StatusCode;
class ITrigTimerSvc;
class TrigTimer;
class IIncidentSvc;
class IROBDataProviderSvc;
class ICoreDumpSvc;

namespace TCS{
  class TopoSteering;
  class TopoInputEvent;
}

namespace TrigConf {
   class ITrigConfigSvc;
   class IL1TopoConfigSvc;
}

namespace HLT {

   class Algo;
   class AlgoConfig;
   class Sequence;
   class Chain;
   class SteeringChain;
   class TriggerElementFactory;
   class Navigation;
   class ResultBuilder;
   class IScalerSvc;
   class ILvlConverter;
   class ILvlTopoConverter;
   class IReusableScaler;
   class IExecutionOrderStrategy;
   class LvlTopoConverter;
   class IEventInfoAccessTool;

   /**
      @class TrigSteer
      This class is the HLT Steering TopAlgorithm. It contains a list of HLT::Chain objects which
      are executed until none is active anymore.

      In more detail, this class is responsible for: Retrieving the trigger configuration
      with the TrigConf::TrigConfigSvc; Setting up the Navigation for the HLT::TriggerElement
      objects and other features created by the PESA algorithms; Initializing all HLT::Chain
      objects which in turn initialize their HLT:Signatures and so on; Loading of all PESA
      algorithms.

      During execution, first the trigger level converter is called; then the Chains are
      executed until they've all finished; then the HLTResult is build with the help of the
      ResultBuilder tool; finally all monitoring tools are called.


      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
      @author Tomasz Bold     <Tomasz.Bold@cern.ch>
   */
   class TrigSteer : public AthAlgorithm,
                     public Athena::TimeoutMaster,
                     public virtual ISequenceProvider,
                     public virtual IIncidentListener
   {

   public:

      TrigSteer(const std::string& name, ISvcLocator* pSvcLocator); //!< Gaudi constructor
      ~TrigSteer();     //!< destructor

      /**
         Set all variables to default, retrieve TrigConfiguration objects:
         - list of chains containing signatures for each step
         - list of all sequences each with input TE type(s), output TE type, algorithm label

         Create all sequences and algorithms in memory:
         - global list of all sequences:   m_sequences
         - global list of all algorithms:  m_algos

         so that each sequence & each algorithm exists only once in memory!

         Setup all HLT::Chain objects, each HLT::Chain object manages its HLT::Signature objects.
         Each HLT::Signature object has pointer(s) to the HLT::Sequence objects.
         The HLT::Sequence objects have pointers to the algorithm objects.
      */
      StatusCode initialize(); //!< Gaudi initialize
      StatusCode start();    //!< Gaudi start
      StatusCode endRun();   //!< Gaudi endRun
      StatusCode stop();     //!< Gaudi stop
      StatusCode finalize(); //!< Gaudi finalize

      /**
         - reset all chains (which in turn reset their own objects ...)
         - call Lvl conversion tool, which activates matching chains and creates TEs
         - loop over steps: call active chains
         - create result
      */
      StatusCode execute(); //!< Gaudi execute

      /** Handle incidents (for event timeouts) */
      void handle (const Incident& inc);    //!< IIncidentListener::handle
    
      /** Method to find the sequence from the global sequence list, which produces
          the given TE type. This is needed by the HLT::Signature class */
      HLT::Sequence* findSeqForOutputTeType(const unsigned int teType); //!< find sequence producing given TE

      HLT::Sequence* createSequence(const TrigConf::HLTSequence& sequence, const unsigned int TeType); //Creates a sequence

      HLT::Sequence* createTopoSequence(const unsigned int TeType);

      const std::vector<const HLT::SteeringChain*> getActiveChains() const;     //<! get all active chains (used in monitoring)
      const std::vector<const HLT::SteeringChain*> getConfiguredChains() const; //<! get all configured chains (used in monitoring)
      std::vector<TriggerInfo::StreamTag> getErrorStreamTags() const;           //<! get all configured debug streams (name,type)

      int stepForEB() const { return m_stepForEB;}; //!< get the step counting at EB for the merged system

      const HLT::AlgoConfig* getAlgoConfig() const { return m_config; } //!< get the hlt runtime config

   private:
      // -----------------
      // methods :
      bool resetChains(std::vector<HLT::SteeringChain*>& chains); //!< reset the given vector of Chains

      HLT::Algo* getAlgo(std::string name);               //!< create/retrieve algorithm corresponding to given name

      bool canContinueEvent(HLT::ErrorCode ec); //!< decides if event processing should be aborted or continued
      bool canContinueJob();                    //!< decides if whole processing should be aborted or continued

      void runChains(bool prescaleStatus);//!< execute next step until no chain is active.

      void sortChains(std::vector<HLT::SteeringChain*>& chains);

      void configureCoherentPrescaling();

      HLT::ErrorCode setEvent();  //!< sets basic paramters for the vent (like LBN and L1ID)

      void issueRobRequests(); //!< sends list of ROB IDs to the ROBDataProvider

      void issueEventBuildingRequest(int step); //!< sends command for the EB

      bool resetChainsROBRequestPreparation(std::vector<HLT::SteeringChain*>& chains);  //!< resets everything related with ROB prefetching

      void  doPrefetching(bool &secondPass, bool& noError); // execute ROS data preparation

      // -----------------
      // private variables
      std::vector<HLT::SteeringChain*> m_chains;          //!< all (configured) HLT chains of this level (L2 or EF or HLT)
      std::vector<HLT::SteeringChain*> m_activeChains;    //!< all HLT chains of this level (L2 or EF or HLT) that were activated by the LvlConverter
      HLT::AlgoConfig* m_config;                  //!< common variables (hlt runtime config)
      std::vector<HLT::Sequence*> m_sequences;    //!< Global list of all configured sequences (used for bookkeeping)
      std::map<std::string, HLT::Algo*> m_algos;  //!< Global list of all configured algorithms (used for bookkeeping)

      std::vector<unsigned int> m_producedTEsL2;    //!< map of TEid which should be produced at this trigger level
      std::vector<unsigned int> m_producedTEsEF;    //!< map of TEid which should be produced at this trigger level
      std::vector<unsigned int> m_producedTEsHLT;    //!< map of TEid which should be produced at this trigger level
      std::string producedFirstAtLevel(unsigned int id); //!< helper to tell at which trigger level given ID will first be created 

      // -----------------
      // properties:
      bool m_doTiming;                //!< flag for timing monitoring
      bool m_doHypo;                  //!< flag for HYPO algorithms
      bool m_doFex;                   //!< flag for HYPO algorithms
      unsigned int m_cachingMode;     //!< flag to set caching mode

      TCS::TopoSteering * m_topoSteer { nullptr };

      std::string m_lvlConverterName; //!< LvlConverter name, default = "Lvl1Converter"
      std::string m_lvlTopoConverterName;   //!< Topological trigger conversion
      std::string m_hltLevel;         //!< JO for trigger LVL [L2/EF/HLT]

      float m_softEventTimeout;//!< Soft event timeout in seconds (0=disabled by default)
      float m_hardEventTimeout;//!< Hard event timeout in seconds (0=disabled by default)
      unsigned int m_doOperationalInfo;  //!< level of operational information collected by steering
      int  m_sortChains;                 //!< directive to sort chains before executing event
      bool m_enableCoherentPrescaling;   //!< directive to enable coherent prescaling
      bool m_enableRobRequestPreparation; //!< directive to enable ROB request preparation step
      bool m_doL1TopoSimulation; //!< Turns on L1Topo Simulation
      int  m_topoOutputLevel{TrigConf::MSGTC::WARNING};  // property to set the outputlevel of the topo algorithms
      bool m_topoUseBitwise;              //!< property to set 'UseBitwise' of TopoSteering
      bool m_enableRerun;                 //!< directive to enable rerun on prescaled chains
      int  m_stepForEB;                   //!< step of the EB in the merged L2EF system
      int  m_strategyEB;                  //!< directive to decide the EB strategy in the merged L2EF system
      bool m_auditChains;                 //!< call auditor hooks for chain execution
      // -----------------
      // Services & tools
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;            //!< TrigConfiguration Service
      ServiceHandle<TrigConf::IL1TopoConfigSvc> m_l1topoConfigSvc;    //!< TrigConfiguration Service of L1Topo
      ServiceHandle<IROBDataProviderSvc>      m_robDataProvider;      //!< ROB data provider (for ROB pre-fetching)
      SmartIF <ITrigROBDataProviderSvc>       m_trigROBDataProvider;  //!< Trig ROB data provider (for Event Building)


      ToolHandle<Navigation> m_navigation;                //!< HLT Navigation, taking care of all TriggerElements and the links etc.
      ServiceHandle<IScalerSvc> m_scalerSvc;              //!< Scaler manager, to produce scaler objects to use in chain prescale/passthrough.
      ToolHandle<IReusableScaler> m_opiScaler;            //!< scaler to collect OPI info 
      ToolHandle< ILvlConverter > m_lvlCnvTool;           //!< tool for conversion of trigger level
      ToolHandle< ILvlTopoConverter > m_lvlTopoCnvTool;       //!< tool for conversion of topo
      ToolHandle< ResultBuilder > m_resultBuilder;        //!< tool that creates the trigger lvl result object
      ToolHandleArray< IMonitorToolBase > m_monTools;     //!< Monitoring tools
      ToolHandleArray< IMonitorToolBase > m_opiTools;     //!< OPI Monitoring tools (they are run before result is built and therfore can add some info to it)
      ServiceHandle<IIncidentSvc> m_incSvc;               //!< IncidentSvc
      ServiceHandle<ICoreDumpSvc> m_coreDumpSvc;          //!< CoreDumpSvc
      ToolHandle<IExecutionOrderStrategy> m_executionOrderStrategy; //!< Tool altering order of chains execution
      ToolHandle<IEventInfoAccessTool> m_EventInfoTool;   //!< Tool  to modify EventInfo after execution

      // ----------------
      // Timings measurements instrumentation
      ServiceHandle<ITrigTimerSvc> m_timerSvc; //!< service for all timing measurements
      TrigTimer* m_timerTotal{0};                 //!< total time of execution
      TrigTimer* m_timerTotalAccepted{0};         //!< total time of events which were acceptde
      TrigTimer* m_timerTotalRejected{0};         //!< total time of events which were rejected
      TrigTimer* m_timerLvlConverter{0};          //!< time of trigger level converter tool
      TrigTimer* m_timerChains{0};                //!< time of chain execution
      TrigTimer* m_timerTotalRerun{0};            //!<time spent on re-running chains
      TrigTimer* m_timerResultBuilder{0};         //!< time of ResultBuilder tool execution
      TrigTimer* m_timerMonitoring{0};            //!< total time of monitoring tool(s) execution
      TrigTimer* m_timerMonitoringSave{0};        //!< total time of monitoring tool(s) execution
      TrigTimer* m_timerExec{0};                  //!< total time of execute() function - not monitored
      TrigTimer* m_timerCallEB{0};                //!< time untill call of EB in the merged system 

      AbortingCookTimer m_abortTimer;   //!< Timer for hard event timeout (abort)
      IncidentTimer m_incidentTimer;    //!< Timer for soft event timeout (fire incident)
   };
} // end namespace

#endif
