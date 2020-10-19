/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolEXOT15.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

/**
 *  Skimming tool used in EXOT15.
 *  The baseline skimming option, ask for at least one trackless jet with pT above minJetPt and within jetEtaCut.
 *  The trackless jet is defined as a jet with no tracks above minTrackPt within minDeltaR from the jet.
 *  Asking for the cleanLLP flag, requires the jet to pass LooseBadLLP isolation criteria. The jetContainer and trackContainer
 *  features indicate the name of the containers to be used to evaluate the trackless jet condition.xs
 *  
 *  Setting cutLeptonPt to true, cuts on leptons are applied. A minumum of minNLeptons are required to have at least
 *  minLeptonPt GeV in pT. This condition is set as logical AND with the requirement on trackless jet.
 *
 *  The flag cutNMSRoIs turns on conditions on MS trigger RoIs: a minimum of minNMSRoIs are required in the event. 
 *  This condition is set in OR with the previous conditions.
 *
 *  The flag cutNMSVtx turns on a condition on the number of MS vertices in the event. A minumum of minNMSVtx vertices
 *  are required to be in the event. This condition is set as logical OR with the previous conditions.
 *
 */


#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT15_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT15_H 1
 
#include <string>
#include <vector>
#include <algorithm>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

// xAOD header files
#include "xAODJet/JetContainer.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class JetCalibrationTool;

namespace DerivationFramework {

  /** @class SkimmingToolEXOT15
      @author paolo.sabatini@cern.ch 
     */
  class SkimmingToolEXOT15 : public AthAlgTool, public ISkimmingTool {

    public: 
      /** Constructor with parameters */
      SkimmingToolEXOT15( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~SkimmingToolEXOT15();

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();

      /** Check that the current event passes this filter */
      virtual bool eventPassesFilter() const;

    private:
      mutable unsigned int m_ntot;
      mutable unsigned int m_npass;
      
      float m_minDeltaR;
      float m_minJetPt;
      float m_jetEtaCut;
      float m_minTrackPt;
      float m_minLeptonPt;
      float m_minNLeptons;
      float m_minNMSRoIs;
      float m_minNMSVtx;
      std::string m_jetContainer;
      std::string m_trkContainer;
      bool m_cleanLLP;
      bool m_cutLeptonPt;
      bool m_cutNMSRoIs;
      bool m_cutNMSVtx;
  }; 

}

#endif 
