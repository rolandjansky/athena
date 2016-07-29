/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETPERFORMANCE_H
#define MISSINGETPERFORMANCE_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/MissingETMuonData.h"

#include "MissingETPerformance/ResolutionTool.h"
#include "MissingETPerformance/BasicPlotsTool.h"
#include "MissingETPerformance/LinearityTool.h"
#include "MissingETPerformance/EtaRingsTool.h"
#include "MissingETPerformance/TrigMissingETTool.h"
#include "MissingETPerformance/TrigVsOfflineMissingETTool.h"
#include "MissingETPerformance/FakeMissingETTool.h"
#include "MissingETPerformance/ZMuMuTool.h"
#include "MissingETPerformance/ZeeTool.h"
#include "MissingETPerformance/MuonTool.h"
#include "MissingETPerformance/ContainerComparatorTool.h"
#include "MissingETPerformance/PileUpTool.h"
#include "MissingETPerformance/MissingETEventSelector.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "MissingETPerformance/MissingETScaleTool.h"
#include "MissingETPerformance/MissingETCompositionTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

//class MissingETPerformance : public CBNT_AthenaAwareBase  {
class MissingETPerformance : public AthAlgorithm {

 public:

   MissingETPerformance(const std::string& name, ISvcLocator* pSvcLocator);
   ~MissingETPerformance();

/*    virtual StatusCode CBNT_initializeBeforeEventLoop(); */
/*    virtual StatusCode CBNT_initialize(); */
/*    virtual StatusCode CBNT_finalize(); */
/*    virtual StatusCode CBNT_execute(); */
/*    virtual StatusCode CBNT_clear(); */

   StatusCode CBNT_initializeBeforeEventLoop();
   StatusCode CBNT_initialize();
   StatusCode CBNT_finalize();
   StatusCode CBNT_execute();
   StatusCode CBNT_clear();

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();

 private:
   std::string m_folderName;

   //missing et data
   ToolHandle<MissingETData>                       m_data;
   /** get a handle on the tools */
   ToolHandle<ResolutionTool>                      m_resolutionTool;
   ToolHandle<BasicPlotsTool>                      m_basicPlotsTool;
   ToolHandle<LinearityTool>                       m_linearityTool;
   ToolHandle<EtaRingsTool>                        m_etaRingsTool;
   ToolHandle<TrigMissingETTool>                   m_trigMissingETTool;
   ToolHandle<TrigVsOfflineMissingETTool>          m_trigVsOfflineMissingETTool;
   ToolHandle<FakeMissingETTool>                   m_fakeMissingETTool;
   ToolHandle<ZMuMuTool>                           m_zMuMuTool;
   ToolHandle<ZeeTool>                             m_zeeTool;
   ToolHandle<MuonTool>                            m_muonTool;
   ToolHandle<ContainerComparatorTool>             m_containerComparatorTool;
   ToolHandle<PileUpTool>                          m_pileUpTool;
   ToolHandle<Trig::TrigDecisionTool>           m_trigDec;
   ToolHandle<MissingETMuonData>                   m_muonData;
   ToolHandle<MissingETScaleTool>                  m_MissingETScaleTool;
   ToolHandle<MissingETEventSelector>              m_eventSelector;
   ToolHandle<MissingETCompositionTool>            m_metCompositionTool;

   bool m_doBasicPlotsTool;
   bool m_doResolution;
   bool m_doLinearity;
   bool m_doEtaRingsTool;
   bool m_doTrigMissingETTool;
   bool m_doTrigVsOfflineMissingETTool;
   bool m_doFakeMissingETTool;
   bool m_doZMuMuTool;
   bool m_doZeeTool;
   bool m_doMuonTool;
   bool m_doContainerComparatorTool;
   bool m_doPileUpTool;
   bool m_doEventSelector;
   bool m_doMissingETScaleTool;
   bool m_doMETCompositionTool;

   //whether to require a trigger from the list to have fired
   bool m_filterOnTrigger;
   //list of triggers to require to have fired, use the OR of all
   std::vector<std::string> m_trigger_names;
   //did a trigger fire?
   bool triggerFired();
};

#endif // MISSINGETPERFORMANCE_H
