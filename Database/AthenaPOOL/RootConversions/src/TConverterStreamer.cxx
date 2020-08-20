/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TConverterStreamer.cxx
 * @author scott snyder <snyder@bnl.gov>, from code by Marcin Nowak.
 * @date Dec, 2006
 * @brief Root streamer that calls our converters when reading
 *        in non-split mode.
 */

#include "RootConversions/TConverterStreamer.h"
#include "TBuffer.h"
#include "TClass.h"
#include "TFile.h"
#include "TStreamerInfo.h"


/**
 * @brief Constructor.
 * @param convmap  Map from checksums to converter instances.
 * @param cls      The transient class that we're reading.
 */
TConverterStreamer::TConverterStreamer (const CheckSumMap& convmap,
                                        TClass* cls)
  : fConvmap (convmap),
    fClass (cls),
    fStreamerChecksum (0),
    fLastFile (0)
{
}


/**
 * @brief Standard Root streamer interface.
 * @param b   Buffer for Root I/O.
 * @param obj Object being read or written.
 */
void TConverterStreamer::operator() (TBuffer& b, void* obj)
{
  if (b.IsReading()) {
    // Reading --- do we need to convert?
    UInt_t startpos, bcnt;
    Version_t version;
    FindVersion (b, startpos, bcnt, version);
    // find converter for the object shape checksum
    // (checksum is read from the file in FindVersion)
    CheckSumMap::const_iterator i = fConvmap.find (fStreamerChecksum);

    if (i != fConvmap.end()) {
      // Found a converter --- call it.
      i->second.first->ReadBuffer (b, obj, version, startpos, bcnt);
    }
    else {
      // No converter --- read normally.
      fClass->ReadBuffer(b, obj, version, startpos, bcnt);
    }
  }
  else {
    // Writing --- just do the standard thing.
    fClass->WriteBuffer(b, obj);
  }
}


/**
 * @brief Read the initial byte count/version from an object.
 * @param buf The Root I/O buffer.
 * @param[out] startpos The starting position of the object.
 * @param[out] bcnt     The length of the object, in bytes.
 * @param[out] version  The version of the object being read.
 *
 * As a side effect, this sets @c fStreamerChecksum.
 */
void TConverterStreamer::FindVersion(TBuffer& buf,
                                     UInt_t& startpos,
                                     UInt_t& bcnt,
                                     Version_t& version)
{
  const UInt_t kByteCountMask     = 0x40000000;  // OR the byte count with this

  // before reading object save start position
  startpos = buf.Length();

  // read byte count (older files don't have byte count)
  // byte count is packed in two individual shorts, this to be
  // backward compatible with old files that have at this location
  // only a single short (i.e. the version)
  union {
    UInt_t     cnt;
    Version_t  vers[2];
  } v;
#ifdef R__BYTESWAP
  // cppcheck-suppress uninitvar
  buf >> v.vers[1];
  buf >> v.vers[0];
#else
  // cppcheck-suppress uninitvar
  buf >> v.vers[0];
  buf >> v.vers[1];
#endif
  if (!(v.cnt & kByteCountMask)) {
    // no bytecount, go back before reading version
    buf.SetBufferOffset(buf.Length() - sizeof(UInt_t));
    v.cnt = 0;
  }
  bcnt = (v.cnt & ~kByteCountMask);
  buf >> version;

  TFile* parent = (TFile*)buf.GetParent();
  if (fLastFile != parent) {
    // new file, reset
    fStreamerChecksum = 0;
  }
  if (fClass && fClass->GetClassVersion() != 0) {
    if (version <= 0) {
      buf >> fStreamerChecksum;
      version = R__FindStreamerInfoVersion(fClass,
                                           fStreamerChecksum);
    } else if (version == 1) {
      // hand made streamer list caching,
      // change that when TFile::GetStreamerInfoCache() is out      
      if (fLastFile == parent) {
        // the same file, nothing changed?
      }
      else if (parent && parent->GetVersion() < 40000)
      {
        // We could have a file created using a Foreign class before
        // the introduction of the CheckSum.  We need to check
        if ((!fClass->IsLoaded() || fClass->IsForeign()) &&
            fClass->GetStreamerInfos()->GetLast() > 1)
        {
          const TList* list  = parent->GetStreamerInfoList();
          const TStreamerInfo* local = (TStreamerInfo*)list->FindObject(fClass->GetName());
          if (local) {
            fStreamerChecksum = local->GetCheckSum();
            version = R__FindStreamerInfoVersion(fClass,
                                                 fStreamerChecksum);
          }
          else {
            version = 0;
          }
        }
      }
    }
  }
  fLastFile = parent;
}

/**
 * @brief Translate a Root checksum to a class version number.
 * @param cl The class being examined.
 * @param checksum The checksum to translate.
 * @return The class version number of @c cl with checksum @c checksum,
 *         or 0 if there's no match.
 */
Version_t
TConverterStreamer::R__FindStreamerInfoVersion(const TClass* cl,
                                               UInt_t checksum)
{
  //FIXME
  // use TBuffer::R__FindStreamerInfoVersion() instead of this function
  // with the next ROOT release

  //find the version number in the StreamerInfos corresponding to checksum
  Version_t version = 0;
  Int_t ninfos = cl->GetStreamerInfos()->GetEntriesFast();
  for (Int_t i = 1; i < ninfos; i++) {
    // TClass::fStreamerInfos has a lower bound not equal to 0,
    // so we should use At and not use UncheckedAt
    TStreamerInfo* info = (TStreamerInfo*)cl->GetStreamerInfos()->At(i);
    if (!info) {
      continue;
    }
    if (info->GetCheckSum() == checksum) {
      version = i;
      break;
    }
  }
  return(version);
}

