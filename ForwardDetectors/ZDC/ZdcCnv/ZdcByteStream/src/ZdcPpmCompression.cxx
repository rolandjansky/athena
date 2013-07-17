/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <stdint.h>
#include <algorithm>
#include <vector>

//#include "ZdcByteStream/ZdcSubBlock.h"
#include "ZdcByteStream/ZdcPpmSubBlock.h"
#include "ZdcByteStream/ZdcPpmCompression.h"

// Static constants

const int ZdcPpmCompression::s_formatsV0;
const int ZdcPpmCompression::s_lowerRange;
const int ZdcPpmCompression::s_upperRange;
const int ZdcPpmCompression::s_formats;
const int ZdcPpmCompression::s_fadcRange;
const int ZdcPpmCompression::s_peakOnly;
const int ZdcPpmCompression::s_lutDataBits;
const int ZdcPpmCompression::s_lutBcidBits;
const int ZdcPpmCompression::s_fadcDataBits;
const int ZdcPpmCompression::s_glinkPins;
const int ZdcPpmCompression::s_statusBits;
const int ZdcPpmCompression::s_errorBits;
const int ZdcPpmCompression::s_statusMask;

// Pack data

bool ZdcPpmCompression::pack(ZdcPpmSubBlock& subBlock)
{
  const int dataFormat = subBlock.format();
  if (dataFormat != ZdcSubBlock::COMPRESSED &&
      dataFormat != ZdcSubBlock::SUPERCOMPRESSED) return false;
  const int sliceL = subBlock.slicesLut();
  const int sliceF = subBlock.slicesFadc();
  if (sliceL != 1 || sliceF != 5) return false;
  const int trigOffset    = subBlock.fadcOffset() - subBlock.lutOffset();
  const int fadcBaseline  = subBlock.fadcBaseline();
  const int fadcThreshold = subBlock.fadcThreshold();
  const int channels      = subBlock.channelsPerSubBlock();
  subBlock.setStreamed();
  std::vector<uint32_t> compStats(s_formats);
  std::vector<int>      fadcDout(sliceF-1);
  std::vector<int>      fadcLens(sliceF-1);
  for (int chan = 0; chan < channels; ++chan) {
    std::vector<int> lutData;
    std::vector<int> lutBcid;
    std::vector<int> fadcData;
    std::vector<int> fadcBcid;
    subBlock.ppmData(chan, lutData, fadcData, lutBcid, fadcBcid);
    if (dataFormat == ZdcSubBlock::SUPERCOMPRESSED) {
      int dataPresent = lutData[0] || lutBcid[0];
      if ( !dataPresent ) {
        for (int sl = 0; sl < sliceF; ++sl) {
	  if (fadcData[sl] >= fadcThreshold || fadcBcid[sl]) {
	    dataPresent = 1;
	    break;
          }
        }
      }
      subBlock.packer(dataPresent, 1);
      if ( !dataPresent ) continue;
    }
    const int lutLen = subBlock.minBits(lutData[0]);
    int  minFadc   = 0;
    int  minOffset = 0;
    bool fadcSame  = true;
    for (int sl = 0; sl < sliceF; ++sl) {
      if (sl == 0) minFadc = fadcData[sl];
      if (fadcData[sl] < minFadc) {
        minFadc   = fadcData[sl];
	minOffset = sl;
      }
      if (fadcData[sl] != fadcData[0] || fadcBcid[sl] != 0) fadcSame = false;
    }
    if (minOffset) std::swap(fadcData[0], fadcData[minOffset]);

    int format = 0;
    if (lutData[0] == 0 && lutBcid[0] == 0 && fadcSame) { // format 6
      const int header = 15;
      subBlock.packer(header, 4);
      if (fadcData[0]) {
        subBlock.packer(1, 1);
	subBlock.packer(fadcData[0], s_fadcDataBits);
      } else subBlock.packer(0, 1);
      format = 6;
    } else {
      const bool minFadcInRange = minFadc >= fadcBaseline &&
                                  minFadc <= fadcBaseline + s_fadcRange;
      int  anyFadcBcid = 0;
      int  maxFadcLen = 0;
      int  idx = 0;
      for (int sl = 0; sl < sliceF; ++sl) {
        if (sl != 0) {
	  fadcDout[idx] = fadcData[sl] - minFadc;
	  fadcLens[idx] = subBlock.minBits(fadcDout[idx]);
	  if (idx == 0 || fadcLens[idx] > maxFadcLen) {
	    maxFadcLen = fadcLens[idx];
	  }
	  ++idx;
        }
	if (sl != trigOffset) anyFadcBcid |= fadcBcid[sl];
	else if (fadcBcid[sl] != (lutBcid[0] & 0x1)) anyFadcBcid |= 1;
      }
      if (lutData[0] == 0 && lutBcid[0] == 0 &&
                          !anyFadcBcid && minFadcInRange && maxFadcLen < 4) {
        // formats 0,1
        int header = minOffset;
	if (maxFadcLen == 3) header += 5;
	subBlock.packer(header, 4);
	minFadc -= fadcBaseline;
	subBlock.packer(minFadc, 4);
	if (maxFadcLen < 2) maxFadcLen = 2;
	for (int idx = 0; idx < sliceF-1; ++idx) {
	  subBlock.packer(fadcDout[idx], maxFadcLen);
        }
	format = maxFadcLen - 2;
      } else if (lutLen <= 3 && ((lutData[0] == 0 && lutBcid[0] == 0) ||
                                 (lutData[0]  > 0 && lutBcid[0] == s_peakOnly))
                 && !anyFadcBcid && minFadcInRange && maxFadcLen <= 4) {
        // format 2
	const int header = minOffset + 10;
	subBlock.packer(header, 4);
	format = 2;
	subBlock.packer(format - 2, 2);
        if (lutData[0]) {
	  subBlock.packer(1, 1);
	  subBlock.packer(lutData[0], 3);
	} else subBlock.packer(0, 1);
	minFadc -= fadcBaseline;
	subBlock.packer(minFadc, 4);
	for (int idx = 0; idx < sliceF-1; ++idx) {
	  subBlock.packer(fadcDout[idx], 4);
        }
      } else {
        // formats 3,4,5
	const int header = minOffset + 10;
	subBlock.packer(header, 4);
	if ( !minFadcInRange) {
	  const int minFadcLen = subBlock.minBits(minFadc);
	  if (minFadcLen > maxFadcLen) maxFadcLen = minFadcLen;
	}
	format = 5;
	if (maxFadcLen <= 8) format = 4;
	if (maxFadcLen <= 6) format = 3;
        subBlock.packer(format - 2, 2);
	if (lutData[0] || lutBcid[0]) subBlock.packer(1, 1);
	else subBlock.packer(0, 1);
	subBlock.packer(anyFadcBcid, 1);
	if (lutData[0] || lutBcid[0]) {
	  subBlock.packer(lutData[0], s_lutDataBits);
	  subBlock.packer(lutBcid[0], s_lutBcidBits);
	}
	if (anyFadcBcid) {
	  for (int idx = 0; idx < sliceF; ++idx) {
	    subBlock.packer(fadcBcid[idx], 1);
          }
        }
	if (minFadcInRange) {
	  subBlock.packer(0, 1);
	  minFadc -= fadcBaseline;
	  subBlock.packer(minFadc, 4);
        } else {
	  subBlock.packer(1, 1);
          subBlock.packer(minFadc, format * 2);
        }
	for (int idx = 0; idx < sliceF-1; ++idx) {
	  if (fadcLens[idx] <= 4) {
	    subBlock.packer(0, 1);
	    subBlock.packer(fadcDout[idx], 4);
          } else {
	    subBlock.packer(1, 1);
	    subBlock.packer(fadcDout[idx], format * 2);
          }
        }
      }
    }
    ++compStats[format];
  }
  // Errors
  std::vector<int> status(s_glinkPins);
  std::vector<int> error(s_glinkPins);
  int statusBit = 0;
  int errorBit  = 0;
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    const int errorWord = subBlock.ppmPinError(pin);
    status[pin] = errorWord &  s_statusMask;
    error[pin]  = errorWord >> s_statusBits;
    if (status[pin]) statusBit = 1;
    if (error[pin])  errorBit  = 1;
  }
  subBlock.packer(statusBit, 1);
  subBlock.packer(errorBit,  1);
  if (statusBit || errorBit) {
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (status[pin] || error[pin]) subBlock.packer(1, 1);
      else subBlock.packer(0, 1);
    }
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (status[pin] || error[pin]) {
        if (statusBit) subBlock.packer(status[pin], s_statusBits);
	if (errorBit)  subBlock.packer(error[pin],  s_errorBits);
      }
    }
  }
  subBlock.packerFlush();
  subBlock.setCompStats(compStats);
  return true;
}

