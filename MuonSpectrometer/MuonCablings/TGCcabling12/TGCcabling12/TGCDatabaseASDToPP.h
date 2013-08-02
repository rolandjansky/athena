/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASEASDTOPP_HH
#define TGCDATABASEASDTOPP_HH

#include "TGCcabling12/TGCDatabase.h"

namespace LVL1TGCCabling12
{
 
class TGCDatabaseASDToPP : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabaseASDToPP (std::string filename, std::string blockname, bool v_isCommon=true);

  TGCDatabaseASDToPP (const TGCDatabaseASDToPP&, bool v_isCommon=true);

  virtual ~TGCDatabaseASDToPP (void);

  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;

  virtual int getIndexDBIn(int* indexIn);
  virtual void getindexDBVectorIn(std::vector<int>& tmpindexDBIn) const;
  virtual void getNIndexDBIn(int& tmpNIndexDBIn) const; 
  virtual void getmaxIndexIn(int* tmpmaxIndexIn) const; 
  virtual void getminIndexIn(int* tmpminIndexIn) const; 

  virtual int getIndexDBOut(int* indexOut);
  virtual void getindexDBVectorOut(std::vector<int>& tmpindexDBOut) const;
  virtual void getNIndexDBOut(int& tmpNIndexDBOut) const; 
  virtual void getmaxIndexOut(int* tmpmaxIndexOut) const; 
  virtual void getminIndexOut(int* tmpminIndexOut) const; 

  enum DATABASESIZE {DATABASESIZE=8};  
  enum INDEXIN {NIndexIn=3};   
  static const int IndexIn[NIndexIn];  
  static const int ReverseIndexIn[DATABASESIZE];  
  enum INDEXOUT {NIndexOut=3};  
  static const int IndexOut[NIndexOut];  
  static const int ReverseIndexOut[DATABASESIZE];

  bool isCommon() const;
  
private:
  virtual void readDB (void);
  TGCDatabaseASDToPP (void) {}

  virtual void makeIndexDBIn(void);
  virtual int convertIndexDBIn(int* indexIn) const;

  virtual void makeIndexDBOut(void);
  virtual int convertIndexDBOut(int* indexOut) const;

  std::vector<int> indexDBIn;
  int NIndexDBIn;
  int maxIndexIn[NIndexIn];
  int minIndexIn[NIndexIn];
  
  std::vector<int> indexDBOut;
  int NIndexDBOut;
  int maxIndexOut[NIndexOut];
  int minIndexOut[NIndexOut];
  
  // This bool variable is used to distinguish between common and sector specific databases.  
  bool m_isCommon;
};
  
} // end of namespace
 
#endif
