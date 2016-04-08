// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODDiTauJetAuxContainerCnv.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAUATHENAPOOL_XAODDITAUJETAUXCONTAINERCNV_H
#define XAODDITAUATHENAPOOL_XAODDITAUJETAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTau/DiTauJetAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::DiTauJetAuxContainer,
                               xAOD::DiTauJetAuxContainer >
   xAODDiTauJetAuxContainerCnvBase;

class xAODDiTauJetAuxContainerCnv :
   public xAODDiTauJetAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODDiTauJetAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODDiTauJetAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::DiTauJetAuxContainer*
   createPersistent( xAOD::DiTauJetAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::DiTauJetAuxContainer* createTransient();

}; // class xAODDiTauJetAuxContainerCnv

#endif // XAODDITAUATHENAPOOL_XAODDITAUJETAUXCONTAINERCNV_H
