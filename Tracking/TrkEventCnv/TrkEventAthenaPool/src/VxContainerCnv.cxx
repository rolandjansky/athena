/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   VxContainerCnv.cxx
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//
//-----------------------------------------------------------------------------


#include "VxContainerCnv.h"

#include "TrkEventTPCnv/VxContainerCnv_tlp1.h"
#include "TrkEventTPCnv/VxContainerCnv_tlp2.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

VxContainerCnv::VxContainerCnv( ISvcLocator *svcloc ):
                           VxContainerCnvBase(svcloc),
                           m_msgSvc( msgSvc() ),
                           m_log( m_msgSvc, "VxContainerCnv" )
 {}
 
//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
VxContainerCnv::~VxContainerCnv()
{}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode VxContainerCnv::initialize()
{
  StatusCode sc = VxContainerCnvBase::initialize();
  if( sc.isFailure() ) 
  {
    m_log << MSG::FATAL << "Could not initialize VxContainerCnvBase" << endmsg;
    return sc;
  }
  
  m_log.setLevel( m_msgSvc->outputLevel() );
  m_log << MSG::INFO << "VxContainerCnv::initialize()" << endmsg;
  
  IConverter  *converter =  m_athenaPoolCnvSvc->converter( CLID(1943140) );
  m_log << MSG::INFO << "VxContainerCnv: MVFVxCandidate converter=" << converter << endmsg;
  registerExtendingCnv( converter );
  
  return StatusCode::SUCCESS;
  
}//end of initialize method

VxContainer_PERS *
VxContainerCnv::createPersistent( VxContainer *transCont) { 
//    std::cout<<"WRITING  VxContainer "<<std::endl;
   m_log.setLevel( m_msgSvc->outputLevel() );
   updateLog(); // Make m_log indicate the current key 
   VxContainer_PERS * pVxCont = m_TPConverter.createPersistent( transCont, m_log );

/* 
 std::cout<<"------------------------------VxVertex related "<<std::endl;
 std::cout<<" m_vxContainers size:      "<< pVxCont->m_vxContainers.size()<<std::endl;
 std::cout<<" m_vxCandidates size:      "<< pVxCont->m_vxCandidates.size() <<std::endl;
 std::cout<<" m_vxTrackAtVertices size: "<< pVxCont->m_vxTrackAtVertices.size()<<std::endl;
 std::cout<<" m_recVertices size:       "<< pVxCont->m_recVertices.size()<<std::endl;
 std::cout<<" m_vertices size:          "<< pVxCont->m_vertices.size()<<std::endl;
 
 std::cout<<"------------------------------TrkTrack related "<<std::endl;
 std::cout<<" m_tracks size:            "<< pVxCont->m_tracks.size()<<std::endl;
 std::cout<<"------------------------------TrkTrackParameters related "<<std::endl;
 std::cout<<" m_trackParameters size:   "<< pVxCont->m_trackParameters.size()<<std::endl;
 std::cout<<" m_perigees size:          "<< pVxCont->m_perigees.size()<<std::endl;
 std::cout<<" m_measPerigees size:      "<< pVxCont->m_measPerigees.size()<<std::endl;
 std::cout<<" m_surfaces size:          "<< pVxCont->m_surfaces.size()<<std::endl; 
 std::cout<<" m_fitQualities size:      "<< pVxCont->m_fitQualities.size()<<std::endl;
 std::cout<<" m_hepSymMatrices size:    "<< pVxCont->m_hepSymMatrices.size()<<std::endl; 
 std::cout<<"*************************************** "<<std::endl;
*/ 
 
 return pVxCont;

}//end of create persistent method


