/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METASSOCTESTALG_H
#define METASSOCTESTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODMissingET/MissingETAssociation.h"

namespace met {

  class METAssocTestAlg : public AthAlgorithm
  {

  public:
    METAssocTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~METAssocTestAlg() {}

    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();

  private:

    StatusCode checkJet(const xAOD::MissingETAssociation&);
    StatusCode checkObjects(const xAOD::MissingETAssociation&);
    StatusCode checkAssoc(const xAOD::MissingETAssociation&);
    StatusCode checkMiscAssoc(const xAOD::MissingETAssociation&);

    std::string m_mapname = "";    // internal var for retrieving MET map
    std::string m_jetname = "";    // internal var for retrieving jet collection
    // configure m_jetname if the container name is not m_mapsuffix+"Jets"

    std::string m_mapsuffix = "";  // configurable var setting m_mapname and m_jetname
    bool m_failOnInconsistency = false;
  };
}

#endif
