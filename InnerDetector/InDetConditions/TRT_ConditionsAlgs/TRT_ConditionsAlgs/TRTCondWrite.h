/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSALGS_TRTCONDWRITE_H
#define TRTCONDITIONSALGS_TRTCONDWRITE_H

/** @file TRTCondWrite.h
 * @brief Algorithm to read/write TRT Conditions objects in
 * the TRTCond namespace from/to text file or StoreGate Conditions Store.
 * This should replace all TRT Conditions CALL-BACKs in tools and services
 * @author Peter Hansen <phansen@nbi.dk>
 **/

//
#include <vector>
#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/ThreadLocalContext.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"

#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"
#include "GaudiKernel/DataObjID.h"


class TRT_ID;

/** @class TRTCondWrite
   write constants in TRTCond namespace to persistency
**/ 

class TRTCondWrite:public AthReentrantAlgorithm {
public:
  typedef TRTCond::RtRelationMultChanContainer RtRelationContainer ;
  typedef TRTCond::StrawT0MultChanContainer StrawT0Container ;


  /** constructor **/
  TRTCondWrite(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor **/
  ~TRTCondWrite(void);

  virtual StatusCode  initialize(void) override;    
  virtual StatusCode  execute_r(const EventContext& ctx) const override;
  virtual StatusCode  finalize(void) override;
  // IOV method
  EventIDBase runlbl (int run, int lbl) const;

 private:
  ServiceHandle<ITRT_CalDbSvc> m_caldbtool;  //<! db tool

  const TRT_ID* m_trtid;                //!< trt id helper
  bool m_setup;                         //!< true at first event
  std::string par_calfile;              //!< calibration text file specified in jobOptions
  std::string par_caloutputfile;


  ServiceHandle<ICondSvc> m_condSvc;

  //  ReadHandle  keys
  SG::ReadCondHandleKey<RtRelationContainer> m_rtReadKey{this,"RtReadKeyName","in","r-t relation in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_errReadKey{this,"ErrorReadKeyName","in","error on r in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_slopeReadKey{this,"SlopeReadKeyName","in","slope of error in-key"};
  SG::ReadCondHandleKey<StrawT0Container> m_t0ReadKey{this,"T0ReadKeyName","in","t0 in-key"};

  //  WriteHandle keys
  SG::WriteCondHandleKey<RtRelationContainer> m_rtWriteKey{this,"RtWriteKeyName","out","r-t relation out-key"};
  SG::WriteCondHandleKey<RtRelationContainer> m_errWriteKey{this,"ErrorWriteKeyName","out","error on r out-key"};
  SG::WriteCondHandleKey<RtRelationContainer> m_slopeWriteKey{this,"SlopeWriteKeyName","out","slope of error out-key"};
  SG::WriteCondHandleKey<StrawT0Container> m_t0WriteKey{this,"T0WriteKeyName","in","t0 out-key"};
  // IOV
  int m_run1;
  int m_lbl1;
  int m_run2;
  int m_lbl2;


};
inline 
EventIDBase TRTCondWrite::runlbl (int run, int lbl) const
{
  return EventIDBase (run,                    // run
		      EventIDBase::UNDEFNUM,  // event
		      EventIDBase::UNDEFNUM,  // timestamp
		      EventIDBase::UNDEFNUM,  // timestamp ns
		      lbl);                   // lumiblock
} 
#endif // TRTCONDITIONSALGS_TRTCONDWRITE_H

