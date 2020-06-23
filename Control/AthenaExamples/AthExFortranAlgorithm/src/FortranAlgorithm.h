// dear emacs, this is really -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXFORTRANALGORITHM_FORTRANALGORITHM_H
#define ATHEXFORTRANALGORITHM_FORTRANALGORITHM_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

/////////////////////////////////////////////////////////////////////////////

class FortranAlgorithm:public AthAlgorithm {
 public:
  FortranAlgorithm (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  int m_lun;
  std::string m_fileName;
};

#endif // !ATHEXFORTRANALGORITHM_FORTRANALGORITHM_H
