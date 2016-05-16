// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StopperAlg.h,v 1.1 2005/01/17 18:17:03 mato Exp $
#ifndef ATHENASERVICES_ATHENASTOPPERALG_H
#define ATHENASERVICES_ATHENASTOPPERALG_H


// Include files
#include "AthenaBaseComps/AthAlgorithm.h"


/**
 * @brief Algorithm to stop the run after a given number of events.
 * Derived from GaudiExamples/StopperAlg.
 */
class AthenaStopperAlg
  : public AthAlgorithm
{
public:
  /// Constructor of this form must be provided
  AthenaStopperAlg(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
private:
  int  m_stopcount;
};

#endif    // ATHENASERVICES_ATHENASTOPPERALG_H
