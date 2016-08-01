/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
         DummyVertexSmoother.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitters/DummyVertexSmoother.h"
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
    msg(MSG::FATAL)<<"Could not find extrapolator tool." << endmsg;
    return sc;
   }   
   msg(MSG::INFO)  << "Initialize successful" << endmsg;
   return StatusCode::SUCCESS;
  }

  StatusCode DummyVertexSmoother::finalize() 
  {
    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Had to make the vtx non-const because xAOD::Vertex stores
  // Trk::VxTrackAtVertex and not Trk::VxTrackAtVertex*
  void DummyVertexSmoother::smooth(xAOD::Vertex & vtx) const
  {

    //vector of associated tracks at vertex:
    if( vtx.vxTrackAtVertexAvailable() ) //TODO: is this the right thing to check?
    {
      std::vector<Trk::VxTrackAtVertex> & tracks = vtx.vxTrackAtVertex();

      if(tracks.size() !=0)
      {
        //iteratively updating private members of VxTrackAtVertex
        std::vector<Trk::VxTrackAtVertex>::iterator t_it = tracks.begin();
        std::vector<Trk::VxTrackAtVertex>::iterator te = tracks.end();
        for( ;t_it != te;++t_it)
        {

          //TODO: There were previously null pointer checks on both the contents and vector itself
          //      of std::vector<Trk::VxTrackAtVertex*>* in VxCandidate
          //      but now in xAOD::Vertex, there is only a std::vector<Trk::VxTrackAtVertex> so
          //      pointer checks are not possible
          //
          //      -David S.

          //now the extrapolation will happen anyway, creating the surface
          PerigeeSurface perigeeSurface(vtx.position());
          const Trk::TrackParameters * initPar = (*t_it).initialPerigee();

          if(initPar != 0)
          {
            const Trk::TrackParameters * extrapolatedPerigee(m_extrapolator->extrapolate(*initPar,perigeeSurface));
            if(extrapolatedPerigee != 0)
            {
              (*t_it).setPerigeeAtVertex(const_cast<Trk::TrackParameters*>(extrapolatedPerigee));
            } else {
              msg(MSG::ERROR)  << " Extrapolation failed; VxTrackAtertex will not be updated" << endmsg;
            }//end of successfull extrapolation check
          } else {
            msg(MSG::WARNING) << " The VxTrackAtVertex passed has no initial Parameters? This track will not be refitted" << endmsg;
          }//end of initial parameters protection check

          /*     
          LinearizedTrack * linState = (*t_it)->linState();     
          
          if(linState != 0)
          {
            //taking the last perigee parameters used in calculation and extrapolating them 
            //toawrds the fitted vertex. This saves calculation time.
            // MeasuredPerigee lastPerigee = linState->expectedPerigeeAtPCA(); 
            TrackParameters * lastPerigee = linState->expectedParametersAtPCA();
 	
            //here the extrapolation extrapolation finally happens
            // const MeasuredPerigee * extrapolatedPerigee(dynamic_cast<const Trk::MeasuredPerigee*>(m_extrapolator->extrapolateDirectly(lastPerigee,perigeeSurface)));
            const TrackParameters * extrapolatedPerigee(m_extrapolator->extrapolateDirectly(*lastPerigee,perigeeSurface));
  
            if(extrapolatedPerigee != 0)
	    {
	      (*t_it)->setPerigeeAtVertex(const_cast<Trk::TrackParameters*>(extrapolatedPerigee));
	    }else{
	      MsgStream msg(msgSvc(), name());
              msg(MSG::ERROR)  << " Extrapolation failed; VxTrackAtertex will not be updated" << endmsg;
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
                msg(MSG::ERROR)  << " Extrapolation failed; VxTrackAtertex will not be updated" << endmsg;
	      }//end of non-zero extrapolated perigee check
	    }else{
	      MsgStream msg(msgSvc(), name());
              msg(MSG::ERROR)  << " The VxTrackAtVertex passed has no LinearizedTrack neither Initial perigee???" << endmsg;
	    }//end of non-zero initial perigee check 
          }//end of non zero linearized state check
          */

        }//end of loop over all fitted tracks
      } else {
        msg(MSG::ERROR)  << "xAOD::Vertex with empty vector of tracks is passed to the Vertex Smoother" << endmsg;
        msg(MSG::ERROR)  << "No action taken; Input xAOD::Vertex stays unchanged " << endmsg;
      }//end of empty vector check

    } else {
      msg(MSG::ERROR)  << "Vertex to be smoothed has no vxTrackAtVertices available!" << endmsg;
      msg(MSG::ERROR)  << "No action taken; Initial xAOD::Vertex returned." << endmsg;
    }//end of protection check whether vxTrackAtVertices are attached to the object
  }//end of smooth method
  
}//end of namespace definitions
