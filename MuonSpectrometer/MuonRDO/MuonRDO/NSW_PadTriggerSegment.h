#ifndef MUONRDO_NSW_PADTRIGGERSEGMENT_H
#define MUONRDO_NSW_PADTRIGGERSEGMENT_H

#include "GaudiKernel/MsgStream.h"

#include <array>
#include <sstream>
#include <string>

namespace Muon {
class NSW_PadTriggerSegment {
friend class NSW_PadTriggerSegmentCnv_p1;
public:
    NSW_PadTriggerSegment() = default;
    NSW_PadTriggerSegment(uint8_t bandID, uint8_t phiID, std::array<uint8_t, 2> activeLayers);

    uint8_t bandID() const noexcept;
    uint8_t phiID() const noexcept;
    std::array<uint8_t, 2> activeLayers() const noexcept;

    std::string string() const;

    friend MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerSegment& rhs);
    friend std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerSegment& rhs);
private:
    uint8_t m_bandID;
    uint8_t m_phiID;
    // aka "coincidences", active layer count in each wedge ([0] = inner, [1] = outer)
    std::array<uint8_t, 2> m_activeLayers;
};
} // namespace Muon

#endif // MUONRDO_NSW_PADTRIGGERSEGMENT_H