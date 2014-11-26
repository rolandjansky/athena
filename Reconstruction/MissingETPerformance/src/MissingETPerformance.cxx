/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <vector>

#include "MissingETPerformance/MissingETPerformance.h"

MissingETPerformance::MissingETPerformance(const std::string& name,
  ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
                              mLog(0),
			      _data ( "MissingETData" ),
			      _resolutionTool ( "ResolutionTool" ),
			      _basicPlotsTool ( "BasicPlotsTool" ),
			      _linearityTool ("LinearityTool" ),
			      _etaRingsTool ( "EtaRingsTool" ),
                              _trigMissingETTool ( "TrigMissingETTool" ),
                              _trigVsOfflineMissingETTool ( "TrigVsOfflineMissingETTool" ),
			      _fakeMissingETTool ( "FakeMissingETTool" ),
			      _zMuMuTool ( "ZMuMuTool" ),
			      _zeeTool ( "ZeeTool" ),
                              _muonTool ( "MuonTool" ),
			      _containerComparatorTool ( "ContainerComparatorTool" ),
			      _pileUpTool ( "PileUpTool" ),
			      m_trigDec ("Trig::TrigDecisionTool/TrigDecisionTool"),
                              _muonData ("MissingETMuonData" ),
			      _MissingETScaleTool ("MissingETScaleTool" ),
			      _eventSelector ( "MissingETEventSelector" ),
			      _metCompositionTool ( "MissingETCompositionTool" ),
                              m_storeGate(0)
{

  //whether to run each tool
  declareProperty("doBasicPlotsTool",              _doBasicPlotsTool=true);
  declareProperty("doResolutionTool",              _doResolution=true);
  declareProperty("doLinearityTool",               _doLinearity=true);
  declareProperty("doTrigMissingETTool",           _doTrigMissingETTool=true);
  declareProperty("doTrigVsOfflineMissingETTool",  _doTrigVsOfflineMissingETTool=true);
  declareProperty("doEtaRingsTool",                _doEtaRingsTool=false);
  declareProperty("doFakeMissingETTool",           _doFakeMissingETTool=false);
  declareProperty("doZMuMuTool",                   _doZMuMuTool=false);
  declareProperty("doZeeTool",                     _doZeeTool=false);
  declareProperty("doMuonTool",                    _doMuonTool=false);
  declareProperty("doContainerComparatorTool",     _doContainerComparatorTool=false);
  declareProperty("doPileUpTool",                  _doPileUpTool=false);
  declareProperty("doEventSelector",               _doEventSelector=false);
  declareProperty("doMissingETScaleTool",           _doMissingETScaleTool=false);
  declareProperty("doMissingETCompositionTool",    _doMETCompositionTool=false);

  declareProperty("FolderName",                    m_folderName="");

  declareProperty("FilterOnTrigger", _filterOnTrigger=false, "whether to require a trigger from the list to have fired");
  declareProperty("TriggerNames", trigger_names, "List of trigger names to accept");
  declareProperty("TrigDecisionTool", m_trigDec, "Tool to access the trigger decision");

  declareProperty("ResolutionTool",                      _resolutionTool);
  declareProperty("BasicPlotsTool",                      _basicPlotsTool);
  declareProperty("LinearityTool",                       _linearityTool);
  declareProperty("EtaRingsTool",                        _etaRingsTool);
  declareProperty("TrigMissingETTool",                   _trigMissingETTool);
  declareProperty("TrigVsOfflineMissingETTool",          _trigVsOfflineMissingETTool);
  declareProperty("FakeMissingETTool",                   _fakeMissingETTool);
  declareProperty("ZMuMuTool",                           _zMuMuTool);
  declareProperty("ZeeTool",                             _zeeTool);
  declareProperty("MuonTool",                            _muonTool);
  declareProperty("ContainerComparatorTool",             _containerComparatorTool);
  declareProperty("PileUpTool",                          _pileUpTool);
  declareProperty("MissingETScaleTool",                  _MissingETScaleTool);
  declareProperty("MissingETEventSelector",              _eventSelector);
  declareProperty("MissingETCompositionTool",            _metCompositionTool);
}

MissingETPerformance::~MissingETPerformance() {}

StatusCode MissingETPerformance::CBNT_initializeBeforeEventLoop() {
  mLog = new MsgStream(messageService(), name() );
  *mLog << MSG::DEBUG << "Initializing MissingETPerformance (before eventloop)" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if (_filterOnTrigger) {
    StatusCode sc = m_trigDec.retrieve();
    if ( !sc.isSuccess() ){
      *mLog << MSG::ERROR << "Can't get handle on TrigDecisionTool" << endreq;
      return sc;
    }
  *mLog << MSG::INFO << "Get handle on TrigDecisionTool" << endreq;
  }

  sc = _data.retrieve();
  sc = _muonData.retrieve();
  if (!sc.isSuccess()) {}
  return sc;
} 

StatusCode MissingETPerformance::initialize() {return CBNT_initialize();}
StatusCode MissingETPerformance::finalize() {return CBNT_finalize();}
StatusCode MissingETPerformance::execute() {return CBNT_execute();}

StatusCode MissingETPerformance::CBNT_initialize() {

  CBNT_initializeBeforeEventLoop();

  *mLog << MSG::DEBUG << "Initializing MissingETPerformance" << endreq;

  // end foldername with slash
  if (!m_folderName.empty()) {
    if (m_folderName.rfind("/")!=m_folderName.size()-1)
      m_folderName = m_folderName + "/";
  }

  /** get a handle of StoreGate for access to the Event Store */
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  StatusCode sctool = StatusCode::SUCCESS;

  if (!sc.isSuccess()) {
     *mLog << MSG::ERROR
          << "Unable to retrieve pointer to StoreGateSvc"
          << endreq;
     return sc;
  }

  //do we run this tool?
  if (_doBasicPlotsTool) {
    /// get a handle on the basic plots tool
    sc = _basicPlotsTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on BasicPlotsTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _basicPlotsTool->SetFolderName(m_folderName); }
    sctool = _basicPlotsTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//BasicPlotsTool

  //do we run this tool?
  if (_doTrigMissingETTool) {
    /// get a handle on TrigMissingET tool
    sc = _trigMissingETTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on TrigMissingETTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _trigMissingETTool->SetFolderName(m_folderName); }
    sctool = _trigMissingETTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//TrigMissingETTool

  //do we run this tool?
  if (_doTrigVsOfflineMissingETTool) {
    /// get a handle on TrigVsOfflineMissingET tool
    sc = _trigVsOfflineMissingETTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on TrigVsOfflineMissingETTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _trigVsOfflineMissingETTool->SetFolderName(m_folderName); }
    sctool = _trigVsOfflineMissingETTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//TrigVsOfflineMissingETTool
 
  
  //do we run this tool?
  if (_doResolution) {
    /// get a handle on the resolution tool
    sc = _resolutionTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on ResolutionTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _resolutionTool->SetFolderName(m_folderName); }
    sctool = _resolutionTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//ResolutionTool

  //do we run this tool?
  if (_doLinearity) {
    /// get a handle on the linearity tool
    sc = _linearityTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on LinearityTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _linearityTool->SetFolderName(m_folderName); }
    sctool = _linearityTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//LinearityTool

  //do we run this tool?
  if (_doEtaRingsTool) {
    /// get a handle on the eta rings tool
    sc = _etaRingsTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on EtaRingsTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _etaRingsTool->SetFolderName(m_folderName); }
    sctool = _etaRingsTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//EtaRingsTool

  //do we run this tool?
  if (_doFakeMissingETTool) {
    /// get a handle on the fake MET tool
    sc = _fakeMissingETTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on FakeMissingETTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _fakeMissingETTool->SetFolderName(m_folderName); }
    sctool = _fakeMissingETTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//FakeMissingETTool

  if (_doMuonTool) {
    /// get a handle on the Muon tool
    sc = _muonTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on MuonTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _muonTool->SetFolderName(m_folderName); }
    sctool = _muonTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//MuonTool

  //do we run this tool?
  if (_doContainerComparatorTool) {
    /// get a handle on the basic plots tool
    sc = _containerComparatorTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on ContainerComparatorTool" << endreq;
      return sc;
    }
    sctool = _containerComparatorTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//ContainerComparatorTool

  //do we run this tool?
  if (_doZMuMuTool) {
    /// get a handle on the ZMuMu tool
    sc = _zMuMuTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on ZMuMuTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _zMuMuTool->SetFolderName(m_folderName); }
    sctool = _zMuMuTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//ZMuMuTool

  //do we run this tool?
  if (_doZeeTool) {
    /// get a handle on the Zee tool
    sc = _zeeTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on ZeeTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _zeeTool->SetFolderName(m_folderName); }
    sctool = _zeeTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//ZeeTool
  
  //do we run this tool?
  if (_doPileUpTool) {
    /// get a handle on the pile up tool
    sc = _pileUpTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on PileUpTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _pileUpTool->SetFolderName(m_folderName); }
    sctool = _pileUpTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//PileUpTool

  //do we run this tool?
   if (_doMissingETScaleTool) {
     /// get a handle on the Scale MET tool
     sc = _MissingETScaleTool.retrieve();
 
     if ( !sc.isSuccess() ) {
       *mLog << MSG::ERROR << "Can't get handle on MissingETScaleTool" << endreq;
       return sc;
     }
     sctool = _MissingETScaleTool->CBNT_initialize();
     if ( !sctool.isSuccess() ) {
       *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
       return sctool;
     }
   }//ScaleTool

  //do we run this tool?
   if (_doMETCompositionTool) {
    /// get a handle on the met composition tool
    sc = _metCompositionTool.retrieve();

    if ( !sc.isSuccess() ) {
      *mLog << MSG::ERROR << "Can't get handle on MissingETCompositionTool" << endreq;
      return sc;
    }
    if (!m_folderName.empty()) { _metCompositionTool->SetFolderName(m_folderName); }
    sctool = _metCompositionTool->CBNT_initialize();
    if ( !sctool.isSuccess() ) {
      *mLog << MSG::ERROR << "Failed to initialize tool." << endreq;
      return sctool;
    }
  }//MissingETCompositionTool

  return sc;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode MissingETPerformance::CBNT_finalize() {

  StatusCode sc;
  if (_doResolution) {
    sc = _resolutionTool->CBNT_finalize();
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "ResolutionTool finalize failed" << endreq;
      return sc;
    }
  }

  if (_doLinearity) {
    sc = _linearityTool->CBNT_finalize();
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "LinearityTool finalize failed" << endreq;
      return sc;
    }
  }
  
  if (_doPileUpTool) {
    sc = _pileUpTool->CBNT_finalize();
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "PileUpTool finalize failed" << endreq;
      return sc;
    }
  }
  
  return sc;
}

