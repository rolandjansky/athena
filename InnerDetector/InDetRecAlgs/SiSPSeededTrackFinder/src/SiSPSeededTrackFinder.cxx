/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SiSPSeededTrackFinder/SiSPSeededTrackFinder.h"

#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include <set>

///////////////////////////////////////////////////////////////////
/// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSPSeededTrackFinder::SiSPSeededTrackFinder
(const std::string& name,ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
{
}

///////////////////////////////////////////////////////////////////
/// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::initialize() 
{
  ATH_CHECK(m_evtKey.initialize());
  if (not m_SpacePointsPixelKey.empty()) {
    ATH_CHECK(m_SpacePointsPixelKey.initialize());
  }
  if (not m_SpacePointsSCTKey.empty()) {
    ATH_CHECK(m_SpacePointsSCTKey.initialize());
  }
  ATH_CHECK(m_outputTracksKey.initialize());

  /// optional PRD to track association map
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty() ) );

  /// Get tool for space points seed maker
  ATH_CHECK( m_seedsmaker.retrieve() );
  ATH_CHECK( m_zvertexmaker.retrieve( DisableTool{ not m_useZvertexTool } ));

  /// Get track-finding tool
  ATH_CHECK( m_trackmaker.retrieve());

  ATH_CHECK( m_trackSummaryTool.retrieve( DisableTool{ m_trackSummaryTool.name().empty()} ));

  if (m_useNewStrategy and m_beamSpotKey.key().empty()) {
    m_useNewStrategy = false;
    m_useZBoundaryFinding = false;
  }

  if (m_useNewStrategy or m_useZBoundaryFinding or m_ITKGeometry) {

    if (not m_beamSpotKey.key().empty()) {
      ATH_CHECK(m_beamSpotKey.initialize());
      /// Get RungeKutta propagator tool
      ATH_CHECK( m_proptool.retrieve() );

      /// Setup for magnetic field
      magneticFieldInit();
      
      /// Setup for Z-histograms
      if (m_histsize < 100) m_histsize = 100;
      m_zstep = static_cast<double>(m_histsize)/(2.*m_zcut);
    } else {
      m_proptool.disable();
      m_useNewStrategy = false;
      m_useZBoundaryFinding = false;
    }
  } else {
    m_proptool.disable();
  }

  /// Get output print level
  if (msgLvl(MSG::DEBUG)) {
    dump(MSG::DEBUG, nullptr);
  }
  m_neventsTotal   = 0;
  m_neventsTotalV  = 0;
  m_problemsTotal  = 0;
  m_problemsTotalV = 0;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
/// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::execute(const EventContext& ctx) const
{ 
  /** Note that !m_useNewStrategy alone is NOT sufficient to trigger the oldStrategy call here! 
  * For example, run-3 central offline Si tracking has m_useNewStrategy=false, 
  * but m_useZBoundaryFinding true --> newStrategy
  **/
  if (not m_useNewStrategy and not m_useZBoundaryFinding and not m_ITKGeometry) {
    return oldStrategy(ctx);
  }
  return newStrategy(ctx);
}

///////////////////////////////////////////////////////////////////
/// Execute with old strategy
///////////////////////////////////////////////////////////////////

namespace InDet {
  class ExtendedSiTrackMakerEventData_xk : public InDet::SiTrackMakerEventData_xk
  {
  public:
    ExtendedSiTrackMakerEventData_xk(const SG::ReadHandleKey<Trk::PRDtoTrackMap> &key) { 
      if (!key.key().empty()) {
        m_prdToTrackMap = SG::ReadHandle<Trk::PRDtoTrackMap>(key);
        setPRDtoTrackMap(m_prdToTrackMap.cptr());
      }
    }
  private:
    void dummy() {}
    SG::ReadHandle<Trk::PRDtoTrackMap> m_prdToTrackMap;
  };
}


