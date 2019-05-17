/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define TIGHTTRACKVERTEXASSOCIATIONTOOL_CXX

#include "TrackVertexAssociationTool/TightTrackVertexAssociationTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

using namespace std;

namespace CP
{

  TightTrackVertexAssociationTool::TightTrackVertexAssociationTool(std::string name):
    AsgTool(name)
  {
    declareProperty("dzSinTheta_cut", m_dz_cut = 3.); 
    declareProperty("doPV", m_doPV=false); // if true, only apply the dz recover for the PV 
    declareProperty("Applyd0Selection", m_dod0sel=false);
    declareProperty("d0_cut", m_d0_cut=2.5);
  }

  StatusCode TightTrackVertexAssociationTool::initialize()
  {
    //    dz_cut=2;

    ATH_MSG_WARNING("TightTrackVertexAssociationTool is being depricated. Please use the new TrackVertexAssociationTool instead.");

    return StatusCode::SUCCESS;
  }

  void TightTrackVertexAssociationTool::print() const
  {

  }



  bool TightTrackVertexAssociationTool::isCompatible( const xAOD::TrackParticle &trk, const xAOD::Vertex &vx) const // return the decision of with the track match to the vertex
  {
    float dz_vx=0.;
    bool status=false;

    MatchStatus matchstatus=doTightMatch( trk, vx, dz_vx);
    //    status=isMatch( trk, vx, dz_vx);
    if(matchstatus == TightTrackVertexAssociationTool::UsedInFit || matchstatus == TightTrackVertexAssociationTool::Matched )
    {
      status = true;
    }
    else
    {
      status = false;
    }

    return status;

  }

