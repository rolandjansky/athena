/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: TrigTrackingxAODCnv.h
//
// author: Jiri Masik
//         Jiri.Masik@manchester.ac.uk
//
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGPARTICLECREATION_TRACKINGXAODCNVMT_H
#define INDETTRIGPARTICLECREATION_TRACKINGXAODCNVMT_H

#include "GaudiKernel/ToolHandle.h"

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


#include "TrkTrack/TrackCollection.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

// STL
#include <map>
#include <string>

class StoreGateSvc;
class TH1F;
class AtlasDetectorID;
class Identifier;
class PixelID;
 

namespace Trk {
  class ITrackParticleCreatorTool;
  class IResidualPullCalculator;
}



namespace InDet
{


  class TrigTrackingxAODCnvMT : public AthAlgorithm
  {
  public:
    TrigTrackingxAODCnvMT(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigTrackingxAODCnvMT();
    StatusCode beginRun();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    StatusCode endRun();

  private:
    void ResetMon();
    void FillMonPerTrack(const xAOD::TrackParticle *, const double&, const double&);
    void FillMonPerRoi(const TrigRoiDescriptor* , const TrackCollection* tracks, const double&, const double&);
    void fillIBLResidual(const Trk::Track *track);

  private:
    ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
    ToolHandle< Trk::IResidualPullCalculator > m_residualCalc;
    const AtlasDetectorID* m_idHelper;
    const PixelID* m_pixelId;

    bool                         m_doIBLresidual;

    //Monitoring
    std::string                  m_slice_name;
    bool                         m_mon_doSliceSpecific;
    int                          m_mon_counter;
    int                          m_mon_prescale;
    float                        m_mon_ptmin;
    //Common Variables
    std::vector<float>          m_dqm_d0;
    std::vector<float>          m_dqm_z0;
    std::vector<float>          m_dqm_phi;
    std::vector<float>          m_dqm_eta;
    std::vector<float>          m_dqm_qOverP;
    std::vector<int>            m_dqm_npix_hits;
    std::vector<int>            m_dqm_nsct_hits;
    std::vector<int>            m_dqm_ntrt_hits;
    std::vector<float>          m_dqm_chi2dof;
    std::vector<float>          m_dqm_eta_roi;
    std::vector<float>          m_dqm_phi_roi;
    double                      m_roiPhi, m_roiEta, m_roiZ;
    double                      m_roiPhiWidth, m_roiEtaWidth, m_roiZ_Width;
    int                         m_dqm_ntrkpart;
    int                         m_dqm_ntrk;
    int                         m_dqm_nvertex;
    //Bjet
    float                       m_dqm_bj_sumpt;
    //Muon
    float                       m_dqm_mu_d0;
    float                       m_dqm_mu_z0;
    float                       m_dqm_mu_eta;
    float                       m_dqm_mu_phi;
    float                       m_dqm_mu_qOverP;
    float                       m_dqm_mu_pt;
    int                         m_dqm_mu_npix_hits;
    int                         m_dqm_mu_nsct_hits;
    int                         m_dqm_mu_ntrt_hits;
    float                       m_dqm_mu_chi2dof;
    float                       m_dqm_mu_eta_roi;
    float                       m_dqm_mu_phi_roi;
    //Tau
    std::vector<float>          m_dqm_ta_roidr;
    float                       m_dqm_ta_L_pt;    //+++ pT of leading (L) pT track
    float                       m_dqm_ta_frac_pt; //+++ fraction pT_L/pT_NL, NL= next-to-leading
    float                       m_dqm_ta_sum_ch;  //+++ Charge sum all tracks
    float                       m_dqm_ta_asy_ch;  //+++ Charge asymmetry all tracks
    float                       m_dqm_ta_m;       //+++ Inv mass all tracks

    //
    std::vector<float>          m_dqm_ibl_z;         //IBL coordinate along z 
    std::vector<float>          m_dqm_ibl_res_x;
    std::vector<float>          m_dqm_ibl_res_y;
    std::vector<float>          m_dqm_ibl_hit_expected_found;

    //Timers
    //TrigTimer* m_timerMemAlloc;
    //TrigTimer* m_timerFeatureAttach;
    //TrigTimer* m_timerTrackConversion;


    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
    SG::ReadHandleKey<TrackCollection> m_trackKey;
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey;

  };

}

#endif
