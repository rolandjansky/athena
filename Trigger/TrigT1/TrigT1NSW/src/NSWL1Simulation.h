/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1NSW_NSWL1SIMULATION_H
#define TRIGT1NSW_NSWL1SIMULATION_H

// Basic includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"

// NSWL1SimTools includes
#include "TrigT1NSWSimTools/IPadTdsTool.h"
#include "TrigT1NSWSimTools/IStripTdsTool.h"
#include "TrigT1NSWSimTools/IStripClusterTool.h"
#include "TrigT1NSWSimTools/IStripSegmentTool.h"
#include "TrigT1NSWSimTools/IMMStripTdsTool.h"
#include "TrigT1NSWSimTools/IMMTriggerTool.h"
#include "TrigT1NSWSimTools/IPadTriggerLogicTool.h"
#include "TrigT1NSWSimTools/IPadTriggerLookupTool.h"

#include "MuonRDO/NSW_PadTriggerDataContainer.h"
#include "MuonRDO/NSW_TrigRawDataContainer.h"
#include "TrigT1NSWSimTools/PadTriggerAdapter.h"
#include "TrigT1NSWSimTools/TriggerProcessorTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TTree.h"

// Forward includes
class StoreGateSvc;
class TTree;


// namespace for the NSW LVL1 related classes
namespace NSWL1 {


  /**
   *
   *   @short NSW L1 simulation algorithm
   *
   * This is the algorithm that masters the simulation of the Level-1 into the NSW.
   * This algorithm will manage the configuration of the NSW LV1 and handle the tools
   * used for simulating the hardware components, for monitoring the basic functionalities
   * and for collecting the data into an ntuple (parameter definition and performance 
   * studies). Depending of the job configuration it executes the pure offline simulation
   * or the detailed hardware simulation.
   *
   *  @authors Alessandro Di Mattia <dimattia@cern.ch>, Geraldine Conti <geraldine.conti@cern.ch>
   *  Major updates for Release 22 processing: Francesco Giuseppe Gravili <francesco.giuseppe.gravili@cern.ch>
   *
   */

  class ATLAS_NOT_THREAD_SAFE  // use of TTree in execute
  NSWL1Simulation: public AthAlgorithm {

  public:
    NSWL1Simulation( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode initialize() override;
    virtual StatusCode start() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  protected:
    SG::WriteHandleKey<Muon::NSW_TrigRawDataContainer> m_trigRdoContainer{this, "NSWTrigRDOContainerName", "NSWTRGRDO", "Name of the NSW trigger RDO container"};

  private:
    ToolHandle <IPadTdsTool>           m_pad_tds{this, "PadTdsTool", "NSWL1::PadTdsOfflineTool", "Tool simulating the functionalities of the PAD TDS"};
    ToolHandle <IPadTriggerLogicTool>  m_pad_trigger{this, "PadTriggerTool", "NSWL1::PadTriggerLogicOfflineTool", "Tool simulating the pad trigger logic"};
    ToolHandle <IPadTriggerLookupTool> m_pad_trigger_lookup{this, "PadTriggerLookupTool", "NSWL1::PadTriggerLookupTool", "Tool to lookup pad trigger patterns per execute against the same LUT as in trigger FPGA"};
    ToolHandle <IStripTdsTool>         m_strip_tds{this, "StripTdsTool", "NSWL1::StripTdsOfflineTool", "Tool simulating the functionalities of the Strip TDS"};
    ToolHandle <IStripClusterTool>     m_strip_cluster{this, "StripClusterTool", "NSWL1::StripClusterTool", "Tool simulating the Strip Clustering"};
    ToolHandle <IStripSegmentTool>     m_strip_segment;
    //ToolHandle <IStripSegmentTool>     m_strip_segment{this, "StripSegmentTool", "NSWL1::StripSegmentTool", "Tool simulating the Segment finding"};
    ToolHandle <IMMStripTdsTool>       m_mmstrip_tds{this, "MMStripTdsTool", "NSWL1::MMStripTdsOfflineTool", "Tool simulating the functionalities of the MM Strip TDS"};
    ToolHandle <IMMTriggerTool>        m_mmtrigger{this, "MMTriggerTool", "NSWL1::MMTriggerTool", "Tool simulating the MM Trigger"};
    ToolHandle <TriggerProcessorTool>  m_trigProcessor{this, "MMTriggerProcessorTool", "NSWL1::TriggerProcessorTool", "Tool simulating the TP"};

    Gaudi::Property<bool> m_useLookup{this, "UseLookup", false, "Toggle Lookup mode on and off default is the otf(old) mode"};
    Gaudi::Property<bool> m_doNtuple{this, "DoNtuple", false,  "Create an ntuple for data analysis"};
    Gaudi::Property<bool> m_doMM{this, "DoMM", false, "Run data analysis for MM"};
    Gaudi::Property<bool> m_doMMDiamonds{this, "DoMMDiamonds", false, "Run data analysis for MM using Diamond Roads algorithm"};
    Gaudi::Property<bool> m_dosTGC{this, "DosTGC", false, "Run data analysis for sTGCs"};
    Gaudi::Property<bool> m_doStrip{this, "DoStrip", false, "Run data analysis for sTGC strip trigger"};

    // put analysis variables here
    TTree*       m_tree;                                    //!< analysis ntuple
    unsigned int m_current_run;                             //!< current run number
    unsigned int m_current_evt;                             //!< current event number
  };  // end of NSWL1Simulation class
} // namespace NSWL1
#endif
