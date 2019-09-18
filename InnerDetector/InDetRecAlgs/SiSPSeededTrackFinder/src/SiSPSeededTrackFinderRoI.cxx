/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <set>
#include <vector>

#include "SiSPSeededTrackFinder/SiSPSeededTrackFinderRoI.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "CxxUtils/make_unique.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include <sstream>
#include <fstream>
#include <string>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSPSeededTrackFinderRoI::SiSPSeededTrackFinderRoI
(const std::string& name,ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
  m_outputlevel(0)                                                     ,
  m_nprint(0)                                                          ,
  m_nseeds(0)                                                          ,
  m_ntracks(0)                                                         ,
  m_maxNumberSeeds(3000000)                                            , 
  m_maxPIXsp(150000)                                                   ,
  m_maxSCTsp(500000) 						       ,
  m_nfreeCut(1)                                                        ,
  m_doRandomSpot(false)                                                        , //see comment proceding line with "double RandZBoundary[2];"
  m_SpacePointsSCT("SCT_SpacePoints"),
  m_SpacePointsPixel("PixelSpacePoints"),
  m_outputTracks("SiSPSeededTracks"),
  m_vxOutputName ( "LowPtRoIVertices" ),
  //m_vxOutputNameAuxPostfix ( "Aux." ),
  m_seedsmaker("InDet::SiSpacePointsSeedMaker_ATLxk/InDetSpSeedsMaker"),
  m_trackmaker("InDet::SiTrackMaker_xk/InDetSiTrackMaker")             ,
  m_fieldmode("MapSolenoid")                                           ,
  m_proptool   ("Trk::RungeKuttaPropagator/InDetPropagator"  ),         
  m_ZWindowRoISeedTool("InDet::ZWindowRoISeedTool"),
  m_RandomRoISeedTool("InDet::RandomRoISeedTool")
{
  m_beamconditions         = "BeamCondSvc"     ;
  m_beam                   = 0                 ;
  m_nseedsTotal            = 0                 ;
  m_ntracksTotal           = 0                 ;
  m_neventsTotal           = 0                 ;
  m_problemsTotal          = 0                 ; 

  // SiSPSeededTrackFinderRoI steering parameters
  //
  declareProperty("ZWindowRoISeedTool"  ,m_ZWindowRoISeedTool  );
  declareProperty("RandomRoISeedTool"  ,m_RandomRoISeedTool  );
  declareProperty("SeedsTool"           ,m_seedsmaker          );
  declareProperty("TrackTool"           ,m_trackmaker          );
  declareProperty("TracksLocation"      ,m_outputTracks        );
  declareProperty ( "VxOutputName",m_vxOutputName );
  //declareProperty ( "VxOutputNameAuxPostfix",m_vxOutputNameAuxPostfix );
  declareProperty("maxNumberSeeds"      ,m_maxNumberSeeds      );
  declareProperty("maxNumberPIXsp"      ,m_maxPIXsp            );
  declareProperty("maxNumberSCTsp"      ,m_maxSCTsp            );
  declareProperty("SpacePointsSCTName"  ,m_SpacePointsSCT      );
  declareProperty("SpacePointsPixelName",m_SpacePointsPixel    );
  declareProperty("FreeClustersCut"     ,m_nfreeCut            );
  declareProperty("doRandomSpot"     ,m_doRandomSpot            );
  declareProperty("PropagatorTool"      ,m_proptool            );
  declareProperty("BeamConditionsService",m_beamconditions     ); 
  declareProperty("MagneticFieldMode"   ,m_fieldmode           );
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinderRoI::initialize() 
{

  // Get the ZWindowRoI seed tool
  if( m_ZWindowRoISeedTool.retrieve().isFailure() ){
    ATH_MSG_FATAL("Failed to retrieve tool "<< m_ZWindowRoISeedTool);
    return StatusCode::FAILURE;
  }
  else{
    ATH_MSG_DEBUG("Retrieved tool " << m_ZWindowRoISeedTool);
  }


  if(m_doRandomSpot){ //see comment proceding line with "double RandZBoundary[2];"
    if( m_RandomRoISeedTool.retrieve().isFailure() ){
      ATH_MSG_FATAL("Failed to retrieve tool "<< m_RandomRoISeedTool);
      return StatusCode::FAILURE;
    }
    else{
      ATH_MSG_DEBUG("Retrieved tool " << m_RandomRoISeedTool);
    }
  }


  // Get tool for space points seed maker
  //
  if ( m_seedsmaker.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_seedsmaker );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Retrieved tool " << m_seedsmaker );
  }

  // Get track-finding tool
  //
  if ( m_trackmaker.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackmaker );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_trackmaker );
  }

  // Get beam condition service and propagator (SP: check if we really need propagator..)
  // 
  if(m_beamconditions!="") {service(m_beamconditions,m_beam);} 
  if(m_beam) {    
    // Get RungeKutta propagator tool
    if ( m_proptool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_proptool );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Retrieved tool " << m_proptool );
    }      
    // Setup for magnetic field
    magneticFieldInit();      
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; ATH_MSG_DEBUG((*this));
  }
  m_nseedsTotal    = 0;
  m_ntracksTotal   = 0;
  m_neventsTotal   = 0;
  m_problemsTotal  = 0; 

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinderRoI::execute() 
{ 

  m_outputTracks = CxxUtils::make_unique<TrackCollection>();

  std::multimap<double,Trk::Track*>    qualityTrack;
  const InDet::SiSpacePointsSeed* seed = 0;

  bool PIX = true ;
  bool SCT = true ;
  bool ERR = false;

  //CREATE CONTAINER FOR RoI INFORMATION
  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
  theVertexContainer->setStore( theVertexAuxContainer );

  //xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;
  //theVertexContainer->push_back( dummyxAODVertex );

  // Find reference point of the event and create z boundary region
  //
  m_listRoIs =  m_ZWindowRoISeedTool->getRoIs();

  double ZBoundary[2];
  //if no RoI found; no need to go further
  if (m_listRoIs.size() == 0) {
    ATH_MSG_DEBUG("no selectedRoIs " );
    if (!evtStore()->contains<xAOD::VertexContainer>(m_vxOutputName)){
      CHECK(evtStore()->record(theVertexContainer, m_vxOutputName));
    }
    if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_vxOutputName+"Aux.")){
      CHECK(evtStore()->record(theVertexAuxContainer, m_vxOutputName+"Aux."));
    }
    return StatusCode::SUCCESS;
  }
  ZBoundary[0] = m_listRoIs[0].z_window[0];
  ZBoundary[1] = m_listRoIs[0].z_window[1];
  //m_listRoIs[0].z_reference is the midpoint
  ATH_MSG_DEBUG("selectedRoIs " << ZBoundary[0] <<" " << ZBoundary[1]);
  
  //VERTEX
  std::vector<xAOD::Vertex *> dummyxAODVertices_vector;
  for( int r = 0; r < m_listRoIs.size(); r++ ){

    xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;

    dummyxAODVertices_vector.push_back(dummyxAODVertex);

    theVertexContainer->push_back( dummyxAODVertices_vector.at(r) );

    dummyxAODVertices_vector.at(r)->setZ( m_listRoIs[r].z_reference );
    dummyxAODVertices_vector.at(r)->auxdecor<double>("boundary_low") = m_listRoIs[r].z_window[0];
    dummyxAODVertices_vector.at(r)->auxdecor<double>("boundary_high") = m_listRoIs[r].z_window[1];

    dummyxAODVertices_vector.at(r)->auxdecor<double>("perigee_z0_lead") = m_listRoIs[r].z_perigee_pos[0];
    dummyxAODVertices_vector.at(r)->auxdecor<double>("perigee_z0_sublead") = m_listRoIs[r].z_perigee_pos[1];
    dummyxAODVertices_vector.at(r)->auxdecor<int>("IsHS") = 1;

  }
  
  //For the Exclusive WW analysis (and other exclusive process analyses), the production point of the exclusive process can essentially be treated as a random point in the beamspot with respect to the pileup.  The mark of an exclusive process is a lack of tracks from an underlying event, but due to pileup, some fraction of exclusive events will not be classified as "exclusive" because of nearby tracks due to pileup.  To check how often an exclusive event will be rejected due to pileup tracks, we look at a random spot in the beamspot that is displaced from the exclusive process's vertex and perform low-pt tracking in that region.  Run with postexec: ToolSvc.InDetSiSpTrackFinder_LowPtRoI.doRandomSpot = True
  double RandZBoundary[2];
  if(m_doRandomSpot){
    //Finding Random Spot in beamspot
    m_listRandRoIs =  m_RandomRoISeedTool->getRoIs();

    while( std::abs( m_listRoIs[0].z_reference - m_listRandRoIs[0].z_reference ) < 5. || std::abs(m_listRandRoIs[0].z_reference) > 250.0 ){
      m_listRandRoIs.clear();
      m_listRandRoIs =  m_RandomRoISeedTool->getRoIs();
    }

    //double RandZBoundary[2];
    RandZBoundary[0] = m_listRandRoIs[0].z_window[0];
    RandZBoundary[1] = m_listRandRoIs[0].z_window[1];
    std::vector<xAOD::Vertex *> dummyxAODVertices_vector_rand;
    for( int r = 0; r < m_listRandRoIs.size(); r++ ){

      xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;

      dummyxAODVertices_vector_rand.push_back(dummyxAODVertex);

      theVertexContainer->push_back( dummyxAODVertices_vector_rand.at(r) );

      dummyxAODVertices_vector_rand.at(r)->setZ( m_listRandRoIs[r].z_reference );
      dummyxAODVertices_vector_rand.at(r)->auxdecor<double>("boundary_low") = m_listRandRoIs[r].z_window[0];
      dummyxAODVertices_vector_rand.at(r)->auxdecor<double>("boundary_high") = m_listRandRoIs[r].z_window[1];

      dummyxAODVertices_vector_rand.at(r)->auxdecor<double>("perigee_z0_lead") = m_listRandRoIs[r].z_perigee_pos[0];
      dummyxAODVertices_vector_rand.at(r)->auxdecor<double>("perigee_z0_sublead") = m_listRandRoIs[r].z_perigee_pos[1];
      dummyxAODVertices_vector_rand.at(r)->auxdecor<int>("IsHS") = 0;

    }
  }
  
  if (!evtStore()->contains<xAOD::VertexContainer>(m_vxOutputName)){
    CHECK(evtStore()->record(theVertexContainer, m_vxOutputName));
  }
  if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_vxOutputName+"Aux.")){
    CHECK(evtStore()->record(theVertexAuxContainer, m_vxOutputName+"Aux."));
  }

  

  // Find seeds that point within the RoI region in z
  //  
  m_seedsmaker  ->newEvent(-1); 
  std::list<Trk::Vertex> VZ; 
  m_seedsmaker->find3Sp(VZ, ZBoundary); 
  if(m_doRandomSpot) m_seedsmaker->find3Sp(VZ, RandZBoundary); //see comment proceding line with "double RandZBoundary[2];"
  m_trackmaker->newEvent(PIX,SCT);

  // Loop through all seed and create track candidates
  //
  m_nseeds  = 0;
  m_ntracks = 0;

  while((seed = m_seedsmaker->next())) {

    ++m_nseeds;
    const std::list<Trk::Track*>& T = m_trackmaker->getTracks(seed->spacePoints()); 
    for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
      qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));
    }
    if( m_nseeds >= m_maxNumberSeeds) {
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

  for(; q!=qe; ++q) {++m_ntracks; m_outputTracks->push_back((*q).second);}

  m_nseedsTotal += m_nseeds ;

  ++m_neventsTotal;

  // In case of errors, clear output tracks
  //
  if(ERR) { m_outputTracks->clear(); }
  else    {m_ntracksTotal+=m_ntracks;}

  // Print common event information
  //
  if(m_outputlevel<=0) {
    m_nprint=1; ATH_MSG_DEBUG((*this));
  }
  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSPSeededTrackFinderRoI::finalize() 
{
  m_nprint=2; ATH_MSG_DEBUG((*this));
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiSPSeededTrackFinderRoI& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiSPSeededTrackFinderRoI& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinderRoI::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out); 
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSPSeededTrackFinderRoI::dumptools( MsgStream& out ) const
{
  int n;
  n     = 65-m_seedsmaker.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_trackmaker.type().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 65-m_outputTracks.name().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
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

MsgStream& InDet::SiSPSeededTrackFinderRoI::dumpevent( MsgStream& out ) const
{
  int ns = m_nseeds;
  int nt = m_ntracks;
  if(m_nprint > 1) {ns = m_nseedsTotal; nt = m_ntracksTotal;}

  out<<"|-------------------------------------------------------------------|" <<std::endl;
  out<<"|  Investigated "
     <<std::setw(9)<<ns<<" space points seeds and found ";
  out<<std::setw(9)<<nt<<" tracks using RoI-z strategy           |"<<std::endl;

  out<<"|-------------------------------------------------------------------|" <<std::endl;
  if(m_problemsTotal > 0) {
    out<<"|  Events       "
       <<std::setw(7)<<m_neventsTotal   <<" |"
       <<std::endl;
    out<<"|  Problems     "
       <<std::setw(7)<<m_problemsTotal  <<" |"
       <<std::endl;
    out<<"|-------------------------------------------------------------------|" <<std::endl;
  }
  return out;
}

///////////////////////////////////////////////////////////////////
// Track quality calculation
///////////////////////////////////////////////////////////////////

double InDet::SiSPSeededTrackFinderRoI::trackQuality(const Trk::Track* Tr)
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

void InDet::SiSPSeededTrackFinderRoI::filterSharedTracks
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
	++nc; 
	if(clusters.find(pr)==fe) {
	  prd[nf++]=pr; 
	  if(nf==100) break;
	}
      }
    }

    if(nf >= m_nfreeCut || nf==nc) {
      for(int n=0; n!=nf; ++n) 
	clusters.insert(prd[n]); 
      ++q;
    } 
    else  {
      delete (*q).second; 
      QT.erase(q++);
    }
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiSPSeededTrackFinderRoI::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

void InDet::SiSPSeededTrackFinderRoI::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //
  Trk::MagneticFieldProperties* pMF = 0;
  if(m_fieldmode == "NoField") pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else                         pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  m_fieldprop = *pMF; delete pMF;
}




