// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPassBitsContainerCnv.h 755818 2016-06-17 14:51:59Z krasznaa $
#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGPASSBITSCONTAINERCNV_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGPASSBITSCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigger/TrigPassBitsContainer.h"

// Base the converter on the helper class:
typedef T_AthenaPoolCustomCnv< xAOD::TrigPassBitsContainer,
                               xAOD::TrigPassBitsContainer >
   xAODTrigPassBitsContainerCnvBase;

/// Custom converter for reading xAOD::TrigPassBitsContainer correctly
///
/// Unfortunately we managed to write quite a lot of AOD files with incorrect
/// xAOD::TrigPassBitsContainer payload in them. (Forgot to declare a container
/// proxy for the type.)
///
/// To overcome this, this converter explicitly creates a new container from
/// scratch. Which has the same size as the one on disk. Connects the new
/// container to the auxiliary store of the original one, and then returns
/// this freshly made container instead of the original one.
///
/// Once incorrectly written files will not be an issue anymore (possibly this
/// will never be the case, as we also wrote some BS files like this...), this
/// hack could be removed.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 755818 $
/// $Date: 2016-06-17 16:51:59 +0200 (Fri, 17 Jun 2016) $
///
class xAODTrigPassBitsContainerCnv : public xAODTrigPassBitsContainerCnvBase {

   friend class CnvFactory< xAODTrigPassBitsContainerCnv >;

public:
   /// Converter constructor
   xAODTrigPassBitsContainerCnv( ISvcLocator* svcLoc );

   /// Overridden function to pick up the name of the container
   virtual StatusCode createObj( IOpaqueAddress* pAddr,
                                 DataObject*& pObj ) override final;

   /// Function creating the persistent object
   virtual xAOD::TrigPassBitsContainer*
   createPersistent( xAOD::TrigPassBitsContainer* trans ) override final;

   /// Function reading in the persistent object from disk
   virtual xAOD::TrigPassBitsContainer* createTransient() override final;

private:
   /// SG key of the container being read
   std::string m_key;

}; // class xAODTrigPassBitsContainerCnv

#endif // XAODTRIGGERATHENAPOOL_XAODTRIGPASSBITSCONTAINERCNV_H
