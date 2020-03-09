// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
   public T_AthenaPoolTPCnvConstBase< xAOD::EnergySumRoI,
                                      xAOD::EnergySumRoI_v1 > {

public:
   using base_class::transToPers;
   using base_class::persToTrans;

   /// Default constructor
   xAODEnergySumRoICnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::EnergySumRoI_v1* oldObj,
                             xAOD::EnergySumRoI* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::EnergySumRoI*,
                             xAOD::EnergySumRoI_v1*,
                             MsgStream& log ) const override;

}; // class xAODEnergySumRoI_v1

#endif // XAODTRIGGERATHENAPOOL_XAODENERGYSUMROICNV_V1_H
