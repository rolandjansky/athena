// emacs : this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_PFLOWPSEUDOJETGETTER_H
#define JETRECTOOLS_PFLOWPSEUDOJETGETTER_H
////////////////////////////////////////////
/// \class PFlowPseudoJetGetter
///
/// Specialized PseudoJetGetter for TrackParticle
///  The only thing done here is to specialize the templated append method and 
///  define a property for the TrackVertexAssociation
/// \author P-A Delsart
//////////////////////////////////////////////////

#include "JetRec/PseudoJetGetter.h"
#include "AsgTools/ToolHandle.h"
#include "PFlowUtils/IRetrievePFOTool.h"

namespace jet{
  class TrackVertexAssociation;
}


namespace PFlowPJHelper {
  class PseudoJetFiller;
}
class PFlowPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(PFlowPseudoJetGetter, IPseudoJetGetter)
public:
  friend class PFlowPJHelper::PseudoJetFiller;
  PFlowPseudoJetGetter(const std::string &name);

  virtual int appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const;


protected:
  virtual jet::IConstituentUserInfo*
  buildCUI(const xAOD::IParticle* ppar, jet::IConstituentUserInfo::Index idx,
           const LabelIndex* pli) const {return PseudoJetGetter::buildCUI(ppar,idx,pli);}

  ToolHandle<CP::IRetrievePFOTool> m_retrievePFOTool;    /// Retrieval tool
  bool m_inputIsEM;   /// If true EM clusters are used for neutral PFOs.
  bool m_calibrate;   /// If true, EM PFOs are calibrated to LC.
};

#endif
