/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HYPOJETDEFS_H
#define TRIGHLTJETHYPO_HYPOJETDEFS_H
/********************************************************************
 *
 * NAME:     HypoJetDefs.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  typedefs for jets used in TrigHLTJetRec
 *********************************************************************/
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <memory>
#include <vector>
#include <set>
#include <queue>
#include <ostream>

using pHypoJet = std::shared_ptr<const HypoJet::IJet>;
//using pHypoJet = const HypoJet::IJet*;
using HypoJetVector = std::vector<pHypoJet>;
using HypoJetIter = HypoJetVector::iterator;
using HypoJetCIter = HypoJetVector::const_iterator;
using HypoJetSet = std::set<pHypoJet>;
using HypoJetQueue = std::queue<pHypoJet>;
using pairHypoJetIter = std::pair<HypoJetIter, HypoJetIter>;
using pairHypoJetCIter = std::pair<HypoJetCIter, HypoJetCIter>;

using HypoJetGroupVector = std::vector<HypoJetVector>;
using HypoJetGroupIter = HypoJetGroupVector::iterator;
using HypoJetGroupCIter = HypoJetGroupVector::const_iterator;

std::ostream& operator<<(std::ostream& out, const HypoJetVector& hjv);

#endif
