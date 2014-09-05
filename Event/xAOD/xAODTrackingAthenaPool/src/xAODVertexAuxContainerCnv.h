// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODVertexAuxContainerCnv.h 597069 2014-05-14 16:35:38Z krasznaa $
#ifndef XAODTRACKPARTICLEATHENAPOOL_XAODVERTEXAUXCONTAINERCNV_H
#define XAODTRACKPARTICLEATHENAPOOL_XAODVERTEXAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#define protected public
#define private public
#include "xAODTracking/VertexAuxContainer.h"
#undef private
#undef protected

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::VertexAuxContainer,
                               xAOD::VertexAuxContainer >
   xAODVertexAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::VertexAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::VertexContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Edward Moyse <Edward.Moyse@cern.ch>
 */
class xAODVertexAuxContainerCnv :
   public xAODVertexAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODVertexAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODVertexAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::VertexAuxContainer*
   createPersistent( xAOD::VertexAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::VertexAuxContainer* createTransient();

}; // class xAODVertexAuxContainerCnv

#endif 
