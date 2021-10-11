#ifndef MUONSTGC_CNVTOOLS_PADTRIG_ROD_DECODER_H
#define MUONSTGC_CNVTOOLS_PADTRIG_ROD_DECODER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonSTGC_CnvTools/IPadTrig_ROD_Decoder.h"

namespace Muon {
class PadTrig_ROD_Decoder : public IPadTrig_ROD_Decoder, public AthAlgTool 
{

 public:
  PadTrig_ROD_Decoder(const std::string& type, const std::string& name, const IInterface* parent);

  // Convert the given ROBFragment to an NSW_PadTriggerData object, and store it
  // in the RDO container at the appropriate hash ID, if no collection is found
  // at that hash ID.
  StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& fragment, NSW_PadTriggerDataContainer& rdo) const override;

 protected:
  // Parse the given eformat ROB data into a NSW_PadTriggerData, and store it in
  // the RDO container at the given hash ID.
  StatusCode parseBytestream(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t size, NSW_PadTriggerDataContainer& rdo, const IdentifierHash hashID) const;
  // Parse the pad bitmap found in the data starting at byteIndex, returning a
  // list of hit addresses and discarding disconnected pFEBs. If using channel
  // mapping, use bitmap sizes appropriate to the sector size. Store pop count
  // in hitCount.
  std::vector<uint16_t> parseBitmap(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t byteIndex, std::size_t& hitCount, bool isSmallSector) const;
  // Parse Pad Trigger segments found in the data starting at byteIndex.
  // Discard any empty segments. Store the resulting NSW_PadTriggerSegments in
  // the given collection.
  void parseSegments(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t byteIndex, NSW_PadTriggerData& collection) const;
  // Return the value of the bit at the given index in data.
  // Treats data as a contigous bit field rather than as an array of 4-byte
  // values, where MSB is at index 0.
  bool extractBit(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t index) const;
  // Return the byte at the given index in data.
  uint8_t extractByte(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, std::size_t index) const;
  // Properties
  Gaudi::Property<bool> m_pfebBCIDs{this,"pFEBBCIDs", false, "Are pFEB BCIDs present?"};
  Gaudi::Property<bool> m_toTPData{this, "toTPData", false,  "Is to-TP data present?"};
  Gaudi::Property<bool> m_channelMapping{this, "channelMapping", "Are pFEB channels re-mapped?"};
  
  // Mappings (go from pFEB0 to pFEB23)
  // Large sector:
  static constexpr std::array<std::size_t, 24> LARGE_PFEB_SIZES{
      102, 96, 96, 104, 96, 96, 91, 96, 56, 56, 75, 75,
      56,  56, 56, 56,  60, 60, 70, 70, 52, 52, 56, 56};
  // Small sector (TODO, this is a placeholder)
  static constexpr std::array<std::size_t, 24> SMALL_PFEB_SIZES{
      85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,
      85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85};
};
}  // namespace Muon

#endif  // MUONSTGC_CNVTOOLS_PADTRIG_ROD_DECODER_H
