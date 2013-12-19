/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMCPHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMCPHITSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMCPHitsContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::CMMCPHitsContainer,
                               xAOD::CMMCPHitsContainer >
   xAODCMMCPHitsContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CMMCPHitsContainer class
 *
 *         Simple converter class making the xAOD::CMMCPHitsContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 */
class xAODCMMCPHitsContainerCnv : public xAODCMMCPHitsContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCMMCPHitsContainerCnv >;

public:
   /// Converter constructor
   xAODCMMCPHitsContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CMMCPHitsContainer*
   createPersistent( xAOD::CMMCPHitsContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::CMMCPHitsContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::CMMCPHits* cluster ) const;

}; // class xAODCMMCPHitsContainerCnv

#endif 
