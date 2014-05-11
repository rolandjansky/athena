///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TauJetCnvAlg.h 
// Header file for class TauJetCnvAlg
// Author: Michel Janus , janus@cern.ch
/////////////////////////////////////////////////////////////////// 
#ifndef XAODTAUCNV_TAUJETCNVALG_H
#define XAODTAUCNV_TAUJETCNVALG_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

//old tau EDM includes
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauJetParameters.h"

//panTau EDM includes
#include "PanTauEvent/PanTauDetails.h"

//xAOD tau EDM includes
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"




namespace xAODMaker {
  class TauJetCnvAlg
    : public ::AthAlgorithm
  { 
    
    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    TauJetCnvAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor: 
    virtual ~TauJetCnvAlg(); 

    // Assignment operator: 
    //TauJetCnvAlg &operator=(const TauJetCnvAlg &alg); 

    // Athena algorithm's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  execute();
    virtual StatusCode  finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  private: 

    /// Default constructor: 
    TauJetCnvAlg();

    void setLinks(const Analysis::TauJet& aodtau, xAOD::TauJet& xaodtau) const;
    ElementLink<xAOD::TrackParticleContainer> getNewTrackLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, const std::string& name) const;
    ElementLink<xAOD::JetContainer> getNewJetLink(const ElementLink<JetCollection>& oldLink, const std::string& name) const;
    void copyPanTauDetails(const Analysis::TauJet& aodtau, xAOD::TauJet& xaodtau) const ;


    /// Containers
    std::string m_inputTauJetContainerName;
    std::string m_xaodTauJetContainerName;
    std::string m_inDetTrackParticles; //!< Location/Key for TrackParticles from ID. 
    std::string m_jets; //!< Location/Key for jets. 
  }; 

  // I/O operators
  //////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Inline methods: 
/////////////////////////////////////////////////////////////////// 

}
#endif //> !XAODTAUCNV_TAUJETCNVALG_H
