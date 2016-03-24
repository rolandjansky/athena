/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ROBFragmentNoTemplates.cxx 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Implements untemplated read-only ROB fragments. 
 */

#include "v40_ROBFragment.h"
#include "eformat/HeaderMarker.h"
#include "eformat/Issue.h"

offline_eformat::v40::ROBFragment::ROBFragment (const uint32_t* it)
  : Header(it),
    m_start()
{
  Header::specific_header(m_start);
}

offline_eformat::v40::ROBFragment::ROBFragment
  (const offline_eformat::v40::ROBFragment& other)
  : Header(other), m_start(other.m_start)
{
}

offline_eformat::v40::ROBFragment::ROBFragment ()
  : Header(), m_start()
{
}

offline_eformat::v40::ROBFragment::ROBFragment (const eformat::helper::u32list& blob)
  : Header(blob.ptr()),
    m_start()
{
  Header::specific_header(m_start);
}

offline_eformat::v40::ROBFragment::ROBFragment (const eformat::helper::u32slice& blob)
  : Header(blob.ptr()),
    m_start()
{
  Header::specific_header(m_start);
}

offline_eformat::v40::ROBFragment::~ROBFragment ()
{
}

offline_eformat::v40::ROBFragment& offline_eformat::v40::ROBFragment::operator=
(const offline_eformat::v40::ROBFragment& other)
{ 
  Header::operator=(other); 
  m_start=other.m_start; 
  return *this; 
}

offline_eformat::v40::ROBFragment&
offline_eformat::v40::ROBFragment::assign (const uint32_t* it)
{
  Header::assign(it);
  Header::specific_header(m_start);
  return *this;
}

bool 
offline_eformat::v40::ROBFragment::check_rob_impl (const uint16_t version,
					    const bool exc) 
  const
{
  bool result = true;

  try{
    if (Header::marker() != eformat::ROB) {
      throw EFORMAT_WRONG_MARKER(marker(), eformat::ROB);
    }
    if (Header::version() >> 16 != version) {
      throw EFORMAT_BAD_VERSION(Header::version() >> 16, version);
    }
    uint32_t calculated = 7 + Header::nstatus();
    if (Header::header_size_word() != calculated) {
      throw EFORMAT_SIZE_CHECK(calculated, Header::header_size_word());
    }
  }catch(eformat::WrongMarkerIssue &ex){
    if(exc) throw ex;
    result = false;
  }catch(eformat::SizeCheckIssue &ex){
    if(exc) throw ex;
    result = false;
  }catch(eformat::BadVersionIssue &ex){
    if(exc) throw ex;
    result = false;
  }
  
  return result;
}

bool offline_eformat::v40::ROBFragment::check_rob_noex (const uint16_t version) const
{
  return this->check_rob_impl(version, false);
}

bool offline_eformat::v40::ROBFragment::check_rob (const uint16_t version) const
{
    return this->check_rob_impl(version, true);
}


bool 
offline_eformat::v40::ROBFragment::check_rod_impl (const uint16_t version,
					    const bool exc) 
  const
{
  bool result = true;

  try{
    if ( rod_version() >> 16 != version ) {
      throw EFORMAT_BAD_ROD_VERSION(rod_version() >> 16, version);
    }
    if ( rod_header_size_word() != 9 ) {
      throw EFORMAT_SIZE_CHECK(9, rod_header_size_word());
    }
    if ( rod_fragment_size_word() != 12 + rod_nstatus() + rod_ndata() ) {
      throw EFORMAT_ROD_SIZE_CHECK(rod_fragment_size_word(),
				   (12 + rod_nstatus() + rod_ndata()));
    }
  }catch(eformat::RODSizeCheckIssue &ex){
    if(exc) throw ex;
    result = false;
  }catch(eformat::SizeCheckIssue &ex){
    if(exc) throw ex;
    result = false;
  }catch(eformat::BadRodVersionIssue &ex){
    if(exc) throw ex;
    result = false;
  }

  return result;
}

bool offline_eformat::v40::ROBFragment::check_rod_noex (const uint16_t version) const
{
  return this->check_rod_impl(version, false);
}

bool offline_eformat::v40::ROBFragment::check_rod (const uint16_t version) const
{
  return this->check_rod_impl(version, true);
}


