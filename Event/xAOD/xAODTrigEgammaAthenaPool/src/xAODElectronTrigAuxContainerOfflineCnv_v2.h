// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronTrigAuxContainerOfflineCnv_v2.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINEROFFLINECNV_V2_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINEROFFLINECNV_V2_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v2.h"
#include "xAODTrigEgamma/ElectronTrigAuxContainer.h"

/// Converter class used to read xAOD::ElectronAuxContainer_v2
///
/// Since we switched from using the offline xAOD::ElectronAuxContainer
/// type to using a trigger specific xAOD::ElectronTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 705790 $
/// $Date: 2015-11-04 14:09:23 +0100 (Wed, 04 Nov 2015) $
///
class xAODElectronTrigAuxContainerOfflineCnv_v2 :
   public T_AthenaPoolTPCnvBase< xAOD::ElectronTrigAuxContainer,
                                 xAOD::ElectronAuxContainer_v2 > {

public:
   /// Default constructor
   xAODElectronTrigAuxContainerOfflineCnv_v2();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::ElectronAuxContainer_v2* oldObj,
                             xAOD::ElectronTrigAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::ElectronTrigAuxContainer*,
                             xAOD::ElectronAuxContainer_v2*,
                             MsgStream& log );

}; // class xAODElectronTrigAuxContainerOfflineCnv_v2

#endif // XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINEROFFLINECNV_V2_H
