/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCEVENTCOLLECTIONFILTER_H
#define MCEVENTCOLLECTIONFILTER_H
 
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
//
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"
//
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEvent/CSCSimHit.h"

#include <vector>
#include <iostream>
 
class StoreGateSvc;
class McEventCollectionFilter : public AthAlgorithm {
//
//
public:
   McEventCollectionFilter(const std::string& name, ISvcLocator* pSvcLocator);
   ~McEventCollectionFilter();
   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();
 
  private:

   StatusCode ReduceMCEvenetCollection();
   StatusCode FindTRTElectronHits();
   StatusCode SiHistsTruthRelink();
   StatusCode TRTHistsTruthRelink();
   StatusCode MDTHistsTruthRelink();
   StatusCode CSCHistsTruthRelink();
   StatusCode RPCHistsTruthRelink();
   StatusCode TGCHistsTruthRelink();

   bool m_IsKeepTRTElect;
   std::string   m_mcEventCollection;
   int m_PileupPartPDGID;
   bool m_UseTRTHits;
   //---------------------
   std::string   m_HitName;
   int m_RefBarcode;
   std::vector<int> m_elecBarcode;


 
}; 
#endif
