/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FSRUTILS_TOOLTESTER_H
#define FSRUTILS_TOOLTESTER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "FsrUtils/IFsrPhotonTool.h"

// to make histogram
#include "TLorentzVector.h"


namespace FSR {

   ///
   ///
   /// Simple algorithm for using the FSR tool in Athena
   ///
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
      /// StoreGate keys for the muon, photon and electron containers to investigate
      std::string m_sgMuonKey;
      std::string m_sgPhotonKey;
      std::string m_sgElectronKey;

      /// Connection to the fsr tool
      ToolHandle< IFsrPhotonTool > m_fsrTool;

      FsrCandidate candidate;

   }; // class ToolTester

} // namespace FSR

#endif // FSRUTILS_TOOLTESTER_H
