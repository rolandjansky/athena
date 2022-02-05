/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ActsGeantFollowerHelper_H
#define ActsGeantFollowerHelper_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h" //typedef, can't fwd declare
#include "IActsGeantFollowerHelper.h"
#include "G4ThreeVector.hh" //typedef, can't fwd declare
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"

#include "Acts/Surfaces/Surface.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Propagator/SurfaceCollector.hpp"

// FIXME: header-global macro for an array size constant!
#ifndef MAXPROBES
#define MAXPROBES 50000
#endif

class TTree;

namespace Trk
{
  class IExtrapolator;
  class IExtrapolationEngine;
} // namespace Trk

class IActsExtrapolationTool;

class ActsGeantFollowerHelper : public extends<AthAlgTool, IActsGeantFollowerHelper>
{

  public:
    // constructor, destructor
    ActsGeantFollowerHelper(const std::string&,const std::string&,const IInterface*);
    virtual ~ActsGeantFollowerHelper ();

    // Athena hooks
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    // Follower interface
    // a) begin event - initialize follower process
    void beginEvent() const;
    // b) track the particle
    void trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0, bool isSensitive) const;
    // c) end event - ntuple writing
    void endEvent() const;

  private:

    // ToolHandle<Trk::IExtrapolator>       m_extrapolator;
    ToolHandle<Trk::IExtrapolationEngine>     m_extrapolationEngine;
    ToolHandle<IActsExtrapolationTool>   m_actsExtrapolator;
    bool                                 m_extrapolateDirectly;
    bool                                 m_extrapolateIncrementally;

    mutable const Trk::TrackParameters* m_parameterCache;
    mutable std::unique_ptr<const Acts::BoundTrackParameters> m_actsParameterCache;
    mutable std::unique_ptr<std::vector<Acts::SurfaceHit>> m_actsSurfaceCache;
    mutable std::vector<Acts::SurfaceHit>::iterator m_actsSurfaceIterator;
    // Hypothesis to pdg converter
    Trk::PdgToParticleHypothesis m_pdgToParticleHypothesis;
    mutable float m_tX0Cache;
    mutable float m_tX0NonSensitiveCache;
    mutable float m_tNonSensitiveCache;
    mutable float m_tX0CacheActs;
    mutable float m_tX0CacheATLAS;

    // put some validation code is
    std::string                    m_validationTreeName;        //!< validation tree name - to be acessed by this from root
    std::string                    m_validationTreeDescription; //!< validation tree description - second argument in TTree
    std::string                    m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out

    TTree*                         m_validationTree;            //!< Root Validation Tree
    /** Ntuple variables : initial parameters
        Split this out into a separate, dynamically-allocated block.
        Otherwise, the CaloCellNoiseAlg is so large that it violates
        the ubsan sanity checks. **/
    struct TreeData {
        mutable float                  m_t_x {0};
        mutable float                  m_t_y {0};
        mutable float                  m_t_z {0};
        mutable float                  m_t_theta {0};
        mutable float                  m_t_eta {0};
        mutable float                  m_t_phi {0};
        mutable float                  m_t_p {0};
        mutable float                  m_t_charge {0};
        mutable int                    m_t_pdg {0};
        /** Ntuple variables : g4 step parameters */
        mutable int                    m_g4_steps {0};
        mutable float                  m_g4_pt[MAXPROBES] {0};
        mutable float                  m_g4_eta[MAXPROBES] {0};
        mutable float                  m_g4_theta[MAXPROBES] {0};
        mutable float                  m_g4_phi[MAXPROBES] {0};
        mutable float                  m_g4_x[MAXPROBES] {0};
        mutable float                  m_g4_y[MAXPROBES] {0};
        mutable float                  m_g4_z[MAXPROBES] {0};
        mutable float                  m_g4_tX0[MAXPROBES] {0};
        mutable float                  m_g4_accX0[MAXPROBES] {0};        
        mutable float                  m_g4_t[MAXPROBES] {0};
        mutable float                  m_g4_X0[MAXPROBES] {0};
        /** Ntuple variables : trk follow up parameters */
        mutable int                    m_trk_status[MAXPROBES] {0};
        mutable float                  m_trk_pt[MAXPROBES] {0};
        mutable float                  m_trk_eta[MAXPROBES] {0};
        mutable float                  m_trk_theta[MAXPROBES] {0};
        mutable float                  m_trk_phi[MAXPROBES] {0};
        mutable float                  m_trk_x[MAXPROBES] {0};
        mutable float                  m_trk_y[MAXPROBES] {0};
        mutable float                  m_trk_z[MAXPROBES] {0};
        mutable float                  m_trk_lx[MAXPROBES] {0};
        mutable float                  m_trk_ly[MAXPROBES] {0};
        mutable float                  m_trk_tX0[MAXPROBES] {0};
        mutable float                  m_trk_accX0[MAXPROBES] {0};        
        mutable float                  m_trk_t[MAXPROBES] {0};
        mutable float                  m_trk_X0[MAXPROBES] {0};
        /** Ntuple variables : acts follow up parameters */
        mutable int                    m_acts_status[MAXPROBES] {0};
        mutable int                    m_acts_volumeID[MAXPROBES] {0};
        mutable float                  m_acts_pt[MAXPROBES] {0};
        mutable float                  m_acts_eta[MAXPROBES] {0};
        mutable float                  m_acts_theta[MAXPROBES] {0};
        mutable float                  m_acts_phi[MAXPROBES] {0};
        mutable float                  m_acts_x[MAXPROBES] {0};
        mutable float                  m_acts_y[MAXPROBES] {0};
        mutable float                  m_acts_z[MAXPROBES] {0};
        mutable float                  m_acts_tX0[MAXPROBES] {0};
        mutable float                  m_acts_accX0[MAXPROBES] {0};        
        mutable float                  m_acts_t[MAXPROBES] {0};
        mutable float                  m_acts_X0[MAXPROBES] {0};
    };
    std::unique_ptr<TreeData> m_treeData;
};

#endif
