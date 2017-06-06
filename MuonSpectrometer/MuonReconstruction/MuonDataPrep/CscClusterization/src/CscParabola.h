/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscParabola_H
#define CscParabola_H

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

/////////////////////////////////////////////////////////////////////////////
// a class to fit CSC strips in a plane into clusters                      // 
//                                                                         //
// BNL March 26 2003  --- Ketevi A. Assamagan                              //
/////////////////////////////////////////////////////////////////////////////


class CscParabola {

 public:

// Constructor

 CscParabola();

 // Fitters
 void cscParabola(double* qstr, int& NStrip, double& thr, double& da, 
	          int& ncl, double* sig, double* zpos, double& noise);

 private:

  // Gaudi message service

  IMessageSvc* m_msgSvc;
  
};

#endif







