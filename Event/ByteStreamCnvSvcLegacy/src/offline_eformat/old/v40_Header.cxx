/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HeaderNoTemplates.cxx 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Implementation of the eformat Header w/o templates
 */

#include "v40_Header.h"
#include "eformat/checksum.h"

offline_eformat::v40::Header::Header (const uint32_t* it)
  : m_top(it)
{
}

offline_eformat::v40::Header::Header ()
  : m_top()
{
}

offline_eformat::v40::Header::Header (const offline_eformat::v40::Header& other)
  : m_top(other.m_top)
{
}

offline_eformat::v40::Header::~Header()
{
}

offline_eformat::v40::Header& offline_eformat::v40::Header::operator= (const offline_eformat::v40::Header& other)
{ 
  m_top = other.m_top; 
  return *this; 
}

uint32_t offline_eformat::v40::Header::marker() const
{ 
  return m_top[0]; 
}

uint32_t offline_eformat::v40::Header::fragment_size_word() const
{ 
  return m_top[1]; 
}

uint32_t offline_eformat::v40::Header::header_size_word() const
{ 
  return m_top[2]; 
}

uint32_t offline_eformat::v40::Header::version() const
{ 
  return m_top[3]; 
}

uint32_t offline_eformat::v40::Header::source_id() const
{ 
  return m_top[4]; 
}

uint32_t offline_eformat::v40::Header::nstatus () const
{ 
  return m_top[5]; 
}

const uint32_t* offline_eformat::v40::Header::start () const
{ 
  return m_top;
}

const uint32_t* offline_eformat::v40::Header::payload () const
{ 
  return m_top + header_size_word();
}

const uint32_t* offline_eformat::v40::Header::end () const
{ 
  return m_top + fragment_size_word();
}

uint32_t offline_eformat::v40::Header::payload_size_word (void) const
{ 
  return fragment_size_word() - (header_size_word() + (checksum_type()?1:0)); 
}

const uint32_t* offline_eformat::v40::Header::status () const
{ 
  return m_top + 6;
}

uint32_t offline_eformat::v40::Header::checksum_type () const
{ 
  return m_top[6 + nstatus()]; 
}

uint32_t offline_eformat::v40::Header::nspecific () const
{ 
  return header_size_word() - ( 7 + nstatus() );
}
    
const uint32_t* offline_eformat::v40::Header::specific_header () const
{ 
  return m_top + 7 + nstatus(); 
}

uint32_t offline_eformat::v40::Header::checksum_value (void) const
{ 
  return (checksum_type()?m_top[fragment_size_word()-1]:0); 
}
    
offline_eformat::v40::Header& offline_eformat::v40::Header::assign (const uint32_t* it)
{
  m_top = it;
  return *this;
}

uint32_t offline_eformat::v40::Header::eval_checksum (void) const
{
  return eformat::helper::checksum(checksum_type(), payload(), payload_size_word());
}

bool offline_eformat::v40::Header::checksum (void) const
{
  return (eval_checksum() == checksum_value());
}
