/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SectorTriggerCandidate.h"
#include "PadWithHits.h" // shouldn't be needed, need to fix it DG

#include <cassert>
#include <sstream>


using nsw::SectorTriggerCandidate;
typedef SectorTriggerCandidate Stc; // just to get shorter lines



Stc::SectorTriggerCandidate(const nsw::SingleWedgePadTrigger &inner,
                            const nsw::SingleWedgePadTrigger &outer)
{
  m_wedgeTrigs.push_back(inner);
  m_wedgeTrigs.push_back(outer);
}

Stc::SectorTriggerCandidate(const nsw::SingleWedgePadTrigger &innerOrOuterInTransition)
{
  m_wedgeTrigs.push_back(innerOrOuterInTransition);
}

nsw::EtaPhiRectangle Stc::triggerRegion() const
{
  assert(m_wedgeTrigs.size()>0); // cannot have any trig with any wedge
  bool hasBothInnerOuter(m_wedgeTrigs.size()>1);
  if(hasBothInnerOuter){
    const SingleWedgePadTrigger &inner = m_wedgeTrigs[0];
    return SingleWedgePadTrigger::padOverlap(inner.pads());
  } else {
    return SingleWedgePadTrigger::padOverlap(m_wedgeTrigs[0].pads());
  }
}

float Stc::deltaR() const
{
  if(m_wedgeTrigs.size()<2) return 0.0;
  else return m_wedgeTrigs[0].direction().DeltaR(m_wedgeTrigs[1].direction());
}

std::string vec2pickle(const std::vector< nsw::SingleWedgePadTrigger > &trigs){
    std::ostringstream oo;
    oo<<"[";
    for(size_t i=0; i<trigs.size(); ++i) oo<<"{"<<trigs[i].pickle()<<"}, ";
    oo<<"]";
    return oo.str();
}

std::string Stc::pickle() const
{
  std::ostringstream oo;
  oo<<"'wedgeTrigs' : "<<vec2pickle(m_wedgeTrigs)<<", ";
  return oo.str();
}

std::string Stc::summaryString() const
{
  std::ostringstream oo;
  EtaPhiRectangle r(triggerRegion());
  oo<<"SectorTriggerCandidate "
    <<" sector "<<m_wedgeTrigs[0].pads()[0].sector // get the sector from the first pad
    <<" eta ["<<r.etaMin()<<", "<<r.etaMax()<<"]"
    <<" phi ["<<r.phiMin()<<", "<<r.phiMax()<<"]";
  return oo.str();
  
}
