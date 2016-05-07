// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:  $
#ifndef XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_V2_H
#define XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_V2_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODMuon/versions/MuonAuxContainer_v2.h"
#include "xAODMuon/MuonAuxContainer.h"

/// Converter class used for reading xAOD::MuonAuxContainer_v2
///
/// This converter implements the conversion from xAOD::MuonAuxContainer_v2
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Edward.Moyse@cern.ch (based on example from Attila)
///
/// $Revision: 619884 $
/// $Date: 2014-10-03 11:00:40 -0400 (Fri, 03 Oct 2014) $
///
class xAODMuonAuxContainerCnv_v2 :
   public T_AthenaPoolTPCnvBase< xAOD::MuonAuxContainer,
                                 xAOD::MuonAuxContainer_v2 > {

public:
   /// Default constructor
   xAODMuonAuxContainerCnv_v2();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::MuonAuxContainer_v2* oldObj,
                             xAOD::MuonAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::MuonAuxContainer*,
                             xAOD::MuonAuxContainer_v2*,
                             MsgStream& log );

}; // class xAODMuonAuxContainerCnv_v2

#endif // XAODMUONATHENAPOOL_xAODMuonAuxContainerCnv_v2_H