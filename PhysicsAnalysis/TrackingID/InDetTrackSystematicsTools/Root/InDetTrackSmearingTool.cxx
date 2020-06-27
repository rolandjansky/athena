/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TRandom.h>
#include <TH2F.h>
#include <TFile.h>

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
//#include "AthenaBaseComps/AthCheckMacros.h"

// Local include(s):
#include "InDetTrackSystematicsTools/InDetTrackSmearingTool.h"

using std::string;
using std::vector;
using std::make_unique;

namespace {
  using TrackCorrTool_t = CP::CorrectionTool< xAOD::TrackParticleContainer >;
}

namespace InDet {

  static const CP::SystematicSet SmearingSystematics = 
    {
      InDet::TrackSystematicMap[TRK_RES_D0_MEAS],
      InDet::TrackSystematicMap[TRK_RES_Z0_MEAS],
      InDet::TrackSystematicMap[TRK_RES_D0_MEAS_UP],
      InDet::TrackSystematicMap[TRK_RES_Z0_MEAS_UP],
      InDet::TrackSystematicMap[TRK_RES_D0_MEAS_DOWN],
      InDet::TrackSystematicMap[TRK_RES_Z0_MEAS_DOWN],
      InDet::TrackSystematicMap[TRK_RES_D0Z0Corl_MEAS],
      InDet::TrackSystematicMap[TRK_RES_D0Z0_MEAS],
      InDet::TrackSystematicMap[TRK_RES_D0_DEAD],
      InDet::TrackSystematicMap[TRK_RES_Z0_DEAD]
    };

  static const CP::SystematicSet RecommendedSystematics = 
    {
      InDet::TrackSystematicMap[TRK_RES_D0_MEAS],
      InDet::TrackSystematicMap[TRK_RES_Z0_MEAS],
      // the TRK_RES_[D|Z]0_MEAS_[UP|DOWN] systematics are for advanced users only, who are using TRK_RES_[D|Z]0_MEAS as the nominal point
      InDet::TrackSystematicMap[TRK_RES_D0_DEAD],
      InDet::TrackSystematicMap[TRK_RES_Z0_DEAD]
    };

  InDetTrackSmearingTool::InDetTrackSmearingTool( const std::string& name )
  : InDet::InDetTrackSystematicsTool( name )
  {
   
#ifndef XAOD_STANDALONE
    declareInterface<IInDetTrackSmearingTool>(this);
#endif

    declareProperty("Seed", m_seed);
  }
    

