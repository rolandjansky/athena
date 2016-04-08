/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolEXOT5.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT5_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT5_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODJet/JetContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetCalibTools/IJetCalibrationTool.h"

class JetCalibrationTool;

namespace DerivationFramework {

  class SkimmingToolEXOT5 : public AthAlgTool, public ISkimmingTool {

    public: 
      SkimmingToolEXOT5( const std::string& t, const std::string& n, const IInterface* p );

      ~SkimmingToolEXOT5();

      StatusCode  initialize();
      StatusCode  finalize();

      virtual bool eventPassesFilter() const;

    private:

      ToolHandle<IJetCalibrationTool> m_jetCalibrationTool;
      std::string m_jetSGKey;

      double m_uncalibMonoJetPt;
      double m_monoJetPt;
      double m_leadingJetPt;
      double m_subleadingJetPt;
      double m_Mjj;

      mutable bool m_isMC;
      mutable unsigned int m_ntot;
      mutable unsigned int m_npass;

  }; 

}

#endif
