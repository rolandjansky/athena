///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ImagingSeedTuningAlg.cxx 
// Implementation file for class ImagingSeedTuningAlg
// Author: Dave Casper <dcasper@uci.edu>
/////////////////////////////////////////////////////////////////// 

// TrkVertexSeedFinderUtils includes
#include "ImagingSeedTuningAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"

#include "TrkVertexSeedFinderUtils/IVertexImageMaker.h"
#include "TrkVertexSeedFinderUtils/IVertexClusterFinder.h"

#include "TrkLinks/LinkToXAODTrackParticle.h"

#include <limits>

namespace Trk
{
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ImagingSeedTuningAlg::ImagingSeedTuningAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_significanceTruthCut(3.0),
  m_truthVertexTracks(2),
  m_truthWindow(0.5),
  m_useBeamConstraint(true),
  m_trackFilter("InDet::InDetTrackSelectionTool"),
  m_seedFinder("Trk::ImagingSeedFinder"),
  m_impactPoint3dEstimator("Trk::ImpactPoint3dEstimator"),
  m_iTHistSvc("THistSvc", name),
  m_h_nTruthVertices(nullptr), m_h_zTruthVertices(nullptr), m_t_seeds(nullptr),
  m_b_nTruth(0), m_b_nConditions(0), m_iCondition(0)
{
  //
  // Property declaration
  // 
  declareProperty( "significanceTruthCut", m_significanceTruthCut );
  declareProperty( "truthVertexTracks", m_truthVertexTracks );
  declareProperty( "truthWindow", m_truthWindow );
  declareProperty( "useBeamConstraint", m_useBeamConstraint );
  declareProperty( "trackFilter", m_trackFilter );
  declareProperty( "seedFinder", m_seedFinder );
  declareProperty( "impactPoint3dEstimator", m_impactPoint3dEstimator );
  declareProperty( "angularCutoffParameterValues", m_angularCutoffParameterValues );
  declareProperty( "clusterWindowXYValues", m_clusterWindowXYValues );
  declareProperty( "mergeParameterValues", m_mergeParameterValues );
  declareProperty( "weightThresholdValues", m_weightThresholdValues );
  declareProperty( "refineZValues", m_refineZValues );
  declareProperty( "gaussianWindowValues", m_gaussianWindowValues );
}

// Destructor
///////////////
ImagingSeedTuningAlg::~ImagingSeedTuningAlg()
= default;

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ImagingSeedTuningAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_trackParticlesKey.initialize() );
  ATH_CHECK( m_truthEventsKey.initialize() );
  ATH_CHECK( m_pileupEventsKey.initialize() );

  Trk::IVertexSeedFinder* p_ivsf;
  CHECK( m_seedFinder.retrieve(p_ivsf) );

  SmartIF<IProperty> i_seedProp(p_ivsf);
  if ( !i_seedProp.isValid() ) 
  {
    msg(MSG::FATAL) << "Failed to retrieve IProperty interface of " << m_seedFinder << endmsg;
    return StatusCode::FAILURE;
  }

  std::string s_imageMaker;
  CHECK( i_seedProp->getProperty("VertexImageMaker", s_imageMaker) );

  std::string s_clusterFinder;
  CHECK( i_seedProp->getProperty("VertexCluster", s_clusterFinder) );

  Trk::IVertexImageMaker* p_ivim;
  ToolHandle< Trk::IVertexImageMaker > h_imageMaker(s_imageMaker);
  CHECK( h_imageMaker.retrieve(p_ivim) );

  Trk::IVertexClusterFinder* p_ivcf;
  ToolHandle< Trk::IVertexClusterFinder > h_clusterFinder(s_clusterFinder);
  CHECK( h_clusterFinder.retrieve(p_ivcf) ); 

  m_imageMakerProperty = SmartIF<IProperty>(p_ivim);
  if ( !m_imageMakerProperty.isValid() ) 
  {
    msg(MSG::FATAL) << "Failed to retrieve IProperty interface for " << s_imageMaker << endmsg;
    return StatusCode::FAILURE;
  }

  m_clusterFinderProperty = SmartIF<IProperty>(p_ivcf);
  if ( !m_clusterFinderProperty.isValid() ) 
  {
    msg(MSG::FATAL) << "Failed to retrieve IProperty interface for " << s_clusterFinder << endmsg;
    return StatusCode::FAILURE;
  }

  CHECK( m_impactPoint3dEstimator.retrieve() );

  CHECK( m_trackFilter.retrieve() );

  // setup histograms/trees
  m_h_nTruthVertices = new TH1F("nTruthVtx", "N truth vertices", 60, 0.0, 60.0);
  m_h_zTruthVertices = new TH1F("zTruthVtx","z (visible truth vertices)",100,-200.0,200.0);
  m_t_seeds          = new TTree("seeds","seeds");

  CHECK( m_iTHistSvc->regHist("/file1/h/truthVertices", m_h_nTruthVertices) );
  CHECK( m_iTHistSvc->regHist("/file1/h/zTruthVtx", m_h_zTruthVertices) );
  CHECK( m_iTHistSvc->regTree("/file1/t/seeds", m_t_seeds) );

  m_t_seeds->Branch("nTruth", &m_b_nTruth, "nTruth/I");
  m_t_seeds->Branch("nConditions", &m_b_nConditions, "nConditions/I");
  m_t_seeds->Branch("nGoodTruth", &m_b_nGoodTruth, "nGoodTruth[nConditions]/I");
  m_t_seeds->Branch("nSeeds", &m_b_nSeeds, "nSeeds[nConditions]/I");
  m_t_seeds->Branch("nGoodSeeds", &m_b_nGoodSeeds, "nGoodSeeds[nConditions]/I");
  m_t_seeds->Branch("nLost", &m_b_nLost, "nLost[nConditions]/I");
  m_t_seeds->Branch("nFake", &m_b_nFake, "nFake[nConditions]/I");
  m_t_seeds->Branch("nSplit", &m_b_nSplit, "nSplit[nConditions]/I");
  m_t_seeds->Branch("nMerge", &m_b_nMerge, "nMerge[nConditions]/I");
  m_t_seeds->Branch("angularCutoff", &m_b_angularCutoffParameter, "angularCutoff[nConditions]/F");
  m_t_seeds->Branch("weightThreshold", &m_b_weightThreshold, "weightThreshold[nConditions]/F");
  m_t_seeds->Branch("mergeParameter", &m_b_mergeParameter, "mergeParameter[nConditions]/F");
  m_t_seeds->Branch("clusterWindowXY", &m_b_clusterWindowXY, "clusterWindowXY[nConditions]/F");
  m_t_seeds->Branch("refineZ", &m_b_refineZ, "refineZ[nConditions]/I1");
  m_t_seeds->Branch("gaussianWindow", &m_b_gaussianWindow, "gaussianWindow[nConditions]/I1");

  m_b_nConditions = m_angularCutoffParameterValues.size() *
    m_clusterWindowXYValues.size()*
    m_mergeParameterValues.size()*
    m_weightThresholdValues.size()*
    m_refineZValues.size()*
    m_gaussianWindowValues.size();

  ATH_CHECK(m_beamSpotKey.initialize());
    
  return StatusCode::SUCCESS;
}

