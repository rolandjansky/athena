/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASEASDTOPP_HH
#define TGCDATABASEASDTOPP_HH

#include "TGCcabling/TGCDatabase.h"

namespace LVL1TGCCabling8
{
 
class TGCDatabaseASDToPP : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabaseASDToPP (std::string filename, std::string blockname)
    : TGCDatabase(TGCDatabase::ASDToPP, filename, blockname), 
    m_NIndexDBIn(0), m_NIndexDBOut(0) 
    {
      for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) { 
	m_maxIndexIn[iIndexIn] = 0; 
	m_minIndexIn[iIndexIn] = 9999; 
      } 
      for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) { 
	m_maxIndexOut[iIndexOut] = 0; 
	m_minIndexOut[iIndexOut] = 9999; 
      } 
    }

  virtual ~TGCDatabaseASDToPP (void){}

  enum DATABASESIZE {DATABASESIZE=8};

  virtual int getIndexDBIn(int* indexIn);
  enum INDEXIN {NIndexIn=3}; 
  static const int s_IndexIn[NIndexIn];
  static const int s_ReverseIndexIn[DATABASESIZE];

  virtual int getIndexDBOut(int* indexOut);
  enum INDEXOUT {NIndexOut=3};
  static const int s_IndexOut[NIndexOut];
  static const int s_ReverseIndexOut[DATABASESIZE];
  
private:
  virtual void readDB (void);
  virtual void makeIndexDBIn(void);
  virtual int convertIndexDBIn(int* indexIn) const;
  virtual void makeIndexDBOut(void);
  virtual int convertIndexDBOut(int* indexOut) const;
  TGCDatabaseASDToPP (void) {}

  std::vector<int> m_indexDBIn;
  int m_NIndexDBIn;
  int m_maxIndexIn[NIndexIn];
  int m_minIndexIn[NIndexIn];
  std::vector<int> m_indexDBOut;
  int m_NIndexDBOut;
  int m_maxIndexOut[NIndexOut];
  int m_minIndexOut[NIndexOut];
};
  
} // end of namespace
 
#endif
