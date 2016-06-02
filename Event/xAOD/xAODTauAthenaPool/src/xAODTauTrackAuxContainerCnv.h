// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAUATHENAPOOL_XAODTAUTRACKAUXCONTAINERCNV_H
#define XAODTAUATHENAPOOL_XAODTAUTRACKAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTau/TauTrackAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TauTrackAuxContainer,
                               xAOD::TauTrackAuxContainer >
   xAODTauTrackAuxContainerCnvBase;

class xAODTauTrackAuxContainerCnv :
   public xAODTauTrackAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTauTrackAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTauTrackAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TauTrackAuxContainer*
   createPersistent( xAOD::TauTrackAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TauTrackAuxContainer* createTransient();

}; // class xAODTauTrackAuxContainerCnv

#endif // XAODTAUATHENAPOOL_XAODTAUTRACKAUXCONTAINERCNV_H
