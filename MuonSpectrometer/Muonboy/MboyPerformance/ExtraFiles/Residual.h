/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Residual_h
#define Residual_h
#include "Analysis.h"
#include "AANTMisc.h"

class Residual : public Analysis  {
public: 
  Residual();
  virtual ~Residual(); 

public:
///////////////////////////////////
    virtual void initialize(char* option);
    virtual void execute();
    virtual void finalize();

private:
///////////////////////////////////
// Methods
    void drawHist(TH1F * histo);
    void drawHist2(TH2F * histo);

// Data

};
#endif // 
