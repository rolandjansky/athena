///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TauJetCnvTool.h 
// Header file for class TauJetCnvTool
// Author: Michel Janus , janus@cern.ch
/////////////////////////////////////////////////////////////////// 
#ifndef XAODTAUCNV_TAUJETCNVTOOL_H
#define XAODTAUCNV_TAUJETCNVTOOL_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTauCnv/ITauJetCnvTool.h"

//old tau EDM includes
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauJetParameters.h"

// //panTau EDM includes
// #include "PanTauEvent/PanTauDetails.h"

//xAOD tau EDM includes
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"




namespace xAODMaker {
  class TauJetCnvTool
    : public ::AthAlgTool,
      public virtual ITauJetCnvTool
  { 
    
    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    TauJetCnvTool( const std::string& type,
		   const std::string& name, 
		   const IInterface* parent );

    /// Destructor: 
    virtual ~TauJetCnvTool(); 

    // Assignment operator: 
    //TauJetCnvTool &operator=(const TauJetCnvTool &alg); 

    // initializationa nd execution
    virtual StatusCode  initialize();
    virtual StatusCode  convert(const Analysis::TauJetContainer* inputTaus,
				xAOD::TauJetContainer* xaodTauJets);
    // virtual StatusCode  finalize();

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
    TauJetCnvTool();

    void setLinks(const Analysis::TauJet& aodtau, xAOD::TauJet& xaodtau) const;
    ElementLink<xAOD::TrackParticleContainer> getNewTrackLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, const std::string& name) const;
    ElementLink<xAOD::JetContainer> getNewJetLink(const ElementLink<JetCollection>& oldLink, const std::string& name) const;
    void copyPanTauDetails(const Analysis::TauJet& aodtau, xAOD::TauJet& xaodtau) const ;


    /// Containers
    std::string m_inDetTrackParticles; //!< Location/Key for TrackParticles from ID. 
    std::string m_jets; //!< Location/Key for jets. 
  }; 

  // I/O operators
  //////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Inline methods: 
/////////////////////////////////////////////////////////////////// 

}
#endif //> !XAODTAUCNV_TAUJETCNVTOOL_H
