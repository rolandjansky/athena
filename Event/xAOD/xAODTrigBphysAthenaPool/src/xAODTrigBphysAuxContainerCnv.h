// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysAuxContainerCnv.h
#ifndef xAODTrigBphysATHENAPOOL_XAODTrigBphysAUXCONTAINERCNV_H
#define xAODTrigBphysATHENAPOOL_XAODTrigBphysAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigBphysAuxContainer,
                               xAOD::TrigBphysAuxContainer >
   xAODTrigBphysAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigBphysAuxContainer class
 *
 * @author Your name here
 *
 * $Revision: 572963 $
 * $Date: 2013-11-29 01:15:54 -0800 (Fri, 29 Nov 2013) $
 */
class xAODTrigBphysAuxContainerCnv :
   public xAODTrigBphysAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigBphysAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigBphysAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigBphysAuxContainer*
   createPersistent( xAOD::TrigBphysAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigBphysAuxContainer* createTransient();

}; // class xAODTrigBphysAuxContainerCnv

#endif // xAODTrigBphysATHENAPOOL_XAODTrigBphysAUXCONTAINERCNV_H
