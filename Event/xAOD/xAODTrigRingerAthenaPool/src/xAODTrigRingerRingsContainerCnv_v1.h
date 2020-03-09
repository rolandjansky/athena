// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRingerRingsContainerCnv_v1.h 707590 2015-11-12 19:09:03Z krasznaa $
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSCONTAINERCNV_V1_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"

/// Converter class used for reading xAOD::TrigRingerRingsContainer_v1
class xAODTrigRingerRingsContainerCnv_v1
  : public T_AthenaPoolTPCnvConstBase< xAOD::TrigRingerRingsContainer,
                                       xAOD::TrigRingerRingsContainer_v1 > 

{ 
  public:
    using base_class::transToPers;
    using base_class::persToTrans;

    /// Default constructor
    xAODTrigRingerRingsContainerCnv_v1();

    /// Function converting from the old type to the current one
    virtual void persToTrans( const xAOD::TrigRingerRingsContainer_v1* oldObj,
                                    xAOD::TrigRingerRingsContainer* newObj,
                                     MsgStream& log ) const override;
    /// Dummy function inherited from the base class
    virtual void transToPers( const xAOD::TrigRingerRingsContainer*,
                                    xAOD::TrigRingerRingsContainer_v1*,
                                    MsgStream& log ) const override;

}; // class  
#endif 


