/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigMultiVarHypo/tools/procedures/MultiLayerPerceptron.h"

#include <cstddef>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;
using namespace Ringer;

MultiLayerPerceptron::MultiLayerPerceptron(std::vector<unsigned int>  &n, 
                                           std::vector<double>        &w, 
                                           std::vector<double>        &b,
                                           std::vector<std::string>   &TF,
                                           double etmin, 
                                           double etmax, 
                                           double etamin, 
                                           double etamax,
                                           double mumin,
                                           double mumax)
  : 
    IModel(etmin,etmax,etamin,etamax,mumin,mumax),
    m_nodes(n),
    m_weights(0),
    m_bias(0),
    m_tfnames(TF)
{

  if ( !n.size() )  throw BAD_BIAS_SIZE; //Nothing to do
  
  //Verifying weight vector size
  unsigned wSize=0;
  for (unsigned k=0; k<n.size()-1; ++k) wSize+=n[k]*n[k+1];
  try{
    if (wSize != w.size() || !w.size() )  throw BAD_WEIGHT_SIZE;
    //Verifying bias vector size
    unsigned bSize=0;
    for (unsigned k=1; k<n.size(); ++k)bSize+=n[k];
    if(bSize != b.size() || !b.size() ) throw BAD_BIAS_SIZE;
  }catch(int i){
    throw;
  }
  //First weight dimension
  try{
    m_weights = new double **[n.size()-1]; //number of layers excluding input
  }catch (const std::bad_alloc &){
    m_weights = nullptr;
    throw;
  }
  //First bias dimension
  try{    
    m_bias = new double *[n.size()-1]; //number of layers excluding input
  }catch (const std::bad_alloc &){
    m_bias = nullptr;
    throw;    
  }


  std::vector<double>::const_iterator itrB = b.begin();
  std::vector<double>::const_iterator itrW = w.begin();
  for (unsigned l = 0; l < n.size()-1; ++l){  
    //Checks if no bad_alloc happened to bias
    if(m_bias){
      try{
        //Second and last dimension of bias
        m_bias[l] = new double[n[l+1]]; //number of nodes in next layer
      } catch (const std::bad_alloc &){
        m_bias[l] = nullptr;
        throw;
      }
    }
    //Checks if no bad_alloc happened to m_weights
    if(m_weights){ 
      try{
        //Second dimension of weights
        m_weights[l] = new double*[n[l+1]]; //number of nodes in next layer
      } catch ( const std::bad_alloc &){
        m_weights[l] = nullptr;
        throw;
      }
    }
    for (unsigned i=0; i<n[l+1]; i++){ 
      //Checks if no bad_alloc happened to weights[l]
      if(m_weights){
        if(m_weights[l]){
          try{
            //Third and last dimension of weights
            m_weights[l][i]=new double [n[l]]; //number of nodes in current layer
          } catch (const std::bad_alloc &){
            m_weights[l][i] = nullptr;
            throw;
          }
        }
      }
      //Populating bias matrix
      if (m_bias){
        //Checks if no bad_alloc happened to bias[l]
        if (m_bias[l]){
            m_bias[l][i]=(*itrB++);
        }
      }
      //Populating weight matrix
      for (unsigned j=0; j<n[l]; j++){
        if (m_weights){
          if (m_weights[l]){
            if(m_weights[l][i]){
              m_weights[l][i][j]=(*itrW++);
            }
          }
        }
      }
    }
  }
}


void MultiLayerPerceptron::release(double** neuronOutputs, double** layerOutputs) const
{

  for (unsigned l=0; l<m_nodes.size(); l++){
    if(layerOutputs){
      if (layerOutputs[l]==nullptr){
        delete layerOutputs[l]; //Deletes null pointer
      }
    }else{
      delete[] layerOutputs[l]; //Deletes array of values at second dimension of layerOutputs
    }
    if(neuronOutputs){
      if (neuronOutputs[l]==nullptr){
        delete neuronOutputs[l]; //Deletes null pointer
      }
    }else{
      delete[] neuronOutputs[l]; //Deletes array of values at second dimension of layerOutputs
    }
  }

  if(layerOutputs==nullptr)
    delete layerOutputs;
  else
    delete[] layerOutputs;

  if(neuronOutputs==nullptr)
    delete neuronOutputs;
  else
    delete[] neuronOutputs;


}




