// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERATHENAPOOL_XAODENERGYSUMROICNV_V1_H
#define XAODTRIGGERATHENAPOOL_XAODENERGYSUMROICNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigger/versions/EnergySumRoI_v1.h"
#include "xAODTrigger/EnergySumRoI.h"

/// Converter class used for reading xAOD::EnergySumRoI_v1
///
class xAODEnergySumRoICnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::EnergySumRoI,
                                 xAOD::EnergySumRoI_v1 > {

public:
   /// Default constructor
   xAODEnergySumRoICnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::EnergySumRoI_v1* oldObj,
                             xAOD::EnergySumRoI* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::EnergySumRoI*,
                             xAOD::EnergySumRoI_v1*,
                             MsgStream& log );

}; // class xAODEnergySumRoI_v1

#endif // XAODTRIGGERATHENAPOOL_XAODENERGYSUMROICNV_V1_H