StatusCode ImagingSeedTuningAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  std::vector<std::pair<std::string, std::pair<float,float> > > performance;
  typedef std::pair<std::string, std::pair<float, float> > performance_entry;
  for (auto & allTruth : m_allTruth)
  {
    std::string desc = allTruth.first;
    int nAllTruth = allTruth.second;
    int nAllSeeds = m_allSeeds[desc];
    int nFake = m_nFake[desc];
    int nLost = m_nLost[desc];
    int nSplit = m_nSplit[desc];
    int nMerge = m_nMerge[desc];
    float efficiency = ((float)(nAllTruth - nLost - nMerge))/((float)nAllTruth);
    float purity = ((float)(nAllSeeds - nFake - nSplit))/((float)nAllSeeds);
    std::pair<float,float> p(efficiency, purity);
    performance.emplace_back(desc, p);
  }
  std::sort(performance.begin(), performance.end(), [](const performance_entry& a, const performance_entry& b) 
	    {return a.second.first*a.second.second > b.second.first*b.second.second;});

  int nPrint = 0;
  for (auto & p : performance)
  {
    ATH_MSG_ALWAYS("SeedPerformance for: " << p.first);
    ATH_MSG_ALWAYS("efficiency: " << p.second.first << ", purity:" << p.second.second << ", product: " << 
		   p.second.first*p.second.second);
    float lossRate = ((float)m_nLost[p.first])/m_allTruth[p.first];
    float mergeRate = ((float)m_nMerge[p.first])/m_allTruth[p.first];
    float fakeRate = ((float)m_nFake[p.first])/m_allSeeds[p.first];
    float splitRate = ((float)m_nSplit[p.first])/m_allSeeds[p.first];
    float simpleEfficiency = ((float)m_goodTruth[p.first]/m_allTruth[p.first]);
    float simplePurity = ((float)m_goodSeeds[p.first]/m_allSeeds[p.first]);
    ATH_MSG_ALWAYS("Simple efficiency: " << simpleEfficiency << ", Simple purity: " << simplePurity);
    ATH_MSG_ALWAYS("Loss: " << lossRate << ", Merge: " << mergeRate << ", Fake: " << fakeRate << ", Split:" << splitRate);
    ATH_MSG_ALWAYS("----------------------");
    nPrint++;
    if (nPrint >= 0.01*performance.size()) break;
  }
  return StatusCode::SUCCESS;
}

