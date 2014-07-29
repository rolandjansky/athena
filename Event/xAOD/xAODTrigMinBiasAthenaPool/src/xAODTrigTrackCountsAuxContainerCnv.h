// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigTrackCountsAuxContainerCnv.h
#ifndef xAODTrigMinBiasATHENAPOOL_XAODTrigTrackCountsAUXCONTAINERCNV_H
#define xAODTrigMinBiasATHENAPOOL_XAODTrigTrackCountsAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigTrackCountsAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigTrackCountsAuxContainer,
                               xAOD::TrigTrackCountsAuxContainer >
   xAODTrigTrackCountsAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigTrackCountsAuxContainer class
 * @author Gordon Watts 
 * @author Andrzej Zemla <azemla@cern.ch>
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigTrackCountsAuxContainerCnv :
   public xAODTrigTrackCountsAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigTrackCountsAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigTrackCountsAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigTrackCountsAuxContainer*
   createPersistent( xAOD::TrigTrackCountsAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigTrackCountsAuxContainer* createTransient();

}; // class xAODTrigTrackCountsAuxContainerCnv

#endif // xAODTrigMinBiasATHENAPOOL_XAODTrigTrackCountsAUXCONTAINERCNV_H
