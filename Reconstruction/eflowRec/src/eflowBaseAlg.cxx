/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowBaseAlg.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  10th November, 2001

********************************************************************/

#include "eflowRec/eflowBaseAlg.h"

eflowBaseAlg::eflowBaseAlg(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator)
{  }


eflowBaseAlg::~eflowBaseAlg() 
{ }


StatusCode eflowBaseAlg::initialize() 
{  

  return StatusCode::SUCCESS;

}


StatusCode eflowBaseAlg::execute()
{

  return StatusCode::SUCCESS;

}

StatusCode eflowBaseAlg::finalize()
{
  
  return StatusCode::SUCCESS;

}
