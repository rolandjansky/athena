/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBTEST_LARREADHADDMCOEFFFILE
#define LARCALIBTEST_LARREADHADDMCOEFFFILE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloHadDMCoeff.h"

#include <vector>
#include <string>


class LArReadHadDMCoeffFile : public AthAlgorithm
{
 public:
  LArReadHadDMCoeffFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArReadHadDMCoeffFile();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();
  StatusCode initDataFromFile(std::string hadDMCoeffFileName);

 private:
  std::string m_key;
  std::string m_hadDMCoeffFileName;

  CaloHadDMCoeff* m_data;

};

#endif
