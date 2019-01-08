/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCDatabase.h"

namespace LVL1TGCCabling8
{

TGCDatabase::TGCDatabase (DatabaseType vtype)
  : m_type(vtype)
{
}

TGCDatabase::TGCDatabase (DatabaseType vtype,
			  std::string  vfilename, 
			  std::string  vblockname)
  : m_filename(vfilename),
    m_blockname(vblockname),
    m_type(vtype)
{
  if(m_database.size()==0) readDB();  
}

TGCDatabase::~TGCDatabase (void)
{}

int TGCDatabase::getEntry (int entry, int column) {
  if(m_database.size()==0) readDB();
  return m_database[entry].at(column);
}
 
int TGCDatabase::getEntrySize (int entry) {
  if(m_database.size()==0) readDB();
  return m_database[entry].size();
}
 
int TGCDatabase::getMaxEntry (void) {
  if(m_database.size()==0) readDB();
  return m_database.size();
}

int TGCDatabase::getIndexDBIn (int* indexIn) {
  if(!indexIn) return -1;

  return -1;
}

int TGCDatabase::getIndexDBOut (int* indexOut) {
  if(!indexOut) return -1;

  return -1;
}

} // end of namespace
