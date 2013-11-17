// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchCrossingExample.h 501857 2012-05-22 14:52:04Z krasznaa $
#ifndef TRIGANALYSISEXAMPLES_BUNCHCROSSINGEXAMPLE_H
#define TRIGANALYSISEXAMPLES_BUNCHCROSSINGEXAMPLE_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// Trigger include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

namespace Trig {

   /**
    *  @short Algorithm demonstrating the usage of Trig::IBunchCrossingTool
    *
    *         This algorithm demonstrates how to use the
    *         Trig::IBunchCrossingTool interface in a generic analysis.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 501857 $
    * $Date: 2012-05-22 16:52:04 +0200 (Tue, 22 May 2012) $
    */
   class BunchCrossingExample : public AthAlgorithm {

   public:
      /// Regular Athena algorithm constructor
      BunchCrossingExample( const std::string& name, ISvcLocator* svcloc );

      /// Initialization run before the event loop
      virtual StatusCode initialize();
      /// Function called once per event
      virtual StatusCode execute();

   private:
      /// Function for printing detailed info about a given bunch crossing
      void printInfo( unsigned int bcid );

      ToolHandle< IBunchCrossingTool > m_bcTool; ///< Handle to the tool

   }; // class BCTester

} // namespace Trig

#endif // TRIGANALYSISEXAMPLES_BUNCHCROSSINGEXAMPLE_H
