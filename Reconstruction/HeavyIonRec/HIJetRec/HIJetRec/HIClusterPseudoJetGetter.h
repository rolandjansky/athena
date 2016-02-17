/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HICLUSTERPSEUDOJETGETTER_H
#define HIJETREC_HICLUSTERPSEUDOJETGETTER_H
////////////////////////////////////////////
/// \class HIClusterPseudoJetGetter
///
/// Specialized PseudoJetGetter for HIClusters, handles neg E/ghosts in way needed for HI jets
/// \author Aaron Angerami
//////////////////////////////////////////////////

#include "JetRec/PseudoJetGetter.h"


class HIClusterPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(HIClusterPseudoJetGetter, IPseudoJetGetter)
public:
  HIClusterPseudoJetGetter(const std::string &name);

  virtual int appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const;

};

#endif
