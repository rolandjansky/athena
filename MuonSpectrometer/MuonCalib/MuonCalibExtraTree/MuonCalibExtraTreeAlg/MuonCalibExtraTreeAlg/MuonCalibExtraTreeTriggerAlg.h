/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONCALIB_MUONCALIBEXTRATREETRIGGERALG_H
#define MUONCALIB_MUONCALIBEXTRATREETRIGGERALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonCalibExtraTreeAlg/MuonCalibMuCTPIBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibCaloBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibTriggerInfoBranch.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"

class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
//class TileDetDescrManager;
class TileID;
class TTree;


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
    StatusCode initialize()override;     
  
    /** Algorithm execute, called once per event. It performs the following tasks:    */
    StatusCode execute()override;

    /** Algorithm finalize */
    StatusCode finalize()override;

  private:
    void addMuCTPI();
    void addCTP();
    void addMBTS();
    void addCalo();
    void finishEvent();

    ToolHandle< LVL1::ITrigT1MuonRecRoiTool > m_rpcRoiTool{ this, "RPCRecRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool", "RPC Rec Roi Tool"};
    ToolHandle< LVL1::ITrigT1MuonRecRoiTool > m_tgcRoiTool{ this, "TGCRecRoiTool", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool", "TGC Rec Roi Tool"};


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

 
