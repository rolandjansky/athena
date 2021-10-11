#ifndef MUONRDO_NSW_PADTRIGGERDATA
#define MUONRDO_NSW_PADTRIGGERDATA

#include "GaudiKernel/MsgStream.h"

#include "AthContainers/DataVector.h"
#include "Identifier/IdentifierHash.h"

#include "MuonRDO/NSW_PadTriggerSegment.h"

#include <sstream>

namespace Muon {
class NSW_PadTriggerData : public DataVector<NSW_PadTriggerSegment> {
using hitlist_t = std::vector<uint16_t>;
public:
    enum class Endcap : uint8_t { C = 0, A = 1 };
    enum class SectorSize : uint8_t { SMALL = 0, LARGE = 1 };

    NSW_PadTriggerData(IdentifierHash identifierHash, uint8_t sectorID, SectorSize sectorSize, Endcap endcap,
        uint32_t BCID, uint32_t L1ID, const std::array<hitlist_t, 3>& hitlists);
    IdentifierHash identifierHash() const;

    std::string string() const;

    uint8_t sectorID() const noexcept;
    SectorSize sectorSize() const noexcept;
    Endcap endcap() const noexcept;
    uint32_t BCID() const noexcept;
    uint32_t L1ID() const noexcept;

    void sectorID(uint8_t sectorID) noexcept;
    void sectorSize(SectorSize sectorSize) noexcept;
    void endcap(Endcap endcap) noexcept;
    void BCID(uint32_t BCID) noexcept;
    void L1ID(uint32_t L1ID) noexcept;
    void hitlists(const std::array<hitlist_t, 3>& hitlists);
    const std::array<hitlist_t, 3>& hitlists() const;

    friend std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerData& rhs);
    friend MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerData& rhs);

private:
    IdentifierHash m_identifierHash;
    uint8_t m_sectorID;
    SectorSize m_sectorSize;
    Endcap m_endcap;
    uint32_t m_BCID;
    uint32_t m_L1ID;
    // List of pad hits, in a 3BC window around the L1A BC
    std::array<hitlist_t, 3> m_hitlists;
};
} // namespace Muon

#endif // MUONRDO_NSW_PADTRIGGERDATA
