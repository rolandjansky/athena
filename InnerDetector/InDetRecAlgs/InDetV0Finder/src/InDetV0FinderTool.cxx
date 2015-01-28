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
  m_doSimpleV0(false),
  m_useorigin(true),
  m_samesign(false),
  //m_pv(true),
  m_pv(false),
  m_use_vertColl(false),
  m_useTRTplusTRT(false),
  m_useTRTplusSi(false),
  m_useV0Fitter(false),
  m_masses(1),
  m_masspi(139.57),
  m_massp(938.272),
  m_masse(0.510999),
  m_massK0S(497.672),
  m_massLambda(1115.68),
  m_ptTRT(700.),
  m_maxsxy(1000.),
  m_uksmin(400.),
  m_uksmax(600.),
  m_ulamin(1000.),
  m_ulamax(1200.),
  m_ksmin(400.),
  m_ksmax(600.),
  m_lamin(1000.),
  m_lamax(1200.),
  m_errmass(100.),
  m_minVertProb(0.0001),
  m_minConstrVertProb(0.0001),
  m_d0_cut(5.),
  m_vert_lxy_sig(5.),
  m_vert_lxy_cut(500.),
  m_vert_a0xy_cut(3.),
  m_vert_a0z_cut(15.),
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
  declareProperty("doSimpleV0", m_doSimpleV0);
  declareProperty("useorigin", m_useorigin);
  declareProperty("AddSameSign", m_samesign);
  declareProperty("trkSelPV", m_pv);
  declareProperty("useVertexCollection", m_use_vertColl);
  declareProperty("useTRTplusTRT", m_useTRTplusTRT);
  declareProperty("useTRTplusSi", m_useTRTplusSi);
  declareProperty("useV0Fitter", m_useV0Fitter);
  declareProperty("masses", m_masses);
  declareProperty("masspi", m_masspi);
  declareProperty("massp", m_massp);
  declareProperty("masse", m_masse);
  declareProperty("massK0S", m_massK0S);
  declareProperty("massLambda", m_massLambda);
  declareProperty("ptTRT", m_ptTRT);
  declareProperty("maxsxy", m_maxsxy);
  declareProperty("uksmin", m_uksmin);
  declareProperty("uksmax", m_uksmax);
  declareProperty("ulamin", m_ulamin);
  declareProperty("ulamax", m_ulamax);
  declareProperty("ksmin", m_ksmin);
  declareProperty("ksmax", m_ksmax);
  declareProperty("lamin", m_lamin);
  declareProperty("lamax", m_lamax);
  declareProperty("errmass", m_errmass);
  declareProperty("minVertProb", m_minVertProb);
  declareProperty("minConstrVertProb", m_minConstrVertProb);
  declareProperty("d0_cut", m_d0_cut );
  declareProperty("vert_lxy_sig", m_vert_lxy_sig );
  declareProperty("vert_lxy_cut", m_vert_lxy_cut );
  declareProperty("vert_a0xy_cut", m_vert_a0xy_cut );
  declareProperty("vert_a0z_cut", m_vert_a0z_cut );
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
  if ( sc.isFailure() ) {
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
  if (m_masses == 1) {
   m_masspi     = pd_pi->mass();
   m_massp      = pd_p->mass();
   m_masse      = pd_e->mass();
   m_massK0S    = pd_K->mass();
   m_massLambda = pd_L->mass();
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
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Problem loading tool service. v0Candidates will be EMPTY!");
    return StatusCode::SUCCESS;;
  };

// making a concrete fitter for the V0Fitter
  Trk::TrkV0VertexFitter* concreteVertexFitter=0;
  if (m_useV0Fitter) {
    concreteVertexFitter = dynamic_cast<Trk::TrkV0VertexFitter * >(&(*m_iVertexFitter));
    if (concreteVertexFitter == 0) {
      ATH_MSG_FATAL("The vertex fitter passed is not a V0 Vertex Fitter");
      return StatusCode::FAILURE;
    }
  }

// Retrieve track particles from StoreGate
  const xAOD::TrackParticleContainer* TPC(0);
  sc = evtStore()->retrieve(TPC,m_TrkParticleCollection);
  if ( sc.isFailure() ){
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
    if ( sc.isFailure() ) {
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
    if ( TP1->summaryValue( temp1 , xAOD::numberOfPixelHits) ) nclus1 += temp1;
    if ( TP1->summaryValue( temp1 , xAOD::numberOfSCTHits)   ) nclus1 += temp1; 
    double pt1 = TP1->pt();

    unsigned int i2 = 0;
    for (tpIt2 = negTracks.begin(); tpIt2 != negTracks.end(); ++tpIt2)
    {
      if (*tpIt1 == *tpIt2) continue;
      if (m_samesign && i1 <= i2) continue;

      const xAOD::TrackParticle* TP2 = (*tpIt2);
      uint8_t temp2(0);
      uint8_t nclus2(0);
      if ( TP2->summaryValue( temp2 , xAOD::numberOfPixelHits) ) nclus2 += temp2;
      if ( TP2->summaryValue( temp2 , xAOD::numberOfSCTHits)   ) nclus2 += temp2; 
      ATH_MSG_DEBUG("nclus1 " << (int)nclus1 << " nclus2 " << (int)nclus2);

      if (!m_useTRTplusTRT && nclus1 == 0 && nclus2 == 0) continue;
      if (!m_useTRTplusSi && (nclus1 == 0 || nclus2 == 0)) continue;

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
        if (errorcode == 0 || errorcode == 5) errorCode = true;
        if (errorCode)
        {

// pair pre-selection cuts
          if ( doFit(TP1,TP2,startingPoint) )
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
      
              if (myVxCandidate != 0)
              {
                myVxCandidate->setVertexType(xAOD::VxType::V0Vtx);
                if ( m_V0Tools->vertexProbability(myVxCandidate) >= m_minVertProb )
                {
                  bool doKshortFit = false;
                  doKshortFit = doMassFit(myVxCandidate,310);
                  bool doLambdaFit = false;
                  doLambdaFit = doMassFit(myVxCandidate,3122);
                  bool doLambdabarFit = false;
                  doLambdabarFit = doMassFit(myVxCandidate,-3122);
                  if (doKshortFit || doLambdaFit || doLambdabarFit)
                  {
                    bool pointAtVert = true;
                    if (m_use_vertColl) {
                      if ( !pointAtVertexColl(myVxCandidate,vertColl) ) pointAtVert = false;
                    }
                    if (!m_use_vertColl && m_pv && primaryVertex) {
                      if ( !pointAtVertex(myVxCandidate,primaryVertex) ) pointAtVert = false;
                    }
                    if (pointAtVert)
                    {
                      Trk::Vertex vertex0(m_V0Tools->vtx(myVxCandidate));

                      xAOD::Vertex* myKshort(0);
                      xAOD::Vertex* myLambda(0);
                      xAOD::Vertex* myLambdabar(0);
                      xAOD::Vertex* myGamma(0);
                      bool foundKshort = false;
                      bool foundLambda = false;
                      bool foundLambdabar = false;

                      if (doKshortFit && !m_doSimpleV0) {
                        myKshort = massFit(310, pairV0, vertex0, concreteVertexFitter);
                        if (myKshort != 0) {
                          if (m_V0Tools->vertexProbability(myKshort) >= m_minConstrVertProb) {
                            myKshort->setVertexType(xAOD::VxType::V0Vtx);
                            //myKshort->setVertexType(xAOD::VxType::V0KShort);
                            foundKshort = true;
                          }
                        }
                      }

                      if (doLambdaFit && !m_doSimpleV0) {
                        myLambda = massFit(3122, pairV0, vertex0, concreteVertexFitter);
                        if (myLambda != 0) {
                          if (m_V0Tools->vertexProbability(myLambda) >= m_minConstrVertProb) {
                            myLambda->setVertexType(xAOD::VxType::V0Vtx);
                            foundLambda = true;
                          }
                        }
                      }

                      if (doLambdabarFit && !m_doSimpleV0) {
                        myLambdabar = massFit(-3122, pairV0, vertex0, concreteVertexFitter);
                        if (myLambdabar != 0) {
                          if (m_V0Tools->vertexProbability(myLambdabar) >= m_minConstrVertProb) {
                            myLambdabar->setVertexType(xAOD::VxType::V0Vtx);
                            foundLambdabar = true;
                          }
                        }
                      }

                      bool doGamma = false;
                      int gamma_fit = 0;
                      double gamma_prob = -1., gamma_mass = -1., gamma_massErr = -1.;
                      if (foundKshort || foundLambda || foundLambdabar) doGamma = true;

                      ElementLink<xAOD::VertexContainer> v0Link;
                      static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0LinksDecor("V0Link");
                      if (m_doSimpleV0 || (!m_doSimpleV0 && doGamma)) {
                        m_V0s_stored++;
                        v0Container->push_back(myVxCandidate);
                        if (foundKshort && !m_doSimpleV0) {
                          m_Kshort_stored++;
                          ksContainer->push_back(myKshort);

                          v0Link.setElement(myVxCandidate);
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*myKshort) = v0Link;

                          static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_ksLinksDecor("KshortLink");
                          v0_ksLinksDecor(*myVxCandidate) = ElementLink<xAOD::VertexContainer>();
                          ElementLink<xAOD::VertexContainer> ksLink;
                          ksLink.setElement(myKshort);
                          ksLink.setStorableObject(*ksContainer);
                          v0_ksLinksDecor(*myVxCandidate) = ksLink;
                        }
                        if (foundLambda && !m_doSimpleV0) {
                          m_Lambda_stored++;
                          laContainer->push_back(myLambda);

                          v0Link.setElement(myVxCandidate);
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*myLambda) = v0Link;

                          static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_laLinksDecor("LambdaLink");
                          v0_laLinksDecor(*myVxCandidate) = ElementLink<xAOD::VertexContainer>();
                          ElementLink<xAOD::VertexContainer> laLink;
                          laLink.setElement(myLambda);
                          laLink.setStorableObject(*laContainer);
                          v0_laLinksDecor(*myVxCandidate) = laLink;
                        }
                        if (foundLambdabar && !m_doSimpleV0) {
                          m_Lambdabar_stored++;
                          lbContainer->push_back(myLambdabar);

                          v0Link.setElement(myVxCandidate);
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*myLambdabar) = v0Link;

                          static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_lbLinksDecor("LambdabarLink");
                          v0_lbLinksDecor(*myVxCandidate) = ElementLink<xAOD::VertexContainer>();
                          ElementLink<xAOD::VertexContainer> lbLink;
                          lbLink.setElement(myLambdabar);
                          lbLink.setStorableObject(*lbContainer);
                          v0_lbLinksDecor(*myVxCandidate) = lbLink;
                        }
                        if (doGamma && !m_doSimpleV0) {
                          myGamma = massFit(22, pairV0, vertex0, concreteVertexFitter);
                          if (myGamma != 0) {
                            gamma_fit = 1;
                            gamma_prob = m_V0Tools->vertexProbability(myGamma);
                            gamma_mass = m_V0Tools->invariantMass(myGamma,m_masse,m_masse);
                            gamma_massErr = m_V0Tools->invariantMassError(myGamma,m_masse,m_masse);
                            delete myGamma;
                          }
                          SG::AuxElement::Decorator<int> mDecor_gfit("gamma_fit");
                          SG::AuxElement::Decorator<float> mDecor_gmass("gamma_mass");
                          SG::AuxElement::Decorator<float> mDecor_gmasserr("gamma_massError");
                          SG::AuxElement::Decorator<float> mDecor_gprob("gamma_probability");
                          mDecor_gfit( *myVxCandidate ) = gamma_fit;
                          mDecor_gmass( *myVxCandidate ) = gamma_mass;
                          mDecor_gmasserr( *myVxCandidate ) = gamma_massErr;
                          mDecor_gprob( *myVxCandidate ) = gamma_prob;
                        }
                      }

                    } else { // pointAtVert
                      delete myVxCandidate;
                    }

                  } else { // in mass window (doMassFit)
                    delete myVxCandidate;
                  }

                } else { // chi2 cut failed
                  delete myVxCandidate;
                }
              } else { // unconstrained fit failed
                ATH_MSG_DEBUG("Fitter failed!");
                //delete myVxCandidate;
              }

          }  // doFit
        }  // no suitable starting point found

      i2++;
    }  // loop over negative tracks
    i1++;
  }  // loop over positive tracks

  }  // posTracks.size() > 0 && negTracks.size() > 0

  if (v0Container->size()==0) ATH_MSG_DEBUG("No Candidates found. Empty container returned");
  if (ksContainer->size()==0) ATH_MSG_DEBUG("No Kshort Candidates found. Empty container returned");
  if (laContainer->size()==0) ATH_MSG_DEBUG("No Lambda Candidates found. Empty container returned");
  if (lbContainer->size()==0) ATH_MSG_DEBUG("No Lambdabar Candidates found. Empty container returned");

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