StatusCode ImagingSeedTuningAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::ReadHandle<xAOD::TrackParticleContainer> trackParticles(m_trackParticlesKey);

  std::vector<Trk::ITrackLink*> trackVector;
  selectTracks(trackParticles.cptr(), trackVector);

  std::vector<const Trk::TrackParameters*> perigeeList;
  analyzeTracks(trackVector, perigeeList);

  std::vector<Amg::Vector3D> truth;
  CHECK( findTruth(trackVector, truth) );
  m_h_nTruthVertices->Fill((float) truth.size());
  m_b_nTruth = truth.size();
  for (auto & v : truth) m_h_zTruthVertices->Fill(v[2]);

  xAOD::Vertex theConstraint;
  theConstraint.makePrivateStore();
  if (m_useBeamConstraint)
  {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    theConstraint.setPosition(beamSpotHandle->beamVtx().position());
    theConstraint.setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());
    theConstraint.setFitQuality(beamSpotHandle->beamVtx().fitQuality().chiSquared(), beamSpotHandle->beamVtx().fitQuality().doubleNumberDoF());
  }

  bool done = true;
  std::string conditions;
  CHECK( initializeConditions(conditions) );
  do
  {
    std::vector<Amg::Vector3D> seeds;
    if (m_useBeamConstraint)
    {
      seeds = m_seedFinder->findMultiSeeds(perigeeList, &theConstraint);
    } else {
      seeds = m_seedFinder->findMultiSeeds(perigeeList);
    }
    analyzeSeeds(conditions, seeds, truth);
    CHECK( iterateConditions(conditions, done) );
  } while (!done);

  m_t_seeds->Fill();

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode ImagingSeedTuningAlg::initializeConditions(std::string& conditions)
{
  m_i_angularCutoffParameter = m_angularCutoffParameterValues.begin();
  m_i_clusterWindowXY        = m_clusterWindowXYValues.begin();
  m_i_mergeParameter         = m_mergeParameterValues.begin();
  m_i_weightThreshold        = m_weightThresholdValues.begin();
  m_i_refineZ                = m_refineZValues.begin();
  m_i_gaussianWindow         = m_gaussianWindowValues.begin();

  m_iCondition = 0;

  CHECK( setupConditions(conditions) );

  return StatusCode::SUCCESS;
}

StatusCode ImagingSeedTuningAlg::iterateConditions(std::string& conditions, bool& done)
{
  done = true;
  conditions = "done";
  m_iCondition++;

  if (++m_i_angularCutoffParameter == m_angularCutoffParameterValues.end())
  {
    m_i_angularCutoffParameter = m_angularCutoffParameterValues.begin();
    if (++m_i_clusterWindowXY == m_clusterWindowXYValues.end())
    {
      m_i_clusterWindowXY = m_clusterWindowXYValues.begin();
      if (++m_i_mergeParameter == m_mergeParameterValues.end())
      {
	m_i_mergeParameter = m_mergeParameterValues.begin();
	if (++m_i_weightThreshold == m_weightThresholdValues.end())
	{
	  m_i_weightThreshold = m_weightThresholdValues.begin();
	  if (++m_i_refineZ == m_refineZValues.end())
	  {
	    m_i_refineZ = m_refineZValues.begin();
	    if (++m_i_gaussianWindow == m_gaussianWindowValues.end())
	    {
	      return StatusCode::SUCCESS;
	    }
	  }
	}
      }
    }
  }

  CHECK( setupConditions(conditions) );
  done = false;
  return StatusCode::SUCCESS;
}

