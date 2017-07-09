///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METSignificanceAlg.h

#ifndef METSignificanceAlg_H
#define METSignificanceAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IMETSignificance;

namespace met {
  class METSignificanceAlg : public AthAlgorithm {

  public: 

    /// Constructor with parameters:
    METSignificanceAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METSignificanceAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METSignificanceAlg();

    std::string m_mapname;
    std::string m_corename;
    std::string m_outname;

    std::string m_softclname;
    std::string m_softtrkname;

    std::string m_jetColl;
    std::string m_eleColl;
    std::string m_gammaColl;
    std::string m_tauColl;
    std::string m_muonColl;

    bool m_doTruthLep;
    
    /// Athena configured tools
    ToolHandle<IMETSignificance> m_metsignif;

  }; 

}

#endif
