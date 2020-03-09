// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTriggerMenuAuxContainerCnv.h 647168 2015-02-16 15:30:50Z krasznaa $
#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGGERMENUAUXCONTAINERCNV_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGGERMENUAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenuAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TriggerMenuAuxContainer >
   xAODTriggerMenuAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TriggerMenuAuxContainer class
 *
 *         This converter takes care of applying the "soft" and "hard"
 *         schema evolution of xAOD::TriggerMenuAuxContainer. For now
 *         only a "soft" one needs to be applied.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 647168 $
 * $Date: 2015-02-16 16:30:50 +0100 (Mon, 16 Feb 2015) $
 */
class xAODTriggerMenuAuxContainerCnv :
   public xAODTriggerMenuAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTriggerMenuAuxContainerCnv >;

protected:
   /// Converter constructor
public:
   xAODTriggerMenuAuxContainerCnv( ISvcLocator* svcLoc );
protected:

   /// Function reading in the object from the input file
   virtual xAOD::TriggerMenuAuxContainer*
   createTransientWithKey( const std::string& key ) override;

}; // class xAODTriggerMenuAuxContainerCnv

#endif // XAODTRIGGERATHENAPOOL_XAODTRIGGERMENUAUXCONTAINERCNV_H
