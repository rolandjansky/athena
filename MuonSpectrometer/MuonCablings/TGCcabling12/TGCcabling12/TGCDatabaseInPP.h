/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASEINPP_HH
#define TGCDATABASEINPP_HH

#include "TGCcabling12/TGCDatabase.h"

namespace LVL1TGCCabling12
{
 
class TGCDatabaseInPP : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabaseInPP (std::string filename, std::string blockname);
 
  TGCDatabaseInPP (const TGCDatabaseInPP&);
 
  virtual ~TGCDatabaseInPP (void);

  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;

  virtual int getIndexDBIn(int* indexIn);

  virtual void getindexDBVectorIn(std::vector<int>& tmpindexDBIn) const;
  virtual void getNIndexDBIn(int& tmpNIndexDBIn) const;
  virtual void getmaxIndexIn(int* tmpmaxIndexIn) const;
  virtual void getminIndexIn(int* tmpminIndexIn) const;

  enum {NIndexIn=3, IndexInMin=0, IndexInMax=2};

private:
  virtual void readDB (void);
  TGCDatabaseInPP (void) {}

  virtual void makeIndexDBIn(void);
  virtual int convertIndexDBIn(int* indexIn) const;

  std::vector<int> indexDBIn;
  int NIndexDBIn;
  int maxIndexIn[NIndexIn];
  int minIndexIn[NIndexIn];
};
  
} // end of namespace
 
#endif
