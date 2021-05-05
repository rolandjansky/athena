/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   V0ContainerCnv.cxx
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//
//-----------------------------------------------------------------------------


#include "TrkParticleBase/TrackParticleBase.h"
#include "V0ContainerCnv.h"

#include "TrkEventTPCnv/V0ContainerCnv_tlp1.h"
#include "TrkEventTPCnv/V0ContainerCnv_tlp2.h"

V0ContainerCnv::V0ContainerCnv( ISvcLocator *svcloc ):
                           V0ContainerCnvBase(svcloc),
                           m_msgSvc( msgSvc() ),
                           m_log( m_msgSvc, "V0ContainerCnv" )
 {}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode V0ContainerCnv::initialize()
{
  StatusCode sc = V0ContainerCnvBase::initialize();
  if( sc.isFailure() ) 
  {
    m_log << MSG::FATAL << "Could not initialize V0ContainerCnvBase" << endmsg;
    return sc;
  }
//-------------------------------------------------------------------------
// Set up the message stream
//-------------------------------------------------------------------------
  m_log.setLevel( m_msgSvc->outputLevel() );
  m_log << MSG::INFO << "V0ContainerCnv::initialize()" << endmsg;
  
  return StatusCode::SUCCESS;
  
}//end of initialize method


V0Container_PERS * V0ContainerCnv::createPersistent( V0Container *transCont)
{
 m_log.setLevel( m_msgSvc->outputLevel() );
 updateLog(); // Make m_log indicate the current key
 V0Container_PERS * pVxCont = m_TPConverter.createPersistent( transCont, m_log );

 // std::cout<<" WRITING: Dumping the contents of the V0Container "<<std::endl;

/* 
 std::cout<<"WRITING: Dumping the contents of the V0Container "<<std::endl;
 std::cout<<"------------------------------VxVertex related "<<std::endl;
 std::cout<<" m_v0Containers size:        "<< pVxCont->m_v0Containers.size()<<std::endl;
 std::cout<<" m_v0Candidates size:        "<< pVxCont->m_v0Candidates.size() <<std::endl;
 std::cout<<" m_v0Hypothesises size:      "<< pVxCont->m_v0Hypothesises.size()<<std::endl;
 std::cout<<" m_extendedVxCandidate size: "<< pVxCont->m_extendedVxCandidates.size()<<std::endl;
 std::cout<<" m_vxCandidate size:         "<< pVxCont->m_vxCandidates.size()<<std::endl;
 std::cout<<" m_vxTrackAtVertices size:   "<< pVxCont->m_vxTracksAtVertex.size()<<std::endl;
 std::cout<<" m_recVertices size:         "<< pVxCont->m_recVertices.size()<<std::endl;
 std::cout<<" m_vertices size:            "<< pVxCont->m_vertices.size()<<std::endl;
 
 std::cout<<"------------------------------TrkTrack related "<<std::endl;
 std::cout<<" m_tracks size:              "<< pVxCont->m_tracks.size()<<std::endl;
 std::cout<<"------------------------------TrkTrackParameters related "<<std::endl;
 std::cout<<" m_trackParameters size:     "<< pVxCont->m_trackParameters.size()<<std::endl;
 std::cout<<" m_perigees size:            "<< pVxCont->m_perigees.size()<<std::endl;
 std::cout<<" m_measPerigees size:        "<< pVxCont->m_measPerigees.size()<<std::endl;
 std::cout<<" m_surfaces size:            "<< pVxCont->m_surfaces.size()<<std::endl; 
 std::cout<<" m_fitQualities size:        "<< pVxCont->m_fitQualities.size()<<std::endl;
 std::cout<<" m_hepSymMatrices size:      "<< pVxCont->m_hepSymMatrices.size()<<std::endl; 
*/ 

 return pVxCont;

}//end of create persistent method

