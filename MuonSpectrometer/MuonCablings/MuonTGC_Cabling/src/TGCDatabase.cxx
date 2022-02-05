/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabase.h"

namespace MuonTGC_Cabling
{

TGCDatabase::TGCDatabase(DatabaseType vtype)
  : m_type(vtype)
{
}
 
TGCDatabase::TGCDatabase(DatabaseType vtype,
			 const std::string& vfilename, 
			 const std::string& vblockname)
  : m_filename(vfilename),
    m_blockname(vblockname),
    m_type(vtype)
{
}

TGCDatabase::~TGCDatabase(void) 
{ 
}

int TGCDatabase::getEntry(int entry, int column) const {
  return m_database[entry].at(column);
}
 
int TGCDatabase::getEntrySize(int entry) const {
  return m_database[entry].size();
}
 
int TGCDatabase::getMaxEntry(void) const {
  return m_database.size();
}

bool TGCDatabase::update(const std::vector<int>&)
{
  // not implemented in the base class
  return false;
}
 
int  TGCDatabase::find(const std::vector<int>&) const
{
  // not implemented in the base class
  return -1;
}

int TGCDatabase::getIndexDBIn(int* indexIn) const
{
  // not implemented in the base class
  if(!indexIn) return -1;

  return -1;
}

int TGCDatabase::getIndexDBOut(int* indexOut) const
{
  // not implemented in the base class
  if(!indexOut) return -1;

  return -1;
}
 
} // end of namespace
