/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASE_HH
#define TGCDATABASE_HH

#include <string>
#include <vector>

namespace LVL1TGCCabling8
{
 
class TGCDatabase
{
public:
  enum DatabaseType {NoDatabaseType=-1,
		     ASDToPP,InPP,PPToSL,SLBToROD,
		     MaxDatabaseType};
  
  // Constructor & Destructor
  TGCDatabase (DatabaseType type=NoDatabaseType);

  TGCDatabase (DatabaseType type,
	       std::string filename, 
	       std::string blockname);

  virtual ~TGCDatabase (void);
  
  DatabaseType getDatabaseType (void) const { return m_type; }
  

  virtual int getEntry (int entry, int column);
 
  virtual int getEntrySize (int entry);
 
  virtual int getMaxEntry (void);

  virtual int getIndexDBIn(int* indexIn);
  virtual int getIndexDBOut(int* indexOut);
  
protected:
  virtual void readDB (void) {}
  std::string m_filename;
  std::string m_blockname;
  std::vector< std::vector<int> > m_database;

private:
  DatabaseType m_type;
  
};
  
} // end of namespace
 
#endif
