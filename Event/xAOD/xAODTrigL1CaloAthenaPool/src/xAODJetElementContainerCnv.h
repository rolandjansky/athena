/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJETELEMENTCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJETELEMENTCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JetElementContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JetElementContainer,
                               xAOD::JetElementContainer >
   xAODJetElementContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetElementContainer class
 *
 *         Simple converter class making the xAOD::JetElementContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODJetElementContainerCnv : public xAODJetElementContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJetElementContainerCnv >;

public:
   /// Converter constructor
   xAODJetElementContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JetElementContainer*
   createPersistent( xAOD::JetElementContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::JetElementContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::JetElement* cluster ) const;

}; // class xAODJetElementContainerCnv

#endif 
