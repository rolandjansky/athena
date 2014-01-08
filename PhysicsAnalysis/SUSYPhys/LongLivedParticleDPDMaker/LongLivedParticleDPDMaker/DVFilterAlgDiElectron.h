/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DVFILTERALGDIELECTRON_H
#define DVFILTERALGDIELECTRON_H

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


class DVFilterAlgDiElectron : public AthFilterAlgorithm {
public: // Constructor and Destructor

   DVFilterAlgDiElectron(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~DVFilterAlgDiElectron();

public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:

   int m_nProcessed;
   int m_nPassElectronCut;
   int m_diElectronCounter;

   ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
   bool m_passAll;
   std::string m_primVtxName;
   std::string m_electronCollName;
   float m_dielectronPtCut;
   float m_dielectronEtaCut;
   float m_dielectronD0Cut;


};


#endif
