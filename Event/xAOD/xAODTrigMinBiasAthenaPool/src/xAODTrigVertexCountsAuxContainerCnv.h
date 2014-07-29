// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigVertexCountsAuxContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigVertexCountsAUXCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigVertexCountsAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigVertexCountsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigVertexCountsAuxContainer,
                               xAOD::TrigVertexCountsAuxContainer >
   xAODTrigVertexCountsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigVertexCountsAuxContainer class
 * @author Gordon Watts 
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigVertexCountsAuxContainerCnv :
   public xAODTrigVertexCountsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigVertexCountsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigVertexCountsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigVertexCountsAuxContainer*
   createPersistent( xAOD::TrigVertexCountsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigVertexCountsAuxContainer* createTransient();

}; // class xAODTrigVertexCountsAuxContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigVertexCountsAUXCONTAINERCNV_H
