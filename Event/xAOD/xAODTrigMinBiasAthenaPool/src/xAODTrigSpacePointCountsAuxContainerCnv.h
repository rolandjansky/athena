// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigSpacePointCountsAuxContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigSpacePointCountsAUXCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigSpacePointCountsAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigSpacePointCountsAuxContainer,
                               xAOD::TrigSpacePointCountsAuxContainer >
   xAODTrigSpacePointCountsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigSpacePointCountsAuxContainer class
 * @author Gordon Watts 
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigSpacePointCountsAuxContainerCnv :
   public xAODTrigSpacePointCountsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigSpacePointCountsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigSpacePointCountsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigSpacePointCountsAuxContainer*
   createPersistent( xAOD::TrigSpacePointCountsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigSpacePointCountsAuxContainer* createTransient();

}; // class xAODTrigSpacePointCountsAuxContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigSpacePointCountsAUXCONTAINERCNV_H
