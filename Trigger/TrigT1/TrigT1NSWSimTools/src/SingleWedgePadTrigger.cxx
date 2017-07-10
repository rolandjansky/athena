/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SingleWedgePadTrigger.h"

#include "PadWithHits.h"
#include "vector_utils.h"
#include "tdr_compat_enum.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

using nsw::SingleWedgePadTrigger;
typedef SingleWedgePadTrigger Swpt; // just to get shorter lines
using nsw::EtaPhiRectangle;


bool sortByLayer(const nsw::PadWithHits &p0, const nsw::PadWithHits& p1) {
  return p0.layer<p1.layer;
}
Swpt::SingleWedgePadTrigger(const std::string &pattern,
                            const vpads_t &pads,
                            const std::vector<size_t> &padIndices) :
  m_pattern(pattern), m_halfPadIndices(-999,-999), m_padIndices(padIndices),
  m_alreadyCombined(false)
{
  //std::cout<<"SingleWedgePadTrigger: "<<pattern<<" padIndices :"<<nsw::vec2str(padIndices)<<std::endl;
  assert(m_padIndices.size()>0); // a trigger without pads doesn't make sense
  for(size_t i=0; i<padIndices.size(); ++i) m_pads.push_back(pads[padIndices[i]]);
  std::sort(m_pads.begin(), m_pads.end(), sortByLayer);
  m_halfPadIndices = halfPadCoordinates();
}

std::string Swpt::EtaPhi::str() const {
  std::ostringstream oss;
  oss<<"("<<ieta<<", "<<iphi<<")";
  return oss.str();
}

Swpt::EtaPhiHalf Swpt::halfPadCoordinates() const {
  const vpads_t &pads = m_pads;
  using std::cout; using std::endl;
  size_t nPads(pads.size());
  bool haveEnoughPads(nPads>2);
  if(not haveEnoughPads)
      std::cerr<<"Swpt::halfPadCoordinates: need at least 3 pads"<<endl;
  assert(haveEnoughPads);
  const PadWithHits &pad0=pads[0], &pad1=pads[1], &pad2=pads[2];
  int l0(pad0.layer), l1(pad1.layer), l2(pad2.layer);
  // DG: Are we assuming that the pad indices are sorted? this assumption might not be always valid
  // DG: Should we also cross-check the 4th pad if present?
  bool missLast  (l0==STGC_LAYER_1 && l1==STGC_LAYER_2); // missLast also includes 4 out of 4.
  bool missMiddle(l0==STGC_LAYER_1 && l2==STGC_LAYER_4);
  bool missFirst (l0==STGC_LAYER_2 && l1==STGC_LAYER_3);
  bool validLayerCombination(missLast || missMiddle || missFirst);
  if(!validLayerCombination) cout<<"buggy layer combination? layers: "<<l0<<","<<l1<<","<<l2<<endl;
  assert(validLayerCombination); // probably got a pattern we don't know how to interpret
  EtaPhiHalf pos(-999,-999);
  EtaPhi posA(-999,-999), posB(-999,-999);
  if       (missLast) { posA = EtaPhi(pad0.ieta, pad0.iphi); posB = EtaPhi(pad1.ieta, pad1.iphi); }
  else if(missMiddle) { posA = EtaPhi(pad0.ieta, pad0.iphi); posB = EtaPhi(pad2.ieta, pad2.iphi); }
  else if (missFirst) { posA = EtaPhi(pad1.ieta, pad1.iphi); posB = EtaPhi(pad0.ieta, pad0.iphi); }
  pos.ieta = ((posA.ieta==posB.ieta) ? (posA.ieta * 2) : (posA.ieta * 2 + 1));
  pos.iphi = ((posA.iphi==posB.iphi) ? (posA.iphi * 2) : (posA.iphi * 2 + 1));

  return pos;
}

bool Swpt::isSmallSector() const
{ 
    // small sectors are the even ones (when counting 1-16, but sectorId is 0-15)
    const PadWithHits &firstPad = m_pads[0];
    bool isEven = ((firstPad.sector + 1) %2 )==1;
    return isEven;
}

float Swpt::avgEtaFromFirstPad() const
{
  return m_pads[0].avgEta();
}

bool Swpt::isInTransitionRegion() const
{
  bool small(isSmallSector());
  float absEta = std::abs(avgEtaFromFirstPad());
  // DG don't know whether these numbers are from the parameter book...ask Shikma
  const double transitionEtaLow_S [3] = {1.43, 1.73, 2.07};
  const double transitionEtaHigh_S[3] = {1.54, 1.87, 2.37};
  const double transitionEtaLow_L [3] = {1.41, 1.67, 2.05};
  const double transitionEtaHigh_L[3] = {1.53, 1.82, 2.35};
  const double* trEtaLo(small ? transitionEtaLow_S  : transitionEtaLow_L  );
  const double* trEtaHi(small ? transitionEtaHigh_S : transitionEtaHigh_L );
  bool isTr((absEta > trEtaLo[0] && absEta < trEtaHi[0]) ||
            (absEta > trEtaLo[1] && absEta < trEtaHi[1]) ||
            (absEta > trEtaLo[2] && absEta < trEtaHi[2]) );
  return isTr;
}

