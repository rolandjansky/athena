/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LArShapeCorrector_H
#define LARCALIBUTILS_LArShapeCorrector_H

#include "AthenaBaseComps/AthAlgorithm.h"

class LArShapeCorrector : public AthAlgorithm
{

 public:

  LArShapeCorrector(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArShapeCorrector();

  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:

  //Job properties
  std::string m_keyShape;
  std::string m_keyShape_newcorr;
  std::string m_keyShapeResidual;

  std::string m_groupingName;
  int m_groupingType;
  int m_phase;
};
#endif
