// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv_v1.h 628099 2014-11-13 09:30:05Z krasznaa $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_V1_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

/// Converter class used for reading xAOD::CaloClusterAuxContainer_v1
///
/// This converter implements the conversion from xAOD::CaloClusterAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 628099 $
/// $Date: 2014-11-13 10:30:05 +0100 (Thu, 13 Nov 2014) $
///
class xAODCaloClusterAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::CaloClusterAuxContainer,
                                 xAOD::CaloClusterAuxContainer_v1 > {

public:
   /// Default constructor
   xAODCaloClusterAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::CaloClusterAuxContainer_v1* oldObj,
                             xAOD::CaloClusterAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::CaloClusterAuxContainer*,
                             xAOD::CaloClusterAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODCaloClusterAuxContainerCnv_v1

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_V1_H
