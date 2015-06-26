/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENERGYSUMSELECTIONTOOL_H
#define ENERGYSUMSELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
/* #include "PATCore/IAsgSelectionTool.h" */


// Local includes
#include "TrigTauEmulation/IEnergySumSelectionTool.h"

class EnergySumSelectionTool : public virtual IEnergySumSelectionTool, public asg::AsgTool

{

  ASG_TOOL_CLASS(EnergySumSelectionTool, IEnergySumSelectionTool)


 public:

  // Default Constructor 
  EnergySumSelectionTool(const std::string& name);

  // Copy Constructor 
  EnergySumSelectionTool(const EnergySumSelectionTool& other);

  // Destructor
  virtual ~EnergySumSelectionTool() {};

  // Tool initialization
  virtual StatusCode initialize();

  // Get the decision for a specific EnergySumRoI
  virtual const Root::TAccept& accept(const xAOD::EnergySumRoI& l1xe) const;



 private:

  mutable Root::TAccept m_accept;
  
  double m_MET_cut;

 protected:

  double calculate_MET(const xAOD::EnergySumRoI& l1xe) const;

};
#endif