bool InDetV0FinderTool::doFit(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2, Amg::Vector3D &startingPoint)
{
  bool pass = false;
  double srxy = startingPoint.perp();
  if (srxy <= m_maxsxy)
  {
    double massKshort_i=2000001., massLambda_i=2000001., massLambdabar_i=2000001.;
    Amg::Vector3D globalPosition = startingPoint;
    Trk::PerigeeSurface perigeeSurface(globalPosition);
    const Trk::TrackParameters* extrapolatedPerigee1 = m_extrapolator->extrapolate(*track1, perigeeSurface);
    const Trk::TrackParameters* extrapolatedPerigee2 = m_extrapolator->extrapolate(*track2, perigeeSurface);

    if (extrapolatedPerigee1 != 0 && extrapolatedPerigee2 != 0) {
      massKshort_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_masspi,m_masspi);
      massLambda_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_massp,m_masspi);
      massLambdabar_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_masspi,m_massp);
      if ( ((massKshort_i >= m_uksmin && massKshort_i <= m_uksmax) ||
            (massLambda_i >= m_ulamin && massLambda_i <= m_ulamax) ||
            (massLambdabar_i >= m_ulamin && massLambdabar_i <= m_ulamax)) ) pass = true;
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

bool InDetV0FinderTool::pointAtVertex(xAOD::Vertex* v0, xAOD::Vertex* PV)
{
  bool pass = false;
  double v0lxy = m_V0Tools->lxy(v0,PV);
  double v0lxyError = m_V0Tools->lxyError(v0,PV);
  double cos = -1;
  if (PV->vxTrackAtVertexAvailable()) {
    double prod = m_V0Tools->V0Momentum(v0).dot(m_V0Tools->V0Momentum(PV));
    cos = prod/(m_V0Tools->V0Momentum(v0).mag()*m_V0Tools->V0Momentum(PV).mag());
  } else {
    cos = m_V0Tools->cosTheta(v0,PV);
  }
  double v0a0xy = m_V0Tools->a0xy(v0,PV);
  double v0a0z = m_V0Tools->a0z(v0,PV);
  if (v0lxy/v0lxyError > m_vert_lxy_sig && cos > 0. &&
      fabs(v0a0xy) < m_vert_a0xy_cut && fabs(v0a0z) < m_vert_a0z_cut &&
      v0lxy < m_vert_lxy_cut) pass = true;
  return pass;
}

