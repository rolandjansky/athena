// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_V1_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRNNOutput.h"

/// Converter class used for reading xAOD::TrigRNNOutputContainer_v1
class xAODTrigRNNOutputContainerCnv_v1 : public T_AthenaPoolTPCnvBase< xAOD::TrigRNNOutputContainer,
                                                                       xAOD::TrigRNNOutputContainer_v1 > 

{ 
  public:
    /// Default constructor
    xAODTrigRNNOutputContainerCnv_v1();

    /// Function converting from the old type to the current one
    virtual void persToTrans( const xAOD::TrigRNNOutputContainer_v1* oldObj,
                                    xAOD::TrigRNNOutputContainer* newObj,
                                     MsgStream& log );
    /// Dummy function inherited from the base class
    virtual void transToPers( const xAOD::TrigRNNOutputContainer*,
                                    xAOD::TrigRNNOutputContainer_v1*,
                                    MsgStream& log );

}; // class  
#endif 


