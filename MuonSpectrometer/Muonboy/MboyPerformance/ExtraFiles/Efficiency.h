/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Efficiency_h
#define Efficiency_h
#include "Analysis.h"
#include "AANTMisc.h"

class Efficiency : public Analysis  {
public: 
  Efficiency();
  virtual ~Efficiency(); 

public:
///////////////////////////////////
    virtual void initialize(char* option);
    virtual void execute();
    virtual void finalize();

private:
///////////////////////////////////
// Methods
   void beginEfficiencies();
   void efficiencies();
   void endEfficiencies();
   int  getEtaBin(float eta);
   int  getEtaZone(float eta);

// Data
   bool m_Debug;

   int  m_nevt ;

};
#endif // 
