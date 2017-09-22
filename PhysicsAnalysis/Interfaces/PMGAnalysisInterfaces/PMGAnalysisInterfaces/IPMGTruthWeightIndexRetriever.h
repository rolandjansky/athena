/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PMGTOOLS_ITRUTHWEIGHTINDEXRETRIEVER_H
#define PMGTOOLS_ITRUTHWEIGHTINDEXRETRIEVER_H

// EDM include(s):
#include "xAODTruth/TruthMetaData.h"

namespace PMGTools
{
  /// Interface for xAOD truth weight index retriever
  /// retrieves the index of each truth-level event weight
  ///
  /// @author Tobias Bisanz <tobias.bisanz@cern.ch>
  /// @author James Robinson <james.robinson@cern.ch>, rewrite for inclusion in PMGTools
  ///
  class IPMGTruthWeightIndexRetriever
  {
  public:
    /// Default constructor
    IPMGTruthWeightIndexRetriever() : m_isValid(false) {};

    /// Update indices from truth meta data
    virtual void update(const xAOD::TruthMetaData* const) = 0;

    /// Update indices from vector of names
    virtual void update(const std::vector<std::string>& weightNamesVec) = 0;

    /// Return weight index
    virtual size_t getIndex() = 0;

    /// Check whether the interface is valid
    virtual bool isValid() {return m_isValid;}

  protected:
    /// Default destructor
    virtual ~IPMGTruthWeightIndexRetriever() {};

    /// Switch to determine whether the interface has been initialised
    bool m_isValid;

    /// Index of current weight
    size_t m_currentIndex;
  };

} // namespace PMGTools

#endif // PMGTOOLS_ITRUTHWEIGHTINDEXRETRIEVER_H
