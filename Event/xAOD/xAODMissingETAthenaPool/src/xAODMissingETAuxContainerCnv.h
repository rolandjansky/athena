// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxContainerCnv.h 586838 2014-03-08 15:10:34Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXCONTAINERCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingETAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::MissingETAuxContainer,
                               xAOD::MissingETAuxContainer >
   xAODMissingETAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MissingETAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::MissingETContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Teng Jian Khoo <Teng.Jian.Khoo@cern.ch>
 *
 * $Revision: 586838 $
 * $Date: 2014-03-08 16:10:34 +0100 (Sat, 08 Mar 2014) $
 */
class xAODMissingETAuxContainerCnv :
   public xAODMissingETAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMissingETAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODMissingETAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::MissingETAuxContainer*
   createPersistent( xAOD::MissingETAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::MissingETAuxContainer* createTransient();

}; // class xAODMissingETAuxContainerCnv

#endif
