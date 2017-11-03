/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SeedToTrackConversionTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <utility>

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "SeedToTrackConversionTool/SeedToTrackConversionTool.h"

//================ Constructor =================================================

InDet::SeedToTrackConversionTool::SeedToTrackConversionTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  : AthAlgTool(t,n,p),
  m_extrapolator("Trk::Extrapolator"),
  m_rotcreator("Trk::RIO_OnTrackCreator/InDetRotCreator"),
  m_totseed(0),
  m_survived(0)
{
    // For SiSPSeedSegment
  //
  m_seedsegmentsCollection = 0;
  m_seedsegmentsOutput = "SiSPSeedSegments";
  m_patternName = "SiSPSeededFinder"; 
  m_nprint = 0;   
  declareInterface<ISeedToTrackConversionTool>(this);
  declareProperty("TrackPatternRecoInfo"    ,m_patternName );
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

InDet::SeedToTrackConversionTool::~SeedToTrackConversionTool()
{}


//================ Initialisation =================================================

StatusCode InDet::SeedToTrackConversionTool::initialize()
{

  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;


 if(m_extrapolator.retrieve().isFailure()) {
    ATH_MSG_FATAL ("Could not retrieve "<< m_extrapolator);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_VERBOSE( "initialize() Retrieved service " << m_extrapolator);

  // Retrieve the Track RotCreator tool

  if(m_rotcreator.retrieve().isFailure()) {
    ATH_MSG_FATAL ("Could not retrieve "<< m_rotcreator);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_VERBOSE( "initialize() Retrieved service " << m_rotcreator);
  
  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::SeedToTrackConversionTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
 return sc;
}

void InDet::SeedToTrackConversionTool::newEvent()
{
  m_seedsegmentsCollection = new TrackCollection;
  m_totseed = 0;
  m_survived = 0; 
}

void InDet::SeedToTrackConversionTool::newEvent(const Trk::TrackInfo& info, const std::string& patternName)
{
  newEvent(); 
  m_trackinfo = info;
  m_patternName = patternName;
  if((int)m_patternName.find("Forward")>-1){
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks);
  }
}

void InDet::SeedToTrackConversionTool::endEvent()
{
  // Print event information
  //
  if (msgLevel()<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }

  const TrackCollection*  inputTracks = 0;

  if(evtStore()->retrieve(inputTracks, m_seedsegmentsOutput)&&inputTracks) {
    TrackCollection::const_iterator t,te = inputTracks->end();
    for (t=inputTracks->begin(); t!=te; ++t) {
      m_seedsegmentsCollection->push_back(new Trk::Track(*(*t)) );
    }

    msg(MSG::INFO)<<"Check SiSPSeedSegments Collection "<<m_seedsegmentsCollection->size()<<" inputTracks: "
		  <<inputTracks->size()<<" trackinfo: "<<m_trackinfo<< endmsg;
    StatusCode s = evtStore()->overwrite(m_seedsegmentsCollection,m_seedsegmentsOutput,true);
    if (s.isFailure() ) {
      msg(MSG::ERROR)<<"Could not overwrite converted SiSPSeedSegments tracks" <<endmsg;
    }
  }
  else{
    msg(MSG::INFO)<<" Check SiSPSeedSegments Collection "<<m_seedsegmentsCollection->size()<<" trackinfo: " 
		  <<m_trackinfo<<endmsg;
    StatusCode s = evtStore()->record(m_seedsegmentsCollection,m_seedsegmentsOutput,true);
    if (s.isFailure() ) {
      msg(MSG::ERROR)<<"Could not save converted SiSPSeedSegments tracks" <<endmsg;
    }
  }

}

void  InDet::SeedToTrackConversionTool::executeSiSPSeedSegments(const Trk::TrackParameters* Tp,const int& mtrk, const std::list<const Trk::SpacePoint*>& Sp)
{
  ++m_totseed; // accumulate all seeds
  if(mtrk>0)++m_survived; // survided seeds 
  std::vector<const Trk::PrepRawData*> prdsInSp;
  std::list<const Trk::SpacePoint*>::const_iterator is=Sp.begin(),ise=Sp.end();

  for(; is !=ise; ++is){
    const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>& prds = (**is).clusterList();
    if(prds.first)prdsInSp.push_back(prds.first);
    if(prds.second&&prds.first != prds.second)prdsInSp.push_back(prds.second);
  }
  Trk::PerigeeSurface persurf;
  const Trk::TrackParameters *per = m_extrapolator->extrapolate(*Tp,persurf,Trk::anyDirection,false,Trk::nonInteracting);
  const Trk::TrackParameters *prevpar = Tp;
  if(per){
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    const Trk::TrackStateOnSurface *pertsos=new Trk::TrackStateOnSurface(0,per,0,0,typePattern);
    DataVector<const Trk::TrackStateOnSurface>* traj = new DataVector<const Trk::TrackStateOnSurface>;
    traj->push_back(pertsos);
    int ix1=0;
    int i=0;
    for ( ;i<(int)prdsInSp.size();i++){
      const Trk::Surface &surf=prdsInSp[i]->detectorElement()->surface(prdsInSp[i]->identify());
      const Trk::TrackParameters *thispar = m_extrapolator->extrapolate(*prevpar,surf,Trk::alongMomentum,false,Trk::nonInteracting);
      if(thispar){
        const Trk::TrackParameters *tmppar=thispar->clone();
        delete thispar;
        thispar=tmppar;
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
        const Trk::RIO_OnTrack *rot=0;
        rot=m_rotcreator->correct(*prdsInSp[i],*thispar);
        if (rot){
          const Trk::TrackStateOnSurface *tsos=new Trk::TrackStateOnSurface(rot,thispar,0,0,typePattern);
          traj->push_back(tsos);
          prevpar=thispar;
          ix1++;
        }
      }
    }
    Trk::TrackInfo trkinfo = m_trackinfo;
    if(mtrk>0){ // survived seeds set as
      trkinfo.setTrackFitter(Trk::TrackInfo::xKalman); // xk seedfinder
    }
    Trk::Track* t = new Trk::Track(trkinfo,traj,0);
    if(t)m_seedsegmentsCollection->push_back(t);
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////
 
MsgStream&  InDet::SeedToTrackConversionTool::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint) dumpevent(out);
  return dumpconditions(out);
}
 
///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SeedToTrackConversionTool::dumpconditions( MsgStream& out ) const
{
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| Output Collection Name   | "<<m_seedsegmentsOutput <<std::endl;
  out<<"} Name of pattern recognition | "<<m_patternName<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}
///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SeedToTrackConversionTool::dumpevent( MsgStream& out ) const
{
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Name of SeedFinder      | "<<m_patternName
     <<"                              |"<<std::endl;
  out<<"| Number of All seeds      | "<<std::setw(12)<<m_totseed 
     <<"                              |"<<std::endl;
  out<<"| Number of survived seeds    | "<<std::setw(12)<<m_survived  
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////
 
std::ostream& InDet::SeedToTrackConversionTool::dump( std::ostream& out ) const
{
  return out;
}
 
///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////
 
MsgStream& InDet::operator    << 
(MsgStream& sl,const InDet::SeedToTrackConversionTool& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
(std::ostream& sl,const InDet::SeedToTrackConversionTool& se)
{
  return se.dump(sl); 
}   

//============================================================================================

