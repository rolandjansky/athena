/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TestIsolationAthenaAlg.cxx 672993 2015-06-06 16:55:34Z dzhang $

// Local include(s):
#include "TestIsolationAthenaAlg.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

// other include(s):
#include <cmath>

namespace CP 
{

  TestIsolationAthenaAlg::TestIsolationAthenaAlg( const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc ),
    m_sgKeyPhotons("Photons"),
    m_sgKeyElectrons("Electrons"),
    m_sgKeyMuons("Muons")
  {  
    declareProperty( "sgKeyPhotons" , m_sgKeyPhotons );
    declareProperty( "sgKeyElectrons" , m_sgKeyElectrons );
    declareProperty( "sgKeyMuons" , m_sgKeyMuons );
    declareProperty( "IsoTool", m_isoTool );
  }

  StatusCode TestIsolationAthenaAlg::initialize()
  {
    ATH_CHECK( m_isoTool.retrieve() );
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }

  StatusCode TestIsolationAthenaAlg::execute()
  {
    const xAOD::PhotonContainer* photons(nullptr);
    ATH_CHECK( evtStore()->retrieve(photons,m_sgKeyPhotons) );
    for (auto x : *photons) {
      if (x->pt() > 7000.) {
        if (x->caloCluster() != nullptr) {
          if (fabs(x->caloCluster()->eta()) < 2.47) {
            if (m_isoTool->accept( *x )) {
              ATH_MSG_INFO(" Photon passes Isolation");
            }
          }       
        }
      }
    }
    
    
    const xAOD::ElectronContainer* electrons(nullptr);
    ATH_CHECK( evtStore()->retrieve(electrons,m_sgKeyElectrons) );
    for (auto x : *electrons) {
      if (x->pt() > 7000.) {
        if (x->caloCluster() != nullptr) {      
          if (fabs(x->caloCluster()->eta()) < 2.47) {
            if (m_isoTool->accept( *x )) {
              ATH_MSG_INFO(" Electron passes Isolation");
            }     
          }
        }
      }
    }
    
    const xAOD::MuonContainer* muons(nullptr);
    ATH_CHECK( evtStore()->retrieve(muons,m_sgKeyMuons) );
    for (auto x : *muons) {
      if (x->pt() > 7000.) {
        if (fabs(x->eta()) < 2.5) {
          if (m_isoTool->accept( *x )) {
            ATH_MSG_INFO(" Muon passes Isolation");
          }
        }
      }
    }
    
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }
}
