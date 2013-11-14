/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Modele_h
#define Modele_h
#include "Analysis.h"
#include "AANTMisc.h"

class Modele : public Analysis  {
public: 
  Modele();
  virtual ~Modele(); 

public:
///////////////////////////////////
    virtual void initialize(char* option);
    virtual void execute();
    virtual void finalize();

private:
///////////////////////////////////
// Methods

// Data
   bool m_Debug;

   int  m_nevt ;

};
#endif // 