// Unpack data

bool ZdcPpmCompression::unpack(ZdcPpmSubBlock& subBlock)
{
  bool rc = false;
  switch (subBlock.seqno()) {
    case 0:
      rc = unpackV100(subBlock);
      break;
    case 1:
      rc = unpackV101(subBlock);
      break;
    case 2:
    case 3:
    case 4:
    case 5: // runs 88701-24 only
      rc = unpackV104(subBlock);
      break;
    default:
      subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_COMPRESSION_VERSION);
      break;
  }
  return rc;
}

// Unpack data - version 1.00

bool ZdcPpmCompression::unpackV100(ZdcPpmSubBlock& subBlock)
{
  if (subBlock.format() != ZdcSubBlock::COMPRESSED) {
    subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_FORMAT);
    return false;
  }
  const int sliceL = subBlock.slicesLut();
  const int sliceF = subBlock.slicesFadc();
  if (sliceL != 1 || sliceF != 5) {
    subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_COMPRESSION_SLICES);
    return false;
  }
  const int trigOffset = subBlock.fadcOffset();
  const int pedestal   = subBlock.pedestal();
  const int channels   = subBlock.channelsPerSubBlock();
  subBlock.setStreamed();
  subBlock.unpackerInit();
  std::vector<uint32_t> compStats(s_formatsV0);
  for (int chan = 0; chan < channels; ++chan) {
    std::vector<int> lutData;
    std::vector<int> lutBcid;
    std::vector<int> fadcData;
    std::vector<int> fadcBcid;
    int format = 0;
    const int header = subBlock.unpacker(4);
    if (header < 10) {
      // formats 0,1 - LUT zero, FADC around pedestal
      const int minOffset = header % 5;
                format    = header / 5;
      // LUT = 0
      lutData.push_back(0);
      lutBcid.push_back(0);
      // FADC
      const uint32_t minFadc = subBlock.unpacker(4) + pedestal - s_lowerRange;
      for (int sl = 0; sl < sliceF; ++sl) {
        if (sl == minOffset) fadcData.push_back(minFadc);
	else fadcData.push_back(subBlock.unpacker(format + 2) + minFadc);
	fadcBcid.push_back(0);
      }
    } else {
      // formats 2-5
      const int minOffset = header - 10;
                format = subBlock.unpacker(2) + 2;
      const int anyLut = subBlock.unpacker(1);
      int lut  = 0;
      int bcid = 0;
      if (format == 2) {
        // LUT
	if (anyLut) {
	  lut  = subBlock.unpacker(3);
	  bcid = s_peakOnly;  // just peak-finding BCID set
        }
	lutData.push_back(lut);
	lutBcid.push_back(bcid);
	// FADC as formats 0,1
        const int minFadc = subBlock.unpacker(4) + pedestal - s_lowerRange;
        for (int sl = 0; sl < sliceF; ++sl) {
          if (sl == minOffset) fadcData.push_back(minFadc);
	  else fadcData.push_back(subBlock.unpacker(format + 2) + minFadc);
	  fadcBcid.push_back(0);
        }
      } else {
        // formats 3,4,5 - full LUT word, variable FADC
	const int anyBcid = subBlock.unpacker(1);
	// LUT
	if (anyLut) {
	  lut  = subBlock.unpacker(s_lutDataBits);
	  bcid = subBlock.unpacker(s_lutBcidBits);
	}
        lutData.push_back(lut);
	lutBcid.push_back(bcid);
	// FADC
	for (int sl = 0; sl < sliceF; ++sl) {
	  int fbcid = 0;
	  if (sl == trigOffset) fbcid = bcid & 0x1; // take from LUT word
	  else if (anyBcid) fbcid = subBlock.unpacker(1);
	  fadcBcid.push_back(fbcid);
        }
	int minFadc = 0;
	if (subBlock.unpacker(1)) minFadc = subBlock.unpacker(format * 2);
	else minFadc = subBlock.unpacker(4) + pedestal - s_lowerRange;
	for (int sl = 0; sl < sliceF; ++sl) {
	  int fadc = minFadc;
	  if (sl != minOffset) {
	    if (subBlock.unpacker(1)) fadc += subBlock.unpacker(format * 2);
	    else                      fadc += subBlock.unpacker(4);
          }
	  fadcData.push_back(fadc);
        }
      }
    }
    subBlock.fillPpmData(chan, lutData, fadcData, lutBcid, fadcBcid);
    ++compStats[format];
  }
  subBlock.setCompStats(compStats);
  const bool rc = subBlock.unpackerSuccess();
  if (!rc) subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack data - version 1.01

