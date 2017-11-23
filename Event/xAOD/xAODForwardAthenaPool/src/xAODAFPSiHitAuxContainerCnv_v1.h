/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARDATHENAPOOL_XAODAFPSIHITAUXCONTAINERCNV_V1_H
#define XAODFORWARDATHENAPOOL_XAODAFPSIHITAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODForward/versions/AFPSiHitAuxContainer_v1.h"
#include "xAODForward/AFPSiHitAuxContainer.h"

/// Converter class used for reading xAOD::AFPSiHitAuxContainer_v1
///
/// This class implements the meat of the v1->vX conversion for xAOD::AFPSiHit.
/// It translates the payload from the v1 object to the latest type.
/// The converter is implemented in a separate class in order to make it
/// visible to the BS reading code as well.
///
/// The implementation is based on xAODTauJetAuxContainerCnv_v1 by
/// Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// @author Grzegorz Gach <grzegorz.gach@cern.ch>
class xAODAFPSiHitAuxContainerCnv_v1 :
  public T_AthenaPoolTPCnvBase< xAOD::AFPSiHitAuxContainer,
				xAOD::AFPSiHitAuxContainer_v1 > {

public:
  xAODAFPSiHitAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::AFPSiHitAuxContainer_v1* oldObj,
                             xAOD::AFPSiHitAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::AFPSiHitAuxContainer*,
                             xAOD::AFPSiHitAuxContainer_v1*,
                             MsgStream& log );

}; // class AFPSiHitAuxContainer_v1

#endif // XAODFORWARDATHENAPOOL_XAODAFPSIHITAUXCONTAINERCNV_V1_H
