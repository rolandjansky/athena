/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file FullEventFragmentNoTemplates.cxx 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Implements full event fragments without using templates to ease
 * maintenance (patching).
 */

#include "v40_FullEventFragment.h"
#include "eformat/HeaderMarker.h"
#include "eformat/util.h"

offline_eformat::v40::FullEventFragment::FullEventFragment (const uint32_t* it)
  : Header(it),
    m_start()
{
  Header::specific_header(m_start);
}

offline_eformat::v40::FullEventFragment::FullEventFragment ()
  : Header(), m_start() 
{
}

offline_eformat::v40::FullEventFragment::FullEventFragment
(const offline_eformat::v40::FullEventFragment& other)
  : Header(other), m_start(other.m_start) 
{
}

offline_eformat::v40::FullEventFragment::FullEventFragment
  (const eformat::helper::u32list& blob)
  : Header(blob.ptr()),
    m_start()
{
  Header::specific_header(m_start);
}

offline_eformat::v40::FullEventFragment::FullEventFragment
  (const eformat::helper::u32slice& blob)
  : Header(blob.ptr()),
    m_start()
{
  Header::specific_header(m_start);
}

offline_eformat::v40::FullEventFragment::~FullEventFragment ()
{
}

offline_eformat::v40::FullEventFragment& offline_eformat::v40::FullEventFragment::operator=
(const offline_eformat::v40::FullEventFragment& other)
{ 
  Header::operator=(other);
  m_start=other.m_start; 
  return *this; 
}

offline_eformat::v40::FullEventFragment& offline_eformat::v40::FullEventFragment::assign
(const uint32_t* it)
{
  Header::assign(it);
  Header::specific_header(m_start);
  return *this;
}

const uint32_t* offline_eformat::v40::FullEventFragment::lvl1_trigger_info () const
{
  return m_start + 10;
}

const uint32_t* offline_eformat::v40::FullEventFragment::lvl2_trigger_info () const
{
  return m_start + 11 + nlvl1_trigger_info();
}

const uint32_t* offline_eformat::v40::FullEventFragment::event_filter_info () const
{
  return m_start + 12 + nlvl1_trigger_info() + nlvl2_trigger_info();
}

const uint32_t* offline_eformat::v40::FullEventFragment::stream_tag () const
{
  return m_start + 13 + nlvl1_trigger_info() + nlvl2_trigger_info() + 
    nevent_filter_info();
}


bool 
offline_eformat::v40::FullEventFragment::check_impl
(const uint16_t version,
 const bool exc) 
  const
{
  
  bool result = true;

  //Fragment checking, only this header
  try{
    if (Header::marker() != eformat::FULL_EVENT) {
      throw EFORMAT_WRONG_MARKER(marker(), eformat::FULL_EVENT);
    }
    
    if (Header::version() >> 16 != version) {
      throw EFORMAT_BAD_VERSION(Header::version() >> 16, version);
    }
    
    uint32_t calculated = 20 + Header::nstatus() +
      nlvl1_trigger_info() + nlvl2_trigger_info() + nevent_filter_info() + 
      nstream_tag();
    if (Header::header_size_word() != calculated) {
      throw EFORMAT_SIZE_CHECK(calculated,  Header::header_size_word());
    }
  }catch(eformat::WrongMarkerIssue &ex){
    if(exc) throw ex;
    result = false;
  }catch(eformat::BadVersionIssue &ex){
    if(exc) throw ex;
    result = false;
  }catch(eformat::SizeCheckIssue &ex){
    if(exc) throw ex;
    result = false;
  }
      
  return result;
}


bool offline_eformat::v40::FullEventFragment::check (const uint16_t version) const
{
  return this->check_impl(version, true);
}

bool offline_eformat::v40::FullEventFragment::check_noex (const uint16_t version) const
{
  return this->check_impl(version, false);
}

bool 
offline_eformat::v40::FullEventFragment::check_tree_impl
(const uint16_t version, const uint16_t rod_version, const bool exc)
  const
{

  if (!check_impl(version, exc)) return false; // check myself
  uint32_t total = nchildren();
  for (size_t i=0; i<total; ++i) {
    const uint32_t* fp;
    child(fp, i);
    ROBFragment f(fp);
    bool rob_check;
    try{
      rob_check = f.check(version, rod_version);
    }catch(eformat::RODSizeCheckIssue &ex){
      //In case of a truncated ROD fragment, we ignore and continue
      //Anyway the ROB is good, hence we can safely navigate the event
      continue;
    }catch(eformat::WrongMarkerIssue &ex){
      if(exc) throw ex;
      rob_check = false;
    }catch(eformat::BadVersionIssue &ex){
      if(exc) throw ex;
      rob_check = false;
    }catch(eformat::SizeCheckIssue &ex){
      if(exc) throw ex;
      rob_check = false;
    }catch(eformat::BadRodVersionIssue &ex){
      if(exc) throw ex;
      rob_check = false;
    }catch(eformat::Issue &ex){
      if(exc) throw ex;
      rob_check = false;
    }
    
    if (!rob_check) return false;
  }
  return true;
}

bool offline_eformat::v40::FullEventFragment::check_tree (const uint16_t version,
    const uint16_t rod_version) 
  const
{
  return this->check_tree_impl(version, rod_version, true);
}

