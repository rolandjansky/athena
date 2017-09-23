/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PMGTOOLS_TRUTHWEIGHTINDEXRETRIEVER_H
#define PMGTOOLS_TRUTHWEIGHTINDEXRETRIEVER_H

// STL include(s):
#include <string>
#include <vector>

// EDM include(s):
#include "xAODTruth/TruthMetaData.h"

// Interface include(s):
#include "PMGAnalysisInterfaces/IPMGTruthWeightIndexRetriever.h"


namespace PMGTools
{
  /// Implementation for the xAOD truth meta data weight index retriever
  ///
  /// @author Tobias Bisanz  <tobias.bisanz@cern.ch>
  /// @author Dag Gillberg <dag.gillberg@cern.ch>, trivial modifications
  /// @author James Robinson <james.robinson@cern.ch>, rewrite for inclusion in PMGTools
  ///
  class PMGTruthWeightIndexRetriever: public IPMGTruthWeightIndexRetriever
  {
  public:
    /// Default constructor
    PMGTruthWeightIndexRetriever(const std::string& weightName);

    /// Implements interface from IPMGTruthWeightIndexRetriever
    virtual void update(const xAOD::TruthMetaData* const truthMetaData);

    /// Implements interface from IPMGTruthWeightIndexRetriever
    virtual void update(const std::vector<std::string>& weightNamesVec);

    /// Implements interface from IPMGTruthWeightIndexRetriever
    virtual size_t getIndex();

    /// Implements interface from IPMGTruthWeightIndexRetriever
    virtual bool isValid();

  protected:
    /// Switch to determine whether the interface has been initialised
    bool m_isValid;

    /// Name of weight
    const std::string m_weightName;
  };
} // namespace PMGTools

#endif // PMGTOOLS_TRUTHWEIGHTINDEXRETRIEVER_H