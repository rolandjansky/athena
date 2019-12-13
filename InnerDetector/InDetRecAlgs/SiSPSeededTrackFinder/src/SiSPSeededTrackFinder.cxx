/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SiSPSeededTrackFinder/SiSPSeededTrackFinder.h"

#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include <set>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSPSeededTrackFinder::SiSPSeededTrackFinder
(const std::string& name,ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
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

  // optional PRD to track association map
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty() ) );

  // Get tool for space points seed maker
  //
  ATH_CHECK( m_seedsmaker.retrieve() );
  ATH_CHECK( m_zvertexmaker.retrieve( DisableTool{ not m_useZvertexTool } ));

  // Get track-finding tool
  //
  ATH_CHECK( m_trackmaker.retrieve());

  ATH_CHECK( m_trackSummaryTool.retrieve( DisableTool{ m_trackSummaryTool.name().empty()} ));

  if (m_useNewStrategy and m_beamSpotKey.key().empty()) {
    m_useNewStrategy = false;
    m_useZBoundaryFinding = false;
  }

  if (m_useNewStrategy or m_useZBoundaryFinding or m_ITKGeometry) {

    if (not m_beamSpotKey.key().empty()) {
      ATH_CHECK(m_beamSpotKey.initialize());
      // Get RungeKutta propagator tool
      //
      ATH_CHECK( m_proptool.retrieve() );

      // Setup for magnetic field
      //
      magneticFieldInit();
      
      // Setup for Z-histograms
      //  
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

  // Get output print level
  //
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
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::execute(const EventContext& ctx) const
{ 
  if (not m_useNewStrategy and not m_useZBoundaryFinding and not m_ITKGeometry) {
    return oldStrategy(ctx);
  }
  return newStrategy(ctx);
}

///////////////////////////////////////////////////////////////////
// Execute with old strategy
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
  // For HI events we can use MBTS information from calorimeter
  //
  if (not isGoodEvent(ctx)) {
    return StatusCode::SUCCESS;
  }

  SiSpacePointsSeedMakerEventData seedEventData;
  bool ZVE = false;
  if (m_useZvertexTool) {
    std::list<Trk::Vertex> vertices = m_zvertexmaker->newEvent(seedEventData);
    if (not vertices.empty()) ZVE = true;
    m_seedsmaker->find3Sp(seedEventData, vertices);
  } else {
    m_seedsmaker->newEvent(seedEventData, -1);
    std::list<Trk::Vertex> VZ;
    m_seedsmaker->find3Sp(seedEventData, VZ);
  }

  const bool PIX = true;
  const bool SCT = true;
  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(m_prdToTrackMap);
  m_trackmaker->newEvent(trackEventData, PIX, SCT);

  bool ERR = false;
  Counter_t counter{};
  const InDet::SiSpacePointsSeed* seed = nullptr;
  std::multimap<double, Trk::Track*> qualityTrack;
  // Loop through all seed and reconsrtucted tracks collection preparation
  //
  while ((seed = m_seedsmaker->next(seedEventData))) {
    ++counter[kNSeeds];
    std::list<Trk::Track*> trackList = m_trackmaker->getTracks(trackEventData, seed->spacePoints());
    for (Trk::Track* t: trackList) {
      qualityTrack.insert(std::make_pair(-trackQuality(t), t));
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
  filterSharedTracks(qualityTrack);

  // Save good tracks in track collection
  //
  for (std::pair<double, Trk::Track*> q: qualityTrack) {
    ++counter[kNTracks];
    if (m_trackSummaryTool.isEnabled()) {
       m_trackSummaryTool->computeAndReplaceTrackSummary(*(q.second),
                                                         trackEventData.combinatorialData().PRDtoTrackMap(),
                                                         false /* DO NOT suppress hole search*/);
    }
    outputTracks->push_back(q.second);
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
  // For HI events we can use MBTS information from calorimeter
  //
  if (not isGoodEvent(ctx)) {
    return StatusCode::SUCCESS;
  }

  // Get beam information and preparation for z -histogramming
  //
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{m_beamSpotKey, ctx};
  Trk::PerigeeSurface per(beamSpotHandle->beamPos());

  SiSpacePointsSeedMakerEventData seedEventData;
 
  m_seedsmaker->newEvent(seedEventData, 0);
  std::list<Trk::Vertex> VZ;
  m_seedsmaker->find3Sp(seedEventData, VZ);

  const bool PIX = true ;
  const bool SCT = true ;
  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(m_prdToTrackMap);
  m_trackmaker->newEvent(trackEventData, PIX, SCT);

  std::vector<int> nhistogram(m_histsize, 0);
  std::vector<double> zhistogram(m_histsize, 0.);
  std::vector<double> phistogram(m_histsize, 0.);

  bool ERR = false;
  Counter_t counter{};
  const InDet::SiSpacePointsSeed* seed = nullptr;
  std::multimap<double, Trk::Track*> qualityTrack;
  // Loop through all seed and reconsrtucted tracks collection preparation
  //
  while ((seed = m_seedsmaker->next(seedEventData))) {
    ++counter[kNSeeds];
    bool firstTrack{true};
    std::list<Trk::Track*> trackList = m_trackmaker->getTracks(trackEventData, seed->spacePoints());
    for (Trk::Track* t: trackList) {
      qualityTrack.insert(std::make_pair(-trackQuality(t), t));

      if (firstTrack and not m_ITKGeometry) {
        fillZHistogram(t, per, nhistogram, zhistogram, phistogram);
      }
      firstTrack = false;
    }
    if (counter[kNSeeds] >= m_maxNumberSeeds) {
      ERR = true;
      ++m_problemsTotal;
      break;
    }
  }

  m_seedsmaker->newEvent(seedEventData, 1);

  double ZB[2];
  if (not m_ITKGeometry) {
    findZvertex(VZ, ZB, nhistogram, zhistogram, phistogram);
    m_seedsmaker->find3Sp(seedEventData, VZ, ZB);
  } else {
    m_seedsmaker->find3Sp(seedEventData, VZ);
  }

  // Loop through all seed and reconsrtucted tracks collection preparation
  //
  while ((seed = m_seedsmaker->next(seedEventData))) {
    ++counter[kNSeeds];
    for (Trk::Track* t: m_trackmaker->getTracks(trackEventData, seed->spacePoints())) {
      qualityTrack.insert(std::make_pair(-trackQuality(t), t));
    }
    if (counter[kNSeeds] >= m_maxNumberSeeds) {
      ERR = true;
      ++m_problemsTotal;
      break;
    }
  }
  m_trackmaker->endEvent(trackEventData);

  // Remove shared tracks with worse quality
  //
  filterSharedTracks(qualityTrack);

  // Save good tracks in track collection
  //
  for (std::pair<double, Trk::Track*> q: qualityTrack) {
    ++counter[kNTracks];
    if (m_trackSummaryTool.isEnabled()) {
       m_trackSummaryTool->computeAndReplaceTrackSummary(*q.second,
                                                         trackEventData.combinatorialData().PRDtoTrackMap(),
                                                         false /* DO NOT suppress hole search*/);
    }
    outputTracks->push_back(q.second);
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
  if (msgLvl(MSG::INFO)) {
    dump(MSG::INFO, &m_counterTotal);
  }
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
    if (not spacePointsPixel.isValid()) {
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
    if (not spacePointsSCT.isValid()) {
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
 double W       = 17.;

 for (const Trk::TrackStateOnSurface* m: *(Tr->trackStateOnSurfaces())) {

   if (not m->type(Trk::TrackStateOnSurface::Measurement)) continue;

   const Trk::FitQualityOnSurface* fq = m->fitQualityOnSurface();
   if (fq==nullptr) continue;

   double x2 = fq->chiSquared();
   double q;
   if (fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
   else                      q =      (W-x2    );
   if (q < 0.) q = 0.;
   quality += q;
 }

 if (Tr->info().trackProperties(Trk::TrackInfo::BremFit)) quality *= 0.7;

 return quality;
}

///////////////////////////////////////////////////////////////////
// Filer shared tracks
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::filterSharedTracks(std::multimap<double, Trk::Track*>& QT) const
{
  std::set<const Trk::PrepRawData*> clusters;
  
  const Trk::PrepRawData* prd[100]; 
  
  std::multimap<double, Trk::Track*>::iterator q = QT.begin();
  while (q!=QT.end()) {

    std::set<const Trk::PrepRawData*>::iterator fe = clusters.end();

    int nf = 0;
    int nc = 0; 

    for (const Trk::MeasurementBase* m: *((*q).second->measurementsOnTrack())) {
      const Trk::PrepRawData* pr = (static_cast<const Trk::RIO_OnTrack*>(m))->prepRawData();
      if (pr) {
	++nc;
        if (clusters.find(pr)==fe) {
          prd[nf++]=pr;
          if (nf==100) break;
        }
      }
    }

    if (nf >= m_nfreeCut or nf==nc) {
      for (int n=0; n<nf; ++n) clusters.insert(prd[n]);
      ++q;
    } else {
      delete (*q).second;
      QT.erase(q++);
    }
  }
}

///////////////////////////////////////////////////////////////////
// Fill z coordinate histogram
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::fillZHistogram(const Trk::Track* Tr,
                                                  Trk::PerigeeSurface& per,
                                                  std::vector<int>& nhistogram,
                                                  std::vector<double>& zhistogram,
                                                  std::vector<double>& phistogram) const
{
  
  if (Tr->measurementsOnTrack()->size() < 10) return;
	
  DataVector<const Trk::TrackStateOnSurface>::const_iterator m = Tr->trackStateOnSurfaces()->begin();
  const Trk::TrackParameters* P = (*m)->trackParameters();
  Amg::Vector3D              gf = P->position()          ;
  Amg::Vector3D              gm = P->momentum()          ;
      
  if (gf.x()*gf.x()+gf.y()*gf.y() >= 3600.) return;

  double pT = sqrt(gm.x()*gm.x()+gm.y()*gm.y());
  if (pT < m_pTcut) return;

  Trk::PatternTrackParameters TP;
  if (not TP.production(P)) return;
	  
  double step;
  if (not m_proptool->propagate(TP, per, TP, Trk::anyDirection, m_fieldprop, step, Trk::pion)) return;
	      
  const double* par = TP.par();
  if (fabs(par[0]) > m_imcut) return;
  int z = static_cast<int>((par[1]+m_zcut)*m_zstep);
  if (z >=0 and z < m_histsize) {
    ++nhistogram[z];
    zhistogram[z] += par[1];
    phistogram[z] += pT;
  }
  
}

///////////////////////////////////////////////////////////////////
// Find verteex  z coordinates
///////////////////////////////////////////////////////////////////

void  InDet::SiSPSeededTrackFinder::findZvertex(std::list<Trk::Vertex>& ZV,
                                                double* ZB,
                                                std::vector<int>& nhistogram,
                                                std::vector<double>& zhistogram,
                                                std::vector<double>& phistogram) const
{
  ZB[0] = 1000.;
  ZB[1] =-1000.;

  std::multimap<int   ,double> vern;
  std::multimap<double,double> verp;

  int imax = m_histsize-1;
  int nmin = 3;

  for (int i=1; i<imax; ++i) { 

    int n = nhistogram[i-1]+nhistogram[i]+nhistogram[i+1];

    if (n>=nmin and (nhistogram[i] >= nhistogram[i-1] and nhistogram[i] >= nhistogram[i+1])) {
  
      double z = (zhistogram[i-1]+zhistogram[i]+zhistogram[i+1])/static_cast<double>(n);

      if (z < ZB[0]) ZB[0] = z;
      if (z > ZB[1]) ZB[1] = z;
 
      if (m_useNewStrategy) { 
	double p = phistogram[i-1]+phistogram[i]+phistogram[i+1];
	vern.insert(std::make_pair(-n, z));
	verp.insert(std::make_pair(-p, z));
      }
    }
  }

  if (m_useNewStrategy) {

    std::set<double> vertex;
    int n = 0;
    std::multimap<double, double>::iterator pz = verp.begin();
    for (std::pair<int, double> nz: vern) {
      if (n++ >= m_nvertex) break; 
      vertex.insert(nz.second);
      vertex.insert((*pz++).second);
    }

    for (double v: vertex) {
      Amg::Vector3D Vp(0., 0., v);
      Trk::Vertex Ver(Vp);
      ZV.push_back(Ver);
    }
  }

  if (ZB[0] > ZB[1]) {
    ZB[0] = -1000.;
    ZB[1] = +1000.;
  } else {
    ZB[0] -= 20.;
    ZB[1] += 20.;
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
