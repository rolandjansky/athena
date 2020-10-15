/*
  Merge inside-out and outside-in muon trigger containers
  
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_MERGEEFMUONSALG_H
#define TRIGMUONEF_MERGEEFMUONSALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"

class MergeEFMuonsAlg : public AthReentrantAlgorithm
{
  public :

    /** Constructor **/
    MergeEFMuonsAlg( const std::string& name, ISvcLocator* pSvcLocator );
  
    /** initialize */
    StatusCode initialize();
  
    /** finalize */
    StatusCode finalize();
  
    /** execute the filter alg */
    StatusCode execute(const EventContext& ctx) const;


  private :

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonCBContainerKey{this,"MuonCBContainerLocation", "MuonsCB", "CB Muon Container"};
    SG::ReadHandleKey<xAOD::MuonContainer> m_muonInsideOutContainerKey{this,"MuonInsideOutContainerLocation", "MuonsInsideOut", "InsideOut Muon Container"};
    SG::WriteHandleKey<xAOD::MuonContainer> m_muonOutputKey{this,"MuonOutputLocation", "MuonsOut", "Output Muon Container"};

};

#endif
