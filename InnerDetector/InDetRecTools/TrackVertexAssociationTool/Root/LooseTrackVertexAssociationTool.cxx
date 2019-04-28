/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define LOOSETRACKVERTEXASSOCIATIONTOOL_CXX

#include "TrackVertexAssociationTool/LooseTrackVertexAssociationTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

using namespace std;

namespace CP
{

  LooseTrackVertexAssociationTool::LooseTrackVertexAssociationTool(std::string name):
    AsgTool(name)
  {
    declareProperty("dzSinTheta_cut", m_dz_cut = 3.); 
    declareProperty("d0_cut", m_d0_cut = 2.);  // cut on the d0
  }

  StatusCode LooseTrackVertexAssociationTool::initialize()
  {
    //    dz_cut=2;

    ATH_MSG_WARNING("LooseTrackVertexAssociationTool is being depricated. Please use the new TrackVertexAssociationTool instead.");

    return StatusCode::SUCCESS;
  }

  void LooseTrackVertexAssociationTool::print() const
  {

  }



  bool LooseTrackVertexAssociationTool::isCompatible( const xAOD::TrackParticle &trk, const xAOD::Vertex &vx) const // return the decision of with the track match to the vertex
  {
    float dz_vx=0.;
    bool status=false;

    status=isMatch(trk, vx, dz_vx);

    return status;

  }
  //
  xAOD::TrackVertexAssociationMap LooseTrackVertexAssociationTool::getMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list ) const
  {
    xAOD::TrackVertexAssociationMap trktovxmap;

    // initialize map
    for(unsigned int vxi=0; vxi < vx_list.size() ; vxi++)
    {
      xAOD::TrackVertexAssociationList trktovxlist;
      trktovxlist.clear();
      for(unsigned int trki=0; trki< trk_list.size();trki++ )
      {
        if(isCompatible(*(trk_list.at(trki)), *(vx_list.at(vxi))))
        {
          trktovxlist.push_back((trk_list.at(trki)));
        }
      }
      trktovxmap[(vx_list.at(vxi))]=trktovxlist;
    }

    return trktovxmap;
  }

  xAOD::TrackVertexAssociationMap LooseTrackVertexAssociationTool::getMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont ) const
  {
    xAOD::TrackVertexAssociationMap trktovxmap;

    // initialize map
    for(unsigned int vxi=0; vxi < vxCont.size() ; vxi++)
    {
      xAOD::TrackVertexAssociationList trktovxlist;
      trktovxlist.clear();

      for(unsigned int trki=0; trki < trkCont.size(); trki++)
      {
        if(isCompatible(*(trkCont.at(trki)),*(vxCont.at(vxi))))
        {
          trktovxlist.push_back((trkCont.at(trki)));
        }
      }
      trktovxmap[(vxCont.at(vxi))]=trktovxlist;
    }

    return trktovxmap;
  }




  ElementLink< xAOD::VertexContainer> LooseTrackVertexAssociationTool::getUniqueMatchVertexLink(const xAOD::TrackParticle &trk,  const xAOD::VertexContainer &vxCont) const
  {
    ElementLink< xAOD::VertexContainer> vx_link_tmp;

    const xAOD::Vertex *vx_tmp=getUniqueMatchVx(trk, vxCont);
    
    if(vx_tmp!=NULL)
    {
      vx_link_tmp.toContainedElement(vxCont,vx_tmp);
    }
    return vx_link_tmp;

  }


  const xAOD::Vertex* LooseTrackVertexAssociationTool::getUniqueMatchVertex( const xAOD::TrackParticle &trk, std::vector< const xAOD::Vertex *> &vx_list) const // return the  vertex matched with the tracks in the vx_list
  {
    int vx_index=-1;

    float mini_dz=m_dz_cut;
    int mini_index=-1;

    for(unsigned int vxi=0; vxi < vx_list.size(); ++vxi)
    {
      float dz_vx=0.;

      bool match_status=isMatch(trk, *(vx_list.at(vxi)), dz_vx);
      if(match_status)
      {
        if(dz_vx<mini_dz)
        {
          mini_dz=dz_vx;
          mini_index=vxi;
        }
      }
    }

    // check if get the matched Vertex, for the tracks not used in vertex fit
    if(mini_index==-1)
    {
      ATH_MSG_DEBUG("Could not find any matched vertex for this track");
      //    return StatusCode::FAILURE;
    }
    else
    {
      vx_index=mini_index;
      ATH_MSG_DEBUG("Find matched vertex, index: "<< vx_index);
      return (vx_list.at(vx_index));
    }

    const xAOD::Vertex *dummyVx=NULL;

    return dummyVx;

  }


  xAOD::TrackVertexAssociationMap LooseTrackVertexAssociationTool::getUniqueMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list ) const
  {
    xAOD::TrackVertexAssociationMap trktovxmap;

    // initialize map
    for(unsigned int vxi=0; vxi < vx_list.size() ; vxi++)
    {
      xAOD::TrackVertexAssociationList trktovxlist;
      trktovxlist.clear();

      trktovxmap[(vx_list.at(vxi))]=trktovxlist;
    }

    for(unsigned int trki=0; trki < trk_list.size(); trki++)
    {
      const xAOD::Vertex* vx_match=getUniqueMatchVertex(*(trk_list.at(trki)), vx_list);
      if(vx_match!=NULL) // can find matched vertex
      {
        trktovxmap[vx_match].push_back((trk_list.at(trki)));
      }
    }

    return trktovxmap;
  }

  xAOD::TrackVertexAssociationMap LooseTrackVertexAssociationTool::getUniqueMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont ) const
  {
    xAOD::TrackVertexAssociationMap trktovxmap;

    // initialize map
    for(unsigned int vxi=0; vxi < vxCont.size() ; vxi++)
    {
      xAOD::TrackVertexAssociationList trktovxlist;
      trktovxlist.clear();

      trktovxmap[(vxCont.at(vxi))]=trktovxlist;
    }

    for(unsigned int trki=0; trki < trkCont.size(); trki++)
    {
      const xAOD::Vertex* vx_match=getUniqueMatchVx(*(trkCont.at(trki)), vxCont);
      if(vx_match!=NULL) // can find matched vertex
      {
        trktovxmap[vx_match].push_back((trkCont.at(trki)));
      }
    }

    return trktovxmap;
  }


  bool LooseTrackVertexAssociationTool::isMatch( const xAOD::TrackParticle &trk, const xAOD::Vertex &vx, float& dz) const
  {
    ATH_MSG_DEBUG("<###### Enter: isMath() function ######>");
    // Note all the parameters is respected to (0,0,0)

    //      float trk_z0=trk->parameterZ( pos_index );
    float trk_z0=trk.z0();
    float trk_d0=trk.d0();
    float beamspot_z0=trk.vz();
    float theta=trk.theta();
    // check the vertex, return false if the vertex is a dummy one
    if(vx.vertexType()!=xAOD::VxType::NoVtx)  // select good vertex
    {
      float vx_z0=vx.z();
      if(fabs((trk_z0-vx_z0+beamspot_z0)*sin(theta))<m_dz_cut && fabs(trk_d0)<=m_d0_cut) // cut 
      {
        dz=fabs((trk_z0-vx_z0+beamspot_z0)*sin(theta));
        return true;
      }
      else return false;
    }
    else
    {
      ATH_MSG_DEBUG("The Vertex is a fake one, will not do track-vertex association");
      return false;
    }

  }

  const xAOD::Vertex* LooseTrackVertexAssociationTool::getUniqueMatchVx( const xAOD::TrackParticle &trk, const xAOD::VertexContainer &vxCont) const // return the  vertex matched with the tracks in the vx_list
  {
    int vx_index=-1;

    float mini_dz=m_dz_cut;
    int mini_index=-1;

    for(unsigned int vxi=0; vxi < vxCont.size(); ++vxi)
    {
      float dz_vx=0.;
      bool match_status=isMatch(trk, *(vxCont.at(vxi)), dz_vx);
      if(match_status)
      {
        if(dz_vx<mini_dz)
        {
          mini_dz=dz_vx;
          mini_index=vxi;
        }
      }
    }

    // check if get the matched Vertex, for the tracks not used in vertex fit

    if(mini_index==-1)
    {
      ATH_MSG_DEBUG("Could not find any matched vertex for this track");
    }
    else
    {
      vx_index=mini_index;
      ATH_MSG_DEBUG("Find matched vertex, index: "<< vx_index);
      return vxCont.at(vx_index);
    }
    const xAOD::Vertex *dummyVx=NULL;

    return dummyVx;


//    return 0;

  }

}
