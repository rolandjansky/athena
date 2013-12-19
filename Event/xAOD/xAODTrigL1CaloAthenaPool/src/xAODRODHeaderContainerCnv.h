/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/RODHeaderContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::RODHeaderContainer,
                               xAOD::RODHeaderContainer >
   xAODRODHeaderContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::RODHeaderContainer class
 *
 *         Simple converter class making the xAOD::RODHeaderContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODRODHeaderContainerCnv : public xAODRODHeaderContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODRODHeaderContainerCnv >;

public:
   /// Converter constructor
   xAODRODHeaderContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::RODHeaderContainer*
   createPersistent( xAOD::RODHeaderContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::RODHeaderContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::RODHeader* cluster ) const;

}; // class xAODRODHeaderContainerCnv

#endif 
