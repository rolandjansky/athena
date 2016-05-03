/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v1.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "xAODTrigger/versions/EnergySumRoI_v1.h"
#include "xAODTrigger/versions/EnergySumRoI_v2.h"

// Local include(s):
#include "xAODEnergySumRoIAuxInfoCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::INFO

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODEnergySumRoIAuxInfoCnv_v1::xAODEnergySumRoIAuxInfoCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::EnergySumRoIAuxInfo,
                           xAOD::EnergySumRoIAuxInfo_v1 >() {

}

void xAODEnergySumRoIAuxInfoCnv_v1::
persToTrans( const xAOD::EnergySumRoIAuxInfo_v1* oldObj,
             xAOD::EnergySumRoIAuxInfo* newObj,
             MsgStream& /*log*/ ) {

   // Greet the user:
   //ATH_MSG( "Converting xAOD::EnergySumRoIAuxInfo_v1 to current version..." );

   xAOD::EnergySumRoI_v1 oldRoI;
   xAOD::EnergySumRoI_v2 newRoI;
   oldRoI.setStore(oldObj);
   newRoI.setStore(newObj);

   uint32_t word0 = oldRoI.roiWord0();
   uint32_t word1 = oldRoI.roiWord1();
   uint32_t word2 = oldRoI.roiWord2();
   float_t ex = oldRoI.energyX();
   float_t ey = oldRoI.energyY();
   float_t et = oldRoI.energyT();

   newRoI.initialize(word0, word1, word2, ex, ey, et);

   return;
}

void xAODEnergySumRoIAuxInfoCnv_v1::transToPers( const xAOD::EnergySumRoIAuxInfo*,
                                                xAOD::EnergySumRoIAuxInfo_v1*,
                                                MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODEnergySumRoIAuxInfoCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODEnergySumRoIAuxInfoCnv_v1::"
                             "transToPers" );

   return;
}
