/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetV0FinderTool.cxx  -  Description
                             -------------------
    begin   : 20-07-2005
    authors : Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes : December 2014
    author  : Evelina Bouhova-Thacker <e.bouhova@cern.ch> 
              Changed to use xAOD

 ***************************************************************************/

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "InDetV0Finder/InDetV0FinderTool.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh" // for chi2prob calculation

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include <vector>
#include <cmath>

namespace InDet
{
  
InDetV0FinderTool::InDetV0FinderTool(const std::string& t, const std::string& n, const IInterface* p)
  :
  AthAlgTool(t,n,p),
  m_iVertexFitter("Trk::V0VertexFitter"),
  m_iVKVertexFitter("Trk::TrkVKalVrtFitter"),
  m_iKshortFitter("Trk::TrkVKalVrtFitter"),
  m_iLambdaFitter("Trk::TrkVKalVrtFitter"),
  m_iLambdabarFitter("Trk::TrkVKalVrtFitter"),
  m_iGammaFitter("Trk::TrkVKalVrtFitter"),
  m_V0Tools("Trk::V0Tools"),
  m_trackToVertexTool("Reco::TrackToVertex"),
  m_helpertool("InDet::ConversionFinderUtils"),
  m_trkSelector("InDet::TrackSelectorTool"),
  m_vertexEstimator("InDet::VertexPointEstimator"),
  //m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
  m_extrapolator("Trk::Extrapolator"),
  m_particleDataTable(0),
  m_startup(true),
  m_useorigin(true),
  m_samesign(false),
  m_pv(true),
  m_use_vertColl(false),
  m_useTRTplusTRT(false),
  m_useTRTplusSi(false),
  m_useV0Fitter(false),
  m_useerrorcode5(true),
  m_CnstType(2),
  m_vertex(1),
  m_masses(1),
  m_gamma(1),
  m_electrons(0),
  m_masspi(139.57),
  m_massp(938.272),
  m_masse(0.510999),
  m_massK0S(497.672),
  m_massLambda(1115.68),
  m_massGamma(0.),
  m_ptTRT(700.),
  m_minRxy(0.0),
  m_minsxy(0.0),
  m_maxsxy(1000.),
  m_minslxy(-1000),
  m_maxslxy(1000),
  m_rmax_s(300.),
  m_rmax_t(1000.),
  m_ksmin(400.),
  m_ksmax(600.),
  m_lamin(1000.),
  m_lamax(1200.),
  m_ugamax(100.0),
  m_errmass_ks(25.),
  m_errmass_la(10.),
  m_errmass_ga(10.),
  m_errmass_t(500.),
  m_minVertProb(0.0001),
  m_minConstrVertProb(0.0001),
  m_hlxymin_ss(0.),
  m_hlxymax_ss(10.),
  m_hlxymin_st(0.),
  m_hlxymax_st(20.),
  m_hlxymin_tt(-50.),
  m_hlxymax_tt(50.),
  m_pt_s(-99.),
  m_pt_t(-99.),
  m_upt_s(-99.),
  m_upt_t(-99.),
  m_kspt_s(-99.),
  m_lapt_s(-99.),
  m_gapt_s(-99.),
  m_kspt_t(-99.),
  m_lapt_t(-99.),
  m_gapt_t(-99.),
  m_ulxy_s(-1000.),
  m_ulxy_t(-1000.),
  m_lalxy_s(-1000.),
  m_lalxy_t(-1000.),
  m_kslxy_s(-1000.),
  m_kslxy_t(-1000.),
  m_galxy_s(-1000.),
  m_galxy_t(-1000.),
  m_d0_cut(5.),

