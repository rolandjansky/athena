/*
  Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolHDBS3.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLHDBS3_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLHDBS3_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"

namespace DerivationFramework {

  /** @class SkimmingToolHDBS3

      @author panagiotis.bellos@cern.ch
     */
  class SkimmingToolHDBS3 : public AthAlgTool, public ISkimmingTool {

  public:
    /** Constructor with parameters */
    SkimmingToolHDBS3( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~SkimmingToolHDBS3();

    // Athena algtool's Hooks
    virtual StatusCode  initialize() final;
    virtual StatusCode  finalize() final;

    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const override;

  private:
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;

    std::string m_muonSGKey;
    std::string m_electronSGKey;
    std::string m_filterType;

    unsigned int m_nLeptons;
    unsigned int m_nElectrons;
    unsigned int m_nMuons;

    double m_electronPtCut;
    double m_muonPtCut;
    double m_invariantMassCut_min;
    double m_invariantMassCut_max;

  };

}


#endif
