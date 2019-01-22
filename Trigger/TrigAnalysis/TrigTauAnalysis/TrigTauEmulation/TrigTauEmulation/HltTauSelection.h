/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef HLTTAUSELECTION_H
#define HLTTAUSELECTION_H


#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

//EDM include
#include "xAODTau/TauJet.h"
#include "xAODTracking/TrackParticle.h"

#include "TrigTauEmulation/DecoratedHltTau.h"

// TAU ID
class HltTauID
{
  public:

    // Default Constructor 
    HltTauID();

    // Destructor
    virtual ~HltTauID() {};

    virtual const asg::AcceptInfo& getAcceptInfo() const;

    // Get the decision
    virtual asg::AcceptData accept(const xAOD::TauJet * hltau) const;

  private:

    asg::AcceptInfo m_accept;

    std::string m_id_level;
    bool m_use_tauid;

  public:

    void SetIdLevel(std::string s) {m_id_level=s;}
    void SetUseTauID(bool b) {m_use_tauid=b;}

};

// CALORIMETRIC PRESELECTION (PT CUT AND CENTRALITY FRACTION CUT)
class HltTauCaloPresel
{
  public:

    // Default Constructor 
    HltTauCaloPresel();

    // Destructor
    virtual ~HltTauCaloPresel() {};

    virtual const asg::AcceptInfo& getAcceptInfo() const;

    // Get the decision
    virtual asg::AcceptData accept(const xAOD::TauJet * hltau) const;

  private:

    asg::AcceptInfo m_accept;

    bool m_use_presel;
    bool m_use_calo_presel;
    double m_pt;
    double m_centfrac;
    std::string m_centfrac_strategy;

    double topo_cluster_pt(const xAOD::TauJet * hltau) const;
    double centfrac_cut_ptdep(double pt_mev) const;
    bool centfrac_cut(const xAOD::TauJet * hltau, std::string strat) const;

  public:

    void SetUsePresel(bool b) {m_use_presel=b;}
    void SetUseCaloPresel(bool b) {m_use_calo_presel=b;}
    void SetPreselPt(double d) {m_pt=d;}
    void SetPreselCentFracCut(double d) {m_centfrac=d;}
    void SetPreselCentFracStrategy(std::string s) {m_centfrac_strategy=s;}

};

// FAST-TRACKING SELECTION
class FastTrackSelection
{

  public:

    // Default Constructor 
    FastTrackSelection();

    // Destructor
    virtual ~FastTrackSelection() {};

    virtual const asg::AcceptInfo& getAcceptInfo() const;

    // Get the decision for a specific FastTrackRoI
    virtual asg::AcceptData accept(const xAOD::TauJet * hltau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const;
    virtual asg::AcceptData accept(const xAOD::TauJet * hltau, const DataVector<xAOD::TrackParticle> * tracks) const;
    virtual asg::AcceptData accept(const xAOD::TauJet * presel_tau) const;

  private:

    bool trackSelection(const xAOD::TauJet *hlt_tau, const xAOD::TrackParticle *leadingTrack, const DataVector<xAOD::TrackParticle> *tracks) const;
    const xAOD::TrackParticle* findLeadingTrack(const xAOD::TauJet*, const DataVector<xAOD::TrackParticle>*) const;

    asg::AcceptInfo m_accept;

    bool m_use_fasttracking;
    double m_d0;
    double m_z0;
    double m_delta_z0;
    double m_core;
    double m_iso;
    int m_ncore_bound;
    int m_niso_bound;

  public:

    void SetUseFastTracking(bool b) {m_use_fasttracking=b;}
    void SetTrackD0(double d) {m_d0=d;}
    void SetTrackZ0(double d) {m_z0=d;}
    void SetDeltaZ0(double d) {m_delta_z0=d;}
    void SetCoreConeDr(double d) {m_core=d;}
    void SetIsoConeDr(double d) {m_iso=d;}
    void SetNcore(int i) {m_ncore_bound=i;}
    void SetNiso(int i) {m_niso_bound=i;}
};
#endif

