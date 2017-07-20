/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOLOCALHADCALIB_CALOREADLCJETENERGYSCALEFILE
#define CALOLOCALHADCALIB_CALOREADLCJETENERGYSCALEFILE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include <vector>
#include <string>


class CaloReadLCJetEnergyScaleFile : public AthAlgorithm
{
 public:
  CaloReadLCJetEnergyScaleFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloReadLCJetEnergyScaleFile();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();
  StatusCode initDataFromFile(std::vector<std::string> &theLCJetEnergyScaleFileNames, std::vector<std::string> &theLCJetEnergyScaleJetCollectionNames, CaloLocalHadCoeff& data);

 private:
  std::string m_key;
  std::vector<std::string> m_LCJetEnergyScaleFileNames;
  std::vector<std::string> m_LCJetEnergyScaleJetCollectionNames;
};

#endif
