// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "xAODEventAuxInfoCnv_v2.h"
#include "MessageMacros.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// System include(s).
#include <stdexcept>

void xAODEventAuxInfoCnv_v2::persToTrans( const xAOD::EventAuxInfo_v2* oldObj,
                                          xAOD::EventAuxInfo* newObj,
                                          MsgStream& log ) {

   // Greet the user.
   ATH_MSG( "Converting xAOD::EventAuxInfo_v2 to the current version..." );

   // Copy the payload of the v2 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj,
                            static_cast< IThinningSvc* >( nullptr ) );

   // Print what happened:
   ATH_MSG( "Converting xAOD::EventAuxInfo_v2 to the current version... "
            "[OK]" );
   return;
}

void xAODEventAuxInfoCnv_v2::transToPers( const xAOD::EventAuxInfo*,
                                          xAOD::EventAuxInfo_v2*,
                                          MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODEventAuxInfoCnv_v2::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODEventAuxInfoCnv_v2::"
                             "transToPers" );
   return;
}
