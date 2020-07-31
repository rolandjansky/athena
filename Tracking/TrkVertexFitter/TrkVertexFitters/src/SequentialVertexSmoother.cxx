/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexFitters/SequentialVertexSmoother.h"
#include "TrkVertexFitterInterfaces/IVertexTrackUpdator.h"
#include "VxVertex/VxTrackAtVertex.h"

namespace  Trk
{

//tool-related methods
 StatusCode SequentialVertexSmoother::initialize()
 {
  
//uploading the corresponding tools
  if ( m_vertexTrackUpdator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_vertexTrackUpdator << endmsg;
    return StatusCode::FAILURE;
  } 
    msg(MSG::INFO) << "Retrieved tool " << m_vertexTrackUpdator << endmsg;
  
  
  msg(MSG::INFO)<<"Initialization successfull"<<endmsg;
  return StatusCode::SUCCESS;
 }//end of initialize method
 
 StatusCode SequentialVertexSmoother::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endmsg;
  return StatusCode::SUCCESS;
 }//end of finalize method
 
 //class constructor 
 SequentialVertexSmoother::SequentialVertexSmoother(const std::string& t, const std::string& n, const IInterface*  p):
    AthAlgTool(t,n,p),
    m_vertexTrackUpdator("Trk::KalmanVertexTrackUpdator", this)
 {
  declareProperty("VertexTrackUpdator",m_vertexTrackUpdator);
  declareInterface<IVertexSmoother>(this);
 }//end of constructor description
 
 //class destructor empty so far
 SequentialVertexSmoother::~SequentialVertexSmoother() = default;

 //smooth method itself
 // Had to make the vtx non-const because xAOD::Vertex stores
 // Trk::VxTrackAtVertex and not Trk::VxTrackAtVertex*
 void SequentialVertexSmoother::smooth(xAOD::Vertex& vtx) const
 {

   //vector of associated tracks at vertex:
   //protection check whether vxTrackAtVertices are attached to the object
   if( vtx.vxTrackAtVertexAvailable() ) //TODO: is this the right thing to check?
   {
     std::vector<Trk::VxTrackAtVertex> & tracks = vtx.vxTrackAtVertex();

     //updating the tracks one by one
     if(!tracks.empty())
     {
       for(std::vector<Trk::VxTrackAtVertex>::iterator i = tracks.begin(); i!=tracks.end(); ++i)
       {
           //TODO: There were previously null pointer checks on both the contents and vector itself
           //      of std::vector<Trk::VxTrackAtVertex*>* in VxCandidate
           //      but now in xAOD::Vertex, there is only a std::vector<Trk::VxTrackAtVertex> so
           //      pointer checks are not possible
           //
           //      -David S.
           m_vertexTrackUpdator->update( *i, vtx );
       }//end of loop over all tracks in particular vertex
     }
     else{

       //something wrong: notification to user
       msg(MSG::ERROR)  << "Vertex to be smoothed containes no tracks!" << endmsg;
       msg(MSG::ERROR)  << "No action taken; Initial xAOD::Vertex returned." << endmsg;
     }//end of protection statement
   }else{
     msg(MSG::ERROR)  << "Vertex to be smoothed has no vxTrackAtVertices available!" << endmsg;
     msg(MSG::ERROR)  << "No action taken; Initial xAOD::Vertex returned." << endmsg;
   }//end of protection check whether vxTrackAtVertices are attached to the object
 }//end of smooth method
 
}//end of the namespace definitions