EtaPhiRectangle Swpt::padOverlap(const vpads_t &pads)
{ // \todo some duplication with halfPadCoordinates()...refactor
  using std::cout; using std::endl;
  size_t nPads(pads.size());
  bool haveEnoughPads(nPads>2);
  if(not haveEnoughPads)
      std::cerr<<"Swpt::halfPadCoordinates: need at least 3 pads"<<endl;
  assert(haveEnoughPads);
  const PadWithHits &pad0=pads[0], &pad1=pads[1], &pad2=pads[2];
  int l0(pad0.layer), l1(pad1.layer), l2(pad2.layer); // DG: Are we assuming that the pad indices are sorted? this assumption might not be always valid
  // identify the two staggered pads (see parameter book)
  bool hasL1L2(l0==STGC_LAYER_1 && l1==STGC_LAYER_2); // also includes 4 out of 4.
  bool hasL1L4(l0==STGC_LAYER_1 && l2==STGC_LAYER_4);
  bool hasL2L3(l0==STGC_LAYER_2 && l1==STGC_LAYER_3);
  bool validLayerCombination(hasL1L2 || hasL1L4 || hasL2L3);
  if(!validLayerCombination) cout<<"buggy layer combination? layers: "<<l0<<","<<l1<<","<<l2<<endl;
  assert(validLayerCombination); // probably got a pattern we don't know how to interpret
  // ASM-2017-07-07
  // ASM-2017-07-07
  // ASM-2017-07-07
  //const PadWithHits &padA = pad0;
  //const PadWithHits &padB = (hasL1L4 ? pad2 : pad1);
  //return EtaPhiRectangle::overlappingRectangle(EtaPhiRectangle(padA.m_loEta, padA.m_hiEta,
  //                                                             padA.m_loPhi, padA.m_hiPhi),
  //                                             EtaPhiRectangle(padB.m_loEta, padB.m_hiEta,
  //                                                             padB.m_loPhi, padB.m_hiPhi));
  EtaPhiRectangle overlap = EtaPhiRectangle::overlappingRectangle(EtaPhiRectangle(pads[0].m_loEta, pads[0].m_hiEta,
                                                                                  pads[0].m_loPhi, pads[0].m_hiPhi),
                                                                  EtaPhiRectangle(pads[1].m_loEta, pads[1].m_hiEta,
                                                                                  pads[1].m_loPhi, pads[1].m_hiPhi));
  for(unsigned int i=2; i<pads.size(); i++) {
    overlap = EtaPhiRectangle::overlappingRectangle(overlap,
                                                    EtaPhiRectangle(pads[i].m_loEta, pads[i].m_hiEta,
                                                                    pads[i].m_loPhi, pads[i].m_hiPhi));
  }
  return overlap;
  // ASM-2017-07-07
  // ASM-2017-07-07
  // ASM-2017-07-07
}

TVector3 Swpt::direction(const nsw::PadWithHits &firstPad,
                         const nsw::EtaPhiRectangle &overlap)
{
  TVector3 v;
  // DG need to think about how this should be implemented
  float z(firstPad.m_cornerXyz[0][2]); // all corners have the same z
  float thetaC(2.0*atan(exp(-overlap.eta()))), phiC(overlap.phi());
  float r(z*tan(thetaC));
  v.SetXYZ(r*sin(thetaC)*cos(phiC), r*sin(thetaC)*sin(phiC), z);
  return v;
}
TVector3 Swpt::direction() const
{
  // DG assuming again pads are sorted by layer
  return SingleWedgePadTrigger::direction(m_pads[0],
                                          SingleWedgePadTrigger::padOverlap(m_pads));
}

std::string vec2pickle(const Swpt::vpads_t &pads){
    std::ostringstream oo;
    oo<<"[";
    for(size_t i=0; i<pads.size(); ++i) oo<<"{"<<pads[i].pickle()<<"}, ";
    oo<<"]";
    return oo.str();
}
std::string Swpt::pickle() const
{
    std::ostringstream oo;
    oo<<"'pattern' : "<<     "'"<<m_pattern<<"'"<<           ", "
      <<"'padIndices' : "       <<vec2str(m_padIndices)<<    ", "
      <<"'alreadyCombined' : "  <<m_alreadyCombined<<        ", "
      <<"'pads' : "             <<vec2pickle(m_pads)<<       ", "
      <<"'halfPadIndices' : "   <<m_halfPadIndices.pickle()<<", ";
    return oo.str();
}

bool Swpt::areInnerOuterConsistent(const EtaPhiHalf &inner, const EtaPhiHalf &outer, bool verbose) {
// ASM-2016-10-4 : Figure out the logic behind this, the staggering changed now relaxing this a bit 
//  bool mismatchEta(outer.ieta < inner.ieta || outer.ieta > inner.ieta + 1);
//  bool mismatchPhi(outer.iphi < inner.iphi || outer.iphi > inner.iphi + 1);
  bool mismatchEta(outer.ieta < inner.ieta - 4 || outer.ieta > inner.ieta + 4);
  bool mismatchPhi(outer.iphi < inner.iphi - 4 || outer.iphi > inner.iphi + 4);
  bool mismatch(mismatchEta || mismatchPhi);
  if(verbose) std::cout<<"areConsistent ? "
                       <<(mismatch ? "no":"yes")
                       <<" : inner "<<inner.str()<<", outer "<<outer.str()
                       <<"(mismatchEta|mismatchPhi) = ("<<mismatchEta<<" | "<<mismatchPhi<<")"
                       <<std::endl;
  return !mismatch;
}

Swpt::EtaPhiHalf Swpt::bandIndices(const EtaPhiHalf &inner, const EtaPhiHalf &outer) {
  // DG check that we should actually multiply *2 twice (it seems to be the case in the 1st implem)
  bool sameEta(inner.ieta == outer.ieta), samePhi(inner.iphi == outer.iphi);
  return EtaPhiHalf((sameEta ? inner.ieta * 2 : inner.ieta * 2 + 1),
                    (samePhi ? inner.iphi * 2 : inner.iphi * 2 + 1));
}