bool offline_eformat::v40::FullEventFragment::check_tree_noex (const uint16_t version,
    const uint16_t rod_version)
  const
{
  return this->check_tree_impl(version, rod_version, false);
}

void offline_eformat::v40::FullEventFragment::problems
  (std::vector<eformat::FragmentProblem>& p, const uint16_t version) const {
  //Fragment checking, only this header
  if (Header::marker() != eformat::FULL_EVENT)
    p.push_back(eformat::WRONG_MARKER);
  if ( Header::version() >> 16 != version )
    p.push_back(eformat::UNSUPPORTED_VERSION);
  uint32_t calculated = 20 + Header::nstatus() +
    nlvl1_trigger_info() + nlvl2_trigger_info() + nevent_filter_info() + 
    nstream_tag();
  if (Header::header_size_word() != calculated)
    p.push_back(eformat::WRONG_FRAGMENT_SIZE);
}

void 
offline_eformat::v40::FullEventFragment::problems_tree
(std::vector<eformat::helper::ProblemContainer>& p,
 const uint16_t version,
 const uint16_t rod_version) 
  const
{
  std::vector<eformat::FragmentProblem> tmp; 
  this->problems(tmp, version);
  
  if (!tmp.empty()){
    eformat::helper::ProblemContainer pc(this->source_id(), tmp);
    p.push_back(pc);
    return;
  }
  
  uint32_t total = this->nchildren();
  for (size_t i=0; i<total; ++i) {
    const uint32_t* fp;
    child(fp, i);
    ROBFragment f(fp);
    std::vector<eformat::FragmentProblem> tmp; 
    f.problems(tmp, version, rod_version);
    eformat::helper::ProblemContainer pc(f.source_id(), tmp);
    p.push_back(pc);
  }
}

void offline_eformat::v40::FullEventFragment::robs
(std::vector<ROBFragment>& r) const
{
  uint32_t total = nchildren();
  r.reserve(total);
  for (size_t i=0; i<total; ++i) {
    const uint32_t* fp;
    child(fp, i);
    r.push_back(fp);
  }
}

void offline_eformat::v40::FullEventFragment::robs
(std::vector<ROBFragment>& good,
 std::vector<ROBFragment>& bad,
  const uint16_t version, const uint16_t rod_version) const
{
  uint32_t total = nchildren();
  for (size_t i=0; i<total; ++i) {
    const uint32_t* fp;
    child(fp, i);
    ROBFragment f(fp);
    if (f.check_noex(version, rod_version)) good.push_back(f);
    else bad.push_back(f);
  }
}

uint32_t offline_eformat::v40::FullEventFragment::nchildren () const
{
  return children(0,0);
}

const uint32_t* offline_eformat::v40::FullEventFragment::child (size_t n) const
{
  const uint32_t* next = Header::payload();
  for (size_t i=0; i<n; ++i) next += next[1];
  return next;
}

const uint32_t* offline_eformat::v40::FullEventFragment::child_check (size_t n) const
{
  uint32_t total = nchildren();
  if (n >= total) throw EFORMAT_NO_SUCH_CHILD(n, total);
  return child(n);
}

uint32_t offline_eformat::v40::FullEventFragment::children (const uint32_t** p, size_t max) const
{
  const uint32_t* payload_start;
  Header::payload(payload_start);
  return find_fragments(child_marker(static_cast<eformat::HeaderMarker>(Header::marker())), payload_start, Header::payload_size_word(), p, max);
}

uint32_t offline_eformat::v40::FullEventFragment::bc_time_seconds() const
{ 
  return m_start[0]; 
}

uint32_t offline_eformat::v40::FullEventFragment::bc_time_nanoseconds() const
{ 
  return m_start[1]; 
}

uint32_t offline_eformat::v40::FullEventFragment::global_id() const
{ 
  return m_start[2]; 
}

uint32_t offline_eformat::v40::FullEventFragment::run_type() const
{ 
  return m_start[3]; 
}

uint32_t offline_eformat::v40::FullEventFragment::run_no() const
{ 
  return m_start[4]; 
}

uint16_t offline_eformat::v40::FullEventFragment::lumi_block() const
{ 
  return m_start[5]; 
}

uint32_t offline_eformat::v40::FullEventFragment::lvl1_id() const
{ 
  return m_start[6]; 
}

uint16_t offline_eformat::v40::FullEventFragment::bc_id() const
{ 
  return m_start[7]; 
}

uint8_t offline_eformat::v40::FullEventFragment::lvl1_trigger_type() const
{ 
  return m_start[8]; 
}

uint32_t offline_eformat::v40::FullEventFragment::nlvl1_trigger_info() const
{ 
  return m_start[9]; 
}

uint32_t offline_eformat::v40::FullEventFragment::nlvl2_trigger_info() const
{ 
  return m_start[10+nlvl1_trigger_info()]; 
}

uint32_t offline_eformat::v40::FullEventFragment::nevent_filter_info () const
{ 
  return m_start[11+nlvl1_trigger_info()+nlvl2_trigger_info()]; 
}

uint32_t offline_eformat::v40::FullEventFragment::nstream_tag () const
{ 
  return m_start[12+nlvl1_trigger_info() + 
    nlvl2_trigger_info() + nevent_filter_info()]; 
}
