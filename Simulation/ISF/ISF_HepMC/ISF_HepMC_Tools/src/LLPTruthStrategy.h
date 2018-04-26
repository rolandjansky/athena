/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LLPTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_LLPTRUTHSTRATEGY_H
#define ISF_TOOLS_LLPTRUTHSTRATEGY_H 1

// stl includes
#include <set>
#include <vector>

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

namespace ISF {

  /** @class LLPTruthStrategy
  
      An ISF truth strategy for recording long lived particles to
      the MC truth.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class LLPTruthStrategy : public extends<AthAlgTool, ITruthStrategy> {
      
    public: 
      /** Constructor with parameters */
      LLPTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~LLPTruthStrategy();

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();

      /** True if the ITruthStrategy implementationapplies to the given ITruthIncident */
      bool pass( ITruthIncident& incident) const;

	private:
      /** The process code range (low-high) and the category of processes that
       *  should pass this strategy */
      int  m_passProcessCodeRangeLow;
      int  m_passProcessCodeRangeHigh;
      int  m_passProcessCategory;

      /** Returns true if the given |pdgID| is a SUSY particle */
      bool isSUSYParticle(const int absPdgID) const;
  }; 

}


#endif //> !ISF_TOOLS_LLPTRUTHSTRATEGY_H
