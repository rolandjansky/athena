// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ToolTester.h 299890 2014-03-29 08:54:38Z krasznaa $
#ifndef CPTOOLTESTS_TOOLTESTER_H
#define CPTOOLTESTS_TOOLTESTER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "CPAnalysisExamples/IMuonSelectionToolExample.h"
#include "CPAnalysisExamples/IMuonSmearingToolExample.h"
#include "CPAnalysisExamples/IMuonEfficiencyToolExample.h"

namespace CP {

   /// Simple algorithm for using the tools in Athena
   ///
   /// This is meant to be the "Athena equivalent" of the code put into
   /// util/CPToolTester.cxx.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299890 $
   /// $Date: 2014-03-29 09:54:38 +0100 (Sat, 29 Mar 2014) $
   ///
   class ToolTester : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      ToolTester( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key for the muon container to investigate
      std::string m_sgKey;

      /// Connection to the selection tool
      ToolHandle< IMuonSelectionToolExample > m_selTool;
      /// Connection to the smearing tool
      ToolHandle< IMuonSmearingToolExample > m_smearTool;
      /// Connection to the efficiency tool
      ToolHandle< IMuonEfficiencyToolExample > m_effTool;

   }; // class ToolTester

} // namespace CP

#endif // CPTOOLTESTS_TOOLTESTER_H
