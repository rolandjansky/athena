/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: TrigParticleCreator.h
//
// author: Patricia Conde Muino 13/09/2005
//         Patricia.Conde.Muino@cern.ch
//
// Description:  Seeded version of the offline ParticleCreator
//
// Modified :
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGPARTICLECREATION_PARTICLECREATOR_H
#define INDETTRIGPARTICLECREATION_PARTICLECREATOR_H

#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

// STL
#include <map>
#include <string>

class StoreGateSvc;
class VxContainer;
class TH1F;


namespace Trk { class RecVertex; }
namespace Trk { class ITrackParticleCreatorTool; }
namespace Trk { class IPRD_AssociationTool; }
namespace Rec { class TrackParticleContainer; }
namespace Rec { class TrackParticle; }

namespace InDet
{


  class TrigParticleCreator : public HLT::FexAlgo
  {
  public:
    TrigParticleCreator(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigParticleCreator();
    HLT::ErrorCode hltBeginRun();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltEndRun();

  private:
    void ResetMon();
    void FillMonPerTrack(const Rec::TrackParticle *, const double&, const double&);
    void FillMonPerRoi(const double&, const double&);

  private:
    ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
    ToolHandle< Trk::IPRD_AssociationTool > m_assoTool; /** used to work out which (if any) PRDs are shared between tracks*/

    Rec::TrackParticleContainer* m_trackPC;
    const TrackCollection*       m_allTracksFromStoreGate;
    const VxContainer*           m_primcontainer;
    bool m_doSharedHits;

    //+++ SA: Monitoring 
    std::string                  m_slice_name;
    bool                         m_mon_doSliceSpecific;
    int                          m_mon_counter;
    int                          m_mon_prescale;
    float                        m_mon_ptmin;
    //+++ Common Variables
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
    int                         m_dqm_ntrkpart;
    int                         m_dqm_ntrk;
    int                         m_dqm_nvertex;
    //+++ Bjet
    float                       m_dqm_bj_sumpt;
    //+++ Muon
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
    //+++ Tau
    std::vector<float>          m_dqm_ta_roidr;
    float                       m_dqm_ta_L_pt;    //+++ pT of leading (L) pT track
    float                       m_dqm_ta_frac_pt; //+++ fraction pT_L/pT_NL, NL= next-to-leading
    float                       m_dqm_ta_sum_ch;  //+++ Charge sum all tracks 
    float                       m_dqm_ta_asy_ch;  //+++ Charge asymmetry all tracks
    float                       m_dqm_ta_m;       //+++ Inv mass all tracks

  };

}

#endif
