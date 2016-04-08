// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthPileupEventAuxContainerCnv.h 622196 2014-10-16 16:10:41Z krasznaa $
#ifndef XAODTRUTHATHENAPOOL_XAODTRUTHPILEUPEVENTAUXCONTAINERCNV_H
#define XAODTRUTHATHENAPOOL_XAODTRUTHPILEUPEVENTAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTruth/TruthPileupEventAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TruthPileupEventAuxContainer,
                               xAOD::TruthPileupEventAuxContainer >
   xAODTruthPileupEventAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TruthPileupEventAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TruthPileupEventContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 622196 $
 * $Date: 2014-10-16 18:10:41 +0200 (Thu, 16 Oct 2014) $
 */
class xAODTruthPileupEventAuxContainerCnv :
   public xAODTruthPileupEventAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTruthPileupEventAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTruthPileupEventAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TruthPileupEventAuxContainer*
   createPersistent( xAOD::TruthPileupEventAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TruthPileupEventAuxContainer* createTransient();

}; // class xAODTruthPileupEventAuxContainerCnv

#endif // XAODTRUTHATHENAPOOL_XAODTRUTHPILEUPEVENTAUXCONTAINERCNV_H
