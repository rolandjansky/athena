/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetV0FinderTool.h  -  Description
                             -------------------
    begin   : 30-11-2014
    authors : Evelina Bouhova-Thacker (Lancater University)
    email   : e.bouhova@cern.ch
    changes :

 ***************************************************************************/

#ifndef INDETV0FINDERTOOL_H
#define INDETV0FINDERTOOL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "DataModel/DataVector.h"
#include "ITrackToVertex/ITrackToVertex.h"

/* Forward declarations */

class IBeamCondSvc;

namespace Trk
{
  class IVertexFitter;
  class TrkV0VertexFitter;
  class V0Tools;
  class IExtrapolator;
  class ITrackSelectorTool;
  class ParticleDataTable;
}

namespace InDet
{
  class VertexPointEstimator;

  static const InterfaceID IID_InDetV0FinderTool("InDetV0FinderTool", 1, 0);

  class InDetV0FinderTool:  virtual public AthAlgTool
  {
  public:
    InDetV0FinderTool(const std::string& t, const std::string& n, const IInterface*  p);
    ~InDetV0FinderTool();
    StatusCode initialize();
    StatusCode finalize();

    static const InterfaceID& interfaceID() { return IID_InDetV0FinderTool;}

    StatusCode performSearch(xAOD::VertexContainer*& v0Container, xAOD::VertexAuxContainer*& v0AuxContainer,
                             xAOD::VertexContainer*& ksContainer, xAOD::VertexAuxContainer*& ksAuxContainer,
                             xAOD::VertexContainer*& laContainer, xAOD::VertexAuxContainer*& laAuxContainer,
                             xAOD::VertexContainer*& lbContainer, xAOD::VertexAuxContainer*& lbAuxContainer,
                             xAOD::Vertex* vertex, std::string vertCollName);

  //protected:
  private:

    ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
    ToolHandle < Trk::IVertexFitter > m_iVKVertexFitter;
    ToolHandle < Trk::IVertexFitter > m_iKshortFitter;
    ToolHandle < Trk::IVertexFitter > m_iLambdaFitter;
    ToolHandle < Trk::IVertexFitter > m_iLambdabarFitter;
    ToolHandle < Trk::IVertexFitter > m_iGammaFitter;
    ToolHandle < Trk::V0Tools > m_V0Tools;
    ToolHandle < Reco::ITrackToVertex > m_trackToVertexTool;
    ToolHandle < InDet::ConversionFinderUtils > m_helpertool;
    ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
    ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
    ToolHandle < Trk::IExtrapolator > m_extrapolator;

    const HepPDT::ParticleDataTable *m_particleDataTable;

    bool          m_doSimpleV0;               //!< = true equivalent to the old InDetSimpleV0Finder (false)
    bool          m_useorigin;                //!< = true only using tracks that have no vertex association (true)
    bool          m_samesign;                 //!< = true select tracks with same sign (false)
    bool          m_pv;                       //!< = true select tracks wrt primary vertex (false)
    bool          m_use_vertColl;             //!< = true select tracks wrt a vertex collection (true)
    bool          m_useTRTplusTRT;            //!< = use TRT+TRT pairs (true)
    bool          m_useTRTplusSi;             //!< = use TRT+Si pairs (true)
    bool          m_useV0Fitter;              //!< = true if using TrkV0Fitter, = false if using VKalVert (true)
    int           m_masses;                   //!< = 1 if using PDG values, = 2 if user set (1)
    double        m_masspi;                   //!< pion mass (139.57 MeV)
    double        m_massp;                    //!< proton mass (938.272 MeV)
    double        m_masse;                    //!< electron mass (0.510999 MeV)
    double        m_massK0S;                  //!< Kshort mass (497.672 MeV)
    double        m_massLambda;               //!< Lambda mass (1115.68 MeV)
    double        m_ptTRT;                    //!< Minimum pT for TRT tracks (700. MeV)
    double        m_maxsxy;                   //!< Maximum Rxy of starting point (1000. mm)
    double        m_uksmin;                   //!< min Kshort mass, unconstrained fit (400. MeV)
    double        m_uksmax;                   //!< max Kshort mass, unconstrained fit (600. MeV)
    double        m_ulamin;                   //!< min Lambda mass, unconstrained fit (1000. MeV)
    double        m_ulamax;                   //!< max Lambda mass, unconstrained fit (1200. MeV)
    double        m_ksmin;                    //!< min Kshort mass (400. MeV)
    double        m_ksmax;                    //!< max Kshort mass (600. MeV)
    double        m_lamin;                    //!< min Lambda mass (1000. MeV)
    double        m_lamax;                    //!< max Lambda mass (1200. MeV)
    double        m_errmass;                  //!< Maximum mass error (100. MeV)
    double        m_minVertProb;              //!< Minimum vertex probability (0.0001)
    double        m_minConstrVertProb;        //!< Minimum vertex probability for constrained fit (0.0001)
    double        m_d0_cut;                   //!< track d0 significance wrt a vertex (>5.)
    double        m_vert_lxy_sig;             //!< V0 lxy significance wrt a vertex (>5.)
    double        m_vert_lxy_cut;             //!< V0 lxy V0 lxy  (<500.)
    double        m_vert_a0xy_cut;            //!< V0 |a0xy| wrt a vertex (<3.)
    double        m_vert_a0z_cut;             //!< V0 |a0z| wrt a vertex (<15.)
    
    unsigned int  m_events_processed;
    unsigned int  m_V0s_stored;
    unsigned int  m_Kshort_stored;
    unsigned int  m_Lambda_stored;
    unsigned int  m_Lambdabar_stored;
    unsigned int  m_Gamma_stored;

    ServiceHandle <IBeamCondSvc> m_beamConditionsService;

    std::string   m_TrkParticleCollection ;   //!< Name of input track particle collection
    std::string   m_V0CandidatesOutputName;   //!< Name of output container to store results


    void SGError(std::string errService);

    double invariantMass(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2, double &m1, double &m2);

    bool doFit(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D &startingPoint);

    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::VertexContainer * vertColl);
    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::Vertex * vertex);
    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D vertex);

    bool pointAtVertex(xAOD::Vertex* v0, xAOD::Vertex* PV);
    bool pointAtVertexColl(xAOD::Vertex* v0, const xAOD::VertexContainer * vertColl);

    bool doMassFit(xAOD::Vertex* vxCandidate, int pdgID);

    xAOD::Vertex* massFit(int pdgID, std::vector<const xAOD::TrackParticle*> pairV0, Trk::Vertex vertex, Trk::TrkV0VertexFitter* concreteVertexFitter);

  };

}//end of namespace InDet

#endif

