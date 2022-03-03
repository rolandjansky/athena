#include "PadTrig_ROD_Decoder.h"

#include <byteswap.h>

#include <vector>

#include "Identifier/Identifier.h"
#include "eformat/Issue.h"
#include "eformat/SourceIdentifier.h"

namespace Muon {

//=====================================================================
PadTrig_ROD_Decoder::PadTrig_ROD_Decoder(const std::string& type, const std::string& name, const IInterface* parent)
: AthAlgTool(type, name, parent)
{
  declareInterface<IPadTrig_ROD_Decoder>(this);
}


//=====================================================================
StatusCode PadTrig_ROD_Decoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& fragment, NSW_PadTriggerDataContainer& rdo) const
{
  try {
    fragment.check();
  } catch (const eformat::Issue& ex) {
    ATH_MSG_ERROR(ex.what());
    return StatusCode::FAILURE;
  }

  eformat::helper::SourceIdentifier sourceID{fragment.rob_source_id()};

  // FIXME this hash calculation MUST be done in an IdHelper!
  // Extract sector from module ID (lowest 4 bits).
  uint32_t hashID = (sourceID.module_id() & 0xf);
  if (sourceID.subdetector_id() == eformat::MUON_STGC_ENDCAP_C_SIDE) {
    // Add 16 sector offset for side C
    hashID += 16;
  }

  if (rdo.indexFind(hashID) != rdo.end()) {
    ATH_MSG_ERROR("Collection ID " << hashID << " already found in RDO container, skipping");
    return StatusCode::FAILURE;
  }

  OFFLINE_FRAGMENTS_NAMESPACE::PointerType data{};
  fragment.rod_data(data);
  if (!data) {
    ATH_MSG_WARNING("No data found for this ROBFragment, skipping");
    return StatusCode::FAILURE;
  }

  // TODO better error handling?
  return parseBytestream(data, fragment.rod_ndata(), rdo, hashID);
}

//=====================================================================
StatusCode PadTrig_ROD_Decoder::parseBytestream(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t size, NSW_PadTriggerDataContainer& rdo, const IdentifierHash hashID) const
{
  // Refer to:
  // https://espace.cern.ch/ATLAS-NSW-ELX/Shared%20Documents/Pad%20Trigger/PadTrig_Raw_Format.pdf
  constexpr std::size_t BITMAP_SIZE = 256;
  const std::size_t EXPECTED_SIZE = [&]() -> std::size_t {
    // netio header + NSW readout header + 3 BCs * (hit_multi + bitmap) + status
    std::size_t packetSize = 8 + 8 + 3 * (2 + BITMAP_SIZE) + 2;
    if (m_pfebBCIDs) packetSize += 3 * 48; // 3 BCs * (2 bytes per BCID * 24 pFEBs)
    if (m_toTPData)  packetSize += 12;
    return (packetSize + 3) / 4;           // Round up to nearest 4-byte multiple
  }();

  if (size != EXPECTED_SIZE) {
    ATH_MSG_ERROR("Corrupted ROBFragment, skipping! Expected " << EXPECTED_SIZE << " words, got " << size);
    return StatusCode::FAILURE;
  }

  const uint16_t bcid       = (extractByte(data, 2) >> 4 & 0xf) << 8 | extractByte(data, 3);
  const uint32_t l1id       = __builtin_bswap32(data[3]);
  const uint8_t  endcap     = (data[2] & (0b1 << 7)) ? 1 : 0; // 1 = Endcap A, 0 = Endcap C
  const uint8_t  sector     = ((data[2] & 0x0f00) >> 8) + 1;  // In range [1, 16]
  const uint8_t  sectorSize = sector % 2; // Small is 0, Large is 1. S is even, L is odd.
  const uint8_t  sectorID   = (sector + sectorSize) / 2; // In range [1, 8]

  // Skip netio + NSW headers
  std::size_t byteIndex{16};

  std::array<std::vector<uint16_t>, 3> hitAddresses;

  // Bitmap parsing loop
  for (std::size_t bc{}; bc < 3; bc++) {
    uint16_t hitMultiplicity = (extractByte(data, byteIndex) << 8) | extractByte(data, byteIndex + 1);
    byteIndex += 2;
    // Skip pFEB BCIDs, if they were sent (used only for commissioning debugging)
    if (m_pfebBCIDs) byteIndex += 48; // 2 bytes per BCID, 24 pFEBs

    std::size_t observedHits{};
    const bool isSmallSector(sectorSize == 0);
    hitAddresses[bc] = parseBitmap(data, byteIndex, observedHits, isSmallSector);
    if (observedHits != (hitMultiplicity & 0xfff)) {
      ATH_MSG_WARNING("Expected " << static_cast<uint32_t>(hitMultiplicity & 0xfff) << " set bits, observed " << observedHits);
    }

    byteIndex += BITMAP_SIZE;
  }

  auto collection = new NSW_PadTriggerData{ hashID, sectorID, static_cast<NSW_PadTriggerData::SectorSize>(sectorSize),
                                           static_cast<NSW_PadTriggerData::Endcap>(endcap), bcid, l1id, hitAddresses};

  // Status word length
  byteIndex += 2;

  if (m_toTPData) {
    parseSegments(data, byteIndex, *collection);
    byteIndex += 12;
  }

  ATH_CHECK(rdo.addCollection(collection, collection->identifierHash()));
  return StatusCode::SUCCESS;
}


