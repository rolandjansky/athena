/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetECPSFractionTool.h

#ifndef JetMomentTools_JetECPSFractionTool_H
#define JetMomentTools_JetECPSFractionTool_H

/// David Adams \n
/// May 2015
///
/// Tool to calculate the fraction of jet energy in endcap preshower (ECPS) clusters.
/// Clusters which have more that ECPSFractionThreshold of their energy in the ECPS are
/// included in the numerator.
///
/// Properties:
///   ECPSFractionThreshold: Threshold for identifying a cluster as ECPS.
///
/// Attributes:
///   ECPSFraction: Fraction of jet energy in the ECPS
///

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "JetInterface/IJetDecorator.h"

class JetECPSFractionTool : public asg::AsgTool,
                            virtual public IJetDecorator{
  ASG_TOOL_CLASS(JetECPSFractionTool, IJetDecorator)

public:

  // Constructor from tool name.
  JetECPSFractionTool(std::string myname);

  // Inherited method to decorate a jet.
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

  // Inherited from AsgTool
  virtual StatusCode initialize() override;

  // Local method to calculate and return the energy fraction.
  double energyFraction(const xAOD::Jet& jet) const;

private:

  Gaudi::Property<double> m_fraclimit{this, "ECPSFractionThreshold", 0.8,
      "Threshold for identifying a cluser as ECPS"};
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "",
      "SG key for input jet container"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_fracKey{this, "ECPSFractionDecorKey", "ECPSFraction",
      "Name for output ECPS fraction decoration"};

};

#endif
