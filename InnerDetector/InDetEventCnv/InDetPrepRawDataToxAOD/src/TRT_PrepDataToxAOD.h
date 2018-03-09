/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_PrepDataToxAOD.h
//   Header file for class TRT_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#ifndef TRT_PREPDATATOXAOD_H
#define TRT_PREPDATATOXAOD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include <string>

class TRT_ID;
class ITRT_CalDbSvc ;
class ITRT_DriftFunctionTool;
class ITRT_StrawSummarySvc;

namespace InDetDD {
  class TRT_DetectorManager;
}

class TRT_PrepDataToxAOD : public AthAlgorithm  {

public:
  enum EGasType {kUnset=-128,kXenon=0,kArgon=1,kKrypton=2, kEmAr=6,kEmKr=7};
  // Constructor with parameters:
  TRT_PrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  // --- Steering and configuration flags
  bool m_useTruthInfo;
  bool m_writeSDOs;

  // --- Configuration keys
  std::string  m_driftcirclecontainer;
  std::string  m_SDOcontainer;
  std::string  m_multiTruth;

  // --- Services and Tools
  ToolHandle< ITRT_DriftFunctionTool >      m_driftFunctionTool ; //!< DriftFunctionTool
  ServiceHandle<ITRT_CalDbSvc>              m_trtcaldbSvc       ;
  ServiceHandle<ITRT_StrawNeighbourSvc>     m_neighbourSvc      ;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawSummarySvc; 
  const TRT_ID *m_TRTHelper;
  const InDetDD::TRT_DetectorManager* m_trtman;

  // ---- Internal members
  bool m_firstEventWarnings;

};


#endif 
