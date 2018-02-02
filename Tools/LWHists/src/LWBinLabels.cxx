/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWBinLabels                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2009                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWBinLabels.h"
#include "LWPools.h"
#include "LWStrUtils.h"
#include "TAxis.h"
#include <algorithm>

//____________________________________________________________________
LWBinLabels::LWBinLabels()
  : m_size(0),
    m_allocated(4),
    m_list(LWPools::acquire<Entry>(4))
{
}

//____________________________________________________________________
LWBinLabels::~LWBinLabels()
{
  for (unsigned i = 0; i < m_size; ++i)
    LWStrUtils::releaseString(m_list[i].second);
  LWPools::release<Entry>(m_list,m_allocated);
}

//____________________________________________________________________
void LWBinLabels::apply(TAxis*a) const
{
  for (unsigned i = 0; i < m_size; ++i) {
    Entry&e = m_list[i];
    a->SetBinLabel(e.first,e.second);
  }
}

//____________________________________________________________________
const char * LWBinLabels::getBinLabel(unsigned bin) const
{
  unsigned i = std::lower_bound(m_list,m_list+m_size,Entry(bin,0),cmp)-m_list;
  if (i>=m_size)
    return "";
  Entry & e = m_list[i];
  return e.first == bin && e.second ? e.second : "";
}

//____________________________________________________________________
void LWBinLabels::setBinLabel(unsigned bin, const char* label)
{
  //Usually we just need to append:
  if (m_size==0||bin>m_list[m_size-1].first) {
    if (m_allocated==m_size)
      grow();
    assert(m_allocated>m_size);
    Entry& e = m_list[m_size];
    e.first = bin;
    e.second = 0;
    LWStrUtils::setStringFromInput(label,e.second);
    ++m_size;
    return;
  }

  //Ok, we can't just append, so we must peek inside:
  unsigned i_lower = std::lower_bound(m_list,m_list+m_size,Entry(bin,0),cmp)-m_list;

  if (m_list[i_lower].first==bin) {
    //We are updating an existing entry:
    LWStrUtils::setStringFromInput(label,m_list[i_lower].second);
    return;
  }

  //We must insert in the middle!
  if (m_allocated==m_size)
    grow();

  //Move higher ones up:
  for (unsigned i = m_size; i>i_lower; --i)
    m_list[i] = m_list[i-1];

  //Insert:
  Entry e(bin,0);
  LWStrUtils::setStringFromInput(label,e.second);
  m_list[i_lower] = e;
  ++m_size;
}


//____________________________________________________________________
void LWBinLabels::grow()
{
  assert(m_allocated==m_size);
  unsigned l = std::min<unsigned>(m_size+4,static_cast<unsigned>(m_allocated*1.5+0.5));
  Entry * new_list = LWPools::acquire<Entry>(l);
  std::copy (m_list, m_list + m_size, new_list);
  LWPools::release<Entry>(m_list,m_allocated);
  m_list = new_list;
  m_allocated = l;
}
