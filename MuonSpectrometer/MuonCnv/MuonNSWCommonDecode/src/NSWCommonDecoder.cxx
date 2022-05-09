/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "ers/ers.h"
#include "eformat/eformat.h"

#include "MuonNSWCommonDecode/NSWElink.h"
#include "MuonNSWCommonDecode/VMMChannel.h"
#include "MuonNSWCommonDecode/NSWCommonDecoder.h"

Muon::nsw::NSWCommonDecoder::NSWCommonDecoder (const eformat::read::ROBFragment &robFrag)
  : m_has_error (false)
{
  static const uint32_t s_min_packet_size = 3; // felix header + null ROC header

  robFrag.check ();

  uint32_t nw = robFrag.rod_ndata ();
  const uint32_t *bs = robFrag.rod_data (); // point directly to the first data element
  const uint32_t *current_data_pointer = const_cast <uint32_t *> (bs);

  uint32_t wcount(0); // data-element (32-bit word) counter

  ERS_DEBUG (1, "NDATA FROM ROB HEADER: " << nw);

  uint32_t remaining = nw;

  while (remaining >= s_min_packet_size)
  {
    try
    {
      Muon::nsw::NSWElink *elink = new Muon::nsw::NSWElink (current_data_pointer, remaining);
      m_elinks.push_back (elink);

      // Append pointers to elink channels to a local channel vector

      const std::vector <Muon::nsw::VMMChannel *> vchan = elink->get_channels ();
      for (auto i = vchan.begin (); i != vchan.end (); ++i)
	m_channels.push_back (*i);

      wcount += elink->nwords ();
      current_data_pointer += elink->nwords ();

      ERS_DEBUG (1, "NDATA: " << nw << " WORD COUNTER: " << wcount);
      ERS_DEBUG (1, "NPACKETS: " << m_elinks.size ());

      remaining = nw - wcount;
    }

    catch (Muon::nsw::NSWElinkFelixHeaderException &e)
    {
      m_has_error = true;
      //ers_error (e);
      break;
    }

    catch (Muon::nsw::NSWElinkROCHeaderException &e)
    {
      m_has_error = true;
      //ers_error (e);

      // Try to move to next link

      wcount += s_min_packet_size;
      current_data_pointer += s_min_packet_size;
      remaining = nw - wcount;
      continue;
    }
  }
}

Muon::nsw::NSWCommonDecoder::~NSWCommonDecoder ()
{
  for (auto i = m_elinks.begin (); i != m_elinks.end (); ++i)
    delete *i;
}
