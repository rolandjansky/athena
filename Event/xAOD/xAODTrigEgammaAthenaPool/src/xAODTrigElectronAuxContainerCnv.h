// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigElectronAuxContainerCnv.h
#ifndef xAODTrigEgammaATHENAPOOL_XAODTrigElectronAUXCONTAINERCNV_H
#define xAODTrigEgammaATHENAPOOL_XAODTrigElectronAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigElectronAuxContainer,
                               xAOD::TrigElectronAuxContainer >
   xAODTrigElectronAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigElectronAuxContainer class
 *
 * @author Your name here
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigElectronAuxContainerCnv :
   public xAODTrigElectronAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigElectronAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigElectronAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigElectronAuxContainer*
   createPersistent( xAOD::TrigElectronAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigElectronAuxContainer* createTransient();

}; // class xAODTrigElectronAuxContainerCnv

#endif // xAODTrigEgammaATHENAPOOL_XAODTrigElectronAUXCONTAINERCNV_H
