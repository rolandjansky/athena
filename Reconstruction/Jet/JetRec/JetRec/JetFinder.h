/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetFinder.h

#ifndef JetFinder_H
#define JetFinder_H


// Tool to find jets using fastjet.
// The output collection is of type xAOD::JetContainer.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetFinder.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "xAODJet/JetContainer.h"
#include "AsgTools/ToolHandle.h"
#include "JetRec/PseudoJetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteHandleKey.h"

namespace fastjet {
  class ClusterSequence;
}

/// Tool to find jets.
/// Parameters:
///   JetAlgorithm - Algorithm: Kt, AntiKt, CamKt
///   JetRadius - Size parameter (or maximum value for variable-R finding)
///   VariableRMinRadius - Minimum radius for variable-R jet finding
///   VariableRMassScale - Mass scale [MeV] for variable-R jet finding
///   PtMin - PT [MeV] threshold for jet finding.
///   GhostArea - Approximate (starting) area (dy x dphi) for ghost finding
///   RandomOption - Option for area random seets (0=fastjet default, 1=run,event)
///   JetBuilder - Tool used to build jets, interface IJetFromPseudojet
///                (code now moved into this class).
/// Jet active area is evaluated if GhostArea > 0.
/// Variable-R jet finding is performed if VariableRMinRadius >= 0 and VariableRMassScale >= 0.
/// Units are MeV assuming these are the input units


class JetFinder
: public asg::AsgTool,
  virtual public IJetFinder {
ASG_TOOL_CLASS(JetFinder, IJetFinder)

public:

  JetFinder(std::string name);

  virtual StatusCode initialize() override;

  // Find jets and put them in a container. Save ClusterSequence to evtstore
  virtual int find(const PseudoJetContainer& cont,
                   xAOD::JetContainer & finalJets,
                   xAOD::JetInput::Type contype ) const override;

  // Find jets and put them in a container - no writes to evet store
  virtual int findNoSave(const PseudoJetContainer& cont, 
                         xAOD::JetContainer & finalJets,
                         xAOD::JetInput::Type contype,
                         fastjet::ClusterSequence*& cs) const override;

  // Save a cluster sequence in the event store.
  // So it lives as long as this event.
  void save(fastjet::ClusterSequence* pcs) const;

  // Is variable-finding done?
  bool isVariableR() const;

  // Dump to log.
  virtual void print() const override;

private:  //data
  SG::ReadHandleKey<xAOD::EventInfo> m_eventinfokey{"EventInfo"};
  SG::ReadHandleKey<fastjet::ClusterSequence> m_cnameRKey;
  SG::WriteHandleKey<fastjet::ClusterSequence> m_cnameWKey;

  // Job options.
  std::string m_jetalg;            // Kt, AntiKt, CamKt
  float m_jetrad;                  // Jet size parameter.
  float m_minrad;                  // Variable-R min radius
  float m_massscale;               // Variable-R mass scale
  float m_ptmin;                   // pT min in MeV
  float m_ghostarea;               // Area for ghosts. 0==>no ghosts.
  int m_ranopt;                    // Rand option: 0=fj default, 1=run/event

  ToolHandle<IJetFromPseudojet> m_bld;  

  // Data
  fastjet::JetAlgorithm m_fjalg;
  bool m_isVariableR;

  // Implmentation of find(), findNoSave()
  int _find(const PseudoJetContainer& cont,
            xAOD::JetContainer & finalJets,
            xAOD::JetInput::Type contype,
            bool doSave,
            fastjet::ClusterSequence*&) const;
  
};

#endif
