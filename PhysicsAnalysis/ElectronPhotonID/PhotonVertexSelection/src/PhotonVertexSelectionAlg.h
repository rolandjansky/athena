/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonVertexSelectionAlg.h

#ifndef PhotonVertexSelectionAlg_H
#define PhotonVertexSelectionAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEgamma/Photon.h"

namespace CP {
  class IPhotonVertexSelectionTool;

  class PhotonVertexSelectionAlg : public AthAlgorithm { 

  public: 

    /// Constructor with parameters:
    PhotonVertexSelectionAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~PhotonVertexSelectionAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    PhotonVertexSelectionAlg();

    bool acceptPhoton(const xAOD::Photon& gam);

  private:

    std::string m_inVxColl;
    std::string m_inGamColl;
    std::string m_outVxColl;

    ToolHandle<CP::IPhotonVertexSelectionTool> m_vxSelTool;

  }; 

}

#endif
