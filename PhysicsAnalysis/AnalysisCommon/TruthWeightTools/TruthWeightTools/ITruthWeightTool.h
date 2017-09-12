// $Id$
#ifndef TRUTHWEIGHTTOOLS_ITRUTHWEIGHTTOOL_H
#define TRUTHWEIGHTTOOLS_ITRUTHWEIGHTTOOL_H

//STL includes
#include <memory>

// Framework include(s):
#include "AsgTools/IAsgTool.h"

#include "TruthWeightTools/IndexRetriever.h"

namespace xAOD
{

  /// Interface for xAOD Truth Weight Tool which retrieves
  /// Meta Data from a truth record to interface the event
  /// weights
  ///
  /// @author Tobias Bisanz <tobias.bisanz@cern.ch>
  ///
  /// $Revision$
  /// $Date$
  ///
  class ITruthWeightTool : public virtual asg::IAsgTool
  {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE(xAOD::ITruthWeightTool)

  public:
    virtual std::shared_ptr<IIndexRetriever> spawnIndexRetriever(std::string weightName) = 0;

    /// Return weight names (descriptions) from meta data
    virtual std::vector<std::string> const &getWeightNames() const = 0;

    /// check if a weight exist
    virtual bool hasWeight(std::string weightName) = 0;

    /// return the MC weight vector. Same as accessing it from TruthEvent or EventInfo
    virtual const std::vector<float> &getWeights() const = 0;

    /// Return weight index
    virtual size_t getWeightIndex(std::string weightName) = 0;

    // Return the weight. Same as getWeights()[getWeightIndex(weightName)]
    virtual float getWeight(std::string weightName) = 0;

  }; // class ITruthWeightTool

} // namespace xAOD

#endif

