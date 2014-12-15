/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EmTauRoIContainerCnv.h
 */

#ifndef EmTauRoIContainerCnv_H
#define EmTauRoIContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODTrigger/EmTauRoIContainer.h"

typedef T_AthenaPoolCustomCnv< xAOD::EmTauRoIContainer,
                               xAOD::EmTauRoIContainer >
   xAODEmTauRoIContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::EmTauRoIContainer class
 *
 *         Simple converter class making the xAOD::EmTauRoIContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 * @author Alan Watson <Alan.Watson@CERN.CH>
 *
 */
class xAODEmTauRoIContainerCnv : public xAODEmTauRoIContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODEmTauRoIContainerCnv >;

public:
   /// Converter constructor
   xAODEmTauRoIContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::EmTauRoIContainer*
   createPersistent( xAOD::EmTauRoIContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::EmTauRoIContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::EmTauRoI* cluster ) const;

}; // class xAODEmTauRoIContainerCnv


#endif

