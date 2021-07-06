/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RingerSelectorTools_tools_onnx_RingerSelector_h 
#define RingerSelectorTools_tools_onnx_RingerSelector_h 



#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AsgMessaging/AsgMessaging.h"
#include "RingerSelectorTools/tools/onnx/Model.h"
#include "RingerSelectorTools/tools/onnx/Threshold.h"
#include "AthOnnxruntimeService/IONNXRuntimeSvc.h"
#include <core/session/onnxruntime_cxx_api.h>
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <memory>
#include "TEnv.h"

namespace Ringer {

  namespace onnx{

    class RingerSelector : public asg::AsgMessaging

    {
      public:
    
        /** Standard constructor */
        RingerSelector( std::string name );
    
        /** Standard destructor */
        ~RingerSelector()=default;


        /**
         * @brief read tunings from configuration file
         **/
        StatusCode read_from( std::string, AthONNX::IONNXRuntimeSvc *);


        /**
         * @brief prepare all inputs
         **/
        std::vector<std::vector<float>>  prepare_inputs( const xAOD::TrigRingerRings *, const xAOD::TrigElectron *) const;


        /**
         * @briel Calculation of model output for fast calorimetry step
         **/
        float predict(const xAOD::TrigRingerRings *, std::vector< std::vector< float > > & ) const;


        /**
         * @brief Accept method
         **/
        bool accept(const xAOD::TrigRingerRings *, float discr, float mu ) const;
    

      private:
   
        /// @brief hold all onnx sessions
        std::vector< Ringer::onnx::Model > m_models;
        /// @brief hold all thresholds definitions
        std::vector< Ringer::onnx::Threshold > m_thresholds;
        /// @brief use shower shapes as second inpit
        bool m_useShowerShapes;
        /// @brief use track as thrird output
        bool m_useTrackVariables;

       /// @brief parse tenv string into list with type T
        template <typename T> bool strtof(const std::string& input, T& f);
        /// @brief Get the list of values inside of tenv
        template <typename T>  std::vector<T> GetValues (const std::string& input,  TEnv& env);
        /// @brief Get the list of paths inside of tenv
        std::vector<std::string> GetPaths(const std::string& input, TEnv& env);  


    };
    
 
  } // namespace onnx
} //namespace Ringer

#endif