StatusCode ImagingSeedTuningAlg::setupConditions(std::string& conditions)
{
  m_b_gaussianWindow[m_iCondition] = *m_i_gaussianWindow;
  m_b_refineZ[m_iCondition] = *m_i_refineZ;
  m_b_weightThreshold[m_iCondition] = *m_i_weightThreshold;
  m_b_mergeParameter[m_iCondition] = *m_i_mergeParameter;
  m_b_clusterWindowXY[m_iCondition] = *m_i_clusterWindowXY;
  m_b_angularCutoffParameter[m_iCondition] = *m_i_angularCutoffParameter;

  std::string gaussianWindow = std::to_string(*m_i_gaussianWindow);
  std::string refineZ = std::to_string(*m_i_refineZ);
  std::string weightThreshold = std::to_string(*m_i_weightThreshold);
  std::string mergeParameter = std::to_string(*m_i_mergeParameter);
  std::string clusterWindowXY = std::to_string(*m_i_clusterWindowXY);
  std::string angularCutoffParameter = std::to_string(*m_i_angularCutoffParameter);

  CHECK( m_imageMakerProperty->setProperty("angularCutoffParameter", angularCutoffParameter) );
  CHECK( m_clusterFinderProperty->setProperty("clusterWindowXY", clusterWindowXY) );
  CHECK( m_clusterFinderProperty->setProperty("mergeParameter", mergeParameter) );
  CHECK( m_clusterFinderProperty->setProperty("weightThreshold", weightThreshold) );
  CHECK( m_clusterFinderProperty->setProperty("refineZ", refineZ) );
  CHECK( m_clusterFinderProperty->setProperty("gaussianWindow", gaussianWindow) );

  conditions = "\ngaussianWindow:";
  conditions += gaussianWindow;
  conditions += "\nrefineZ:";
  conditions += refineZ;
  conditions += "\nweightThreshold:";
  conditions += weightThreshold;
  conditions += "\nmergeParameter:";
  conditions += mergeParameter;
  conditions += "\nclusterWindowXY:";
  conditions += clusterWindowXY;
  conditions += "\nangularCutoffParameter:";
  conditions += angularCutoffParameter;

  return StatusCode::SUCCESS;
}

