/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCDatabase.h"

namespace LVL1TGCCabling8
{

TGCDatabase::TGCDatabase (DatabaseType vtype)
  : type(vtype)
{
}

TGCDatabase::TGCDatabase (DatabaseType vtype,
			  std::string  vfilename, 
			  std::string  vblockname)
  : filename(vfilename),
    blockname(vblockname),
    type(vtype)
{
  if(database.size()==0) readDB();  
}

TGCDatabase::~TGCDatabase (void)
{}

int TGCDatabase::getEntry (int entry, int column) {
  if(database.size()==0) readDB();
  return database[entry].at(column);
}
 
int TGCDatabase::getEntrySize (int entry) {
  if(database.size()==0) readDB();
  return database[entry].size();
}
 
int TGCDatabase::getMaxEntry (void) {
  if(database.size()==0) readDB();
  return database.size();
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
