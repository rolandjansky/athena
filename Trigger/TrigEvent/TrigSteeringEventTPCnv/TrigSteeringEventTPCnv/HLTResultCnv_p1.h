/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigSteeringEventTPCnv
 * @Class  : HLTResultCnv_p1
 *
 * @brief  The converter for the HLTResult_p1 persistent format.
 *
 * @author Andrew Hamilton <Andrew.Hamilton@cern.ch> - U. of Geneva, Switzerland
 * @author Francesca Bucci <f.bucci@cern.ch>         - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - U. of California
 *
 * File and Version Information:
 * $Id: HLTResultCnv_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/

#ifndef TrigSteeringEventTPCnv_HLTResultCnv_H
#define TrigSteeringEventTPCnv_HLTResultCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

namespace HLT {
  class HLTResult;
  class HLTResult_p1;
}

namespace HLT {

  class HLTResultCnv_p1  : public T_AthenaPoolTPCnvBase<HLTResult, HLTResult_p1>  {
    
  public:
    HLTResultCnv_p1() {}
    void persToTrans(const HLTResult_p1* persObj,  HLTResult* transObj,   MsgStream& log);
    void transToPers(const HLTResult*    transObj, HLTResult_p1* persObj, MsgStream& log);
  };

}

#endif
