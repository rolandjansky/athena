/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSALGS_TRTCONDWRITE_H
#define TRTCONDITIONSALGS_TRTCONDWRITE_H

/** @file TRTCondWrite.h
 * @brief Algorithm to read/write TRT Conditions objects in
 * the TRTCond namespace from/to text file or StoreGate.
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
   write/read constants in TRTCond namespace to/from text
**/ 

class TRTCondWrite:public AthAlgorithm {
public:
  typedef TRTCond::RtRelationMultChanContainer RtRelationContainer ;
  typedef TRTCond::StrawT0MultChanContainer StrawT0Container ;


  /** constructor **/
  TRTCondWrite(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor **/
  ~TRTCondWrite(void);

  virtual StatusCode  initialize(void);    
  virtual StatusCode  execute(void);
  virtual StatusCode  finalize(void);

 private:
  ServiceHandle<ITRT_CalDbSvc> m_caldbtool;  //<! db tool

  const TRT_ID* m_trtid;                //!< trt id helper
  bool m_setup;                         //!< true at first event
  std::string m_par_calfile;            //!< calibration text file specified in jobOptions
  std::string m_par_caloutputfile;

};
 
#endif // TRTCONDITIONSALGS_TRTCONDWRITE_H

