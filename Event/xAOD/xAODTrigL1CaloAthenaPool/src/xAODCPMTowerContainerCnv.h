/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMTOWERCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMTOWERCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMTowerContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CPMTowerContainer,
                               xAOD::CPMTowerContainer >
   xAODCPMTowerContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CPMTowerContainer class
 *
 *         Simple converter class making the xAOD::CPMTowerContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODCPMTowerContainerCnv : public xAODCPMTowerContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCPMTowerContainerCnv >;

public:
   /// Converter constructor
   xAODCPMTowerContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CPMTowerContainer*
   createPersistent( xAOD::CPMTowerContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CPMTowerContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::CPMTower* cluster ) const;

}; // class xAODCPMTowerContainerCnv

#endif 
