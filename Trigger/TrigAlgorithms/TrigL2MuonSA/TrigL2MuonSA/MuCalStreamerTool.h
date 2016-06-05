/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUCALSTREAMERTOOL_H
#define  TRIGL2MUONSA_MUCALSTREAMERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigL2MuonSA/MdtData.h"
#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/CscData.h"
#include "TrigL2MuonSA/TrackData.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "TrigL2MuonSA/TgcDataPreparator.h"
#include "RegionSelector/IRegSelSvc.h"

//#include "TGCcablingInterface/ITGCcablingServerSvc.h"
//#include "TGCcablingInterface/ITGCcablingSvc.h"
//#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

/////////////////////////////////////////////////////////////

#include <fstream>
#include <string>
#include <vector>

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace LVL1 {
  class RecMuonRoI;
}

namespace LVL2_MUON_CALIBRATION {
  class MdtCalibFragment;
  class RpcCalibFragment;
  class TgcCalibFragment;
  class CscCalibFragment;
}

class RpcPad;
class StoreGateSvc;
class ROBDataProviderSvc;

inline unsigned bcBitmap(uint16_t bcTag) { 
  return (bcTag == 1 ? 4 : 
          ((bcTag == 0 || bcTag == 2) ? 2 :
           (bcTag == 3 ? 1 : 0)));
}
 
namespace TrigL2MuonSA {
  
  class MuCalStreamerTool: public AthAlgTool
  {
  public:
    
    static const InterfaceID& interfaceID();
    
  public:
    
    MuCalStreamerTool(const std::string& type, 
		      const std::string& name,
		      const IInterface*  parent);
    
    ~MuCalStreamerTool();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    std::string instanceName() const {return m_algInstanceName;}
    void setInstanceName(std::string name) { m_algInstanceName = name; }

    // set the properties
    void setBufferName(std::string buffName) {m_calBufferName=buffName;}

    std::vector<int>* getLocalBuffer()   {return m_localBuffer;}
    int getLocalBufferSize() {return m_localBuffer->size();}
    void clearLocalBuffer();

    //
    // initialize the stream
    StatusCode openStream(int calBufferSize);

    //
    // finalize and close the stream
    StatusCode closeStream();
    
    //
    // create the fragment corresponding to an roi
    StatusCode createRoiFragment(const LVL1::RecMuonRoI* roi,
				 TrigL2MuonSA::TrackPattern& trackPattern,
				 TrigL2MuonSA::MdtHits& mdtHits,
				 TrigL2MuonSA::RpcHits& rpcHits,
				 TrigL2MuonSA::TgcHits& tgcHits,
				 int calBufferSize,
				 bool doDataScouting,
                                 bool& updateTriggerElement);                                                              

  private:

    BooleanProperty m_writeToFile;

    // name of the calibration buffer or of the 
    // output file
    std::string  m_calBufferName;
    std::string m_algInstanceName;

    // output file 
    std::ofstream* m_outputFile;

    // Reference to StoreGateSvc;
    ServiceHandle<StoreGateSvc>    m_storeGate;

    // the region selector
    IRegSelSvc*  m_regionSelector;
      
    // RPC cabling service
    const IRPCcablingSvc* m_rpcCabling;
    const CablingRPCBase* m_rpcCablingSvc;

    // tgc cabling maps
    //    const ITGCcablingSvc* m_tgcCabling;
    //    const TGCCablingBase* m_tgcCablingSvc;

    // ROB DataProvider
    ROBDataProviderSvc* m_robDataProvider;

    // id of the circular buffer
    int m_cid;
    int m_calibEvent;

    // local buffer for the TrigComposite object
    int m_localBufferSize;
    //    int m_maxLocalBufferSize;
    std::vector<int>* m_localBuffer;

    // pointer to the muon roi
    const LVL1::RecMuonRoI* m_roi;

    // the tgc data preparator needed to retrieve the list of Hashes
    ToolHandle<TgcDataPreparator>  m_tgcDataPreparator;

    
    // TGC raw data provider
    //    ToolHandle<Muon::IMuonRawDataProviderTool>  m_tgcRawDataProvider;

    
    //
    // create the MDT fragment
    StatusCode createMdtFragment(TrigL2MuonSA::MdtHits& mdtHits,  
				 LVL2_MUON_CALIBRATION::MdtCalibFragment& mdtFragment, float phi);

    //
    // create the RPC fragment
    StatusCode createRpcFragment(const LVL1::RecMuonRoI* roi,
				 LVL2_MUON_CALIBRATION::RpcCalibFragment& rpcFragment);

    //
    // create the TGC fragment
    StatusCode createTgcFragment(std::vector<uint32_t>& tgcRobIdList,
				 LVL2_MUON_CALIBRATION::TgcCalibFragment& tgcFragment);
    //
    //
    StatusCode createCscFragment(std::vector<uint32_t>& cscRobIdList, 
    				 LVL2_MUON_CALIBRATION::CscCalibFragment& cscFragment);

  };
  
} // namespace TrigL2MuonSA

#endif  // 
