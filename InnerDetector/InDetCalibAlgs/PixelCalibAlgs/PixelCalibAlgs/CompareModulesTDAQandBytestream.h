/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CompareModulesTDAQandBytestream_H
#define CompareModulesTDAQandBytestream_H

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"

class IPixelByteStreamErrorsSvc;
class IPixelTDAQSvc;
class PixelID;

class CompareModulesTDAQandBytestream:  public AthAlgorithm{

 public:
  CompareModulesTDAQandBytestream(const std::string& name,
                                  ISvcLocator* pSvcLocator);
  ~CompareModulesTDAQandBytestream();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ServiceHandle< IPixelByteStreamErrorsSvc > m_bsSvc;
  ServiceHandle< IPixelTDAQSvc > m_tdaqSvc;

  std::map<unsigned int, unsigned int> runs_and_lumi_blocks;
  unsigned int n_lumi_blocks;
  std::map<unsigned int, std::vector<std::string> > disabled_modules;

  const PixelID* m_pixelID;
};
#endif
