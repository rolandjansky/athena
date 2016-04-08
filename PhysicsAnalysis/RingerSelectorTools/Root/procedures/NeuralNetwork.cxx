/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NeuralNetwork.cxx 704615 2015-10-29 18:50:12Z wsfreund $
#include "RingerSelectorTools/procedures/NeuralNetwork.h"

// STL includes:
#include <vector>
#include <cmath>
#include <exception>
#include <algorithm>

//#undef NDEBUG

//#include <boost/python/stl_iterator.hpp>

namespace Ringer {

//template< typename T >
//inline
//std::vector< T > to_std_vector( const boost::python::object& iterable )
//{
//  return std::vector< T >( boost::python::stl_input_iterator< T >( iterable ),
//      boost::python::stl_input_iterator< T >( ) );
//}

namespace Discrimination {

// =============================================================================
NNFeedForward::NNFeedForward(
    const std::vector<unsigned int> &n, 
    const std::vector<float> &w, 
    const std::vector<float> &b)
  : m_wM(nullptr),
    m_bM(nullptr),
    m_mM(nullptr)
{
  changeArchiteture(n,w,b);
}


// =============================================================================
// void NNFeedForward::changeArchiteture(
//     boost::python::list &n, 
//     boost::python::list &w, 
//     boost::python::list &b)
// {
//   changeArchiteture(to_std_vector<unsigned int>(n), 
//       to_std_vector<float>(w),
//       to_std_vector<float>(b));
// }

// =============================================================================
void NNFeedForward::changeArchiteture(
    const std::vector<unsigned int> &n, 
    const std::vector<float> &w, 
    const std::vector<float> &b)
{

  if ( n.empty() ) {
    throw std::invalid_argument(
        std::string("Invalid node argument (empty)."));
  }
  //Verifying weight vector size
  unsigned int wSize=0;
  for (unsigned int k=0; k<n.size()-1; ++k) {
    wSize+=n[k]*n[k+1];
  }
  if (wSize != w.size() || !w.size() ) {
    throw std::invalid_argument(
      std::string("Invalid weight argument."));
  }
  //Verifying bias vector size
  unsigned int bSize=0;
  for (unsigned int k=1; k<n.size(); ++k) {
    bSize+=n[k];
  }
  if(bSize != b.size() || !b.size() ) {
    throw std::invalid_argument(
      std::string("Invalid bias argument."));
  }

  // Configuration is ok, copy it:
  m_nodes = n;
  m_nLayers = n.size();

  // Release possible older architeture:
  releaseWeights();
  releaseBias();
  releaseMMatrix();

  //First weight dimension
  m_wM = new float **[m_nLayers-1]; //number of layers excluding input
  //First bias dimension
  m_bM = new float *[m_nLayers-1]; //number of layers excluding input
  //First multiplication dimension
  m_mM = new float *[m_nLayers]; //number of layers including input

  for (unsigned int l = 0; l<m_nLayers; ++l){ 
    m_mM[l] = new float[n[l]]; //number of m_nodes in current layer
  }
  std::vector<float>::const_iterator itrB = b.begin();
  std::vector<float>::const_iterator itrW = w.begin();
  for (unsigned int l = 0; l < m_nLayers-1; ++l){  
    //Second and last dimension of m_bM
    m_bM[l] = new float[n[l+1]]; //number of m_nodes in next layer
    //Second dimension of m_wM
    m_wM[l] = new float*[n[l+1]]; //number of m_nodes in next layer
    for (unsigned int i=0; i<n[l+1]; i++){ 
      //Third and last dimension of m_wM
      m_wM[l][i]=new float [n[l]]; //number of m_nodes in current layer
      //Populating bias matrix
      m_bM[l][i]=(*itrB++);
      //Populating weight matrix
      for (unsigned int j=0; j<n[l]; j++){
        m_wM[l][i][j]=(*itrW++);
      }
    }
    //Populating multiplication matrix so that starting sum equals zero
    for (unsigned int i=0; i<n[l]; i++){
      m_mM[l][i]=0;
    }   
  }
}

// =============================================================================
void NNFeedForward::releaseBias(){
  if (m_bM){
    for (unsigned int l=0; l<m_nodes.size()-1; ++l){
      delete[] m_bM[l]; //Deletes array of values at second dimension of m_bM
    } 
  }
  delete[] m_bM; //Deletes array of pointers at first dimension of m_bM
  m_bM = nullptr;
}

// =============================================================================
void NNFeedForward::releaseWeights(){
  if (m_wM){ 
    for (unsigned int l=0; l<m_nodes.size()-1; ++l){
      if (m_wM[l]){
        for (unsigned int i=0; i<m_nodes[l+1]; ++i){
          delete[] m_wM[l][i]; //Deletes array of values at third dimension of m_wM
        }
      }
      delete[] m_wM[l]; // Deletes array of pointers at second dimension of m_wM
    }
  }
  delete[] m_wM; //Deletes array of pointers at first dimension of m_wM
  m_wM = nullptr;
}

// =============================================================================
void NNFeedForward::releaseMMatrix(){
  if(m_mM){
    for (unsigned int l=0; l<m_nodes.size(); ++l){
      delete[] m_mM[l]; //Deletes array of values at second dimension of m_mM
    }
  }
  delete[] m_mM; //Deletes array of pointers at first dimension of m_mM
  m_mM = nullptr;
}


// =============================================================================
NNFeedForward::~NNFeedForward()
{
  releaseWeights();
  releaseBias();
  releaseMMatrix();
}

// =============================================================================
void NNFeedForward::execute(
    const std::vector<float> &input,
    std::vector<float> &output) const 
{
#ifndef NDEBUG
  ATH_MSG_DEBUG("Applying NNFeedForward. Pattern space is: " << input);
#endif
  if ( input.size() != m_nodes[0] ){
    throw std::runtime_error(std::string("Pattern space size (") + 
        std::to_string(input.size()) + ") fed to discriminator is "
        "not compatible with this discriminator pattern space size ("
        + std::to_string(m_nodes[0]) + ".");
  }
  // Copy vector to input layer:
  for(unsigned int i=0; i<input.size();i++){
    m_mM[0][i]=input[i];
  }
  // Propagate:
  for(unsigned int l=0; l<m_nodes.size()-1;l++){
    for(unsigned int k=0; k<m_nodes[l+1]; k++){
      m_mM[l+1][k]=m_bM[l][k];
      for (unsigned int j=0;j<m_nodes[l]; j++){
        m_mM[l+1][k]+=m_mM[l][j]*m_wM[l][k][j];
      }
      m_mM[l+1][k]=tanh(m_mM[l+1][k]);
    }
  }

  // Resize output to hold new entries
  output.clear();

  // Insert result at output vector:
  output.insert( output.begin(),
      m_mM[m_nLayers-1],
      m_mM[m_nLayers-1] + m_nodes[m_nLayers-1]
    );

#ifndef NDEBUG
  ATH_MSG_DEBUG("Output space is: " << output);
#endif
}

// =============================================================================
void NNFeedForward::getWeigthsAndBias(std::vector<float> &weights,
    std::vector<float> &bias) const
{
  weights.clear();
  bias.clear();
  for ( size_t cLayer = 0; cLayer < m_nLayers - 1; ++cLayer ){
    unsigned nCurLayer = m_nodes[cLayer];
    unsigned nNextLayer = m_nodes[cLayer+1];
    for ( unsigned cNextLayer = 0; cNextLayer < nNextLayer; ++cNextLayer ){
      float *weight_ptr = m_wM[cLayer][cNextLayer];
      std::copy( weight_ptr , weight_ptr + nCurLayer, back_inserter(weights));
    }
    float *bias_ptr = m_bM[cLayer];
    std::copy(bias_ptr, bias_ptr + nNextLayer, back_inserter(bias));
  }
}


// =============================================================================
void NNFeedForward::print(MSG::Level lvl) const
{
  if ( !this->isStreamAvailable() ) {
    std::cerr << "Cannot print " << this->name() << ", stream unavailable"
      << std::endl;
  }
  if ( this->level() > lvl ) {
    return;
  }
  std::vector<float> weights, bias;
  getWeigthsAndBias(weights, bias);
  msg() << lvl << "Nodes: " << m_nodes << endreq;
  msg() << static_cast<MSG::Level>((lvl>MSG::VERBOSE)?(lvl-1):(lvl)) 
    << "Vectorized Weights: " << weights << endreq;
  msg() << static_cast<MSG::Level>((lvl>MSG::VERBOSE)?(lvl-1):(lvl)) 
    << "Vectorized Bias: " << bias << endreq;
}

// =============================================================================
void NNFeedForward::read(NNFeedForward *newObj, 
    TDirectory *configDir, 
    unsigned /*version*/ )
{
  std::vector<unsigned int> nodes;
  std::vector<float> weights, bias;
  IOHelperFcns::readVar(configDir, "nodes", nodes); 
  IOHelperFcns::readVar(configDir, "weights", weights); 
  IOHelperFcns::readVar(configDir, "bias", bias); 
  newObj->changeArchiteture(nodes,weights,bias);
}

// =============================================================================
void NNFeedForward::write(TDirectory *configDir, const char *) const
{
  if ( m_nLayers && m_wM && m_bM ){
    std::vector<float> weights, bias;
    getWeigthsAndBias(weights, bias);
    IOHelperFcns::writeVar(configDir, "nodes",   m_nodes );
    IOHelperFcns::writeVar(configDir, "weights", weights );
    IOHelperFcns::writeVar(configDir, "bias",    bias    );
  } else {
    std::runtime_error(std::string("Cannot write configuration from empty" 
          " (or with pointers not allocated) neural network."));
  }
}

} // namespace Discrimination
} // namespace Ringer