  m_beamConditionsService("BeamCondSvc", n),
  m_TrkParticleCollection("TrackParticleCandidate"),
  m_V0CandidatesOutputName("V0Candidates")
{
  declareInterface<InDetV0FinderTool>(this);
  declareProperty("VertexFitterTool", m_iVertexFitter);
  declareProperty("VKVertexFitterTool", m_iVKVertexFitter);
  declareProperty("KshortFitterTool", m_iKshortFitter);
  declareProperty("LambdaFitterTool", m_iLambdaFitter);
  declareProperty("LambdabarFitterTool", m_iLambdabarFitter);
  declareProperty("GammaFitterTool", m_iGammaFitter);
  declareProperty("V0Tools",m_V0Tools);
  declareProperty("TrackToVertexTool",m_trackToVertexTool);
  declareProperty("ConversionFinderHelperTool", m_helpertool);
  declareProperty("TrackSelectorTool", m_trkSelector);
  declareProperty("VertexPointEstimator", m_vertexEstimator);
  declareProperty("Extrapolator", m_extrapolator);
  declareProperty("startup", m_startup);
  declareProperty("useorigin", m_useorigin);
  declareProperty("AddSameSign", m_samesign);
  declareProperty("trkSelPV", m_pv);
  declareProperty("useVertexCollection", m_use_vertColl);
  declareProperty("useTRTplusTRT", m_useTRTplusTRT);
  declareProperty("useTRTplusSi", m_useTRTplusSi);
  declareProperty("useV0Fitter", m_useV0Fitter);
  declareProperty("useerrorcode5", m_useerrorcode5);
  declareProperty("CnstType", m_CnstType);
  declareProperty("vertex", m_vertex);
  declareProperty("masses", m_masses);
  declareProperty("gamma", m_gamma);
  declareProperty("electrons", m_electrons);
  declareProperty("masspi", m_masspi);
  declareProperty("massp", m_massp);
  declareProperty("masse", m_masse);
  declareProperty("massK0S", m_massK0S);
  declareProperty("massLambda", m_massLambda);
  declareProperty("massGamma", m_massGamma);
  declareProperty("ptTRT", m_ptTRT);
  declareProperty("minRxy", m_minRxy);
  declareProperty("minsxy", m_minsxy);
  declareProperty("maxsxy", m_maxsxy);

  declareProperty("minslxy", m_minslxy);
  declareProperty("maxslxy", m_maxslxy);

  declareProperty("rmax_s", m_rmax_s);
  declareProperty("rmax_t", m_rmax_t);
  declareProperty("ksmin", m_ksmin);
  declareProperty("ksmax", m_ksmax);
  declareProperty("lamin", m_lamin);
  declareProperty("lamax", m_lamax);
  declareProperty("ugamax", m_ugamax),
  declareProperty("errmass_ks", m_errmass_ks);
  declareProperty("errmass_la", m_errmass_la);
  declareProperty("errmass_ga", m_errmass_ga);
  declareProperty("errmass_t", m_errmass_t);
  declareProperty("minVertProb", m_minVertProb);
  declareProperty("minConstrVertProb", m_minConstrVertProb);
  declareProperty("hlxymin_ss", m_hlxymin_ss);
  declareProperty("hlxymin_st", m_hlxymin_st);
  declareProperty("hlxymin_tt", m_hlxymin_tt);
  declareProperty("hlxymax_ss", m_hlxymax_ss);
  declareProperty("hlxymax_st", m_hlxymax_st);
  declareProperty("hlxymax_tt", m_hlxymax_tt);

  declareProperty("pt_s"  , m_pt_s   );
  declareProperty("pt_t"  , m_pt_t   );
  declareProperty("upt_s" , m_upt_s  );
  declareProperty("upt_t" , m_upt_t  );
  declareProperty("kspt_s", m_kspt_s );
  declareProperty("lapt_s", m_lapt_s );
  declareProperty("gapt_s", m_gapt_s );
  declareProperty("kspt_t", m_kspt_t );
  declareProperty("lapt_t", m_lapt_t );
  declareProperty("gapt_t", m_gapt_t );

  declareProperty("ulxy_s" , m_ulxy_s  );
  declareProperty("ulxy_t" , m_ulxy_t  );
  declareProperty("kslxy_s", m_kslxy_s );
  declareProperty("lalxy_s", m_lalxy_s );
  declareProperty("galxy_s", m_galxy_s );
  declareProperty("kslxy_t", m_kslxy_t );
  declareProperty("lalxy_t", m_lalxy_t );
  declareProperty("galxy_t", m_galxy_t );
  declareProperty("d0_cut", m_d0_cut );

  declareProperty("BeamConditionsSvc", m_beamConditionsService); 
  declareProperty("TrackParticleCollection", m_TrkParticleCollection);
  declareProperty("V0CandidatesOutputName",m_V0CandidatesOutputName="V0Candidates");
}

InDetV0FinderTool::~InDetV0FinderTool() {}

StatusCode InDetV0FinderTool::initialize()
{
  StatusCode sc;

// Get the right vertex fitting tool from ToolSvc 
  if (m_useV0Fitter) {
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;
    }
  
    if ( m_iGammaFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iGammaFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iGammaFitter << endreq;
    }

  } else {
    if ( m_iVKVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVKVertexFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iVKVertexFitter << endreq;
    }

// Get the VKalVrt Ks vertex fitting tool from ToolSvc
    if ( m_iKshortFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iKshortFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iKshortFitter << endreq;
    }

// Get the VKalVrt Lambda vertex fitting tool from ToolSvc
    if ( m_iLambdaFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iLambdaFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iLambdaFitter << endreq;
    }

// Get the VKalVrt Lambdabar vertex fitting tool from ToolSvc
    if ( m_iLambdabarFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iLambdabarFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iLambdabarFitter << endreq;
    }

// Get the VKalVrt Gamma vertex fitting tool from ToolSvc
    if ( m_iGammaFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iGammaFitter << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iGammaFitter << endreq;
    }

  }

// get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  sc = service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not initialize Particle Properties Service" << endreq;
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

// uploading the V0 tools
  if ( m_V0Tools.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_V0Tools << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endreq;
  }

// Get the TrackToVertex extrapolator tool
  if ( m_trackToVertexTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackToVertexTool << endreq;
    return StatusCode::FAILURE;
  }

// Get the extrapolator
  if ( m_extrapolator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
  }

