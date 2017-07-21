/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <set>

#include "SiSPSeededTrackFinder/SiSPSeededTrackFinder.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "CxxUtils/make_unique.h"
///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSPSeededTrackFinder::SiSPSeededTrackFinder
(const std::string& name,ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
  m_useZvertexTool(true)                                               ,
  m_useMBTS(false)                                                     ,
  m_useNewStrategy(false)                                              ,
  m_useZBoundaryFinding(false)                                         ,
  m_ITKGeometry(false)                                                 ,
  m_counterTotal {}                                                    ,
  m_maxNumberSeeds(3000000)                                            ,
  m_maxPIXsp(150000)                                                   ,
  m_maxSCTsp(500000) 						       ,
  m_nfreeCut(1)                                                        ,
  m_evtKey("EventInfo")                                                ,
  m_SpacePointsSCT("SCT_SpacePoints"),
  m_SpacePointsPixel("PixelSpacePoints"),
  m_outputTracks("SiSPSeededTracks"),
  m_seedsmaker("InDet::SiSpacePointsSeedMaker_ATLxk/InDetSpSeedsMaker"),
  m_zvertexmaker("InDet::SiZvertexMaker_xk/InDetSiZvertexMaker")       ,
  m_trackmaker("InDet::SiTrackMaker_xk/InDetSiTrackMaker")             ,
  m_fieldmode("MapSolenoid")                                           ,
  m_proptool   ("Trk::RungeKuttaPropagator/InDetPropagator"  )
{
  m_beamconditions         = "BeamCondSvc"     ;
  m_beam                   = 0                 ;
  m_histsize               = 1400              ;
  m_nvertex                = 4                 ;
  m_zcut                   = 350.              ;
  m_pTcut                  = 500.              ;
  m_imcut                  = 2.                ;
  m_nhistogram             = 0                 ;
  m_zhistogram             = 0                 ;
  m_phistogram             = 0                 ;
  m_neventsTotal           = 0                 ;
  m_neventsTotalV          = 0                 ;
  m_problemsTotal          = 0                 ; 
  m_problemsTotalV         = 0                 ; 
  m_zstep                  = 0                 ;

  // SiSPSeededTrackFinder steering parameters
  //
  declareProperty("SeedsTool"           ,m_seedsmaker          );
  declareProperty("ZvertexTool"         ,m_zvertexmaker        );
  declareProperty("TrackTool"           ,m_trackmaker          );
  declareProperty("TracksLocation"      ,m_outputTracks        );
  declareProperty("useZvertexTool"      ,m_useZvertexTool      );
  declareProperty("maxNumberSeeds"      ,m_maxNumberSeeds      );
  declareProperty("useMBTSTimeDiff"     ,m_useMBTS             );
  declareProperty("maxNumberPIXsp"      ,m_maxPIXsp            );
  declareProperty("maxNumberSCTsp"      ,m_maxSCTsp            );
  declareProperty("SpacePointsSCTName"  ,m_SpacePointsSCT      );
  declareProperty("SpacePointsPixelName",m_SpacePointsPixel    );
  declareProperty("FreeClustersCut"     ,m_nfreeCut            );
  declareProperty("useNewStrategy"      ,m_useNewStrategy      );
  declareProperty("useZBoundFinding"    ,m_useZBoundaryFinding );
  declareProperty("maxVertices"         ,m_nvertex             );
  declareProperty("PropagatorTool"      ,m_proptool            );
  declareProperty("BeamConditionsService",m_beamconditions     ); 
  declareProperty("HistSize"            ,m_histsize            );
  declareProperty("Zcut"                ,m_zcut                );
  declareProperty("MagneticFieldMode"   ,m_fieldmode           );
  declareProperty("ITKGeometry"         ,m_ITKGeometry         );
  declareProperty("EventInfoKey"        ,m_evtKey              );
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::initialize() 
{

  // Get tool for space points seed maker
  //
  ATH_CHECK( m_seedsmaker.retrieve() );
  if(m_useZvertexTool) {
    ATH_CHECK( m_zvertexmaker.retrieve() );
  }

  // Get track-finding tool
  //
  ATH_CHECK(m_trackmaker.retrieve());

  if(m_useNewStrategy && m_beamconditions=="") {m_useNewStrategy = false; m_useZBoundaryFinding = false;}

  if(m_useNewStrategy || m_useZBoundaryFinding || m_ITKGeometry) {

    // Get beam condition service 
    // 
    if(m_beamconditions!="") {service(m_beamconditions,m_beam);} 

    if(m_beam) {
    
      // Get RungeKutta propagator tool
      //
      ATH_CHECK( m_proptool.retrieve() );

      // Setup for magnetic field
      //
      magneticFieldInit();
      
      // Setup for Z-histograms
      //  
      if(m_histsize < 100) m_histsize = 100;
      m_nhistogram = new int   [m_histsize];
      m_zhistogram = new double[m_histsize];
      m_phistogram = new double[m_histsize];
      m_zstep      = double(m_histsize)/(2.*m_zcut);
    }
    else {m_useNewStrategy = false; m_useZBoundaryFinding = false;}
  }

  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)) {
    dump(MSG::DEBUG,nullptr);
  }
  m_counterTotal = {};
  m_neventsTotal   = 0;
  m_neventsTotalV  = 0;
  m_problemsTotal  = 0; 
  m_problemsTotalV = 0; 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::execute() 
{ 
  if(!m_useNewStrategy && !m_useZBoundaryFinding && !m_ITKGeometry) return oldStrategy();
  else                                                              return newStrategy();
}

///////////////////////////////////////////////////////////////////
// Execute with old strategy
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::oldStrategy()
{ 
  m_outputTracks = CxxUtils::make_unique<TrackCollection>();
  // For HI events we can use MBTS information from calorimeter
  //
  if(!isGoodEvent()) {
    return StatusCode::SUCCESS;
  }

  std::multimap<double,Trk::Track*>    qualityTrack;
  const InDet::SiSpacePointsSeed* seed = 0;

  // Test is sct clusters collection for given event
  //
  bool PIX = true ;
  bool SCT = true ;
  bool ZVE = false;
  bool ERR = false;

  if( m_useZvertexTool ) {
    m_zvertexmaker->newEvent(); 
    if(!m_zvertexmaker->getVertices().empty()) ZVE = true;
    m_seedsmaker->find3Sp(m_zvertexmaker->getVertices()); 
  }
  else                   {
    m_seedsmaker  ->newEvent(-1); 
    std::list<Trk::Vertex> VZ; m_seedsmaker->find3Sp(VZ); 
  }

  m_trackmaker->newEvent(PIX,SCT);

  // Loop through all seed and reconsrtucted tracks collection preparation
  //
  
  Counter_t counter {};

  while((seed = m_seedsmaker->next())) {

    ++counter[kNSeeds];
    const std::list<Trk::Track*>& T = m_trackmaker->getTracks(seed->spacePoints()); 
    for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
      qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));
    }
    if( ZVE == false  && counter[kNSeeds] >= m_maxNumberSeeds) {
      ERR = true; ++m_problemsTotal;  break;
    }
  }
  
  m_trackmaker->endEvent();

  // Remove shared tracks with worse quality
  //
  filterSharedTracks(qualityTrack);

  // Save good tracks in track collection
  //
  std::multimap<double,Trk::Track*>::iterator 
    q = qualityTrack.begin(), qe =qualityTrack.end(); 

  for(; q!=qe; ++q) {++counter[kNTracks]; m_outputTracks->push_back((*q).second);}

  m_counterTotal[kNSeeds] += counter[kNSeeds] ;

  ZVE == true ? ++m_neventsTotalV : ++m_neventsTotal;

  if(ERR) { m_outputTracks->clear(); }
  else    {m_counterTotal[kNTracks]+=counter[kNTracks];}



  // Save reconstructed tracks
  //

  // Print common event information
  //
  if(msgLvl(MSG::DEBUG)) {
    dump(MSG::DEBUG,&counter);
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute with new strategy
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::newStrategy()
{ 
  m_outputTracks = CxxUtils::make_unique<TrackCollection>();

  // For HI events we can use MBTS information from calorimeter
  //
  if(!isGoodEvent()) {
    return StatusCode::SUCCESS;
  }

  // Get beam information and preparation for z -histogramming
  //
  Amg::Vector3D       cb = m_beam->beamPos();
  Amg::Vector3D       gBeam(cb.x(),cb.y(),cb.z());
  Trk::PerigeeSurface per(gBeam);
 
  // Initiate histograms
  //
  for(int i=0; i!=m_histsize; ++i) {
    m_nhistogram[i] = 0 ;
    m_zhistogram[i] = 0.;
    m_phistogram[i] = 0.;
  }
  
  std::multimap<double,Trk::Track*>    qualityTrack;
  const InDet::SiSpacePointsSeed* seed = 0;

  // Test is sct clusters collection for given event
  //
  bool PIX = true ;
  bool SCT = true ;
  bool ERR = false;
  m_seedsmaker->newEvent(0); 
  std::list<Trk::Vertex> VZ   ; m_seedsmaker->find3Sp(VZ); 
  double                 ZB[2];
  m_trackmaker->newEvent(PIX,SCT);

  // Loop through all seed and reconsrtucted tracks collection preparation
  //
  Counter_t counter {};
  while((seed = m_seedsmaker->next())) {

    ++counter[kNSeeds];
    const std::list<Trk::Track*>& T = m_trackmaker->getTracks(seed->spacePoints()); 
    for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
      qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));

      if(t==T.begin() && !m_ITKGeometry) fillZHistogram(*t,per);

    }
    if( counter[kNSeeds] >= m_maxNumberSeeds) {
      ERR = true; ++m_problemsTotal;  break;
    }
  }

  m_seedsmaker->newEvent(1); 

  if(!m_ITKGeometry) {findZvertex(VZ,ZB); m_seedsmaker->find3Sp(VZ,ZB);}
  else               {                    m_seedsmaker->find3Sp(VZ   );}

  // Loop through all seed and reconsrtucted tracks collection preparation
  //
  while((seed = m_seedsmaker->next())) {
    ++counter[kNSeeds];
    const std::list<Trk::Track*>& T = m_trackmaker->getTracks(seed->spacePoints()); 
    for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
      qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));

    }
    if( counter[kNSeeds] >= m_maxNumberSeeds) {
      ERR = true; ++m_problemsTotal;  break;
    }
  }
  m_trackmaker->endEvent();

  // Remove shared tracks with worse quality
  //
  filterSharedTracks(qualityTrack);

  // Save good tracks in track collection
  //
  std::multimap<double,Trk::Track*>::iterator 
    q = qualityTrack.begin(), qe =qualityTrack.end(); 

  for(; q!=qe; ++q) {++counter[kNTracks]; m_outputTracks->push_back((*q).second);}

  m_counterTotal[kNSeeds] += counter[kNSeeds] ;

  ++m_neventsTotal;

  if(ERR) {m_outputTracks->clear();}
  else    {m_counterTotal[kNTracks]+=counter[kNTracks];                            }


  // Save reconstructed tracks
  //

  // Print common event information
  //
  if(msgLvl(MSG::DEBUG)) {
    dump(MSG::DEBUG,&counter);
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinder::finalize() 
{
  if(m_nhistogram) delete [] m_nhistogram;
  if(m_zhistogram) delete [] m_zhistogram;
  if(m_phistogram) delete [] m_phistogram;

  if (msgLvl(MSG::INFO)) {
    dump(MSG::INFO,&m_counterTotal);
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

// MsgStream& InDet::operator    << 
//   (MsgStream& sl,const InDet::SiSPSeededTrackFinder& se)
// { 
//   return se.dump(sl);
// }

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

// std::ostream& InDet::operator << 
//   (std::ostream& sl,const InDet::SiSPSeededTrackFinder& se)
// {
//   return se.dump(sl);
// }   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinder::dump( MSG::Level assign_level, const InDet::SiSPSeededTrackFinder::Counter_t *counter ) const
{
  msg(assign_level) <<std::endl;
  MsgStream& out_msg=msg();
  if(counter)  dumpevent(out_msg ,*counter);
  else dumptools( out_msg );
  out_msg << endmsg;
  return out_msg;
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinder::dumptools( MsgStream& out) const
{
  int n = 65-m_zvertexmaker.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_seedsmaker.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_trackmaker.type().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 65-m_outputTracks.name().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  std::string s5; 
  if(m_useZvertexTool) s5= "Yes"; else s5 = "No";
  n     = 65-s5.size(); for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  out<<"| Use primary vertices z-coordinates      finding?| "<<s5
     <<std::endl;
  if(m_useZvertexTool) {
    out<<"| Tool for primary vertices z-coordinates finding | "<<m_zvertexmaker.type()<<s1
       <<std::endl;
  }
  out<<"| Tool for space points seeds             finding | "<<m_seedsmaker.type()<<s2
     <<std::endl;
  out<<"| Tool for space points seeded track      finding | "<<m_trackmaker.type()<<s3
     <<std::endl;
  out<<"| Location of output tracks                       | "<<m_outputTracks.name()<<s4
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinder::dumpevent( MsgStream& out,  const InDet::SiSPSeededTrackFinder::Counter_t &counter) const
{
  out<<"|-------------------------------------------------------------------";
  out<<"---------------------------------|"
     <<std::endl;
  out<<"|  Investigated "
     <<std::setw(9)<<counter[kNSeeds]<<" space points seeds and found ";
  out<<std::setw(9)<<counter[kNTracks];
  if     (m_ITKGeometry        ) out<<" tracks using new strategy for ITK   |"<<std::endl; 
  else if(m_useNewStrategy     ) out<<" tracks using new strategy ("<<std::setw(2)<<  m_nvertex <<")      |"<<std::endl;
  else if(m_useZBoundaryFinding) out<<" tracks using old strategy with Zb   |"<<std::endl;
  else                           out<<" tracks using old strategy           |"<<std::endl;

  out<<"|-------------------------------------------------------------------";
  out<<"---------------------------------|"
     <<std::endl;
  if(m_problemsTotal || m_problemsTotalV ) {
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

bool InDet::SiSPSeededTrackFinder::isGoodEvent() {

  if(!m_useMBTS) return true;

  // Test MBTS information from calorimeter
  //
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_evtKey);
  if(!eventInfo->isEventFlagBitSet(xAOD::EventInfo::Background,xAOD::EventInfo::MBTSTimeDiffHalo)) {
    return true;
  }

  // Test total number pixels space points
  //
  unsigned int               nsp = 0;

   if(!m_SpacePointsPixel.isValid()){
    SpacePointContainer::const_iterator spc  =  m_SpacePointsPixel->begin();
    SpacePointContainer::const_iterator spce =  m_SpacePointsPixel->end  ();
    
    for(; spc!=spce; ++spc) nsp+=(*spc)->size();

    if(int(nsp) > m_maxPIXsp) {
      msg(MSG::WARNING)<<"Found more than "<<m_maxPIXsp<<" pixels space points in background event. Skip track finding"<<endmsg;
      return false;
    }
  }
 
  // Test total number sct space points
  //
  nsp = 0;
  
  if(!m_SpacePointsSCT.isValid()) {
    
    SpacePointContainer::const_iterator spc  =  m_SpacePointsSCT->begin();
    SpacePointContainer::const_iterator spce =  m_SpacePointsSCT->end  ();
    
    for(; spc!=spce; ++spc) nsp+=(*spc)->size();

    if(int(nsp) > m_maxSCTsp) {
      msg(MSG::WARNING)<<"Found more than "<<m_maxSCTsp<<" sct space points in background event. Skip track finding"<<endmsg;
      return false;
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Track quality calculation
///////////////////////////////////////////////////////////////////

double InDet::SiSPSeededTrackFinder::trackQuality(const Trk::Track* Tr)
{
  DataVector<const Trk::TrackStateOnSurface>::const_iterator  
    m  = Tr->trackStateOnSurfaces()->begin(), 
    me = Tr->trackStateOnSurfaces()->end  ();

 double quality = 0. ;
 double W       = 17.;

 for(; m!=me; ++m) {

   if(!(*m)->type(Trk::TrackStateOnSurface::Measurement)) continue;
   const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
   if(!fq) continue;

   double x2 = fq->chiSquared();
   double q;
   if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
   else                     q =      (W-x2    );
   if(q < 0.) q = 0.;
   quality+=q;
 }
 if( Tr->info().trackProperties(Trk::TrackInfo::BremFit) ) quality*=.7;
 return quality;
}

///////////////////////////////////////////////////////////////////
// Filer shared tracks
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::filterSharedTracks
(std::multimap<double,Trk::Track*>& QT)
{
  std::set<const Trk::PrepRawData*> clusters;
  std::multimap<double,Trk::Track*>::iterator q = QT.begin();
  
  const Trk::PrepRawData* prd[100]; 
  
  while(q!=QT.end()) {

    std::set<const Trk::PrepRawData*>::iterator fe = clusters.end();

    int nf = 0, nc = 0; 

    DataVector<const Trk::MeasurementBase>::const_iterator 
      m  = (*q).second->measurementsOnTrack()->begin(), 
      me = (*q).second->measurementsOnTrack()->end  ();

    for(; m!=me; ++m) {

      const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
      if(pr) {
	++nc; if(clusters.find(pr)==fe) {prd[nf++]=pr; if(nf==100) break;}
      }
    }

    if(nf >= m_nfreeCut || nf==nc) {
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]);
      ++q;
    } 
    else  {
      delete (*q).second; QT.erase(q++);
    }
  }
}

///////////////////////////////////////////////////////////////////
// Fill z coordinate histogram
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::fillZHistogram(const Trk::Track* Tr,Trk::PerigeeSurface& per)
{
  
  if(Tr->measurementsOnTrack()->size() < 10) return;
	
  DataVector<const Trk::TrackStateOnSurface>::const_iterator m  = Tr->trackStateOnSurfaces()->begin();
  const Trk::TrackParameters* P = (*m)->trackParameters();
  Amg::Vector3D              gf = P->position()          ;
  Amg::Vector3D              gm = P->momentum()          ;
      
  if(gf.x()*gf.x()+gf.y()*gf.y() >= 3600.) return;

  double pT = sqrt(gm.x()*gm.x()+gm.y()*gm.y());
  if(pT < m_pTcut) return;

  Trk::PatternTrackParameters TP;
  if(!TP.production(P)) return;
	  
  double step;
  if(!m_proptool->propagate(TP,per,TP,Trk::anyDirection,m_fieldprop,step,Trk::pion)) return;
	      
  const double* par  = TP.par();
  if(fabs(par[0]) > m_imcut) return;
  int z = int((par[1]+m_zcut)*m_zstep);
  if(z >=0 && z < m_histsize) {
    ++m_nhistogram[z]; m_zhistogram[z]+=par[1]; m_phistogram[z]+=pT;
  }
  
}

///////////////////////////////////////////////////////////////////
// Find verteex  z coordinates
///////////////////////////////////////////////////////////////////

void  InDet::SiSPSeededTrackFinder::findZvertex(std::list<Trk::Vertex>& ZV,double* ZB)
{
  ZB[0]    = 1000.;
  ZB[1]    =-1000.;

  std::multimap<int   ,double> vern;
  std::multimap<double,double> verp;

  int imax = m_histsize-1;
  int nmin = 3;

  for(int i=1; i!=imax; ++i) { 

    int n =  m_nhistogram[i-1]+m_nhistogram[i]+m_nhistogram[i+1];

    if(n>=nmin && (m_nhistogram[i] >= m_nhistogram[i-1] && m_nhistogram[i] >= m_nhistogram[i+1])) {
  
      double z = (m_zhistogram[i-1]+m_zhistogram[i]+m_zhistogram[i+1])/double(n);

      if(z < ZB[0]) ZB[0] = z;
      if(z > ZB[1]) ZB[1] = z;
 
      if(m_useNewStrategy) { 

	double p = m_phistogram[i-1]+m_phistogram[i]+m_phistogram[i+1];
	vern.insert(std::make_pair(-n,z));
	verp.insert(std::make_pair(-p,z));
      }
    }
  }

  if(m_useNewStrategy) {

    std::set<double> vertex;
    int n = 0;
    std::multimap<int   ,double>::iterator nz = vern.begin();
    std::multimap<double,double>::iterator pz = verp.begin();
    
    for(; nz!=vern.end(); ++nz) {
      if(n++ >= m_nvertex) break; 
      vertex.insert((*nz).second); vertex.insert((*pz++).second);
    }
    
    std::set<double>::iterator v = vertex.begin();
    for(; v!=vertex.end(); ++v) {
      Amg::Vector3D Vp(0.,0.,(*v)); Trk::Vertex Ver(Vp); ZV.push_back(Ver);
    }
  }

  if(ZB[0] > ZB[1]) {ZB[0] = -1000.; ZB[1] = +1000.;}
  else              {ZB[0]-=20.    ; ZB[1]+=20.;    }
 
} 


///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinder::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //
  Trk::MagneticFieldProperties* pMF = 0;
  if(m_fieldmode == "NoField") pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else                         pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  m_fieldprop = *pMF; delete pMF;
}




