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
   MsgStream *mLog;

   std::string m_folderName;

   //missing et data
   ToolHandle<MissingETData>                       _data;
   /** get a handle on the tools */
   ToolHandle<ResolutionTool>                      _resolutionTool;
   ToolHandle<BasicPlotsTool>                      _basicPlotsTool;
   ToolHandle<LinearityTool>                       _linearityTool;
   ToolHandle<EtaRingsTool>                        _etaRingsTool;
   ToolHandle<TrigMissingETTool>                   _trigMissingETTool;
   ToolHandle<TrigVsOfflineMissingETTool>          _trigVsOfflineMissingETTool;
   ToolHandle<FakeMissingETTool>                   _fakeMissingETTool;
   ToolHandle<ZMuMuTool>                           _zMuMuTool;
   ToolHandle<ZeeTool>                             _zeeTool;
   ToolHandle<MuonTool>                            _muonTool;
   ToolHandle<ContainerComparatorTool>             _containerComparatorTool;
   ToolHandle<PileUpTool>                          _pileUpTool;
   ToolHandle<Trig::TrigDecisionTool>           m_trigDec;
   ToolHandle<MissingETMuonData>                   _muonData;
   ToolHandle<MissingETScaleTool>                  _MissingETScaleTool;
   ToolHandle<MissingETEventSelector>              _eventSelector;
   ToolHandle<MissingETCompositionTool>            _metCompositionTool;

   bool _doBasicPlotsTool;
   bool _doResolution;
   bool _doLinearity;
   bool _doEtaRingsTool;
   bool _doTrigMissingETTool;
   bool _doTrigVsOfflineMissingETTool;
   bool _doFakeMissingETTool;
   bool _doZMuMuTool;
   bool _doZeeTool;
   bool _doMuonTool;
   bool _doContainerComparatorTool;
   bool _doPileUpTool;
   bool _doEventSelector;
   bool _doMissingETScaleTool;
   bool _doMETCompositionTool;

   //whether to require a trigger from the list to have fired
   bool _filterOnTrigger;
   //list of triggers to require to have fired, use the OR of all
   std::vector<std::string> trigger_names;
   //did a trigger fire?
   bool triggerFired();

   /** a handle on Store Gate for access to the Event Store */
   StoreGateSvc* m_storeGate;
};

#endif // MISSINGETPERFORMANCE_H