  if ( m_beamConditionsService.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Failed to retrieve service " << m_beamConditionsService << endreq;
  } else {
    msg(MSG::INFO) << "Retrieved service " << m_beamConditionsService << endreq;
  }

// Get the helpertool from ToolSvc
  if ( m_helpertool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_helpertool << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_helpertool << endreq;
  }

// Get the track selector tool from ToolSvc
  if ( m_trkSelector.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSelector << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_trkSelector << endreq;
  }

// Get the vertex point estimator tool from ToolSvc
  if ( m_vertexEstimator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_vertexEstimator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_vertexEstimator << endreq;
  }

  const HepPDT::ParticleData* pd_pi = m_particleDataTable->particle(PDG::pi_plus);
  const HepPDT::ParticleData* pd_p  = m_particleDataTable->particle(PDG::p_plus);
  const HepPDT::ParticleData* pd_e  = m_particleDataTable->particle(PDG::e_minus);
  const HepPDT::ParticleData* pd_K  = m_particleDataTable->particle(PDG::K_S0);
  const HepPDT::ParticleData* pd_L  = m_particleDataTable->particle(PDG::Lambda0);
  const HepPDT::ParticleData* pd_G  = m_particleDataTable->particle(PDG::gamma);
  if (m_masses == 1) {
   m_masspi     = pd_pi->mass();
   m_massp      = pd_p->mass();
   m_masse      = pd_e->mass();
   m_massK0S    = pd_K->mass();
   m_massLambda = pd_L->mass();
   m_massGamma  = pd_G->mass();
  }

  m_events_processed = 0;
  m_V0s_stored       = 0;
  m_Kshort_stored    = 0;
  m_Lambdabar_stored = 0;
  m_Lambda_stored    = 0;
  m_Gamma_stored     = 0;

