/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   MdtCalibDbAlg reads raw condition data and writes derived condition data to the condition store
*/

#ifndef MDTCALIBDBCOOLSTRTOOL_MDTCALIBDBALG_H
#define MDTCALIBDBCOOLSTRTOOL_MDTCALIBDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MdtCalibInterfaces/IMdtCalibDBTool.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtRtRelationCollection.h"
#include "MdtCalibData/MdtCorFuncSetCollection.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MdtCalibData/RtResolutionLookUp.h"
#include "MuonCalibMath/SamplePoint.h"

#include "zlib.h"

#include <string>
#include <vector>

namespace coral	{
  class Blob;
}

class MdtCalibDbAlg: public AthAlgorithm {

 public:

  MdtCalibDbAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MdtCalibDbAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  
 private:
  
  ServiceHandle<ICondSvc> m_condSvc;
  MuonCalib::MdtTubeCalibContainer* buildMdtTubeCalibContainer(const Identifier &id, const MuonGM::MuonDetectorManager* muDetMgr);
  
  StatusCode loadRt(const MuonGM::MuonDetectorManager* muDetMgr);
  StatusCode defaultRt(std::unique_ptr<MdtRtRelationCollection>& writeCdoRt);
  StatusCode loadTube(const MuonGM::MuonDetectorManager* muDetMgr);
  StatusCode defaultT0s(std::unique_ptr<MdtTubeCalibContainerCollection>& writeCdoTube, const MuonGM::MuonDetectorManager* muDetMgr);

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 
  ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool {this, "IdToFixedIdTool", "MuonCalib::IdToFixedIdTool"};
  ServiceHandle<MdtCalibrationRegionSvc> m_regionSvc {this, "MdtCalibrationRegionSvc", "MdtCalibrationRegionSvc"};

  std::string m_rtFolder;
  std::string m_tubeFolder;

  //like MdtCalibrationDbSvc
  //for corData in loadRt
  bool m_create_b_field_function;
  bool m_createWireSagFunction;
  bool m_createSlewingFunction;
  void initialize_B_correction(MuonCalib::MdtCorFuncSet *funcSet, const MuonCalib::MdtRtRelation *rt);
  void initializeSagCorrection(MuonCalib::MdtCorFuncSet *funcSet);

  //if m_TimeSlewingCorrection is set to true then it is assumed that the
  //time slewing correction is applied. If false not. If this flag does
  //not match the bit in the creation parameters, the rt-relation and t0
  //will be corrected.
  //NOTE: This was a preliminary solution for 17.2. In principle each
  //MdtDriftCircleOnTrackCreator could decide individually if it wants to
  //have TS-correction. In the default reco-jobs however, this is
  //configured by one muonRecFlag, that will be used to set this job-option.
  
  bool   m_TimeSlewingCorrection;
  bool   m_UseMLRt;
  std::vector<float> m_MeanCorrectionVsR;
  float  m_TsCorrectionT0;
  double m_defaultT0;
  double m_t0Shift;
  double m_t0Spread;
  double m_rtShift;
  double m_rtScale;
  double m_prop_beta;

  ServiceHandle<IAthRNGSvc> m_AthRNGSvc;
  std::string m_randomStream;
  ATHRNG::RNGWrapper* m_RNGWrapper;

  StringArrayProperty m_RTfileNames; //temporary!!!

  //decompression buffer and length of buffer
  uLongf m_buffer_length;
  std::unique_ptr<Bytef[]> m_decompression_buffer;
  
  //wrapper function for the zlib uncompress, 
  //that automatically creates or increases the buffer if needed.    
  inline bool uncompressInMyBuffer(const coral::Blob &blob);
  inline MuonCalib::RtResolutionLookUp* getRtResolutionInterpolation(const std::vector<MuonCalib::SamplePoint> &sample_points);
  inline StatusCode extractString(std::string& input, std::string& output, std::string separator);  

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyRt;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyTube;
  SG::WriteCondHandleKey<MdtRtRelationCollection> m_writeKeyRt{this,"MdtRtRelationCollection","MdtRtRelationCollection","MDT RT relations"};
  SG::WriteCondHandleKey<MdtTubeCalibContainerCollection> m_writeKeyTube{this,"MdtTubeCalibContainerCollection","MdtTubeCalibContainerCollection","MDT tube calib"};
  SG::WriteCondHandleKey<MdtCorFuncSetCollection> m_writeKeyCor{this,"MdtCorFuncSetCollection","MdtCorFuncSetCollection","MDT cor Funcs"};

  unsigned int m_regionIdThreshold;
   
};

#endif