bool 
offline_eformat::v40::ROBFragment::check_impl(const uint16_t version,
				       const uint16_t rod_version,
				       const bool exc) 
  const
{
  return this->check_rob_impl(version, exc) && 
    this->check_rod_impl(rod_version, exc);
}

bool 
offline_eformat::v40::ROBFragment::check_noex (const uint16_t version,
					const uint16_t rod_version) const
{
  return this->check_impl(version, rod_version, false);
}

bool 
offline_eformat::v40::ROBFragment::check (const uint16_t version,
				   const uint16_t rod_version) const
{
  return this->check_impl(version, rod_version, true);
}

void offline_eformat::v40::ROBFragment::rob_problems
  (std::vector<eformat::FragmentProblem>& p, const uint16_t version) const 
{
  //ROB checking
  if (Header::marker() != eformat::ROB)
    p.push_back(eformat::WRONG_MARKER);
  if ( Header::version() >> 16 != version )
    p.push_back(eformat::UNSUPPORTED_VERSION);
  uint32_t calculated = 7 + Header::nstatus();
  if (Header::header_size_word() != calculated)
    p.push_back(eformat::WRONG_FRAGMENT_SIZE);
}

void offline_eformat::v40::ROBFragment::rod_problems
  (std::vector<eformat::FragmentProblem>& p, const uint16_t version) const 
{
  //ROD stuff
  if (rod_marker() != eformat::ROD) p.push_back(eformat::WRONG_ROD_MARKER); 
  if (rod_version() >> 16 != version)
    p.push_back(eformat::UNSUPPORTED_ROD_VERSION);
  if (rod_header_size_word() != 9) p.push_back(eformat::WRONG_ROD_HEADER_SIZE);
  if (rod_fragment_size_word() != 12 + rod_nstatus() + rod_ndata())
    p.push_back(eformat::WRONG_ROD_FRAGMENT_SIZE);
}

void offline_eformat::v40::ROBFragment::problems
(std::vector<eformat::FragmentProblem>& p,
 const uint16_t version, const uint16_t rod_version) const
{
  rob_problems(p, version);
  rod_problems(p, rod_version);
}

const uint32_t* offline_eformat::v40::ROBFragment::rod_status () const
{
  if (rod_status_position()) return m_start + 9 + rod_ndata();
  return m_start + 9;
}

const uint32_t* offline_eformat::v40::ROBFragment::rod_data () const
{
  if (!rod_status_position()) return m_start + 9 + rod_nstatus();
  return m_start + 9;
}

const uint32_t* offline_eformat::v40::ROBFragment::child (size_t n) const
{
  if (n == 0) return m_start;
  return 0;
}

uint32_t offline_eformat::v40::ROBFragment::children (const uint32_t** p, size_t max)
  const
{
  if (max == 0) return 0;
  p[0] = m_start;
  return 1;
}

uint32_t offline_eformat::v40::ROBFragment::rod_nstatus () const
{ 
  return m_start[Header::payload_size_word()-3];
}

uint32_t offline_eformat::v40::ROBFragment::rod_ndata () const
{ 
  return m_start[Header::payload_size_word()-2];
}

uint32_t offline_eformat::v40::ROBFragment::rod_status_position () const
{ 
  return m_start[Header::payload_size_word()-1];
}

uint32_t offline_eformat::v40::ROBFragment::rob_source_id () const
{ 
  return Header::source_id(); 
}

const uint32_t* offline_eformat::v40::ROBFragment::rod_start () const
{ 
  return m_start; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_marker() const
{ 
  return m_start[0]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_fragment_size_word() const
{ 
  return Header::payload_size_word();
}

uint32_t offline_eformat::v40::ROBFragment::rod_header_size_word() const
{ 
  return m_start[1]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_trailer_size_word() const
{ 
  return 3; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_version() const
{ 
  return m_start[2]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_source_id() const
{ 
  return m_start[3]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_run_no() const
{ 
  return m_start[4]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_lvl1_id() const
{ 
  return m_start[5]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_bc_id() const
{ 
  return m_start[6]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_lvl1_trigger_type() const
{ 
  return m_start[7]; 
}

uint32_t offline_eformat::v40::ROBFragment::rod_detev_type() const
{ 
  return m_start[8]; 
}

uint32_t offline_eformat::v40::ROBFragment::nchildren () const
{ 
  return 1; 
}
