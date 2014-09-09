/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_validator_header
#define jetsubstructuremomenttools_validator_header

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"

class TH1;

using namespace std;

class Validator :
  virtual public IJetExecuteTool,
  virtual public asg::AsgTool {
    ASG_TOOL_CLASS(Validator, IJetExecuteTool)

    public:
      // Constructor and destructor
      Validator(std::string name);

      int execute() const;

    private:
      std::string m_InputContainer;
      vector<string> m_FloatMoments;

};

#endif
