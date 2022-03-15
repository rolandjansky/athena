/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTMultiTruth.h"
#include <AsgMessaging/MessageCheck.h>

using namespace asg::msgUserCode;

ClassImp(HTTMultiTruth)

// HTTMultiTruth
// ================================================================
// code to match clusters and tracks to GEANT charge deposition
// information
// ================================================================
//   20-04-2009 Antonio Boveia (boveia@hep.uchicago.edu)

long HTTMultiTruth::best_barcode() const {
  Barcode code;
  Weight weight;
  return !best(code, weight) ? -999 : code.second;
}

void HTTMultiTruth::add(const HTTMultiTruth::Barcode& code, const HTTMultiTruth::Weight& weight) {
  assert(weight >= 0.);
  m_truth[code] += weight;
}

void HTTMultiTruth::add(const HTTMultiTruth& rval) {
  for (auto const& rval_truth : rval.m_truth) {
    m_truth[rval_truth.first] += rval_truth.second;
  }
}

// if this doesn't have any truth info, assign maxweight_barcode to be the truth.
// otherwise, this should only have the barcode of the largest weight seen so far;
// compare with the input barcode and keep the best.
void HTTMultiTruth::maximize(const HTTMultiTruth::Barcode& code, const HTTMultiTruth::Weight& weight)
{
  if (m_truth.empty()) m_truth[code] = weight;
  else
  {
    assert(m_truth.size() == 1);
    if (m_truth.begin()->second < weight)
    {
      m_truth.clear();
      m_truth[code] = weight;
    }
  }
}

void HTTMultiTruth::maximize(const HTTMultiTruth& rval)
{
  if (rval.m_truth.empty()) return; // if rval has no truth info, do nothing.

  // find input barcode with maximum weight;
  Weight maxweight = 0.;
  Barcode maxweight_barcode(-1l, -1l);
  for (auto const& rval_truth : rval.m_truth)
  {
    if (rval_truth.second > maxweight)
    {
      maxweight = rval_truth.second;
      maxweight_barcode = rval_truth.first;
    }
  }

  maximize(maxweight_barcode, maxweight);
}

void HTTMultiTruth::assign_equal_normalization()
{
  for (auto& truth : m_truth) truth.second = 1. / m_truth.size();
}

void HTTMultiTruth::display() const
{
  ANA_MSG_INFO("HTTMultiTruth (event index,barcode) <=> weight, entries: " << m_truth.size());

  if (m_truth.empty()) return;

  for (auto const& truth : m_truth)
    ANA_MSG_INFO("   " << (truth.first).first << "," << (truth.first).second << " <=> " << truth.second);

  ANA_MSG_INFO("      best: ");

  Barcode code;
  Weight weight(0);

  if (!best(code, weight)) ANA_MSG_INFO("N/A");
  else ANA_MSG_INFO(code.first << "," << code.second << " " << weight);

}

const HTTMultiTruth HTTMultiTruth::add(const HTTMultiTruth& rval) const
{
  HTTMultiTruth result(*this);
  result.add(rval);
  return result;
}

const HTTMultiTruth HTTMultiTruth::maximize(const HTTMultiTruth& rval) const
{
  HTTMultiTruth result(*this);
  result.maximize(rval);
  return result;
}
