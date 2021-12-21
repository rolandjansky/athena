// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTINFOATHENAPOOL_XAODEVENAUXINFOCNV_V3_H
#define XAODEVENTINFOATHENAPOOL_XAODEVENAUXINFOCNV_V3_H

// Gaudi/Athena include(s).
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s).
#include "xAODEventInfo/versions/EventAuxInfo_v3.h"
#include "xAODEventInfo/EventAuxInfo.h"

/// Converter for reading @c xAOD::EventAuxInfo_v3
///
/// This converter takes care of reading @c xAOD::EventAuxInfo_v3 into
/// the "current" version of @c xAOD::EventAuxInfo. (Which is
/// @c xAOD::EventAuxInfo_v1 in this branch.)
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
class xAODEventAuxInfoCnv_v3 :
   public T_AthenaPoolTPCnvBase< xAOD::EventAuxInfo,
                                 xAOD::EventAuxInfo_v3 > {

public:
   /// Default constructor
   xAODEventAuxInfoCnv_v3() = default;

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::EventAuxInfo_v3* oldObj,
                             xAOD::EventAuxInfo* newObj,
                             MsgStream& log ) override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::EventAuxInfo*,
                             xAOD::EventAuxInfo_v3*,
                             MsgStream& log ) override;

}; // class xAODEventAuxInfoCnv_v3

#endif // XAODEVENTINFOATHENAPOOL_XAODEVENAUXINFOCNV_V3_H
