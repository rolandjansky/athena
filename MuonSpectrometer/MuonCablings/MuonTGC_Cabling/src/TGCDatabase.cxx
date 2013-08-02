/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabase.h"

namespace MuonTGC_Cabling
{

TGCDatabase::TGCDatabase(DatabaseType vtype)
  : type(vtype)
{
}
 
TGCDatabase::TGCDatabase(DatabaseType vtype,
			 std::string vfilename, 
			 std::string vblockname)
  : filename(vfilename),
    blockname(vblockname),
    type(vtype)
{
}

TGCDatabase::TGCDatabase(const TGCDatabase& right)
{
  // copy member
  type      = right.type;
  filename  = right.filename;
  blockname = right.blockname;
  
  // copy database
  const size_t database_size = right.database.size();
  for(size_t ip=0; ip < database_size; ip+=1){
    std::vector<int> entry;
    const size_t database_ip_size = right.database[ip].size();
    for(size_t ic=0; ic < database_ip_size; ic+=1) {
      entry.push_back(right.database[ip].at(ic));
    } 
    database.push_back(entry);    
  }
}

TGCDatabase& TGCDatabase::operator=(const TGCDatabase& right)
{
  if(this != &right) {
    // clear all contents
    clear();

    // copy member
    type      = right.type;
    filename  = right.filename;
    blockname = right.blockname;

    // copy database
    database = right.database;
    /*
    const size_t database_size = right.database.size();
    for(size_t ip=0; ip < database_size; ip+=1){
      std::vector<int> entry;
      const size_t database_ip_size = right.database[ip].size();
      for(size_t ic=0; ic < database_ip_size; ic+=1) {
	entry.push_back(right.database[ip].at(ic));
      } 
      database.push_back(entry);    
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
  const unsigned int size = database.size();
  for(unsigned int i=0; i<size; i++){
    database[i].clear();
  }
  database.clear();
}
 
int TGCDatabase::getEntry(int entry, int column) {
  if(database.size()==0) readDB();
  return database[entry].at(column);
}
 
int TGCDatabase::getEntrySize(int entry) {
  if(database.size()==0) readDB();
  return database[entry].size();
}
 
int TGCDatabase::getMaxEntry(void) {
  if(database.size()==0) readDB();
  return database.size();
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
