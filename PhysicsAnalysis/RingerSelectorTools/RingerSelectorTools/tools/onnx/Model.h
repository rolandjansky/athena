/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RingerSelectorTools_tools_onnx_Model_h 
#define RingerSelectorTools_tools_onnx_Model_h 

#include "AsgMessaging/AsgMessaging.h"
#include <core/session/onnxruntime_cxx_api.h>
#include "AthOnnxruntimeService/IONNXRuntimeSvc.h"
#include <assert.h>
#include <vector>




namespace Ringer{

  namespace onnx{

    class Model
    {
      public:
        
        /*! Constructor */ 
        Model( std::string modelPath, AthONNX::IONNXRuntimeSvc *svc,
               float etmin, float etmax, float etamin, float etamax);
        
        /*! Destructor */
        ~Model()=default;
      
        void compile();

        /*! Get the Et lower edge */ 
        float etMin() const { return m_etmin; };
        
        /*! Get the Et high edge */
        float etMax() const { return m_etmax; };
        
        /*! Get the Eta lower edge */ 
        float etaMin() const { return m_etamin; };

        /*! Get the Eta high edge */
        float etaMax() const { return m_etamax; };
        
        /*! Calculate the disriminant */
        float predict ( std::vector< std::vector<float> > & ) const;
    

      private:

        // onnx session
        std::shared_ptr<Ort::Session> m_session;
        std::vector<int64_t>          m_input_node_dims;
        std::vector<int64_t>          m_output_node_dims;
        std::vector<const char*>      m_input_node_names;
        std::vector<const char*>      m_output_node_names;
 
        float m_etmin, m_etmax;
        float m_etamin, m_etamax;
    };

  } // namepace onnx

}// namespace ringer

#endif
