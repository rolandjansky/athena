// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSW_SECTORTRIGGERCANDIDATE_H
#define NSW_SECTORTRIGGERCANDIDATE_H

#include "SingleWedgePadTrigger.h"
#include "TrigT1NSWSimTools/GeoUtils.h"
#include <string>
#include <vector>

/*!

A trigger trigger candidate for a stgc sector

davide.gerbaudo@gmail.com, April 2013
*/

namespace nsw {
  class SingleWedgePadTrigger;
}

namespace nsw {
  class SectorTriggerCandidate {
  public:
    SectorTriggerCandidate(const nsw::SingleWedgePadTrigger &inner,
                           const nsw::SingleWedgePadTrigger &outer);
    SectorTriggerCandidate(const nsw::SingleWedgePadTrigger &innerOrOuterInTransition);
    std::string pickle() const; //!< simple dict-like representation
    const std::vector< nsw::SingleWedgePadTrigger >& wedgeTrigs() const { return m_wedgeTrigs;}
    GeoUtils::Polygon triggerRegion3() const;

  private:
    std::vector< nsw::SingleWedgePadTrigger > m_wedgeTrigs;
  }; // end class SectorTriggerCandidate

} // end namespace nsw

#endif
