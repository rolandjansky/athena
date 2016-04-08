// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv_v1.h 619884 2014-10-03 15:00:40Z $
#ifndef XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_V1_H
#define XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

/// Converter class used for reading xAOD::TrigEMClusterAuxContainer_v1
///
/// This converter implements the conversion from xAOD::TrigEMClusterAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 619884 $
/// $Date: 2014-10-03 17:00:40 +0200 (Fri, 03 Oct 2014) $
///
class xAODTrigEMClusterAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::TrigEMClusterAuxContainer,
                                 xAOD::TrigEMClusterAuxContainer_v1 > {

public:
   /// Default constructor
   xAODTrigEMClusterAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::TrigEMClusterAuxContainer_v1* oldObj,
                             xAOD::TrigEMClusterAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::TrigEMClusterAuxContainer*,
                             xAOD::TrigEMClusterAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODTrigEMClusterAuxContainerCnv_v1

#endif // XAODTRIGCALOATHENAPOOL_XAODTRIGEMCLUSTERAUXCONTAINERCNV_V1_H
