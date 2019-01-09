/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/*
 * Neural Network Implementation v3.0
 * Developed by: Dhiana Deva Cavalcanti Rocha
 * Contact: dhiana.deva@gmail.com
 * Laboratorio de Processamento de Sinais
 * Universidade Federal do Rio de Janeiro
 *
 * Adapt by Joao victor da Fonseca Pinto:
 * jodafons@cern.ch
 */

#ifndef TOOLS_MULTILAYERPERCEPTRON_H
#define TOOLS_MULTILAYERPERCEPTRON_H

#define BAD_WEIGHT_SIZE   101
#define BAD_BIAS_SIZE     102

#include <vector>

typedef double REAL;

class MultiLayerPerceptron {

  private:

    std::vector<unsigned int> m_nodes; 
    REAL                   ***m_weights;
    REAL                    **m_bias;
    REAL                    **m_layerOutputs;
    REAL                    **m_neuronOutputs; // Use this to hold the neuron sum values before to apply the tansig function
    std::vector<REAL>         m_threshold;
    REAL                      m_etmin;
    REAL                      m_etmax;
    REAL                      m_etamin;
    REAL                      m_etamax;
    REAL                      m_mumin;
    REAL                      m_mumax;


  public:

    MultiLayerPerceptron(std::vector<unsigned int> &, 
                         std::vector<REAL> &, std::vector<REAL> &, 
                         REAL etmin,  REAL etmax, 
                         REAL etamin,  REAL etamax, REAL mumin, REAL mumax);

    ~MultiLayerPerceptron();

    float propagate(std::vector<float> &input);

    float getOutput();
    
    float getOutputBeforeTheActivationFunction();

    REAL etamin(){ return m_etamin;};
    REAL etamax(){ return m_etamax;};
    REAL etmin(){  return m_etmin;};
    REAL etmax(){  return m_etmax;};
    REAL mumin(){  return m_mumin;};
    REAL mumax(){  return m_mumax;};

};
#endif
