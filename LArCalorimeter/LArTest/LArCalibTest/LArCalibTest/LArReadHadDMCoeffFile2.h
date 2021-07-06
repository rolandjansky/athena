/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBTEST_LARREADHADDMCOEFFFILE2
#define LARCALIBTEST_LARREADHADDMCOEFFFILE2

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloHadDMCoeff2.h"

#include <vector>
#include <string>


class LArReadHadDMCoeffFile2 : public AthAlgorithm
{
  public:
    LArReadHadDMCoeffFile2(const std::string & name, ISvcLocator * pSvcLocator);
    ~LArReadHadDMCoeffFile2();

    StatusCode initialize(); 
    StatusCode execute();
    StatusCode finalize();
    StatusCode initDataFromFile(const std::string& hadDMCoeffFileName);
    int parse_dim(std::string &sLine, CaloHadDMCoeff2::HadDMDimension &dim);

  private:
    std::string m_key;
    std::string m_hadDMCoeffFileName;

    CaloHadDMCoeff2* m_data;
};

#endif
