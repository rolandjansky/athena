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

#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"


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
      ToolHandle< IMuonCalibrationAndSmearingTool > m_tool;

   }; // class 

} // namespace CP

#endif //
