/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Copyright (c) 2000-2007, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROOEGGEPDF_H
#define ROOEGGEPDF_H

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
 
class RooEggePdf : public RooAbsPdf {
 public:
  RooEggePdf() {} ; 
  RooEggePdf(const char *name, const char *title,
	     RooAbsReal& _x,
	     RooAbsReal& _A,
	     RooAbsReal& _B,
	     RooAbsReal& _C,
	     RooAbsReal& _mean,
	     RooAbsReal& _width);
  RooEggePdf(const RooEggePdf& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new RooEggePdf(*this,newname); }
  inline virtual ~RooEggePdf() { }
  
 protected:
  
  RooRealProxy x ;
  RooRealProxy A ;
  RooRealProxy B ;
  RooRealProxy C ;
  RooRealProxy mean ;
  RooRealProxy width ;
  
  Double_t evaluate() const ;
  
 private:
  
  // ClassDef(RooEggePdf,1) // --> this line commented as we link within root and not with make (Salva 4/February/2016)

    };

#endif
