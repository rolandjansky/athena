/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARDATHENAPOOL_XAODAFPTRACKAUXCONTAINERCNV_V1_H
#define XAODFORWARDATHENAPOOL_XAODAFPTRACKAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODForward/versions/AFPTrackAuxContainer_v1.h"
#include "xAODForward/AFPTrackAuxContainer.h"

/// Converter class used for reading xAOD::AFPTrackAuxContainer_v1
///
/// This class implements the meat of the v1->vX conversion for xAOD::AFPTrack.
/// It translates the payload from the v1 object to the latest type.
/// The converter is implemented in a separate class in order to make it
/// visible to the BS reading code as well.
///
/// The implementation is based on xAODTauJetAuxContainerCnv_v1 by
/// Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// @author Grzegorz Gach <grzegorz.gach@cern.ch>
class xAODAFPTrackAuxContainerCnv_v1 :
  public T_AthenaPoolTPCnvBase< xAOD::AFPTrackAuxContainer,
				xAOD::AFPTrackAuxContainer_v1 > {

public:
  xAODAFPTrackAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::AFPTrackAuxContainer_v1* oldObj,
                             xAOD::AFPTrackAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::AFPTrackAuxContainer*,
                             xAOD::AFPTrackAuxContainer_v1*,
                             MsgStream& log );

}; // class AFPTrackAuxContainer_v1

#endif //  XAODFORWARDATHENAPOOL_XAODAFPTRACKAUXCONTAINERCNV_V1_H