MultiLayerPerceptron::~MultiLayerPerceptron(){

  for (unsigned l=0; l<m_nodes.size()-1; l++){
    if (m_bias){
      if (m_bias[l]==nullptr){
        delete m_bias[l]; //Deletes null pointer
      }else{
        delete[] m_bias[l]; //Deletes array of values at second dimension of bias
      }
    } 
    for (unsigned i=0; i<m_nodes[l+1]; i++){
      if (m_weights){ 
        if(m_weights[l]){
          if (m_weights[l][i]==nullptr){
            delete m_weights[l][i]; //Deletes null pointer
          }else{
            delete[] m_weights[l][i]; //Deletes array of values at third dimension of weights
          }
        }
      }
    }
    if (m_weights){
      if (m_weights[l]==nullptr){
        delete m_weights[l]; //Deletes null pointer
      }else{
        delete[] m_weights[l]; //Deletes array of pointers at second dimension of weights
      }
    } 
  }//Layers


  if (m_weights==nullptr){
    delete m_weights; //Deletes null pointer
  }else{
    delete[] m_weights; //Deletes array of pointers at first dimension of weights
  }

  if (m_bias==nullptr){
    delete m_bias; //Deletes null pointer
  }else{
    delete[] m_bias; //Deletes array of pointers at first dimension of bias
  }
}



Ringer::RnnOutInfo MultiLayerPerceptron::propagate(std::vector<float> &input ) const
{
  double **layerOutputs;
  double **neuronOutputs;
  //this->malloc(neuronOutputs, layerOutputs);

  //First multiplication dimension
  neuronOutputs   = new double *[m_nodes.size()];
  layerOutputs = new double *[m_nodes.size()]; //number of layers including input

  for (unsigned l = 0; l<m_nodes.size(); ++l){ 
    //Second and last dimension of layerOutputs
    layerOutputs[l] = new double[m_nodes[l]]; //number of nodes in current layer
    //Second and last dimension of layerOutputs
    neuronOutputs[l] = new double[m_nodes[l]]; //number of nodes in current layer
  }


  for (unsigned l = 0; l<m_nodes.size()-1; ++l){ 
    for (unsigned i=0; i<m_nodes[l]; i++){ 
      //Populating multiplication matrix so that starting sum equals zero
      if(l==0)
        layerOutputs[0][i]=(double)input.at(i);
      else
        layerOutputs[l][i]=0;
      neuronOutputs[l][i]=0;
    }
  }

  // Propagate
  for(unsigned l=0; l<m_nodes.size()-1;l++){
    for(unsigned i=0; i<m_nodes[l+1]; i++){
      layerOutputs[l+1][i]=m_bias[l][i];
      for (unsigned j=0;j<m_nodes[l]; j++)  layerOutputs[l+1][i]+=layerOutputs[l][j]*m_weights[l][i][j];
      neuronOutputs[l+1][i] = layerOutputs[l+1][i]; // Hold the sum neuron output before apply the activation function
      //layerOutputs[l+1][i]  = this->activation(layerOutputs[l+1][i], m_tfnames[l]);
      layerOutputs[l+1][i]  = tanh(layerOutputs[l+1][i]);
    } 
  }

  Ringer::RnnOutInfo answer;
  answer.output=(float)(layerOutputs[m_nodes.size()-1][0]);
  answer.outputBeforeTheActivationFunction=(float)(neuronOutputs[m_nodes.size()-1][0]);
  this->release(neuronOutputs,layerOutputs);
  return answer;
}

/*
double MultiLayerPerceptron::activation( double value, std::string &tfname ){

  // Apply the hiperbolic tangent
  if(tfname=="tanh"){
    return tanh(value);
  // Applyt the RELU transfer function 
  }else if(tfname=="relu"){
    return (value > 0) ? value : 0;
  }else if(tfname=="linear"){
    return value;
  }else{ // Bypass the TF
    return value;
  }
}
*/

