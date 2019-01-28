/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef HLTTAUSELECTIONTOOL_H
#define HLTTAUSELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TrigTauEmulation/HltTauSelection.h"
/* #include "TrigTauEmulation/HltTauCaloPresel.h" */
/* #include "TrigTauEmulation/HltTauID.h" */
/* #include "TrigTauEmulation/FastTrackSelection.h" */

#include "TrigTauEmulation/IHltTauSelectionTool.h"
#include "TrigTauEmulation/SelectionTool.h"
#include "TrigTauEmulation/IFastTrackSelectionTool.h"
/* #ifdef ASGTOOL_STANDALONE */
/*  #include "TauDiscriminant/TauDiscriminantTool.h" */
/* #endif */

class HltTauSelectionTool : public virtual IHltTauSelectionTool, public SelectionTool
{
  ASG_TOOL_CLASS(HltTauSelectionTool, IHltTauSelectionTool)

  public:

    HltTauSelectionTool(const std::string & name);
    HltTauSelectionTool(const HltTauSelectionTool & other);
    HltTauSelectionTool& operator=(const HltTauSelectionTool & other) = delete;
    virtual ~HltTauSelectionTool();

    virtual StatusCode initialize() override;

    virtual const asg::AcceptInfo& getAcceptInfo() const override;

    virtual asg::AcceptData accept(const DecoratedHltTau& hlttau) const override;
    virtual asg::AcceptData accept(const xAOD::TauJet * hlttau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const override;
    virtual asg::AcceptData accept(const xAOD::TauJet * hlttau, const xAOD::TauJetContainer * presel_taus) const override;

  private:

    asg::AcceptInfo m_accept;

    HltTauCaloPresel* m_calopresel;
    HltTauID* m_tauid;
    // --> not needed anymore (central tool now) / will remove soooon
    /* FastTrackSelection* m_ftk; */
    ToolHandle<IFastTrackSelectionTool> m_ftf_tool;

    // calo presel
    bool m_use_presel;
    bool m_use_calo_presel;
    double m_pt;
    double m_centfrac;
    std::string m_centfrac_strategy;
    // ftk
    // --> not needed anymore (central tool now) / will remove soooon
    bool m_use_fasttracking;
    /* double m_d0; */
    /* double m_z0; */
    /* double m_delta_z0; */
    /* double m_core; */
    /* double m_iso; */
    /* int m_ncore_bound; */
    /* int m_niso_bound; */
    // tauID
    std::string m_id_level;
    bool m_use_tauid;

    // tau BDT
    std::string m_TauDiscriminantToolName;
    bool m_recalculateBDTscore;
    /* #ifdef ASGTOOL_STANDALONE */
    /*   ToolHandle<TauDiscriminantTool> *m_tauIDTool; */
    /* #endif */

};
#endif
