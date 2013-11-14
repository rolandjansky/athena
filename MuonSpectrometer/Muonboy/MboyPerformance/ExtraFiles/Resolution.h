/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Resolution_h
#define Resolution_h
#include "Analysis.h"
#include "AANTMisc.h"

class Resolution : public Analysis {
public: 
  Resolution();
  virtual ~Resolution( ); 

public:
///////////////////////////////////
    virtual void initialize(char* option);
    virtual void execute();
    virtual void finalize();

private:
///////////////////////////////////
// Methods
   void fitHist(TH1F *histo, FILE *File);
   int  getEtaZone(float eta);

// Data
   bool m_Debug;

   int  m_nevt ;

};
#endif // 
