// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSELECTIONTOOL_TOOLTESTER_H
#define INDETTRACKSELECTIONTOOL_TOOLTESTER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace InDet {

  // forward declaration of tool
  class IInDetTrackSelectionTool;

   /// Simple algorithm for using the tools in Athena
   ///
   /// This is meant to be the "Athena equivalent" of the code put into
   /// util/InDetTrackSelectionToolTester.cxx.
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
      /// StoreGate key for the track container to investigate
      std::string m_sgKey;

      /// Connection to the selection tool
      ToolHandle< IInDetTrackSelectionTool > m_selTool;

   }; // class ToolTester

} // namespace InDet

#endif // INDETTRACKSELECTIONTOOL_TOOLTESTER_H
