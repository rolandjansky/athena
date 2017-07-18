/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FitDetailedShape_h
#define FitDetailedShape_h

#include "ISF_FastCaloSimParametrization/TFCS2DFunction.h"
#include "TTree.h"

class FitDetailedShape{

  public:
  	
    FitDetailedShape();
    virtual ~FitDetailedShape() {}
    void run(std::string,int,std::string,std::string,int,float,bool);
	  
  private:
    
  ClassDef(FitDetailedShape,2);
  
};

#endif
