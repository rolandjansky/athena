/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONCALIB_MUONCALIBALG_H
#define MUONCALIB_MUONCALIBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonPrdSelector/MuonIdCutTool.h"

class StoreGateSvc;
class MdtIdHelper;

class CscIdHelper;
class ICscStripFitter;

class RpcIdHelper;
class TgcIdHelper;
class TileTBID;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class RpcPrepData;
}

namespace MuonCalib {

  class MuonCalibEventInfo;
  class MuonCalibTriggerTimeInfo;
  class MuonCalibTruthCollection;
  class MuonCalibRawHitCollection;
  class MuonCalibRawTriggerHitCollection;
  class IMuonCalibTool;
  class IIdToFixedIdTool;
  class MuonCalibRawRpcHit;

  /**
     @class MuonCalibAlg
     ATHENA Algorithm retrieving the ingredients for a MuonCalibEvent from storegate and passing them to a calibration tool.
     
     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
  */   
  class MuonCalibAlg : public AthAlgorithm {
  public:
    /**
       Algorithm constructor declaring all properties and opening a ROOT file
    */
    MuonCalibAlg(const std::string& name, ISvcLocator* pSvcLocator); 
    
    /**
       Algorithm destructor (closing the ROOT file)
    */
    ~MuonCalibAlg(); 

    /**
       Algorithm initialize:
       - retrieves StoreGateSvc
       - retrieves MuonCalibTool
       - retrieves auxillairy classes to construct Calib EDM classes (IdHelpers, IdToFixedIdTool, DetectorStore)
       
     */
    StatusCode initialize();     
  
    /** Algorithm execute, called once per event. It performs the following tasks:
	- retrieve MuonCalibEvent
	- feeds MuonCalibEvent to chosen MuonCalibTool
    */
    StatusCode execute();

    /** Algorithm finalize 
	Calls finalize routine of the MuonCalibTool
    */
    StatusCode finalize();

  private:
    /** create a MuonCalibRawRpcHit from a RpcPrepData */
    MuonCalibRawRpcHit* createRawRpcHit( const Muon::RpcPrepData& prd ) const;

    /** retrieve patterns, truth, eventinfo and raw hits and store in MuonCalibEvent */
    const MuonCalibEvent* retrieveEvent() const;

    /** retrieve PrepRawData of all technologies, convert them to MuonCalibRawXxxHits and store them on MuonCalibRawHitCollection */
    const MuonCalibRawHitCollection* retrieveRawHits( const MuonCalibEvent::MCPVec &patterns ) const;

    /** retrieve RawTriggerData convert them to MuonCalibRawTriggerXxxHits and store them on MuonCalibRawTriggerHitCollection */
    const MuonCalibRawTriggerHitCollection* retrieveRawTriggerHits() const;

    /** retrieve patterns and segments from storegate */
    const MuonCalibEvent::MCPVec retrievePatterns() const;

    /** retrieve truth information from storegate and convert to MuonCalibTruthCollection */
    const MuonCalibTruthCollection* retrieveTruthCollection() const;
   
    /** retrieve event information from storegate and convert to MuonCalibEventInfo */
    MuonCalibEventInfo retrieveEventInfo() const;
 
    /** retrieve event trigger time information from storegate and convert to MuonCalibTriggerTimeInfo */
    const MuonCalibTriggerTimeInfo* retrieveTriggerTimeInfo() const;

    const MuonGM::MuonDetectorManager*  m_detMgr;   //!< Pointer to MuonDetectorManager 
    StoreGateSvc* p_StoreGateSvc;                   //!< Pointer to StoreGateSvc 
    std::string m_globalPatternLocation;            //!< Location of the MuonCalibPattern in StoreGate

    /* RtCalibration initialization */
    ToolHandle<MuonCalib::IMuonCalibTool> m_muonCalibTool;     //!< pointer to RtCalibrationCreator
    unsigned int m_eventNumber;       //!< counter keeping track of the event
    std::string  m_eventTag;                         //!< tag of the event, describing reconstruction mechanism 
    std::string  m_ntupleName;        //!< name of the ntuple
  
    /* IdentifierTool initialization */
    ToolHandle<IIdToFixedIdTool> m_idToFixedIdTool;      //!< pointer to IdToFixedIdtool

    // Strip fitter to fit 
    // time samples and extract strip charge
    ToolHandle<ICscStripFitter> m_stripFitter;


    // Tool to cut on identifiers
    ToolHandle<IMuonIdCutTool> m_muonIdCutTool;
   
    /* Pointers to the Identifier Helpers */
    const MdtIdHelper*  m_mdtIdHelper;              //!< MDT specific ID helper
    const CscIdHelper*  m_cscIdHelper;              //!< CSC specific ID helper
    const RpcIdHelper*  m_rpcIdHelper;              //!< RPC specific ID helper
    const TgcIdHelper*  m_tgcIdHelper;              //!< TGC specific ID helper

    std::vector <const MuonCalibEvent*> m_events;         //!< vector holding pointers to events, for deletion at finalize

    bool m_doTruth;           //!< property toggeling if truth info is stored on ntuple 
    bool m_doTruthMuonOnly;   //!< property toggeling to store ONLY muon truth data on ntuple for cavern background
    bool m_doRawData ;        //!< property toggeling if raw hits are stored on ntuple 
    bool m_doDeleteEvent;     //!< property toggling if event deleted at execute or at finalize, false = delete at finalize (default)
    bool m_createRootFile;    //!< property toggeling if the ntuple is created 
    bool m_addTriggerTag;     //!< property toggling if the Trigger type should be added to the EventTag
    bool m_mdt_tdc_cut;       //!< property toggling if a tdc cut should be applied for MDT data 
    mutable bool m_trigger_waning_printed;
    mutable bool m_lar_waning_printed;
    mutable bool m_mbts_warning_printed;
    bool m_doPrdSelect;       //!< property turning on cuts on truth hits if the global flag doPrdSelect = True
    bool m_doMDTs;            //!< Process MDTs or not
    bool m_doCSCs;            //!< Process CSCs or not
    bool m_doRPCs;            //!< Process RPCs or not
    bool m_doTGCs;            //!< Process TGCs or not
    bool m_doRPCCoin;         //!< Process Coin RPCs or not
    bool m_doTGCCoin;         //!< Process Coin TGCs or not
    bool m_doRpcSectorLogic;  //!< Process rpc sector logic
    bool m_doRPCTriggerHits;  //!< Process rpc trigger hit info
    float m_mbts_threshold;   //!< MBTS threshold
    const TileTBID* m_tileTBID; //!< TileTBID
    bool m_ntupFileOpen;
  };

}

#endif

 
