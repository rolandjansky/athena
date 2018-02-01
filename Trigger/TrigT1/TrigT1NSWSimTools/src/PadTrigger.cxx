/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/PadData.h"

using NSWL1::PadData;

namespace NSWL1 {

BcTag_t PadTrigger::bctag() const {
    return m_pads.size() ? m_pads[0]->BC_Tag() : 0x0;
}

// Side ID
int PadTrigger::sideId() const {
  return m_pads.size() ? m_pads[0]->sideId() : -1;
} 

// Sector ID
int PadTrigger::sectorId() const {
  return m_pads.size() ? m_pads[0]->sectorId() : -1;
} 

} // NSWL1

// std::ostream& operator<<(std::ostream& stream, NSWL1::PadTrigger& pt) {
    
//     if(pt.m_pads.size()) {
//         const PadData &data = *pt.m_pads[0]; // for now just grab main info from first pad
//         stream<<"trigger build from "<<pt.m_pads.size()<<" pads"
//               <<" band id "<<pt.m_bandid
//               <<" with first pad: "
//               <<data;
//     } else {
//         stream<<"trigger invalid (no pads)";
//     }
//     return stream;
// }

