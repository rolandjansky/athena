// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigMissingETAuxContainerCnv.h 592512 2014-04-11 09:38:00Z krasznaa $
#ifndef xAODTrigMissingETATHENAPOOL_XAODTrigMissingETAUXCONTAINERCNV_H
#define xAODTrigMissingETATHENAPOOL_XAODTrigMissingETAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigMissingETAuxContainer,
                               xAOD::TrigMissingETAuxContainer >
   xAODTrigMissingETAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigMissingETAuxContainer class
 *
 * @author Your name here
 *
 * $Revision: 592512 $
 * $Date: 2014-04-11 11:38:00 +0200 (Fri, 11 Apr 2014) $
 */
class xAODTrigMissingETAuxContainerCnv :
   public xAODTrigMissingETAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigMissingETAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigMissingETAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigMissingETAuxContainer*
   createPersistent( xAOD::TrigMissingETAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigMissingETAuxContainer* createTransient();

}; // class xAODTrigMissingETAuxContainerCnv

#endif // xAODTrigMissingETATHENAPOOL_XAODTrigMissingETAUXCONTAINERCNV_H