  msg(MSG::INFO) << "Initialization successful" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode InDetV0FinderTool::performSearch(xAOD::VertexContainer*& v0Container, xAOD::VertexAuxContainer*& v0AuxContainer,
                                            xAOD::VertexContainer*& ksContainer, xAOD::VertexAuxContainer*& ksAuxContainer,
                                            xAOD::VertexContainer*& laContainer, xAOD::VertexAuxContainer*& laAuxContainer,
                                            xAOD::VertexContainer*& lbContainer, xAOD::VertexAuxContainer*& lbAuxContainer,
                                            xAOD::Vertex* primaryVertex, std::string vertCollName)
{
  StatusCode sc;

  ATH_MSG_DEBUG( "InDetV0FinderTool::performSearch" );
  v0Container = new xAOD::VertexContainer;
  v0AuxContainer = new xAOD::VertexAuxContainer;
  v0Container->setStore(v0AuxContainer);
  ksContainer = new xAOD::VertexContainer;
  ksAuxContainer = new xAOD::VertexAuxContainer;
  ksContainer->setStore(ksAuxContainer);
  laContainer = new xAOD::VertexContainer;
  laAuxContainer = new xAOD::VertexAuxContainer;
  laContainer->setStore(laAuxContainer);
  lbContainer = new xAOD::VertexContainer;
  lbAuxContainer = new xAOD::VertexAuxContainer;
  lbContainer->setStore(lbAuxContainer);

  m_events_processed ++;

// Get the ToolSvc
  IToolSvc* toolsvc;
  sc=service("ToolSvc",toolsvc);
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("Problem loading tool service. v0Candidates will be EMPTY!");
    return StatusCode::SUCCESS;;
  };

// making a concrete fitter for the V0Fitter
  Trk::TrkV0VertexFitter* concreteVertexFitter=0;
  if (m_useV0Fitter) {
    concreteVertexFitter = dynamic_cast<Trk::TrkV0VertexFitter * >(&(*m_iVertexFitter));
    if(concreteVertexFitter == 0) {
      ATH_MSG_FATAL("The vertex fitter passed is not a V0 Vertex Fitter");
      return StatusCode::FAILURE;
    }
  }

// Retrieve track particles from StoreGate
  const xAOD::TrackParticleContainer* TPC(0);
  sc = evtStore()->retrieve(TPC,m_TrkParticleCollection);
  if(sc.isFailure()){
      ATH_MSG_ERROR("No TrackParticle collection with name " << m_TrkParticleCollection << " found in StoreGate!");
      return StatusCode::SUCCESS;;
  } else {
      ATH_MSG_DEBUG("Found TrackParticle collection " << m_TrkParticleCollection << " in StoreGate!");
  }
  ATH_MSG_DEBUG("Track particle container size " <<  TPC->size());

  const xAOD::VertexContainer * vertColl(0);
  if (m_use_vertColl) {
// Retrieve vertex collection for V0 selection
    sc = evtStore()->retrieve(vertColl,vertCollName);
    if (sc.isFailure() ) {
      ATH_MSG_WARNING("No vertex collection with name " << vertCollName << " found in StoreGate!");
      return StatusCode::RECOVERABLE;
    } else {
      ATH_MSG_DEBUG("Found " << vertCollName << " in StoreGate!");
    }
  }

  Amg::Vector3D beamspot = Amg::Vector3D(m_beamConditionsService->beamVtx().position());

// track preselection
  std::vector<const xAOD::TrackParticle*> posTracks; posTracks.clear();
  std::vector<const xAOD::TrackParticle*> negTracks; negTracks.clear();
  const xAOD::Vertex* vx = 0;
  if (m_pv && primaryVertex) vx = primaryVertex;

  if (TPC->size() > 1) {
    xAOD::TrackParticleContainer::const_iterator tpIt;
    for ( tpIt=TPC->begin(); tpIt!=TPC->end(); ++tpIt )
    {
      const xAOD::TrackParticle* TP = (*tpIt);
      double charge = TP->charge();

      if (m_trkSelector->decision(*TP, vx))
      {
        if (m_samesign) {
          posTracks.push_back(TP);
          negTracks.push_back(TP);
        } else {
          if (charge > 0) {
            posTracks.push_back(*tpIt);
          } else {
            negTracks.push_back(*tpIt);
          }
        }
      }
    }
  }
  ATH_MSG_DEBUG("number of tracks passing preselection, positive " << posTracks.size() << " negative " << negTracks.size());

