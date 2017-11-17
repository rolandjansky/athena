/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthMetaDataTool.cxx 685407 2015-07-24 16:15:06Z cranshaw $

// Local include(s):
#include "xAODTruthMetaDataTool.h"

// EDM include(s):
#include "xAODTruth/TruthMetaDataContainer.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"

namespace xAODMaker {

   xAODTruthMetaDataTool::xAODTruthMetaDataTool( const std::string& name )
      : asg::AsgMetadataTool( name ) {
   }

   StatusCode xAODTruthMetaDataTool::initialize() {

      // Greet the user:
      ATH_MSG_DEBUG( "Initialising - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODTruthMetaDataTool::beginInputFile() {

      std::vector<std::string> keys;
      inputMetaStore()->keys<xAOD::TruthMetaDataContainer>(keys);
      for (const auto &key : keys) {
         ATH_MSG_VERBOSE("Attempting to copy xAOD::TruthMetaDataContainer: " << key);
         const xAOD::TruthMetaDataContainer* truthMD_in = 0;
         if (!inputMetaStore()->retrieve(truthMD_in, key).isSuccess()) {
            ATH_MSG_ERROR("Could not retrieve object for xAOD::TruthMetaDataContainer");
            return StatusCode::FAILURE;
         }
         xAOD::TruthMetaDataContainer* truthMD_out = 0;
         if (!outputMetaStore()->contains<xAOD::TruthMetaDataContainer>(key)) {
            truthMD_out = new xAOD::TruthMetaDataContainer();
            xAOD::TruthMetaDataAuxContainer* truthAuxMD_out = new xAOD::TruthMetaDataAuxContainer();
            truthMD_out->setStore(truthAuxMD_out);
            if (!outputMetaStore()->record(truthMD_out, key).isSuccess()) {
               ATH_MSG_ERROR("Could not record xAOD::TruthMetaDataContainer:  " << key);
               return StatusCode::FAILURE;
            }
            if (!outputMetaStore()->record(truthAuxMD_out, key + "Aux.").isSuccess()) {
               ATH_MSG_ERROR("Could not record xAOD::TruthMetaDataAuxContainer: " << key << "Aux.");
               return StatusCode::FAILURE;
            }
         } else {
            if (!outputMetaStore()->retrieve(truthMD_out, key).isSuccess()) {
               ATH_MSG_ERROR("Could not find xAOD::TruthMetaDataAuxContainer in output: " << key);
               return StatusCode::FAILURE;
            }
         }
         for (const auto& input : *truthMD_in) {
            xAOD::TruthMetaData* copy = new xAOD::TruthMetaData(*input);
            truthMD_out->push_back(copy);
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
