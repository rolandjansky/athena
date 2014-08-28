//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ELECTRONTOPHOTONALG_H
#define ELECTRONTOPHOTONALG_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "PhotonAnalysisUtils/IElectronToPhotonTool.h"

#include "D3PDMakerUtils/SGKeyResolver.h"

class Photon;
class Electron;
class PhotonContainer;

class ElectronToPhotonAlg : public AthAlgorithm {
  
 public:
  
  ElectronToPhotonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~ElectronToPhotonAlg();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  // public methods:
  Analysis::Photon* ConvertElectronToPhoton(const egamma* eg, bool conv, const std::string& egDetailContainerName);

private:
  ToolHandle<IElectronToPhotonTool> m_ElectronToPhotonTool;
  D3PD::SGKeyResolver* m_resolver;
  std::string m_electronContainerName;
};

#endif

