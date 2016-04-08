/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolHIGG5VBF.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLHSG5VBF_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLHSG5VBF_H

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

// xAOD header files
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

class TLoretzVector;

namespace DerivationFramework {


  /** @class SkimmingToolHIGG5VBF
      @author Yasuyuki.Okumura@cern.ch
     */
  class SkimmingToolHIGG5VBF : public AthAlgTool, public ISkimmingTool {
    
  public:
    /** Constructor with parameters */
    SkimmingToolHIGG5VBF( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~SkimmingToolHIGG5VBF();

    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
    
  private:
    bool m_debug;

    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
    
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    
    mutable std::vector<TLorentzVector> m_goodAllJets;
    mutable std::vector<TLorentzVector> m_goodCentralJets;
    
    std::string m_jetSGKey;
    std::string m_calibedJetMomKey;
    
    // for jet multiplicity
    bool         m_reqNAllJets;
    unsigned int m_nAllJets;
    double       m_allJetPtCut;
    double       m_allJetEtaCut;

    bool         m_reqNCentralJets;
    unsigned int m_nCentralJets;
    double       m_centralJetPtCut;
    double       m_centralJetEtaCut;

    // for trigger
    bool                     m_reqTrigger;
    std::vector<std::string> m_triggers;
    
    // for Mjj
    bool   m_reqVbfMjj;
    double m_vbfMjjCut;
    
    bool checkAllJetQuality(const TLorentzVector& jet) const;
    bool checkCentralJetQuality(const TLorentzVector& jet) const;
    TLorentzVector getCalibedJets(const xAOD::Jet* jet) const;
    
    //for photon (p. rose)
    std::string m_phSGKey;
    bool m_reqPh;
    double m_phPtCut;
    double m_centralPhEtaCut;
  };
  
}

#endif // #ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLHSG5VBF_H