  if (posTracks.size() > 0 && negTracks.size() > 0)
  {

  std::vector<const xAOD::TrackParticle*>::const_iterator tpIt1;
  std::vector<const xAOD::TrackParticle*>::const_iterator tpIt2;
  unsigned int i1 = 0;
  for (tpIt1 = posTracks.begin(); tpIt1 != posTracks.end(); ++tpIt1)
  {
    const xAOD::TrackParticle* TP1 = (*tpIt1);
    uint8_t temp1(0);
    uint8_t nclus1(0);
    if( TP1->summaryValue( temp1 , xAOD::numberOfPixelHits) ) nclus1 += temp1;
    if( TP1->summaryValue( temp1 , xAOD::numberOfSCTHits)   ) nclus1 += temp1; 
    double pt1 = TP1->pt();

    unsigned int i2 = 0;
    for (tpIt2 = negTracks.begin(); tpIt2 != negTracks.end(); ++tpIt2)
    {
      if (*tpIt1 == *tpIt2) continue;
      if (m_samesign && i1 <= i2) continue;

      const xAOD::TrackParticle* TP2 = (*tpIt2);
      uint8_t temp2(0);
      uint8_t nclus2(0);
      if( TP2->summaryValue( temp2 , xAOD::numberOfPixelHits) ) nclus2 += temp2;
      if( TP2->summaryValue( temp2 , xAOD::numberOfSCTHits)   ) nclus2 += temp2; 

      if (!m_useTRTplusTRT && nclus1 == 0 && nclus2 == 0) continue;
      if (!m_useTRTplusSi && (nclus1 == 0 || nclus2 == 0)) continue;


//      const Trk::Perigee* perBS1;
//      perBS1 = m_trackToVertexTool->perigeeAtBeamspot( *TP1 );
//      double d0BS_1 = perBS1->parameters()[Trk::d0];
//      const Trk::Perigee* per1;
//      per1 = m_trackToVertexTool->perigeeAtVertex( *TP1, primaryVertex->position() );
//      double d0_1 = per1->parameters()[Trk::d0];
//      //const AmgSymMatrix(5)* cov_tmp1 = per1->covariance();
//      //double d0err_1 = (*cov_tmp1)(0,0);
//      double d0err_1 = sqrt((*per1->covariance())(0,0));
//  ATH_MSG_INFO("d0BS_1 " << d0BS_1 << " d0 " << TP1->d0() << " d0_1 " << d0_1 << " d0err_1 " << d0err_1);

      double pt2 = TP2->pt();
      bool trk_cut1 = false;
      bool trk_cut2 = false;
      if (nclus1 != 0) trk_cut1 = true;
      if (nclus1 == 0 && pt1 >= m_ptTRT) trk_cut1 = true;
      if (!trk_cut1) continue;      
      if (nclus2 != 0) trk_cut2 = true;
      if (nclus2 == 0 && pt2 >= m_ptTRT) trk_cut2 = true;
      if (!trk_cut2) continue;      

      bool d0wrtVertex = true;
      if (m_use_vertColl) {
        if ( !d0Pass(TP1,TP2,vertColl) ) d0wrtVertex = false;
      }
      if (!m_use_vertColl && m_pv) {
        if (primaryVertex) {
          if ( !d0Pass(TP1,TP2,primaryVertex) ) d0wrtVertex = false;
        } else {
          if ( !d0Pass(TP1,TP2,beamspot) ) d0wrtVertex = false;
        }
      }
      if (!d0wrtVertex) continue;

      bool usepair = false;
      if (!m_useorigin) usepair = true;
      if (m_useorigin && TP1->vertex() == 0 && TP2->vertex() == 0) usepair = true;
      if (!usepair) continue;      

// find a starting point
        const Trk::Perigee& aPerigee1 = TP1->perigeeParameters();
        const Trk::Perigee& aPerigee2 = TP2->perigeeParameters();
        int sflag = 0;
        int errorcode = 0;
        Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
        if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
        bool errorCode = false;
        if (errorcode == 0) errorCode = true;
        if (errorcode == 5 && m_useerrorcode5) errorCode = true;
        if (errorCode)
        {

          //bool is_TRT = false;
          if (nclus1 == 0 || nclus2 == 0) {
            //is_TRT = true;
            m_rmax    = m_rmax_t;
            m_pt      = m_pt_t;
          } else {
            m_rmax    = m_rmax_s;
            m_pt      = m_pt_s;
          }

// pair pre-selection cuts
          if ( doFit(TP1, TP2, startingPoint, primaryVertex) )
          {

            Trk::Vertex vertex(startingPoint);
            std::vector<const xAOD::TrackParticle*> pairV0;
            pairV0.clear();
            pairV0.push_back(TP1);
            pairV0.push_back(TP2);

// vertex fit
            ATH_MSG_DEBUG("unconstrained fit now");

            xAOD::Vertex* myVxCandidate(0);
            if (m_useV0Fitter) {
              myVxCandidate = concreteVertexFitter->fit(pairV0, vertex);
            } else {
              myVxCandidate = m_iVKVertexFitter->fit(pairV0, vertex);
            }
      
              if(myVxCandidate != 0)
              {
                myVxCandidate->setVertexType(xAOD::VxType::V0Vtx);

                if (nclus1 == 0 && nclus2 == 0) {                // TRT + TRT
                    m_hlxymin = m_hlxymin_tt;
                    m_hlxymax = m_hlxymax_tt;
                    m_upt     = m_upt_t;
                    m_kspt    = m_kspt_t;
                    m_lapt    = m_lapt_t;
                    m_gapt    = m_gapt_t;
                    m_ulxy    = m_ulxy_t;
                    m_lalxy   = m_lalxy_t;
                    m_kslxy   = m_kslxy_t;
                    m_galxy   = m_galxy_t;
                } else if (nclus1 != 0 && nclus2 != 0) {         // Si + Si
                    m_hlxymin = m_hlxymin_ss;
                    m_hlxymax = m_hlxymax_ss;
                    m_upt     = m_upt_s;
                    m_kspt    = m_kspt_s;
                    m_lapt    = m_lapt_s;
                    m_gapt    = m_gapt_s;
                    m_ulxy    = m_ulxy_s;
                    m_lalxy   = m_lalxy_s;
                    m_kslxy   = m_kslxy_s;
                    m_galxy   = m_galxy_s;
                } else {                                         // Si + TRT
                    m_hlxymin = m_hlxymin_st;
                    m_hlxymax = m_hlxymax_st;
                    m_upt     = m_upt_t;
                    m_kspt    = m_kspt_t;
                    m_lapt    = m_lapt_t;
                    m_gapt    = m_gapt_t;
                    m_ulxy    = m_ulxy_t;
                    m_lalxy   = m_lalxy_t;
                    m_kslxy   = m_kslxy_t;
                    m_galxy   = m_galxy_t;
                }


                double chi2 = myVxCandidate->chiSquared();
                if (chi2 <= 20.)
                {

                  double mass_Ks = m_V0Tools->invariantMass(myVxCandidate,m_masspi,m_masspi);
                  double mass_La = m_V0Tools->invariantMass(myVxCandidate,m_massp,m_masspi);
                  double mass_Lb = m_V0Tools->invariantMass(myVxCandidate,m_masspi,m_massp);
                  if ((mass_Ks >= m_ksmin && mass_Ks <= m_ksmax) || (mass_La >= m_lamin && mass_La <= m_lamax) || (mass_Lb >= m_lamin && mass_Lb <= m_lamax))
                  {

                    m_V0s_stored++;
                    v0Container->push_back(myVxCandidate);

                    static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0LinksDecor("V0Link");
                    static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_ksLinksDecor("KshortLink");
                    static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_laLinksDecor("LambdaLink");
                    static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_lbLinksDecor("LambdabarLink");
                    v0_ksLinksDecor(*myVxCandidate) = ElementLink<xAOD::VertexContainer>();
                    v0_laLinksDecor(*myVxCandidate) = ElementLink<xAOD::VertexContainer>();
                    v0_lbLinksDecor(*myVxCandidate) = ElementLink<xAOD::VertexContainer>();
                    ElementLink<xAOD::VertexContainer> v0Link;

                    Trk::Vertex vertex0(m_V0Tools->vtx(myVxCandidate));

                    xAOD::Vertex* myKshort(0);
                    xAOD::Vertex* myLambda(0);
                    xAOD::Vertex* myLambdabar(0);
                    if (mass_Ks >= m_ksmin && mass_Ks <= m_ksmax) {
                      std::vector<double> masses;
                      masses.push_back(m_masspi);
                      masses.push_back(m_masspi);
                      if (m_useV0Fitter) {
                        myKshort = concreteVertexFitter->fit(pairV0, masses, m_massK0S, 0, vertex);
                      } else {
                        myKshort = m_iKshortFitter->fit(pairV0, vertex);
                      }
                      if(myKshort != 0) {
                        double chi2_ks = myKshort->chiSquared();
                        if (chi2_ks <= 20.) {
                          m_Kshort_stored++;
                          ksContainer->push_back(myKshort);

                          v0Link.setElement(myVxCandidate);
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*myKshort) = v0Link;

                          ElementLink<xAOD::VertexContainer> ksLink;
                          ksLink.setElement(myKshort);
                          ksLink.setStorableObject(*ksContainer);
                          v0_ksLinksDecor(*myVxCandidate) = ksLink;


                        }
                      } else {
                        delete myKshort;
                      }

                    }

                    if (mass_La >= m_lamin && mass_La <= m_lamax) {
                      std::vector<double> masses;
                      masses.push_back(m_massp);
                      masses.push_back(m_masspi);
                      if (m_useV0Fitter) {
                        myLambda = concreteVertexFitter->fit(pairV0, masses, m_massLambda, 0, vertex);
                      } else {
                        myLambda = m_iLambdaFitter->fit(pairV0, vertex);
                      }
                      if(myLambda != 0) {
                        double chi2_la = myLambda->chiSquared();
                        if (chi2_la <= 20.) {
                          m_Lambda_stored++;
                          laContainer->push_back(myLambda);

                          v0Link.setElement(myVxCandidate);
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*myLambda) = v0Link;

                          ElementLink<xAOD::VertexContainer> laLink;
                          laLink.setElement(myLambda);
                          laLink.setStorableObject(*laContainer);
                          v0_laLinksDecor(*myVxCandidate) = laLink;

                        }
                      } else {
                        delete myLambda;
                      }

                    }

                    if (mass_Lb >= m_lamin && mass_Lb <= m_lamax) {
                      std::vector<double> masses;
                      masses.push_back(m_masspi);
                      masses.push_back(m_massp);
                      if (m_useV0Fitter) {
                        myLambdabar = concreteVertexFitter->fit(pairV0, masses, m_massLambda, 0, vertex);
                      } else {
                        myLambdabar = m_iLambdabarFitter->fit(pairV0, vertex);
                      }
                      if(myLambdabar != 0) {
                        double chi2_lb = myLambdabar->chiSquared();
                        if (chi2_lb <= 20.) {
                          m_Lambdabar_stored++;
                          lbContainer->push_back(myLambdabar);

                          v0Link.setElement(myVxCandidate);
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*myLambdabar) = v0Link;

                          ElementLink<xAOD::VertexContainer> lbLink;
                          lbLink.setElement(myLambdabar);
                          lbLink.setStorableObject(*lbContainer);
                          v0_lbLinksDecor(*myVxCandidate) = lbLink;

                        }
                      } else {
                        delete myLambdabar;
                      }

                    }


                  } else {
                    delete myVxCandidate;
                  }

                } else { // chi2 cut failed
                  delete myVxCandidate;
                }
              } else { // fit failed
                ATH_MSG_DEBUG("Fitter failed!");
                delete myVxCandidate;
              }

          }  // doFit
        }  // no suitable starting point found

