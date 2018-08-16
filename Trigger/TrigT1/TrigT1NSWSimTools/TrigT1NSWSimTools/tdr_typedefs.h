//  -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSW_TDR_TYPEDEFS_H
#define NSW_TDR_TYPEDEFS_H

/**
   @brief typedefs used in the sTGC TDR code

   see svn+ssh://svn.cern.ch/reps/atlasusr/ataffard/UpgradeNSW/NSWAna/NSWNtuple/trunk

   @author davide.gerbaudo@gmail.com
   @date 2013
*/

#include <string>
#include <vector>

namespace nsw{
  class Pad;
  class PadWithHits;
  class SingleWedgePadTrigger;
  class SectorTriggerCandidate;
}

namespace nsw {
typedef std::vector< bool >                      vbool_t;
typedef std::vector< size_t >                    vsize_t;
typedef std::vector< int >                       vint_t;
typedef std::vector< vint_t >                    vvint_t;
typedef std::vector< nsw::PadWithHits >          vpads_t;
typedef std::vector< SingleWedgePadTrigger >     vswptrig_t;
typedef std::vector< SectorTriggerCandidate >    vstcand_t;
}

#endif
