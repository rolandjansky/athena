/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaRootStreamer.cxx
 *  @brief This file contains the implementation for the AthenaRootStreamer class.
 *  @author Marcin Nowak
 *  $Id:
 **/

#include "AthenaRootStreamer.h"
#include "AthenaRootConverterHandle.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include "TFile.h"
#include "TStreamerInfo.h"


AthenaRootStreamer::AthenaRootStreamer(const std::string& class_name, ::Service* service)
      : m_className(class_name),
        m_class(0),
        m_streamerChecksum (0),
        m_streamerVersion (0),
	m_lastFile(0),
	m_service(service)
{
}

AthenaRootStreamer::~AthenaRootStreamer()
{
}


StatusCode AthenaRootStreamer::AddConverter(std::unique_ptr<AthenaRootConverterHandle> converter) {
  if (m_converterMap.try_emplace (converter->StreamerChecksum(), std::move(converter)).second) {
    return(StatusCode::SUCCESS);
  }
  return(StatusCode::FAILURE);
}


//__________________________________________________________________________
StatusCode AthenaRootStreamer::Adopt() {
   m_class = gROOT->GetClass(m_className.c_str());
   if (m_class == 0) {
      return(StatusCode::FAILURE);
   }
   m_class->AdoptStreamer(this);
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
void AthenaRootStreamer::operator()(TBuffer& b, void* obj) {
   if (b.IsReading()) {
      UInt_t R__s, R__c;
      FindVersion(&b, &R__s, &R__c);
      // find converter for the object shape checksum
      // (checksum is read from the file in FindVersion)
      auto it = m_converterMap.find(m_streamerChecksum);
      if (it != m_converterMap.end()) {
         it->second->ReadBuffer(b, obj, m_streamerVersion, R__s, R__c);
      } else {
	 if(m_service) {
	    if( m_seenChecksums.find( m_streamerChecksum ) == m_seenChecksums.end() ) {
	       // report unknown checksums only once
	       MsgStream log(m_service->msgSvc(), m_service->name()); 
	       log << MSG::INFO << "Found unknown streamer checksum " << m_streamerChecksum
		   << " for class "
		   << m_className << " - using default ROOT streamer" << endmsg;
	       m_seenChecksums.insert( m_streamerChecksum );
	    }
	 }
         m_class->ReadBuffer(b, obj, m_streamerVersion, R__s, R__c);
      }
   } else {
      m_class->WriteBuffer(b, obj);
   }
}
//__________________________________________________________________________
// Find class version and streamer checksum
void AthenaRootStreamer::FindVersion(TBuffer* buf, UInt_t* startpos, UInt_t* bcnt) {
   const UInt_t kByteCountMask     = 0x40000000;  // OR the byte count with this
   const Version_t kByteCountVMask = 0x4000;      // OR the version byte count with this

   if (startpos && bcnt) {
      // before reading object save start position
      *startpos = buf->Length();

      // read byte count (older files don't have byte count)
      // byte count is packed in two individual shorts, this to be
      // backward compatible with old files that have at this location
      // only a single short (i.e. the version)
      union {
         UInt_t     cnt;
         Version_t  vers[2];
      } v;
      v.cnt = 0;  // to prevent Valgrind complaining about uninitialized variable
#ifdef R__BYTESWAP
      *buf >> v.vers[1];
      *buf >> v.vers[0];
#else
      *buf >> v.vers[0];
      *buf >> v.vers[1];
#endif
      if( !(v.cnt & kByteCountMask) ) {
         // no bytecount, go back before reading version
         buf->SetBufferOffset(buf->Length() - sizeof(UInt_t));
         v.cnt = 0;
      }
      *bcnt = (v.cnt & ~kByteCountMask);
      *buf >> m_streamerVersion;
   } else {
      // not interested in byte count
      *buf >> m_streamerVersion;

      // if this is a byte count, then skip next short and read version
      if (m_streamerVersion & kByteCountVMask) {
         *buf >> m_streamerVersion;
         *buf >> m_streamerVersion;
      }
   }
   //Version_t objectVersion = m_streamerVersion;
   TFile* fParent = (TFile*)buf->GetParent();
   if (m_lastFile != fParent) {
      // new file, reset
      m_streamerChecksum = 0;
   }
   if (m_class && m_class->GetClassVersion() != 0) {
      if (m_streamerVersion <= 0) {
         *buf >> m_streamerChecksum;
         m_streamerVersion = R__FindStreamerInfoVersion(m_class, m_streamerChecksum);
      } else if (m_streamerVersion == 1) {
         // hand made streamer list caching, change that when TFile::GetStreamerInfoCache() is out      
         if (m_lastFile == fParent) {
            // the same file, nothing changed?
         } else if (fParent && fParent->GetVersion() < 40000) {
            // We could have a file created using a Foreign class before
            // the introduction of the CheckSum.  We need to check
            if ((!m_class->IsLoaded() || m_class->IsForeign()) &&
                m_class->GetStreamerInfos()->GetLast() > 1) {
               const TList* list  = fParent->GetStreamerInfoList();
               const TStreamerInfo* local = (TStreamerInfo*)list->FindObject(m_className.c_str());
               if (local) {
                  m_streamerChecksum = local->GetCheckSum();
                  m_streamerVersion = R__FindStreamerInfoVersion(m_class, m_streamerChecksum);
               } else {
                  m_streamerVersion = 0;
               }
            }
         }
      }
   }
   m_lastFile = fParent;
}


//__________________________________________________________________________

//FIXME
// use TBuffer::R__FindStreamerInfoVersion() instead of this function
// with the next ROOT release
Version_t AthenaRootStreamer::R__FindStreamerInfoVersion(const TClass* cl, UInt_t checksum) {
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