  StatusCode InDetTrackSmearingTool::initialize() {

    // Greet the user:
    ATH_MSG_INFO( "Initializing..." );
    
    ATH_CHECK( initObject<TH1>(m_smearD0Dead, "res_diff_d0_vs_pt.hist.root", "res_pt_d0_0") );
    ATH_CHECK( initObject<TH1>(m_smearZ0Dead, "res_diff_z0_vs_pt.hist.root", "res_pt_z0_0") );
   
    string rootfile = "trackIPAlign_r21.root";
    ATH_CHECK( initObject<TH2>(m_smearD0, rootfile, "quad_diff/d0quaddiff_comb_Pt_Eta" ) );
    ATH_CHECK( initObject<TH2>(m_smearZ0, rootfile, "quad_diff/z0quaddiff_comb_Pt_Eta" ) );
    ATH_CHECK( initObject<TH2>(m_smearD0_sys_up, rootfile, "quad_diff/d0quaddiff_comb_Pt_Eta_sys_up" ) );
    ATH_CHECK( initObject<TH2>(m_smearZ0_sys_up, rootfile, "quad_diff/z0quaddiff_comb_Pt_Eta_sys_up" ) );
    ATH_CHECK( initObject<TH2>(m_smearD0_sys_dw, rootfile, "quad_diff/d0quaddiff_comb_Pt_Eta_sys_dw" ) );
    ATH_CHECK( initObject<TH2>(m_smearZ0_sys_dw, rootfile, "quad_diff/z0quaddiff_comb_Pt_Eta_sys_dw" ) );

    ATH_MSG_INFO( "Using seed of " << m_seed << " to initialize RNG" );
    m_rnd = make_unique<TRandom3>(m_seed);

    // do common initialization (at time of writing, register affecting systematics)
    ATH_CHECK( InDetTrackSystematicsTool::initialize() );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  InDetTrackSmearingTool::~InDetTrackSmearingTool()
  {
    delete m_smearD0Dead; m_smearD0Dead = nullptr;
    delete m_smearZ0Dead; m_smearZ0Dead = nullptr;

    delete m_smearD0; m_smearD0 = nullptr;
    delete m_smearZ0; m_smearZ0 = nullptr;    
    delete m_smearD0_sys_up; m_smearD0_sys_up = nullptr;
    delete m_smearZ0_sys_up; m_smearZ0_sys_up = nullptr;
    delete m_smearD0_sys_dw; m_smearD0_sys_dw = nullptr;
    delete m_smearZ0_sys_dw; m_smearZ0_sys_dw = nullptr;
  }

  float InDetTrackSmearingTool::GetSmearD0Sigma(const xAOD::TrackParticle& track) {
    float pt = 1.e-3*track.pt(); // need to convert pt to GeV
    float eta = track.eta();
    float sigma_D0 = 0.f;

    if ( isActive(TRK_RES_D0_DEAD) ) {
      float d0Smear = m_smearD0Dead->GetBinContent(m_smearD0Dead->FindBin(pt));
      sigma_D0 += d0Smear*d0Smear;
    }

    bool isActiveD0Meas = isActive(TRK_RES_D0_MEAS);
    bool isActiveD0MeasUp = isActive(TRK_RES_D0_MEAS_UP);
    bool isActiveD0MeasDown = isActive(TRK_RES_D0_MEAS_DOWN);

    if (isActiveD0Meas + isActiveD0MeasUp + isActiveD0MeasDown > 1) {
      // this should be checked against in applySystematicVariation
      throw std::runtime_error( "Multiple incompatible D0 systematics are activated." );
    }

    if ( isActiveD0Meas || isActiveD0MeasUp || isActiveD0MeasDown 
         || isActive(TRK_RES_D0Z0Corl_MEAS) || isActive(TRK_RES_D0Z0_MEAS) ) {
      TH2* d0hist = m_smearD0;
      if(isActiveD0MeasUp) d0hist = m_smearD0_sys_up;
      if(isActiveD0MeasDown) d0hist = m_smearD0_sys_dw;
                                               
      if (d0hist == nullptr) ATH_MSG_ERROR( "d0 histogram is null!" );
      float d0Smear = readHistogram(d0hist, pt, eta);
      sigma_D0 += d0Smear*d0Smear;
    }

    return std::sqrt(sigma_D0);
  }   
   
  
  float InDetTrackSmearingTool::GetSmearZ0Sigma(const xAOD::TrackParticle& track) {
    float pt = 1.e-3*track.pt(); // need to convert pt to GeV
    float eta = track.eta();
    float sigma_Z0 = 0.f;

    if ( isActive(TRK_RES_Z0_DEAD) ) {
      // the histogram returns a smeared value for z0*sin(theta), so we need to divide by sin(theta)
      float z0Smear = m_smearZ0Dead->GetBinContent(m_smearZ0Dead->FindBin(pt))/std::sin(track.theta());
      sigma_Z0 += z0Smear*z0Smear;
    }


    bool isActiveZ0Meas = isActive(TRK_RES_Z0_MEAS);
    bool isActiveZ0MeasUp = isActive(TRK_RES_Z0_MEAS_UP);
    bool isActiveZ0MeasDown = isActive(TRK_RES_Z0_MEAS_DOWN);

    if (isActiveZ0Meas + isActiveZ0MeasUp + isActiveZ0MeasDown > 1) {
      // this should be checked against in applySystematicVariation
      throw std::runtime_error( "Multiple incompatible Z0 systematics are activated." );
    }

    if ( isActiveZ0Meas || isActiveZ0MeasUp || isActiveZ0MeasDown 
         || isActive(TRK_RES_D0Z0Corl_MEAS) || isActive(TRK_RES_D0Z0_MEAS) ) {
      TH2* z0hist = m_smearZ0;
      if(isActiveZ0MeasUp) z0hist = m_smearZ0_sys_up;
      if(isActiveZ0MeasDown) z0hist = m_smearZ0_sys_dw;
      if (z0hist == nullptr) ATH_MSG_ERROR( "z0 histogram is null!" );
      float z0Smear = readHistogram(z0hist, pt, eta);
      sigma_Z0 += z0Smear*z0Smear;
    }

    return std::sqrt(sigma_Z0);
  }
 
  CP::CorrectionCode InDetTrackSmearingTool::applyCorrection( xAOD::TrackParticle& track ) {
    float sigmaD0 = GetSmearD0Sigma( track );
    float sigmaZ0 = GetSmearZ0Sigma( track );

    static SG::AuxElement::Accessor< float > accD0( "d0" );
    static SG::AuxElement::Accessor< float > accZ0( "z0" );

    //NB: only call the RNG if the widths are greater than 0

    if(isActive(TRK_RES_D0Z0Corl_MEAS)){ 
      //for correlation studies: use same random number to smear both d0 and z0
      double rnd1 = m_rnd->Gaus();
      if(sigmaD0 > 0. ) accD0( track ) = track.d0() + (rnd1*sigmaD0);
      if(sigmaZ0 > 0. ) accZ0( track ) = track.z0() + (rnd1*sigmaZ0);
    } else {
      if ( sigmaD0 > 0. ) accD0( track ) = m_rnd->Gaus( track.d0(), sigmaD0 );
      if ( sigmaZ0 > 0. ) accZ0( track ) = m_rnd->Gaus( track.z0(), sigmaZ0 );
    }

    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode InDetTrackSmearingTool::correctedCopy( const xAOD::TrackParticle& in,
							    xAOD::TrackParticle*& out )
  {
    return TrackCorrTool_t::correctedCopy(in, out);
  }
  
  CP::CorrectionCode InDetTrackSmearingTool::applyContainerCorrection( xAOD::TrackParticleContainer& cont )
  {
    return TrackCorrTool_t::applyContainerCorrection(cont);
  }

  bool InDetTrackSmearingTool::isAffectedBySystematic( const CP::SystematicVariation& syst ) const
  {
    return InDetTrackSystematicsTool::isAffectedBySystematic( syst );
  }

  CP::SystematicSet InDetTrackSmearingTool::affectingSystematics() const
  {
    return SmearingSystematics;
  }

  CP::SystematicSet InDetTrackSmearingTool::recommendedSystematics() const
  {
    //    return InDetTrackSystematicsTool::recommendedSystematics();
    return RecommendedSystematics;
  }

  CP::SystematicCode InDetTrackSmearingTool::applySystematicVariation( const CP::SystematicSet& systs )
  {
    // should do check in here and return CP::SystematicCode::Unsupported if LOOSE and TIGHT versions are set simultaneously
    return InDetTrackSystematicsTool::applySystematicVariation(systs);
  }

  float InDetTrackSmearingTool::readHistogram(TH2* histogram, float pt, float eta) const {
    // safety measure:
    if( eta>2.499 )  eta= 2.499;
    if( eta<-2.499 ) eta=-2.499;
    if ( pt >= histogram->GetXaxis()->GetXmax() ) {
      pt = histogram->GetXaxis()->GetXmax() - 0.01;
    }

    int bin = histogram->FindFixBin(pt, eta);
    float val = histogram->GetBinContent(bin);
    return val;
  }

} // namespace InDet

