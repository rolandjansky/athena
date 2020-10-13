//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file ROBFragment.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: salnikov $
 * $Revision: 726501 $
 * $Date: 2016-02-26 17:52:33 +0100 (Fri, 26 Feb 2016) $
 *
 * Implements the ROB fragment writing helper class
 */

#include "v40_write_FullEventFragment.h"
#include "v40_write_ROBFragment.h"
#include "eformat/HeaderMarker.h"
#include "v40_ROBFragment.h"
#include "eformat/Status.h"
#include "eformat/Issue.h"
#include "eformat/checksum.h"
#include "CxxUtils/checker_macros.h"
#include <cstring>

static const unsigned int ROB_HEADER = 0;
static const unsigned int ROB_STATUS = 1;
static const unsigned int ROB_CHECKSUM_TYPE = 2;
static const unsigned int ROD_HEADER = 3;
static const unsigned int ROD_STATUS = 4;
static const unsigned int ROD_DATA = 5;
static const unsigned int ROD_TRAILER = 6;
static const unsigned int ROB_CHECKSUM = 7;

void offline_eformat::v40_write::ROBFragment::initialize(void) {
  m_parent = 0;
  m_next = 0;
  m_header[0] = eformat::ROB; //marker
  m_header[1] = 21;
  m_header[2] = 8; //this header size + status size
  eformat::helper::Version version(0, eformat::MAJOR_V40_VERSION);
  m_header[3] = version.code(); //format version
  m_header[4] = 0; //source identifier of the ROB fragment
  m_header[5] = 1; //number of status
  m_header[6] = eformat::NO_CHECKSUM; //check sum
  m_rod_header[0] = eformat::ROD; //ROD marker
  m_rod_header[1] = 9; //ROD header size
  m_rod_header[2] = eformat::DEFAULT_ROD_VERSION; //format version
  m_rod_header[3] = 0; //source identifier of the ROD fragment
  m_rod_header[4] = 0; //run number
  m_rod_header[5] = 0; //LVL1 identifier
  m_rod_header[6] = 0; //bunch crossing identifier
  m_rod_header[7] = 0; //LVL1 type
  m_rod_header[8] = 0; //detector event type
  m_rod_trailer[0] = 1; //number of status in the ROD
  m_rod_trailer[1] = 0; //number of data words in the ROD
  m_rod_trailer[2] = eformat::STATUS_FRONT; //status block position

  //now initialize pages
  set(m_node[ROB_HEADER], m_header, 6, &m_node[ROB_STATUS]);
  set(m_node[ROB_STATUS], &eformat::DEFAULT_STATUS, 1, 
      &m_node[ROB_CHECKSUM_TYPE]);
  set(m_node[ROB_CHECKSUM_TYPE], &m_header[6], 1, &m_node[ROD_HEADER]);
  if (m_rod_trailer[2] == eformat::STATUS_FRONT) {
    set(m_node[ROD_HEADER], m_rod_header, 9, &m_node[ROD_STATUS]);
    set(m_node[ROD_STATUS], &eformat::DEFAULT_STATUS, 1, &m_node[ROD_DATA]);
    set(m_node[ROD_DATA], 0, 0, &m_node[ROD_TRAILER]);
  }
  else {
    set(m_node[ROD_HEADER], m_rod_header, 9, &m_node[ROD_DATA]);
    set(m_node[ROD_DATA], 0, 0, &m_node[ROD_STATUS]);
    set(m_node[ROD_STATUS], &eformat::DEFAULT_STATUS, 1, &m_node[ROD_TRAILER]);
  }
  set(m_node[ROD_TRAILER], m_rod_trailer, 3, 0);

  //optional checksum stuff (trailer), for the initialization, it remains
  //disconnected from the rest of the fragment.
  m_checksum = 0;
  set(m_node[ROB_CHECKSUM], &m_checksum, 1, 0); 
}

