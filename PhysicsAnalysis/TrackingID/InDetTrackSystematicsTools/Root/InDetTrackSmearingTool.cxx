/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TRandom.h>
#include <TH2F.h>
#include <TFile.h>

// EDM include(s):
#include "CxxUtils/make_unique.h"
#include "xAODEventInfo/EventInfo.h"
//#include "AthenaBaseComps/AthCheckMacros.h"

#include "PathResolver/PathResolver.h"

// Local include(s):
#include "InDetTrackSystematicsTools/InDetTrackSmearingTool.h"

using std::string;
using std::vector;
using CxxUtils::make_unique;

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

    declareProperty("calibFileD0Dead", m_calibFileD0Dead = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/res_diff_d0_vs_pt.hist.root");
    declareProperty("calibFileZ0Dead", m_calibFileZ0Dead = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/res_diff_z0_vs_pt.hist.root");

    declareProperty("calibFileIP_lowpt", m_calibFileIP_lowpt = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/trackIPAlign_dec2017.root");
    declareProperty("calibFileIP_highpt_Data16", m_calibFileIP_highpt_Data16 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v18/trackIPAlignTight.root");
    declareProperty("calibFileIP_highpt_Data17", m_calibFileIP_highpt_Data17 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v19/trackIPAlignTight_data2017.root");
    declareProperty("calibFileIP_dijet_Data16", m_calibFileIP_dijet_Data16 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v20/trackIPAlignTightPrimary_syst_dijet_2016.root");
    declareProperty("calibFileIP_dijet_Data17", m_calibFileIP_dijet_Data17 = "InDetTrackSystematicsTools/CalibData_21.2_2018-v20/trackIPAlignTightPrimary_syst_dijet_2017.root");
    declareProperty("runNumber", m_runNumber);
    declareProperty("useDijetMaps", m_useDijetMaps);

    m_firstTime=true;
  }
    

  StatusCode InDetTrackSmearingTool::initialize() {

    // Greet the user:
    ATH_MSG_INFO( "Initializing..." );

    ATH_MSG_INFO( "Using seed of " << m_seed << " to initialize RNG" );
    m_rnd = make_unique<TRandom3>(m_seed);

    ATH_MSG_INFO( "Using for TRK_RES_D0_DEAD case the calibration file " << PathResolverFindCalibFile(m_calibFileD0Dead) );
    ATH_MSG_INFO( "Using for TRK_RES_Z0_DEAD case the calibration file " << PathResolverFindCalibFile(m_calibFileZ0Dead) );

    if (m_useDijetMaps) {
      ATH_MSG_INFO( "Using dedicated dijet smearing maps for tracks in jets" );
      ATH_MSG_INFO( "Using for the full pT range the dijet Data16 calibration file " << PathResolverFindCalibFile(m_calibFileIP_dijet_Data16) );
      ATH_MSG_INFO( "Using for the full pT range the dijet Data17 calibration file " << PathResolverFindCalibFile(m_calibFileIP_dijet_Data17) );
    } else {
      ATH_MSG_INFO( "Using the low pT (< 15 GeV) calibration file " << PathResolverFindCalibFile(m_calibFileIP_lowpt) );
      ATH_MSG_INFO( "Using the high pT (> 15 GeV) Data16 calibration file " << PathResolverFindCalibFile(m_calibFileIP_highpt_Data16) );
      ATH_MSG_INFO( "Using the high pT (> 15 GeV) Data17 calibration file " << PathResolverFindCalibFile(m_calibFileIP_highpt_Data17) );
    }

    // do common initialization (at time of writing, register affecting systematics)
    ATH_CHECK( InDetTrackSystematicsTool::initialize() );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackSmearingTool::finalize()
  {
    delete m_smearD0Dead; m_smearD0Dead = nullptr;
    delete m_smearZ0Dead; m_smearZ0Dead = nullptr;

    delete m_smearD0_lowpt; m_smearD0_lowpt = nullptr;
    delete m_smearZ0_lowpt; m_smearZ0_lowpt = nullptr;
    delete m_smearD0_lowpt_sys_up; m_smearD0_lowpt_sys_up = nullptr;
    delete m_smearZ0_lowpt_sys_up; m_smearZ0_lowpt_sys_up = nullptr;
    delete m_smearD0_lowpt_sys_dw; m_smearD0_lowpt_sys_dw = nullptr;
    delete m_smearZ0_lowpt_sys_dw; m_smearZ0_lowpt_sys_dw = nullptr;

    delete m_smearD0_highpt; m_smearD0_highpt = nullptr;
    delete m_smearZ0_highpt; m_smearZ0_highpt = nullptr;
    delete m_smearD0_highpt_sys_up; m_smearD0_highpt_sys_up = nullptr;
    delete m_smearZ0_highpt_sys_up; m_smearZ0_highpt_sys_up = nullptr;
    delete m_smearD0_highpt_sys_dw; m_smearD0_highpt_sys_dw = nullptr;
    delete m_smearZ0_highpt_sys_dw; m_smearZ0_highpt_sys_dw = nullptr;

    return StatusCode::SUCCESS;
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
    } else if (isActiveD0Meas + isActiveD0MeasUp + isActiveD0MeasDown == 0) {
      // pass-through D0Meas, return sigma_D0 early
      return std::sqrt(sigma_D0);
    }

    TH2* d0hist_lowpt = nullptr;
    TH2* d0hist_highpt = nullptr;

    if(isActiveD0Meas) {
      d0hist_lowpt = m_smearD0_lowpt;
      d0hist_highpt = m_smearD0_highpt;
    } else if(isActiveD0MeasUp) {
      d0hist_lowpt = m_smearD0_lowpt_sys_up;
      d0hist_highpt = m_smearD0_highpt_sys_up;
    } else if(isActiveD0MeasDown) {
      d0hist_lowpt = m_smearD0_lowpt_sys_dw;
      d0hist_highpt = m_smearD0_highpt_sys_dw;
    }
                                               
    if (d0hist_lowpt == nullptr) ATH_MSG_ERROR( "d0 lowpt histogram is null!" );
    if (d0hist_highpt == nullptr) ATH_MSG_ERROR( "d0 highpt histogram is null!" );

    float d0Smear = 0.f;
    if (pt < 15) {
      d0Smear = readHistogram(d0hist_lowpt, pt, eta);
    } else {
      d0Smear = readHistogram(d0hist_highpt, pt, eta);
    }

    // Apply a minimum smearing in the case where there is an empty bin
    // TODO: Remove for next round of recommendations
    if (d0Smear == 0) {
      d0Smear = 0.002;
    }

    sigma_D0 += d0Smear*d0Smear;


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
    } else if (isActiveZ0Meas + isActiveZ0MeasUp + isActiveZ0MeasDown == 0) {
      // pass-through Z0Meas, return sigma_Z0 early
      return std::sqrt(sigma_Z0);
    }

    TH2* z0hist_lowpt = nullptr;
    TH2* z0hist_highpt = nullptr;

    if(isActiveZ0Meas) {
      z0hist_lowpt = m_smearZ0_lowpt;
      z0hist_highpt = m_smearZ0_highpt;
    } else if(isActiveZ0MeasUp) {
      z0hist_lowpt = m_smearZ0_lowpt_sys_up;
      z0hist_highpt = m_smearZ0_highpt_sys_up;
    } else if(isActiveZ0MeasDown) {
      z0hist_lowpt = m_smearZ0_lowpt_sys_dw;
      z0hist_highpt = m_smearZ0_highpt_sys_dw;
    }

    if (z0hist_lowpt == nullptr) ATH_MSG_ERROR( "z0 lowpt histogram is null!" );
    if (z0hist_highpt == nullptr) ATH_MSG_ERROR( "z0 highpt histogram is null!" );

    float z0Smear = 0.f;
    if (pt < 15) {
      z0Smear = readHistogram(z0hist_lowpt, pt, eta);
    } else {
      z0Smear = readHistogram(z0hist_highpt, pt, eta);
    }

    // Apply a minimum smearing in the case where there is an empty bin
    // TODO: Remove for next round of recommendations
    if (z0Smear == 0) {
      z0Smear = 0.03;
    }

    sigma_Z0 += z0Smear*z0Smear;

    return std::sqrt(sigma_Z0);
  }
 
  CP::CorrectionCode InDetTrackSmearingTool::applyCorrection( xAOD::TrackParticle& track ) {

    if (m_firstTime) {
      m_firstTime = false; // don't try to do this again
      if ( ! firstCall().isSuccess() ) { // this will check the run number
        return CP::CorrectionCode::Error;
      }
    }

    float sigmaD0 = GetSmearD0Sigma( track );
    float sigmaZ0 = GetSmearZ0Sigma( track );

    static SG::AuxElement::Accessor< float > accD0( "d0" );
    static SG::AuxElement::Accessor< float > accZ0( "z0" );

    // only call the RNG if the widths are greater than 0
    if ( sigmaD0 > 0. ) accD0( track ) = m_rnd->Gaus( track.d0(), sigmaD0 );
    if ( sigmaZ0 > 0. ) accZ0( track ) = m_rnd->Gaus( track.z0(), sigmaZ0 );

    return CP::CorrectionCode::Ok;
  }

  StatusCode InDetTrackSmearingTool::initHistograms(int runNumber)
  {

    // For now, the Dead and lowpt histograms can be initialized without knowing the run number
    ATH_CHECK( initObject<TH1>(m_smearD0Dead, m_calibFileD0Dead, "res_pt_d0_0") );
    ATH_CHECK( initObject<TH1>(m_smearZ0Dead, m_calibFileZ0Dead, "res_pt_z0_0") );

    // determine maps based on which year to use
    bool useData17Maps = false;
    if (runNumber <= 0) {
        ATH_MSG_WARNING( "Run number not set." );
    }
    if (runNumber >= 311481 && runNumber <= 341649) {
      ATH_MSG_INFO( "runNumber corresponds to 2017 - using the Data17 smearing maps where appropriate" );
      useData17Maps = true;
    } else {
      ATH_MSG_INFO( "using the Data16 smearing maps where appropriate" );
    }

    string rootfileName_lowpt;
    string rootfileName_highpt;

    if (m_useDijetMaps) {

      // tool configured for tracks in jets
      // same map for lowpt and highpt

      if (useData17Maps) {
        rootfileName_lowpt  = m_calibFileIP_dijet_Data17;
        rootfileName_highpt = m_calibFileIP_dijet_Data17;
      } else {
        rootfileName_lowpt  = m_calibFileIP_dijet_Data16;
        rootfileName_highpt = m_calibFileIP_dijet_Data16;
      }
      ATH_CHECK( initObject<TH2>(m_smearD0_lowpt, rootfileName_lowpt, "quad_diff/d0unf_PtEta_quaddiff_comb" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_lowpt, rootfileName_lowpt, "quad_diff/z0unf_PtEta_quaddiff_comb" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_lowpt_sys_up, rootfileName_lowpt, "quad_diff/d0unf_PtEta_quaddiff_comb_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_lowpt_sys_up, rootfileName_lowpt, "quad_diff/z0unf_PtEta_quaddiff_comb_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_lowpt_sys_dw, rootfileName_lowpt, "quad_diff/d0unf_PtEta_quaddiff_comb_sys_dw" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_lowpt_sys_dw, rootfileName_lowpt, "quad_diff/z0unf_PtEta_quaddiff_comb_sys_dw" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_highpt, rootfileName_highpt, "quad_diff/d0unf_PtEta_quaddiff_comb" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_highpt, rootfileName_highpt, "quad_diff/z0unf_PtEta_quaddiff_comb" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_highpt_sys_up, rootfileName_highpt, "quad_diff/d0unf_PtEta_quaddiff_comb_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_highpt_sys_up, rootfileName_highpt, "quad_diff/z0unf_PtEta_quaddiff_comb_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_highpt_sys_dw, rootfileName_highpt, "quad_diff/d0unf_PtEta_quaddiff_comb_sys_dw" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_highpt_sys_dw, rootfileName_highpt, "quad_diff/z0unf_PtEta_quaddiff_comb_sys_dw" ) );

    } else {

      // nominally use the minbias and Zmumu maps

      // for now, the minbias maps used for lowpt don't have a year dependence
      rootfileName_lowpt = m_calibFileIP_lowpt;
      ATH_CHECK( initObject<TH2>(m_smearD0_lowpt, rootfileName_lowpt, "d0quaddiff_comb_Pt_Eta" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_lowpt, rootfileName_lowpt, "z0quaddiff_comb_Pt_Eta" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_lowpt_sys_up, rootfileName_lowpt, "d0quaddiff_comb_Pt_Eta_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_lowpt_sys_up, rootfileName_lowpt, "z0quaddiff_comb_Pt_Eta_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_lowpt_sys_dw, rootfileName_lowpt, "d0quaddiff_comb_Pt_Eta_sys_dw" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_lowpt_sys_dw, rootfileName_lowpt, "z0quaddiff_comb_Pt_Eta_sys_dw" ) );

      // Zmumu maps for highpt
      if (useData17Maps) {
        rootfileName_highpt = m_calibFileIP_highpt_Data17;
      } else {
        rootfileName_highpt = m_calibFileIP_highpt_Data16;
      }
      ATH_CHECK( initObject<TH2>(m_smearD0_highpt, rootfileName_highpt, "quad_diff/d0quaddiff_comb_Pt_Eta" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_highpt, rootfileName_highpt, "quad_diff/z0quaddiff_comb_Pt_Eta" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_highpt_sys_up, rootfileName_highpt, "quad_diff/d0quaddiff_comb_Pt_Eta_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_highpt_sys_up, rootfileName_highpt, "quad_diff/z0quaddiff_comb_Pt_Eta_sys_up" ) );
      ATH_CHECK( initObject<TH2>(m_smearD0_highpt_sys_dw, rootfileName_highpt, "quad_diff/d0quaddiff_comb_Pt_Eta_sys_dw" ) );
      ATH_CHECK( initObject<TH2>(m_smearZ0_highpt_sys_dw, rootfileName_highpt, "quad_diff/z0quaddiff_comb_Pt_Eta_sys_dw" ) );

    }

    return StatusCode::SUCCESS;
  }

  StatusCode InDetTrackSmearingTool::firstCall()
  {

    const xAOD::EventInfo* ei = nullptr;
    auto sc = evtStore()->retrieve( ei, "EventInfo" );
    if ( ! sc.isSuccess() ) {
      if (m_runNumber <= 0) {
        ATH_MSG_ERROR( "Unable to retrieve from event store. Manually set run number." );
        return CP::CorrectionCode::Error;
      }
    }

    auto runNumber = ei->runNumber();
    if (m_runNumber > 0) {
      if ( m_runNumber != runNumber ) {
        ATH_MSG_WARNING( "Manually-set run number (" << m_runNumber <<
                         ") does not match that from the event store (" << runNumber << ")." );
        ATH_MSG_WARNING( "Will use the manually set run number, but you must make sure this is the desired behaviour!" );
      }
      runNumber = m_runNumber;
    }
    if ( ! initHistograms( runNumber ).isSuccess() ) {
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
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
    if ( pt < histogram->GetXaxis()->GetXmin() ) {
      pt = histogram->GetXaxis()->GetXmin() + 0.01;
    }
    if ( pt >= histogram->GetXaxis()->GetXmax() ) {
      pt = histogram->GetXaxis()->GetXmax() - 0.01;
    }

    int bin = histogram->FindFixBin(pt, eta);
    float val = histogram->GetBinContent(bin);
    return val;
  }

} // namespace InDet

