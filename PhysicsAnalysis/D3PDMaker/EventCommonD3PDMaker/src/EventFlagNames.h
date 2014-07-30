// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/EventFlagNames.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Assign names to subdet symbols used in @c EventID.
 */


#ifndef EVENTCOMMOND3PPDMAKER_EVENTFLAGNAMES_H
#define EVENTCOMMOND3PPDMAKER_EVENTFLAGNAMES_H


#include "xAODEventInfo/EventInfo.h"

namespace D3PD {


struct EventFlagNames
{
  xAOD::EventInfo::EventFlagSubDet m_flag;
  const char* m_name;
};


static EventFlagNames eventFlagNames[] = {
  { xAOD::EventInfo::Pixel,      "pixel" },
  { xAOD::EventInfo::SCT,        "sct"   },
  { xAOD::EventInfo::TRT,        "trt"   },
  { xAOD::EventInfo::LAr,        "lar"   },
  { xAOD::EventInfo::Tile,       "tile"  },
  { xAOD::EventInfo::Muon,       "muon"  },
  { xAOD::EventInfo::ForwardDet, "fwd"   },
  { xAOD::EventInfo::Core,       "core"  },
};

static const size_t nEventFlagNames =
  sizeof(eventFlagNames) / sizeof(eventFlagNames[0]);


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_EVENTFLAGNAMES_H