      i2++;
    }  // loop over negative tracks
    i1++;
  }  // loop over positive tracks

  }  // posTracks.size() > 0 && negTracks.size() > 0

  if ( v0Container->size()==0) ATH_MSG_DEBUG("No Candidates found. Empty container returned");
  if ( ksContainer->size()==0) ATH_MSG_DEBUG("No Kshort Candidates found. Empty container returned");
  if ( laContainer->size()==0) ATH_MSG_DEBUG("No Lambda Candidates found. Empty container returned");
  if ( lbContainer->size()==0) ATH_MSG_DEBUG("No Lambdabar Candidates found. Empty container returned");

  return StatusCode::SUCCESS;
}

StatusCode InDetV0FinderTool::finalize()
{
  msg(MSG::DEBUG)
    << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq
    << "\tSummary" << endreq 
    << "\tProcessed              : " << m_events_processed            << " events" << endreq
    << "\tStored                 : " << m_V0s_stored                  << " V0s" << endreq
    << "\tof which               : " << m_Kshort_stored               << " Kshorts" << endreq
    << "\t                       : " << m_Lambda_stored               << " Lambdas" << endreq
    << "\t                       : " << m_Lambdabar_stored            << " Lambdabars" << endreq;
  msg(MSG::INFO) << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq;

  return StatusCode::SUCCESS;
}

