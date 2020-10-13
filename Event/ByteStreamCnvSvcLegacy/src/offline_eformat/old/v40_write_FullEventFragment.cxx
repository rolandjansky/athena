//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file SubDetectorFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: salnikov $
 * $Revision: 726501 $
 * $Date: 2016-02-26 17:52:33 +0100 (Fri, 26 Feb 2016) $
 *
 * Implements the SubDetector fragment writing helper class
 */

#include "v40_write_FullEventFragment.h"
#include "v40_FullEventFragment.h"
#include "eformat/HeaderMarker.h"
#include "eformat/Status.h"
#include "eformat/Issue.h"
#include "eformat/checksum.h"

void offline_eformat::v40_write::FullEventFragment::initialize()
{
  m_header[0] = eformat::FULL_EVENT; //marker
  m_header[1] = 21; //this header size + status size
  m_header[2] = 21; //this header size + status size
  eformat::helper::Version version(0, eformat::MAJOR_V40_VERSION);
  m_header[3] = version.code(); //format version
  m_header[4] = 0; //source identifier
  m_header[5] = 1; //number of status
  m_header[6] = eformat::NO_CHECKSUM; //check sum
  m_header[7] = 0; //bc time seconds
  m_header[8] = 0; //bc time nanoseconds
  m_header[9] = 0; //global identifier
  m_header[10] = 0; //run type
  m_header[11] = 0; //run number
  m_header[12] = 0; //luminosity block number
  m_header[13] = 0; //LVL1 identifiery
  m_header[14] = 0; //bunch crossing identifier
  m_header[15] = 0; //LVL1 trigger type 
  m_header[16] = 0; //number of LVL1 info words
  m_header[17] = 0; //number of LVL2 info words
  m_header[18] = 0; //number of EF info words
  m_header[19] = 0; //number of Stream Tag words

  //now initialize pages
  set(m_node[0], m_header, 6, &m_node[1]);
  set(m_node[1], &eformat::DEFAULT_STATUS, 1, &m_node[2]);
  set(m_node[2], &m_header[6], 11, &m_node[3]);
  set(m_node[3], 0, 0, &m_node[4]); //no LVL1 info yet
  set(m_node[4], &m_header[17], 1, &m_node[5]); 
  set(m_node[5], 0, 0, &m_node[6]); //no LVL2 info yet
  set(m_node[6], &m_header[18], 1, &m_node[7]); 
  set(m_node[7], 0, 0, &m_node[8]); //no EF info yet
  set(m_node[8], &m_header[19], 1, &m_node[9]); 
  set(m_node[9], 0, 0, 0); //no stream tag yet

  //optional extra payload 
  set(m_node[10], 0, 0, 0);
  
  //optional checksum stuff (trailer)
  m_checksum = 0;
  set(m_node[11], &m_checksum, 1, 0);

  //our unchecked data
  m_n_unchecked = 0;
  set(m_unchecked[0], 0, 0, 0);
  
  m_child = 0;
  m_last = 0;
}

offline_eformat::v40_write::FullEventFragment::FullEventFragment
(uint32_t source_id, uint32_t bc_time_secs, uint32_t bc_time_nsec,
 uint32_t global_id, uint32_t run_type,
 uint32_t run_no, uint16_t lumi_block,
 uint32_t lvl1_id, uint16_t bc_id, uint8_t lvl1_type)
{
  initialize();
  this->source_id(source_id);
  bc_time_seconds(bc_time_secs);
  bc_time_nanoseconds(bc_time_nsec);
  this->global_id(global_id);
  this->run_type(run_type);
  this->run_no(run_no);
  this->lumi_block(lumi_block);
  this->lvl1_id(lvl1_id);
  this->bc_id(bc_id);
  lvl1_trigger_type(lvl1_type);
}

offline_eformat::v40_write::FullEventFragment::FullEventFragment (const uint32_t* fe)
{
  initialize();
  copy_header(fe);
  v40::FullEventFragment read(fe);
  if (read.payload_size_word() > 0) {
    const uint32_t* tmp;
    m_node[9].next = &m_node[10];
    read.payload(tmp);
    set(m_node[10], tmp, read.payload_size_word(), 0);
    m_header[1] += read.payload_size_word();
  }
}

offline_eformat::v40_write::FullEventFragment::FullEventFragment ()
{
  initialize();
}

