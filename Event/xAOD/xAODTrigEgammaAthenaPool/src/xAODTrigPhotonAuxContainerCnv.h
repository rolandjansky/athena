// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPhotonAuxContainerCnv.h
#ifndef xAODTrigEgammaATHENAPOOL_XAODTrigPhotonAUXCONTAINERCNV_H
#define xAODTrigEgammaATHENAPOOL_XAODTrigPhotonAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigPhotonAuxContainer,
                               xAOD::TrigPhotonAuxContainer >
   xAODTrigPhotonAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigPhotonAuxContainer class
 *
 * @author Your name here
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigPhotonAuxContainerCnv :
   public xAODTrigPhotonAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigPhotonAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigPhotonAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigPhotonAuxContainer*
   createPersistent( xAOD::TrigPhotonAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigPhotonAuxContainer* createTransient();

}; // class xAODTrigPhotonAuxContainerCnv

#endif // xAODTrigEgammaATHENAPOOL_XAODTrigPhotonAUXCONTAINERCNV_H
