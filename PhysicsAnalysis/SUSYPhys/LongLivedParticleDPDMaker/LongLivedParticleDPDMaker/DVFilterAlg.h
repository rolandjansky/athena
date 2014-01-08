/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DVFILTERALG_H
#define DVFILTERALG_H

/** 
 Class definition for DV filter algorithm
author: Nick Barlow
 **/

#include <string>
#include <vector>
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Reco {
  class ITrackToVertex;
}


class DVFilterAlg : public AthFilterAlgorithm {
public: // Constructor and Destructor

   DVFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~DVFilterAlg();

public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:

   int m_nProcessed;
   int m_nPassMuonCut;

   ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
   bool m_passAll;
   std::string m_primVtxName;
   std::string m_muonCollName;
   float m_muonPtCut;
   float m_muonEtaCut;
   float m_muonD0Cut;

};


#endif
