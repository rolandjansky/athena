/*
  Filter algorithms for muon trigger
  
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_MUONFILTERALG_H
#define TRIGMUONEF_MUONFILTERALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"

class MuonFilterAlg : public AthReentrantAlgorithm
{
  public :

    /** Constructor **/
    MuonFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  
    /** initialize */
    StatusCode initialize();
  
    /** finalize */
    StatusCode finalize();
  
    /** execute the filter alg */
    StatusCode execute(const EventContext& ctx) const;


  private :

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey{this,"MuonContainerLocation", "Muons", "Muon Container"};

};

#endif
