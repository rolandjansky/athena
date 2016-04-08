// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonAuxContainerCnv_v1.h 707606 2015-11-12 19:59:49Z krasznaa $
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_V1_H
#define XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v1.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"

/// Converter class used for reading xAOD::L2StandAloneMuonAuxContainer_v1
///
/// This converter implements the conversion from xAOD::L2StandAloneMuonAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author sss
///
/// $Revision: 707606 $
/// $Date: 2015-11-12 20:59:49 +0100 (Thu, 12 Nov 2015) $
///
class xAODL2StandAloneMuonAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::L2StandAloneMuonAuxContainer,
                                 xAOD::L2StandAloneMuonAuxContainer_v1 > {

public:
   /// Default constructor
   xAODL2StandAloneMuonAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::L2StandAloneMuonAuxContainer_v1* oldObj,
                             xAOD::L2StandAloneMuonAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::L2StandAloneMuonAuxContainer*,
                             xAOD::L2StandAloneMuonAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODL2StandAloneMuonAuxContainer_v1

#endif // XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_V1_H
