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
#include "HepPDT/ParticleDataTable.hh"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "TrkParticleBase/TrackParticleBase.h"

/**
   @class InDetV0Finder
   Execute method for the main V0 finding module.
   InDetV0Finder first gets its input tracks from StoreGate.
*/

/* Forward declarations */

namespace Trk
{
  class IVertexFitter;
  class TrkV0VertexFitter;
  class V0Tools;
  class IExtrapolator;
  class VxCandidate;
  class ExtendedVxCandidate;
  class ITrackSelectorTool;
  class ParticleDataTable;
  class Vertex;
  class RecVertex;
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

    ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
    ToolHandle < Trk::IVertexFitter > m_iVKVertexFitter;
    ToolHandle < Trk::IVertexFitter > m_iKshortFitter;
    ToolHandle < Trk::IVertexFitter > m_iLambdaFitter;
    ToolHandle < Trk::IVertexFitter > m_iLambdabarFitter;
    ToolHandle < Trk::IVertexFitter > m_iGammaFitter;
    ToolHandle < Trk::V0Tools > m_V0Tools;
    ToolHandle < InDet::ConversionFinderUtils > m_helpertool;
    ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
    ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
    ToolHandle < Trk::IExtrapolator > m_extrapolator;

    const HepPDT::ParticleDataTable *m_particleDataTable;

    bool          m_startup;                  //!< = true not using cuts on constrained fits (true)
    bool          m_useorigin;                //!< = true only using tracks that have no vertex association (true)
    bool          m_samesign;                 //!< = true select tracks with same sign (false)
    bool          m_pv;                       //!< = true select tracks wrt primary vertex (false)
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
    double        m_ksmin_s;                  //!< min Kshort mass at starting point Si (400. MeV)
    double        m_ksmax_s;                  //!< max Kshort mass at starting point Si (600. MeV)
    double        m_lamin_s;                  //!< min Lambda mass at starting point Si (1050. MeV)
    double        m_lamax_s;                  //!< max Lambda mass at starting point Si (1200. MeV)
    double        m_ksmin_t;                  //!< min Kshort mass at starting point TRT (0. MeV)
    double        m_ksmax_t;                  //!< max Kshort mass at starting point TRT (2000. MeV)
    double        m_lamin_t;                  //!< min Lambda mass at starting point TRT (1050. MeV)
    double        m_lamax_t;                  //!< max Lambda mass at starting point TRT (2000. MeV)
    double        m_uksmin_s;                 //!< min Lambda mass for unconstrained fit (460. MeV)
    double        m_uksmax_s;                 //!< max Lambda mass for unconstrained fit (540. MeV)
    double        m_ulamin_s;                 //!< min Lambda mass for unconstrained fit (1100. MeV)
    double        m_ulamax_s;                 //!< max Lambda mass for unconstrained fit (1135. MeV)
    double        m_uksmin_t;                 //!< min Kshort mass for unconstrained fit (350. MeV)
    double        m_uksmax_t;                 //!< max Kshort mass for unconstrained fit (800. MeV)
    double        m_ulamin_t;                 //!< min Kshort mass for unconstrained fit (1100. MeV)
    double        m_ulamax_t;                 //!< max Kshort mass for unconstrained fit (1450. MeV)
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


    std::string   m_TrkParticleCollection ;   //!< Name of input track particle collection
    std::string   m_VxPrimaryCandidateName;   //!< Name of primary vertex container
    std::string   m_V0CandidatesOutputName;   //!< Name of output container to store results

    double        m_rmax;
    double        m_ksmin;
    double        m_ksmax;
    double        m_lamin;
    double        m_lamax;
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
    
    long          m_events_processed;
    long          m_V0s_stored;
    long          m_Kshort_stored;
    long          m_Lambda_stored;
    long          m_Lambdabar_stored;
    long          m_Gamma_stored;

    void SGError(std::string errService);

    double invariantMass(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2, double &m1, double &m2);

    bool doFit(const Trk::TrackParameters* track1, const Trk::TrackParameters* track2, Amg::Vector3D &startingPoint, Trk::RecVertex* primaryVertex);

    bool useFit(const Trk::TrackParameters* track1, const Trk::TrackParameters* track2, const Trk::ExtendedVxCandidate* vxCandidate, Trk::RecVertex* primaryVertex);

    bool doMassFit(const Trk::ExtendedVxCandidate* vxCandidate, int pdgID, bool is_TRT);

    bool electronPair(const Trk::TrackSummary* tSum1, const Trk::TrackSummary* tSum2, int nclus1, int nclus2);

    Trk::VxCandidate* massFit(int pdgID, std::vector<const Trk::TrackParameters*> pairV0, Trk::RecVertex * pointingVertex, Trk::Vertex vertex, Trk::TrkV0VertexFitter* concreteVertexFitter, Trk::RecVertex* primaryVertex);

    double getPt(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2);

  };

}//end of namespace InDet

#endif

