/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSALGS_TRTCONDWRITE_H
#define TRTCONDITIONSALGS_TRTCONDWRITE_H

/** @file TRTCondWrite.h
 * @brief Algorithm to create TRT Calibration objects in
 * the TRTCond namespace and write them in the Conditions DB.
 * @author Peter Hansen <phansen@nbi.dk>
 **/

//
#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"

class TRT_ID;

/** @class TRTCondWrite
   write constants in TRTCond namespace to persistency
**/ 

class TRTCondWrite:public AthAlgorithm {
public:
  /** constructor **/
  TRTCondWrite(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor **/
  ~TRTCondWrite(void);

  StatusCode  initialize(void);    
  StatusCode  execute(void);
  StatusCode  finalize(void);

 private:
  ServiceHandle<ITRT_CalDbSvc> m_caldbtool;  //<! db tool
  const TRT_ID* m_trtid;                //!< trt id helper
  bool m_setup;                         //!< true at first event
  std::string par_calfile;   //!< text file specified in jobOptions

};
#endif // TRTCONDITIONSALGS_TRTCONDWRITE_H

