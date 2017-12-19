/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGRNNOUTPUT_H
#define TRIGRNNOUTPUT_H


#include <vector>

#include "AthLinks/ElementLink.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEvent/TrigEMCluster.h"

#include "AthenaKernel/CLASS_DEF.h"

/**
 * This class carries the output of the Neural Network of TrigMultiVarHypo.
 */

class TrigRNNOutput {

private:
  std::vector<float> m_output; //< Holds the output vector of the neural network.
  ElementLink<TrigEMClusterContainer> m_cluster; //< Links this object with the cluster on which the rings were generated.

public:

  /**
   * Class default constructor
   */
  TrigRNNOutput();
    
  /**
   * Class constructor
   * 
   * @param output The neural network output.
   */
  TrigRNNOutput(const std::vector<float> &output);


  /** Acessor methods **/
  std::vector<float> &output();
  const std::vector<float> &output() const;
  void output(const std::vector<float> &o);
  float &at(const unsigned int i);
  const float &at(const unsigned int i) const;
  unsigned int size() const;
  const float &operator [](const unsigned int i) const;
  float &operator [](const unsigned int i);
  
  /**
   * Set and manipulate ElementLink
   *
   */

  bool isValid() const;
  void setCluster(const TrigEMClusterContainer &cluster_con, unsigned int cluster_idx);
  void setCluster(const ElementLink<TrigEMClusterContainer>& cluster);
  const TrigEMCluster *cluster() const;
  const ElementLink<TrigEMClusterContainer>& clusterLink() const;

 private:
  template <class T>
    friend class TrigRNNOutputConverterBase;
};

CLASS_DEF( TrigRNNOutput , 55078062 , 1 )

#endif

  
