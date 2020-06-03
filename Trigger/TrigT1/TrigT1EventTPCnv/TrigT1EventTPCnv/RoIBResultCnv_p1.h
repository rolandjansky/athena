// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_ROIBRESULTCNV_P1_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_ROIBRESULTCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TrigT1 inlcude(s):
#include "TrigT1Result/RoIBResult.h"

// Local include(s):
#include "TrigT1EventTPCnv/RoIBResult_p1.h"

/**
 *   @short T/P converter for RoIBResult_p1
 *
 *          This converter is used to transfer the data between the
 *          transient ROIB::RoIBResult and persistent RoIBResult_p1 objects.
 *
 * @version $Revision: 1.1.1.1 $
 *    @date $Date: 2007-10-12 14:21:02 $
 *  @author Attila Krasznahorkay Jr.
 */
class RoIBResultCnv_p1 : public T_AthenaPoolTPCnvBase< ROIB::RoIBResult, RoIBResult_p1 > {

public:
   RoIBResultCnv_p1();

   virtual void persToTrans( const RoIBResult_p1* persObj, ROIB::RoIBResult* transObj,
                             MsgStream& log );
   virtual void transToPers( const ROIB::RoIBResult* transObj, RoIBResult_p1* persObj,
                             MsgStream& log );

}; // class RoIBResultCnv_p1

#endif // TRIGT1EVENTTPCNV_ROIBRESULTCNV_P1_H
