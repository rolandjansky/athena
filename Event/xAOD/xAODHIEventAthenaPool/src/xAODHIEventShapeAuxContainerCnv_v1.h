// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeAuxContainerCnv_v1.h 693658 2015-09-08 10:00:53Z krasznaa $
#ifndef XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_V1_H
#define XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v1.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"

/// Converter class used for reading xAOD::HIEventShapeAuxContainer_v1
///
/// This class does the heavy lifting of converting the payload of the
/// _v1 container to the current one. By translating the variables from
/// the former to the latter.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 693658 $
/// $Date: 2015-09-08 12:00:53 +0200 (Tue, 08 Sep 2015) $
///
class xAODHIEventShapeAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::HIEventShapeAuxContainer,
                                 xAOD::HIEventShapeAuxContainer_v1 > {

public:
   /// Default constructor
   xAODHIEventShapeAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::HIEventShapeAuxContainer_v1* oldObj,
                             xAOD::HIEventShapeAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::HIEventShapeAuxContainer*,
                             xAOD::HIEventShapeAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODHIEventShapeAuxContainerCnv_v1

#endif // XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_V1_H
