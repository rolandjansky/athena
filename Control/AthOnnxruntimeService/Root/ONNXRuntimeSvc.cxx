// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "AthOnnxruntimeService/ONNXRuntimeSvc.h"

namespace AthONNX {
  ONNXRuntimeSvc::ONNXRuntimeSvc(const std::string& name, ISvcLocator* svc) :
      asg::AsgService(name, svc)
   {
     declareServiceInterface<AthONNX::IONNXRuntimeSvc>();
   }
   StatusCode ONNXRuntimeSvc::initialize() {

      // Create the environment object.
      m_env = std::make_unique< Ort::Env >( ORT_LOGGING_LEVEL_WARNING,
                                            name().c_str() );
      ATH_MSG_DEBUG( "Ort::Env object created" );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode ONNXRuntimeSvc::finalize() {

      // Dekete the environment object.
      m_env.reset();
      ATH_MSG_DEBUG( "Ort::Env object deleted" );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   Ort::Env& ONNXRuntimeSvc::env() const {

      return *m_env;
   }

} // namespace AthONNX