void ImagingSeedTuningAlg::analyzeSeeds(const std::string& conditions,
					const std::vector<Amg::Vector3D>& seeds, 
					const std::vector<Amg::Vector3D>& truth)
{
  // simple analysis - count seeds that have a true vertex close to them
  m_allTruth[conditions] += truth.size();
  m_allSeeds[conditions] += seeds.size();
  m_b_nSeeds[m_iCondition] = seeds.size();
  int goodSeeds = 0;
  for (auto& seed : seeds) {
    for (auto& tru : truth) {
      if (std::abs(seed[2] - tru[2]) <= m_truthWindow) {
	goodSeeds++;
	break;
      }
    }
  }
  // simple analysis - count truth vertices that have a seed close to them
  m_goodSeeds[conditions] += goodSeeds;
  m_b_nGoodSeeds[m_iCondition] = goodSeeds;
  int goodTruth = 0;
  for (auto& tru : truth) {
    for (auto& seed : seeds) {
      if (std::abs(seed[2] - tru[2]) <= m_truthWindow){
	goodTruth++;
	break;
      }
    }
  }
  m_goodTruth[conditions] += goodTruth;
  m_b_nGoodTruth[m_iCondition] = goodTruth;

  // more careful analysis - match seeds and truth
  std::map<float, float > nearestSeed;
  std::map<float, float > nearestTruth;
  m_b_nLost[m_iCondition] = 0;
  m_b_nFake[m_iCondition] = 0;
  m_b_nSplit[m_iCondition] = 0;
  m_b_nMerge[m_iCondition] = 0;
  for (auto & tru : truth) {
    nearestSeed[tru[2]] = std::numeric_limits<float>::infinity();
    for (auto & seed : seeds) {
      if (std::abs(seed[2] - tru[2]) < 
	  std::min(m_truthWindow, std::abs(nearestSeed[tru[2]] - tru[2])))
      {
	nearestSeed[tru[2]] = seed[2];
      }
    }
    // if a true vertex is not close to any seed, it is "lost"
    if (nearestSeed[tru[2]] == std::numeric_limits<float>::infinity())
    {
      m_b_nLost[m_iCondition]++;
      m_nLost[conditions]++;
    }
  }
  for (auto & seed : seeds) {
    nearestTruth[seed[2]] = std::numeric_limits<float>::infinity();
    for (auto & tru : truth) {
      if (std::abs(tru[2] - seed[2]) < 
	  std::min(m_truthWindow, std::abs(nearestTruth[seed[2]] - seed[2])))
      {
	nearestTruth[seed[2]] = tru[2];
      }
    }
    // if a seed is not close to any true vertex, it is a "fake"
    if (nearestTruth[seed[2]] == std::numeric_limits<float>::infinity())
    {
      m_b_nFake[m_iCondition]++;
      m_nFake[conditions]++;
    }
  }
  // if a true vertex is closest to > 1 seed, that true vertex has been "split"
  for (auto & tru : truth)
  {
    if (nearestSeed[tru[2]] == std::numeric_limits<float>::infinity()) continue;
    int nClosest = 0; // number of seeds that this true vertex is closest to
    for (auto & seed : seeds)
    {
      if (nearestTruth[seed[2]] == tru[2]) nClosest++;
    }
    if (nClosest > 1) 
    {
      m_nSplit[conditions] += (nClosest - 1);
      m_b_nSplit[m_iCondition] += (nClosest - 1);
    }
  }
  // if a seed is closest to > 1 true vertex, those true vertices have been "merged"
  for (auto & seed : seeds)
  {
    if (nearestTruth[seed[2]] == std::numeric_limits<float>::infinity()) continue;
    int nClosest = 0; // number of truth vertices that this seed is closest to
    for (auto & tru : truth)
    {
      if (nearestSeed[tru[2]] == seed[2]) nClosest++;
    }
    if (nClosest > 1) 
    {
      m_nMerge[conditions] += (nClosest - 1);
      m_b_nMerge[m_iCondition] += (nClosest - 1);
    }
  }
}

void ImagingSeedTuningAlg::analyzeTracks(const std::vector<Trk::ITrackLink*>& trackVector,
					 std::vector<const Trk::TrackParameters*>& perigeeList)
{
  for (auto seedtrkAtVtxIter = trackVector.begin();
       seedtrkAtVtxIter != trackVector.end();
       ++seedtrkAtVtxIter) 
  {
    perigeeList.push_back( (*seedtrkAtVtxIter)->parameters() );
  }
}

