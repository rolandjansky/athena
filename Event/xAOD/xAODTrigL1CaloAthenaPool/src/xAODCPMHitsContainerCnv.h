/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMHitsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CPMHitsContainer,
                               xAOD::CPMHitsContainer >
   xAODCPMHitsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CPMHitsContainer class
 *
 *         Simple converter class making the xAOD::CPMHitsContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODCPMHitsContainerCnv : public xAODCPMHitsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCPMHitsContainerCnv >;

public:
   /// Converter constructor
   xAODCPMHitsContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CPMHitsContainer*
   createPersistent( xAOD::CPMHitsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CPMHitsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::CPMHits* cluster ) const;

}; // class xAODCPMHitsContainerCnv

#endif 
