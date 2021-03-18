#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERSEGMENT_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERSEGMENT_P1_H

#include <cinttypes>

namespace Muon {
struct NSW_PadTriggerSegment_p1 {
    uint8_t m_bandID;
    uint8_t m_phiID;
    uint8_t m_innerActiveLayers;
    uint8_t m_outerActiveLayers;
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERSEGMENT_P1_H