StatusCode InDet::SiSPSeededTrackFinder::oldStrategy(const EventContext& ctx) const
{
  SG::WriteHandle<TrackCollection> outputTracks{m_outputTracksKey, ctx};
  ATH_CHECK(outputTracks.record(std::make_unique<TrackCollection>()));
  /// For HI events we can use MBTS information from calorimeter
  if (not isGoodEvent(ctx)) {
    return StatusCode::SUCCESS;
  }

  SiSpacePointsSeedMakerEventData seedEventData;
  bool ZVE = false;
  if (m_useZvertexTool) {
    std::list<Trk::Vertex> vertices = m_zvertexmaker->newEvent(ctx, seedEventData);
    if (not vertices.empty()) ZVE = true;
    m_seedsmaker->find3Sp(ctx, seedEventData, vertices);
  } else {
    m_seedsmaker->newEvent(ctx, seedEventData, -1);
    std::list<Trk::Vertex> vertexList;
    m_seedsmaker->find3Sp(ctx, seedEventData, vertexList);
  }

  const bool PIX = true;
  const bool SCT = true;
  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(m_prdToTrackMap);
  m_trackmaker->newEvent(ctx, trackEventData, PIX, SCT);

  bool ERR = false;
  Counter_t counter{};
  const InDet::SiSpacePointsSeed* seed = nullptr;
  std::multimap<double, Trk::Track*> qualitySortedTrackCandidates;
  // Loop through all seed and reconsrtucted tracks collection preparation
  //
  while ((seed = m_seedsmaker->next(ctx, seedEventData))) {
    ++counter[kNSeeds];
    std::list<Trk::Track*> trackList = m_trackmaker->getTracks(ctx, trackEventData, seed->spacePoints());
    for (Trk::Track* t: trackList) {
      qualitySortedTrackCandidates.insert(std::make_pair(-trackQuality(t), t));
    }
    if (not ZVE and (counter[kNSeeds] >= m_maxNumberSeeds)) {
      ERR = true;
      ++m_problemsTotal;
      break;
    }
  }
  m_trackmaker->endEvent(trackEventData);

  // Remove shared tracks with worse quality
  //
  filterSharedTracks(qualitySortedTrackCandidates);

  // Save good tracks in track collection
  //
  for (const std::pair<const double, Trk::Track*> & qualityAndTrack: qualitySortedTrackCandidates) {
    ++counter[kNTracks];
    if (m_trackSummaryTool.isEnabled()) {
       m_trackSummaryTool->computeAndReplaceTrackSummary(*(qualityAndTrack.second),
                                                         trackEventData.combinatorialData().PRDtoTrackMap(),
                                                         false /* DO NOT suppress hole search*/);
    }
    outputTracks->push_back(qualityAndTrack.second);
  }

  m_counterTotal[kNSeeds] += counter[kNSeeds];

  if (ZVE) ++m_neventsTotalV;
  else     ++m_neventsTotal;

  if (ERR) {
    outputTracks->clear();
  } else {
    m_counterTotal[kNTracks] += counter[kNTracks];
  }

  // Print common event information
  //
  if (msgLvl(MSG::DEBUG)) {
    dump(MSG::DEBUG, &counter);
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute with new strategy
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::newStrategy(const EventContext& ctx) const
{
  SG::WriteHandle<TrackCollection> outputTracks{m_outputTracksKey, ctx};
  ATH_CHECK(outputTracks.record(std::make_unique<TrackCollection>()));
  /// For HI events we can use MBTS information from calorimeter
  if (not isGoodEvent(ctx)) {
    return StatusCode::SUCCESS;
  }

  /// Get beam information and preparation for z -histogramming
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{m_beamSpotKey, ctx};
  Trk::PerigeeSurface beamPosPerigee(beamSpotHandle->beamPos());

  SiSpacePointsSeedMakerEventData seedEventData;
  
  /** 
   * We run two passes of seeding & track finding. 
   * 
   * First, we use strip seeds, perform finding with them and obtain an 
   * estimate of the beam spot region from the found tracks. 
   * 
   * Then, we run PPP-seeded tracking within the resulting z interval. 
   **/ 

  /**
   * Set up the first pass (strip seeds), and prepare to 
   * obtain a vertex Z estimate from the candidates we find 
   **/ 

  /// set up the seed maker for first pass 
  m_seedsmaker->newEvent(ctx, seedEventData, 0);
  std::list<Trk::Vertex> vertexList;
  /// and run seeding - starting with an empty list of vertices for the first pass
  m_seedsmaker->find3Sp(ctx, seedEventData, vertexList);

  const bool PIX = true ;
  const bool SCT = true ;
  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(m_prdToTrackMap);
  /// set up the track maker 
  m_trackmaker->newEvent(ctx, trackEventData, PIX, SCT);

  /// initialize empty histograms for the vertex estimate
  std::vector<int> numberHistogram(m_histsize, 0);
  std::vector<double> zWeightedHistogram(m_histsize, 0.);
  std::vector<double> ptWeightedHistogram(m_histsize, 0.);

  bool ERR = false;
  Counter_t counter{};
  const InDet::SiSpacePointsSeed* seed = nullptr;

  /// prepare a collection for the quality-sorted track canddiates
  std::multimap<double, Trk::Track*> qualitySortedTrackCandidates;

  /// Loop through all seeds from the first pass and attempt to form track candidates
  while ((seed = m_seedsmaker->next(ctx, seedEventData))) {
    ++counter[kNSeeds];
    /// we only want to fill the Z histo with the first candidate for each seed. 
    bool firstTrack{true};
    /// combinatorial track finding for one given seed 
    std::list<Trk::Track*> trackList = m_trackmaker->getTracks(ctx, trackEventData, seed->spacePoints());

    /// record found candidates
    for (Trk::Track* t: trackList) {
      qualitySortedTrackCandidates.insert(std::make_pair(-trackQuality(t), t));
      /// For the first (highest quality) track from each seed, populate the vertex finding histograms 
      if (firstTrack and not m_ITKGeometry) {
        fillZHistogram(t, beamPosPerigee, numberHistogram, zWeightedHistogram, ptWeightedHistogram);
      }
      firstTrack = false;
    }
    if (counter[kNSeeds] >= m_maxNumberSeeds) {
      ERR = true;
      ++m_problemsTotal;
      break;
    }
  }

  /** Now set up the second seeding pass, using pixel seeds
  * The seed maker will internally reconfigure itself based on
  * the "1" argument for the "iteration" argument in this call. 
  **/ 
  m_seedsmaker->newEvent(ctx, seedEventData, 1);

  /// perform vertex Z estimation and run second seeding pass
  std::pair<double,double> zBoundaries;
  if (not m_ITKGeometry) {
    /// Estimate a Z vertex interval and, if running the new strategy, also a list of the HS candidates 
    findZvertex(vertexList, zBoundaries, numberHistogram, zWeightedHistogram, ptWeightedHistogram);
    /// pass the Z boundary pair c-array-style to satisfy existing interfaces of the seeds maker family. 
    /// Trigger second seed finding pass (PPP) 
    m_seedsmaker->find3Sp(ctx, seedEventData, vertexList, &(zBoundaries.first));
  } else {
    m_seedsmaker->find3Sp(ctx, seedEventData, vertexList);
  }

  /// Again, loop over the newly found seeds and attempt to form track candidates
  while ((seed = m_seedsmaker->next(ctx, seedEventData))) {
    ++counter[kNSeeds];
    /// insert the new tracks into the quality-sorted list
    for (Trk::Track* t: m_trackmaker->getTracks(ctx, trackEventData, seed->spacePoints())) {
      qualitySortedTrackCandidates.insert(std::make_pair(-trackQuality(t), t));
    }
    if (counter[kNSeeds] >= m_maxNumberSeeds) {
      ERR = true;
      ++m_problemsTotal;
      break;
    }
  }
  m_trackmaker->endEvent(trackEventData);

  /// Remove shared tracks with worse quality
  filterSharedTracks(qualitySortedTrackCandidates);

  /// Save good tracks in track collection
  for (const std::pair<const double, Trk::Track*> & qualityAndTrack: qualitySortedTrackCandidates) {
    ++counter[kNTracks];
    if (m_trackSummaryTool.isEnabled()) {
      /// Note that for run-3 the tool here is configured to not perform a hole search,
      /// regardless of the 'false' argument below
       m_trackSummaryTool->computeAndReplaceTrackSummary(*qualityAndTrack.second,
                                                         trackEventData.combinatorialData().PRDtoTrackMap(),
                                                         false /* DO NOT suppress hole search*/);
    }
    outputTracks->push_back(qualityAndTrack.second);
  }

  m_counterTotal[kNSeeds] += counter[kNSeeds] ;

  ++m_neventsTotal;

  if (ERR) {
    outputTracks->clear();
  } else {
    m_counterTotal[kNTracks] += counter[kNTracks];
  }

  // Print common event information
  //
  if (msgLvl(MSG::DEBUG)) {
    dump(MSG::DEBUG, &counter);
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::finalize()
{
  
    dump(MSG::INFO, &m_counterTotal);
  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinder::dump(MSG::Level assign_level, const InDet::SiSPSeededTrackFinder::Counter_t* counter) const
{
  msg(assign_level) <<std::endl;
  MsgStream& out_msg=msg();
  if (counter) dumpevent(out_msg ,*counter);
  else dumptools(out_msg);
  out_msg << endmsg;
  return out_msg;
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinder::dumptools(MsgStream& out) const
{
  int n = 65-m_zvertexmaker.type().size();
  std::string s1; for (int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_seedsmaker.type().size();
  std::string s2; for (int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_trackmaker.type().size();
  std::string s3; for (int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 65-m_outputTracksKey.key().size();
  std::string s4; for (int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  std::string s5; 
  if (m_useZvertexTool) s5= "Yes"; else s5 = "No";
  n     = 65-s5.size(); for (int i=0; i<n; ++i) s5.append(" "); s5.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  out<<"| Use primary vertices z-coordinates      finding?| "<<s5
     <<std::endl;
  if (m_useZvertexTool) {
    out<<"| Tool for primary vertices z-coordinates finding | "<<m_zvertexmaker.type()<<s1
       <<std::endl;
  }
  out<<"| Tool for space points seeds             finding | "<<m_seedsmaker.type()<<s2
     <<std::endl;
  out<<"| Tool for space points seeded track      finding | "<<m_trackmaker.type()<<s3
     <<std::endl;
  out<<"| Location of output tracks                       | "<<m_outputTracksKey.key()<<s4
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinder::dumpevent(MsgStream& out, const InDet::SiSPSeededTrackFinder::Counter_t& counter) const
{
  out<<"|-------------------------------------------------------------------";
  out<<"---------------------------------|"
     <<std::endl;
  out<<"|  Investigated "
     <<std::setw(9)<<counter[kNSeeds]<<" space points seeds and found ";
  out<<std::setw(9)<<counter[kNTracks];
  if      (m_ITKGeometry        ) out<<" tracks using new strategy for ITK   |"<<std::endl; 
  else if (m_useNewStrategy     ) out<<" tracks using new strategy ("<<std::setw(2)<<  m_nvertex <<")      |"<<std::endl;
  else if (m_useZBoundaryFinding) out<<" tracks using old strategy with Zb   |"<<std::endl;
  else                            out<<" tracks using old strategy           |"<<std::endl;

  out<<"|-------------------------------------------------------------------";
  out<<"---------------------------------|"
     <<std::endl;
  if (m_problemsTotal or m_problemsTotalV) {
    out<<"|  Events       "
       <<std::setw(7)<<m_neventsTotal   <<"            without Z-vertz   "
       <<std::setw(7)<<m_neventsTotalV  <<"            with    Z-vertex         |"
       <<std::endl;
    out<<"|  Problems     "
       <<std::setw(7)<<m_problemsTotal  <<"            without Z-vertz   "
       <<std::setw(7)<<m_problemsTotalV <<"            with    Z-vertex         |"
       <<std::endl;
    out<<"|-------------------------------------------------------------------";
    out<<"-----------------------------|"
       <<std::endl;
  }
  return out;
}

///////////////////////////////////////////////////////////////////
// Test is it good event for reconstruction (mainly for HI events)
///////////////////////////////////////////////////////////////////

bool InDet::SiSPSeededTrackFinder::isGoodEvent(const EventContext& ctx) const {

  if (not m_useMBTS) return true;

  // Test MBTS information from calorimeter
  //
  SG::ReadHandle<xAOD::EventInfo> eventInfo{m_evtKey, ctx};
  if (not eventInfo->isEventFlagBitSet(xAOD::EventInfo::Background, xAOD::EventInfo::MBTSTimeDiffHalo)) {
    return true;
  }

  // Test total number pixels space points
  //
  unsigned int nsp = 0;
  if (not m_SpacePointsPixelKey.empty()) {
    SG::ReadHandle<SpacePointContainer> spacePointsPixel{m_SpacePointsPixelKey, ctx};
    if (spacePointsPixel.isValid()) {
      for (const SpacePointCollection* spc: *spacePointsPixel) {
        nsp += spc->size();
      }
      if (static_cast<int>(nsp) > m_maxPIXsp) {
        ATH_MSG_WARNING("Found more than "<<m_maxPIXsp<<" pixels space points in background event. Skip track finding");
        return false;
      }
    }
  }
 
  // Test total number sct space points
  //
  nsp = 0;
  if (not m_SpacePointsSCTKey.empty()) {
    SG::ReadHandle<SpacePointContainer> spacePointsSCT{m_SpacePointsSCTKey, ctx};
    if (spacePointsSCT.isValid()) {
      for (const SpacePointCollection* spc: *spacePointsSCT) {
        nsp += spc->size();
      }
      if (static_cast<int>(nsp) > m_maxSCTsp) {
        ATH_MSG_WARNING("Found more than "<<m_maxSCTsp<<" sct space points in background event. Skip track finding");
        return false;
      }
    }
  }

  return true;
}

///////////////////////////////////////////////////////////////////
// Track quality calculation
///////////////////////////////////////////////////////////////////

double InDet::SiSPSeededTrackFinder::trackQuality(const Trk::Track* Tr) const
{
 double quality = 0. ;
 double baseScorePerHit       = 17.;

  /// check all hits on the track
 for (const Trk::TrackStateOnSurface* m: *(Tr->trackStateOnSurfaces())) {
   /// exclude anything which is not an actual hit 
   if (not m->type(Trk::TrackStateOnSurface::Measurement)) continue;
  /// retrieve the fit quality for a given hit
   const Trk::FitQualityOnSurface* fq = m->fitQualityOnSurface();
   if (fq==nullptr) continue;
  
   double x2 = fq->chiSquared();
   double hitQualityScore;
   /// score the hit based on the technology (pixels get higher score) and 
   /// the local chi2 for the hit 
   if (fq->numberDoF() == 2) hitQualityScore = (1.2*(baseScorePerHit-x2*.5));   // pix
   else                      hitQualityScore =      (baseScorePerHit-x2    );   // sct
   if (hitQualityScore < 0.) hitQualityScore = 0.;    // do not allow a bad hit to decrement the overall score 
   quality += hitQualityScore;
 }
  /// penalise brem tracks 
 if (Tr->info().trackProperties(Trk::TrackInfo::BremFit)) quality *= 0.7;

 return quality;
}

///////////////////////////////////////////////////////////////////
// Filer shared tracks
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::filterSharedTracks(std::multimap<double, Trk::Track*>& qualitySortedTracks) const
{
  std::set<const Trk::PrepRawData*> clusters;
  
  std::vector<const Trk::PrepRawData*> freeClusters;
  freeClusters.reserve(15);    
  
  std::multimap<double, Trk::Track*>::iterator it_qualityAndTrack = qualitySortedTracks.begin();

  /// loop over all track candidates, sorted by quality
  while (it_qualityAndTrack!=qualitySortedTracks.end()) {
    freeClusters.clear();

    std::set<const Trk::PrepRawData*>::iterator it_clustersEnd = clusters.end();

    int nClusters = 0; 
    /// loop over measurements on the track candidate 
    for (const Trk::MeasurementBase* m: *((*it_qualityAndTrack).second->measurementsOnTrack())) {
      /// get the PRD from the measurement
      const Trk::PrepRawData* pr = (static_cast<const Trk::RIO_OnTrack*>(m))->prepRawData();
      if (pr) {
        /// increase cluster count
	      ++nClusters;
        /// and check if the cluster was already used in a previous ( = higher quality) track 
        if (clusters.find(pr)==it_clustersEnd) {
          /// if not, record as a free (not prevously used) cluster 
          freeClusters.push_back(pr); 
        }
      }
    }
    /// check if the track has the minimum number of free clusters or if it has no shared clusters 
    int nFreeClusters = static_cast<int>(freeClusters.size()); 
    if (nFreeClusters >= m_nfreeCut || nFreeClusters==nClusters) {
      /// if this is fulfilled, we keep the candidate 
      /// add the free clusters to our cluster set 
      clusters.insert(freeClusters.begin(), freeClusters.end());
      ++it_qualityAndTrack;
    } else {
      /// if we do not keep the track, clean up candidate 
      delete (*it_qualityAndTrack).second;
      qualitySortedTracks.erase(it_qualityAndTrack++);
    }
  }
}

///////////////////////////////////////////////////////////////////
// Fill z coordinate histogram
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::fillZHistogram(const Trk::Track* Tr,
                                                  const Trk::PerigeeSurface& beamPosPerigee,
                                                  std::vector<int>& numberHistogram,
                                                  std::vector<double>& zWeightedHistogram,
                                                  std::vector<double>& ptWeightedHistogram) const
{
  
  if (Tr->measurementsOnTrack()->size() < 10) return;
	
  const Trk::TrackParameters* paramsAtFirstSurface = Tr->trackStateOnSurfaces()->front()->trackParameters();
  Amg::Vector3D              position = paramsAtFirstSurface->position()          ;
  Amg::Vector3D              momentum = paramsAtFirstSurface->momentum()          ;
  
  /// only take into accounts tracks with a hit inside r < 60mm 
  constexpr double rSquare_max_forZHisto = 60.*60.; 
  if (position.x()*position.x()+position.y()*position.y() >= rSquare_max_forZHisto) return;

  double pT = sqrt(momentum.x()*momentum.x()+momentum.y()*momentum.y());
  if (pT < m_pTcut) return;

  Trk::PatternTrackParameters TP;
  if (not TP.production(paramsAtFirstSurface)) return;
	  
  double step;
  /// propagate from innermost hit to beam spot
  if (not m_proptool->propagate(TP, beamPosPerigee, TP, Trk::anyDirection, m_fieldprop, step, Trk::pion)) return;
	      
  const double* parsAtBeamSpot = TP.par();
  if (std::abs(parsAtBeamSpot[0]) > m_imcut) return;
  /// determine bin number - m_zstep is the inverse bin width, where the histo axis extends from -m_zcut to +m_zcut
  int z = static_cast<int>((parsAtBeamSpot[1]+m_zcut)*m_zstep);
  /// fill histograms if we are not in the over/underflow
  if (z >=0 and z < m_histsize) {
    /// simple z histogram, counting tracks per z 
    ++numberHistogram[z];
    /// z weighted histogram binned in z - used for vertex z calculation
    zWeightedHistogram[z] += parsAtBeamSpot[1];
    /// pt weighted histogram binned in z - used for vertex sumpt calculation 
    ptWeightedHistogram[z] += pT;
  }
  
}

///////////////////////////////////////////////////////////////////
// Find verteex  z coordinates
///////////////////////////////////////////////////////////////////

void  InDet::SiSPSeededTrackFinder::findZvertex(std::list<Trk::Vertex>& vertexZList,
                                                std::pair<double, double> & zBoundaries,
                                                const std::vector<int>& numberHistogram,
                                                const std::vector<double>& zWeightedHistogram,
                                                const std::vector<double>& ptWeightedHistogram) const
{
  zBoundaries = {1000., -1000};

  std::multimap<int   ,double> vertexZ_sortedByNtracks;
  std::multimap<double,double> vertexZ_sortedBySumPt;

  int lastBin = m_histsize-1;
  int minBinContentSum = 3;

  /// loop over the bins in z. Start with the second, end with the second to last, as we investigate triplets of bins
  for (int binIndex=1; binIndex<lastBin; ++binIndex) { 

    /// total entries in the three neighbouring bins 
    int vertexNtracks = numberHistogram.at(binIndex-1)+numberHistogram.at(binIndex)+numberHistogram.at(binIndex+1);

    /// if we have at least 3 tracks in this z0 interval with the peak of the local triplet in the central bin,
    /// add a vertex candidate
    if (vertexNtracks>=minBinContentSum and (numberHistogram.at(binIndex) >= numberHistogram.at(binIndex-1) and numberHistogram.at(binIndex) >= numberHistogram.at(binIndex+1))) {
      /// vertex z estimate as the mean z0 of the tracks in the three bins
      double vertexZestimate = (zWeightedHistogram.at(binIndex-1)+zWeightedHistogram.at(binIndex)+zWeightedHistogram.at(binIndex+1))/static_cast<double>(vertexNtracks);

      /// if this vertex is to the left of the lower edge or the right of the upper edge, broaden the z interval accordingly 
      /// to include this vertex. 
      if (vertexZestimate < zBoundaries.first) zBoundaries.first = vertexZestimate;
      if (vertexZestimate > zBoundaries.second) zBoundaries.second = vertexZestimate;
 
      if (m_useNewStrategy) { 
        /// also get the sum(pt) of the vertex candidate
        double vertexSumPt = ptWeightedHistogram.at(binIndex-1)+ptWeightedHistogram.at(binIndex)+ptWeightedHistogram.at(binIndex+1);
        vertexZ_sortedByNtracks.insert(std::make_pair(-vertexNtracks, vertexZestimate));
        vertexZ_sortedBySumPt.insert(std::make_pair(-vertexSumPt, vertexZestimate));
      }
    }
  }

  if (m_useNewStrategy) {

    std::set<double> leadingVertices;
    int n = 0;
    std::multimap<double, double>::iterator vertex_pt_and_z = vertexZ_sortedBySumPt.begin();
    for (std::pair<int, double> nTrackAndZ: vertexZ_sortedByNtracks) {
      /// stop when we have collected the requested number of leading vertices
      if (n++ >= m_nvertex) break; 
      /// insert one vertex from the ntracks-sorted list and one from the pt-sorted list. 
      /// If both are the same, only one insertion is performed
      leadingVertices.insert(nTrackAndZ.second);
      leadingVertices.insert((*vertex_pt_and_z++).second);
    }

    for (double v: leadingVertices) {
      vertexZList.emplace_back(Amg::Vector3D{0.,0.,v});
    }
  }
  /// if we did not find any PV candidates, assume a very wide interval
  if (zBoundaries.first > zBoundaries.second) {
    zBoundaries.first = -1000.;
    zBoundaries.second = +1000.;
  } else {
    /// otherwise, add a reasonable bit of space on each side of the first / last vertex candidate
    zBoundaries.first -= 20.;
    zBoundaries.second += 20.;
  }
} 


///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //
  if(m_fieldmode == "NoField") {
    m_fieldprop = Trk::MagneticFieldProperties(Trk::NoField);
  } else {
    m_fieldprop = Trk::MagneticFieldProperties(Trk::FastField);
  }
}
