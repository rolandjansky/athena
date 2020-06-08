// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_INDETTRACKSMEARINGTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_INDETTRACKSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/CorrectionTool.h"
#include "PATInterfaces/SystematicVariation.h"

//EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "InDetTrackSystematicsTools/IInDetTrackSmearingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackSystematicsTool.h"

#include <TFile.h>
#include <TH2F.h>
#include <TF2.h>
#include <TRandom3.h>

namespace InDet {

  class InDetTrackSmearingTool
    : public virtual IInDetTrackSmearingTool
    , public virtual InDetTrackSystematicsTool
    , public virtual CP::CorrectionTool< xAOD::TrackParticleContainer >
  {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( InDetTrackSmearingTool, 
		    InDet::IInDetTrackSmearingTool )
    public:
    /// Create a constructor for standalone usage
    InDetTrackSmearingTool( const std::string& name );
    virtual ~InDetTrackSmearingTool();

    /// Function initialising the tool
    virtual StatusCode initialize() override;
    /// Smearing method
    virtual CP::CorrectionCode applyCorrection( xAOD::TrackParticle& ID ) override;

    // we need explicit forwarding calls because CP::CorrectionTool does not have a pure interface
    // this isn't really elegant (ideally these would come automatically) but we'd need IInDetTrackSmearingTool
    //  to inherit from CP::CorrectionTool, which would make it not pure-virtual.
    // at least, forwarding calls are easy enough to read and understand.
    virtual CP::CorrectionCode correctedCopy( const xAOD::TrackParticle& in,
					      xAOD::TrackParticle*& out ) override;
    virtual CP::CorrectionCode applyContainerCorrection( xAOD::TrackParticleContainer& cont ) override;
    
    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const override;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const override;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const override;
    /// configure the tool to apply a given list of systematic variations
    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& ) override;

    /// Get smearing widths to add to IPs
    float GetSmearD0Sigma(const xAOD::TrackParticle&);
    float GetSmearZ0Sigma(const xAOD::TrackParticle&);

  private:

    int m_seed = 0;
    std::unique_ptr<TRandom3> m_rnd; //!

    // StatusCode initHistogram(TH1*& histogram, std::string rootFileName, std::string histogramName) const;
    // StatusCode initFunction(TF2*& function, std::string rootFileName, std::string functionName) const;
    float readHistogram( TH2*, float pt, float eta ) const;
      
    // float readHistogram(TH1* histogram, float pt, float eta) const;
    // float readHistograms(std::vector<TH1 *> histogram, float pt, float eta) const;    
    // histograms for dead module smearing as a function of p
    TH1* m_smearD0Dead = nullptr;
    TH1* m_smearZ0Dead = nullptr;

    // for 20.7, we'll use histograms directly, instead of fit functions
    TH2* m_smearD0 = nullptr;
    TH2* m_smearZ0 = nullptr;
    TH2* m_smearD0_sys_up = nullptr;
    TH2* m_smearZ0_sys_up = nullptr;
    TH2* m_smearD0_sys_dw = nullptr;
    TH2* m_smearZ0_sys_dw = nullptr;
    
     
  }; /// class InDetTrackSmearingTool

} /// namespace InDet

#endif
