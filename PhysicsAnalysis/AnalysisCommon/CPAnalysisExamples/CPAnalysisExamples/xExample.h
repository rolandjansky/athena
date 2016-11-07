///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xExample.h:
// xExample is a package to test the xAOD and CP tools by histogramming
// variables for each type of object. Some plots are specific to ttbar.
// Author: Frank Paige <paige@bnl.gov>
// Thanks: David Adams, Joe Taenzer, Iacopo Vivarelli
///////////////////////////////////////////////////////////////////

#ifndef XEXAMPLE_XEXAMPLE_H
#define XEXAMPLE_XEXAMPLE_H


#ifdef ROOTCORE
#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

// Root includes
#include <TH1.h>
#include <TH2.h>
#endif // ROOTCORE

// STL includes
#include <string>
#include <vector>
#include <map>
#include <fstream>


class xExample : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // float cutValue;



  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
public:
  // Tree *myTree; //!
  // TH1 *myHist; //!

  xAOD::TEvent *m_event;			//!

  // Variables should be protected from CINT using //!
  // Using map(string,TH*) simplifies booking multiple histograms

  std::map<std::string,TH1*> m_TH1;		//!
  std::map<std::string,TH2*> m_TH2;		//!
  std::string ptStr[17];			//!
  float ptLim[17];				//!

  int m_eventCounter; //!
  int m_warnCounter; //!
  int m_warnLimit; //!

  long m_startTime; //!

  // this is a standard constructor
  xExample ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(xExample, 1);
};

#endif //XEXAMPLE_XEXAMPLE_H

