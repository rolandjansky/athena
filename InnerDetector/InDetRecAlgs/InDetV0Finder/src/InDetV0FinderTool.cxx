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
  m_d0_cut(2.),
  m_vert_lxy_sig(2.),
  m_vert_lxy_cut(500.),
  m_vert_a0xy_cut(3.),
  m_vert_a0z_cut(15.),
  m_beamConditionsService("BeamCondSvc", n),
  m_TrkParticleCollection("TrackParticleCandidate")
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
}

InDetV0FinderTool::~InDetV0FinderTool() {}

StatusCode InDetV0FinderTool::initialize()
{
  StatusCode sc;

// Get the right vertex fitting tool from ToolSvc 
  if (m_useV0Fitter) {
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endmsg;
    }
  
    if ( m_iGammaFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iGammaFitter << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iGammaFitter << endmsg;
    }

  } else {
    if ( m_iVKVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVKVertexFitter << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iVKVertexFitter << endmsg;
    }

// Get the VKalVrt Ks vertex fitting tool from ToolSvc
    if ( m_iKshortFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iKshortFitter << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iKshortFitter << endmsg;
    }

// Get the VKalVrt Lambda vertex fitting tool from ToolSvc
    if ( m_iLambdaFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iLambdaFitter << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iLambdaFitter << endmsg;
    }

// Get the VKalVrt Lambdabar vertex fitting tool from ToolSvc
    if ( m_iLambdabarFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iLambdabarFitter << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iLambdabarFitter << endmsg;
    }

// Get the VKalVrt Gamma vertex fitting tool from ToolSvc
    if ( m_iGammaFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iGammaFitter << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_iGammaFitter << endmsg;
    }

  }

// get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  sc = service("PartPropSvc", partPropSvc, true);
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not initialize Particle Properties Service" << endmsg;
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

// uploading the V0 tools
  if ( m_V0Tools.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_V0Tools << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endmsg;
  }

// Get the TrackToVertex extrapolator tool
  if ( m_trackToVertexTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackToVertexTool << endmsg;
    return StatusCode::FAILURE;
  }

// Get the extrapolator
  if ( m_extrapolator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
  }

  if ( m_beamConditionsService.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Failed to retrieve service " << m_beamConditionsService << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved service " << m_beamConditionsService << endmsg;
  }

// Get the helpertool from ToolSvc
  if ( m_helpertool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_helpertool << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_helpertool << endmsg;
  }

// Get the track selector tool from ToolSvc
  if ( m_trkSelector.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSelector << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_trkSelector << endmsg;
  }

