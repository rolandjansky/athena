// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDTExample.h 771105 2016-08-31 19:04:44Z rwhite $
#ifndef TRIGANALYSISEXAMPLES_TDTEXAMPLE_H
#define TRIGANALYSISEXAMPLES_TDTEXAMPLE_H

// STL include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

// Forward declaration(s):
class JetInfo;

namespace Trig {

   // Forward declaration(s):
   class ChainGroup;
   class TriggerAnalysisHelper;
   /**
    *  @short Example for the usage of TrigDecisionTool in C++
    *
    * @author Tomasz Bold     <tomasz.bold@cern.ch>     - UC Irvine, AGH-UST Krakow
    *
    * $Revision: 771105 $
    * $Date: 2016-08-31 21:04:44 +0200 (Wed, 31 Aug 2016) $
    */
   class TDTExample : public AthAlgorithm {

   public: 
      /// Regular algorithm constructor
      TDTExample( const std::string &name, ISvcLocator *pSvcLocator );

      /// Function called at the beginning of the job
      virtual StatusCode initialize();
      /// Function called at each event
      virtual StatusCode execute(); 
  
   private:
      StatusCode checkTriggerDecision(); //!< decision bit analysis
      StatusCode checkLevels(); //!< checks if levels passed are right (i.e. L1 pass if L2 passed)
      StatusCode printLevels();
      StatusCode printChainConfiguration();
      StatusCode printRoIs();
      StatusCode printHelpers();
      StatusCode matchRoIs(); //!< features access example
      StatusCode electronsAndFlags();
      StatusCode featuresOfCompositeChain();
      StatusCode printBGInformation();

      ToolHandle< Trig::TrigDecisionTool > m_trigDec; //!< TDT handle
      ToolHandle< Trig::TriggerAnalysisHelper > m_tah; //! Helper class for tutorial, provides an additional layer to illustrate TDT functionality
      int m_evtNr;
      const ChainGroup* m_all;
      const ChainGroup* m_allL1;
      const ChainGroup* m_allHLT;

      std::vector< std::string > m_chain_names;
      std::vector< std::string > m_cfg_chains;

      StatusCode jetRoItoChains( std::vector<JetInfo>& jetRoIs );
      StatusCode addChainInfo( std::vector<JetInfo>& jetRoIs, const ChainGroup* cg );

   }; // class TDTExample

} // namespace Trig

#endif // TRIGANALYSISEXAMPLES_TDTEXAMPLE_H