///////////////////////////////////////////////////////////////////////////////////
/// Clear - clear CBNT members
StatusCode MissingETPerformance::CBNT_clear() {

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode MissingETPerformance::CBNT_execute() {
  
  *mLog << MSG::DEBUG << "in execute()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  //if requested, check whether event passed trigger
  if (_filterOnTrigger && !triggerFired()) {
    //no trigger in the list given in jobOptions fired, don't process event
    return sc;
  }

  MissingETData *data = &(*_data);
  sc = data->retrieveContainers();

  if ( !sc.isSuccess() ) {
    *mLog << MSG::WARNING << "retrieveContainers in MissingETData Failed" << endreq;
    return sc;
  }

  //if requested, check whether event passed EventSelector
  if (_doEventSelector) {
    sc = _eventSelector->retrieveContainers();
    _eventSelector->passMissingETData(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "retrieveContainers in MissingETPerformanceEventSelector Failed" << endreq;
      return sc;
    }
    if (!_eventSelector->isSelectedEvent()) {
      return StatusCode::SUCCESS;
    }//if passes EventSelector
  }//if doEventSelector

  if (_doLinearity) {
    sc = _linearityTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "LinearityTool Failed" << endreq;
      return sc;
    }
  }

  if (_doBasicPlotsTool) {
    sc = _basicPlotsTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "BasicPlotsTool Failed" << endreq;
      return sc;
    }
  }

  if (_doResolution) {
    sc = _resolutionTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "ResolutionTool Failed" << endreq;
      return sc;
    }
  }

  if (_doTrigMissingETTool) {
    sc = _trigMissingETTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "TrigMissingETTool Failed" << endreq;
      return sc;
    }
  }

  if (_doTrigVsOfflineMissingETTool) {
    sc = _trigVsOfflineMissingETTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "TrigVsOfflineMissingETTool Failed" << endreq;
      return sc;
    }
  }

  if (_doEtaRingsTool) {
    sc = _etaRingsTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "EtaRingsTool Failed" << endreq;
      return sc;
    }
  }

  MissingETMuonData *muondata = &(*_muonData);
  sc = _muonData->retrieveMuons();

  if (_doFakeMissingETTool) {
    sc = _fakeMissingETTool->execute(data, muondata);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "FakeMissingETTool Failed" << endreq;
      return sc;
    }
  }

  if (_doMuonTool) {
    sc = _muonTool->execute(muondata);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "MuonTool Failed" << endreq;
      return sc;
    }
  }

  if (_doContainerComparatorTool) {
    sc = _containerComparatorTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "ContainerComparatorTool Failed" << endreq;
      return sc;
    }
  }

  if (_doZMuMuTool) {
    sc = _zMuMuTool->execute(data, muondata);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "ZMuMuTool Failed" << endreq;
      return sc;
    }
  }

  if (_doZeeTool) {
    sc = _zeeTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "ZeeTool Failed" << endreq;
      return sc;
    }
  }
  
  if (_doPileUpTool) {
    sc = _pileUpTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "PileUpTool Failed" << endreq;
      return sc;
    }
  }

  if (_doMissingETScaleTool) {
     sc = _MissingETScaleTool->execute(data);
     if ( !sc.isSuccess() ) {
       *mLog << MSG::WARNING << "MissingETScaleTool Failed" << endreq;
       return sc;
     }
   }

  if (_doMETCompositionTool) {
    sc = _metCompositionTool->execute(data);
    if ( !sc.isSuccess() ) {
      *mLog << MSG::WARNING << "MissingETCompositionTool Failed" << endreq;
      return sc;
    }
  }

  return sc;
}

bool MissingETPerformance::triggerFired() {

  //loop over trigger names
  for (std::vector<std::string>::iterator trigname = trigger_names.begin(); trigname != trigger_names.end(); ++trigname) {
    if (m_trigDec->isPassed(*trigname, TrigDefs::eventAccepted)) {
      //trigger passed, return true
      return true;
    }
  }
  //no trigger passed, return false
  return false;
}//end triggerFired()
