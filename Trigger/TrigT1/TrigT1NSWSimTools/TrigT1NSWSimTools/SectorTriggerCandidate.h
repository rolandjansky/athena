// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSW_SECTORTRIGGERCANDIDATE_H
#define NSW_SECTORTRIGGERCANDIDATE_H

#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/GeoUtils.h"
#include <string>
#include <vector>

/*!

A trigger candidate for a stgc sector

davide.gerbaudo@gmail.com, April 2013
*/


namespace NSWL1 {
  
  class SectorTriggerCandidate {
  public:
    SectorTriggerCandidate(const SingleWedgePadTrigger &inner,
                           const SingleWedgePadTrigger &outer);
    SectorTriggerCandidate(const SingleWedgePadTrigger &innerOrOuterInTransition);
    std::string pickle() const; //!< simple dict-like representation
    const std::vector< SingleWedgePadTrigger >& wedgeTrigs() const { return m_wedgeTrigs;}
    Polygon triggerRegion3() const;

  private:
    std::vector< SingleWedgePadTrigger > m_wedgeTrigs;
  }; // end class SectorTriggerCandidate

} // end namespace nsw

#endif
