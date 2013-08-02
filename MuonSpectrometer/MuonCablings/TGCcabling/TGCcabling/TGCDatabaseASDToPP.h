/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    NIndexDBIn(0), NIndexDBOut(0) 
    {
      for(int iIndexIn=0; iIndexIn<NIndexIn; iIndexIn++) { 
	maxIndexIn[iIndexIn] = 0; 
	minIndexIn[iIndexIn] = 9999; 
      } 
      for(int iIndexOut=0; iIndexOut<NIndexOut; iIndexOut++) { 
	maxIndexOut[iIndexOut] = 0; 
	minIndexOut[iIndexOut] = 9999; 
      } 
    }

  virtual ~TGCDatabaseASDToPP (void){}

  enum DATABASESIZE {DATABASESIZE=8};

  virtual int getIndexDBIn(int* indexIn);
  enum INDEXIN {NIndexIn=3}; 
  static const int IndexIn[NIndexIn];
  static const int ReverseIndexIn[DATABASESIZE];

  virtual int getIndexDBOut(int* indexOut);
  enum INDEXOUT {NIndexOut=3};
  static const int IndexOut[NIndexOut];
  static const int ReverseIndexOut[DATABASESIZE];
  
private:
  virtual void readDB (void);
  virtual void makeIndexDBIn(void);
  virtual int convertIndexDBIn(int* indexIn) const;
  virtual void makeIndexDBOut(void);
  virtual int convertIndexDBOut(int* indexOut) const;
  TGCDatabaseASDToPP (void) {}

  std::vector<int> indexDBIn;
  int NIndexDBIn;
  int maxIndexIn[NIndexIn];
  int minIndexIn[NIndexIn];
  std::vector<int> indexDBOut;
  int NIndexDBOut;
  int maxIndexOut[NIndexOut];
  int minIndexOut[NIndexOut];
};
  
} // end of namespace
 
#endif
