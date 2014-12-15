/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JetRoIContainerCnv.h
 */

#ifndef JetRoIContainerCnv_H
#define JetRoIContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODTrigger/JetRoIContainer.h"

typedef T_AthenaPoolCustomCnv< xAOD::JetRoIContainer,
                               xAOD::JetRoIContainer >
   xAODJetRoIContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetRoIContainer class
 *
 *         Simple converter class making the xAOD::JetRoIContainer
 *         class known to POOL.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 * @author Alan Watson <Alan.Watson@CERN.CH>
 *
 */
class xAODJetRoIContainerCnv : public xAODJetRoIContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJetRoIContainerCnv >;

public:
   /// Converter constructor
   xAODJetRoIContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JetRoIContainer*
   createPersistent( xAOD::JetRoIContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::JetRoIContainer* createTransient();

private:
   /// Function preparing a vertex object for persistence
   void toPersistent( xAOD::JetRoI* cluster ) const;

}; // class xAODJetRoIContainerCnv


#endif