void InDetV0FinderTool::SGError(std::string errService)
{
  msg(MSG::FATAL) << errService << " not found. Exiting !" << endreq;
  return;
}


bool InDetV0FinderTool::doFit(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D &startingPoint, xAOD::Vertex* primaryVertex)
{
  bool pass = false;
  double slxy = 0.;
  if (primaryVertex) {
    slxy = (startingPoint-primaryVertex->position()).perp();
  } else {
    slxy = startingPoint.perp();
  }

  if (startingPoint.perp() > m_minsxy && startingPoint.perp() < m_maxsxy && slxy > m_minslxy && slxy < m_maxslxy)
  {
    double massKshort_i=2001., massLambda_i=2001., massLambdabar_i=2001., pt_i=2001.;
    Amg::Vector3D globalPosition = startingPoint;
    Trk::PerigeeSurface perigeeSurface(globalPosition);
    const Trk::TrackParameters* extrapolatedPerigee1 = m_extrapolator->extrapolate(*track1, perigeeSurface);
    const Trk::TrackParameters* extrapolatedPerigee2 = m_extrapolator->extrapolate(*track2, perigeeSurface);

    if (extrapolatedPerigee1 != 0 && extrapolatedPerigee2 != 0) {
      massKshort_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_masspi,m_masspi);
      massLambda_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_massp,m_masspi);
      massLambdabar_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_masspi,m_massp);
      pt_i = getPt(extrapolatedPerigee1,extrapolatedPerigee2);
      if ( ((massKshort_i >= m_ksmin && massKshort_i <= m_ksmax) ||
            (massLambda_i >= m_lamin && massLambda_i <= m_lamax) ||
            (massLambdabar_i >= m_lamin && massLambdabar_i <= m_lamax)) &&
           pt_i > m_pt ) pass = true;
    }
    delete extrapolatedPerigee1; delete extrapolatedPerigee2;
  }

  return pass;
}

