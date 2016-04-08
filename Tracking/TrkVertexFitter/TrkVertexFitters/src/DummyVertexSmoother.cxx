/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
         DummyVertexSmoother.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitters/DummyVertexSmoother.h"
#include "VxVertex/VxCandidate.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "VxVertex/VxTrackAtVertex.h"

namespace Trk
{

  DummyVertexSmoother::DummyVertexSmoother(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p), m_extrapolator(0), m_extrapolatorName("Trk::Extrapolator"), m_extrapolatorInstance("InDetExtrapolator") 
  {   
   declareProperty("ExtrapolatorName",	 m_extrapolatorName);
   declareProperty("ExtrapolatorInstance", m_extrapolatorInstance);  
   declareInterface<IVertexSmoother>(this);
  }

  DummyVertexSmoother::~DummyVertexSmoother() {}

  StatusCode DummyVertexSmoother::initialize() 
  { 
   StatusCode sc = AlgTool::initialize();
   sc = toolSvc()->retrieveTool(m_extrapolatorName, m_extrapolatorInstance, m_extrapolator);
   if (sc.isFailure())
   {
    msg(MSG::FATAL)<<"Could not find extrapolator tool." << endreq;
    return sc;
   }   
   msg(MSG::INFO)  << "Initialize successful" << endreq;
   return StatusCode::SUCCESS;
  }

  StatusCode DummyVertexSmoother::finalize() 
  {
    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  void DummyVertexSmoother::smooth(const VxCandidate & vtx) const 
  {
    
//this is a RecVertex created by a fitter, so we can just store it so far 
   Trk::RecVertex rv = vtx.recVertex();
  
//vector of associated tracks at vertex:
   if(vtx.vxTrackAtVertex()!=0)
   {
    std::vector<Trk::VxTrackAtVertex*> tracks = *vtx.vxTrackAtVertex();
    if(tracks.size() !=0 )
    {
 //iteratively updating private members of VxTrackAtVertex
     std::vector<Trk::VxTrackAtVertex*>::const_iterator t_it = tracks.begin();
     std::vector<Trk::VxTrackAtVertex*>::const_iterator te = tracks.end();
     for( ;t_it != te;++t_it)
     {
     
// one more protection check: looking for zero track pointers    
      if((*t_it) != 0)
      {
      
//now the extrapolation will happen anyway, creating the surface      
       PerigeeSurface perigeeSurface(rv.position());
       const Trk::TrackParameters * initPar = (*t_it)->initialPerigee();

       if(0 != initPar)
       {
        const Trk::TrackParameters * extrapolatedPerigee(m_extrapolator->extrapolate(*initPar,perigeeSurface));
	if(extrapolatedPerigee != 0)
	{
	 (*t_it)->setPerigeeAtVertex(const_cast<Trk::TrackParameters*>(extrapolatedPerigee));
	}else{
         msg(MSG::ERROR)  << " Extrapolation failed; VxTrackAtertex will not be updated" << endreq;
	}//end of successfull extrapolation check
       }else{       
        msg(MSG::WARNING) << " The VxTrackAtVertex passed has no initial Parameters? This track will not be refitted" << endreq;
       }//end of initial parameters protecion check
             
/*     
       LinearizedTrack * linState = (*t_it)->linState();     
          
       if(linState != 0)
       {
//taking the last perigee parameters used in calculation and extrapolating them 
//toawrds the fitted vertex. This saves calculation time.
//        MeasuredPerigee lastPerigee = linState->expectedPerigeeAtPCA(); 
        TrackParameters * lastPerigee = linState->expectedParametersAtPCA();
 	
//here the extrapolation extrapolation finally happens
//        const MeasuredPerigee * extrapolatedPerigee(dynamic_cast<const Trk::MeasuredPerigee*>(m_extrapolator->extrapolateDirectly(lastPerigee,perigeeSurface)));
        const TrackParameters * extrapolatedPerigee(m_extrapolator->extrapolateDirectly(*lastPerigee,perigeeSurface));
  
        if(extrapolatedPerigee != 0)
	{
	 (*t_it)->setPerigeeAtVertex(const_cast<Trk::TrackParameters*>(extrapolatedPerigee));
	}else{
	 MsgStream msg(msgSvc(), name());
         msg(MSG::ERROR)  << " Extrapolation failed; VxTrackAtertex will not be updated" << endreq;
	}//end of successfull extrapolation check	
       }else{
       
//there is no linearized state available; taking initial MeasuredPerigee
//and trying to work with it       
        const Trk::MeasuredPerigee * inPerigee = (*t_it)->initialPerigee(); 
	if(inPerigee !=0 )
	{
	 const MeasuredPerigee * extrapolatedPerigee(dynamic_cast<const Trk::MeasuredPerigee*>(m_extrapolator->extrapolateDirectly(*inPerigee,perigeeSurface)));
	 if(extrapolatedPerigee != 0)
         {
	  (*t_it)->setPerigeeAtVertex(const_cast<Trk::MeasuredPerigee*>(extrapolatedPerigee));
	 }else{
	  MsgStream msg(msgSvc(), name());
          msg(MSG::ERROR)  << " Extrapolation failed; VxTrackAtertex will not be updated" << endreq;
	 }//end of non-zero extrapolated perigee check
	}else{
	 MsgStream msg(msgSvc(), name());
         msg(MSG::ERROR)  << " The VxTrackAtVertex passed has no LinearizedTrack neither Initial perigee???" << endreq;
	}//end of non-zero initial perigee check 
       }//end of non zero linearized state check
*/       
      }else{
       msg(MSG::ERROR)  << "There is a zero pointer track passed to the Vertex Smoother." << endreq;
       msg(MSG::ERROR)  << "No action taken; Proceeding to the next track." << endreq;
      }//end of non-zero track pointers check   
     }//end of loop over all fitted tracks    
    }else{
     msg(MSG::ERROR)  << "VxCandidate with empty vector of tracks is passed to the Vertex Smoother" << endreq;
     msg(MSG::ERROR)  << "No action taken; Input VxCandidate stays unchanged " << endreq;
    }//end of empty vector check
   }//end of zero pointer check 
  }//end of smooth method
  
}//end of namespace definitions
