/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
///  RetrievePFOTool - Tool to fetch the pflow
///  InputIsEM - If true, EM-scale is used for the neutral pflow
///  CalibratePFO - If true the EM-scale pflow is calibrated
///  UseNeutral - If true, the neutral component of pflow is used
///  UseCharged - If true, the charged component of pflow is used
/// \author P-A Delsart, D. Adams
//////////////////////////////////////////////////

#include "JetRec/PseudoJetGetter.h"
#include "AsgTools/ToolHandle.h"
#include "PFlowUtils/IRetrievePFOTool.h"
#include "PFlowUtils/IWeightPFOTool.h"

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
  ToolHandle<CP::IWeightPFOTool> m_weightPFOTool;    /// Retrieval tool
  bool m_inputIsEM;   /// If true EM clusters are used for neutral PFOs.
  bool m_calibrate;   /// If true, EM PFOs are calibrated to LC.
  bool m_useneutral;  // IF true, neutral pflow is included.
  bool m_usecharged;  // IF true, neutral pflow is included.
  bool m_usevertices; //If true, then we make use of the primary vertex information
  bool m_useChargedWeights; //If true, them we make use of the charged PFO weighting scheme
  bool m_useTrackToVertexTool; //If true, use jet tracktovertex tool
  bool m_applyChargedHadronSubtraction; //If true, remove charged hadrons not matched to PV
  std::string m_trkVtxAssocName; //Name of track-vertex association container
};

#endif
