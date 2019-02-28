/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
   MdtCalibDbAlg reads raw condition data and writes derived condition data to the condition store
*/

#ifndef MDTCALIBDBCOOLSTRTOOL_MDTCALIBDBALG_H
#define MDTCALIBDBCOOLSTRTOOL_MDTCALIBDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//from MdtCalibDbCoolStrTool.h
//removing obsolete ones

#include "AthenaBaseComps/AthAlgTool.h"
#include "MdtCalibInterfaces/IMdtCalibDBTool.h"
#include "MdtCalibData/MdtTubeCalibContainerCollection.h"
#include "MdtCalibData/MdtRtRelationCollection.h"
#include "MdtCalibData/MdtCorFuncSetCollection.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

//c - c++
#include "zlib.h"
#include "vector"

class MdtCalibrationRegionSvc;
class Identifier; 
class MdtIdHelper;
class IIOVDbSvc;
class MsgStream;
class IAtRndmGenSvc;

namespace MuonGM{
  class MuonDetectorManager;
}

namespace coral	{
  class Blob;
}

//class RtResolutionLookUp;
//class SamplePoint;

#include "MdtCalibData/RtResolutionLookUp.h"
#include "MuonCalibMath/SamplePoint.h"

class MdtCalibDbAlg: public AthAlgorithm {

 public:

  MdtCalibDbAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MdtCalibDbAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private:
  
  ServiceHandle<ICondSvc> m_condSvc;

  //like MdtCalibDbCoolStrTool
  //removing obsolete ones

  MuonCalib::MdtTubeCalibContainer* buildMdtTubeCalibContainer(const Identifier &id);
  
  StatusCode loadRt();
  StatusCode defaultRt(std::unique_ptr<MdtRtRelationCollection>& writeCdoRt);
  StatusCode loadTube();
  StatusCode defaultT0s(std::unique_ptr<MdtTubeCalibContainerCollection>& writeCdoTube);

  const MdtIdHelper *m_mdtIdHelper;
  const MuonGM::MuonDetectorManager *m_detMgr;
  ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;
  ServiceHandle<MdtCalibrationRegionSvc> m_regionSvc;

  std::string      m_rtFolder;
  std::string      m_tubeFolder;

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
  const double m_speed_of_light;
  ServiceHandle<IAtRndmGenSvc> m_AtRndmGenSvc;
  CLHEP::HepRandomEngine *p_engine;

  StringArrayProperty m_RTfileNames; //temporary!!!

  //decompression buffer and length of buffer
  uLongf m_buffer_length;
  Bytef *m_decompression_buffer;
  
  //wrapper function for the zlib uncompress, 
  //that automatically creates or increases the buffer if needed.    
  inline bool uncompressInMyBuffer(const coral::Blob &blob);
  inline MuonCalib::RtResolutionLookUp* getRtResolutionInterpolation(const std::vector<MuonCalib::SamplePoint> &sample_points);
  inline StatusCode extractString(std::string& input, std::string& output, std::string separator);  

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyRt;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyTube;
  SG::WriteCondHandleKey<MdtRtRelationCollection> m_writeKeyRt;
  SG::WriteCondHandleKey<MdtTubeCalibContainerCollection> m_writeKeyTube;
  SG::WriteCondHandleKey<MdtCorFuncSetCollection> m_writeKeyCor;
   
};

#endif
