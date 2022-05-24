// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1EVENTATHENAPOOL_MUCTPI_RDOCNV_H
#define TRIGT1EVENTATHENAPOOL_MUCTPI_RDOCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TrigT1 include(s):
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1EventTPCnv/MuCTPI_RDO_p1.h"
#include "TrigT1EventTPCnv/MuCTPI_RDOCnv_p1.h"

// Define the latest persistent representation of MuCTPI_RDO:
typedef MuCTPI_RDO_p1 MuCTPI_RDO_PERS;
typedef T_AthenaPoolCustomCnv< MuCTPI_RDO, MuCTPI_RDO_PERS > MuCTPI_RDOCnvBase;

/**
 *   @short POOL converter for MuCTPI_RDO
 *
 *          Custom POOL converter for the MuCTPI_RDO object that implements the
 *          T/P separation for the LVL1 object.
 *
 *  @author Attila Krasznahorkay Jr.
 */
class MuCTPI_RDOCnv : public MuCTPI_RDOCnvBase {

   friend class CnvFactory< MuCTPI_RDOCnv >;

protected:
public:
   MuCTPI_RDOCnv( ISvcLocator* svcLoc ) : MuCTPI_RDOCnvBase( svcLoc ) {}
protected:

   virtual MuCTPI_RDO_PERS* createPersistent( MuCTPI_RDO* transObj );
   virtual MuCTPI_RDO*      createTransient();
private:
   MuCTPI_RDOCnv_p1 m_converter;

}; // class MuCTPI_RDOCnv

#endif // TRIGT1EVENTATHENAPOOL_MUCTPI_RDOCNV_H