  xAOD::TrackVertexAssociationMap TightTrackVertexAssociationTool::getMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list ) const
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

  xAOD::TrackVertexAssociationMap TightTrackVertexAssociationTool::getMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont ) const
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
          trktovxlist.push_back(trkCont.at(trki));
        }
      }
      trktovxmap[vxCont.at(vxi)]=trktovxlist;
    }

    return trktovxmap;
  }



  //

  ElementLink< xAOD::VertexContainer> TightTrackVertexAssociationTool::getUniqueMatchVertexLink(const xAOD::TrackParticle &trk,  const xAOD::VertexContainer &vxCont) const
  {
    ElementLink< xAOD::VertexContainer> vx_link_tmp;
    const xAOD::Vertex *vx_tmp=getUniqueMatchVx(trk, vxCont);

    if(vx_tmp!=NULL)
    {
      vx_link_tmp.toContainedElement(vxCont,vx_tmp);
    }
    return vx_link_tmp;
  }


  const xAOD::Vertex* TightTrackVertexAssociationTool::getUniqueMatchVertex( const xAOD::TrackParticle &trk, std::vector< const xAOD::Vertex *> &vx_list) const // return the  vertex matched with the tracks in the vx_list
  {
    int vx_index=-1;

    float mini_dz=m_dz_cut;
    int mini_index=-1;

    for(unsigned int vxi=0; vxi < vx_list.size(); ++vxi)
    {
      float dz_vx=0.;

      MatchStatus matchstatus=doTightMatch(trk, *(vx_list.at(vxi)), dz_vx);
      if(matchstatus == TightTrackVertexAssociationTool::UsedInFit)  // track used in fit 
      {
        vx_index=vxi;  // return the best matched vertex
        ATH_MSG_DEBUG("Find matched vertex, index: "<< vx_index);
        return vx_list.at(vx_index);  
      }
      else
      {
        if((m_doPV&& (vx_list.at(vxi))->vertexType()==xAOD::VxType::PriVtx) || (!m_doPV))   // if m_doPV flag set to true, only apply recover the track associated to PV by using dz cut, otherwise, will doing this procedure for all the vertices
        {
          if(matchstatus == TightTrackVertexAssociationTool::Matched) // not a fit used track, but get matched
          {
            if(dz_vx<mini_dz)
            {
              mini_dz=dz_vx;
              mini_index=vxi;
            }
          }
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


//    return 0;
  }

  xAOD::TrackVertexAssociationMap TightTrackVertexAssociationTool::getUniqueMatchMap( std::vector< const xAOD::TrackParticle *> &trk_list , std::vector< const xAOD::Vertex *> &vx_list ) const
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

  xAOD::TrackVertexAssociationMap TightTrackVertexAssociationTool::getUniqueMatchMap( const xAOD::TrackParticleContainer &trkCont , const xAOD::VertexContainer &vxCont ) const
  {
    xAOD::TrackVertexAssociationMap trktovxmap;

    // initialize map
    for(unsigned int vxi=0; vxi < vxCont.size() ; vxi++)
    {
      xAOD::TrackVertexAssociationList trktovxlist;
      trktovxlist.clear();

      trktovxmap[vxCont.at(vxi)]=trktovxlist;
    }

    for(unsigned int trki=0; trki < trkCont.size(); trki++)
    {
      const xAOD::Vertex* vx_match=getUniqueMatchVx(*(trkCont.at(trki)), vxCont);
      if(vx_match!=NULL) // can find matched vertex
      {
        trktovxmap[vx_match].push_back(trkCont.at(trki));
      }
    }

    return trktovxmap;
  }

  // private functions

  TightTrackVertexAssociationTool::MatchStatus TightTrackVertexAssociationTool::doTightMatch(  const xAOD::TrackParticle &trk, const xAOD::Vertex &vx, float& dz) const
  {
    ATH_MSG_DEBUG("<###### Enter: doTightMatch() function ######>");

    if(vx.vertexType()!=xAOD::VxType::NoVtx )
    {

      if(trk.vertex()==&vx) // check whether the track is used for the given vertex fit.
      {
        ATH_MSG_DEBUG("This track is used to fit the vertex");

        float trk_z0=trk.z0();
        float beamspot_z0=trk.vz();
        float theta=trk.theta();

        dz=fabs((trk_z0-(vx.z())+beamspot_z0)*sin(theta)); // calculate dz
        return UsedInFit;
      }

      if(trk.vertex()==0) // track is not used for any vertex fit
      {
        // tracks not used in fitting process
        float trk_z0=trk.z0();
        float beamspot_z0=trk.vz();
        float theta=trk.theta();
        float trk_d0=trk.d0();

        dz=fabs((trk_z0-(vx.z())+beamspot_z0)*sin(theta)); // calculate dz
        if((dz < m_dz_cut && !m_dod0sel) ||(dz < m_dz_cut && m_dod0sel && fabs(trk_d0)<m_d0_cut ) ) // apply d0 selection when turn on the d0 selection tag
        {
          ATH_MSG_DEBUG("Track match to Vertex");
          return Matched;
        }
        else
        {
          return UnMatch;
        }
      }
      else return UnMatch;
    }
    else
    {
      ATH_MSG_DEBUG("The vertex is a fake one");
      return UnMatch;
    }

    return Matched;
  }

  const xAOD::Vertex* TightTrackVertexAssociationTool::getUniqueMatchVx( const xAOD::TrackParticle &trk, const xAOD::VertexContainer &vxCont) const // return the  vertex matched with the tracks in the vx_list
  {
    int vx_index=-1;

    float mini_dz=m_dz_cut;
    int mini_index=-1;

    for(unsigned int vxi=0; vxi < vxCont.size(); ++vxi)
    {
      float dz_vx=0.;

      MatchStatus matchstatus=doTightMatch(trk, *(vxCont.at(vxi)), dz_vx);
      if(matchstatus == TightTrackVertexAssociationTool::UsedInFit)  // track used in fit 
      {      
        vx_index=vxi;  // return the best matched vertex
        ATH_MSG_DEBUG("Find matched vertex, index: "<< vx_index);
        return vxCont.at(vx_index);  
      }
      else
      {
        if((m_doPV&& (vxCont.at(vxi))->vertexType()==xAOD::VxType::PriVtx) || (!m_doPV))   // if m_doPV flag set to true, only apply recover the track associated to PV by using dz cut, otherwise, will doing this procedure for all the vertices
        {
          if(matchstatus == TightTrackVertexAssociationTool::Matched) // not a fit used track, but get matched
          {
            if(dz_vx<mini_dz)
            {
              mini_dz=dz_vx;
              mini_index=vxi;
            }
          }
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
      return (vxCont.at(vx_index));
    }
    const xAOD::Vertex *dummyVx=NULL;

    return dummyVx;


    //    return 0;


  }



}