VxContainer * VxContainerCnv::createTransient()
{
    static pool::Guid p2_guid( "B5254571-6B6D-47F7-A52C-CBE857CF1812" );
    static pool::Guid p1_guid( "AFA5FE76-EEC6-4BD1-B704-B5747F729291" );
    static pool::Guid p0_guid( "639B478A-7355-4430-B6FC-F0A6148A2E87" );

  VxContainer *p_collection = 0;
  if( compareClassGuid( p2_guid ) ) {
  //  std::cout << "VxContainerCnv::createTransient: do new TP 2" << std::endl;  
     usingTPCnvForReading( m_TPConverter );
     std::unique_ptr< VxContainer_PERS >  p_coll( poolReadObject< VxContainer_PERS >() );

  /*  
     std::cout<<"READING: Dumping the contents of the VxContainer "<<std::endl;
     std::cout<<"------------------------------VxVertex related "<<std::endl;
     std::cout<<" m_vxContainers size:      "<< p_coll->m_vxContainers.size()<<std::endl;
     std::cout<<" m_vxCandidates size:      "<< p_coll->m_vxCandidates.size() <<std::endl;
     std::cout<<" m_vxTrackAtVertices size: "<< p_coll->m_vxTrackAtVertices.size()<<std::endl;
     std::cout<<" m_recVertices size:       "<< p_coll->m_recVertices.size()<<std::endl;
     std::cout<<" m_vertices size:          "<< p_coll->m_vertices.size()<<std::endl;

     std::cout<<"------------------------------TrkTrack related "<<std::endl;
     std::cout<<" m_tracks size:            "<< p_coll->m_tracks.size()<<std::endl;
     std::cout<<"------------------------------TrkTrackParameters related "<<std::endl;
     std::cout<<" m_trackParameters size:   "<< p_coll->m_trackParameters.size()<<std::endl;
     std::cout<<" m_perigees size:          "<< p_coll->m_perigees.size()<<std::endl;
     std::cout<<" m_measPerigees size:      "<< p_coll->m_measPerigees.size()<<std::endl;
     std::cout<<" m_surfaces size:          "<< p_coll->m_surfaces.size()<<std::endl; 
     std::cout<<" m_fitQualities size:      "<< p_coll->m_fitQualities.size()<<std::endl;
     std::cout<<" m_hepSymMatrices  size:   "<< p_coll->m_hepSymMatrices.size()<<std::endl; 
     std::cout<<"*************************************** "<<std::endl;  
  */    
     p_collection = m_TPConverter.createTransient( p_coll.get(), m_log );

    }
    else if( compareClassGuid( p1_guid ) ) {
 //       std::cout << "VxContainerCnv::createTransient: do TP 1" << std::endl;  
        
        // usingTPCnvForReading( m_TPConverter );
        // std::unique_ptr< VxContainer_PERS >  p_coll( poolReadObject< VxContainer_PERS >() );
        // p_collection = m_TPConverter.createTransient( p_coll.get(), m_log );
        
        VxContainerCnv_tlp1 tmpTPCnv;
        usingTPCnvForReading( tmpTPCnv );
        std::unique_ptr< Trk::VxContainer_tlp1 >  p_coll( poolReadObject< Trk::VxContainer_tlp1 >() );
        p_collection = tmpTPCnv.createTransient( p_coll.get(), m_log );
        
    //    std::cout << "VxContainerCnv::createTransient: done! " << std::endl;
/*  
   std::cout<<"READING: Dumping the contents of the VxContainer "<<std::endl;
   std::cout<<"------------------------------VxVertex related "<<std::endl;
   std::cout<<" m_vxContainers size:      "<< p_coll->m_vxContainers.size()<<std::endl;
   std::cout<<" m_vxCandidates size:      "<< p_coll->m_vxCandidates.size() <<std::endl;
   std::cout<<" m_vxTrackAtVertices size: "<< p_coll->m_vxTrackAtVertices.size()<<std::endl;
   std::cout<<" m_recVertices size:       "<< p_coll->m_recVertices.size()<<std::endl;
   std::cout<<" m_vertices size:          "<< p_coll->m_vertices.size()<<std::endl;
 
   std::cout<<"------------------------------TrkTrack related "<<std::endl;
   std::cout<<" m_tracks size:            "<< p_coll->m_tracks.size()<<std::endl;
   std::cout<<"------------------------------TrkTrackParameters related "<<std::endl;
   std::cout<<" m_trackParameters size:   "<< p_coll->m_trackParameters.size()<<std::endl;
   std::cout<<" m_perigees size:          "<< p_coll->m_perigees.size()<<std::endl;
   std::cout<<" m_measPerigees size:      "<< p_coll->m_measPerigees.size()<<std::endl;
   std::cout<<" m_surfaces size:          "<< p_coll->m_surfaces.size()<<std::endl; 
   std::cout<<" m_fitQualities size:      "<< p_coll->m_fitQualities.size()<<std::endl;
   std::cout<<" m_hepSymMatrices  size:   "<< p_coll->m_hepSymMatrices.size()<<std::endl; 
   std::cout<<"*************************************** "<<std::endl;  
*/    

  }else if( compareClassGuid( p0_guid ) ){
//   std::cout << "VxContainerCnv::createTransient: use old converter" << std::endl;  
   p_collection = poolReadObject< VxContainer >();
   
  }else  throw std::runtime_error( "Unsupported persistent version of VxContainer" );
    
  return p_collection;
  
}//end of create transient method

void VxContainerCnv::updateLog(){  
     const DataObject* dObj = getDataObject();
     if (dObj==0) return; // Can't do much if this fails.
     const std::string  key = (dObj->name());
 
     m_log.m_source="VxContainerCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}


