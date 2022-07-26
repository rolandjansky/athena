/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONNSWCOMMONDECODE_NSWELINK_H
#define MUONNSWCOMMONDECODE_NSWELINK_H

#include <stdint.h>
#include <vector>
#include <exception>

#include "MuonNSWCommonDecode/NSWDecodeBitmaps.h"
#include "MuonNSWCommonDecode/NSWDecodeHelper.h"

namespace Muon
{
  namespace nsw
  {
    class VMMChannel;
    class NSWResourceId;

    class NSWElinkException: public std::exception
    {
     public:
      explicit NSWElinkException (const char *s)
	: m_description (s) {};

      virtual const char *what () const throw () {return m_description.c_str ();};

     private:
      std::string m_description;
    };

    class NSWElinkROCHeaderException: public NSWElinkException
    {
     public:
      explicit NSWElinkROCHeaderException (const char *s)
	: NSWElinkException (s) {};
    };

    class NSWElinkFelixHeaderException: public NSWElinkException
    {
     public:
      explicit NSWElinkFelixHeaderException (const char *s)
	: NSWElinkException (s) {};
    };

    class NSWElink
    {
     public:
      //NSWElink (); to be implemented for simulation
      NSWElink (const uint32_t *bs, uint32_t remaining);
      virtual ~NSWElink ();

      NSWElink (const NSWElink&) = delete;
      NSWElink& operator= (const NSWElink&) = delete;

      const std::vector <Muon::nsw::VMMChannel *> &get_channels () const {return m_channels;};

      unsigned int nwords () const {return m_wordCount;};
      unsigned int nhits  () const {return m_nhits;};
      unsigned int status () const {return m_packet_status;};
 
      // Detector Logical ID and components
      uint32_t elinkWord  () const {return m_elinkWord;};
      const Muon::nsw::NSWResourceId *elinkId () const {return m_elinkId;};
      Muon::nsw::NSWResourceId *elinkId () {return m_elinkId;};

      // ROC header
      bool isNull () const {return m_isNull;};
      bool noTdc  () const {return m_noTdc;};

      uint16_t l1Id     () const {return m_l1Id;};
      uint16_t rocId    () const {return m_rocId;};
      uint16_t bcId     () const {return m_bcId;};
      uint16_t orbit    () const {return m_orbit;};

      // ROC trailer
      bool tout           () const {return m_tout;};
      bool extended       () const {return m_extended;};

      uint8_t checksum    () const {return m_checksum;};

      uint16_t nhitsTrail () const {return m_nhitsTrail;};
      uint16_t l0Id       () const {return m_l0Id;};
      uint16_t flagMiss   () const {return m_flagMiss;};

     private:
      unsigned int test_checksum (const uint8_t *buffer, unsigned int buflen);

     private:
      unsigned int m_wordCount;
      unsigned int m_nhits;
      unsigned int m_running_checksum;
      unsigned int m_packet_status;

      bool m_isNull;
      bool m_noTdc;
      bool m_tout;
      bool m_extended;

      uint8_t m_checksum;

      uint16_t m_l1Id;
      uint16_t m_rocId;
      uint16_t m_bcId;
      uint16_t m_orbit;
      uint16_t m_nhitsTrail;
      uint16_t m_l0Id;
      uint16_t m_flagMiss;

      uint32_t m_elinkWord;
      Muon::nsw::NSWResourceId *m_elinkId;
      std::vector <Muon::nsw::VMMChannel *> m_channels;

      static const unsigned int s_null_packet_length = 10;
    };
  }
}

inline unsigned int Muon::nsw::NSWElink::test_checksum (const uint8_t *buffer, unsigned int buflen)
{
  const uint8_t *p = buffer;
  unsigned int running_checksum = 0;

  for (unsigned int i = 0; i < buflen; ++i, ++p)
    running_checksum += *p;

  running_checksum = (running_checksum & 0xff) + (running_checksum >> 8); // add carry bits
  running_checksum = (running_checksum & 0xff) + (running_checksum >> 8); // do it twice
  running_checksum = (~running_checksum) & 0xff;

  return running_checksum;
}

#endif // MUONNSWCOMMONDECODE_NSWELINK_H
