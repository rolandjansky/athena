/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCDATABASE_HH
#define MUONTGC_CABLING_TGCDATABASE_HH

#include <string>
#include <vector>

namespace MuonTGC_Cabling
{
 
class TGCDatabase
{
public:
  enum DatabaseType {NoDatabaseType=-1, 
		     ASDToPP, InPP, PPToSL, SLBToROD, 
		     MaxDatabaseType};
  
  // Constructor & Destructor
  TGCDatabase(DatabaseType type=NoDatabaseType);
  
  TGCDatabase(DatabaseType type,
	      std::string filename, 
	      std::string blockname);

  TGCDatabase(const TGCDatabase &); 

  virtual ~TGCDatabase(void);
  
  // assignment operator 
  TGCDatabase& operator=(const TGCDatabase&);

  DatabaseType getDatabaseType(void) const { return m_type; }
  
  void clear();

  virtual int getEntry(int entry, int column);
 
  virtual int getEntrySize(int entry);
 
  virtual int getMaxEntry(void);
 
  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;

  virtual int getIndexDBIn(int* indexIn);
  virtual int getIndexDBOut(int* indexOut);
 
protected:
  virtual void readDB(void) {}
  std::string m_filename;
  std::string m_blockname;
  std::vector< std::vector<int> > m_database;

private:
  DatabaseType m_type;
  
};
  
} // end of namespace
 
#endif
