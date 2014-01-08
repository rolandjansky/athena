/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DVFILTERALGEGAMMA_H
#define DVFILTERALGEGAMMA_H

/** 
 Class definition for DV filter algorithm
 **/

#include <string>
#include <vector>
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Reco {
  class ITrackToVertex;
}


class DVFilterAlgEgamma : public AthFilterAlgorithm {
public: // Constructor and Destructor

   DVFilterAlgEgamma(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~DVFilterAlgEgamma();

public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:

   int m_nProcessed;
   int m_nPassElectronCut;

   ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
   bool m_passAll;
   std::string m_primVtxName;
   std::string m_electronCollName;
   float m_electronPtCut;
   float m_electronEtaCut;
   float m_electronD0Cut;

};


#endif
