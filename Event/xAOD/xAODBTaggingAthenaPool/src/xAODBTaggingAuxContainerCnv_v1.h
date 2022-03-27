// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id:  $
#ifndef XAODMUONATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_V1_H
#define XAODMUONATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

/// Converter class used for reading xAOD::BTaggingAuxContainer_v1
///
/// This converter implements the conversion from xAOD::BTaggingAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Dan Guest (based on example from Edward Moyse)
///
///
class xAODBTaggingAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAOD::BTaggingAuxContainer,
                                      xAOD::BTaggingAuxContainer_v1 > {

public:
   using base_class::transToPers;
   using base_class::persToTrans;

   /// Default constructor
   xAODBTaggingAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::BTaggingAuxContainer_v1* oldObj,
                             xAOD::BTaggingAuxContainer* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::BTaggingAuxContainer*,
                             xAOD::BTaggingAuxContainer_v1*,
                             MsgStream& log ) const override;

}; // class xAODBTaggingAuxContainerCnv_v1

#endif // XAODMUONATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_V1_H
