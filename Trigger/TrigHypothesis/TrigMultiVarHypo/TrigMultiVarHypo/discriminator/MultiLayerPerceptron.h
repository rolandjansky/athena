/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef MULTILAYERPERCEPTRON_H
#define MULTILAYERPERCEPTRON_H

#define BAD_WEIGHT_SIZE   101
#define BAD_BIAS_SIZE     102

#include <vector>

typedef double REAL;

class MultiLayerPerceptron {

  private:

    std::vector<unsigned int> m_nodes; 
    REAL ***m_weights;
    REAL  **m_bias;
    REAL  **m_layerOutputs;
    REAL    m_threshold;
    REAL    m_etmin;
    REAL    m_etmax;
    REAL    m_etamin;
    REAL    m_etamax;



  public:

    MultiLayerPerceptron(const std::vector<unsigned int> &, 
                         const std::vector<REAL> &, const std::vector<REAL> &, 
                         REAL th, REAL etmin,  REAL etmax, 
                         REAL etamin,  REAL etamax);

    ~MultiLayerPerceptron();

    float propagate(std::vector<float> &input);

    REAL threshold(){  return m_threshold;};
    REAL etamin(){ return m_etamin;};
    REAL etamax(){ return m_etamax;};
    REAL etmin(){  return m_etmin;};
    REAL etmax(){  return m_etmax;};

};
#endif