bool InDetV0FinderTool::d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::VertexContainer * vertColl)
{
  bool pass = false;
  xAOD::VertexContainer::const_iterator vItr = vertColl->begin();
  for ( vItr=vertColl->begin(); vItr!=vertColl->end(); ++vItr )
  {
    const xAOD::Vertex* PV = (*vItr);
    const Trk::Perigee* per1 = m_trackToVertexTool->perigeeAtVertex( *track1, PV->position() );
    const Trk::Perigee* per2 = m_trackToVertexTool->perigeeAtVertex( *track2, PV->position() );
    double d0_1 = per1->parameters()[Trk::d0];
    double sig_d0_1 = sqrt((*per1->covariance())(0,0));
    double d0_2 = per2->parameters()[Trk::d0];
    double sig_d0_2 = sqrt((*per2->covariance())(0,0));
    if (fabs(d0_1/sig_d0_1) > m_d0_cut && fabs(d0_2/sig_d0_2) > m_d0_cut) pass = true; 
    delete per1;
    delete per2;
  }
  return pass;
}

bool InDetV0FinderTool::d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, const xAOD::Vertex* PV)
{
  bool pass = false;
  const Trk::Perigee* per1 = m_trackToVertexTool->perigeeAtVertex( *track1, PV->position() );
  const Trk::Perigee* per2 = m_trackToVertexTool->perigeeAtVertex( *track2, PV->position() );
  double d0_1 = per1->parameters()[Trk::d0];
  double sig_d0_1 = sqrt((*per1->covariance())(0,0));
  double d0_2 = per2->parameters()[Trk::d0];
  double sig_d0_2 = sqrt((*per2->covariance())(0,0));
  if (fabs(d0_1/sig_d0_1) > m_d0_cut && fabs(d0_2/sig_d0_2) > m_d0_cut) pass = true; 
  delete per1;
  delete per2;
  return pass;
}

bool InDetV0FinderTool::d0Pass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D PV)
{
  bool pass = false;
  const Trk::Perigee* per1 = m_trackToVertexTool->perigeeAtVertex( *track1, PV );
  const Trk::Perigee* per2 = m_trackToVertexTool->perigeeAtVertex( *track2, PV );
  double d0_1 = per1->parameters()[Trk::d0];
  double sig_d0_1 = sqrt((*per1->covariance())(0,0));
  double d0_2 = per2->parameters()[Trk::d0];
  double sig_d0_2 = sqrt((*per2->covariance())(0,0));
  if (fabs(d0_1/sig_d0_1) > m_d0_cut && fabs(d0_2/sig_d0_2) > m_d0_cut) pass = true; 
  delete per1;
  delete per2;
  return pass;
}

double InDetV0FinderTool::invariantMass(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2, double &m1, double &m2)
{
  double e1sq = per1->momentum().mag2() + m1*m1;
  double e1 = (e1sq>0.) ? sqrt(e1sq) : 0.;
  double e2sq = per2->momentum().mag2() + m2*m2;
  double e2 = (e2sq>0.) ? sqrt(e2sq) : 0.;
  double p = (per1->momentum()+per2->momentum()).mag();
  double msq = (e1+e2+p)*(e1+e2-p);
  double mass = (msq>0.) ? sqrt(msq) : 0.;
  return mass;
}

double InDetV0FinderTool::getPt(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2) {

  Amg::Vector3D momentum( per1->momentum() + per2->momentum() );
  return momentum.perp();

}


}//end of namespace InDet

