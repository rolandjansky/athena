// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRNNOutputAuxContainerCnv.h 707590 2015-11-12 19:09:03Z krasznaa $
#ifndef XAODTRIGRINGERATHENAPOOL_XAODRNNOUTPUTAUXCONTAINERCNV_H
#define XAODTRIGRINGERATHENAPOOL_XAODRNNOUTPUTAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigRNNOutputAuxContainer,
                               xAOD::TrigRNNOutputAuxContainer >
   xAODTrigRNNOutputAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigRNNOutputAuxContainer class
 *
 * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
 *
 * $Revision: 707590 $
 * $Date: 2015-11-12 20:09:03 +0100 (Thu, 12 Nov 2015) $
 */
class xAODTrigRNNOutputAuxContainerCnv :
   public xAODTrigRNNOutputAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigRNNOutputAuxContainerCnv >;

protected:
   /// Converter constructor
  xAODTrigRNNOutputAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigRNNOutputAuxContainer*
   createPersistent( xAOD::TrigRNNOutputAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigRNNOutputAuxContainer* createTransient();

}; // class xAODRingerRNNOutputAuxContainerCnv

#endif // XAODTRIGRINGERATHENAPOOL_XAODRNNOUTPUTAUXCONTAINERCNV_H
