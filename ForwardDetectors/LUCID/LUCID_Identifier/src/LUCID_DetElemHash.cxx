/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_Identifier/LUCID_DetElemHash.h"
#include "Identifier/IdentifierHash.h"


//__________________________________________________________________________
LUCID_DetElemHash::LUCID_DetElemHash()
  :
  m_helper(nullptr), 
  m_context(nullptr),
  m_max(16)
{}
//__________________________________________________________________________
LUCID_DetElemHash::LUCID_DetElemHash(const LUCID_DetElemHash& other)
{
  m_helper  = other.m_helper;
//   IdContext& cntx = *other.m_context;
  // m_context = new IdContext(cntx.prefix_id(), cntx.begin_index(), cntx.end_index());
  m_max     = other.m_max;
  m_context = nullptr;
}
//__________________________________________________________________________
LUCID_DetElemHash::~LUCID_DetElemHash()
{
//   delete m_context;
}

//__________________________________________________________________________
int LUCID_DetElemHash::max() const
{
 return m_max;
}

