/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RingerSelectorTools_tools_onnx_RingerSelectorTool_h 
#define RingerSelectorTools_tools_onnx_RingerSelectorTool_h 



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
#include "AthenaMonitoringKernel/MonitoredTimer.h"
#include <memory>
#include "TEnv.h"

namespace Ringer {

  namespace onnx{

    class RingerSelectorTool : public asg::AsgMessaging

    {
      public:
    
        /** Standard constructor */
        RingerSelectorTool();
    
        /** Standard destructor */
        ~RingerSelectorTool()=default;


        /**
         * @brief read tunings from configuration file
         **/
        StatusCode read_from( std::string, AthONNX::IONNXRuntimeSvc *);
        
        /**
         * @briel Calculation of model output for fast calorimetry step
         **/
        float predict(const xAOD::TrigRingerRings *,
                      Monitored::Timer<> &propagate_time, Monitored::Timer<> &preproc_time) const;
              
 
        /**
         * @briel Calculation of model output for fast calorimetry step
         **/
        float predict(const xAOD::TrigRingerRings *, const xAOD::TrigElectron *, 
                      Monitored::Timer<> &propagate_time, Monitored::Timer<> &preproc_time) const;


        /**
         * @brief Accept method
         **/
        bool accept(const xAOD::TrigRingerRings *, float discriminant, float mu ) const;
    

      private:
   
        /// @brief hold all onnx sessions
        std::vector< Ringer::onnx::Model > m_models;
        /// @brief hold all thresholds definitions
        std::vector< Ringer::onnx::Threshold > m_thresholds;

        bool m_useShowerShapes;

        bool m_useTrackVariables;

        /// @brief prepare inputs to apply into the model
        void prepare_inputs( const xAOD::TrigRingerRings *, const xAOD::TrigElectron * , std::vector< std::vector<float> > &) const;
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
