/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASEINPP_HH
#define TGCDATABASEINPP_HH

#include "TGCcabling/TGCDatabase.h"

namespace LVL1TGCCabling8
{
 
class TGCDatabaseInPP : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabaseInPP (std::string filename, std::string blockname)
    : TGCDatabase(TGCDatabase::InPP, filename, blockname), 
    m_NIndexDBIn(0) 
  {
    for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) { 
      m_maxIndexIn[iIndexIn] = 0; 
      m_minIndexIn[iIndexIn] = 9999; 
    } 
  }

  virtual ~TGCDatabaseInPP (void){}

  virtual int getIndexDBIn(int* indexIn);
  enum {NIndexIn=3, IndexInMin=0, IndexInMax=2};
  
private:
  virtual void readDB (void);
  virtual void makeIndexDBIn(void);
  virtual int convertIndexDBIn(int* indexIn) const;
  TGCDatabaseInPP (void) {}

  std::vector<int> m_indexDBIn;
  int m_NIndexDBIn;
  int m_maxIndexIn[NIndexIn];
  int m_minIndexIn[NIndexIn];
};
  
} // end of namespace
 
#endif
