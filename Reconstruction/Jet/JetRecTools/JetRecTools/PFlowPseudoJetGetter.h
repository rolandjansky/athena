/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PFlowPseudoJetGetter.h

#ifndef JETRECTOOLS_PFLOWPSEUDOJETGETTER_H
#define JETRECTOOLS_PFLOWPSEUDOJETGETTER_H

////////////////////////////////////////////
/// \class PFlowPseudoJetGetter
///
/// PseudoJetGetter for pflow
///
/// Properties:
///  UseNeutral - If true, the neutral component of pflow is used
///  UseCharged - If true, the charged component of pflow is used
///  UseChargedPV - If true, require charged particles are associated with PV
///  UseChargedPUsideband - If true, require charged particles are in PV sideband
/// \author P-A Delsart, D. Adams
//////////////////////////////////////////////////

#include "JetRec/PseudoJetGetter.h"
#include "AsgTools/ToolHandle.h"

class PFlowPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(PFlowPseudoJetGetter, IPseudoJetGetter)
public:
  PFlowPseudoJetGetter(const std::string &name);

  virtual int appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const;


protected:

  bool m_useCharged;        /// Flag indicating to use charged particles at all
  bool m_useNeutral;        /// Flag indicating to use neutral particles at all
  bool m_useChargedPV;        /// Flag indicating to use charged particles only from PU z0 sideband
  bool m_useChargedPUsideband;        /// Flag indicating to use charged particles only from PU z0 sideband

  virtual jet::IConstituentUserInfo*
  buildCUI(const xAOD::IParticle* ppar, jet::IConstituentUserInfo::Index idx,
           const LabelIndex* pli) const {return PseudoJetGetter::buildCUI(ppar,idx,pli);}

};

#endif
