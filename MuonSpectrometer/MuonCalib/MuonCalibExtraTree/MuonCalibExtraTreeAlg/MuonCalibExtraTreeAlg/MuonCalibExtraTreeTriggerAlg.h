/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONCALIB_MUONCALIBEXTRATREETRIGGERALG_H
#define MUONCALIB_MUONCALIBEXTRATREETRIGGERALG_H

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonCalibExtraTreeAlg/MuonCalibMuCTPIBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibCaloBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibTriggerInfoBranch.h"

class StoreGateSvc;
class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
//class TileDetDescrManager;
class TileID;
class TTree;

namespace MuonGM {
  class MuonDetectorManager;
}
namespace LVL1 {
  class RecMuonRoiSvc;
}

namespace MuonCalib {

  class IIdToFixedIdTool;

  /**
     @class MuonCalibExtraTreeTriggerAlg
     ATHENA Algorithm to add none-muon trigger information to the calibration ntuple 
     
     @author Niels.Van.Eldik@cern.ch
  */   
  class MuonCalibExtraTreeTriggerAlg : public AthAlgorithm {
  public:
    /**
       Algorithm constructor declaring all properties and opening a ROOT file
    */
    MuonCalibExtraTreeTriggerAlg(const std::string& name, ISvcLocator* pSvcLocator); 
    
    /** Algorithm destructor (closing the ROOT file) */
    ~MuonCalibExtraTreeTriggerAlg(); 

    /**       Algorithm initialize:     */
    StatusCode initialize();     
  
    /** Algorithm execute, called once per event. It performs the following tasks:    */
    StatusCode execute();

    /** Algorithm finalize */
    StatusCode finalize();

  private:
    void addMuCTPI();
    void addCTP();
    void addMBTS();
    void addCalo();
    void finishEvent();

    StoreGateSvc* p_StoreGateSvc;                   //!< Pointer to StoreGateSvc 
    StoreGateSvc* m_detStore;                     //!< Pointer to Detector store 
    ServiceHandle< LVL1::RecMuonRoiSvc > m_rpcRoiService;
    ServiceHandle< LVL1::RecMuonRoiSvc > m_tgcRoiService;

    unsigned int m_eventNumber;       //!< counter keeping track of the event
    std::string  m_eventTag;                         //!< tag of the event, describing reconstruction mechanism 
    std::string  m_ntupleName;        //!< name of the ntuple
  
    const TileID * m_tileID; 
//    const TileDetDescrManager * m_tileMgr;

    TTree* m_tree;

    bool m_doMuCTPI;         //!< process MuCTPI
    bool m_doCTP;            //!< Process CTP  
    bool m_doMBTS;           //!< Process MBTS  
    bool m_doLVL1Calo;       //!< process LVL1Calo

    std::string m_ctpLocation;
    std::string m_muCPTILocation;
    std::string m_caloLocation;
    std::string m_mbtsLocation;

    MuonCalibMuCTPIBranch m_muCTPIBranch;
    MuonCalibCaloBranch   m_mbtsBranch;
    MuonCalibCaloBranch   m_caloBranch;
    MuonCalibTriggerInfoBranch m_ctpBranch;
    bool m_init;
  };

}  //  namespace MuonCalib

#endif

 
