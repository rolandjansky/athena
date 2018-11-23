// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_MUCTPI_RDOCNV_P1_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_MUCTPI_RDOCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TrigT1 include(s):
#include "TrigT1Result/MuCTPI_RDO.h"

// Local include(s):
#include "TrigT1EventTPCnv/MuCTPI_RDO_p1.h"

/**
 *   @short T/P converter for MuCTPI_RDO_p1
 *
 *          This converter is used to transfer the data between the
 *          transient MuCTPI_RDO and persistent MuCTPI_RDO_p1 objects.
 *
 * @version $Revision: 1.1.1.1 $
 *    @date $Date: 2007-10-12 14:21:02 $
 *  @author Attila Krasznahorkay Jr.
 */
class MuCTPI_RDOCnv_p1 : public T_AthenaPoolTPCnvBase< MuCTPI_RDO, MuCTPI_RDO_p1 > {

public:
   MuCTPI_RDOCnv_p1();

   virtual void persToTrans( const MuCTPI_RDO_p1* persObj, MuCTPI_RDO* transObj,
                             MsgStream& log );
   virtual void transToPers( const MuCTPI_RDO* transObj, MuCTPI_RDO_p1* persObj,
                             MsgStream& log );

}; // class MuCTPI_RDOCnv_p1

#endif // TRIGT1EVENTTPCNV_MUCTPI_RDOCNV_P1_H
