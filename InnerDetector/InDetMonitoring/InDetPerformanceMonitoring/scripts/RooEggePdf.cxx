
 /***************************************************************************** 
  * Project: RooFit                                                           * 
  *                                                                           * 
  * Copyright (c) 2000-2005, Regents of the University of California          * 
  *                          and Stanford University. All rights reserved.    * 
  *                                                                           * 
  * Redistribution and use in source and binary forms,                        * 
  * with or without modification, are permitted according to the terms        * 
  * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             * 
  *****************************************************************************/ 

#ifndef ROOEGGEPDF_C
#define ROOEGGEPDF_C

 // -- CLASS DESCRIPTION [PDF] -- 
 // Your description goes here... 

#include "Riostream.h" 

#include "RooEggePdf.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 

//ClassImp(RooEggePdf) // --> this line commented as we link within root and not with make (Salva 4/February/2016)

RooEggePdf::RooEggePdf(const char *name, const char *title, 
		       RooAbsReal& _x,
		       RooAbsReal& _A,
		       RooAbsReal& _B,
		       RooAbsReal& _C,
		       RooAbsReal& _mean,
		       RooAbsReal& _width) :
RooAbsPdf(name,title), 
  x("x","x",this,_x),
  A("A","A",this,_A),
  B("B","B",this,_B),
  C("C","C",this,_C),
  mean("mean","mean",this,_mean),
  width("width","width",this,_width)
{ 
} 


RooEggePdf::RooEggePdf(const RooEggePdf& other, const char* name) :  
  RooAbsPdf(other,name), 
  x("x",this,other.x),
  A("A",this,other.A),
  B("B",this,other.B),
  C("C",this,other.C),
  mean("mean",this,other.mean),
  width("width",this,other.width)
{ 
} 



Double_t RooEggePdf::evaluate() const 
{ 
  // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE 
  if (x<0) return 0 ;
  
  Double_t ret(0) ;
  //  ret += 0.0001;
  ret += A/(x*x+1) ;
  ret += B*(x*x-mean*mean)/((x*x-mean*mean)*(x*x-mean*mean)+width*width*mean*mean) ;
  ret += C*x*x/((x*x-mean*mean)*(x*x-mean*mean)+width*width*mean*mean) ;
  

//   if(ret<0) return 0.00001;
  return ret ;
} 
#endif


