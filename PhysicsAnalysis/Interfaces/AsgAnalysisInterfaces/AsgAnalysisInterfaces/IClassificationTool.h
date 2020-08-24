/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>


#ifndef ICLASSIFICATIONTOOL_H_
#define ICLASSIFICATIONTOOL_H_

#include <AsgTools/IAsgTool.h>
#include <xAODBase/IParticle.h>

namespace CP
{

/// \brief a tool interface to classify particles into multiple categories
/// by assigning them an unsigned integer
class IClassificationTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IClassificationTool)

public:
  virtual ~IClassificationTool() = default;

  /// \brief classify a particle and pass the classification as an integer
  virtual StatusCode classify(const xAOD::IParticle &particle,
                              unsigned int &classification) const = 0;
};

} // namespace CP

#endif // ICLASSIFICATIONTOOL_H_
