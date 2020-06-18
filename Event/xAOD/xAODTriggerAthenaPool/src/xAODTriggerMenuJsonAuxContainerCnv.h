// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGGERMENUJSONAUXCONTAINERCNV_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGGERMENUJSONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenuJsonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TriggerMenuJsonAuxContainer >
   xAODTriggerMenuJsonAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TriggerMenuJsonAuxContainer class
 */
class xAODTriggerMenuJsonAuxContainerCnv :
   public xAODTriggerMenuJsonAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTriggerMenuJsonAuxContainerCnv >;

protected:
   /// Converter constructor
public:
   xAODTriggerMenuJsonAuxContainerCnv( ISvcLocator* svcLoc );
protected:

   /// Function reading in the object from the input file
   virtual xAOD::TriggerMenuJsonAuxContainer*
   createTransientWithKey( const std::string& key ) override;

}; // class xAODTriggerMenuJsonAuxContainerCnv

#endif // XAODTRIGGERATHENAPOOL_XAODTRIGGERMENUJSONAUXCONTAINERCNV_H
