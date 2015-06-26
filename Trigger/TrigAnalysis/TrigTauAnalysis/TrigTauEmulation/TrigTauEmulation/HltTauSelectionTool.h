/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTTAUSELECTIONTOOL_H
#define HLTTAUSELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"


#include "TrigTauEmulation/HltTauSelection.h"
/* #include "TrigTauEmulation/HltTauCaloPresel.h" */
/* #include "TrigTauEmulation/HltTauID.h" */
/* #include "TrigTauEmulation/FastTrackSelection.h" */

#include "TrigTauEmulation/IHltTauSelectionTool.h"
/* #ifdef ASGTOOL_STANDALONE */
/*  #include "TauDiscriminant/TauDiscriminantTool.h" */
/* #endif */

class HltTauSelectionTool : public virtual IHltTauSelectionTool, public asg::AsgTool

{


  /// Proper constructor for Athena
  ASG_TOOL_CLASS(HltTauSelectionTool, IHltTauSelectionTool)

 public:
  
  HltTauSelectionTool(const std::string & name);

  HltTauSelectionTool(const HltTauSelectionTool & other);


  virtual StatusCode initialize();

  virtual const Root::TAccept& accept(const xAOD::TauJet * hlttau, const DataVector<xAOD::TrackParticle> * tracks) const;
  virtual const Root::TAccept& accept(const xAOD::TauJet * hlttau, const xAOD::TauJetContainer * presel_taus) const;

 private:
  
  mutable Root::TAccept m_accept;

  HltTauCaloPresel* m_calopresel;
  HltTauID* m_tauid;
  FastTrackSelection* m_ftk;

  // calo presel
  bool m_use_presel;
  bool m_use_calo_presel;
  double m_pt;
  double m_centfrac;
  std::string m_centfrac_strategy;
  // ftk
  bool m_use_fasttracking;
  double m_d0;
  double m_z0;
  double m_delta_z0;
  double m_core;
  double m_iso;
  int m_ncore_bound;
  int m_niso_bound;
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
