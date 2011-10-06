/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MT2REC_H
#define STRANSVERSEMASSUTILS_MT2REC_H

//////////////////////////////////////////////////////////////////
//C S Cowden					17 August 2009
//Algorithm to calculate mT2/mTGen and setup mT2 event reconstruction
//At the moment this is just filler for later use...perhaps.
//////////////////////////////////////////////////////////////////

#include "GaudiKernel/Algorithm.h"

#include<string>

class mT2Rec : public Algorithm {

public:
  mT2Rec(const std::string& name, ISvcLocator* pSvcLocator);
  ~mT2Rec();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();




};


#endif
