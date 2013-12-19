/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMROICONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMROICONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMRoIContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CPMRoIContainer,
                               xAOD::CPMRoIContainer >
   xAODCPMRoIContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CPMRoIContainer class
 *
 *         Simple converter class making the xAOD::CPMRoIContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODCPMRoIContainerCnv : public xAODCPMRoIContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCPMRoIContainerCnv >;

public:
   /// Converter constructor
   xAODCPMRoIContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CPMRoIContainer*
   createPersistent( xAOD::CPMRoIContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CPMRoIContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::CPMRoI* cluster ) const;

}; // class xAODCPMRoIContainerCnv

#endif 
