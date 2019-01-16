/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Neural Network Implementation v3.0
 * Developed by: Dhiana Deva Cavalcanti Rocha
 * Contact: dhiana.deva@gmail.com
 * Laboratorio de Processamento de Sinais
 * Universidade Federal do Rio de Janeiro
 */


///Local include(s)
#include "TrigMultiVarHypo/tools/MultiLayerPerceptron.h"

///std library(s)
#include <cstddef>
#include <vector>
#include <cmath>

MultiLayerPerceptron::MultiLayerPerceptron(std::vector<unsigned int> &n, 
                                           std::vector<REAL> &w, 
                                           std::vector<REAL> &b,
                                           REAL etmin, 
                                           REAL etmax, 
                                           REAL etamin, 
                                           REAL etamax,
                                           REAL mumin,
                                           REAL mumax)
  : m_nodes(n),
    m_weights(0),
    m_bias(0),
    m_layerOutputs(0),
    m_neuronOutputs(0),
    m_etmin(etmin),
    m_etmax(etmax),
    m_etamin(etamin),
    m_etamax(etamax),
    m_mumin(mumin),
    m_mumax(mumax)
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
    m_weights = new REAL **[n.size()-1]; //number of layers excluding input
  }catch (const std::bad_alloc& xa){
    m_weights = nullptr;
    throw;
  }
  //First bias dimension
  try{    
    m_bias = new REAL *[n.size()-1]; //number of layers excluding input
  }catch (const std::bad_alloc& xa){
    m_bias = nullptr;
    throw;    
  }

  //First multiplication dimension
  try{
    m_neuronOutputs   = new REAL *[n.size()];
    m_layerOutputs = new REAL *[n.size()]; //number of layers including input
  } catch (const std::bad_alloc& xa){
    m_layerOutputs = nullptr; 
    m_neuronOutputs   = nullptr;
    throw;
  }

  for (unsigned l = 0; l<n.size(); ++l){ 
    //Checks if no bad_alloc happened to layerOutputs
    if(m_layerOutputs){
      try{
        //Second and last dimension of layerOutputs
        m_layerOutputs[l] = new REAL[n[l]]; //number of nodes in current layer
      } catch (const std::bad_alloc& xa){
        m_layerOutputs[l] = nullptr;
        throw;
      }
    }

    //Checks if no bad_alloc happened to layerOutputs
    if(m_neuronOutputs){
      try{
        //Second and last dimension of layerOutputs
        m_neuronOutputs[l] = new REAL[n[l]]; //number of nodes in current layer
      } catch (const std::bad_alloc& xa){
        m_neuronOutputs[l] = nullptr;
        throw;
      }
    }
  }

  std::vector<REAL>::const_iterator itrB = b.begin();
  std::vector<REAL>::const_iterator itrW = w.begin();
  for (unsigned l = 0; l < n.size()-1; ++l){  
    //Checks if no bad_alloc happened to bias
    if(m_bias){
      try{
        //Second and last dimension of bias
        m_bias[l] = new REAL[n[l+1]]; //number of nodes in next layer
      } catch (const std::bad_alloc& xa){
        m_bias[l] = nullptr;
        throw;
      }
    }
    //Checks if no bad_alloc happened to m_weights
    if(m_weights){ 
      try{
        //Second dimension of weights
        m_weights[l] = new REAL*[n[l+1]]; //number of nodes in next layer
      } catch (const std::bad_alloc& xa){
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
            m_weights[l][i]=new REAL [n[l]]; //number of nodes in current layer
          } catch (const std::bad_alloc& xa){
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

    //Populating multiplication matrix so that starting sum equals zero
    for (unsigned i=0; i<n[l]; i++){
      //Checks if no bad_alloc happened to layerOutputs[L]
      if (m_layerOutputs){
        if (m_layerOutputs[l]){
          m_layerOutputs[l][i]=0;
        }
      }
      if (m_neuronOutputs){
        if (m_neuronOutputs[l]){
          m_neuronOutputs[l][i]=0;
        }
      }
    }   
  }

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

  for (unsigned l=0; l<m_nodes.size(); l++){
    if(m_layerOutputs){
      if (m_layerOutputs[l]==nullptr){
        delete m_layerOutputs[l]; //Deletes null pointer
      }
    }else{
      delete[] m_layerOutputs[l]; //Deletes array of values at second dimension of layerOutputs
    }
  }

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

  if (m_layerOutputs==nullptr){
    delete m_layerOutputs; //Deletes null pointer
  }else{
    delete[] m_layerOutputs; //Deletes array of pointers at first dimension of layerOutputs
  }
}

float MultiLayerPerceptron::propagate(std::vector<float> &input){

  for(unsigned i=0; i<input.size();i++){
    m_layerOutputs[0][i]=(REAL)input[i];
  }

  for(unsigned l=0; l<m_nodes.size()-1;l++){
    for(unsigned i=0; i<m_nodes[l+1]; i++){
      m_layerOutputs[l+1][i]=m_bias[l][i];
      for (unsigned j=0;j<m_nodes[l]; j++)  m_layerOutputs[l+1][i]+=m_layerOutputs[l][j]*m_weights[l][i][j];
      m_neuronOutputs[l+1][i] = m_layerOutputs[l+1][i]; // Hold the sum neuron output before apply the activation function
      m_layerOutputs[l+1][i]  = tanh(m_layerOutputs[l+1][i]);
    }
  }
  return (float)(m_layerOutputs[m_nodes.size()-1][0]);
}

float MultiLayerPerceptron::getOutput(){
  return (float)(m_layerOutputs[m_nodes.size()-1][0]);
}

float MultiLayerPerceptron::getOutputBeforeTheActivationFunction(){
  return (float)(m_neuronOutputs[m_nodes.size()-1][0]);
}