offline_eformat::v40_write::ROBFragment::ROBFragment
(uint32_t source_id, 
 uint32_t run_no, 
 uint32_t lvl1_id,
 uint32_t bc_id, 
 uint32_t lvl1_type, 
 uint32_t detev_type,
 uint32_t ndata, const uint32_t* data, uint32_t status_position)
{
  initialize();
  rob_source_id(source_id);
  rod_source_id(source_id);
  rod_run_no(run_no);
  rod_lvl1_id(lvl1_id);
  rod_bc_id(bc_id);
  rod_lvl1_type(lvl1_type);
  rod_detev_type(detev_type);

  //now initialize pages
  this->status_position(status_position);
  status(1, &eformat::DEFAULT_STATUS);
  rod_data(ndata, data);
}

offline_eformat::v40_write::ROBFragment::ROBFragment
(uint32_t rob_source_id, 
 uint32_t rod_source_id, 
 uint32_t run_no, 
 uint32_t lvl1_id,
 uint32_t bc_id, 
 uint32_t lvl1_type, 
 uint32_t detev_type,
 uint32_t ndata, const uint32_t* data, uint32_t status_position)
{
  initialize();
  this->rob_source_id(rob_source_id);
  this->rod_source_id(rod_source_id);
  rod_run_no(run_no);
  rod_lvl1_id(lvl1_id);
  rod_bc_id(bc_id);
  rod_lvl1_type(lvl1_type);
  rod_detev_type(detev_type);

  //now initialize pages
  this->status_position(status_position);
  status(1, &eformat::DEFAULT_STATUS);
  rod_data(ndata, data);
}

offline_eformat::v40_write::ROBFragment::ROBFragment ()
{
  initialize();
}

offline_eformat::v40_write::ROBFragment::ROBFragment
(const offline_eformat::v40_write::ROBFragment& other)
{
  *this = other;
}

offline_eformat::v40_write::ROBFragment::ROBFragment (const uint32_t* rod,
    uint32_t size_word)
{
  initialize();
  status(1, &eformat::DEFAULT_STATUS);
  memcpy(m_rod_header, rod, 9*sizeof(uint32_t)); 
  status_position(rod[size_word-1]);
  rod_data(rod[size_word-2], 
      status_position()?&rod[9]:&rod[9+rod[size_word-3]]); 
  rod_status(rod[size_word-3], 
      status_position()?&rod[9+rod[size_word-2]]:&rod[9]); 
  //make sure the ROD version is correct
  m_rod_header[2] = eformat::DEFAULT_ROD_VERSION | (0xffff & m_rod_header[2]);

  rob_source_id(rod_source_id()); //copies ROD source identifier
}

offline_eformat::v40_write::ROBFragment::ROBFragment (const uint32_t* rob)
{
  initialize();
  copy_header(rob);
  v40::ROBFragment read(rob);
  const uint32_t* tmp;

  //cares about the data section of the ROD fragment.
  if (read.check_rod_noex()) {
    read.rod_data(tmp);
    rod_data(read.rod_ndata(), tmp);
  }
  else if (read.payload_size_word() >= 9) {
    read.rod_start(tmp);
    rod_data(read.payload_size_word()-9, tmp+9);
  }
  else { //there is not even data here
    rod_data(0, 0);
  }
}

offline_eformat::v40_write::ROBFragment::~ROBFragment()
{
}

offline_eformat::v40_write::ROBFragment& offline_eformat::v40_write::ROBFragment::operator=
  (const offline_eformat::v40_write::ROBFragment& other)
{
  initialize();
  copy_header(other);
  rod_data(other.rod_ndata(), other.rod_data());
  return *this;
}

