///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMakerAlg.h

#ifndef METMakerAlg_H
#define METMakerAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"

class IMETMaker;

namespace met {
  class METMakerAlg : public AthAlgorithm {

  public: 

    /// Constructor with parameters:
    METMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METMakerAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METMakerAlg();

    bool accept(const xAOD::Electron* el);
    bool accept(const xAOD::Photon* ph);
    bool accept(const xAOD::TauJet* tau);
    bool accept(const xAOD::Muon* muon);

    std::string m_mapname;
    std::string m_corename;
    std::string m_outname;

    std::string m_jetColl;
    std::string m_eleColl;
    std::string m_gammaColl;
    std::string m_tauColl;
    std::string m_muonColl;
    
    /// Athena configured tools
    ToolHandle<IMETMaker> m_metmaker;

  }; 

}

#endif
