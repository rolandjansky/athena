/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonAuxContainerCnv_v1.cxx 707606 2015-11-12 19:59:49Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODL2StandAloneMuonAuxContainerCnv_v1.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODL2StandAloneMuonAuxContainerCnv_v1::xAODL2StandAloneMuonAuxContainerCnv_v1()
{
}


namespace {

void migrateVector (const SG::IConstAuxStore& oldObj,
                    SG::IAuxStore& newObj,
                    const char* oldName,
                    const char* newName)
{
  size_t sz = oldObj.size();
  if (!sz) return;

  SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
  SG::auxid_t oldId = reg.getAuxID<uint32_t> (oldName);
  SG::auxid_t newId = reg.getAuxID<float> (newName);

  const uint32_t* oldPtr = reinterpret_cast<const uint32_t*> (oldObj.getData (oldId));
  float* newPtr = reinterpret_cast<float*> (newObj.getData (newId, sz, sz));
  if (oldPtr && newPtr) {
    std::copy (oldPtr, oldPtr+sz, newPtr);
  }
}


}

void xAODL2StandAloneMuonAuxContainerCnv_v1::
persToTrans( const xAOD::L2StandAloneMuonAuxContainer_v1* oldObj,
             xAOD::L2StandAloneMuonAuxContainer* newObj,
             MsgStream& log ) const
{
   // Greet the user:
   ATH_MSG( "Converting xAOD::L2StandAloneMuonAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   size_t sz = oldObj->size();
   newObj->resize (sz);
   migrateVector (*oldObj, *newObj, "roiEtaUint", "roiEta");
   migrateVector (*oldObj, *newObj, "roiPhiUint", "roiPhi");


   // Print what happened:
   ATH_MSG( "Converting xAOD::L2StandAloneMuonAuxContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODL2StandAloneMuonAuxContainerCnv_v1::
transToPers( const xAOD::L2StandAloneMuonAuxContainer*,
             xAOD::L2StandAloneMuonAuxContainer_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODL2StandAloneMuonAuxContainer_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODL2StandAloneMuonAuxContainer_v1::"
                             "transToPers" );

   return;
}
