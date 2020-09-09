/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOLOCALHADCALIB_CALOREADLCCLASSIFICATIONFILE
#define CALOLOCALHADCALIB_CALOREADLCCLASSIFICATIONFILE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include <string>


class CaloReadLCClassificationFile : public AthAlgorithm
{
 public:
  CaloReadLCClassificationFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloReadLCClassificationFile();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();
  StatusCode initDataFromFile(const std::string& theLCClassificationFileName,
                              CaloLocalHadCoeff& data);

 private:
  std::string m_key;
  std::string m_LCClassificationFileName;
};

#endif