V0Container * V0ContainerCnv::createTransient()
{
    static pool::Guid p2_guid( "9BC2B917-940A-4121-8E8B-8C99E178D8C1" );
    static pool::Guid p1_guid( "402A9BD1-4476-4E97-A5E4-EB4D742FBB38" );
    static pool::Guid p0_guid( "0C363A21-1B86-4B39-8A84-A62161B13C7B" );

  V0Container *p_collection = nullptr;
    if( compareClassGuid( p2_guid ) )     
    {
       // std::cout << "V0ContainerCnv::createTransient: do new TP p2" << std::endl;
       /*
     usingTPCnvForReading( m_TPConverter );
     std::unique_ptr< V0Container_PERS >  p_coll( poolReadObject< V0Container_PERS >() );
       */

  /* 
     std::cout<<"READING: Dumping the contents of the V0Container "<<std::endl;
     std::cout<<"------------------------------VxVertex related "<<std::endl;
     std::cout<<" m_v0Containers size:         "<< p_coll->m_v0Containers.size()<<std::endl;
     std::cout<<" m_v0Candidates size:         "<< p_coll->m_v0Candidates.size() <<std::endl;
     std::cout<<" m_v0Hypothesises size:       "<< p_coll->m_v0Hypothesises.size()<<std::endl;
     std::cout<<" m_extendedVxCandidate size:  "<< p_coll->m_extendedVxCandidates.size()<<std::endl;
     std::cout<<" m_vxCandidate size:          "<< p_coll->m_vxCandidates.size()<<std::endl;
     std::cout<<" m_vxTrackAtVertices size:    "<< p_coll->m_vxTracksAtVertex.size()<<std::endl;
     std::cout<<" m_recVertices size:          "<< p_coll->m_recVertices.size()<<std::endl;
     std::cout<<" m_vertices size:             "<< p_coll->m_vertices.size()<<std::endl;

     std::cout<<"------------------------------TrkTrack related "<<std::endl;
     std::cout<<" m_tracks size:            "<< p_coll->m_tracks.size()<<std::endl;
     std::cout<<"------------------------------TrkTrackParameters related "<<std::endl;
     std::cout<<" m_trackParameters size:   "<< p_coll->m_trackParameters.size()<<std::endl;
     std::cout<<" m_perigees size:          "<< p_coll->m_perigees.size()<<std::endl;
     std::cout<<" m_measPerigees size:      "<< p_coll->m_measPerigees.size()<<std::endl;
     std::cout<<" m_surfaces size:          "<< p_coll->m_surfaces.size()<<std::endl; 
     std::cout<<" m_fitQualities size:      "<< p_coll->m_fitQualities.size()<<std::endl;
     std::cout<<" m_hepSymMatrices size:    "<< p_coll->m_hepSymMatrices.size()<<std::endl; 
  */
       poolReadObject< Trk::V0Container_tlp2 >(m_TPConverter);
       p_collection = m_TPConverter.createTransient( m_log );

    }else if( compareClassGuid( p1_guid ) )     
  {
     // std::cout << "V0ContainerCnv::createTransient: do TP p1" << std::endl;
     /*
   usingTPCnvForReading( m_TPConverter );
   std::unique_ptr< V0Container_PERS >  p_coll( poolReadObject< V0Container_PERS >() );
     */
 
/* 
   std::cout<<"READING: Dumping the contents of the V0Container "<<std::endl;
   std::cout<<"------------------------------VxVertex related "<<std::endl;
   std::cout<<" m_v0Containers size:         "<< p_coll->m_v0Containers.size()<<std::endl;
   std::cout<<" m_v0Candidates size:         "<< p_coll->m_v0Candidates.size() <<std::endl;
   std::cout<<" m_v0Hypothesises size:       "<< p_coll->m_v0Hypothesises.size()<<std::endl;
   std::cout<<" m_extendedVxCandidate size:  "<< p_coll->m_extendedVxCandidates.size()<<std::endl;
   std::cout<<" m_vxCandidate size:          "<< p_coll->m_vxCandidates.size()<<std::endl;
   std::cout<<" m_vxTrackAtVertices size:    "<< p_coll->m_vxTracksAtVertex.size()<<std::endl;
   std::cout<<" m_recVertices size:          "<< p_coll->m_recVertices.size()<<std::endl;
   std::cout<<" m_vertices size:             "<< p_coll->m_vertices.size()<<std::endl;
 
   std::cout<<"------------------------------TrkTrack related "<<std::endl;
   std::cout<<" m_tracks size:            "<< p_coll->m_tracks.size()<<std::endl;
   std::cout<<"------------------------------TrkTrackParameters related "<<std::endl;
   std::cout<<" m_trackParameters size:   "<< p_coll->m_trackParameters.size()<<std::endl;
   std::cout<<" m_perigees size:          "<< p_coll->m_perigees.size()<<std::endl;
   std::cout<<" m_measPerigees size:      "<< p_coll->m_measPerigees.size()<<std::endl;
   std::cout<<" m_surfaces size:          "<< p_coll->m_surfaces.size()<<std::endl; 
   std::cout<<" m_fitQualities size:      "<< p_coll->m_fitQualities.size()<<std::endl;
   std::cout<<" m_hepSymMatrices size:    "<< p_coll->m_hepSymMatrices.size()<<std::endl; 
*/
     V0ContainerCnv_tlp1 tpCnv;
     poolReadObject< Trk::V0Container_tlp1 >(tpCnv);
     p_collection = tpCnv.createTransient( m_log );
  
  }else if( compareClassGuid( p0_guid ) ){
  // std::cout << "V0ContainerCnv::createTransient: use old converter" << std::endl;  
   p_collection = poolReadObject< V0Container >();
   
  }else  throw std::runtime_error( "Unsupported persistent version of V0Container" );
    
  return p_collection;
  
}//end of create transient method

void V0ContainerCnv::updateLog(){  
    //const DataObject* dObj = getDataObject();
    //if (dObj==0) return; // Can't do much if this fails.
  //const std::string  key = (dObj->name());
 
     //m_log.m_source="V0ContainerCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}


