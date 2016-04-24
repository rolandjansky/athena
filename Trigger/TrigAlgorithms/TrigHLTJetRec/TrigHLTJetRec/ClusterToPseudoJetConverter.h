/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_CLUSTERTOPSEUDOJETCONVERTER_H
#define TRIGHLTJETREC_CLUSTERTOPSEUDOJETCONVERTER_H

/* Accept a CaloCluster and convert it to a PseudoJet taking into account
whether the cluster local calibration should be set to "EM" */

#include "xAODCaloEvent/CaloClusterChangeSignalState.h"
#include "TrigHLTJetRec/AnyToPseudoJet.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

using InputContainer = xAOD::CaloClusterContainer;
using ValueType = InputContainer::const_value_type;
using ClusterToPseudoJet = AnyToPseudoJet<InputContainer::const_value_type>;

class ClusterToPseudoJetConverter {
public:
  ClusterToPseudoJetConverter(const ClusterToPseudoJet& apj,
                              bool uncalibrated);

  PseudoJet operator()(ValueType c) const;

private: 
  ClusterToPseudoJet m_apj;
  bool m_uncalibrated;
};
#endif
