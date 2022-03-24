/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRAWDATAMONITORING_TGCRAWDATAMONITORTOOL_H
#define TGCRAWDATAMONITORING_TGCRAWDATAMONITORTOOL_H
#include "ITgcRawDataMonitorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class TgcRawDataMonitorTool: public extends<AthAlgTool, ITgcRawDataMonitorTool> {
 public:
  TgcRawDataMonitorTool(const std::string& type, 
			const std::string& name,
			const IInterface* parent);
  virtual ~TgcRawDataMonitorTool();
  virtual StatusCode initialize() override;

  virtual std::set<std::string> getPassedChambers(const Amg::Vector3D& extVec) const override;
  virtual bool getMapIndex(const TGC::TgcHit& tgcHit,
			   int& etamapindex, int& phimapindex, int& globalphimapindex ) const override;
  virtual bool getMapIndex(const TGC::TgcChamber& tgcCham, int ilay,
			   int& etamapindex, int& phimapindex, int& globalphimapindex ) const override;
 private:

  DoubleProperty m_fiduciFrame{this,"FiducialFrame",-200.0,"Fidicual Frame size"};
  
  DoubleProperty m_M1Mod1Pl1L1_offsetZ{this,"M1Mod1Pl1L1_offsetZ",13281.3,"Offset in Z of M1Mod1Pl1L1"};
  DoubleProperty m_M1Mod2Pl2L3_offsetZ{this,"M1Mod2Pl2L3_offsetZ",13591.7,"Offset in Z of M1Mod2Pl2L3"};
  
  DoubleProperty m_M2Mod1Pl1L1_offsetZ{this,"M2Mod1Pl1L1_offsetZ",14610.1,"Offset in Z of M2Mod1Pl1L1"};
  DoubleProperty m_M2Mod2Pl2L2_offsetZ{this,"M2Mod2Pl2L2_offsetZ",14846.3,"Offset in Z of M2Mod2Pl2L2"};

  DoubleProperty m_M3Mod1Pl1L1_offsetZ{this,"M3Mod1Pl1L1_offsetZ",15030.1,"Offset in Z of M3Mod1Pl1L1"};
  DoubleProperty m_M3Mod2Pl2L2_offsetZ{this,"M3Mod2Pl2L2_offsetZ",15266.3,"Offset in Z of M3Mod2Pl2L2"};
  
  DoubleProperty m_EIMod1Pl1L1_offsetZ{this,"EIMod1Pl1L1_offsetZ",7318.3, "Offset in Z of EIMod1Pl1L1"};
  DoubleProperty m_EIMod2Pl1L2_offsetZ{this,"EIMod2Pl1L2_offsetZ",7411.1, "Offset in Z of EIMod2Pl1L2"};
  
  DoubleProperty m_FIMod1Pl1L1_offsetZ{this,"FIMod1Pl1L1_offsetZ",6940.0, "Offset in Z of FIMod1Pl1L1"};
  DoubleProperty m_FIMod2Pl1L2_offsetZ{this,"FIMod2Pl1L2_offsetZ",7016.4, "Offset in Z of FIMod2Pl1L2"};

  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey","MuonDetectorManager","Key of input MuonDetectorManager condition data"}; 

};


#endif
