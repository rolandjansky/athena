/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetV0Finder.cxx  -  Description
                             -------------------
    begin   : 20-07-2005
    authors : Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes : June 1 2006
    author  : Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
              Changed to use the ExtendedVxCandidate..

 ***************************************************************************/

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/VxContainer.h"
#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkV0Vertex/V0Candidate.h"
#include "TrkV0Vertex/V0Container.h"
#include "InDetV0Finder/InDetV0Finder.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/Vertex.h"
#include "VxVertex/RecVertex.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh" // for chi2prob calculation
#include <vector>
#include <cmath>

namespace InDet
{
  
InDetV0Finder::InDetV0Finder(const std::string &n, ISvcLocator *pSvcLoc)
  :
  AthAlgorithm(n, pSvcLoc),
  m_iVertexFitter("Trk::V0VertexFitter"),
  m_iVKVertexFitter("Trk::TrkVKalVrtFitter"),
  m_iKshortFitter("Trk::TrkVKalVrtFitter"),
  m_iLambdaFitter("Trk::TrkVKalVrtFitter"),
  m_iLambdabarFitter("Trk::TrkVKalVrtFitter"),
  m_iGammaFitter("Trk::TrkVKalVrtFitter"),
  m_V0Tools("Trk::V0Tools"),
  m_helpertool("InDet::ConversionFinderUtils"),
  m_trkSelector("InDet::TrackSelectorTool"),
  m_vertexEstimator("InDet::VertexPointEstimator"),
  m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
  m_particleDataTable(0),
  m_startup(true),
  m_useorigin(true),
  m_samesign(false),
  m_pv(false),
  m_doUnconstrPointing(false),
  m_doConstrPointing(false),
  m_useTRTplusTRT(true),
  m_useTRTplusSi(true),
  m_useV0Fitter(true),
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
  m_eprob_s(0.995),
  m_eprob_t(0.5),
  m_minRxy(0.0),
  m_minsxy(0.0),
  m_maxsxy(1000.),
  m_minslxy(-1000),
  m_maxslxy(1000),
  m_rmax_s(300.),
  m_rmax_t(1000.),
  m_ksmin_s(400.),
  m_ksmax_s(600.),
  m_lamin_s(1050.),
  m_lamax_s(1200.),
  m_ksmin_t(0.),
  m_ksmax_t(2000.), 
  m_lamin_t(1050.),
  m_lamax_t(2000.), 
  m_uksmin_s(460.),
  m_uksmax_s(540.),
  m_ulamin_s(1100.),
  m_ulamax_s(1135.),
  m_uksmin_t(350.),
  m_uksmax_t(800.),
  m_ulamin_t(1100.),
  m_ulamax_t(1450.),
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

  m_TrkParticleCollection("TrackParticleCandidate"),
  m_VxPrimaryCandidateName("VxPrimaryCandidate"),
  m_V0CandidatesOutputName("V0Candidates")
{
  declareProperty("VertexFitterTool", m_iVertexFitter);
  declareProperty("VKVertexFitterTool", m_iVKVertexFitter);
  declareProperty("KshortFitterTool", m_iKshortFitter);
  declareProperty("LambdaFitterTool", m_iLambdaFitter);
  declareProperty("LambdabarFitterTool", m_iLambdabarFitter);
  declareProperty("GammaFitterTool", m_iGammaFitter);
  declareProperty("V0Tools",m_V0Tools);
  declareProperty("ConversionFinderHelperTool", m_helpertool);
  declareProperty("TrackSelectorTool", m_trkSelector);
  declareProperty("VertexPointEstimator", m_vertexEstimator);
  declareProperty("Extrapolator", m_extrapolator);
  declareProperty("startup", m_startup);
  declareProperty("useorigin", m_useorigin);
  declareProperty("AddSameSign", m_samesign);
  declareProperty("trkSelPV", m_pv);
  declareProperty("doUnconstrPointing", m_doUnconstrPointing);
  declareProperty("doConstrPointing", m_doConstrPointing);
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
  declareProperty("eprob_s", m_eprob_s);
  declareProperty("eprob_t", m_eprob_t);
  declareProperty("minRxy", m_minRxy);
  declareProperty("minsxy", m_minsxy);
  declareProperty("maxsxy", m_maxsxy);

  declareProperty("minslxy", m_minslxy);
  declareProperty("maxslxy", m_maxslxy);

  declareProperty("rmax_s", m_rmax_s);
  declareProperty("rmax_t", m_rmax_t);
  declareProperty("ksmin_s", m_ksmin_s);
  declareProperty("ksmax_s", m_ksmax_s);
  declareProperty("lamin_s", m_lamin_s);
  declareProperty("lamax_s", m_lamax_s);
  declareProperty("ksmin_t", m_ksmin_t);
  declareProperty("ksmax_t", m_ksmax_t);
  declareProperty("lamin_t", m_lamin_t);
  declareProperty("lamax_t", m_lamax_t);
  declareProperty("uksmin_s", m_uksmin_s),
  declareProperty("uksmax_s", m_uksmax_s),
  declareProperty("ulamin_s", m_ulamin_s),
  declareProperty("ulamax_s", m_ulamax_s),
  declareProperty("uksmin_t", m_uksmin_t),
  declareProperty("uksmax_t", m_uksmax_t),
  declareProperty("ulamin_t", m_ulamin_t),
  declareProperty("ulamax_t", m_ulamax_t),
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
  
  declareProperty("TrackParticleCollection", m_TrkParticleCollection);
  declareProperty("VxPrimaryCandidateName", m_VxPrimaryCandidateName);
  declareProperty("V0CandidatesOutputName",m_V0CandidatesOutputName="V0Candidates");
}

InDetV0Finder::~InDetV0Finder() {}

StatusCode InDetV0Finder::initialize()
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

