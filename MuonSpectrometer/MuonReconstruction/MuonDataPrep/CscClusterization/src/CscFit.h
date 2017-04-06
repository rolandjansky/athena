/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscFit_H
#define CscFit_H

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TROOT.h"

/////////////////////////////////////////////////////////////////////////////
// a class to fit CSC strips in a plane into clusters                      //
//                                                                         //
// BNL March 26 2003  --- Ketevi A. Assamagan                              //
/////////////////////////////////////////////////////////////////////////////

Double_t f1gauss(Double_t *x, Double_t *par);
Double_t f2gauss(Double_t *x, Double_t *par);

class CscFit {

 public:

// Constructor

 CscFit(double sigma);

 // Fitter
 void cscfit(double* qstr, int& maxStrip, double& thr, double& da, 
	     int& ncl, double* sig, double* zpos, double& error);

 private:
 
 int  icmax(double* qstr, const int& i1, const int& i2);
 friend Double_t f1gauss(Double_t *x, Double_t *par);
 friend Double_t f2gauss(Double_t *x, Double_t *par);

 private:

 double m_sigma;
 IMessageSvc* m_msgSvc;

};

#endif







