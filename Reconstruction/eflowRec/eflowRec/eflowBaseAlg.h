/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWBASEALG_H
#define EFLOWREC_EFLOWBASEALG_H
/********************************************************************

NAME:     eflowBaseAlg.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R.Tovey
CREATED:  10th November, 2001

********************************************************************/

// INCLUDE HEADER FILES:

#include "AthenaBaseComps/AthAlgorithm.h"

class eflowBaseAlg : public AthAlgorithm
{

 public:

  eflowBaseAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~eflowBaseAlg();


  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

};

#endif
