// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionToolAthD3PD.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_TRIGDECISIONTOOLATHD3PD_H
#define TRIGROOTANALYSIS_TRIGDECISIONTOOLATHD3PD_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "../TrigRootAnalysis/ITrigDecisionToolD3PD.h"
#include "../TrigRootAnalysis/ChainGroupHandling.h"
#include "../TrigRootAnalysis/TrigDecisionFunctions.h"
#include "AthDataAccess.h"
#include "AthConfigAccess.h"

namespace D3PD {

   /**
    *  @short Athena implementation for the IITrigDecisionToolD3PD interface
    *
    *         This implementation can be used in an Athena D3PD analysis
    *         to access the full trigger decision information.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502443 $
    * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
    */
   class TrigDecisionToolAthD3PD :
      public AthAlgTool,
      public ITrigDecisionToolD3PD,
      public AthDataAccess,
      public AthConfigAccess,
      public D3PD::Trig::ChainGroupHandling,
      public D3PD::Trig::TrigDecisionFunctions {

   public:
      /// Gaudi AlgTool constructor
      TrigDecisionToolAthD3PD( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();

   }; // class TrigDecisionToolAthD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_TRIGDECISIONTOOLATHD3PD_H
