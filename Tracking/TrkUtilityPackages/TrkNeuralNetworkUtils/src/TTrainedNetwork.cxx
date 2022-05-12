/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include <iostream>
#include <set>
#include <limits>
#include <numeric>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include "TargetBuffer_t.h"
#include "TargetBuffer_t.icc"

TTrainedNetwork::TTrainedNetwork()
{
  mnInput=0;
  mnHidden=0;
  mnOutput=0;
  mActivationFunction=1;
  mLinearOutput=false;
  mNormalizeOutput=false;
  maxExpValue=log(std::numeric_limits<double>::max());
  m_bufferSizeMax=0;
}

TTrainedNetwork::TTrainedNetwork(Int_t nInput, 
				 Int_t nHidden, 
                                 Int_t nOutput,
				 std::vector<Int_t> & nHiddenLayerSize, 
                                 std::vector<TVectorD*> & thresholdVectors,
                                 std::vector<TMatrixD*> & weightMatrices,
                                 Int_t activationFunction,
                                 bool linearOutput,
                                 bool normalizeOutput)
{
  assert(nInput >= 0); 
  assert(nOutput >= 0); 
  assert(nHidden >= 0); 
  mnInput = nInput; 
  mnHidden = nHidden; 
  mnOutput = nOutput;
  mnHiddenLayerSize = nHiddenLayerSize; 
  mThresholdVectors = thresholdVectors;
  mWeightMatrices = weightMatrices;
  mActivationFunction = activationFunction;
  mLinearOutput = linearOutput;
  mNormalizeOutput = normalizeOutput;
  maxExpValue = log(std::numeric_limits<double>::max());

  auto maxElement = *(std::max_element(mnHiddenLayerSize.begin(), mnHiddenLayerSize.end()));
  int nlayer_max = std::max(static_cast<int>(mnOutput), maxElement);
  //
  std::vector<TVectorD*>::const_iterator hidden_layer_threshold_vector_end = mThresholdVectors.end();
  --hidden_layer_threshold_vector_end;
  //
  for (std::vector<TVectorD*>::const_iterator tr_itr= mThresholdVectors.begin(); tr_itr != hidden_layer_threshold_vector_end; ++tr_itr){
     nlayer_max = std::max(nlayer_max, (*tr_itr)->GetNrows());
  }
  m_bufferSizeMax=nlayer_max;
}

void TTrainedNetwork::setOffsets(const std::vector<double>& offsets) 
{ 
  assert(check_norm_size(offsets.size())); 
  m_input_node_offset = offsets; 
}
void TTrainedNetwork::setScales(const std::vector<double>& scales) 
{ 
  assert(check_norm_size(scales.size())); 
  m_input_node_scale = scales; 
}

TTrainedNetwork::TTrainedNetwork(std::vector<TTrainedNetwork::Input> inputs, 
			       unsigned nOutput,
			       std::vector<TVectorD*> & thresholdVectors,
			       std::vector<TMatrixD*> & weightMatrices,
			       ActivationFunction activationFunction,
			       unsigned options)
{
  mnInput = inputs.size(); 
  mnHidden = thresholdVectors.size() - 1;
  mnOutput = nOutput;
  mThresholdVectors = thresholdVectors;
  mWeightMatrices = weightMatrices;
  mActivationFunction = activationFunction;
  mLinearOutput = options & linearOutput;
  mNormalizeOutput = options & normalizeOutput;
  maxExpValue = log(std::numeric_limits<double>::max());

  std::vector<TVectorD*>::const_iterator hidden_layer_threshold_vector_end = 
    mThresholdVectors.end(); 
  --hidden_layer_threshold_vector_end; 

  for (std::vector<TVectorD*>::const_iterator tr_itr 
	 = mThresholdVectors.begin(); 
       tr_itr != hidden_layer_threshold_vector_end; 
       ++tr_itr){ 
    mnHiddenLayerSize.push_back((*tr_itr)->GetNrows()); 
  }

  unsigned node_n = 0; 
  for (std::vector<Input>::const_iterator itr = inputs.begin(); 
       itr != inputs.end(); 
       ++itr) { 
    m_input_node_offset.push_back(itr->offset); 
    m_input_node_scale.push_back(itr->scale); 
    if (!itr->name.empty()) { 
      m_inputStringToNode[itr->name] = node_n; 
    }
    node_n++; 
  }

  unsigned n_node = node_n; 
  assert(n_node == m_input_node_offset.size()); 
  assert(n_node == m_input_node_scale.size()); 

  // mapping should either be unique or non-existent
  unsigned n_mapped = m_inputStringToNode.size(); 
  if (n_node != n_mapped && n_mapped != 0) { 
    throw std::runtime_error("Names for NN inputs must be unique (if given)");
  }

  int nlayer_max(mnOutput);
  for (unsigned i = 0; i < mnHiddenLayerSize.size(); ++i)
    nlayer_max = std::max(nlayer_max, mnHiddenLayerSize[i]);
  m_bufferSizeMax=nlayer_max;

  unsigned n_zero = std::count(m_input_node_scale.begin(), 
			       m_input_node_scale.end(), 0); 
  if (n_zero == n_node) {
    m_input_node_scale.clear(); 
    m_input_node_offset.clear(); 
  }

  assert(is_consistent()); 
}