bool ZdcPpmCompression::unpackV101(ZdcPpmSubBlock& subBlock)
{
  if (subBlock.format() != ZdcSubBlock::COMPRESSED) {
    subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_FORMAT);
    return false;
  }
  const int sliceL = subBlock.slicesLut();
  const int sliceF = subBlock.slicesFadc();
  if (sliceL != 1 || sliceF != 5) {
    subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_COMPRESSION_SLICES);
    return false;
  }
  const int trigOffset = subBlock.fadcOffset();
  const int pedestal   = subBlock.pedestal();
  const int channels   = subBlock.channelsPerSubBlock();
  subBlock.setStreamed();
  subBlock.unpackerInit();
  std::vector<uint32_t> compStats(s_formats);
  for (int chan = 0; chan < channels; ++chan) {
    std::vector<int> lutData;
    std::vector<int> lutBcid;
    std::vector<int> fadcData;
    std::vector<int> fadcBcid;
    int format = 0;
    const int header = subBlock.unpacker(4);
    if (header < 10) {
      // formats 0,1 - LUT zero, FADC around pedestal
      const int minOffset = header % 5;
                format    = header / 5;
      // LUT = 0
      lutData.push_back(0);
      lutBcid.push_back(0);
      // FADC
      const int minFadc = subBlock.unpacker(4) + pedestal - s_lowerRange;
      fadcData.push_back(minFadc);
      fadcBcid.push_back(0);
      for (int sl = 1; sl < sliceF; ++sl) {
	fadcData.push_back(subBlock.unpacker(format + 2) + minFadc);
        fadcBcid.push_back(0);
      }
      if (minOffset) std::swap(fadcData[0], fadcData[minOffset]);
    } else if (header < 15) {
      // formats 2-5
      const int minOffset = header - 10;
                format = subBlock.unpacker(2) + 2;
      const int anyLut = subBlock.unpacker(1);
      int lut  = 0;
      int bcid = 0;
      if (format == 2) {
        // LUT
	if (anyLut) {
	  lut  = subBlock.unpacker(3);
	  bcid = s_peakOnly;  // just peak-finding BCID set
        }
	lutData.push_back(lut);
	lutBcid.push_back(bcid);
	// FADC as formats 0,1
        const int minFadc = subBlock.unpacker(4) + pedestal - s_lowerRange;
        fadcData.push_back(minFadc);
        fadcBcid.push_back(0);
        for (int sl = 1; sl < sliceF; ++sl) {
	  fadcData.push_back(subBlock.unpacker(format + 2) + minFadc);
          fadcBcid.push_back(0);
        }
        if (minOffset) std::swap(fadcData[0], fadcData[minOffset]);
      } else {
        // formats 3,4,5 - full LUT word, variable FADC
	const int anyBcid = subBlock.unpacker(1);
	// LUT
	if (anyLut) {
	  lut  = subBlock.unpacker(s_lutDataBits);
	  bcid = subBlock.unpacker(s_lutBcidBits);
	}
        lutData.push_back(lut);
	lutBcid.push_back(bcid);
	// FADC
	for (int sl = 0; sl < sliceF; ++sl) {
	  int fbcid = 0;
	  if (sl == trigOffset) fbcid = bcid & 0x1; // take from LUT word
	  else if (anyBcid) fbcid = subBlock.unpacker(1);
	  fadcBcid.push_back(fbcid);
        }
	int minFadc = 0;
	if (subBlock.unpacker(1)) minFadc = subBlock.unpacker(format * 2);
	else minFadc = subBlock.unpacker(4) + pedestal - s_lowerRange;
        fadcData.push_back(minFadc);
	for (int sl = 1; sl < sliceF; ++sl) {
	  int len = 4;
	  if (subBlock.unpacker(1)) len = format * 2;
	  fadcData.push_back(subBlock.unpacker(len) + minFadc);
        }
	if (minOffset) std::swap(fadcData[0], fadcData[minOffset]);
      }
    } else {
      // format 6 - LUT zero, FADC all equal
      format = 6;
      // LUT
      lutData.push_back(0);
      lutBcid.push_back(0);
      // FADC
      int fadc = 0;
      if (subBlock.unpacker(1)) fadc = subBlock.unpacker(s_fadcDataBits);
      for (int sl = 0; sl < sliceF; ++sl) {
        fadcData.push_back(fadc);
        fadcBcid.push_back(0);
      }
    }
    subBlock.fillPpmData(chan, lutData, fadcData, lutBcid, fadcBcid);
    ++compStats[format];
  }
  // Errors
  const int statusBit = subBlock.unpacker(1);
  const int errorBit  = subBlock.unpacker(1);
  if (statusBit || errorBit) {
    std::vector<int> err(s_glinkPins);
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      err[pin] = subBlock.unpacker(1);
    }
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (err[pin]) {
        int status = 0;
	int error  = 0;
	if (statusBit) status = subBlock.unpacker(s_statusBits-1);
	if (errorBit)  error  = subBlock.unpacker(s_errorBits+1);
	subBlock.fillPpmPinError(pin, (error << (s_statusBits-1)) | status);
      }
    }
  }
  subBlock.setCompStats(compStats);
  const bool rc = subBlock.unpackerSuccess();
  if (!rc) subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_DATA_TRUNCATED);
  return rc;
}

