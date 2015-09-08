// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxAssociationMapCnv.h 692089 2015-08-30 14:36:16Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXASSOCIATIONMAPCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXASSOCIATIONMAPCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingETAuxAssociationMap.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::MissingETAuxAssociationMap,
                               xAOD::MissingETAuxAssociationMap >
   xAODMissingETAuxAssociationMapCnvBase;

/**
 *  @short POOL converter for the xAOD::MissingETAuxAssociationMap class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::MissingETAssociationMap
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Teng Jian Khoo <Teng.Jian.Khoo@cern.ch>
 *
 * $Revision: 692089 $
 * $Date: 2015-08-30 16:36:16 +0200 (Sun, 30 Aug 2015) $
 */
class xAODMissingETAuxAssociationMapCnv :
   public xAODMissingETAuxAssociationMapCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMissingETAuxAssociationMapCnv >;

protected:
   /// Converter constructor
   xAODMissingETAuxAssociationMapCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::MissingETAuxAssociationMap*
   createPersistent( xAOD::MissingETAuxAssociationMap* trans );
   /// Function reading in the object from the input file
   virtual xAOD::MissingETAuxAssociationMap* createTransient();

}; // class xAODMissingETAuxAssociationMapCnv

#endif
