/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JetRoIAuxContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef JetRoIAuxContainerCnv_H
#define JetRoIAuxContainerCnv_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigger/JetRoIAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::JetRoIAuxContainer,
                               xAOD::JetRoIAuxContainer >
   xAODJetRoIAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetRoIAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::JetRoIContainer
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
class xAODJetRoIAuxContainerCnv :
   public xAODJetRoIAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJetRoIAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODJetRoIAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JetRoIAuxContainer*
   createPersistent( xAOD::JetRoIAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::JetRoIAuxContainer* createTransient();

}; // class xAODJetRoIAuxContainerCnv


#endif

