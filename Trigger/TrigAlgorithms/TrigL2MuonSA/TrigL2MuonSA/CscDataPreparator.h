/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_CSCDATAPREPARATOR_H
#define  TRIGL2MUONSA_CSCDATAPREPARATOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "RegionSelector/IRegSelSvc.h"
#include "Identifier/IdentifierHash.h"

#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/CscData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
//#include "TrigL2MuonSA/MuonRoad.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "CscClusterization/ICscClusterBuilder.h"

class StoreGateSvc;
class ActiveStoreSvc;
class CscIdHelper;
class ICscClusterBuilder;

namespace Muon {
  class IMuonRdoToPrepDataTool;
}

namespace MuonGM {
  class MuonDetectorManager;
  class CscReadoutElement;
  class MuonStation;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {
  
  class CscDataPreparator: public AlgTool
  {
  public:
    
    static const InterfaceID& interfaceID();
    
  public:
    
    CscDataPreparator(const std::string& type, 
		      const std::string& name,
		      const IInterface*  parent);
    
    ~CscDataPreparator();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
    
  public:
    
    StatusCode prepareData(const LVL1::RecMuonRoI*  p_roi,
			   //TrigL2MuonSA::MuonRoad&  muonRoad,
			   double aw, double bw, double rWidth,
			   TrigL2MuonSA::CscHits&   cscHits);

  private:
    
    MsgStream* m_msg;
    inline MsgStream& msg() const { return *m_msg; }

    StatusCode getCscHits(const LVL1::RecMuonRoI*  p_roi,
			  //TrigL2MuonSA::MuonRoad&  muonRoad,
			  double aw, double bw, double rWidth,
			  TrigL2MuonSA::CscHits&   cscHits);

    double calc_residual(double aw,
			 double bw,
			 double x,
			 double y);

  public:
    
    inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
    
  private:

    // Reference to StoreGateSvc;
    ServiceHandle<StoreGateSvc>    m_storeGateSvc;
    
    // Cache the ActiveStoreSvc ptr
    ActiveStoreSvc* p_ActiveStore;

    // Geometry Services
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const CscIdHelper* m_cscIdHelper;

    // Region Selector
    IRegSelSvc*          m_regionSelector;
    
    // CSC PrepDataProvider
    ToolHandle<Muon::IMuonRdoToPrepDataTool> m_cscPrepDataProvider;
    ToolHandle<ICscClusterBuilder>           m_cscClusterProvider;

  };
  
} // namespace TrigL2MuonSA

#endif  // 
