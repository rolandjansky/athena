/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOCALLOCALHADCALIB_CALOREADLCWEIGHTSFILE
#define CALOCALLOCALHADCALIB_CALOREADLCWEIGHTSFILE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include <string>


class CaloReadLCWeightsFile : public AthAlgorithm
{
 public:
  CaloReadLCWeightsFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloReadLCWeightsFile();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();
  StatusCode initDataFromFile(const std::string& theLCWeightFileName,
                              CaloLocalHadCoeff& data);

 private:
  std::string m_key;
  std::string m_LCWeightFileName;
};

#endif
