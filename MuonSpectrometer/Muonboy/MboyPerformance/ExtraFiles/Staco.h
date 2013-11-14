/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Staco_h
#define Staco_h
#include "Analysis.h"
#include "AANTMisc.h"

class Staco : public Analysis  {
public: 
  Staco();
  virtual ~Staco(); 

public:
///////////////////////////////////
    virtual void initialize(char* option);
    virtual void execute();
    virtual void finalize();

private:
///////////////////////////////////
// Methods
    int getMboy(int i);
    int getInDet(int i);
    float getMatchedChi2(int i);
    void dumpSimul();
// Data
   bool m_Debug;

   int  m_nevt ;

};
#endif // 
