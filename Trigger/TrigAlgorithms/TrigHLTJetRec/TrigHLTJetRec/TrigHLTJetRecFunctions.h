/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETRECFUNCTIONS_H
#define TRIGHLTJETRECFUNCTIONS_H

/* 
   convertToPseudoJets: produce a container of PseudoJets
   from an  input container. Typical input containers constain clusters,
   trigger towers or jets (eg for reclustering).
*/

#include <string>
#include <cmath>
#include <algorithm>
#include "JetEDM/LabelIndex.h"
#include "TrigNavStructure/TriggerElement.h"
#include "JetEDM/PseudoJetVector.h"
#include "./AnyToPseudoJet.h"


using jet::LabelIndex;
using jet::PseudoJetVector;


template <typename InputCollection>
  class AllSelector{
 public:
  bool operator()(typename InputCollection::const_value_type ){return true;}
};

template <typename InputCollection>
  class EtaMaxPtMinSelector{
 public:
    EtaMaxPtMinSelector(double etaMax,
                        double ptMin): m_etaMax(etaMax), m_ptMin(ptMin){}

  bool operator()(typename InputCollection::const_value_type v){
    if (v -> pt() < m_ptMin){
      return false;
    }
    
    if (std::abs(v -> eta()) > m_etaMax){
      return false;
    }

    return true;
  }
 private:
  double m_etaMax;
  double m_ptMin;
};


template<typename InputContainer, typename InputContainerSelector > 
  HLT::ErrorCode convertToPseudoJets(LabelIndex* indexMap,
                                     std::string clusterCalib,
                                     const InputContainer* inContainer, 
                                     PseudoJetVector& pjv)
{
  
  // setup LabelIndex: clusterCalib = "LC" or"EM"
  
  indexMap->addLabel(clusterCalib + "Topo");

  // convert the incoming objects which pass selection cuts 
  // of the InputContainerSelector predicate
  // to the pseudo jets.

  // setup input object to PseudoJet convertor
  //
  // InputContainer::const_value_type rferes to the containees of the 
  // input container (which are pointers to the concrete types)
  AnyToPseudoJet<typename InputContainer::const_value_type> toPseudoJet(indexMap);

  std::transform(inContainer->begin(),
                 std::partition_point(inContainer->begin(),
                                      inContainer->end(),
                                      InputContainerSelector()),
                 std::back_inserter(pjv),
                 toPseudoJet);

  return HLT::OK;
}


#endif
