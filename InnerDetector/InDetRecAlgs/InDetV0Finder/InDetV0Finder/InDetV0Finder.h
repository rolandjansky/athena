/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetV0Finder.h  -  Description
                             -------------------
    begin   : 20-07-2005
    authors : Evelina Bouhova-Thacker (Lancater University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes :

 ***************************************************************************/

#ifndef INDETV0FINDER_INDETV0FINDER_H
#define INDETV0FINDER_INDETV0FINDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetV0Finder/InDetV0FinderTool.h"
#include "HepPDT/ParticleDataTable.hh"

/**
   @class InDetV0Finder
   Execute method for the main V0 finding module.
   InDetV0Finder uses the InDetV0FinderTool and records the V0 containers:
   V0UnconstrVertices, V0KshortVertices, V0LambdaVertices and V0LambdabarVertices.
   If decorateV0 = True, the mass, pT and Rxy with corresponding errors are stored.
*/

/* Forward declarations */

namespace Trk
{
  class V0Tools;
  class ParticleDataTable;
}

namespace InDet
{
  class VertexPointEstimator;

  class InDetV0Finder : public AthAlgorithm

  {
  public:
    InDetV0Finder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~InDetV0Finder();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    StatusCode resetStatistics();

  protected:

    ToolHandle<InDet::InDetV0FinderTool> m_v0FinderTool;
    ToolHandle < Trk::V0Tools >          m_V0Tools;
    const HepPDT::ParticleDataTable *m_particleDataTable;

    bool          m_decorate;                 //!< decorate V0 containers
    int           m_masses;                   //!< = 1 if using PDG values, = 2 if user set (1)
    double        m_masspi;                   //!< pion mass (139.57 MeV)
    double        m_massp;                    //!< proton mass (938.272 MeV)
    double        m_masse;                    //!< electron mass (0.510999 MeV)
    double        m_massK0S;                  //!< Kshort mass (497.672 MeV)
    double        m_massLambda;               //!< Lambda mass (1115.68 MeV)

    std::string   m_VxPrimaryCandidateName;   //!< Name of primary vertex container

    long          m_events_processed;
    long          m_V0s_stored;
    long          m_Kshort_stored;
    long          m_Lambda_stored;
    long          m_Lambdabar_stored;

    std::string                          m_v0ContainerName;
    std::string                          m_ksContainerName;
    std::string                          m_laContainerName;
    std::string                          m_lbContainerName;

  };

}//end of namespace InDet

#endif

