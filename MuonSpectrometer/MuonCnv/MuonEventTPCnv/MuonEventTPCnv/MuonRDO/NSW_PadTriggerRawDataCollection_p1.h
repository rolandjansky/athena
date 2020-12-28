#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACOLLECTION_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACOLLECTION_P1_H

#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawData_p1.h"

#include <vector>

namespace Muon {
struct NSW_PadTriggerRawDataCollection_p1 : public std::vector<NSW_PadTriggerRawData_p1> {
    unsigned int m_identifierHash;
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACOLLECTION_P1_H