void offline_eformat::v40_write::ROBFragment::copy_header(const uint32_t* other) {
  v40::ROBFragment read(other);
  if (!read.check_rob_noex()) { //gives us a reasonable start up scenario
    throw EFORMAT_UNSUPPORTED_OPERATION("operate on corrupted", eformat::ROB);
  }
  eformat::helper::Version robver(read.version());
  minor_version(robver.minor_version());
  rob_source_id(read.source_id());
  const uint32_t* tmp;
  read.status(tmp);
  status(read.nstatus(), tmp);
  const uint32_t* rod;
  read.rod_start(rod);
  uint32_t rod_size = read.payload_size_word();
  
  if (rod_size >= 9) {
    memcpy(m_rod_header, rod, 9*sizeof(uint32_t));
  } 
  else {
    //we have less than 9 words!
    uint32_t size_diff = 9 - rod_size;
    memcpy(m_rod_header, rod, rod_size*sizeof(uint32_t));
    if (m_parent) m_parent->size_change(m_node[ROB_HEADER].base[1],
       m_node[ROB_HEADER].base[1] - size_diff);
    m_node[ROD_HEADER].size_word = rod_size;
    m_node[ROB_HEADER].base[1] -= size_diff; //total size should be reduced
  }

  if (read.check_rod_noex()) {
    status_position(read.rod_status_position());
    read.rod_status(tmp);
    rod_status(read.rod_nstatus(), tmp);
  }
  else {
    //this ROD is corrupted, so we should ignore its status words and the
    //trailer as well. Just put out what is there as it was data.
    rod_status(0, 0);
    m_node[ROD_TRAILER].size_word = 0;
    const uint32_t size_diff = 3;
    if (m_parent) m_parent->size_change(m_node[ROB_HEADER].base[1],
                m_node[ROB_HEADER].base[1] - size_diff);
    m_node[ROB_HEADER].base[1] -= size_diff;
  }
  checksum_type(read.checksum_type()); //re-links node[6] => node[7] if nedeed
}

void offline_eformat::v40_write::ROBFragment::copy_header
(const offline_eformat::v40_write::ROBFragment& other) {

  minor_version(other.minor_version());
  rob_source_id(other.source_id());
  memcpy(reinterpret_cast<void*>(m_rod_header),
	 reinterpret_cast<const void*>(other.m_rod_header), 9*sizeof(uint32_t));
  
  if (other.m_node[ROD_HEADER].size_word != 9) {
    //probably a corrupted ROD in the other side, set the appropriate size for
    //this header
    uint32_t size_diff = 9 - other.m_node[ROD_HEADER].size_word;
    m_node[ROD_HEADER].size_word = other.m_node[ROD_HEADER].size_word;
    if (m_parent) m_parent->size_change(m_node[ROB_HEADER].base[1],
       m_node[ROB_HEADER].base[1] - size_diff);
    m_node[ROB_HEADER].base[1] -= size_diff;
  }

  status(other.nstatus(), other.status());
  status_position(other.status_position());
  rod_status(other.rod_nstatus(), other.rod_status());

  if (other.m_node[ROD_TRAILER].size_word != 3) {
    //probably a corrupted ROD in the other side, zero trailer!
    uint32_t size_diff = 3 - other.m_node[ROD_TRAILER].size_word;
    m_node[ROD_TRAILER].size_word = other.m_node[ROD_TRAILER].size_word;
    if (m_parent) m_parent->size_change(m_node[ROB_HEADER].base[1],
                m_node[ROB_HEADER].base[1] - size_diff);
    m_node[ROB_HEADER].base[1] -= size_diff;
  }
  checksum_type(other.checksum_type()); //re-links node[6] => node[7] if nedeed
}

void offline_eformat::v40_write::ROBFragment::status (uint32_t n, const uint32_t* status)
{ 
  if (m_parent) m_parent->size_change(m_node[ROB_HEADER].base[1],
				      m_node[ROB_HEADER].base[1]-m_node[ROB_HEADER].base[5]+n);
  m_node[ROB_HEADER].base[1] -= m_node[ROB_HEADER].base[5]; //remove count from previous status
  m_node[ROB_HEADER].base[2] -= m_node[ROB_HEADER].base[5]; //remove count from previous status
  m_node[ROB_STATUS].size_word = m_node[ROB_HEADER].base[5] = n; //set new values
  m_node[ROB_HEADER].base[1] += n;
  m_node[ROB_HEADER].base[2] += n;
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* status_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(status);
  m_node[ROB_STATUS].base = status_nc;
}

