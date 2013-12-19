/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMETSUMSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMETSUMSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JEMEtSumsContainer,
                               xAOD::JEMEtSumsContainer >
   xAODJEMEtSumsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JEMEtSumsContainer class
 *
 *         Simple converter class making the xAOD::JEMEtSums
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODJEMEtSumsContainerCnv : public xAODJEMEtSumsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJEMEtSumsContainerCnv >;

public:
   /// Converter constructor
   xAODJEMEtSumsContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JEMEtSumsContainer*
   createPersistent( xAOD::JEMEtSumsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::JEMEtSumsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::JEMEtSums* cluster ) const;

}; // class xAODJEMEtSumsContainerCnv

#endif 