TTrainedNetwork::~TTrainedNetwork()
{
  std::vector<TVectorD*>::const_iterator vectBegin=mThresholdVectors.begin();
  std::vector<TVectorD*>::const_iterator vectEnd=mThresholdVectors.end();

  for (std::vector<TVectorD*>::const_iterator vectIter=vectBegin;
       vectIter!=vectEnd;
       ++vectIter)
  {
    delete *vectIter;
  }

  std::vector<TMatrixD*>::const_iterator matrixBegin=mWeightMatrices.begin();
  std::vector<TMatrixD*>::const_iterator matrixEnd=mWeightMatrices.end();

  for (std::vector<TMatrixD*>::const_iterator matrixIter=matrixBegin;
       matrixIter!=matrixEnd;
       ++matrixIter)
  {
    delete *matrixIter;
  }

}

std::vector<TTrainedNetwork::Input> TTrainedNetwork::getInputs() const { 

  assert(m_inputStringToNode.size() == 0 || 
	 m_inputStringToNode.size() == mnInput); 
  assert(m_input_node_scale.size() == m_input_node_offset.size()); 

  std::map<int,std::string> input_n_to_name; 
  for (std::map<std::string,int>::const_iterator 
	 itr = m_inputStringToNode.begin(); 
       itr != m_inputStringToNode.end(); ++itr){ 
    input_n_to_name[itr->second] = itr->first; 
  }

  std::vector<Input> inputs_vector; 
  if (m_input_node_offset.size() != mnInput) { 
    return inputs_vector; 
  }
  for (unsigned input_n = 0; input_n < mnInput; input_n++){ 
    std::map<int,std::string>::const_iterator 
      name_itr = input_n_to_name.find(input_n); 
    Input the_input; 
    if (name_itr != input_n_to_name.end()) { 
      the_input.name = name_itr->second; 
    }
    the_input.offset = m_input_node_offset.at(input_n); 
    the_input.scale = m_input_node_scale.at(input_n);
    inputs_vector.push_back(the_input); 
  }
  return inputs_vector; 
}

void TTrainedNetwork::setNewWeights(std::vector<TVectorD*> & thresholdVectors,
				    std::vector<TMatrixD*> & weightMatrices)
{

  std::vector<TVectorD*>::const_iterator vectBegin=mThresholdVectors.begin();
  std::vector<TVectorD*>::const_iterator vectEnd=mThresholdVectors.end();

  for (std::vector<TVectorD*>::const_iterator vectIter=vectBegin;
       vectIter!=vectEnd;
       ++vectIter)
  {
    delete *vectIter;
  }

  std::vector<TMatrixD*>::const_iterator matrixBegin=mWeightMatrices.begin();
  std::vector<TMatrixD*>::const_iterator matrixEnd=mWeightMatrices.end();

  for (std::vector<TMatrixD*>::const_iterator matrixIter=matrixBegin;
       matrixIter!=matrixEnd;
       ++matrixIter)
  {
    delete *matrixIter;
  }

  mThresholdVectors.clear();
  mWeightMatrices.clear();

  mThresholdVectors=thresholdVectors;
  mWeightMatrices=weightMatrices;

}

