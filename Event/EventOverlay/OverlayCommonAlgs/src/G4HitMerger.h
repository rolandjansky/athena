/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOMMONALGS_G4HitMerger_H
#define OVERLAYCOMMONALGS_G4HitMerger_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "GeneratorObjects/McEventCollection.h"

#include <vector>
#include <iostream>

class PileUpMergeSvc;

class G4HitMerger : public AthAlgorithm
{
 public:
  G4HitMerger(const std::string& name, ISvcLocator* pSvcLocator);
  ~G4HitMerger();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  ServiceHandle<PileUpMergeSvc> m_mergeSvc;

  std::vector <std::string> m_HitContainer; // hit container name list

  std::string m_Detectors;

  friend std::ostream& operator<<(std::ostream& o, const McEventCollection& m);

};

#endif //OVERLAYCOMMONALGS_G4HitMerger_H
