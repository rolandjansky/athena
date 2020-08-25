/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/CaloMuonScoreTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "PathResolver/PathResolver.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrkParameters/TrackParameters.h"
#include "ParticleCaloExtension/ParticleCellAssociationCollection.h"

#include <string>
#include <iostream>
#include <cmath>
#include <map>

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool constructor
///////////////////////////////////////////////////////////////////////////////
CaloMuonScoreTool::CaloMuonScoreTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  AthAlgTool(type,name,parent)
{
  declareInterface<ICaloMuonScoreTool>(this);
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonScoreTool::initialize() {
  ATH_MSG_INFO("Initializing " << name());
  
  ATH_CHECK( m_svc.retrieve() );
  ATH_CHECK(m_caloCellAssociationTool.retrieve());
  
  if (m_modelFileName.empty()) {
    ATH_MSG_FATAL("Could not find an ONNX model file!");
    return StatusCode::FAILURE;
  }

  // initialise session
  Ort::SessionOptions session_options;
  Ort::AllocatorWithDefaultOptions allocator;  
  session_options.SetIntraOpNumThreads(1);
  session_options.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );

  const std::string model_file_name = PathResolverFindDataFile( m_modelFileName );

  m_session = std::make_unique< Ort::Session > (m_svc->env(), model_file_name.c_str(), session_options);

  ATH_MSG_INFO("Created ONNX runtime session with model " << model_file_name);

  size_t num_input_nodes = m_session->GetInputCount();
  m_input_node_names.resize(num_input_nodes);

  for( std::size_t i = 0; i < num_input_nodes; i++ ) {
    // print input node names
    char* input_name = m_session->GetInputName(i, allocator);
    ATH_MSG_INFO("Input "<<i<<" : "<<" name= "<<input_name);
    m_input_node_names[i] = input_name;
    // print input node types
    Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType type = tensor_info.GetElementType();
    ATH_MSG_INFO("Input "<<i<<" : "<<" type= "<<type);
    
    // print input shapes/dims
    m_input_node_dims = tensor_info.GetShape();
    ATH_MSG_INFO("Input "<<i<<" : num_dims= "<<m_input_node_dims.size());
    for (std::size_t j = 0; j < m_input_node_dims.size(); j++){
      if(m_input_node_dims[j]<0)
	m_input_node_dims[j] =1;
      ATH_MSG_INFO("Input "<<i<<" : dim "<<j<<"= "<<m_input_node_dims[j]);
    }  
  }


  //output nodes
  std::vector<int64_t> output_node_dims;
  size_t num_output_nodes = m_session->GetOutputCount();
  ATH_MSG_INFO("Have output nodes " << num_output_nodes);
  m_output_node_names.resize(num_output_nodes);

  for( std::size_t i = 0; i < num_output_nodes; i++ ) {
    // print output node names
    char* output_name = m_session->GetOutputName(i, allocator);
    ATH_MSG_INFO("Output "<<i<<" : "<<" name= "<<output_name);
    m_output_node_names[i] = output_name;
    
    Ort::TypeInfo type_info = m_session->GetOutputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
    ONNXTensorElementDataType type = tensor_info.GetElementType();
    ATH_MSG_INFO("Output "<<i<<" : "<<" type= "<<type);
    
    // print output shapes/dims
    output_node_dims = tensor_info.GetShape();
    ATH_MSG_INFO("Output "<<i<<" : num_dims= "<<output_node_dims.size());
    for (std::size_t j = 0; j < output_node_dims.size(); j++){
      if(output_node_dims[j]<0)
	output_node_dims[j] =1;
      ATH_MSG_INFO("Output"<<i<<" : dim "<<j<<"= "<<output_node_dims[j]);
    }  
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::fillInputVectors
///////////////////////////////////////////////////////////////////////////////
void CaloMuonScoreTool::fillInputVectors(std::unique_ptr<const Rec::ParticleCellAssociation>& association, std::vector<float> &eta, std::vector<float> &phi, std::vector<float> &energy, std::vector<int> &samplingId) const {
  int cell_count = 0;

  for( auto cluster: association->data()){
    eta.push_back(cluster->eta());
    phi.push_back(cluster->phi());
    samplingId.push_back(cluster->caloDDE()->getSampling());
    energy.push_back(cluster->energy());
    
    cell_count++;
  }

  ATH_MSG_DEBUG("Iterated over " << cell_count << " calo cells");

  return;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getMuonScore
///////////////////////////////////////////////////////////////////////////////
float CaloMuonScoreTool::getMuonScore( const xAOD::TrackParticle* trk ) const {  
  ATH_MSG_DEBUG("in CaloMuonScoreTool::getMuonScore()");

  double track_eta = trk->eta();

  // calculate muon score at all eta values
  if(std::abs(track_eta) > m_CaloMuonEtaCut){
    ATH_MSG_DEBUG("Skip calculation of muon score for track particle due to failed eta cut of " << m_CaloMuonEtaCut << " (eta="<<track_eta<<")");
    return -1;
  }

  ATH_MSG_DEBUG("Calculating muon score for track particle with eta="<<track_eta);

  // - associate calocells to trackparticle, cone size 0.2, use cache
  std::unique_ptr<const Rec::ParticleCellAssociation> association = m_caloCellAssociationTool->particleCellAssociation(*trk,0.2,nullptr);
  if(!association){
    ATH_MSG_VERBOSE("Could not get particleCellAssociation");
    return -1.;
  }
  ATH_MSG_VERBOSE(" particleCellAssociation done  " << association.get() );

  // create input vectors from calo cell association
  std::vector<float> eta, phi, energy;
  std::vector<int> sampling;
  fillInputVectors(association, eta, phi, energy, sampling);

  // create tensor from vectors
  std::vector<float> inputTensor = getInputTensor(eta, phi, energy, sampling);

  // run inference on input tensor
  float outputScore = runOnnxInference(inputTensor);
  ATH_MSG_DEBUG("Computed CaloMuonScore: " << outputScore);

  return outputScore;
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::runOnnxInference
///////////////////////////////////////////////////////////////////////////////
float CaloMuonScoreTool::runOnnxInference( std::vector<float> &tensor ) const {  
  // create input tensor object from data values
  ATH_MSG_DEBUG("in CaloMuonScoreTool::runOnnxInference()");
  
  auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  int input_tensor_size(m_etaBins * m_phiBins * m_nChannels);
  Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, tensor.data(), input_tensor_size, m_input_node_dims.data(), m_input_node_dims.size());

  // score model & input tensor, get back output tensor

  auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, m_input_node_names.data(), &input_tensor, m_input_node_names.size(), m_output_node_names.data(), m_output_node_names.size());

  // Get pointer to output tensor float values
  float* output_score_array = output_tensors.front().GetTensorMutableData<float>();

  // Binary classification - the score is just the first element of the output tensor
  float output_score = output_score_array[0];
  
  return output_score;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::channelForSamplingId
///////////////////////////////////////////////////////////////////////////////
int CaloMuonScoreTool::channelForSamplingId(int &samplingId) const{
  //List of 7 central calo sampling IDs: [0,1,2,3,12,13,14]
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
float CaloMuonScoreTool::getMedian(std::vector<float> vec) const{
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
int CaloMuonScoreTool::getBin(std::vector<float> &bins, float &val) const{
  // return -1 if value is outside the range
  if(val < bins.front()){
    return -1;
  }

  if(val > bins.back()){
    return -1;
  }
  
  for(long unsigned int i = 0; i < bins.size(); i++) {
    if(val < bins[i]) return i;
  }

  return -1;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getLinearlySpacedBins
///////////////////////////////////////////////////////////////////////////////
std::vector<float> CaloMuonScoreTool::getLinearlySpacedBins(float min, float max, int nBins) const{
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
std::vector<float> CaloMuonScoreTool::getInputTensor(std::vector<float> &eta, std::vector<float> &phi, std::vector<float> &energy, std::vector<int> &sampling) const {
  int n_cells = eta.size();

  float median_eta = getMedian(eta);
  float median_phi = getMedian(phi);

  // initialise output matrix of zeros
  std::vector<float> tensor(m_etaBins * m_phiBins * m_nChannels, 0.);

  std::vector<float> eta_bins = getLinearlySpacedBins(-m_etaCut, m_etaCut, m_etaBins);
  std::vector<float> phi_bins = getLinearlySpacedBins(-m_phiCut, m_phiCut, m_phiBins);

  int skipped_cells = 0;

  for(int i = 0; i < n_cells; i++ ){
    // take eta and phi values, and shift them by their repsective median
    float shifted_eta = eta[i] - median_eta;
    float shifted_phi = phi[i] - median_phi;

    int eta_bin = getBin(eta_bins, shifted_eta); 
    int phi_bin = getBin(phi_bins, shifted_phi);

    // the cell lies outside the acceptable range
    if(eta_bin == -1 || phi_bin == -1){
      skipped_cells++;
      ATH_MSG_DEBUG("Skipping cell because eta or phi bin lies outside of range. Eta bin: " << eta_bin << " phi bin: " << phi_bin);
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