// Get the vertex point estimator tool from ToolSvc
  if ( m_vertexEstimator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_vertexEstimator << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_vertexEstimator << endmsg;
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

  msg(MSG::INFO) << "Initialization successful" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode InDetV0FinderTool::performSearch(xAOD::VertexContainer*& v0Container, xAOD::VertexAuxContainer*& v0AuxContainer,
                                            xAOD::VertexContainer*& ksContainer, xAOD::VertexAuxContainer*& ksAuxContainer,
                                            xAOD::VertexContainer*& laContainer, xAOD::VertexAuxContainer*& laAuxContainer,
                                            xAOD::VertexContainer*& lbContainer, xAOD::VertexAuxContainer*& lbAuxContainer,
                                            const xAOD::Vertex* primaryVertex, std::string vertCollName)
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
        if (errorcode == 0 || errorcode == 5 || errorcode == 6 || errorcode == 8) errorCode = true;
        if (errorCode)
        {

// pair pre-selection cuts
          if ( doFit(TP1,TP2,startingPoint) )
          {
            std::vector<const xAOD::TrackParticle*> pairV0;
            pairV0.clear();
            pairV0.push_back(TP1);
            pairV0.push_back(TP2);

// vertex fit
            ATH_MSG_DEBUG("unconstrained fit now");

            std::unique_ptr<xAOD::Vertex> myVxCandidate;
            if (m_useV0Fitter) {
              myVxCandidate = std::unique_ptr<xAOD::Vertex>( concreteVertexFitter->fit(pairV0, startingPoint) );
            } else {
              myVxCandidate = std::unique_ptr<xAOD::Vertex>(  m_iVKVertexFitter->fit(pairV0, startingPoint) );
            }
      
              if (myVxCandidate)
              {
                myVxCandidate->setVertexType(xAOD::VxType::V0Vtx);
                if ( m_V0Tools->vertexProbability(myVxCandidate.get()) >= m_minVertProb )
                {
                  bool doKshortFit = false;
                  doKshortFit = doMassFit(myVxCandidate.get(),310);
                  bool doLambdaFit = false;
                  doLambdaFit = doMassFit(myVxCandidate.get(),3122);
                  bool doLambdabarFit = false;
                  doLambdabarFit = doMassFit(myVxCandidate.get(),-3122);
                  if (doKshortFit || doLambdaFit || doLambdabarFit)
                  {
                    bool pointAtVert = true;
                    if (m_use_vertColl) {
                      if ( !pointAtVertexColl(myVxCandidate.get(),vertColl) ) pointAtVert = false;
                    }
                    if (!m_use_vertColl && m_pv && primaryVertex) {
                      if ( !pointAtVertex(myVxCandidate.get(),primaryVertex) ) pointAtVert = false;
                    }
                    if (m_doSimpleV0) pointAtVert = true;
                    if (pointAtVert)
                    {
                      Amg::Vector3D vertex = m_V0Tools->vtx(myVxCandidate.get());

                      std::unique_ptr<xAOD::Vertex> myKshort;
                      std::unique_ptr<xAOD::Vertex> myLambda;
                      std::unique_ptr<xAOD::Vertex> myLambdabar;
		      std::unique_ptr<xAOD::Vertex> myGamma;
                      bool foundKshort = false;
                      bool foundLambda = false;
                      bool foundLambdabar = false;

                      if (doKshortFit && !m_doSimpleV0) {
                        myKshort = std::unique_ptr<xAOD::Vertex>( massFit(310, pairV0, vertex, concreteVertexFitter) );
                        if (myKshort) {
                          if (m_V0Tools->vertexProbability(myKshort.get()) >= m_minConstrVertProb) {
                            myKshort->setVertexType(xAOD::VxType::V0Vtx);
                            //myKshort->setVertexType(xAOD::VxType::V0KShort);
                            foundKshort = true;
                          }
                        }
                      }

                      if (doLambdaFit && !m_doSimpleV0) {
                        myLambda = std::unique_ptr<xAOD::Vertex>( massFit(3122, pairV0, vertex, concreteVertexFitter) );
                        if (myLambda) {
                          if (m_V0Tools->vertexProbability(myLambda.get()) >= m_minConstrVertProb) {
                            myLambda->setVertexType(xAOD::VxType::V0Vtx);
                            foundLambda = true;
                          }
                        }
                      }

                      if (doLambdabarFit && !m_doSimpleV0) {
                        myLambdabar = std::unique_ptr<xAOD::Vertex>( massFit(-3122, pairV0, vertex, concreteVertexFitter));
                        if (myLambdabar) {
                          if (m_V0Tools->vertexProbability(myLambdabar.get()) >= m_minConstrVertProb) {
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
                      ElementLink<xAOD::VertexContainer> ksLink;
                      static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_ksLinksDecor("KshortLink");
                      ElementLink<xAOD::VertexContainer> laLink;
                      static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_laLinksDecor("LambdaLink");
                      ElementLink<xAOD::VertexContainer> lbLink;
                      static SG::AuxElement::Decorator< ElementLink<xAOD::VertexContainer> > v0_lbLinksDecor("LambdabarLink");

                      if (m_doSimpleV0 || (!m_doSimpleV0 && doGamma)) {
                        m_V0s_stored++;
                        myVxCandidate->clearTracks();
                        ElementLink<xAOD::TrackParticleContainer> newLink1;
                        newLink1.setElement(*tpIt1);
                        newLink1.setStorableObject(*TPC);
                        ElementLink<xAOD::TrackParticleContainer> newLink2;
                        newLink2.setElement(*tpIt2);
                        newLink2.setStorableObject(*TPC);
                        myVxCandidate->addTrackAtVertex(newLink1);
                        myVxCandidate->addTrackAtVertex(newLink2);
                        v0Container->push_back(myVxCandidate.release());

                        if (foundKshort && !m_doSimpleV0) {
                          m_Kshort_stored++;
                          myKshort->clearTracks();
                          ElementLink<xAOD::TrackParticleContainer> ksLink1;
                          ksLink1.setElement(*tpIt1);
                          ksLink1.setStorableObject(*TPC);
                          ElementLink<xAOD::TrackParticleContainer> ksLink2;
                          ksLink2.setElement(*tpIt2);
                          ksLink2.setStorableObject(*TPC);
                          myKshort->addTrackAtVertex(ksLink1);
                          myKshort->addTrackAtVertex(ksLink2);
                          ksContainer->push_back(myKshort.release());

                          v0Link.setElement(v0Container->back());
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*(ksContainer->back())) = v0Link;

                          ksLink.setElement(ksContainer->back());
                          ksLink.setStorableObject(*ksContainer);
                          v0_ksLinksDecor(*(v0Container->back())) = ksLink;
                        } else {
                          v0_ksLinksDecor(*(v0Container->back())) = ksLink;
                        }
                        if (foundLambda && !m_doSimpleV0) {
                          m_Lambda_stored++;
                          myLambda->clearTracks();
                          ElementLink<xAOD::TrackParticleContainer> laLink1;
                          laLink1.setElement(*tpIt1);
                          laLink1.setStorableObject(*TPC);
                          ElementLink<xAOD::TrackParticleContainer> laLink2;
                          laLink2.setElement(*tpIt2);
                          laLink2.setStorableObject(*TPC);
                          myLambda->addTrackAtVertex(laLink1);
                          myLambda->addTrackAtVertex(laLink2);
                          laContainer->push_back(myLambda.release());

                          v0Link.setElement(v0Container->back());
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*(laContainer->back())) = v0Link;

                          laLink.setElement(laContainer->back());
                          laLink.setStorableObject(*laContainer);
                          v0_laLinksDecor(*(v0Container->back())) = laLink;
                        } else {
                          v0_laLinksDecor(*(v0Container->back())) = laLink;
                        }
                        if (foundLambdabar && !m_doSimpleV0) {
                          m_Lambdabar_stored++;
                          myLambdabar->clearTracks();
                          ElementLink<xAOD::TrackParticleContainer> lbLink1;
                          lbLink1.setElement(*tpIt1);
                          lbLink1.setStorableObject(*TPC);
                          ElementLink<xAOD::TrackParticleContainer> lbLink2;
                          lbLink2.setElement(*tpIt2);
                          lbLink2.setStorableObject(*TPC);
                          myLambdabar->addTrackAtVertex(lbLink1);
                          myLambdabar->addTrackAtVertex(lbLink2);
                          lbContainer->push_back(myLambdabar.release());

                          v0Link.setElement(v0Container->back());
                          v0Link.setStorableObject(*v0Container);
                          v0LinksDecor(*(lbContainer->back())) = v0Link;

                          lbLink.setElement(lbContainer->back());
                          lbLink.setStorableObject(*lbContainer);
                          v0_lbLinksDecor(*(v0Container->back())) = lbLink;
                        } else {
                          v0_lbLinksDecor(*(v0Container->back())) = lbLink;
                        }
                        if (doGamma && !m_doSimpleV0) {
		          myGamma = std::unique_ptr<xAOD::Vertex>( massFit(22, pairV0, vertex, concreteVertexFitter) );
                          if (myGamma && m_V0Tools->vertexProbability(myGamma.get()) >= m_minConstrVertProb) {
                            gamma_fit = 1;
                            gamma_prob = m_V0Tools->vertexProbability(myGamma.get());
                            gamma_mass = m_V0Tools->invariantMass(myGamma.get(),m_masse,m_masse);
                            gamma_massErr = m_V0Tools->invariantMassError(myGamma.get(),m_masse,m_masse);
                          }
                          SG::AuxElement::Decorator<int> mDecor_gfit("gamma_fit");
                          SG::AuxElement::Decorator<float> mDecor_gmass("gamma_mass");
                          SG::AuxElement::Decorator<float> mDecor_gmasserr("gamma_massError");
                          SG::AuxElement::Decorator<float> mDecor_gprob("gamma_probability");
                          mDecor_gfit( *(v0Container->back()) ) = gamma_fit;
                          mDecor_gmass( *(v0Container->back()) ) = gamma_mass;
                          mDecor_gmasserr( *(v0Container->back()) ) = gamma_massErr;
                          mDecor_gprob( *(v0Container->back()) ) = gamma_prob;
                        }
                      }

                    } // pointAtVert

                  } // in mass window (doMassFit)

                } // chi2 cut failed
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
    << "----------------------------------------------------------------------------------------------------------------------------------------------" << endmsg
    << "\tSummary" << endmsg 
    << "\tProcessed              : " << m_events_processed            << " events" << endmsg
    << "\tStored                 : " << m_V0s_stored                  << " V0s" << endmsg
    << "\tof which               : " << m_Kshort_stored               << " Kshorts" << endmsg
    << "\t                       : " << m_Lambda_stored               << " Lambdas" << endmsg
    << "\t                       : " << m_Lambdabar_stored            << " Lambdabars" << endmsg;
  msg(MSG::INFO) << "----------------------------------------------------------------------------------------------------------------------------------------------" << endmsg;

  return StatusCode::SUCCESS;
}

void InDetV0FinderTool::SGError(std::string errService)
{
  msg(MSG::FATAL) << errService << " not found. Exiting !" << endmsg;
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
    std::vector<std::unique_ptr<const Trk::TrackParameters> >  cleanup;
    const Trk::TrackParameters* extrapolatedPerigee1(0);
    const Trk::TrackParameters* extrapolatedPerigee2(0);
    extrapolatedPerigee1 = m_extrapolator->extrapolate(track1->perigeeParameters(), perigeeSurface);
    if (extrapolatedPerigee1 == 0) extrapolatedPerigee1 = &track1->perigeeParameters();
    else cleanup.push_back(std::unique_ptr<const Trk::TrackParameters>(extrapolatedPerigee1));

    extrapolatedPerigee2 = m_extrapolator->extrapolate(track2->perigeeParameters(), perigeeSurface);
    if (extrapolatedPerigee2 == 0) extrapolatedPerigee2 = &track2->perigeeParameters();
    else cleanup.push_back(std::unique_ptr<const Trk::TrackParameters>(extrapolatedPerigee2));

    if (extrapolatedPerigee1 != 0 && extrapolatedPerigee2 != 0) {
      massKshort_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_masspi,m_masspi);
      massLambda_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_massp,m_masspi);
      massLambdabar_i = invariantMass(extrapolatedPerigee1,extrapolatedPerigee2,m_masspi,m_massp);
      if ( ((massKshort_i >= m_uksmin && massKshort_i <= m_uksmax) ||
            (massLambda_i >= m_ulamin && massLambda_i <= m_ulamax) ||
            (massLambdabar_i >= m_ulamin && massLambdabar_i <= m_ulamax)) ) pass = true;
    }
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
    if (per1 == 0) return pass;
    const Trk::Perigee* per2 = m_trackToVertexTool->perigeeAtVertex( *track2, PV->position() );
    if (per2 == 0) {
      delete per1;
      return pass;
    }
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
  if (per1 == 0) return pass;
  const Trk::Perigee* per2 = m_trackToVertexTool->perigeeAtVertex( *track2, PV->position() );
  if (per2 == 0) {
    delete per1;
    return pass;
  }
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
  if (per1 == 0) return pass;
  const Trk::Perigee* per2 = m_trackToVertexTool->perigeeAtVertex( *track2, PV );
  if (per2 == 0) {
    delete per1;
    return pass;
  }
  double d0_1 = per1->parameters()[Trk::d0];
  double sig_d0_1 = sqrt((*per1->covariance())(0,0));
  double d0_2 = per2->parameters()[Trk::d0];
  double sig_d0_2 = sqrt((*per2->covariance())(0,0));
  if (fabs(d0_1/sig_d0_1) > m_d0_cut && fabs(d0_2/sig_d0_2) > m_d0_cut) pass = true; 
  delete per1;
  delete per2;
  return pass;
}

bool InDetV0FinderTool::pointAtVertex(const xAOD::Vertex* v0, const xAOD::Vertex* PV)
{
  bool pass = false;
  double v0lxy = m_V0Tools->lxy(v0,PV);
  double v0lxyError = m_V0Tools->lxyError(v0,PV);
  double cos = -1;
  //if (PV->vxTrackAtVertexAvailable()) {
  //  double prod = m_V0Tools->V0Momentum(v0).dot(m_V0Tools->V0Momentum(PV));
  //  cos = prod/(m_V0Tools->V0Momentum(v0).mag()*m_V0Tools->V0Momentum(PV).mag());
  //} else {
    cos = m_V0Tools->cosTheta(v0,PV);
  //}
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

xAOD::Vertex* InDetV0FinderTool::massFit(int pdgID, std::vector<const xAOD::TrackParticle*> pairV0, Amg::Vector3D vertex, Trk::TrkV0VertexFitter* concreteVertexFitter)
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

