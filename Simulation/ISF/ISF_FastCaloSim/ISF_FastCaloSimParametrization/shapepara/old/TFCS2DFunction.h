/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS2DFunction_h
#define TFCS2DFunction_h


#include "TTree.h"
#include "TH2.h"

class TFCS2DFunction:public TObject
{
  public:
    TFCS2DFunction();
    TFCS2DFunction(std::string);
    
    void Initialize(std::string);
    int fitShape(std::string, std::string, std::string, std::string, std::string, int,float,bool);
    
    void tmvaregression_training(int,std::string, std::string, std::string, int,bool);
    std::vector<float> tmvaregression_application(Float_t, Float_t, std::string, int);
    void plotpolar(TH2F*, TH2F*, std::string, std::string, std::string,int,float,bool);


  private:
 
 ClassDef(TFCS2DFunction,1)  //TFCS2DFunction
};

#endif