std::vector<Double_t> 
TTrainedNetwork::calculateNormalized(const TTrainedNetwork::DMap& in) 
  const { 

  std::vector<Double_t> inputs(mnInput); 
  size_t n_filled = 0;
  for (std::map<std::string,double>::const_iterator itr = in.begin(); 
       itr != in.end(); 
       ++itr){ 
    std::map<std::string,int>::const_iterator input_node_ptr = 
      m_inputStringToNode.find(itr->first); 
    if (input_node_ptr == m_inputStringToNode.end()) { 
      throw std::runtime_error(itr->first + "not found in NN"); 
    }

    const int node_n = input_node_ptr->second; 

    // get and scale the raw input value
    double raw_value = itr->second; 
    raw_value += m_input_node_offset.at(node_n); 
    raw_value *= m_input_node_scale.at(node_n); 

    // store in the inputs vector
    inputs.at(node_n) = raw_value; 
    n_filled++; 
  }

  // make sure all nodes are filled
  if (n_filled != m_inputStringToNode.size() ) { 
    assert(n_filled < m_inputStringToNode.size() ); 
    std::set<std::string> input_set;
    for (DMapI itr = in.begin(); itr != in.end(); itr++) { 
      input_set.insert(itr->first); 
    }
    std::string err = "nodes not filled in NN: "; 
    for (std::map<std::string,int>::const_iterator itr = 
	   m_inputStringToNode.begin(); 
	 itr != m_inputStringToNode.end(); 
	 ++itr){
      if (input_set.find(itr->first) == input_set.end() ) 
	err.append(itr->first + " "); 
    }
    throw std::runtime_error(err); 
  }
  return calculateOutputValues(inputs); 
}

std::vector<Double_t>
TTrainedNetwork::calculateNormalized(const TTrainedNetwork::DVec& input)
  const 
{
  // asserts can be turned off in optomized code anyway, 
  // use them to be safe without having to call vector.at()
  assert(mnInput == input.size()); 
  assert(mnInput == m_input_node_scale.size()); 
  assert(mnInput == m_input_node_offset.size()); 
  std::vector<double> transformed_inputs(input); 
  for (unsigned input_n = 0; input_n < mnInput; input_n++) { 
    transformed_inputs[input_n] += m_input_node_offset[input_n]; 
    transformed_inputs[input_n] *= m_input_node_scale[input_n]; 
  }
  return calculateOutputValues(transformed_inputs); 
}
std::vector<Double_t>  
TTrainedNetwork::calculateOutputValues(const std::vector<Double_t>& input) 
  const 
{
  // This method is now highly optimised (apart from the potential use
  // of a cheaper sigmoid function). Please be very careful changing
  // anything here since it is used heavily in reconstruction during
  // Pixel clusterization - Thomas Kittelmann, Oct 2011.


  if (input.size() != mnInput)
  {
    std::cerr << "TTrainedNetwork WARNING Input size: " << input.size()
	      << " does not match with network: " << mnInput << std::endl;
    return {};
  }

  TTN::DoubleBuffer_t tmp_array;
  tmp_array.clear(m_bufferSizeMax);          // make sure it is big enough and initialise with zero

  const unsigned nTargetLayers(mnHidden+1);
  const unsigned lastTargetLayer(mnHidden);
  unsigned nSource = mnInput, nTarget(0);
  TTN::ConstBuffer_t source(input);
  const double * weights(nullptr);
  const double * thresholds(nullptr);
  double nodeVal(0);

  for (unsigned iLayer = 0; iLayer < nTargetLayers; ++iLayer) {
    //Find data area for target layer:
    nTarget = ( iLayer == lastTargetLayer ? 
		mnOutput : 
		mnHiddenLayerSize[iLayer] );
    TTN::Buffer_t target( tmp_array[iLayer] );

    //Transfer the input nodes to the output nodes in this layer transition:
    weights = mWeightMatrices[iLayer]->GetMatrixArray();
    thresholds = mThresholdVectors[iLayer]->GetMatrixArray();
    for (unsigned inodeTarget=0;inodeTarget<nTarget;++inodeTarget) {
      nodeVal = 0.0;//Better would be "nodeVal = *thresholds++" and
		    //remove the line further down, but this way we
		    //get exactly the same results that an earlier
		    //version of the package gave.
      const double * weights_tmp = weights++;
      const double * source_end(&(source.upper_bound_at(nSource)));
      for (const double* source_iter = &source[0];
	   source_iter != source_end; ++source_iter)
	{
	  nodeVal += (*weights_tmp) * (*source_iter);
	  weights_tmp += nTarget;
	}
      nodeVal += *thresholds++;//see remark above.
      target[inodeTarget] = ( mLinearOutput && iLayer == lastTargetLayer )
	                    ? nodeVal : sigmoid(nodeVal);
    }
    //Prepare for next layer transition:
    source = target;
    nSource = nTarget;
  }

  //std::vector<double> result(nTarget);
  if (!mNormalizeOutput) {
    //    std::memcpy(&result[0], target, sizeof(*target)*nTarget);
    // the result is already in the buffer half with index (nTargetLayers-1)%2
    // copy this to the front of the full buffer and shrink the array
    return tmp_array.releaseData(nTarget,(nTargetLayers-1));
  } else {
    // take the other half buffer to store the normalised output
    TTN::Buffer_t norm_target=tmp_array[nTargetLayers];
    TTN::Buffer_t target=tmp_array[(nTargetLayers-1)];
    const double sumLastLayer = 
      std::accumulate(&target[0], &target[nTarget], 0.0 );
    const double normFact = sumLastLayer ? 1.0/sumLastLayer : 0.0;
    for (unsigned i = 0; i < nTarget; ++i)
      norm_target[i] = normFact * target[i];
    // copy the half buffer to the front of the full buffer
    // if necessary and shrink the array
    return tmp_array.releaseData(nTarget,nTargetLayers);
  }
  
}


