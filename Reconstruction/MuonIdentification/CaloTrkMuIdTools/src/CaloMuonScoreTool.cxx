/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/CaloMuonScoreTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "PathResolver/PathResolver.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"
#include "TrkParameters/TrackParameters.h"

#include "TFile.h"
#include "TH1F.h"

#include <string>
#include <iostream>
#include <cmath>
#include <map>

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool constructor
///////////////////////////////////////////////////////////////////////////////
CaloMuonScoreTool::CaloMuonScoreTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  AthAlgTool(type,name,parent),
  m_modelFileName("CaloMuonCNN_0.onnx");
{
  declareInterface<ICaloMuonScoreTool>(this);  
  declareProperty("ModelFileName", m_modelFileName);
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonScoreTool::initialize() {
  ATH_MSG_INFO("Initializing " << name());

  ATH_CHECK(m_caloExtensionTool.retrieve());
  
  if (m_modelFileName.empty()) {
    ATH_MSG_FATAL("Could not find an ONNX model file!");
    return StatusCode::FAILURE;
  }

  // initialise session
  Ort::Env env(ORT_LOGGING_LEVEL_INFO, "CaloMuonScoreTool ONNX inference session");
  Ort::SessionOptions session_options;
  Ort::AllocatorWithDefaultOptions allocator;  
  session_options.SetIntraOpNumThreads(1);
  session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

  // declared in header, so we should not need this
  // std::unique_ptr<Ort::Session> m_session;

  m_session = std::make_unique< Ort::Session > (env, ModelFileName, session_options);

  ATH_MSG_INFO("Created ONNX runtime session");
  
  std::vector<int64_t> input_node_dims;
  size_t num_input_nodes = m_session->GetInputCount();
  std::vector<const char*> input_node_names(num_input_nodes);
  for( std::size_t i = 0; i < num_input_nodes; i++ ) {
    // print input node names
    char* input_name = m_session->GetInputName(i, allocator);
    ATH_MSG_DEBUG(<<"Input "<<i<<" : "<<" name= "<<input_name);
    input_node_names[i] = input_name;
    // print input node types
    Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType type = tensor_info.GetElementType();
    ATH_MSG_DEBUG("Input "<<i<<" : "<<" type= "<<type);
    
    // print input shapes/dims
    input_node_dims = tensor_info.GetShape();
    ATH_MSG_DEBUG("Input "<<i<<" : num_dims= "<<input_node_dims.size());
    for (std::size_t j = 0; j < input_node_dims.size(); j++){
      if(input_node_dims[j]<0)
	input_node_dims[j] =1;
      ATH_MSG_DEBUG(<<"Input "<<i<<" : dim "<<j<<"= "<<input_node_dims[j]);
    }  
  }

  //output nodes
  std::vector<int64_t> output_node_dims;
  size_t num_output_nodes = m_session->GetOutputCount();
  std::vector<const char*> output_node_names(num_output_nodes);
  
  for( std::size_t i = 0; i < num_output_nodes; i++ ) {
    // print output node names
    char* output_name = m_session->GetOutputName(i, allocator);
    ATH_MSG_DEBUG("Output "<<i<<" : "<<" name= "<<output_name);
    output_node_names[i] = output_name;
    
    Ort::TypeInfo type_info = m_session->GetOutputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType type = tensor_info.GetElementType();
    ATH_MSG_DEBUG("Output "<<i<<" : "<<" type= "<<type);
    
    // print output shapes/dims
    output_node_dims = tensor_info.GetShape();
    ATH_MSG_DEBUG("Output "<<i<<" : num_dims= "<<output_node_dims.size());
    for (std::size_t j = 0; j < output_node_dims.size(); j++){
      if(output_node_dims[j]<0)
	output_node_dims[j] =1;
      ATH_MSG_DEBUG("Output"<<i<<" : dim "<<j<<"= "<<output_node_dims[j]);
    }  
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getInputVectors
///////////////////////////////////////////////////////////////////////////////
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::vector<int>> getInputVectors(const Rec::ParticleCellAssociation* association){
  std::vector<float> eta, phi, energy;
  std::vector<int> sampling;

  int cell_count = 0;

  for( auto cluster: association->data()){
    eta->push_back(cluster->eta());
    phi->push_back(cluster->phi());
    sampling->push_back(cluster->caloDDE()->getSampling());
    energy->push_back(cluster->energy());
    
    cell_count++;
  }

  ATH_MSG_DEBUG("Iterated over " << cell_count << " calo cells");

  return std::make_tuple(eta, phi, energy, sampling);
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getMuonScore
///////////////////////////////////////////////////////////////////////////////
float CaloMuonScoreTool::getMuonScore( const xAOD::TrackParticle* trk ) const {  
  ATH_MSG_DEBUG("in CaloMuonScoreTool::getMuonScore()");

  // - associate calocells to trackparticle, cone size 0.2, use cache
  std::unique_ptr<const Rec::ParticleCellAssociation> association = m_caloCellAssociationTool->particleCellAssociation(*tp,0.2,nullptr);
  if(!association){
    ATH_MSG_VERBOSE("Could not get particleCellAssociation");
    return -1.;
  }
  ATH_MSG_VERBOSE(" particleCellAssociation done  " << association.get() );

  // create input vectors from calo cell association
  std::vector<float> eta, phi, energy;
  std::vector<int> sampling;
  std::tie(eta, phi, energy, sampling) = getInputVectors(association);

  // create tensor from vectors
  std::vector<float> inputTensor = getInputTensor(eta, phi, energy, sampling);

  // run inference on input tensor
  float outputScore = runOnnxInference(inputTensor);
  ATH_MSG_INFO("Computed CaloMuonScore: " << outputScore);

  return outputScore;
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::runOnnxInference
///////////////////////////////////////////////////////////////////////////////
float CaloMuonScoreTool::runOnnxInference( std::vector<float> &tensor ) {  
  // create input tensor object from data values
  auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  int input_tensor_size(m_etaBins * m_phiBins * m_nChannels);
  
  Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, tensor.data(), input_tensor_size, input_node_dims.data(), input_node_dims.size());
  assert(input_tensor.IsTensor());

  // score model & input tensor, get back output tensor
  auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, input_node_names.size(), output_node_names.data(), output_node_names.size());
  assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());

  // Get pointer to output tensor float values
  float* output_score_array = output_tensors.front().GetTensorMutableData<float>();

  // Binary classification - the score is just the first element of the output tensor
  float output_score = output_score_array[0];
  
  ATH_MSG_DEBUG("Have output score: " << output_score);
  
  return output_score;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::channelForSamplingId
///////////////////////////////////////////////////////////////////////////////
int channelForSamplingId(int &samplingId){
  //[0,1,2,3,12,13,14]
  switch(samplingId){
  case 0: return 0;
  case 1: return 1;
  case 2: return 2;
  case 3: return 3;
  case 12: return 4;
  case 13: return 5;
  case 14: return 6;
  default: return -1;
  }
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getMedian
///////////////////////////////////////////////////////////////////////////////
float getMedian(std::vector<float> vec){
  std::nth_element( vec.begin(), vec.begin() + vec.size() / 2, vec.end() );

  float val_right = *(vec.begin() + (vec.size() / 2));

  if(vec.size() % 2 == 1){
    return val_right;
  }

  float val_left = *(vec.begin() + (vec.size() / 2 - 1 ));

  return (val_left + val_right) / 2;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getBin
///////////////////////////////////////////////////////////////////////////////
int getBin(std::vector<float> &bins, float &val){
  // return -1 if value is outside the range
  if(val < bins.front()){
    return -1;
  }

  if(val > bins.back()){
    return -1;
  }
  
  for(int i = 0; i < bins.size(); i++) {
    if(val < bins[i]) return i;
  }

  return -1;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getLinearlySpacedBins
///////////////////////////////////////////////////////////////////////////////
std::vector<float> getLinearlySpacedBins(float min, float max, int nBins){
  double h = (max - min) / static_cast<float>(nBins-1);
  std::vector<float> xs(nBins);
  std::vector<float>::iterator x;
  double val;
  for (x = xs.begin(), val = min; x != xs.end(); ++x, val += h) {
    *x = val;
  }
  return xs;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getInputTensor
///////////////////////////////////////////////////////////////////////////////
std::vector<float> getInputTensor(std::vector<float> &eta, std::vector<float> &phi, std::vector<float> &energy, std::vector<int> &sampling){
  int n_cells = eta.size();

  float median_eta = getMedian(eta);
  float median_phi = getMedian(phi);

  ATH_MSG_DEBUG("Median eta value: " << median_eta << ", median phi value: " << median_phi);

  // initialise output matrix of zeros
  std::vector<float> tensor(m_etaBins * m_phiBins * m_nChannels, 0.);
  
  std::vector<float> eta_bins = getLinearlySpacedBins(-m_etaCut, m_etaCut, m_etaBins);
  std::vector<float> phi_bins = getLinearlySpacedBins(-m_phiCut, m_phiCut, m_phiBins);
  
  int skipped_cells = 0;

  for(int i = 0; i < n_cells; i++ ){
    // take eta and phi values, and shift them by their repsective median
    float _eta = eta[i] - median_eta;
    float _phi = phi[i] - median_phi;

    int eta_bin = getBin(eta_bins, _eta); 
    int phi_bin = getBin(phi_bins, _phi);

    // the cell lies outside the acceptable range
    if(eta_bin == -1 || phi_bin == -1){
      skipped_cells++;
      ATH_MSG_DEBUG("Skipping cell because eta or phi bin lies outside of range. Eta bin: " << eta_bin << " phi bin: " << phi_bin);
      ATH_MSG_DEBUG("eta value: " << eta[i] << ", phi value: " << phi[i]);
      ATH_MSG_DEBUG("Median-shifted eta value: " << _eta << ", median-shifted phi value: " << _phi);
      continue;
    }

    int channel = channelForSamplingId(sampling[i]);

    // this really should not happen, but let's skip this cell if it does
    if(channel == -1){
      skipped_cells++;
      ATH_MSG_DEBUG("Skipping cell because sampling ID does not correspond to low-eta layers. Sampling ID: " << sampling[i]);
      continue;
    }

    // 3D array flattening in row-major style: https://en.wikipedia.org/wiki/Row-_and_column-major_order#Explanation_and_example
    int tensor_idx = eta_bin * m_phiBins * m_nChannels + phi_bin * m_nChannels + channel;

    tensor[tensor_idx] += energy[i];
  }

  ATH_MSG_DEBUG("Skipped " << skipped_cells << " out of " << n_cells << " cells");
  
  return tensor;
}


