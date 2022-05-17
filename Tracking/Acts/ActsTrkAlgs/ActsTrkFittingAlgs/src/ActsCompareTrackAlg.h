/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSCOMPARETRACKALG_H
#define ACTSGEOMETRY_ACTSCOMPARETRACKALG_H

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
// STL
#include <memory>
#include <string>

// FIXME: header-global macro for an array size constant!
#ifndef MAXPROBES
#define MAXPROBES 50000
#endif

class EventContext;

class TTree;
class TH1;
class TH1F;

class ActsCompareTrackAlg : public AthAlgorithm {
public:
  ActsCompareTrackAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

private:

  // --- job options
  std::string m_atlasTrackName;
  std::string m_actsTrackName;

  std::string m_validationTreeName;        //!< validation tree name - to be acessed by this from root
  std::string m_validationTreeDescription; //!< validation tree description - second argument in TTree
  std::string m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out

  TTree *m_validationTree{}; //!< Root Validation Tree
  /** Ntuple variables : initial parameters
        Split this out into a separate, dynamically-allocated block.
        Otherwise, the CaloCellNoiseAlg is so large that it violates
        the ubsan sanity checks. **/
  struct TreeData
  {
    /** Ntuple variables : event and track */
    mutable int m_track_nb{0};
    /** Ntuple variables : ATLAS tracks */
    mutable float m_trk_d0[MAXPROBES]{0};
    mutable float m_trk_z0[MAXPROBES]{0};
    mutable float m_trk_phi[MAXPROBES]{0};
    mutable float m_trk_theta[MAXPROBES]{0};
    mutable float m_trk_q_p[MAXPROBES]{0};

    mutable float m_trk_sigma_d0[MAXPROBES]{0};
    mutable float m_trk_sigma_z0[MAXPROBES]{0};
    mutable float m_trk_sigma_phi[MAXPROBES]{0};
    mutable float m_trk_sigma_theta[MAXPROBES]{0};
    mutable float m_trk_sigma_q_p[MAXPROBES]{0};

    mutable int m_trk_pixelhits[MAXPROBES]{0};
    mutable int m_trk_pixelholes[MAXPROBES]{0};
    mutable int m_trk_pixeloutliers[MAXPROBES]{0};
    mutable int m_trk_scthits[MAXPROBES]{0};
    mutable int m_trk_sctholes[MAXPROBES]{0};
    mutable int m_trk_sctoutliers[MAXPROBES]{0};

    mutable float m_trk_x[MAXPROBES]{0};
    mutable float m_trk_y[MAXPROBES]{0};
    mutable float m_trk_z[MAXPROBES]{0};
    mutable float m_trk_eta[MAXPROBES]{0};
    mutable float m_trk_pt[MAXPROBES]{0};

    /** Ntuple variables : Acts Tracks */
    mutable float m_acts_d0[MAXPROBES]{0};
    mutable float m_acts_z0[MAXPROBES]{0};
    mutable float m_acts_phi[MAXPROBES]{0};
    mutable float m_acts_theta[MAXPROBES]{0};
    mutable float m_acts_q_p[MAXPROBES]{0};

    mutable float m_acts_sigma_d0[MAXPROBES]{0};
    mutable float m_acts_sigma_z0[MAXPROBES]{0};
    mutable float m_acts_sigma_phi[MAXPROBES]{0};
    mutable float m_acts_sigma_theta[MAXPROBES]{0};
    mutable float m_acts_sigma_q_p[MAXPROBES]{0};

    mutable int m_acts_pixelhits[MAXPROBES]{0};
    mutable int m_acts_pixelholes[MAXPROBES]{0};
    mutable int m_acts_pixeloutliers[MAXPROBES]{0};
    mutable int m_acts_scthits[MAXPROBES]{0};
    mutable int m_acts_sctholes[MAXPROBES]{0};
    mutable int m_acts_sctoutliers[MAXPROBES]{0};

    mutable float m_acts_x[MAXPROBES]{0};
    mutable float m_acts_y[MAXPROBES]{0};
    mutable float m_acts_z[MAXPROBES]{0};
    mutable float m_acts_eta[MAXPROBES]{0};    
    mutable float m_acts_pt[MAXPROBES]{0};


    };
    std::unique_ptr<TreeData> m_treeData;

    TH1* m_diff_d0{};
    TH1* m_diff_z0{};
    TH1* m_diff_phi{};
    TH1* m_diff_theta{};
    TH1* m_diff_q_p{};
    
    TH1* m_sigma_d0{};
    TH1* m_sigma_z0{};
    TH1* m_sigma_phi{};
    TH1* m_sigma_theta{};
    TH1* m_sigma_q_p{};
    TH1F** m_cov_comparison{};

};

#endif 