Double_t TTrainedNetwork::sigmoid(Double_t x) const { 
  if (-2*x >= maxExpValue){
    return 0.;
  }
  return 1./(1.+exp(-2*x)); 
}

bool TTrainedNetwork::is_consistent() const { 
  if (mThresholdVectors.size() != mWeightMatrices.size()) { 
    std::cerr << "ERROR: " 
	      << "n threshold vectors: " << mThresholdVectors.size() 
	      << " n weight matrices: " << mWeightMatrices.size() 
	      << std::endl; 
    return false; 
  }
  int nodes_last_layer = mnInput; 
  for (unsigned layer_n = 0; layer_n < mThresholdVectors.size(); layer_n++){ 
    int n_threshold_nodes = mThresholdVectors.at(layer_n)->GetNrows(); 
    int n_weights_nodes = mWeightMatrices.at(layer_n)->GetNcols(); 
    if (n_threshold_nodes != n_weights_nodes) {
      std::cerr << "ERROR: in layer " << layer_n 
		<< " --- n threshold: " << n_threshold_nodes
		<< " n_weights: " << n_weights_nodes << std::endl;
      return false; 
    }
    int n_incoming_connections = mWeightMatrices.at(layer_n)->GetNrows(); 
    if (n_incoming_connections != nodes_last_layer) { 
      std::cerr << "ERROR: in layer " << layer_n 
		<< " --- last layer nodes: " << nodes_last_layer
		<< " connected to this layer: " <<  n_incoming_connections
		<< std::endl;
      return false; 
    }
    nodes_last_layer = n_weights_nodes; 
  }
  
  if (mThresholdVectors.size() - 1 != mnHiddenLayerSize.size() ){ 
    std::cerr << "ERROR: "
	      << "size mThresholdVectors: " << mThresholdVectors.size() 
	      << " size mnHiddenLayerSize: " << mnHiddenLayerSize.size()
	      << std::endl; 
    return false; 
  }

  return true; 
}

bool TTrainedNetwork::check_norm_size(unsigned size) const { 
  if (size != mnInput) { 
    std::cerr << "ERROR: TTrainedNetwork has " << mnInput << " inputs, "
	      << size << " normalization values provided\n"; 
    return false; 
  }
  return true; 
}

ClassImp( TTrainedNetwork)



