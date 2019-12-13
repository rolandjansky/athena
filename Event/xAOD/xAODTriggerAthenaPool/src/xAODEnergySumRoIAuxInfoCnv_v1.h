// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERATHENAPOOL_XAODENERGYSUMROIAUXINFOCNV_V1_H
#define XAODTRIGGERATHENAPOOL_XAODENERGYSUMROIAUXINFOCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v1.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"

/// Converter class used for reading xAOD::EnergySumRoIAuxInfo_v1
///
class xAODEnergySumRoIAuxInfoCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAOD::EnergySumRoIAuxInfo,
                                      xAOD::EnergySumRoIAuxInfo_v1 > {

public:
   using base_class::transToPers;
   using base_class::persToTrans;

   /// Default constructor
   xAODEnergySumRoIAuxInfoCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::EnergySumRoIAuxInfo_v1* oldObj,
                             xAOD::EnergySumRoIAuxInfo* newObj,
                             MsgStream& log ) const override;

   /// Function for converting from old type to current one
//   xAOD::EnergySumRoIAuxInfo* createTransient(const xAOD::EnergySumRoIAuxInfo_v1* oldObj,
//                                              MsgStream& log );

   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::EnergySumRoIAuxInfo*,
                             xAOD::EnergySumRoIAuxInfo_v1*,
                             MsgStream& log ) const override;

}; // class xAODEnergySumRoIAuxInfo_v1

#endif // XAODTRIGGERATHENAPOOL_XAODENERGYSUMROIAUXINFOCNV_V1_H
