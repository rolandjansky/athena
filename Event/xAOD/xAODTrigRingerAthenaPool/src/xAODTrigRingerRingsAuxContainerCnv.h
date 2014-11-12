// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingerRingsAuxContainerCnv.h
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSAUXCONTAINERCNV_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigRingerRingsAuxContainer, xAOD::TrigRingerRingsAuxContainer > xAODTrigRingerRingsAuxContainerCnvBase;
/**
 *  @short POOL converter for the xAOD::TrigRingerRingsAuxContainer class
 *
 * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
 *
 * $Revision:  $
 * $Date: $
 */
class xAODTrigRingerRingsAuxContainerCnv : public xAODTrigRingerRingsAuxContainerCnvBase {

     // Declare the factory as our friend:
     friend class CnvFactory< xAODTrigRingerRingsAuxContainerCnv >;

   protected:
     /// Converter constructor
     xAODTrigRingerRingsAuxContainerCnv( ISvcLocator* svcLoc );
     /// Function preparing the container to be written out
     virtual xAOD::TrigRingerRingsAuxContainer* createPersistent( xAOD::TrigRingerRingsAuxContainer* trans );
     /// Function reading in the object from the input file
     virtual xAOD::TrigRingerRingsAuxContainer* createTransient();

}; // class xAODRingerRingsAuxContainerCnv

#endif // XAOD
