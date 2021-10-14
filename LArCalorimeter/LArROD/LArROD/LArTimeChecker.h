/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArTimeChecker
 * @author Mohamed Aharrouche
 * @brief CTB time checker
 * 
 */

#ifndef LARROD_LARTIMECHECKER
#define LARROD_LARTIMECHECKER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawUtils/LArRawOrdering.h"

class LArTimeChecker : public AthAlgorithm
{

public:

  LArTimeChecker (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(){return StatusCode::SUCCESS;}

private:
  std::string m_DataLocation;
  short m_AdcCut;
};

#endif