void offline_eformat::v40_write::ROBFragment::rod_status (uint32_t n,
					      const uint32_t* status)
{ 
  if (m_parent) m_parent->size_change(m_node[ROB_HEADER].base[1], 
				      m_node[ROB_HEADER].base[1]-m_node[ROD_TRAILER].base[0]+n);
  m_node[ROB_HEADER].base[1] -= m_node[ROD_TRAILER].base[0]; //remove count from previous status
  m_node[ROD_STATUS].size_word = m_node[ROD_TRAILER].base[0] = n; //set new values
  m_node[ROB_HEADER].base[1] += n; //set ROB header's total fragment size
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* status_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(status);
  m_node[ROD_STATUS].base = status_nc;
}

void offline_eformat::v40_write::ROBFragment::status_position (uint32_t v)
{ 
  if (v == m_node[ROD_TRAILER].base[2]) return; //do nothing in this case:)
  m_node[ROD_TRAILER].base[2] = v;
  if (m_node[ROD_TRAILER].base[2] == eformat::STATUS_FRONT) {
    m_node[ROD_HEADER].next = &m_node[ROD_STATUS];
    m_node[ROD_STATUS].next = &m_node[ROD_DATA];
    m_node[ROD_DATA].next = &m_node[ROD_TRAILER];
  }
  else {
    m_node[ROD_HEADER].next = &m_node[ROD_DATA];
    m_node[ROD_DATA].next = &m_node[ROD_STATUS];
    m_node[ROD_STATUS].next = &m_node[ROD_TRAILER];
  }
}

void offline_eformat::v40_write::ROBFragment::rod_data (uint32_t n, const uint32_t* data)
{
  if (m_parent) m_parent->size_change(m_node[ROB_HEADER].base[1], 
				      m_node[ROB_HEADER].base[1]-m_node[ROD_TRAILER].base[1]+n);

  //remove count from previous data size
  m_node[ROB_HEADER].base[1] -= m_node[ROD_TRAILER].base[1]; 
  m_node[ROD_DATA].size_word = m_node[ROD_TRAILER].base[1] = n; //set new values
  m_node[ROB_HEADER].base[1] += n; //set ROB header's total fragment size back
  // FIXME: I'm assuming that these are ok.
  // Probably can't avoid this as long as we're using node_t from eformat.
  uint32_t* data_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*>(data); 
  m_node[ROD_DATA].base = data_nc;
}

void offline_eformat::v40_write::ROBFragment::checksum_type(uint32_t s)
{ 
  if (m_node[ROB_CHECKSUM_TYPE].base[0] == eformat::NO_CHECKSUM && s != eformat::NO_CHECKSUM) {
    //Going from no checksum to having a checksum: update sizes
    if (m_parent) 
      m_parent->size_change(m_node[ROB_HEADER].base[1], m_node[ROB_HEADER].base[1]+1);
    m_node[ROB_HEADER].base[1] += 1;
  }
  else if (m_node[ROB_CHECKSUM_TYPE].base[0] != eformat::NO_CHECKSUM && 
      s == eformat::NO_CHECKSUM) {
    //Going from having a checksum to no checksum: update sizes
    if (m_parent) 
      m_parent->size_change(m_node[ROB_HEADER].base[1], m_node[ROB_HEADER].base[1]-1);
    m_node[ROB_HEADER].base[1] -= 1;
  }
  m_header[6] = m_node[ROB_CHECKSUM_TYPE].base[0] = s;
}

const eformat::write::node_t* offline_eformat::v40_write::ROBFragment::rod_bind(void)
{
  m_node[ROD_TRAILER].next = 0; //cuts off the checksum word
  return &m_node[ROD_HEADER];
}

eformat::write::node_t* offline_eformat::v40_write::ROBFragment::bind(void)
{
  m_node[ROD_TRAILER].next = 0; //cuts off the checksum word
  m_node[ROB_CHECKSUM].next = 0; //cuts off relation ships with other fragments 
  if (checksum_type() != eformat::NO_CHECKSUM) {
    m_checksum = eformat::write::checksum(checksum_type(), rod_bind());
    m_node[ROD_TRAILER].next = &m_node[ROB_CHECKSUM];
  }
  return m_node;
}

