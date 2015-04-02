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


#include <string>

class TRT_ID;
class ITRT_CalDbSvc ;
class ITRT_DriftFunctionTool;
class ITRT_StrawSummarySvc;



class TRT_PrepDataToxAOD : public AthAlgorithm  {

public:
  // Constructor with parameters:
  TRT_PrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  bool m_useTruthInfo;

  ToolHandle< ITRT_DriftFunctionTool >      m_driftFunctionTool ; //!< DriftFunctionTool
  ServiceHandle<ITRT_CalDbSvc>              m_trtcaldbSvc       ;
  ServiceHandle<ITRT_StrawNeighbourSvc>     m_neighbourSvc      ;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawSummarySvc; 

  const TRT_ID *m_TRTHelper;
  std::string  m_driftcirclecontainer;
  std::string  m_SDOcontainer;
  std::string  m_multiTruth;
};


#endif 