// Unpack data - versions 1.02, 1.03, 1.04, 1.05 (by mistake for a few runs)

bool ZdcPpmCompression::unpackV104(ZdcPpmSubBlock& subBlock)
{
  const int dataFormat = subBlock.format();
  if (dataFormat != ZdcSubBlock::COMPRESSED &&
      dataFormat != ZdcSubBlock::SUPERCOMPRESSED) {
    subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_FORMAT);
    return false;
  }
  const int compressionVersion = subBlock.seqno();
  if (compressionVersion == 2 && dataFormat != ZdcSubBlock::COMPRESSED) {
    subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_FORMAT);
    return false;
  }
  if (compressionVersion == 5) {
    const int run = subBlock.runNumber();
    if (run < 88701 || run > 88724) {
      subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_COMPRESSION_VERSION);
      return false;
    }
  }
  const int sliceL = subBlock.slicesLut();
  const int sliceF = subBlock.slicesFadc();
  if (sliceL != 1 || sliceF != 5) {
    subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_COMPRESSION_SLICES);
    return false;
  }
  const int trigOffset   = subBlock.fadcOffset() - subBlock.lutOffset();
  const int fadcBaseline = subBlock.fadcBaseline();
  const int channels     = subBlock.channelsPerSubBlock();
  subBlock.setStreamed();
  subBlock.unpackerInit();
  std::vector<uint32_t> compStats(s_formats);
  for (int chan = 0; chan < channels; ++chan) {
    if (dataFormat == ZdcSubBlock::SUPERCOMPRESSED) {
      if ( !subBlock.unpacker(1) ) continue;
    }
    std::vector<int> lutData;
    std::vector<int> lutBcid;
    std::vector<int> fadcData;
    std::vector<int> fadcBcid;
    int format = 0;
    const int header = subBlock.unpacker(4);
    if (header < 10) {
      // formats 0,1 - LUT zero, FADC around pedestal
      const int minOffset = header % 5;
                format    = header / 5;
      // LUT = 0
      lutData.push_back(0);
      lutBcid.push_back(0);
      // FADC
      const int minFadc = subBlock.unpacker(4) + fadcBaseline;
      fadcData.push_back(minFadc);
      fadcBcid.push_back(0);
      for (int sl = 1; sl < sliceF; ++sl) {
	fadcData.push_back(subBlock.unpacker(format + 2) + minFadc);
        fadcBcid.push_back(0);
      }
      if (minOffset) std::swap(fadcData[0], fadcData[minOffset]);
    } else if (header < 15) {
      // formats 2-5
      const int minOffset = header - 10;
                format = subBlock.unpacker(2) + 2;
      const int anyLut = subBlock.unpacker(1);
      int lut  = 0;
      int bcid = 0;
      if (format == 2) {
        // LUT
	if (anyLut) {
	  lut  = subBlock.unpacker(3);
	  bcid = s_peakOnly;  // just peak-finding BCID set
        }
	lutData.push_back(lut);
	lutBcid.push_back(bcid);
	// FADC as formats 0,1
        const int minFadc = subBlock.unpacker(4) + fadcBaseline;
        fadcData.push_back(minFadc);
        fadcBcid.push_back(0);
        for (int sl = 1; sl < sliceF; ++sl) {
	  fadcData.push_back(subBlock.unpacker(format + 2) + minFadc);
          fadcBcid.push_back(0);
        }
        if (minOffset) std::swap(fadcData[0], fadcData[minOffset]);
      } else {
        // formats 3,4,5 - full LUT word, variable FADC
	const int anyBcid = subBlock.unpacker(1);
	// LUT
	if (anyLut) {
	  lut  = subBlock.unpacker(s_lutDataBits);
	  bcid = subBlock.unpacker(s_lutBcidBits);
	}
        lutData.push_back(lut);
	lutBcid.push_back(bcid);
	// FADC
	for (int sl = 0; sl < sliceF; ++sl) {
	  int fbcid = 0;
	  if (anyBcid) fbcid = subBlock.unpacker(1);
	  else if (sl == trigOffset) fbcid = bcid & 0x1; // take from LUT word
	  fadcBcid.push_back(fbcid);
        }
	int minFadc = 0;
	if (subBlock.unpacker(1)) minFadc = subBlock.unpacker(format * 2);
	else minFadc = subBlock.unpacker(4) + fadcBaseline;
        fadcData.push_back(minFadc);
	for (int sl = 1; sl < sliceF; ++sl) {
	  int len = 4;
	  if (subBlock.unpacker(1)) len = format * 2;
	  fadcData.push_back(subBlock.unpacker(len) + minFadc);
        }
	if (minOffset) std::swap(fadcData[0], fadcData[minOffset]);
      }
    } else {
      // format 6 - LUT zero, FADC all equal
      format = 6;
      // LUT
      lutData.push_back(0);
      lutBcid.push_back(0);
      // FADC
      int fadc = 0;
      if (subBlock.unpacker(1)) fadc = subBlock.unpacker(s_fadcDataBits);
      for (int sl = 0; sl < sliceF; ++sl) {
        fadcData.push_back(fadc);
        fadcBcid.push_back(0);
      }
    }
    subBlock.fillPpmData(chan, lutData, fadcData, lutBcid, fadcBcid);
    ++compStats[format];
  }
  // Errors
  const int statusBit = subBlock.unpacker(1);
  const int errorBit  = subBlock.unpacker(1);
  const bool mcmAbsentIsError = compressionVersion < 4;
  const int sBits = (mcmAbsentIsError) ? s_statusBits - 1 : s_statusBits;
  const int eBits = (mcmAbsentIsError) ? s_errorBits  + 1 : s_errorBits;
  if (statusBit || errorBit) {
    std::vector<int> err(s_glinkPins);
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      err[pin] = subBlock.unpacker(1);
    }
    for (int pin = 0; pin < s_glinkPins; ++pin) {
      if (err[pin]) {
        int status = 0;
	int error  = 0;
	if (statusBit) status = subBlock.unpacker(sBits);
	if (errorBit)  error  = subBlock.unpacker(eBits);
	subBlock.fillPpmPinError(pin, (error << sBits) | status);
      }
    }
    // There is a bug in versions < 4 but if I've understood it correctly
    // it is not possible to detect it reliably in data.
  }
  subBlock.setCompStats(compStats);
  const bool rc = subBlock.unpackerSuccess();
  if (!rc) subBlock.setUnpackErrorCode(ZdcSubBlock::UNPACK_DATA_TRUNCATED);
  return rc;
}