  sc = resetStatistics();     // reset counters
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Error in resetStatistics !" << endreq;
    return sc;
  }

// uploading the corresponding V0 tools
  if ( m_V0Tools.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_V0Tools << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endreq;
  }

// Get the extrapolator
  if ( m_extrapolator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
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

  msg(MSG::INFO) << "Initialization successful" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode InDetV0Finder::execute()
{
  StatusCode sc;

  m_events_processed ++;

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
  const Trk::TrackParticleBaseCollection* TPC(0);
  sc = evtStore()->retrieve(TPC,m_TrkParticleCollection);
  if(sc.isFailure()){
    ATH_MSG_ERROR("No track particle collection with name " << m_TrkParticleCollection << " found in StoreGate!");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Found track particle collection " << m_TrkParticleCollection << " in StoreGate!");
  }
  ATH_MSG_DEBUG("Track particle container size " <<  TPC->size());

// Get primary vertex from StoreGate
  Trk::RecVertex* primaryVertex=NULL;
  const VxContainer* importedVxContainer(0);
  sc = evtStore()->retrieve(importedVxContainer,m_VxPrimaryCandidateName);
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("No VxPrimaryCandidate found in StoreGate");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Found " << m_VxPrimaryCandidateName << " in StoreGate!");
  }
  if (importedVxContainer!=0) {
    if (importedVxContainer->size()==0){
      ATH_MSG_WARNING("You have no primary vertices: " << importedVxContainer->size());
      // we will make a "dummy vertex" at 0,0,0
      primaryVertex = new Trk::RecVertex(Amg::Vector3D(0.,0.,0.));
    } else {
      primaryVertex=&((*(importedVxContainer->begin()))->recVertex());
    }
  }

  Trk::RecVertex * pointingVertex = 0;
  if (m_doUnconstrPointing || m_doConstrPointing) {
    if (primaryVertex != 0) pointingVertex = primaryVertex;
  }

// V0Container to store V0Candidates in StoreGate
  V0Container* theV0Container = new V0Container;

// track preselection
  std::vector<const Trk::TrackParticleBase*> posTracks; posTracks.clear();
  std::vector<const Trk::TrackParticleBase*> negTracks; negTracks.clear();
  const Trk::Vertex* vx = 0;
  if (m_pv) vx = primaryVertex;

  if (TPC->size() > 1) {
    Trk::TrackParticleBaseCollection::const_iterator tpIt;
    for ( tpIt =(*TPC).begin(); tpIt !=(*TPC).end(); ++tpIt )
    {
      const Trk::Perigee * aPerigee = dynamic_cast<const Trk::Perigee*>(&(*tpIt)->definingParameters());
      if (aPerigee == 0) ATH_MSG_DEBUG("No perigees for the tracks!!!");
      if (aPerigee != 0) {
        double charge = 1.;
        if (aPerigee->parameters()[Trk::qOverP] < 0.) charge = -1.;
        if (m_trkSelector->decision(**tpIt, vx)) {
          if (m_samesign) {
            posTracks.push_back(*tpIt);
            negTracks.push_back(*tpIt);
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
  }
  ATH_MSG_DEBUG("number of tracks passing preselection, positive " << posTracks.size() << " negative " << negTracks.size());

// select tracks pairs with opposite charge
  if (posTracks.size() > 0 && negTracks.size() > 0) {

  std::vector<const Trk::TrackParticleBase*>::const_iterator tpIt1;
  std::vector<const Trk::TrackParticleBase*>::const_iterator tpIt2;
  unsigned int i1 = 0;
  for (tpIt1 = posTracks.begin(); tpIt1 != posTracks.end(); ++tpIt1)
  {
    const Trk::Perigee * aPerigee1 = dynamic_cast<const Trk::Perigee*>(&(*tpIt1)->definingParameters());
    if (aPerigee1 == 0) { ATH_MSG_DEBUG("No perigees for the tracks!!!"); }
    if (aPerigee1 != 0) {
    const Trk::TrackSummary* tSum1 = (*tpIt1)->trackSummary();
    if (!tSum1) continue;
    int nclus1 = tSum1->get(Trk::numberOfPixelHits) + tSum1->get(Trk::numberOfSCTHits);
    double eprob1 = tSum1->getPID(Trk::eProbabilityComb);
    double pt1 = fabs(sin(aPerigee1->parameters()[Trk::theta])/aPerigee1->parameters()[Trk::qOverP]);
    double charge1 = 1.;
    if (aPerigee1->parameters()[Trk::qOverP] < 0.) charge1 = -1.;

    unsigned int i2 = 0;
    for (tpIt2 = negTracks.begin(); tpIt2 != negTracks.end(); ++tpIt2)
    {
      const Trk::Perigee * aPerigee2 = dynamic_cast<const Trk::Perigee*>(&(*tpIt2)->definingParameters());
      if (aPerigee2 == 0) { ATH_MSG_DEBUG("No perigees for the tracks!!!"); }
      if (aPerigee2 != 0) {
      const Trk::TrackSummary* tSum2 = (*tpIt2)->trackSummary();
      if (!tSum2) continue;
      int nclus2  = tSum2->get(Trk::numberOfPixelHits) + tSum2->get(Trk::numberOfSCTHits);
      double eprob2 = tSum2->getPID(Trk::eProbabilityComb);
      double pt2 = fabs(sin(aPerigee2->parameters()[Trk::theta])/aPerigee2->parameters()[Trk::qOverP]);
      double charge2 = 1.;
      if (aPerigee2->parameters()[Trk::qOverP] < 0.) charge2 = -1.;

      if (*tpIt1 == *tpIt2) continue;
      if (m_samesign && i1 <= i2) continue;
      if (!m_useTRTplusTRT && nclus1 == 0 && nclus2 == 0) continue;
      if (!m_useTRTplusSi && (nclus1 == 0 || nclus2 == 0)) continue;

      bool trk_cut1 = false;
      bool trk_cut2 = false;
      if (nclus1 != 0 && eprob1 < m_eprob_s) trk_cut1 = true;
      if (nclus1 == 0 && eprob1 < m_eprob_t && pt1 >= m_ptTRT) trk_cut1 = true;
      if (nclus2 != 0 && eprob2 < m_eprob_s) trk_cut2 = true;
      if (nclus2 == 0 && eprob2 < m_eprob_t && pt2 >= m_ptTRT) trk_cut2 = true;

      bool usepair = false;
      if (!m_useorigin) usepair = true;
      if (m_useorigin && (*tpIt1)->particleOriginType() == 0 && (*tpIt2)->particleOriginType() == 0) usepair = true;

      if (trk_cut1 && trk_cut2 && usepair) {

// find a starting point
        Amg::Vector3D startingPoint(0.,0.,0.);
        int sflag = 0;
        int errorcode = 0;
        startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(aPerigee1,aPerigee2,sflag,errorcode);

        bool errorCode = false;
        if (errorcode == 0) errorCode = true;
        if (errorcode == 5 && m_useerrorcode5) errorCode = true;
        if (errorCode) {
          const Trk::TrackParameters* atrack1 = m_helpertool->getTrkParticleParameters((*tpIt1));
          const Trk::TrackParameters* atrack2 = m_helpertool->getTrkParticleParameters((*tpIt2));

          bool is_TRT = false;
          if (nclus1 == 0 || nclus2 == 0) {
            is_TRT = true;
            m_rmax    = m_rmax_t;
            m_ksmin   = m_ksmin_t;
            m_ksmax   = m_ksmax_t;
            m_lamin   = m_lamin_t;
            m_lamax   = m_lamax_t;
            m_pt      = m_pt_t;
          } else {
            m_rmax    = m_rmax_s;
            m_ksmin   = m_ksmin_s;
            m_ksmax   = m_ksmax_s;
            m_lamin   = m_lamin_s;
            m_lamax   = m_lamax_s;
            m_pt      = m_pt_s;
          }

// pair pre-selection cuts
          if ( atrack1 && atrack2 && doFit(atrack1, atrack2, startingPoint, primaryVertex) ) {

            Trk::Vertex vertex(startingPoint);
            std::vector<const Trk::TrackParameters*> pairV0;
            pairV0.clear();
            pairV0.push_back(atrack1);
            pairV0.push_back(atrack2);

// vertex fit
            ATH_MSG_DEBUG("unconstrained fit now");

            Trk::VxCandidate* myExtVxCandidate(0);
            if (m_useV0Fitter) {
              if (!m_doUnconstrPointing) {
                myExtVxCandidate = concreteVertexFitter->fit(pairV0, vertex);
              } else {
                std::vector<double> masses;
                myExtVxCandidate = concreteVertexFitter->fit(pairV0, masses, -9999., pointingVertex, vertex);
              }
            } else {
              if (m_doUnconstrPointing) {
                Trk::TrkVKalVrtFitter* m_PointVKVertexFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVKVertexFitter));
                m_PointVKVertexFitter->setVertexForConstraint(*pointingVertex);
                m_PointVKVertexFitter->setCnstType(m_CnstType);
                myExtVxCandidate = m_PointVKVertexFitter->fit(pairV0, vertex);
              } else {
                myExtVxCandidate = m_iVKVertexFitter->fit(pairV0, vertex);
              }
            }
            if (0 != myExtVxCandidate) {
              const Trk::TrackParameters*  bPer0 = (*(myExtVxCandidate->vxTrackAtVertex()))[0]->perigeeAtVertex();
              const Trk::TrackParameters*  bPer1 = (*(myExtVxCandidate->vxTrackAtVertex()))[1]->perigeeAtVertex();
              double trkCharge0 = 1., trkCharge1 = 1.;
              if (bPer0->parameters()[Trk::qOverP] < 0.) trkCharge0 = -1.;
              if (bPer1->parameters()[Trk::qOverP] < 0.) trkCharge1 = -1.;
//              if ((trkCharge0 != 1.) || (trkCharge1 != -1.)) myExtVxCandidate = 0;
              if ((trkCharge0*charge1 < 0.) || (trkCharge1*charge2 < 0.)) {
                delete myExtVxCandidate;
                myExtVxCandidate = 0;
              }

            }
            if(myExtVxCandidate != 0) {
              Trk::ExtendedVxCandidate* myVxCandidate = dynamic_cast<Trk::ExtendedVxCandidate* >(myExtVxCandidate);
      
              if(myVxCandidate != 0) {
                myVxCandidate->setVertexType(Trk::V0Vtx);

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

                if ( useFit(atrack1, atrack2, myVxCandidate, primaryVertex) ) {

                  bool doKshortFit = false;
                  doKshortFit = doMassFit(myVxCandidate,310,is_TRT);
                  bool doLambdaFit = false;
                  doLambdaFit = doMassFit(myVxCandidate,3122,is_TRT);
                  bool doLambdabarFit = false;
                  doLambdabarFit = doMassFit(myVxCandidate,-3122,is_TRT);

                  if ( doKshortFit || doLambdaFit || doLambdabarFit ) {

// unconstrained V0 hypothesis
                    Trk::V0Hypothesis* myUnconstrV0 = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                    if (myUnconstrV0 != 0) {

                      bool electron = false;
                      if (m_electrons) electron = electronPair(tSum1, tSum2, nclus1, nclus2);

// constrained fits now
                      Trk::Vertex vertex0(m_V0Tools->vtx(myVxCandidate));
                      if (m_vertex == 2) vertex = vertex0;

// Kshort
                      Trk::V0Hypothesis* myKshort(0);
                      bool goodKs = false;
                      if ( !electron && doKshortFit ) {
                        ATH_MSG_DEBUG("Kshort fit, mass constraint, no pointing");
                        Trk::VxCandidate* myExtVxCandidateKs = massFit(310, pairV0, pointingVertex, vertex, concreteVertexFitter, primaryVertex);
                        if(myExtVxCandidateKs != 0) {
                          Trk::ExtendedVxCandidate* myVxCandidateKs = dynamic_cast<Trk::ExtendedVxCandidate* >(myExtVxCandidateKs);
                          if(myVxCandidateKs != 0) {
                            myVxCandidateKs->setVertexType(Trk::V0Vtx);
                            if (m_V0Tools->vertexProbability(myVxCandidateKs) >= m_minConstrVertProb && m_V0Tools->rxy(myVxCandidateKs) <= m_maxsxy) goodKs = true;
                            if (m_startup) {
                              if (goodKs) {
                                myKshort = new Trk::V0Hypothesis(*myVxCandidateKs,PDG::pi_plus,PDG::pi_minus,PDG::K_S0);
                              } else {
                                myKshort = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                              }
                            } else {
                              if (goodKs) myKshort = new Trk::V0Hypothesis(*myVxCandidateKs,PDG::pi_plus,PDG::pi_minus,PDG::K_S0);
                            }
                            if (!m_startup && myKshort == 0) delete myKshort;
                          } else {
                            if (m_startup) myKshort = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                          }
                        } else {
                          if (m_startup) myKshort = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                        }
                        delete myExtVxCandidateKs;
                      }
        
// Lambda
                      Trk::V0Hypothesis* myLambda(0);
                      bool goodL = false;
                      if ( !electron && doLambdaFit ) {
                        ATH_MSG_DEBUG("Lambda fit, mass constraint, no pointing");
                        Trk::VxCandidate* myExtVxCandidateL = massFit(3122, pairV0, pointingVertex, vertex, concreteVertexFitter, primaryVertex);
                        if(myExtVxCandidateL != 0) {
                          Trk::ExtendedVxCandidate* myVxCandidateL = dynamic_cast<Trk::ExtendedVxCandidate* >(myExtVxCandidateL);
                          if(myVxCandidateL != 0) {
                            myVxCandidateL->setVertexType(Trk::V0Vtx);
                            if (m_V0Tools->vertexProbability(myVxCandidateL) >= m_minConstrVertProb && m_V0Tools->rxy(myVxCandidateL) <= m_maxsxy) goodL = true;
                            if (m_startup) {
                              if (goodL) {
                                myLambda = new Trk::V0Hypothesis(*myVxCandidateL,PDG::p_plus,PDG::pi_minus,PDG::Lambda0);
                              } else {
                                myLambda = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                              }
                            } else {
                              if (goodL) myLambda = new Trk::V0Hypothesis(*myVxCandidateL,PDG::p_plus,PDG::pi_minus,PDG::Lambda0);
                            }
                            if (!m_startup && myLambda == 0) delete myLambda;
                          } else {
                            if (m_startup) myLambda = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                          }
                        } else {
                          if (m_startup) myLambda = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                        }
                        delete myExtVxCandidateL;
                      }
        
// Lambdabar
                      Trk::V0Hypothesis* myLambdabar(0);
                      bool goodLbar = false;
                      if ( !electron && doLambdabarFit ) {
                        ATH_MSG_DEBUG("Lambdabar fit, mass constraint, no pointing");
                        Trk::VxCandidate* myExtVxCandidateLbar = massFit(-3122, pairV0, pointingVertex, vertex, concreteVertexFitter, primaryVertex);
                        if(myExtVxCandidateLbar != 0) {
                          Trk::ExtendedVxCandidate* myVxCandidateLbar = dynamic_cast<Trk::ExtendedVxCandidate* >(myExtVxCandidateLbar);
                          if(myVxCandidateLbar != 0) {
                            myVxCandidateLbar->setVertexType(Trk::V0Vtx);
                            if (m_V0Tools->vertexProbability(myVxCandidateLbar) >= m_minConstrVertProb && m_V0Tools->rxy(myVxCandidateLbar) <= m_maxsxy) goodLbar = true;
                            if (m_startup) {
                              if (goodLbar) {
                                myLambdabar = new Trk::V0Hypothesis(*myVxCandidateLbar,PDG::pi_plus,PDG::anti_p_minus,PDG::anti_Lambda0);
                              } else {
                                myLambdabar = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                              }
                            } else {
                              if (goodLbar) myLambdabar = new Trk::V0Hypothesis(*myVxCandidateLbar,PDG::pi_plus,PDG::anti_p_minus,PDG::anti_Lambda0);
                            }
                            if (!m_startup && myLambdabar == 0) delete myLambdabar;
                          } else {
                            if (m_startup) myLambdabar = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                          }
                        } else {
                          if (m_startup) myLambdabar = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
                        }
                        delete myExtVxCandidateLbar;
                      }

// Gamma
                      bool doGamma = false;
                      if (0 != myKshort || 0 != myLambda || 0 != myLambdabar) doGamma = true; 
                      Trk::V0Hypothesis* myGamma(0);
                      if ( doGamma ) {
                        ATH_MSG_DEBUG("Gamma fit, mass constraint, no pointing");
                        Trk::VxCandidate* myExtVxCandidateG = massFit(22, pairV0, pointingVertex, vertex, concreteVertexFitter, primaryVertex);
                        if(myExtVxCandidateG != 0) {
                          Trk::ExtendedVxCandidate* myVxCandidateG = dynamic_cast<Trk::ExtendedVxCandidate* >(myExtVxCandidateG);
                          if(myVxCandidateG != 0) {
                            myVxCandidateG->setVertexType(Trk::V0Vtx);
                            if (m_V0Tools->vertexProbability(myVxCandidateG) >= m_minConstrVertProb && m_V0Tools->rxy(myVxCandidateG) <= m_maxsxy) myGamma = new Trk::V0Hypothesis(*myVxCandidateG,PDG::e_plus,PDG::e_minus,PDG::gamma);
                            if (myGamma == 0) delete myGamma;
                          }
                        }
                        delete myExtVxCandidateG;
                      }
        
                      bool fin_cuts = false;
                      if (0!= myKshort || 0!= myLambda || 0!= myLambdabar) fin_cuts = true;
        
                      if (fin_cuts) {
// store a V0Candidate
                        std::vector<Trk::V0Hypothesis *> myV0Hyp;
                        if(0!= myUnconstrV0) {
                          myV0Hyp.push_back(myUnconstrV0);
                        }
                        if(0!= myKshort) {
                          myV0Hyp.push_back(myKshort);
                          if(goodKs) m_Kshort_stored++;
                        }
                        if(0!= myLambda) {
                          myV0Hyp.push_back(myLambda);
                          if(goodL) m_Lambda_stored++;
                        }
                        if(0!= myLambdabar) {
                          myV0Hyp.push_back(myLambdabar);
                          if(goodLbar) m_Lambdabar_stored++;
                        }
                        if(0!= myGamma) {
                          myV0Hyp.push_back(myGamma);
                          m_Gamma_stored++;
                        }
        
                        Trk::V0Candidate* myV0Candidate = new Trk::V0Candidate(myV0Hyp);
        
// set LinkToTrackParticleBase
                        std::vector<Trk::V0Hypothesis *>::iterator hb = (myV0Candidate->v0Hypothesis())->begin();
                        std::vector<Trk::V0Hypothesis *>::iterator he = (myV0Candidate->v0Hypothesis())->end();
                        for(;hb!=he;++hb) {
                          const std::vector<Trk::VxTrackAtVertex*>& trackAtVertexVec = *((*hb)->vxTrackAtVertex());
                          for (unsigned int i = 0; i < trackAtVertexVec.size(); i++) {
                            Trk::LinkToTrackParticleBase* newLinkToTrackPB = new Trk::LinkToTrackParticleBase();
                            if(i == 0) newLinkToTrackPB->setElement((*tpIt1));
                            if(i == 1) newLinkToTrackPB->setElement((*tpIt2));
                            newLinkToTrackPB->setStorableObject(*TPC);
                            trackAtVertexVec[i]->setOrigTrack(newLinkToTrackPB);
                          }
                        } // end of loop over all hypothesis
        
                        m_V0s_stored++;
        
// check if correctly stored 
                        if (msgLvl(MSG::DEBUG)) {
                          msg(MSG::DEBUG) << "found a V0" << endreq;
                          const Trk::V0Hypothesis* aUnconstr = m_V0Tools->v0Unconstr(myV0Candidate);
                          if(aUnconstr != 0) msg(MSG::DEBUG) << "V0Candidate UnconstrV0 prob: " << m_V0Tools->vertexProbability(aUnconstr) << endreq;
                          const Trk::V0Hypothesis* aKshort = m_V0Tools->v0Kshort(myV0Candidate);
                          if(aKshort != 0) msg(MSG::DEBUG) << "V0Candidate KshortV0 prob: " << m_V0Tools->vertexProbability(aKshort) << endreq;
                          const Trk::V0Hypothesis* aLambda = m_V0Tools->v0Lambda(myV0Candidate);
                          if(aLambda != 0) msg(MSG::DEBUG) << "V0Candidate LambdaV0 prob: " << m_V0Tools->vertexProbability(aLambda) << endreq;
                          const Trk::V0Hypothesis* aLambdabar = m_V0Tools->v0LambdaBar(myV0Candidate);
                          if(aLambdabar != 0) msg(MSG::DEBUG) << "V0Candidate LambdabarV0 prob: " << m_V0Tools->vertexProbability(aLambdabar) << endreq;
                          const Trk::V0Hypothesis* aGamma = m_V0Tools->v0Gamma(myV0Candidate);
                          if(aGamma != 0) msg(MSG::DEBUG) << "V0Candidate GammaV0 prob: " << m_V0Tools->vertexProbability(aGamma) << endreq;
                        }
         
// write to the V0Container        
                        theV0Container->push_back(myV0Candidate);
      
                      } else {
                        if (myUnconstrV0 != 0) delete myUnconstrV0;
                        if (myKshort != 0) delete myKshort;
                        if (myLambda != 0) delete myLambda;
                        if (myLambdabar != 0) delete myLambdabar;
                        if (myGamma != 0) delete myGamma;
                      }
                    } else { delete myUnconstrV0; }
                  }  // doMassFit
                }  // useFit
              }
              delete myExtVxCandidate;

            }
          }  // doFit
        }  // no suitable starting point found
      }  // use tracks not associated to any vertex

      }
      i2++;
    }  // loop over negative tracks
    } 
    i1++;
  }  // loop over positive tracks
  }

  if ( theV0Container->size()==0) ATH_MSG_DEBUG("No Candidates found. Empty container returned");

  //---- Recording section: write the results to StoreGate ---//
    
  ATH_MSG_DEBUG("Recording to StoreGate: " << m_V0CandidatesOutputName << " size:" <<theV0Container->size());
  sc = evtStore()->record(theV0Container, m_V0CandidatesOutputName,false);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to record V0Container in TDS");
    return sc;
  }

  return StatusCode::SUCCESS;
}// end execute block

StatusCode InDetV0Finder::finalize()
{
  msg(MSG::INFO)
    << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq
    << "\tSummary" << endreq 
    << "\tProcessed              : " << m_events_processed            << " events" << endreq
    << "\tStored                 : " << m_V0s_stored                  << " V0s" << endreq
    << "\tof which               : " << m_Kshort_stored               << " Kshorts" << endreq
    << "\t                       : " << m_Lambda_stored               << " Lambdas" << endreq
    << "\t                       : " << m_Lambdabar_stored            << " Lambdabars" << endreq
    << "\t                       : " << m_Gamma_stored                << " Conversions" << endreq;
  msg(MSG::INFO) << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode InDetV0Finder::resetStatistics() {
    m_events_processed           = 0;
    m_V0s_stored                 = 0;
    m_Kshort_stored              = 0;
    m_Lambdabar_stored           = 0;
    m_Lambda_stored              = 0;
    m_Gamma_stored               = 0;
  
    return StatusCode :: SUCCESS;
}

void InDetV0Finder::SGError(std::string errService)
{
  msg(MSG::FATAL) << errService << " not found. Exiting !" << endreq;
  return;
}

bool InDetV0Finder::doFit(const Trk::TrackParameters* track1, const Trk::TrackParameters* track2, Amg::Vector3D &startingPoint, Trk::RecVertex* primaryVertex)
{
  bool pass = false;
  double rmax = std::max((startingPoint-track1->position()).perp(),(startingPoint-track2->position()).perp());
  double slxy = (startingPoint-primaryVertex->position()).perp();
  
  if (startingPoint.perp() > m_minsxy && startingPoint.perp() < m_maxsxy && rmax < m_rmax && slxy > m_minslxy && slxy < m_maxslxy) {
    double massKshort_i=2001., massLambda_i=2001., massLambdabar_i=2001., pt_i=2001.;
    Amg::Vector3D globalPosition = startingPoint;
    Trk::PerigeeSurface perigeeSurface(globalPosition);
    const Trk::TrackParameters* chargePar1 = dynamic_cast<const Trk::TrackParameters*> (track1);
    const Trk::TrackParameters* chargePar2 = dynamic_cast<const Trk::TrackParameters*> (track2);
    if (chargePar1 != 0 && chargePar2 != 0) {
      const Trk::TrackParameters* extrapolatedPerigee1 = m_extrapolator->extrapolateDirectly(*chargePar1, perigeeSurface);
      if (extrapolatedPerigee1 == 0) {
        msg(MSG::DEBUG) << "extrapolation failed, using original perigee" << endreq;
        extrapolatedPerigee1 = chargePar1;
      }
      const Trk::TrackParameters* extrapolatedPerigee2 = m_extrapolator->extrapolateDirectly(*chargePar2, perigeeSurface);
      if (extrapolatedPerigee2 == 0) {
        msg(MSG::DEBUG) << "extrapolation failed, using original perigee" << endreq;
        extrapolatedPerigee2 = chargePar2;
      }
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
  }

  return pass;
}

bool InDetV0Finder::useFit(const Trk::TrackParameters* track1, const Trk::TrackParameters* track2, const Trk::ExtendedVxCandidate* vxCandidate, Trk::RecVertex* primaryVertex)
{
  bool pass = false;
  double aRxy = m_V0Tools->rxy(vxCandidate);

// chi2 and minRxy cuts
  if (m_V0Tools->vertexProbability(vxCandidate) >= m_minVertProb && aRxy >= m_minRxy) {

// no hits on the wrong side of the vertex
    //HepGeom::Point3D<double> globPos_hit1 = track1->position();
    //HepGeom::Point3D<double> globPos_hit2 = track2->position();
    Amg::Vector3D globPos_hit1 = track1->position();
    Amg::Vector3D globPos_hit2 = track2->position();
    double lxy1 = (globPos_hit1.x()*m_V0Tools->vtx(vxCandidate).x()+globPos_hit1.y()*m_V0Tools->vtx(vxCandidate).y())/(aRxy*aRxy);
    double lxy2 = (globPos_hit2.x()*m_V0Tools->vtx(vxCandidate).x()+globPos_hit2.y()*m_V0Tools->vtx(vxCandidate).y())/(aRxy*aRxy);
    double pt   = m_V0Tools->V0Momentum(vxCandidate).perp();
    double lxy  = m_V0Tools->lxy(vxCandidate, *primaryVertex);

    if (lxy1 >= m_hlxymin && lxy2 >= m_hlxymin && lxy1 <= m_hlxymax && lxy2 <= m_hlxymax && pt > m_upt && lxy > m_ulxy ) pass = true;
  }

  return pass;
}

bool InDetV0Finder::doMassFit(const Trk::ExtendedVxCandidate* vxCandidate, int pdgID, bool is_TRT)
{
  bool pass = false;
  double mass = 1000000.;
  double error = 1001.;

  bool in_mass_window = false;

  double winmass_min = 0., winmass_max = 0., errmass = 0.;
  if (is_TRT) errmass = m_errmass_t;

  if (pdgID == 310) {
    if (!is_TRT) {
      winmass_min = m_uksmin_s;
      winmass_max = m_uksmax_s;
      errmass = m_errmass_ks;
    } else {
      winmass_min = m_uksmin_t;
      winmass_max = m_uksmax_t;
    }
    mass = m_V0Tools->invariantMass(vxCandidate,m_masspi,m_masspi);
    error = m_V0Tools->invariantMassError(vxCandidate,m_masspi,m_masspi);
    if (mass >= winmass_min && mass <= winmass_max && error <= errmass) in_mass_window = true;
  } else if (pdgID == 3122) {
    if (!is_TRT) {
      winmass_min = m_ulamin_s;
      winmass_max = m_ulamax_s;
      errmass = m_errmass_la;
    } else {
      winmass_min = m_ulamin_t;
      winmass_max = m_ulamax_t;
    }
    mass = m_V0Tools->invariantMass(vxCandidate,m_massp,m_masspi);
    error = m_V0Tools->invariantMassError(vxCandidate,m_massp,m_masspi);
    if (mass >= winmass_min && mass <= winmass_max && error <= errmass) in_mass_window = true;
  } else if (pdgID == -3122) {
    if (!is_TRT) {
      winmass_min = m_ulamin_s;
      winmass_max = m_ulamax_s;
      errmass = m_errmass_la;
    } else {
      winmass_min = m_ulamin_t;
      winmass_max = m_ulamax_t;
    }
    mass = m_V0Tools->invariantMass(vxCandidate,m_masspi,m_massp);
    error = m_V0Tools->invariantMassError(vxCandidate,m_masspi,m_massp);
    if (mass >= winmass_min && mass <= winmass_max && error <= errmass) in_mass_window = true;
  } else if (pdgID == 22) {
    if (!is_TRT) {
      winmass_max = m_ugamax;
      errmass = m_errmass_ga;
    }
    mass = m_V0Tools->invariantMass(vxCandidate,m_masse,m_masse);
    error = m_V0Tools->invariantMassError(vxCandidate,m_masse,m_masse);
    if (mass <= winmass_max && error <= errmass) in_mass_window = true;
  }

  if (in_mass_window) pass = true;

  return pass;
}

double InDetV0Finder::invariantMass(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2, double &m1, double &m2)
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

bool InDetV0Finder::electronPair(const Trk::TrackSummary* tSum1, const Trk::TrackSummary* tSum2, int nclus1, int nclus2)
{
  bool pass = false;
  int nhtrt1 = tSum1->get(Trk::numberOfTRTHighThresholdHits);
  double eprob1 = tSum1->getPID(Trk::eProbabilityComb);
  int nhtrt2 = tSum2->get(Trk::numberOfTRTHighThresholdHits);
  double eprob2 = tSum2->getPID(Trk::eProbabilityComb);
  bool nh1 = false;
  if ((nclus1 != 0 && nhtrt1 > 6) || (nclus1 == 0 && nhtrt1 > 4)) nh1 = true;
  bool nh2 = false;
  if ((nclus2 != 0 && nhtrt2 > 6) || (nclus2 == 0 && nhtrt2 > 4)) nh2 = true;
  if (nh1 || nh2 || eprob1 > 0.995 || eprob2 > 0.995) pass = true;

  return pass;
}

Trk::VxCandidate* InDetV0Finder::massFit(int pdgID, std::vector<const Trk::TrackParameters*> pairV0, Trk::RecVertex * pointingVertex, Trk::Vertex vertex, Trk::TrkV0VertexFitter* concreteVertexFitter, Trk::RecVertex* primaryVertex)
{
  Trk::VxCandidate* myVxCandidate(0);
  std::vector<double> masses;
  double mass_V0 = 0., ptCut = 0., lxyCut = 0;
  if (pdgID == 310) {
    masses.push_back(m_masspi);
    masses.push_back(m_masspi);
    mass_V0 = m_massK0S;
    ptCut = m_kspt;
    lxyCut = m_kslxy;
  } else if (pdgID == 3122) {
    masses.push_back(m_massp);
    masses.push_back(m_masspi);
    mass_V0 = m_massLambda;
    ptCut = m_lapt;
    lxyCut = m_lalxy;
  } else if (pdgID == -3122) {
    masses.push_back(m_masspi);
    masses.push_back(m_massp);
    mass_V0 = m_massLambda;
    ptCut = m_lapt;
    lxyCut = m_lalxy;
  } else if (pdgID == 22) {
    masses.push_back(m_masse);
    masses.push_back(m_masse);
    mass_V0 = m_massGamma;
    ptCut = m_gapt;
    lxyCut = m_galxy;
  }
        
  if (m_useV0Fitter) {
    if (m_gamma == 1) {
      if (pdgID == 22) {
        if (m_doConstrPointing) {
          Trk::TrkVKalVrtFitter* m_PointGammaFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iGammaFitter));
          m_PointGammaFitter->setVertexForConstraint(*pointingVertex);
          m_PointGammaFitter->setCnstType(m_CnstType);
          myVxCandidate = m_PointGammaFitter->fit(pairV0, vertex);
        } else {
          myVxCandidate = m_iGammaFitter->fit(pairV0, vertex);
        }
      } else {
        myVxCandidate = concreteVertexFitter->fit(pairV0, masses, mass_V0, pointingVertex, vertex);
      }
    } else if (m_gamma == 2) {
      myVxCandidate = concreteVertexFitter->fit(pairV0, masses, mass_V0, pointingVertex, vertex);
    }
  } else {
    if (pdgID ==   310) {
      if (m_doConstrPointing) {
        Trk::TrkVKalVrtFitter* m_PointKshortFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iKshortFitter));
        m_PointKshortFitter->setVertexForConstraint(*pointingVertex);
        m_PointKshortFitter->setCnstType(m_CnstType);
        myVxCandidate = m_PointKshortFitter->fit(pairV0, vertex);
      } else {
        myVxCandidate = m_iKshortFitter->fit(pairV0, vertex);
      }
    }
    if (pdgID ==  3122) {
      if (m_doConstrPointing) {
        Trk::TrkVKalVrtFitter* m_PointLambdaFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iLambdaFitter));
        m_PointLambdaFitter->setVertexForConstraint(*pointingVertex);
        m_PointLambdaFitter->setCnstType(m_CnstType);
        myVxCandidate = m_PointLambdaFitter->fit(pairV0, vertex);
      } else {
        myVxCandidate = m_iLambdaFitter->fit(pairV0, vertex);
      }
    }
    if (pdgID == -3122) {
      if (m_doConstrPointing) {
        Trk::TrkVKalVrtFitter* m_PointLambdabarFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iLambdabarFitter));
        m_PointLambdabarFitter->setVertexForConstraint(*pointingVertex);
        m_PointLambdabarFitter->setCnstType(m_CnstType);
        myVxCandidate = m_PointLambdabarFitter->fit(pairV0, vertex);
      } else {
        myVxCandidate = m_iLambdabarFitter->fit(pairV0, vertex);
      }
    }
    if (pdgID ==    22) {
      if (m_doConstrPointing) {
        Trk::TrkVKalVrtFitter* m_PointGammaFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iGammaFitter));
        m_PointGammaFitter->setVertexForConstraint(*pointingVertex);
        m_PointGammaFitter->setCnstType(m_CnstType);
        myVxCandidate = m_PointGammaFitter->fit(pairV0, vertex);
      } else {
        myVxCandidate = m_iGammaFitter->fit(pairV0, vertex);
      }
    }
  }
  // protect against track sign change
  if (0 != myVxCandidate) {
    const Trk::TrackParameters*  bPer0 = (*(myVxCandidate->vxTrackAtVertex()))[0]->perigeeAtVertex();
    const Trk::TrackParameters*  bPer1 = (*(myVxCandidate->vxTrackAtVertex()))[1]->perigeeAtVertex();
    double trkCharge0 = 1., trkCharge1 = 1.;
    if (bPer0->parameters()[Trk::qOverP] < 0.) trkCharge0 = -1.;
    if (bPer1->parameters()[Trk::qOverP] < 0.) trkCharge1 = -1.;
    double charge0 = pairV0[0]->charge();
    double charge1 = pairV0[1]->charge();
//    if ((trkCharge0 != 1.) || (trkCharge1 != -1.)) myVxCandidate = 0;
    if ((trkCharge0*charge0 < 0.) || (trkCharge1*charge1 < 0.)) {
      delete myVxCandidate;
      myVxCandidate = 0;
    }
    // pt cut
    const Trk::ExtendedVxCandidate* myExVxCandidate = dynamic_cast<const Trk::ExtendedVxCandidate*>(myVxCandidate);
    if( myExVxCandidate!=0 && ptCut > m_V0Tools->V0Momentum(myExVxCandidate).perp() ) {
      delete myVxCandidate;
      myVxCandidate = 0;
    }
    // lxy cut
    if( myExVxCandidate!=0 && lxyCut > m_V0Tools->lxy(myExVxCandidate, *primaryVertex) ) {
      delete myVxCandidate;
      myVxCandidate = 0;
    }
  }

  return myVxCandidate;
}

double InDetV0Finder::getPt(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2) {

  Amg::Vector3D momentum( per1->momentum() + per2->momentum() );
  return momentum.perp();

}

}//end of namespace InDet

