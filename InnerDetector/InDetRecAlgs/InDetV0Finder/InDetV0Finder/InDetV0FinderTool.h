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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
/**
   The InDetV0FinderTool reads in the TrackParticle container from StoreGate,
   if useorigin = True only tracks not associated to a primary vertex are used.
   There are options to use silicon tracks only (default) or include Si+TRT and TRT+TRT.

   Pairs of tracks passing pre-selection (using InDetTrackSelectorTool) are
   fitted to a common vertex using TrkV0Fitter or TrkVKalVrtFitter (useV0Fitter = False).

   InDetV0FinderTool can take as input a vertex (or a collection of vertices)
   w.r.t which the V0s can be selected to minimise combinatorics.
     - if such a vertex is provided, tracks are used if fabs(d0/sig_d0) > d0_cut (= 2.)
     - if useVertexCollection = True all vertices in the collection are used
     - if useVertexCollection = False
       - if trkSelPV = True either a primary vertex, if provided, or the beam spot are used
       - if trkSelPV = False (default) all track combinations are used.

   The unconstrained vertex fit is attempted if the radius of the starting point is < maxsxy (= 1000 mm)
   and at least one invariant mass at the starting point is in the allowed range:
   uksmin < m(pipi) < uksmax or ulamin < m(ppi) < ulamax or ulamin < m(pip) < ulamax

   V0s are kept if the cumulative chi2 probability of the unconstrained fit is > minVertProb (= 0.0001)

   If doSimpleV0 = True all vertices that pass these cuts are stored in V0UnconstrVertices.

   If doSimpleV0 = False (default) mass constrained fits are attempted if
     - the invariant mass of the unconstrained V0 is in the allowed range:
       ksmin < m(pipi) < ksmax, lamin < m(ppi), m(pip) < lamax
       and the error on the invariant mass is < errmass (= 100 MeV)
     - if an input vertex (collection) is provided the unconstrained V0 is required to
       have an impact parameter w.r.t the vertex < vert_a0xy_cut (= 3 mm) in xy and
       < vert_a0z_cut (= 15 mm) in z, the cosine of the angle between the V0 momentum and
       the direction from the input vertex to the V0 vertex is > 0,
       Lxy w.r.t the vertex is < vert_lxy_cut (= 500 mm) and Lxy/sigma(Lxy) > vert_lxy_sig (= 2)

   Mass constrainedV0s are kept if the cumulative chi2 probability of the fit is > minVertProb (= 0.0001)

   For successful mass constrained fits a conversion fit is also attempted and if successful,
   the corresponding unconstrained V0 is decorated with the invariant mass, its error and
   the vertex probability of the conversion fit.

   The links between the unconstrained V0 and the successful mass constrained V0s are stored.
*/


/* Forward declarations */

namespace Trk
{
  class IVertexFitter;
  class TrkV0VertexFitter;
  class V0Tools;
  class IExtrapolator;
  class ITrackSelectorTool;
}

namespace HepPDT{
  class ParticleDataTable;
}

namespace Reco{
  class ITrackToVertex;
}

namespace InDet
{
  class VertexPointEstimator;
  class ConversionFinderUtils;

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
                             const xAOD::Vertex* vertex,
			     // AthenaMT migration: passing the vertex collection name at run-time is not supported
			     SG::ReadHandle<xAOD::VertexContainer> vertColl
			     );

  //protected:
  private:
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey { this, "TrackParticleCollection", "InDetTrackParticles",
                                                                         "key for retrieval of TrackParticles" };

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
    bool          m_use_vertColl;             //!< = true select tracks wrt a vertex collection (false)
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
    double        m_d0_cut;                   //!< track d0 significance wrt a vertex (>2.)
    double        m_vert_lxy_sig;             //!< V0 lxy significance wrt a vertex (>2.)
    double        m_vert_lxy_cut;             //!< V0 lxy V0 lxy  (<500.)
    double        m_vert_a0xy_cut;            //!< V0 |a0xy| wrt a vertex (<3.)
    double        m_vert_a0z_cut;             //!< V0 |a0z| wrt a vertex (<15.)

    unsigned int  m_events_processed;
    unsigned int  m_V0s_stored;
    unsigned int  m_Kshort_stored;
    unsigned int  m_Lambda_stored;
    unsigned int  m_Lambdabar_stored;
    unsigned int  m_Gamma_stored;


    void SGError(std::string errService);

    double invariantMass(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2, double &m1, double &m2);

    bool doFit(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D &startingPoint);

    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::VertexContainer * vertColl);
    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::Vertex * vertex);
    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D vertex);

    bool pointAtVertex(const xAOD::Vertex* v0, const xAOD::Vertex* PV);
    bool pointAtVertexColl(xAOD::Vertex* v0, const xAOD::VertexContainer * vertColl);

    bool doMassFit(xAOD::Vertex* vxCandidate, int pdgID);

    xAOD::Vertex* massFit(int pdgID, std::vector<const xAOD::TrackParticle*> pairV0, Amg::Vector3D vertex, Trk::TrkV0VertexFitter* concreteVertexFitter);

    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey { this, "VertexContainer", "PrimaryVertices",
	                                                   "primary vertex container" };
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_v0LinksDecorkey;
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_v0_ksLinksDecorkey;
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_v0_laLinksDecorkey;
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_v0_lbLinksDecorkey;

    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_mDecor_gfit;
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_mDecor_gmass;
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_mDecor_gmasserr;
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_mDecor_gprob;

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };


  };

}//end of namespace InDet

#endif

