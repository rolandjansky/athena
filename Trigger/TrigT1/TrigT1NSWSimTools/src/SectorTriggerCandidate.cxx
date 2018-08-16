/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/SectorTriggerCandidate.h"
#include "TrigT1NSWSimTools/PadWithHits.h" // shouldn't be needed, need to fix it DG

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


Polygon Stc::triggerRegion3() const{
  assert(m_wedgeTrigs.size()>0); // cannot have any trig with any wedge
  bool hasBothInnerOuter(m_wedgeTrigs.size()>1);
  if(hasBothInnerOuter){
    const SingleWedgePadTrigger &inner = m_wedgeTrigs[0];
    const SingleWedgePadTrigger &outer = m_wedgeTrigs[1];
    Polygon inner_overlap=SingleWedgePadTrigger::padOverlap3(inner.pads());
    Polygon outer_overlap=SingleWedgePadTrigger::padOverlap3(outer.pads());
    //project the outer overlap into first one's plane
    float Z1=outer.pads()[0].m_cornerXyz[1][2];
    float Z0=inner.pads()[0].m_cornerXyz[1][2];
    //write the outer overlap in Z0 and overlap inner+outer to calculate the overall overlap laplaplap
    
    //mproject ? from Z1->Z0
    return GeoUtils::largestIntersection(inner_overlap,GeoUtils::Project(outer_overlap,Z1,Z0));
    //end of S.I
  } else {
    return SingleWedgePadTrigger::padOverlap3(m_wedgeTrigs[0].pads());
  }
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
