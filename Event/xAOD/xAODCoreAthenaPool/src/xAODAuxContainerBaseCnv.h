// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODAuxContainerBaseCnv.h 635273 2014-12-11 13:58:24Z krasznaa $
#ifndef XAODCOREATHENAPOOL_XAODAUXCONTAINERBASECNV_H
#define XAODCOREATHENAPOOL_XAODAUXCONTAINERBASECNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::AuxContainerBase,
                               xAOD::AuxContainerBase >
   xAODAuxContainerBaseCnvBase;

/**
 * @short POOL converter for the xAOD::AuxContainerBase class
 *
 * We need a custom converter for this class, since it's possible to apply
 * thinning on containers that hold their payload in such a generic store
 * object. This happens especially when applying thinning and slimming on the
 * same container in derivation framework jobs.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 635273 $
 * $Date: 2014-12-11 14:58:24 +0100 (Thu, 11 Dec 2014) $
 */
class xAODAuxContainerBaseCnv : public xAODAuxContainerBaseCnvBase {

   /// Declare the factory as this class's friend
   friend class CnvFactory< xAODAuxContainerBaseCnv >;

protected:
   /// Converter constructor
   xAODAuxContainerBaseCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::AuxContainerBase*
   createPersistent( xAOD::AuxContainerBase* trans );
   /// Function reading in the object from the input file
   virtual xAOD::AuxContainerBase* createTransient();

}; // class xAODAuxContainerBaseCnv

#endif // XAODCOREATHENAPOOL_XAODAUXCONTAINERBASECNV_H
