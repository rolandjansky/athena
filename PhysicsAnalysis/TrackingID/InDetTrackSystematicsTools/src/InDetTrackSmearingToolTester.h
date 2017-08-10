// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_INDETTRACKSMEARINGTOOLTESTER_H
#define INDETTRACKSYSTEMATICSTOOLS_INDETTRACKSMEARINGTOOLTESTER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "AsgTools/ToolHandle.h"
// Local include(s):
#include "InDetTrackSystematicsTools/IInDetTrackSmearingTool.h"
#include <TH1.h>


namespace InDet {

   /// Simple algorithm for testing the tools in Athena
   ///
   /// @author Michael (Felix) Clark <michael.ryan.clark@cern.ch>
   ///
   class InDetTrackSmearingToolTester : public AthHistogramAlgorithm {

   public:
      /// Regular Algorithm constructor
      InDetTrackSmearingToolTester( const std::string& name, ISvcLocator* svcLoc );
      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key for the track container to investigate//--->delete in future
      std::string Track_IP;
            
      /// Connection to the smearing tool
      ToolHandle< IInDetTrackSmearingTool > m_smearTool;

     std::vector< std::string > m_systematicsNames;
     CP::SystematicSet m_systActive;

   }; // class ITSmeToolTester

} // namespace InDet

#endif
