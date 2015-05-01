// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDTExample.h 491249 2012-03-28 13:25:12Z stelzer $
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

   /**
    *  @short Example for the usage of TrigDecisionTool in C++
    *
    * @author Tomasz Bold     <tomasz.bold@cern.ch>     - UC Irvine, AGH-UST Krakow
    *
    * $Revision: 491249 $
    * $Date: 2012-03-28 15:25:12 +0200 (Wed, 28 Mar 2012) $
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
      StatusCode checkLevels(); //!< checks if levels passed are right (i.e. L1 pass if L2 passed)
      StatusCode printLevels(); 
      StatusCode printRoIs();
      StatusCode matchRoIs(); //!< features access example
      StatusCode electronsAndFlags();
      StatusCode featuresOfCompositeChain();
      StatusCode printBGInformation();

      ToolHandle< Trig::TrigDecisionTool > m_trigDec; //!< TDT handle

      const ChainGroup* m_all;
      const ChainGroup* m_allL1;
      const ChainGroup* m_allL2;
      const ChainGroup* m_allEF;

      std::vector< std::string > m_chain_names;

      StatusCode jetRoItoChains( std::vector<JetInfo>& jetRoIs );
      StatusCode addChainInfo( std::vector<JetInfo>& jetRoIs, const ChainGroup* cg );

   }; // class TDTExample

} // namespace Trig

#endif // TRIGANALYSISEXAMPLES_TDTEXAMPLE_H