offline_eformat::v40_write::FullEventFragment::~FullEventFragment()
{
}

void offline_eformat::v40_write::FullEventFragment::copy_header(const uint32_t* other)
{
  v40::FullEventFragment read(other);
  read.check(); //this gives us a good start-up scenario
  eformat::helper::Version fe_ver(read.version());
  minor_version(fe_ver.minor_version());
  source_id(read.source_id());
  bc_time_seconds(read.bc_time_seconds());
  bc_time_nanoseconds(read.bc_time_nanoseconds());
  global_id(read.global_id());
  run_type(read.run_type());
  run_no(read.run_no());
  lumi_block(read.lumi_block());
  lvl1_id(read.lvl1_id());
  bc_id(read.bc_id());
  lvl1_trigger_type(read.lvl1_trigger_type());
  const uint32_t* tmp;
  read.status(tmp);
  status(read.nstatus(), tmp);
  read.lvl1_trigger_info(tmp);
  lvl1_trigger_info(read.nlvl1_trigger_info(), tmp);
  read.lvl2_trigger_info(tmp);
  lvl2_trigger_info(read.nlvl2_trigger_info(), tmp);
  read.event_filter_info(tmp);
  event_filter_info(read.nevent_filter_info(), tmp);
  read.stream_tag(tmp);
  stream_tag(read.nstream_tag(), tmp);
  checksum_type(read.checksum_type());
}

void offline_eformat::v40_write::FullEventFragment::copy_header
(const offline_eformat::v40_write::FullEventFragment& other)
{
  minor_version(other.minor_version());
  source_id(other.source_id());
  bc_time_seconds(other.bc_time_seconds());
  bc_time_nanoseconds(other.bc_time_nanoseconds());
  global_id(other.global_id());
  run_type(other.run_type());
  run_no(other.run_no());
  lumi_block(other.lumi_block());
  lvl1_id(other.lvl1_id());
  bc_id(other.bc_id());
  lvl1_trigger_type(other.lvl1_trigger_type());
  status(other.nstatus(), other.status());
  lvl1_trigger_info(other.nlvl1_trigger_info(), other.lvl1_trigger_info());
  lvl2_trigger_info(other.nlvl2_trigger_info(), other.lvl2_trigger_info());
  event_filter_info(other.nevent_filter_info(), other.event_filter_info());
  stream_tag(other.nstream_tag(), other.stream_tag());
  checksum_type(other.checksum_type());
}

void offline_eformat::v40_write::FullEventFragment::status
(uint32_t n, const uint32_t* status)
{ 
  m_node[0].base[1] -= m_node[0].base[5]; //remove count from previous status
  m_node[0].base[2] -= m_node[0].base[5]; //remove count from previous status
  m_node[1].size_word = m_node[0].base[5] = n; //set new values
  m_node[0].base[1] += n;
  m_node[0].base[2] += n;
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* status_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(status);
  m_node[1].base = status_nc;
}

void offline_eformat::v40_write::FullEventFragment::lvl1_trigger_info
(uint32_t n, const uint32_t* data)
{ 
  m_node[0].base[1] -= nlvl1_trigger_info(); //remove previous count 
  m_node[0].base[2] -= nlvl1_trigger_info(); //remove previous count 
  m_node[3].size_word = m_node[2].base[10] = n; //set new values
  m_node[0].base[1] += n; //fragment size
  m_node[0].base[2] += n; //header size
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* data_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(data);
  m_node[3].base = data_nc;
}

  void offline_eformat::v40_write::FullEventFragment::lvl2_trigger_info
(uint32_t n, const uint32_t* data)
{ 
  m_node[0].base[1] -= nlvl2_trigger_info(); //remove previous count 
  m_node[0].base[2] -= nlvl2_trigger_info(); //remove previous count 
  m_node[5].size_word = m_node[4].base[0] = n; //set new values
  m_node[0].base[1] += n; //fragment size
  m_node[0].base[2] += n; //header size
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* data_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(data);
  m_node[5].base = data_nc;
}

void offline_eformat::v40_write::FullEventFragment::event_filter_info
(uint32_t n, const uint32_t* data)
{ 
  m_node[0].base[1] -= nevent_filter_info(); //remove previous count 
  m_node[0].base[2] -= nevent_filter_info(); //remove previous count 
  m_node[7].size_word = m_node[6].base[0] = n; //set new values
  m_node[0].base[1] += n; //fragment size
  m_node[0].base[2] += n; //header size
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* data_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(data);
  m_node[7].base = data_nc;
}

