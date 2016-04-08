// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeContainerCnv_v1.h 693514 2015-09-07 15:51:04Z krasznaa $
#ifndef XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_V1_H
#define XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

/// Converter class used for reading xAOD::HIEventShapeContainer_v1
///
/// This very simple converter is provided mostly for the benefit of the
/// trigger BS reading code. As it really doesn't have to do much to convert
/// between the interface objects. The heavy duty work happens in the converter
/// of the auxiliary container.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 693514 $
/// $Date: 2015-09-07 17:51:04 +0200 (Mon, 07 Sep 2015) $
///
class xAODHIEventShapeContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::HIEventShapeContainer,
                                 xAOD::HIEventShapeContainer_v1 > {

public:
   /// Default constructor
   xAODHIEventShapeContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::HIEventShapeContainer_v1* oldObj,
                             xAOD::HIEventShapeContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::HIEventShapeContainer*,
                             xAOD::HIEventShapeContainer_v1*,
                             MsgStream& log );

}; // class xAODHIEventShapeContainerCnv_v1

#endif // XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_V1_H
