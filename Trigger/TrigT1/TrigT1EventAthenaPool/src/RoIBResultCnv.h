// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1EVENTATHENAPOOL_ROIBRESULTCNV_H
#define TRIGT1EVENTATHENAPOOL_ROIBRESULTCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TrigT1 include(s):
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1EventTPCnv/RoIBResult_p1.h"
#include "TrigT1EventTPCnv/RoIBResultCnv_p1.h"


// Define the latest persistent representation of ROIB::RoIBResult:
typedef RoIBResult_p1 RoIBResult_PERS;
typedef T_AthenaPoolCustomCnv< ROIB::RoIBResult, RoIBResult_PERS > RoIBResultCnvBase;

/**
 *   @short POOL converter for ROIB::RoIBResult
 *
 *          Custom POOL converter for the ROIB::RoIBResult object that implements the
 *          T/P separation for the LVL1 object.
 *
 *  @author Attila Krasznahorkay Jr.
 */
class RoIBResultCnv : public RoIBResultCnvBase {

   friend class CnvFactory< RoIBResultCnv >;

protected:
public:
   RoIBResultCnv( ISvcLocator* svcLoc ) : RoIBResultCnvBase( svcLoc ) {}
protected:

   virtual RoIBResult_PERS*  createPersistent( ROIB::RoIBResult* transObj );
   virtual ROIB::RoIBResult* createTransient();
private:
   RoIBResultCnv_p1 m_converter;

}; // class RoIBResultCnv

#endif // TRIGT1EVENTATHENAPOOL_ROIBRESULTCNV_H
