// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPassBitsContainerCnv.h 755768 2016-06-17 13:43:10Z krasznaa $
#ifndef XAODTRIGGERATHENAPOOL_XAODTRIGPASSBITSCONTAINERCNV_H
#define XAODTRIGGERATHENAPOOL_XAODTRIGPASSBITSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODTrigger/TrigPassBitsContainer.h"

// Base the converter on the helper class:
typedef T_AthenaPoolxAODCnv< xAOD::TrigPassBitsContainer >
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
/// $Revision: 755768 $
/// $Date: 2016-06-17 15:43:10 +0200 (Fri, 17 Jun 2016) $
///
class xAODTrigPassBitsContainerCnv : public xAODTrigPassBitsContainerCnvBase {

public:
   /// Converter constructor
   xAODTrigPassBitsContainerCnv( ISvcLocator* svcLoc );

   /// Function reading in the persistent object from disk
   virtual xAOD::TrigPassBitsContainer* createTransient() override final;
};

#endif // XAODTRIGGERATHENAPOOL_XAODTRIGPASSBITSCONTAINERCNV_H
