/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DiphotonVertexDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DiphotonVertexDecorator_H
#define DERIVATIONFRAMEWORK_DiphotonVertexDecorator_H
 
#include <string>
#include <vector>
#include <algorithm>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
// xAOD header files
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
// xAOD header files
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"


namespace CP { 
  class IPhotonVertexSelectionTool;
  class IPhotonPointingTool;
}

namespace DerivationFramework {

  /** @class DiphotonVertexDecorator
      @author Bruno Lenzi
      @author Leo Cerda
      @author magdac@cern.ch
     */
  class DiphotonVertexDecorator : public AthAlgTool, public IAugmentationTool {

    public: 
      /** Constructor with parameters */
      DiphotonVertexDecorator( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~DiphotonVertexDecorator();

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();

      virtual StatusCode addBranches() const;

    private:

      ///////////////
      ///// TOOLS 
      ToolHandle<CP::IPhotonVertexSelectionTool> m_photonVertexSelectionTool;
      ToolHandle<CP::IPhotonPointingTool> m_photonPointingTool;
      
      ///////////////
      ///// SETTINGS

      std::string m_photonSGKey;
      std::string m_diphotonVertexSGKey;
      std::string m_primaryVertexSGKey;

      double m_minPhotonPt;
      bool   m_removeCrack;
      double m_maxEta;
      bool   m_ignoreConv;

      bool  PhotonPreselect(const xAOD::Photon *ph) const;

  }; 

}

#endif // DERIVATIONFRAMEWORK_DiphotonVertexDecorator_H
