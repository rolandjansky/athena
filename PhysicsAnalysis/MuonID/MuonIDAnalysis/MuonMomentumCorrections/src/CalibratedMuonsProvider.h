// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBRATEDMUONSPROVIDER
#define CALIBRATEDMUONSPROVIDER
//author: will buttinger

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include <AsgAnalysisInterfaces/IPileupReweightingTool.h>

namespace CP {

   /// decorates a muon collection with efficiency and scale factor

   class CalibratedMuonsProvider : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
       CalibratedMuonsProvider( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// muon container
      std::string m_inputKey,m_outputKey;

      /// Scale factor tool
      ToolHandle <IMuonCalibrationAndSmearingTool > m_tool;
      ToolHandle <IPileupReweightingTool> m_prwTool;
      bool m_useRndNumber;

   }; // class 

} // namespace CP

#endif //
