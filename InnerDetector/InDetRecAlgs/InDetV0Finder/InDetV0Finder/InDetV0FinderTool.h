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

    bool          m_startup;                  //!< = true not using cuts on constrained fits (true)
    bool          m_useorigin;                //!< = true only using tracks that have no vertex association (true)
    bool          m_samesign;                 //!< = true select tracks with same sign (false)
    bool          m_pv;                       //!< = true select tracks wrt primary vertex (false)
    bool          m_use_vertColl;             //!< = true select tracks wrt a vertex collection (true)
    bool          m_doUnconstrPointing;       //!< = pointing to PV for unconstrained fit (false)
    bool          m_doConstrPointing;         //!< = pointing to PV for mass constrained fit (false)
    bool          m_useTRTplusTRT;            //!< = use TRT+TRT pairs (true)
    bool          m_useTRTplusSi;             //!< = use TRT+Si pairs (true)
    bool          m_useV0Fitter;              //!< = true if using TrkV0Fitter, = false if using VKalVert (true)
    bool          m_useerrorcode5;            //!< = true if using starting points with errorcode = 5 (true)
    int           m_CnstType;                 //!< = 2 VKalVrt exact pointing, = 7 "pass near" (2)
    int           m_vertex;                   //!< = 1 if using starting point, = 2 if using unconstrained fit (1)
    int           m_masses;                   //!< = 1 if using PDG values, = 2 if user set (1)
    int           m_gamma;                    //!< = 1 if using VKalVert for conversion fit, = 2 if using TrkV0Fitter (1)
    int           m_electrons;                //!< = 1 if using electron cuts, = 0 if not (0)
    double        m_masspi;                   //!< pion mass (139.57 MeV)
    double        m_massp;                    //!< proton mass (938.272 MeV)
    double        m_masse;                    //!< electron mass (0.510999 MeV)
    double        m_massK0S;                  //!< Kshort mass (497.672 MeV)
    double        m_massLambda;               //!< Lambda mass (1115.68 MeV)
    double        m_massGamma;                //!< Gamma mass (0.0 MeV)
    double        m_ptTRT;                    //!< Minimum pT for TRT tracks (700. MeV)
    double        m_eprob_s;                  //!< electron probability Si (< 0.995)
    double        m_eprob_t;                  //!< electron probability TRT (< 0.5)
    double        m_minRxy;                   //!< Minimum Rxy of vertex (0.0 mm)
    double        m_minsxy;                   //!< Minimum Rxy of starting point (0.0 mm)
    double        m_maxsxy;                   //!< Maximum Rxy of starting point (1000. mm)
    double        m_minslxy;                  //!< Minimum Lxy of starting point (-1000 mm)
    double        m_maxslxy;                  //!< Maximum Lxy of starting point (1000. mm)
    double        m_rmax_s;                   //!< distance of first measurement to the starting point Si (300. mm)
    double        m_rmax_t;                   //!< distance of first measurement to the starting point TRT (1000. mm)
    double        m_ksmin;                    //!< min Kshort mass (400. MeV)
    double        m_ksmax;                    //!< max Kshort mass (600. MeV)
    double        m_lamin;                    //!< min Lambda mass (1050. MeV)
    double        m_lamax;                    //!< max Lambda mass (1200. MeV)
    double        m_ugamax;                   //!< max Gamma mass for unconstrained fit (100. MeV)
    double        m_errmass_ks;               //!< Kshort mass error (25. MeV)
    double        m_errmass_la;               //!< Lambda mass error (10. MeV)
    double        m_errmass_ga;               //!< Gamma  mass error (10. MeV)
    double        m_errmass_t;                //!< Maximum mass error for TRT (500. MeV)
    double        m_minVertProb;              //!< Minimum vertex probability (0.0001)
    double        m_minConstrVertProb;        //!< Minimum vertex probability for constrained fit (0.0001)
    double        m_hlxymin_ss;               //!< min hitlxy/Rxy for   Si+Si pairs (0. mm)
    double        m_hlxymax_ss;               //!< max hitlxy/Rxy for   Si+Si pairs (10. mm)
    double        m_hlxymin_st;               //!< min hitlxy/Rxy for  Si+TRT pairs (0. mm)
    double        m_hlxymax_st;               //!< max hitlxy/Rxy for  Si+TRT pairs (20. mm)
    double        m_hlxymin_tt;               //!< min hitlxy/Rxy for TRT+TRT pairs (-50. mm)
    double        m_hlxymax_tt;               //!< max hitlxy/Rxy for TRT+TRT pairs (50. mm)

    double        m_pt_s;                     //!< min pt at starting point Si  (-99. MeV)
    double        m_pt_t;                     //!< min pt at starting point TRT (-99. MeV)
    double        m_upt_s;                    //!< min pt of mass-unconstrained candidate Si (-99. MeV)
    double        m_upt_t;                    //!< min pt of mass-unconstrained candidate TRT (-99. MeV)
    double        m_kspt_s;                   //!< min Kshort pt of mass constrained candidate Si (-99. MeV)
    double        m_lapt_s;                   //!< min Lambda pt of mass constrained candidate Si (-99. MeV)
    double        m_gapt_s;                   //!< min conversion pt of mass constrained candidate Si (-99. MeV)
    double        m_kspt_t;                   //!< min Kshort pt of mass constrained candidate TRT (-99. MeV)
    double        m_lapt_t;                   //!< min Lambda pt of mass constrained candidate TRT (-99. MeV)
    double        m_gapt_t;                   //!< min conversion pt of mass constrained candidate TRT (-99. MeV)

    double        m_ulxy_s;                   //!< Minimum Lxy of the mass-unconstrained candidate Si (-1000.)
    double        m_ulxy_t;                   //!< Minimum Lxy of the mass-unconstrained candidate TRT (-1000.)
    double        m_lalxy_s;                  //!< Minimum Lxy of the Lambda candidate Si (-1000.)
    double        m_lalxy_t;                  //!< Minimum Lxy of the Lambda candidate TRT (-1000.)
    double        m_kslxy_s;                  //!< Minimum Lxy of the Ks candidate Si (-1000.)
    double        m_kslxy_t;                  //!< Minimum Lxy of the Ks candidate TRT (-1000.)
    double        m_galxy_s;                  //!< Minimum Lxy of the conversion candidate Si (-1000.)
    double        m_galxy_t;                  //!< Minimum Lxy of the conversion candidate TRT (-1000.)

    double        m_rmax;
    double        m_hlxymin;
    double        m_hlxymax;

    double        m_pt;
    double        m_upt;
    double        m_kspt;
    double        m_lapt;
    double        m_gapt;

    double        m_ulxy;
    double        m_lalxy;
    double        m_kslxy;
    double        m_galxy;
    double        m_d0_cut;
    
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

    bool doFit(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D &startingPoint, xAOD::Vertex* primaryVertex);

    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::VertexContainer * vertColl);
    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::Vertex * vertex);
    bool d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D vertex);

    double getPt(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2);

  };

}//end of namespace InDet

#endif

