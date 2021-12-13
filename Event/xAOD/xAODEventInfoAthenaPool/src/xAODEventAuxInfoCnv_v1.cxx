// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "xAODEventAuxInfoCnv_v1.h"
#include "MessageMacros.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// System include(s).
#include <stdexcept>

void xAODEventAuxInfoCnv_v1::persToTrans( const xAOD::EventAuxInfo_v1* oldObj,
                                          xAOD::EventAuxInfo* newObj,
                                          MsgStream& log ) const {

   // Greet the user.
   ATH_MSG( "Converting xAOD::EventAuxInfo_v1 to the current version..." );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Print what happened:
   ATH_MSG( "Converting xAOD::EventAuxInfo_v1 to the current version... "
            "[OK]" );
   return;
}

void xAODEventAuxInfoCnv_v1::transToPers( const xAOD::EventAuxInfo*,
                                          xAOD::EventAuxInfo_v1*,
                                          MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODEventAuxInfoCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODEventAuxInfoCnv_v1::"
                             "transToPers" );
   return;
}
