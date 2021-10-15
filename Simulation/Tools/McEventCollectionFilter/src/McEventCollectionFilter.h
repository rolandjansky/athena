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
   StatusCode decoratePileUpTruth();
   StatusCode FindTRTElectronHits();
   StatusCode SiHistsTruthRelink();
   StatusCode TRTHistsTruthRelink();
   StatusCode MDTHistsTruthRelink();
   StatusCode CSCHistsTruthRelink();
   StatusCode RPCHistsTruthRelink();
   StatusCode TGCHistsTruthRelink();

   bool m_IsKeepTRTElect{false};
   std::string   m_mcEventCollection{"TruthEvent"};
   int m_PileupPartPDGID{999};  //Geantino
   bool m_UseTRTHits{true};
   bool m_decoratePileUpTruth{true};
   bool m_decorateTruthParticles{true};
   bool m_decorateAntiKt4TruthJets{true};
   bool m_decorateAntiKt6TruthJets{true};
   //---------------------
   int m_RefBarcode{0};
   float m_RefPVtxZ{0.0};
   std::vector<int> m_elecBarcode;



};
#endif
