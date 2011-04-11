// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1EVENTATHENAPOOL_CTP_RDOCNV_H
#define TRIGT1EVENTATHENAPOOL_CTP_RDOCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TrigT1 include(s):
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1EventTPCnv/CTP_RDO_p1.h"
#include "TrigT1EventTPCnv/CTP_RDO_p2.h"

// Define the latest persistent representation of CTP_RDO:
typedef CTP_RDO_p2 CTP_RDO_PERS;
typedef T_AthenaPoolCustomCnv< CTP_RDO, CTP_RDO_PERS > CTP_RDOCnvBase;

/**
 *   @short POOL converter for CTP_RDO
 *
 *          Custom POOL converter for the CTP_RDO object that implements the
 *          T/P separation for the LVL1 object.
 *
 * @version $Revision: 1.2 $
 *    @date $Date: 2008-02-28 13:41:01 $
 *  @author Attila Krasznahorkay Jr., Wolfgang Ehrenfeld
 */
class CTP_RDOCnv : public CTP_RDOCnvBase {

   friend class CnvFactory< CTP_RDOCnv >;

protected:
   CTP_RDOCnv( ISvcLocator* svcLoc ) : CTP_RDOCnvBase( svcLoc ) {}

   virtual CTP_RDO_PERS* createPersistent( CTP_RDO* transObj );
   virtual CTP_RDO*      createTransient();

}; // class CTP_RDOCnv

#endif // TRIGT1EVENTATHENAPOOL_CTP_RDOCNV_H
