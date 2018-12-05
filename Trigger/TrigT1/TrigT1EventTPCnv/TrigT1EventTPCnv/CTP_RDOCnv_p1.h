// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_CTP_RDOCNV_P1_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_CTP_RDOCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TrigT1 inlcude(s):
#include "TrigT1Result/CTP_RDO.h"

// Local include(s):
#include "TrigT1EventTPCnv/CTP_RDO_p1.h"

/**
 *   @short T/P converter for CTP_RDO_p1
 *
 *          This converter is used to transfer the data between the
 *          transient CTP_RDO and persistent CTP_RDO_p1 objects.
 *
 * @version $Revision: 1.1.1.1 $
 *    @date $Date: 2007-10-12 14:21:02 $
 *  @author Attila Krasznahorkay Jr.
 */
class CTP_RDOCnv_p1 : public T_AthenaPoolTPCnvBase< CTP_RDO, CTP_RDO_p1 > {

public:
   CTP_RDOCnv_p1();

   virtual void persToTrans( const CTP_RDO_p1* persObj, CTP_RDO* transObj,
                             MsgStream& log );
   virtual void transToPers( const CTP_RDO* transObj, CTP_RDO_p1* persObj,
                             MsgStream& log );

}; // class CTP_RDOCnv_p1

#endif // TRIGT1EVENTTPCNV_CTP_RDOCNV_P1_H
