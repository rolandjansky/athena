/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_DIGITIZATIONALG_H
#define PILEUPCOMPS_DIGITIZATIONALG_H
/** @file DigitizationAlg.h
 * @brief an algorithm to run digitization in the original 'algorithm'
 * approach using the common interface of IPileUpTool
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/////////////////////////////////////////////////////////////////////////////

class DigitizationAlg: public AthAlgorithm {
public:
  DigitizationAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandleArray<IPileUpTool> m_puTools;
};


#endif
