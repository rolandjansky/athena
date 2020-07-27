/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRUTHCLASSIFICATIONTOOL_H_
#define ITRUTHCLASSIFICATIONTOOL_H_

#include <AsgAnalysisInterfaces/IClassificationTool.h>
#include <xAODBase/IParticle.h>

#include "TruthClassification/TruthClassificationDefs.h"

/// \brief a specialisation of \ref CP::IClassificationTool to allow
/// direct enum return value
class ITruthClassificationTool : virtual public CP::IClassificationTool
{
  ASG_TOOL_INTERFACE(ITruthClassificationTool)

public:
  virtual ~ITruthClassificationTool() = default;

  /// \brief classify and return unsigned int
  virtual StatusCode classify(const xAOD::IParticle &particle,
                              unsigned int &classification) const = 0;

  /// \brief classify and return Truth::Type
  virtual StatusCode classify(const xAOD::IParticle &particle,
                              Truth::Type &classification) const = 0;
};

#endif // ITRUTHCLASSIFICATIONTOOL_H_
