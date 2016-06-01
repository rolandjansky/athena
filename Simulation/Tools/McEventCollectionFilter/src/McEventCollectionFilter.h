/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
