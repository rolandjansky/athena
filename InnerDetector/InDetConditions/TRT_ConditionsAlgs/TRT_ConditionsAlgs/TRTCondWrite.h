/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSALGS_TRTCONDWRITE_H
#define TRTCONDITIONSALGS_TRTCONDWRITE_H

/** @file TRTCondWrite.h
 * @brief Algorithm to read/write TRT Conditions objects in
 * the TRTCond namespace from/to text file or StoreGate.
 * @author Peter Hansen <phansen@nbi.dk>
 **/

//
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/DataHandle.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsData/RtRelationMultChanContainer.h"
#include "TRT_ConditionsData/StrawT0MultChanContainer.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

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

  virtual StatusCode  initialize(void) override;    
  virtual StatusCode  execute(void) override;
  virtual StatusCode  finalize(void) override;

  /// create an TRTCond::ExpandedIdentifier from a TRTID identifier
  virtual TRTCond::ExpandedIdentifier trtcondid( const Identifier& id, int level = TRTCond::ExpandedIdentifier::STRAW) const;

  // methods for persistency
  
  /// write calibration constants to flat text file 
  virtual StatusCode writeTextFile_Format1(std::ostream&) const;
  virtual StatusCode writeTextFile_Format2(std::ostream&) const;
  virtual StatusCode writeTextFile_Format3(std::ostream&) const;

  /// read calibration from text file into TDS
  virtual StatusCode checkTextFile(const std::string& file, int& format);
  virtual StatusCode readTextFile(const std::string& file, int& format);
  virtual StatusCode readTextFile_Format1(std::istream&);
  virtual StatusCode readTextFile_Format2(std::istream&);
  virtual StatusCode readTextFile_Format3(std::istream&);

  
  /// access to calibration constant containers
  virtual const RtRelationContainer* getRtContainer() const;
  virtual const RtRelationContainer* getErrContainer() const;
  virtual const RtRelationContainer* getSlopeContainer() const;
  virtual const StrawT0Container* getT0Container() const;


 private:

  std::string m_par_rtcontainerkey;        //"/TRT/Calib/RT"
  std::string m_par_errcontainerkey;       //"/TRT/Calib/errors2d"
  std::string m_par_slopecontainerkey;     //"/TRT/Calib/slopes"
  std::string m_par_t0containerkey;        //"/TRT/Calib/T0"
  bool m_setup;                            //!< true at first event
  std::string m_par_caltextfile;           //!< calibration text file specified in jobOptions
  std::string m_par_caloutputfile;         //set this to "calibout_n,txt", where n=0,1,2,3 refers to the format
  const TRT_ID* m_trtid;                   //!< trt id helper
  std::string m_streamer;                  //"AthenaOutputStreamTool/CondStream1"
  ServiceHandle<StoreGateSvc> m_detstore;
  ServiceHandle<ICondSvc> m_condSvc;

  //  ReadHandle  keys
  SG::ReadCondHandleKey<RtRelationContainer> m_rtReadKey{this,"RtReadKeyName","/TRT/Calib/RT","r-t relation in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_errReadKey{this,"ErrorReadKeyName","/TRT/Calib/errors2d","error on r in-key"};
  SG::ReadCondHandleKey<RtRelationContainer> m_slopeReadKey{this,"SlopeReadKeyName","/TRT/Calib/slopes","slope of error in-key"};
  SG::ReadCondHandleKey<StrawT0Container> m_t0ReadKey{this,"T0ReadKeyName","/TRT/Calib/T0","t0 in-key"};



};
 
#endif // TRTCONDITIONSALGS_TRTCONDWRITE_H

