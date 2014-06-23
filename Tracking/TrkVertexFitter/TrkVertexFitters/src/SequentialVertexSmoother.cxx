/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexFitters/SequentialVertexSmoother.h"
#include "VxVertex/VxCandidate.h"
#include "TrkVertexFitterInterfaces/IVertexTrackUpdator.h"
#include "VxVertex/VxTrackAtVertex.h"

namespace  Trk
{

//tool-related methods
 StatusCode SequentialVertexSmoother::initialize()
 {
  
//uploading the corresponding tools
  if ( m_vertexTrackUpdator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_vertexTrackUpdator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_vertexTrackUpdator << endreq;
  }
  
  msg(MSG::INFO)<<"Initialization successfull"<<endreq;
  return StatusCode::SUCCESS;
 }//end of initialize method
 
 StatusCode SequentialVertexSmoother::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endreq;
  return StatusCode::SUCCESS;
 }//end of finalize method
 
//class constructor 
 SequentialVertexSmoother::SequentialVertexSmoother(const std::string& t, const std::string& n, const IInterface*  p):
    AthAlgTool(t,n,p),
    m_vertexTrackUpdator("Trk::KalmanVertexTrackUpdator")
 {
  declareProperty("VertexTrackUpdator",m_vertexTrackUpdator);
  declareInterface<IVertexSmoother>(this);
 }//end of constructor description
 
//class destructor empty so far
 SequentialVertexSmoother::~SequentialVertexSmoother() {}
 
//smooth method itself 
 void SequentialVertexSmoother::smooth(const VxCandidate& vtx) const
 {
 
//this is a RecVertex created by a fitter, so we can just store it so far 
  Trk::RecVertex rv = vtx.recVertex();
  
//vector of associated tracks at vertex:
//protection check whether this pointer really exists  
  if(vtx.vxTrackAtVertex() != 0)
  {
   std::vector<Trk::VxTrackAtVertex*> tracks = * vtx.vxTrackAtVertex();
  
//updating the tracks one by one 
   if(tracks.size() !=0)
   { 
    for(std::vector<Trk::VxTrackAtVertex*>::iterator i = tracks.begin(); i!=tracks.end(); ++i) 
    {
     if( (*i) != 0)
     {
      m_vertexTrackUpdator->update(**i,rv);
     }else{
      msg(MSG::ERROR)  << "Vector of tracks contain empty pointers!" << endreq;
      msg(MSG::ERROR)  << "No action taken on this track." << endreq;
     }//end of empty track pointer protection
    }//end of loop over all tracks in particular vertex
   }else{

//something wrong: notification to user  
    msg(MSG::ERROR)  << "Vertex to be smoothed containes no tracks!" << endreq;
    msg(MSG::ERROR)  << "No action taken; Initial VxCandidate returned." << endreq;
   }//end of protection statement
  }else{
   msg(MSG::ERROR)  << "Vertex to be smoothed has an empty pointer to its tracks!" << endreq;
   msg(MSG::ERROR)  << "No action taken; Initial VxCandidate returned." << endreq;
  }//end of protection against empty pointer to fitted tracks
 }//end of smooth method

}//end of the namespace definitions