void ImagingSeedTuningAlg::selectTracks(const xAOD::TrackParticleContainer* trackParticles, 
					std::vector<Trk::ITrackLink*>& trackVector)
{
  bool selectionPassed{false};
  const InDet::BeamSpotData* beamdata = nullptr;
  if(m_useBeamConstraint){
     SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
     beamdata =  beamSpotHandle.retrieve();
  }
  for (auto itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
    if (m_useBeamConstraint) {
      xAOD::Vertex beamposition;
      beamposition.makePrivateStore();
      beamposition.setPosition(beamdata->beamVtx().position());
      beamposition.setCovariancePosition(beamdata->beamVtx().covariancePosition());
      selectionPassed=static_cast<bool> (m_trackFilter->accept(**itr,&beamposition));
    }
    else
    {
      xAOD::Vertex null;
      null.makePrivateStore();
      null.setPosition(Amg::Vector3D(0,0,0));
      AmgSymMatrix(3) vertexError;
      vertexError.setZero();
      null.setCovariancePosition(vertexError);
      selectionPassed=static_cast<bool>(m_trackFilter->accept(**itr,&null));
    }
    if (selectionPassed)
    {
      ElementLink<xAOD::TrackParticleContainer> link;
      link.setElement(*itr);
      Trk::LinkToXAODTrackParticle * linkTT = new Trk::LinkToXAODTrackParticle(link);
      linkTT->setStorableObject(*trackParticles);
      trackVector.push_back(linkTT);
    }
  }
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
StatusCode ImagingSeedTuningAlg::findTruth(const std::vector<Trk::ITrackLink*>& trackVector, std::vector<Amg::Vector3D>& truth) const
{
    xAOD::TrackParticle::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > truthParticleAssoc("truthParticleLink");

    SG::ReadHandle<xAOD::TruthEventContainer> signalEvents(m_truthEventsKey);

    for (const xAOD::TruthEventBase* evt : *signalEvents)
    {
      const xAOD::TruthVertex* vLink = *(evt->truthVertexLink(0));
      Amg::Vector3D vTruth(Amg::Vector3D(vLink->x(),vLink->y(),vLink->z()));
      int nGoodTracks = 0;
      for (auto trk : trackVector)
      {
	Trk::LinkToXAODTrackParticle* lxtp = dynamic_cast<Trk::LinkToXAODTrackParticle*>(trk);
        if (lxtp)
	{
	  bool isAssoc = truthParticleAssoc(**(*lxtp)).isValid();
	  if (isAssoc)
	  {
	    auto assocParticle = truthParticleAssoc(**(*lxtp));
	    for (const auto& truthParticle : evt->truthParticleLinks())
	    {
	      if (assocParticle == truthParticle)
	      {
		double error;
		double distance = distanceAndError(trk->parameters(), &vTruth, error);
		if (distance < m_significanceTruthCut * error) nGoodTracks++;
		break;
	      }
	    }
	  }
	}
	if (nGoodTracks >= m_truthVertexTracks)
	{
	  truth.push_back(vTruth);
	  break;
	}
      }
    }
    SG::ReadHandle<xAOD::TruthPileupEventContainer> pileupEvents(m_pileupEventsKey);

    for (const xAOD::TruthEventBase* evt : *pileupEvents)
    {
      const xAOD::TruthVertex* vLink = *(evt->truthVertexLink(0));
      Amg::Vector3D vTruth(Amg::Vector3D(vLink->x(),vLink->y(),vLink->z()));
      int nGoodTracks = 0;
      for (auto trk : trackVector)
      {
	Trk::LinkToXAODTrackParticle* lxtp = dynamic_cast<Trk::LinkToXAODTrackParticle*>(trk);
	bool isAssoc = truthParticleAssoc(**(*lxtp)).isValid();
	if (isAssoc)
	{
	  auto assocParticle = truthParticleAssoc(**(*lxtp));
	  for (const auto& truthParticle : evt->truthParticleLinks())
	  {
	    if (assocParticle == truthParticle)
	    {
	      double error;
	      double distance = distanceAndError(trk->parameters(), &vTruth, error);
	      if (distance < m_significanceTruthCut * error) nGoodTracks++;
	      break;
	    }
	  }
	}
	if (nGoodTracks >= m_truthVertexTracks)
	{
	  truth.push_back(vTruth);
	  break;
	}
      }
    }
    return StatusCode::SUCCESS;
}

double ImagingSeedTuningAlg::distanceAndError(const Trk::TrackParameters* params, const Amg::Vector3D * vertex, double & error) const
{
    //find distance safely
    double distance=0.;
    try {
      std::unique_ptr<Trk::PlaneSurface> mySurface=m_impactPoint3dEstimator->Estimate3dIP(params,vertex,distance);
    }
    catch (error::ImpactPoint3dEstimatorProblem err) {
      msg(MSG::WARNING) << " ImpactPoin3dEstimator failed to find minimum distance between track and vertex seed: " << 
        err.p << endmsg;
    }
    if (distance<0) {
      msg(MSG::WARNING) << " Distance between track and seed vtx is negative: " << distance << endmsg;
    }
    //very approximate error
    error= 0.;
  
    if(params) {
      error = std::sqrt( (*params->covariance())(Trk::d0,Trk::d0) + (*params->covariance())(Trk::z0,Trk::z0) );
    }
      
    if (error==0.) {
      msg(MSG::ERROR) << " Error is zero! " << distance << endmsg;
      error=1.;
    }
    msg(MSG::VERBOSE) << " Distance between track and seed vtx: " << distance << " d/s(d) = " << 
      distance/error << " err " << error << endmsg;

    return distance;
}


} // namespace
