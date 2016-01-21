/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigEventAthenaPool
 * @Class  : HLTResultCnv
 *
 * @brief  The top-level converter for TrigEventAthenaPool/HLTResult_pX. It calls the
 *         converters for specific persistent data formats (for now only HLTResult_p1).
 *
 * @author Andrew Hamilton <Andrew.Hamilton@cern.ch> - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - U. of California
 *
 * File and Version Information:
 * $Id: HLTResultCnv.h 719129 2016-01-21 17:02:55Z will $
 **********************************************************************************/

#ifndef HLTResultCnv_H
#define HLTResultCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigSteeringEvent/HLTResult.h"
class HLTResultCnv_p1;
namespace HLT {
  class HLTResult_p1;
  class HLTResultCnv_p1;
}

// the latest persistent representation type of HLTResult:
typedef  HLT::HLTResult_p1  HLTResult_PERS;

typedef  T_AthenaPoolCustomCnv<HLT::HLTResult, HLTResult_PERS > HLTResultCnvBase;

class MsgStream;
  
class HLTResultCnv : public HLTResultCnvBase {
  
  friend class CnvFactory< HLTResultCnv >;
  
 protected:

  HLTResultCnv (ISvcLocator* svcloc);

  ~HLTResultCnv();

  virtual HLTResult_PERS* createPersistent (HLT::HLTResult* transCont);

  virtual HLT::HLTResult* createTransient ();

 private:

  HLT::HLTResultCnv_p1* m_TPConverter;

};
  
#endif
