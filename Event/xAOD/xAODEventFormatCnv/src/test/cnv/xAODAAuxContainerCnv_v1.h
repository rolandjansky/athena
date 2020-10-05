// Dear emacs, this is -*- c++ -*-
/*  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */
#ifndef XAODEVENTFORMATCNV_TEST_XAODAAUXCONTAINERCNV_V1_H
#define XAODEVENTFORMATCNV_TEST_XAODAAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODEventFormatCnv/test/versions/AAuxContainer_v1.h"
#include "xAODEventFormatCnv/test/AAuxContainer.h"

class xAODAAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAODMakerTest::AAuxContainer,
                                      xAODMakerTest::AAuxContainer_v1 > {

public:
  using base_class::transToPers;
  using base_class::persToTrans;


   /// Default constructor
   xAODAAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAODMakerTest::AAuxContainer_v1* oldObj,
                             xAODMakerTest::AAuxContainer* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAODMakerTest::AAuxContainer*,
                             xAODMakerTest::AAuxContainer_v1*,
                             MsgStream& log ) const override;

};

#endif  // XAODEVENTFORMATCNV_TEST_XAODAAUXCONTAINERCNV_V1_H
