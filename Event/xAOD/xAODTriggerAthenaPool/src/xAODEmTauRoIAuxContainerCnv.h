/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EmTauRoIAuxContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef EmTauRoIAuxContainerCnv_H
#define EmTauRoIAuxContainerCnv_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigger/EmTauRoIAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::EmTauRoIAuxContainer,
                               xAOD::EmTauRoIAuxContainer >
   xAODEmTauRoIAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::EmTauRoIAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::EmTauRoIContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 * @author Alan Watson <Alan.Watson@CERN.CH>
 *
 * $Revision$
 * $Date$
 */
class xAODEmTauRoIAuxContainerCnv :
   public xAODEmTauRoIAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODEmTauRoIAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODEmTauRoIAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::EmTauRoIAuxContainer*
   createPersistent( xAOD::EmTauRoIAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::EmTauRoIAuxContainer* createTransient();

}; // class xAODEmTauRoIAuxContainerCnv


#endif

