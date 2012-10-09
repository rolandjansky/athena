/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Overlay_G4HitMerger_H
#define Overlay_G4HitMerger_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include <vector>
#include <iostream>
#include "GeneratorObjects/McEventCollection.h"

class StoreGateSvc;
class PileUpMergeSvc;

using namespace std;

class G4HitMerger : public Algorithm
{
 public:
  G4HitMerger(const string& name, ISvcLocator* pSvcLocator);
  ~G4HitMerger();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  ServiceHandle<StoreGateSvc> m_storeGateSvc; 
  StoreGateSvc* detStore;
  PileUpMergeSvc *p_mergeSvc;
  
  std::vector <std::string> m_HitContainer; // hit container name list

  std::string m_Detectors;

  friend std::ostream& operator<<(std::ostream& o, const McEventCollection& m);
 
};

#endif
