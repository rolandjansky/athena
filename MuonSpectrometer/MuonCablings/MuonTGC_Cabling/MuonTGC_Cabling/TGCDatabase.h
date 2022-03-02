/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	      const std::string& filename, 
	      const std::string& blockname);

  TGCDatabase(const TGCDatabase &) = default;

  virtual ~TGCDatabase(void);
  
  // assignment operator 
  TGCDatabase& operator=(const TGCDatabase&) = default;

  DatabaseType getDatabaseType(void) const { return m_type; }
  
  virtual int getEntry(int entry, int column) const;
 
  virtual int getEntrySize(int entry) const;
 
  virtual int getMaxEntry(void) const;
 
  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;

  virtual int getIndexDBIn(int* indexIn) const;
  virtual int getIndexDBOut(int* indexOut) const;
 
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
