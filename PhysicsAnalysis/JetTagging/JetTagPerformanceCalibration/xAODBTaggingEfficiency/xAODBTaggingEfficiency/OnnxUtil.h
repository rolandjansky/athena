/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ONNXUTIL_H
#define ONNXUTIL_H

#include <core/session/onnxruntime_cxx_api.h>


class OnnxUtil final{

    public:

    // Constructor/destructor/init
    OnnxUtil(const std::string& name);
    ~OnnxUtil();

    void initialize();
    
    // for fixed cut wp
    void runInference(
        const std::vector<std::vector<float>> & node_feat,
        std::vector<float>& effAllJet
    ) const;

    // for continuous wp
    void runInference(
        const std::vector<std::vector<float>> & node_feat,
        std::vector<std::vector<float>>& effAllJetAllWp
    ) const;

    private:

    std::vector<std::string> m_input_node_names;
    std::vector<std::string> m_output_node_names;
    std::unique_ptr< Ort::Session > m_session;
    std::unique_ptr< Ort::Env > m_env;

    std::string m_path_to_onnx;
    
    // num_wp=1 for fixed cut;
    int m_num_wp;
    
}; // Class OnnxUtil


#endif //ONNXUTIL_H