void offline_eformat::v40_write::FullEventFragment::stream_tag
(uint32_t n, const uint32_t* data)
{ 
  m_node[0].base[1] -= nstream_tag(); //remove previous count 
  m_node[0].base[2] -= nstream_tag(); //remove previous count 
  m_node[9].size_word = m_node[8].base[0] = n; //set new values
  m_node[0].base[1] += n; //fragment size
  m_node[0].base[2] += n; //header size
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* data_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(data);
  m_node[9].base = data_nc;
}

void offline_eformat::v40_write::FullEventFragment::append
(v40_write::ROBFragment* rob)
{
  rob->parent(this);
  rob->next(0); //reset any previous relationship
  m_node[0].base[1] += rob->size_word();

  //adjust `m_last' and `m_child' to point to the new last ROB
  if (m_last) m_last->next(rob);
  else m_child = rob;
  m_last = rob;
}

void offline_eformat::v40_write::FullEventFragment::append_unchecked (const uint32_t* rob)
{
  if (m_n_unchecked == MAX_UNCHECKED_FRAGMENTS)
    throw EFORMAT_TOO_MANY_UNCHECKED(MAX_UNCHECKED_FRAGMENTS);
  v40::ROBFragment frag(rob);
  if (m_n_unchecked) //more fragments are available before this one
    m_unchecked[m_n_unchecked-1].next = &m_unchecked[m_n_unchecked];
  set(m_unchecked[m_n_unchecked++], rob, frag.fragment_size_word(), 0);
  m_node[0].base[1] += frag.fragment_size_word();
}

uint32_t offline_eformat::v40_write::FullEventFragment::page_count (void) const
{
  uint32_t retval = 10 + (m_node[9].next?1:0) + (checksum_type()?1:0);
  for (const v40_write::ROBFragment* curr = m_child; curr; curr = curr->next())
    retval += curr->page_count();
  return retval+m_n_unchecked;
}

const eformat::write::node_t* 
offline_eformat::v40_write::FullEventFragment::bind (void)
{
  //the header is already concatenated by construction
  eformat::write::node_t* last = &m_node[9];
  eformat::write::node_t* payload = last; //buffer for an instant...
  if (m_node[10].size_word) last = last->next; //advance one node
  last->next = 0; //potentially remove old checksum

  //iterate over the attached children
  for (v40_write::ROBFragment* curr = m_child; curr; curr = curr->next()) {
    last->next = curr->bind();
    while (last->next) last = last->next; //advance until end
  }

  //make sure the last page of the last attached children points to the first
  //unchecked page, but only if we have unchecked ROB fragments appended
  if (m_n_unchecked) {
    last->next = m_unchecked;
    last = &m_unchecked[m_n_unchecked-1];
    last->next = 0; //potentially remove old checksum
  }

  //calculate the checksum if it was requested and we have data
  //in this part of the code "payload" points to 1 node before the begin
  if (checksum_type() != eformat::NO_CHECKSUM) {
    payload = payload->next; //position cursor at payload start
    if (!payload) {
      eformat::write::node_t null;
      set(null, 0, 0, 0);
      m_checksum = eformat::write::checksum(checksum_type(), &null);
    }
    else
      m_checksum = eformat::write::checksum(checksum_type(), payload);
    last->next = &m_node[11];
  }

  return m_node;
}

void offline_eformat::v40_write::FullEventFragment::checksum_type(uint32_t s)
{ 
  if (m_node[2].base[0] == eformat::NO_CHECKSUM && s != eformat::NO_CHECKSUM) {
    //Going from no checksum to having a checksum: update sizes
    m_node[0].base[1] += 1;
  }
  else if (m_node[2].base[0] != eformat::NO_CHECKSUM && 
      s == eformat::NO_CHECKSUM) {
    //Going from having a checksum to no checksum: update sizes
    m_node[0].base[1] -= 1;
  }
  m_node[2].base[0] = s;
}

const uint32_t* offline_eformat::v40_write::FullEventFragment::unchecked_fragment
(uint32_t n) const
{
  return m_unchecked[n].base;
}