//=====================================================================
std::vector<uint16_t> PadTrig_ROD_Decoder::parseBitmap(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t byteIndex, std::size_t& hitCount, bool isSmallSector) const
{
  std::vector<uint16_t> addressList{};
  constexpr std::size_t PFEB_COUNT = 24;
  std::array<std::vector<bool>, PFEB_COUNT> bitmaps{};
  std::size_t bitOffset{};

  size_t Nchannels{85}; // default value
  for (std::size_t pfeb{}; pfeb < PFEB_COUNT; pfeb++) {

    // pFEBs are sent in reverse order: last pFEB (pFEB23) sent first
    auto& currentBitmap = bitmaps[PFEB_COUNT - pfeb - 1];

    // Size of the current pFEB bitmap
    if (m_channelMapping) Nchannels = isSmallSector ? SMALL_PFEB_SIZES[PFEB_COUNT - pfeb - 1] : LARGE_PFEB_SIZES[PFEB_COUNT - pfeb - 1];

    for (std::size_t channel{}; channel < Nchannels; channel++)
      currentBitmap.push_back(extractBit(data, byteIndex * 8 + (bitOffset + channel) + 8));

    bitOffset += currentBitmap.size();
  }

  // Discard disconnected pFEBs, convert to addresses
  std::size_t chOffset{};
  for (std::size_t pfeb{}; pfeb < PFEB_COUNT; pfeb++) {
    const auto popcount = std::count(bitmaps[pfeb].begin(), bitmaps[pfeb].end(), 1);
    bool disconnected{true};
    bool lastChannel = bitmaps[pfeb][0];

    // Disconnected: alternating 01010... or 10101... pattern
    for (std::size_t i{1}; i < bitmaps[pfeb].size(); i++) {
      if (bitmaps[pfeb][i] == lastChannel) {
        disconnected = false;
        break;
      }
      lastChannel = bitmaps[pfeb][i];
    }
    hitCount += popcount;

    if (disconnected) {
      chOffset += bitmaps[pfeb].size();
      continue;
    }

    for (std::size_t ch{}; ch < bitmaps[pfeb].size(); ch++) {
      if (bitmaps[pfeb][ch]) {
        // TODO The address is a simple running counter for now.
        // Replace with an actual scheme (maybe a LUT for layer/pad-id or layer/eta/phi?)
        addressList.push_back(chOffset + ch);
      }
    }
    chOffset += bitmaps[pfeb].size();
  }
  return addressList;
}


//=====================================================================
void PadTrig_ROD_Decoder::parseSegments(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t byteIndex, NSW_PadTriggerData& collection) const
{
  // Refer to:
  // https://espace.cern.ch/ATLAS-NSW-ELX/Shared%20Documents/Pad%20Trigger/Pad_to_TP_data_format.pdf
  // const uint16_t bcidAndFlags = extractByte(data, byteIndex) << 8 | extractByte(data, byteIndex + 1);

  // TODO compare BCID with one found in header? Do something with flags?
  //const uint8_t flags = (bcidAndFlags & 0xf000) >> 12;
  //const uint16_t bcid = bcidAndFlags & 0xfff;
  byteIndex += 2;

  // Max. 4 segments
  for (std::size_t i{}; i < 4; i++) {
    const auto bandID = extractByte(data, byteIndex + i);
    // If 0xff, no bandID
    if (bandID == 0xff) {
      continue;
    }

    // FIXME phiID currently bit-packed (6 bits * 4), should be 8 bits * 4?
    const uint8_t phiID = extractByte(data, byteIndex + 4 + i);
    const auto wedge0ActiveLayers = extractByte(data, byteIndex + 8);
    const auto wedge1ActiveLayers = extractByte(data, byteIndex + 9);
    std::array<uint8_t, 2> activeLayers{};
    // 2 bits per bandID; assume wedge0 is inner
    activeLayers[0] = (wedge0ActiveLayers & (0x3 << i * 2)) >> i * 2;
    activeLayers[1] = (wedge1ActiveLayers & (0x3 << i * 2)) >> i * 2;
    auto segment    = std::make_unique<NSW_PadTriggerSegment>(bandID, phiID, activeLayers);
    collection.push_back(std::move(segment));
  }
}


//=====================================================================
bool PadTrig_ROD_Decoder::extractBit(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t index) const
{
  // Swap to big endian, as we're dealing with a contiguous bit vector
  const uint32_t word = __builtin_bswap32(data[index / 32]);
  // Treat index 0 as the most significant bit -- again, contiguous bit vector
  std::size_t subWordIndex = 31 - (index % 32);
  return (word & (0b1 << subWordIndex)) != 0;
}


//=====================================================================
uint8_t PadTrig_ROD_Decoder::extractByte(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t index) const
{
  // Swap to big endian, so byte extracted will match the byte at the given index in the contiguous packet
  union Conv
  {
    uint32_t u32;
    uint8_t u8[4];
  } c{};
  c.u32 = data[index / 4];
  return c.u8[index % 4];
}

}  // namespace Muon
