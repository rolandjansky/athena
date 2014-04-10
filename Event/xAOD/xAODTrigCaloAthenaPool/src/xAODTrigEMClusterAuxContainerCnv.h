// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv.h
#ifndef XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_H
#define XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigEMClusterAuxContainer,
                               xAOD::TrigEMClusterAuxContainer >
   xAODTrigEMClusterAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigEMClusterAuxContainer class
 *
 * @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 592328 $
 * $Date: 2014-04-10 12:58:05 +0200 (Thu, 10 Apr 2014) $
 */
class xAODTrigEMClusterAuxContainerCnv :
   public xAODTrigEMClusterAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigEMClusterAuxContainerCnv >;

protected:
   /// Converter constructor
  xAODTrigEMClusterAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigEMClusterAuxContainer*
   createPersistent( xAOD::TrigEMClusterAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigEMClusterAuxContainer* createTransient();

}; // class xAODTrigEMClusterAuxContainerCnv

#endif // XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_H
