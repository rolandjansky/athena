/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabase.h"

namespace MuonTGC_Cabling
{

TGCDatabase::TGCDatabase(DatabaseType vtype)
  : m_type(vtype)
{
}
 
TGCDatabase::TGCDatabase(DatabaseType vtype,
			 std::string vfilename, 
			 std::string vblockname)
  : m_filename(vfilename),
    m_blockname(vblockname),
    m_type(vtype)
{
}

TGCDatabase::TGCDatabase(const TGCDatabase& right)
{
  // copy member
  m_type      = right.m_type;
  m_filename  = right.m_filename;
  m_blockname = right.m_blockname;
  
  // copy database
  const size_t database_size = right.m_database.size();
  for(size_t ip=0; ip < database_size; ip+=1){
    std::vector<int> entry;
    const size_t database_ip_size = right.m_database[ip].size();
    for(size_t ic=0; ic < database_ip_size; ic+=1) {
      entry.push_back(right.m_database[ip].at(ic));
    } 
    m_database.push_back(entry);    
  }
}

TGCDatabase& TGCDatabase::operator=(const TGCDatabase& right)
{
  if(this != &right) {
    // clear all contents
    clear();

    // copy member
    m_type      = right.m_type;
    m_filename  = right.m_filename;
    m_blockname = right.m_blockname;

    // copy database
    m_database = right.m_database;
    /*
    const size_t database_size = right.m_database.size();
    for(size_t ip=0; ip < database_size; ip+=1){
      std::vector<int> entry;
      const size_t database_ip_size = right.m_database[ip].size();
      for(size_t ic=0; ic < database_ip_size; ic+=1) {
	entry.push_back(right.m_database[ip].at(ic));
      } 
      m_database.push_back(entry);    
    } 
    */    
  }
  return *this;
}

TGCDatabase::~TGCDatabase(void) 
{ 
  clear();
}

void TGCDatabase::clear()
{
  const unsigned int size = m_database.size();
  for(unsigned int i=0; i<size; i++){
    m_database[i].clear();
  }
  m_database.clear();
}
 
int TGCDatabase::getEntry(int entry, int column) {
  if(m_database.size()==0) readDB();
  return m_database[entry].at(column);
}
 
int TGCDatabase::getEntrySize(int entry) {
  if(m_database.size()==0) readDB();
  return m_database[entry].size();
}
 
int TGCDatabase::getMaxEntry(void) {
  if(m_database.size()==0) readDB();
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

int TGCDatabase::getIndexDBIn(int* indexIn) 
{
  // not implemented in the base class
  if(!indexIn) return -1;

  return -1;
}

int TGCDatabase::getIndexDBOut(int* indexOut) 
{
  // not implemented in the base class
  if(!indexOut) return -1;

  return -1;
}
 
} // end of namespace
