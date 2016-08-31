// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBRATEDMUONSPROVIDER
#define CALIBRATEDMUONSPROVIDER
//author: will buttinger

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonTPTools/IIDTrackIsolationDecoratorTool.h"



   /// decorates a muon collection with efficiency and scale factor

   class HighPtTrackPicker : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
       HighPtTrackPicker( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// muon container
      std::string m_inputKey;
      std::string m_outputKey;
      float m_pt_cut;
      ToolHandle<IIDTrackIsolationDecoratorTool> m_trk_iso_tool;

   }; // class 


#endif //
