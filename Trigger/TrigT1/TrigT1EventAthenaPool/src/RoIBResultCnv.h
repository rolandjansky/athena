// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1EVENTATHENAPOOL_ROIBRESULTCNV_H
#define TRIGT1EVENTATHENAPOOL_ROIBRESULTCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TrigT1 include(s):
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1EventTPCnv/RoIBResult_p1.h"

// Define the latest persistent representation of ROIB::RoIBResult:
typedef RoIBResult_p1 RoIBResult_PERS;
typedef T_AthenaPoolCustomCnv< ROIB::RoIBResult, RoIBResult_PERS > RoIBResultCnvBase;

/**
 *   @short POOL converter for ROIB::RoIBResult
 *
 *          Custom POOL converter for the ROIB::RoIBResult object that implements the
 *          T/P separation for the LVL1 object.
 *
 * @version $Revision: 1.3 $
 *    @date $Date: 2007-10-12 14:42:11 $
 *  @author Attila Krasznahorkay Jr.
 */
class RoIBResultCnv : public RoIBResultCnvBase {

   friend class CnvFactory< RoIBResultCnv >;

protected:
   RoIBResultCnv( ISvcLocator* svcLoc ) : RoIBResultCnvBase( svcLoc ) {}

   virtual RoIBResult_PERS*  createPersistent( ROIB::RoIBResult* transObj );
   virtual ROIB::RoIBResult* createTransient();

}; // class RoIBResultCnv

#endif // TRIGT1EVENTATHENAPOOL_ROIBRESULTCNV_H
