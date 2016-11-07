///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAthExample.h:
// xExample is a package to test the xAOD and CP tools by histogramming
// variables for each type of object. Some plots are specific to ttbar.
// Author: Will Buttinger - conversion of code from Frank Paige <paige@bnl.gov>
// Thanks: David Adams, Joe Taenzer, Iacopo Vivarelli
///////////////////////////////////////////////////////////////////

#ifndef XEXAMPLE_XATHEXAMPLE_H
#define XEXAMPLE_XATHEXAMPLE_H


#ifndef ROOTCORE

#include "AthenaBaseComps/AthAlgorithm.h"

// Root includes
#include <TH1.h>
#include <TH2.h>


class xAthExample : public AthAlgorithm
{
public:
  
  std::map<std::string,TH1*> m_TH1;		
  std::map<std::string,TH2*> m_TH2;		
  std::string ptStr[17];		
  float ptLim[17];			

  int m_eventCounter;
  int m_warnCounter; 
  int m_warnLimit;

   long m_startTime;

  // this is a standard constructor
  xAthExample(const std::string& name, ISvcLocator* pSvcLocator);

  // these are the functions inherited from Algorithm
  virtual StatusCode initialize ();
  virtual StatusCode execute ();
  virtual StatusCode finalize ();

};

#endif //ROOTCORE

#endif //XEXAMPLE_XATHEXAMPLE_H

