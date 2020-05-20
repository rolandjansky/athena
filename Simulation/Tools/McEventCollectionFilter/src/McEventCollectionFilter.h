/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCEVENTCOLLECTIONFILTER_H
#define MCEVENTCOLLECTIONFILTER_H

// Base class include 
#include "AthenaBaseComps/AthAlgorithm.h"

// std library includes
#include <string>
#include <vector>

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

   StatusCode ReduceMCEventCollection();
   StatusCode FindTRTElectronHits();
   StatusCode SiHitsTruthRelink();
   StatusCode TRTHitsTruthRelink();
   StatusCode MDTHitsTruthRelink();
   StatusCode CSCHitsTruthRelink();
   StatusCode RPCHitsTruthRelink();
   StatusCode TGCHitsTruthRelink();
   StatusCode STGC_HitsTruthRelink();
   StatusCode MM_HitsTruthRelink();

   bool m_IsKeepTRTElect;
   std::string   m_mcEventCollection;
   int m_PileupPartPDGID;
   bool m_UseTRTHits;
   bool m_UseCSCHits;
   bool m_UseSTGCHits;
   bool m_UseMMHits;
   //---------------------
   std::string   m_HitName;
   int m_RefBarcode;
   std::vector<int> m_elecBarcode;


 
}; 
#endif
