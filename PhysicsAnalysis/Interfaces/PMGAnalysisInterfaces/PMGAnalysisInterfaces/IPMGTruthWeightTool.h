/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PMGTOOLS_IPMGTRUTHWEIGHTTOOL_H
#define PMGTOOLS_IPMGTRUTHWEIGHTTOOL_H

// STL include(s):
#include <memory>
#include <string>
#include <vector>

// EDM include(s):
#include "AsgTools/IAsgTool.h"

// Local include(s):
#include "PMGAnalysisInterfaces/IPMGTruthWeightIndexRetriever.h"

namespace PMGTools
{
  /// Interface for xAOD Truth Weight Tool which retrieves
  /// Meta Data from a truth record to interface the event
  /// weights
  ///
  /// @author Tobias Bisanz <tobias.bisanz@cern.ch>
  /// @author James Robinson <james.robinson@cern.ch>, rewrite for inclusion in PMGTools
  ///
  class IPMGTruthWeightTool: public virtual asg::IAsgTool
  {
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE(xAOD::IPMGTruthWeightTool)

  public:
    /// Return vector of weight names (descriptions) from meta data
    virtual const std::vector<std::string>& getWeightNames() const = 0;

    /// Return vector of MC event weights. Same as accessing it from TruthEvent or EventInfo
    virtual const std::vector<float>& getWeights() const = 0;

    // Return the weight. Same as getWeights()[getWeightIndex(weightName)]
    virtual float getWeight(const std::string& weightName) const = 0;

    /// Return weight index for weightName
    virtual size_t getWeightIndex(const std::string& weightName) const = 0;

    /// Check if a weight with the current name exists
    virtual bool hasWeight(const std::string& weightName) const = 0;

  protected:
    /// Create an instance of the index retriever
    virtual std::shared_ptr<IPMGTruthWeightIndexRetriever> spawnTruthWeightIndexRetriever(std::string weightName) const = 0;

  }; // class IPMGTruthWeightTool

} // namespace xAOD

#endif // PMGTOOLS_IPMGTRUTHWEIGHTTOOL_H
