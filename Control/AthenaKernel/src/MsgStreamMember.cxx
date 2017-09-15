/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file MsgStreamMember.cxx
 *  @brief MsgStreamMember implementation
 *
 *  $Id: MsgStreamMember.cxx,v 1.1 2008-07-14 22:10:14 calaf Exp $
 *  @author Paolo Calafiura - Atlas collaboration
 */
#include "AthenaKernel/MsgStreamMember.h"
using namespace Athena;
/// @param o: if o is @c Athena::Options::Eager it will create a
/// @c MsgStream instance there and then.
MsgStreamMember::MsgStreamMember(const Athena::Options::CreateOptions opt, 
				 const std::string& label) :   
  m_ims(opt), m_label(label), m_stream (0)
{
  if (opt == Athena::Options::Eager) m_stream = new MsgStream(m_ims.get(), m_label);
}

MsgStreamMember& MsgStreamMember::operator= (const MsgStreamMember& rhs)
{
  if (this != &rhs) {
    m_ims = rhs.m_ims;
    m_label = rhs.m_label;
    m_stream = 0;
  }
  return *this;
}

MsgStreamMember::~MsgStreamMember() { delete m_stream; }

/// upon first access sets m_ims as needed
MsgStream& MsgStreamMember::get() {
  if (0 == m_stream) m_stream = new MsgStream(m_ims.get(), m_label);
  return *m_stream;
}
