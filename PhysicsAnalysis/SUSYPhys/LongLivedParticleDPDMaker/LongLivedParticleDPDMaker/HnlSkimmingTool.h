// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// HnlSkimmingTool.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HNLSKIMMINGTOOL_H
#define DERIVATIONFRAMEWORK_HNLSKIMMINGTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "xAODMuon/MuonContainer.h"

#include <string>
#include <vector>

namespace DerivationFramework {

  class HnlSkimmingTool : public AthAlgTool, public ISkimmingTool {
   
  public: 
    /** Constructor with parameters */
    HnlSkimmingTool(const std::string& t, const std::string& n, const IInterface* p);

    /** Destructor */
    virtual ~HnlSkimmingTool() = default;
   
    // Athena algtool's Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
 
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const override;
    bool isGood(const xAOD::Muon& mu) const;

  private:
    // Trigger
    ToolHandle< Trig::TrigDecisionTool > m_trigDecisionTool;
    std::vector< std::string > m_triggers;
    // Muon
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
