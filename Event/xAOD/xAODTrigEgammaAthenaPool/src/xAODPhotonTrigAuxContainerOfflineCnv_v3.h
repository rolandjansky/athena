// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonTrigAuxContainerOfflineCnv_v3.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINEROFFLINECNV_V3_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINEROFFLINECNV_V3_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v3.h"
#include "xAODTrigEgamma/PhotonTrigAuxContainer.h"

/// Converter class used to read xAOD::PhotonAuxContainer_v3
///
/// Since we switched from using the offline xAOD::PhotonAuxContainer
/// type to using a trigger specific xAOD::PhotonTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 705790 $
/// $Date: 2015-11-04 14:09:23 +0100 (Wed, 04 Nov 2015) $
///
class xAODPhotonTrigAuxContainerOfflineCnv_v3 :
   public T_AthenaPoolTPCnvBase< xAOD::PhotonTrigAuxContainer,
                                 xAOD::PhotonAuxContainer_v3 > {

public:
   /// Default constructor
   xAODPhotonTrigAuxContainerOfflineCnv_v3();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::PhotonAuxContainer_v3* oldObj,
                             xAOD::PhotonTrigAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::PhotonTrigAuxContainer*,
                             xAOD::PhotonAuxContainer_v3*,
                             MsgStream& log );

}; // class xAODPhotonTrigAuxContainerOfflineCnv_v3

#endif // XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINEROFFLINECNV_V3_H
