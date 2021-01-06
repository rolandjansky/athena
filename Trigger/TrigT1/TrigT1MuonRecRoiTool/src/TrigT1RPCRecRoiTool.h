/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RPCRECROITOOL_H
#define TRIGT1RPCRECROITOOL_H

#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Muon{
  class IMuonIdHelperSvc;
}
namespace MuonGM{
  class MuonDetectorManager;
}
class RpcCablingCondData;
class IRPCcablingSvc;

namespace LVL1 {

  class TrigT1RPCRecRoiTool: public extends<AthAlgTool, ITrigT1MuonRecRoiTool> {
  public:
      TrigT1RPCRecRoiTool(const std::string& type, 
			  const std::string& name,
			  const IInterface* parent);
      virtual ~TrigT1RPCRecRoiTool();
      virtual StatusCode initialize() override;

      virtual TrigT1MuonRecRoiData roiData (const unsigned int& roiWord) const override;
      
      virtual void RoIsize(const unsigned int& roiWord,
			   double& etaMin, double& etaMax, 
			   double& phiMin, double& phiMax) const override;
      
      virtual bool dumpRoiMap(const std::string& filename) const override;
      
      // RoI edges for Low-pt and High-pt confirm planes 
      bool etaDimLow (const TrigT1MuonRecRoiData& data,
		      double& etaMin, double& etaMax,
		      std::unique_ptr<const RpcCablingCondData> readCdo) const;
      bool etaDimHigh(const TrigT1MuonRecRoiData& data,
		      double& etaMin, double& etaMax,
		      std::unique_ptr<const RpcCablingCondData> readCdo) const;
      
  private:
      BooleanProperty m_useRun3Config{this,"UseRun3Config",false,"use Run 3 config"};
      BooleanProperty m_useConditionData{this,"UseConditionData",true,"use condition data"};
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey","MuonDetectorManager","Key of input MuonDetectorManager condition data"};
      SG::ReadCondHandleKey<RpcCablingCondData> m_readKey{this, "ReadKey", "RpcCablingCondData", "Key of RpcCablingCondData"};
      const MuonGM::MuonDetectorManager* m_muonMgr{nullptr};
      const IRPCcablingSvc* m_cabling{nullptr};
      
    }; // end of TrigT1RPCRecRoiTool
  
} // namespace LVL1


#endif