bool InDetV0FinderTool::pointAtVertexColl(xAOD::Vertex* v0, const xAOD::VertexContainer * vertColl)
{
  bool pass = false;
  xAOD::VertexContainer::const_iterator vItr = vertColl->begin();
  for ( vItr=vertColl->begin(); vItr!=vertColl->end(); ++vItr ) { if (pointAtVertex(v0,(*vItr))) pass = true; }
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

bool InDetV0FinderTool::doMassFit(xAOD::Vertex* vxCandidate, int pdgID)
{
  bool pass = false;
  double mass = 1000000000.;
  double error = 1000000001.;
  bool in_mass_window = false;
  double winmass_min = 0., winmass_max = 0.;

  if (pdgID == 310) {
    winmass_min = m_ksmin;
    winmass_max = m_ksmax;
    mass = m_V0Tools->invariantMass(vxCandidate,m_masspi,m_masspi);
    error = m_V0Tools->invariantMassError(vxCandidate,m_masspi,m_masspi);
    if (mass >= winmass_min && mass <= winmass_max && error <= m_errmass) in_mass_window = true;
  } else if (pdgID == 3122 || pdgID == -3122) {
    winmass_min = m_lamin;
    winmass_max = m_lamax;
    if (pdgID == 3122) {
      mass = m_V0Tools->invariantMass(vxCandidate,m_massp,m_masspi);
      error = m_V0Tools->invariantMassError(vxCandidate,m_massp,m_masspi);
    } else if (pdgID == -3122) {
      mass = m_V0Tools->invariantMass(vxCandidate,m_masspi,m_massp);
      error = m_V0Tools->invariantMassError(vxCandidate,m_masspi,m_massp);
    }
    if (mass >= winmass_min && mass <= winmass_max && error <= m_errmass) in_mass_window = true;
  }
  if (in_mass_window) pass = true;

  return pass;
}

xAOD::Vertex* InDetV0FinderTool::massFit(int pdgID, std::vector<const xAOD::TrackParticle*> pairV0, Trk::Vertex vertex, Trk::TrkV0VertexFitter* concreteVertexFitter)
{
  xAOD::Vertex* vxCandidate(0);
  std::vector<double> masses;
  if (pdgID == 310) {
    masses.push_back(m_masspi);
    masses.push_back(m_masspi);
  } else if (pdgID == 3122) {
    masses.push_back(m_massp);
    masses.push_back(m_masspi);
  } else if (pdgID == -3122) {
    masses.push_back(m_masspi);
    masses.push_back(m_massp);
  } else if (pdgID == 22) {
    masses.push_back(m_masse);
    masses.push_back(m_masse);
  }

  if (pdgID == 22) {
    vxCandidate = m_iGammaFitter->fit(pairV0, vertex);
  }
  if (pdgID ==   310) {
    if (m_useV0Fitter) {
      vxCandidate = concreteVertexFitter->fit(pairV0, masses, m_massK0S, 0, vertex);
    } else {
      vxCandidate = m_iKshortFitter->fit(pairV0, vertex);
    }
  }
  if (pdgID ==  3122) {
    if (m_useV0Fitter) {
      vxCandidate = concreteVertexFitter->fit(pairV0, masses, m_massLambda, 0, vertex);
    } else {
      vxCandidate = m_iLambdaFitter->fit(pairV0, vertex);
    }
  }
  if (pdgID == -3122) {
    if (m_useV0Fitter) {
      vxCandidate = concreteVertexFitter->fit(pairV0, masses, m_massLambda, 0, vertex);
    } else {
      vxCandidate = m_iLambdabarFitter->fit(pairV0, vertex);
    }
  }

  return vxCandidate;
}

}//end of namespace InDet

