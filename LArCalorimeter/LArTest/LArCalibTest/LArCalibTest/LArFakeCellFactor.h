/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCALIBTEST_LARFAKECELLFACTOR
#define LARCALIBTEST_LARFAKECELLFACTOR

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>


class LArFakeCellFactor : public AthAlgorithm
{
 public:
  LArFakeCellFactor(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArFakeCellFactor();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();

 private:
  std::string m_key;
  std::string m_fileName;
  struct FileEntry {
    float eta;
    float phi;
    float value;
  };

};

#endif
