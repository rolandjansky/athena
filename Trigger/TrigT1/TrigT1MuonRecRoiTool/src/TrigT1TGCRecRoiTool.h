/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1TGCRECROITOOL_H
#define TRIGT1TGCRECROITOOL_H

#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TGCIdBase;
class Identifier;
class ITGCcablingSvc;
namespace Muon{
  class IMuonIdHelperSvc;
}
namespace MuonGM{
  class MuonDetectorManager;
}

namespace LVL1 {
  
  class TrigT1TGCRecRoiTool: public extends<AthAlgTool, ITrigT1MuonRecRoiTool> {
  public:
      TrigT1TGCRecRoiTool(const std::string& type, 
			  const std::string& name,
			  const IInterface* parent);
      virtual ~TrigT1TGCRecRoiTool();
      virtual StatusCode initialize() override;
      
      virtual TrigT1MuonRecRoiData roiData(const unsigned int& roiWord ) const override;
      
      virtual void RoIsize(const unsigned int& roiWord,
			   double& etaMin, double& etaMax, 
			   double& phiMin, double& phiMax) const override;
      
      virtual bool dumpRoiMap(const std::string& filename) const override;

      // does not exists for TGCs
      bool etaDimLow (const TrigT1MuonRecRoiData& , double& , double& ) const override {return false;}
      bool etaDimHigh(const TrigT1MuonRecRoiData& , double& , double& ) const override {return false;}
      
  private:
      
      enum EdgeType {NonEdge=-1,
		     LowerREdge,
		     UpperREdge,
		     LowerPhiEdge,
		     UpperPhiEdge};
      enum DummyOffset {DummyOffset=-9999};
      
      bool getSLBparameters(const unsigned int & roIWord,
			    TGCIdBase & tgcIdBase,
			    unsigned int & sectorID,
			    unsigned int & roiNumber,
			    unsigned int & r,
			    unsigned int & phi,
			    int & wireSLBId,
			    int & block ) const;
      
      std::unique_ptr<TGCIdBase> getWireASDOut(TGCIdBase tgcIdBase,
					       unsigned int sectorID,
					       int wireSLBId,
					       int block,
					       int phi ,
					       int offset) const;
      
      std::unique_ptr<TGCIdBase> getStripASDOut(TGCIdBase tgcIdBase,
						unsigned int sectorID,
						int wireSLBId,
						int block,
						int phi,
						int wireOffset,
						int stripOffset=DummyOffset) const;
      
      void getWireInfo(double& eta, double& phi,
		       Identifier & wireId,
		       std::unique_ptr<TGCIdBase> w_asdout,
		       EdgeType edge=NonEdge) const;
      
      void getStripInfo(double& eta, double& phi,
			Identifier & stripId,
			std::unique_ptr<TGCIdBase> s_asdout,
			EdgeType edge=NonEdge) const;
      
      
      BooleanProperty m_useRun3Config{this,"UseRun3Config",false,"use Run 3 config"};
      BooleanProperty m_patchForRoIWord{this,"PatchForRoIWord",false,"apply a patch to RoI word"};
      BooleanProperty m_useConditionData{this,"UseConditionData",true,"use condition data"};
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey","MuonDetectorManager","Key of input MuonDetectorManager condition data"};
      const MuonGM::MuonDetectorManager* m_muonMgr{nullptr};
      const ITGCcablingSvc* m_cabling{nullptr};
      bool m_isAtlas{true};
      
    }; // end of TrigT1TGCRecRoiTool
} // namespace LVL1


#endif
