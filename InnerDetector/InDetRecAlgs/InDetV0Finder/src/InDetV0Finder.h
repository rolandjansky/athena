/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetV0Finder.h  -  Description
                             -------------------
    begin   : 20-07-2005
    authors : Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes :

 ***************************************************************************/

#ifndef INDETV0FINDER_INDETV0FINDER_H
#define INDETV0FINDER_INDETV0FINDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/VertexContainerFwd.h"



/**
   @class InDetV0Finder
   Execute method for the main V0 finding module.
   InDetV0Finder uses the InDetV0FinderTool and records the V0 containers:
   V0UnconstrVertices, V0KshortVertices, V0LambdaVertices and V0LambdabarVertices.
   If decorateV0 = True, the mass, pT and Rxy with corresponding errors are stored.
*/

/* Forward declarations */

namespace Trk{
  class V0Tools;
}

namespace HepPDT{
  class ParticleDataTable;
}

namespace InDet
{
  class VertexPointEstimator;
  class InDetV0FinderTool;
  
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
    // input primary vertices
    SG::ReadHandleKey<xAOD::VertexContainer>        m_vertexKey { this, "VxPrimaryCandidateName", "PrimaryVertices", 
                                                                  "key for retrieving vertices" };
    // V0 candidate output containers
    SG::WriteHandleKey<xAOD::VertexContainer>       m_v0Key { this, "V0ContainerName", "V0Candidates", "V0 container" };
    SG::WriteHandleKey<xAOD::VertexContainer>       m_ksKey { this, "KshortContainerName", "KshortCandidates", "Ks container" };
    SG::WriteHandleKey<xAOD::VertexContainer>       m_laKey { this, "LambdaContainerName", "LambdaCandidates",
                                                              "Lambda container" };
    SG::WriteHandleKey<xAOD::VertexContainer>       m_lbKey { this, "LambdabarContainerName", "LambdabarCandidates", 
                                                              "Lambdabar container" };

    // Decorator keys will be modified at run-time to conform to the correct container name
    // For the run-time update to work, the decoration key name properties must start with a period (".")
    // V0 decorators
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorKsMass 
                    { this, "KsMass_v0", ".Kshort_mass", "Ks mass for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorKsMassErr 
                    { this, "KsMassErr_v0", ".Kshort_massError", "Ks mass error for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorLaMass 
                    { this, "LaMass_v0", ".Lambda_mass", "Lambda mass for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorLaMassErr 
                    { this, "LaMassErr_v0", ".Lambda_massError", "Lambda mass error for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorLbMass 
                    { this, "LbMass_v0", ".Lambdabar_mass", "Lambdabar mass for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorLbMassErr 
                    { this, "LbMassErr_v0", ".Lambdabar_massError", "Lambdabar mass error for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPt_v0 
                    { this, "Pt_v0", ".pT", "Transverse momentum for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPtErr_v0 
                    { this, "PtErr_v0", ".pTError", "Transverse momentum error for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxy_v0 
                    { this, "Rxy_v0", ".Rxy", "Rxy for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxyErr_v0 
                    { this, "RxyErr_v0", ".RxyError", "Rxy error for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPx_v0 
                    { this, "Px_v0", ".px", "Px for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPy_v0 
                    { this, "Py_v0", ".py", "Py for v0" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPz_v0 
                    { this, "Pz_v0", ".pz", "Pz for v0" };

    // Ks decorators
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorMass_ks 
                    { this, "Mass_ks", ".mass", "mass for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorMassErr_ks 
                    { this, "MassErr_ks", ".massError", "mass error for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPt_ks
                    { this, "Pt_ks", ".pT", "Transverse momentum for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPtErr_ks
                    { this, "PtErr_ks", ".pTError", "Transverse momentum error for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxy_ks 
                    { this, "Rxy_ks", ".Rxy", "Rxy for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxyErr_ks 
                    { this, "RxyErr_ks", ".RxyError", "Rxy error for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPx_ks 
                    { this, "Px_ks", ".px", "Px for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPy_ks
                    { this, "Py_ks", ".py", "Py for Ks" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPz_ks 
                    { this, "Pz_ks", ".pz", "Pz for Ks" };

    // Lambda decorators
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorMass_la 
                    { this, "Mass_la", ".mass", "mass for Lambda" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorMassErr_la 
                    { this, "MassErr_la", ".massError", "mass error for Lambda" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPt_la 
                    { this, "Pt_la", ".pT", "Transverse momentum for Lambda" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPtErr_la 
                    { this, "PtErr_la", ".pTError", "Transverse momentum error for Lambda" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxy_la
                    { this, "Rxy_la", ".Rxy", "Rxy for la" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxyErr_la
                    { this, "RxyErr_la", ".RxyError", "Rxy error for Lambda" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPx_la 
                    { this, "Px_la", ".px", "Px for Lambda" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPy_la 
                    { this, "Py_la", ".py", "Py for Lambda" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPz_la 
                    { this, "Pz_la", ".pz", "Pz for Lambda" };

    // Lambdabar decorators
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorMass_lb 
                    { this, "Mass_lb", ".mass", "mass for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorMassErr_lb 
                    { this, "MassErr_lb", ".massError", "mass error for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPt_lb 
                    { this, "Pt_lb", ".pT", "Transverse momentum for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPtErr_lb 
                    { this, "PtErr_lb", ".pTError", "Transverse momentum error for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxy_lb 
                    { this, "Rxy_lb", ".Rxy", "Rxy for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorRxyErr_lb 
                    { this, "RxyErr_lb", ".RxyError", "Rxy error for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPx_lb
                    { this, "Px_lb", ".px", "Px for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPy_lb 
                    { this, "Py_lb", ".py", "Py for Lambdabar" };
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::VertexContainer> > m_decorPz_lb 
                    { this, "Pz_lb", ".pz", "Pz for Lambdabar" };

    // Tools

    ToolHandle<InDet::InDetV0FinderTool> m_v0FinderTool;
    ToolHandle < Trk::V0Tools >          m_V0Tools;

    // Other members

    const HepPDT::ParticleDataTable *m_particleDataTable;

    bool          m_decorate;                 //!< decorate V0 containers
    int           m_masses;                   //!< = 1 if using PDG values, = 2 if user set (1)
    double        m_masspi;                   //!< pion mass (139.57 MeV)
    double        m_massp;                    //!< proton mass (938.272 MeV)
    double        m_masse;                    //!< electron mass (0.510999 MeV)
    double        m_massK0S;                  //!< Kshort mass (497.672 MeV)
    double        m_massLambda;               //!< Lambda mass (1115.68 MeV)

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

