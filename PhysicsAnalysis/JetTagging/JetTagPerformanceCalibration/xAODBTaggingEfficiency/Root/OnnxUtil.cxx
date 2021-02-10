/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>

#include "xAODBTaggingEfficiency/OnnxUtil.h"
#include "PathResolver/PathResolver.h"

// Constructor
OnnxUtil::OnnxUtil(const std::string& name){
    m_path_to_onnx = name;
}

// Destructor
OnnxUtil::~OnnxUtil(){
}

void OnnxUtil::initialize(){

	std::string fullPathToFile = PathResolverFindCalibFile(m_path_to_onnx);

	//load the onnx model to memory using the path m_path_to_onnx
	m_env = std::make_unique< Ort::Env >(ORT_LOGGING_LEVEL_WARNING, "");

	// initialize session options if needed
	Ort::SessionOptions session_options;
	session_options.SetIntraOpNumThreads(1);
	session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

	// create session and load model into memory
	m_session = std::make_unique< Ort::Session >(*m_env, fullPathToFile.c_str(), session_options);
	Ort::AllocatorWithDefaultOptions allocator;

	// get the input nodes
	size_t num_input_nodes = m_session->GetInputCount();
    
	// iterate over all input nodes
	for (std::size_t i = 0; i < num_input_nodes; i++) {
		char* input_name = m_session->GetInputName(i, allocator);
		m_input_node_names.push_back(std::string(input_name));
	}

	// get the output nodes
	size_t num_output_nodes = m_session->GetOutputCount();
    std::vector<int64_t> output_node_dims;

	// iterate over all output nodes
	for(std::size_t i = 0; i < num_output_nodes; i++ ) {
		char* output_name = m_session->GetOutputName(i, allocator);
		m_output_node_names.push_back(std::string(output_name));
    
        // get output node types
        Ort::TypeInfo type_info = m_session->GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        output_node_dims = tensor_info.GetShape();
        
        // output is of the shape {1, num_jets, num_wp}
        m_num_wp = output_node_dims.at(2);
    }
}


// for fixed cut wp
void OnnxUtil::runInference(
    const std::vector<std::vector<float>> & node_feat,
    std::vector<float>& effAllJet) const {

    // Inputs:
    //    node_feat : vector<vector<float>>
    //    effAllJet : vector<double>&

    std::vector<float> input_tensor_values;
    std::vector<int64_t> input_node_dims = {1, static_cast<int>(node_feat.size()), static_cast<int>(node_feat.at(0).size())};

    for (auto it = node_feat.begin(); it != node_feat.end(); ++it){
        input_tensor_values.insert(input_tensor_values.end(), it->begin(), it->end());
    }
        
    // create input tensor object from data values
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(), input_node_dims.data(), input_node_dims.size());

    // casting vector<string> to vector<const char*>. this is what ORT expects
    std::vector<const char*> input_node_names(m_input_node_names.size(),nullptr);
    for (int i=0; i<static_cast<int>(m_input_node_names.size()); i++) {
        input_node_names[i]= m_input_node_names.at(i).c_str();
    }
    std::vector<const char*> output_node_names(m_output_node_names.size(),nullptr);
    for (int i=0; i<static_cast<int>(m_output_node_names.size()); i++) {
        output_node_names[i]= m_output_node_names.at(i).c_str();
    }

    // score model & input tensor, get back output tensor
    auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, input_node_names.size(), output_node_names.data(), output_node_names.size());

    // set the output vector values to the inference results
    float* float_ptr = output_tensors.front().GetTensorMutableData<float>();
    int num_jets = node_feat.size();
    effAllJet = {float_ptr, float_ptr + num_jets};
}


// for continuous wp
void OnnxUtil::runInference(
    const std::vector<std::vector<float>> & node_feat,
    std::vector<std::vector<float>> & effAllJetAllWp) const{

    // Inputs:
    //    node_feat      : vector<vector<float>>
    //    effAllJetAllWp : vector<vector<double>>& shape:{num_jets, num_wp}

    // using float because that's what the model expects
    // ort exectues type casting wrong (x = x.float()), so can't change the type inside the model
    std::vector<float> input_tensor_values;
    std::vector<int64_t> input_node_dims = {1, static_cast<int>(node_feat.size()), static_cast<int>(node_feat.at(0).size())};

    for (auto it = node_feat.begin(); it != node_feat.end(); ++it){
        input_tensor_values.insert(input_tensor_values.end(), it->begin(), it->end());
    }
        
    // create input tensor object from data values
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(), input_node_dims.data(), input_node_dims.size());
    
    // casting vector<string> to vector<const char*>. this is what ORT expects
    std::vector<const char*> input_node_names(m_input_node_names.size(),nullptr);
    for (int i=0; i<static_cast<int>(m_input_node_names.size()); i++) {
        input_node_names[i]= m_input_node_names.at(i).c_str();
    }
    std::vector<const char*> output_node_names(m_output_node_names.size(),nullptr);
    for (int i=0; i<static_cast<int>(m_output_node_names.size()); i++) {
        output_node_names[i]= m_output_node_names.at(i).c_str();
    }

    // score model & input tensor, get back output tensor
    auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, input_node_names.size(), output_node_names.data(), output_node_names.size());
    
    // set the output vector values to the inference results
    float* float_ptr = output_tensors.front().GetTensorMutableData<float>();

    int num_jets = node_feat.size();

    for (int i=0; i<num_jets; i++){
        std::vector<float> eff_one_jet_tmp;
        for (int j=0; j<m_num_wp; j++){
            eff_one_jet_tmp.push_back(float_ptr[i*m_num_wp+j]);
        }
        effAllJetAllWp.push_back(eff_one_jet_tmp);
    }
}

