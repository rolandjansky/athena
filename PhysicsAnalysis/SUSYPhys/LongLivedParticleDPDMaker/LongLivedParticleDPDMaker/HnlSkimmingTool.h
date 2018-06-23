/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// HnlSkimmingTool.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HNLSKIMMINGTOOL_H
#define DERIVATIONFRAMEWORK_HNLSKIMMINGTOOL_H 1

#include<string>
#include<vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODMuon/MuonContainer.h"

namespace DerivationFramework {

  class HnlSkimmingTool : public AthAlgTool, public ISkimmingTool {
   
  public: 
    /** Constructor with parameters */
    HnlSkimmingTool( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~HnlSkimmingTool();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
 		bool IsGood(const xAOD::Muon& mu) const;
  private:
    ToolHandle< Trig::TrigDecisionTool > m_trigDecisionTool;
    std::vector< std::string > m_triggers;
    std::string m_muonSGKey;
    float m_mu1PtMin;
    float m_mu1AbsEtaMax;
    std::vector< int > m_mu1Types;
    int m_mu1IsoType; //http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODPrimitives/xAODPrimitives/IsolationType.h
    int m_mu1IsoCutIsRel; //is the isolation cut relative or absolute
    float m_mu1IsoCut; //cut value
    float m_mu2PtMin;
    float m_mu2AbsEtaMax;
    std::vector< int > m_mu2Types;
    int m_mu2IsoType;
    int m_mu2IsoCutIsRel;
    float m_mu2IsoCut;
    float m_mu2d0Min;

  }; 
 
}

#endif
