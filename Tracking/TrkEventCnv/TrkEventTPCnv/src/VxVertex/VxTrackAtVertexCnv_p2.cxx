/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "VxVertex/VxTrackAtVertex.h"
#undef private
#undef protected
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertexCnv_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrackLink/ITrackLink.h"

#include "TrkParameters/TrackParameters.h"

static FitQualityCnv_p1 m_fitQualityCnv;

void VxTrackAtVertexCnv_p2::persToTrans (const Trk::VxTrackAtVertex_p2 * persObj, Trk::VxTrackAtVertex * transObj, MsgStream & log) {
      // std::cout<<"ILIJA read VxTrackAtVertexCnv_p2 "<<std::endl;
  if (persObj->m_typeOfLink == 0) {
    // m_elementLinkConverterForTrack.resetForCnv(persObj->m_origTrackNames); // for element link change - Gerhard !?
    Trk::LinkToTrack* tmpLink = new Trk::LinkToTrack;
    transObj->m_trackOrParticleLink  = tmpLink;
    m_elementLinkConverterForTrack.persToTrans(&persObj->m_origTrack,tmpLink,log);
  } else if (persObj->m_typeOfLink == 1) {
    // m_elementLinkConverterForTrackParticle.resetForCnv(persObj->m_origTrackNames);
    Trk::LinkToTrackParticleBase * tmpLink = new Trk::LinkToTrackParticleBase;
    transObj->m_trackOrParticleLink = tmpLink;
    m_elementLinkConverterForTrackParticle.persToTrans (  &persObj->m_origTrack, tmpLink, log);
  }
  // log << "P2 persToTrans() Type of link is: " << persObj->m_typeOfLink << endreq;
  // log << "link saved at: " << persObj->m_origTrackNames.m_names.front() 
  //                          << "\tcontIndex: " << persObj->m_origTrack.m_contIndex 
  //                          << "\telIndex : " << persObj->m_origTrack.m_elementIndex << endreq; 

 
 // fillTransFromPStore( &m_fqCnv, persObj->m_fitQuality, &transObj->m_fitQuality, log );
 
 m_fitQualityCnv.persToTrans(&persObj->m_fitQuality,  &transObj->m_fitQuality, log);
 
// transObj->m_fitQuality = *(createTransFromPStore( &m_fqCnv, persObj->m_fitQuality, log ));
 // Trk::FitQuality* qtmp = createTransFromPStore( &m_fqCnv, persObj->m_fitQuality, log);
 // if (qtmp){
 //   transObj->m_fitQuality  = *qtmp;
 //   delete qtmp;
 // } else 
 //    transObj->m_fitQuality = Trk::FitQuality();


 transObj->m_trkWeight	          = persObj->m_trkWeight; 
 transObj->m_VertexCompatibility  = persObj->m_VertexCompatibility;
 
 // ITPConverterFor<Trk::ParametersBase> * paramsCnv = 0;
 // transObj->m_perigeeAtVertex      = dynamic_cast<Trk::TrackParameters*>(createTransFromPStore(&paramsCnv, persObj->m_perigeeAtVertex,log ));
 transObj->m_perigeeAtVertex      = createTransFromPStore(&m_paramsCnv, persObj->m_perigeeAtVertex,log );
 
 // std::cout<<transObj->m_fitQuality<<std::endl;
  
}

void VxTrackAtVertexCnv_p2::transToPers (const Trk::VxTrackAtVertex * transObj, Trk::VxTrackAtVertex_p2 * persObj, MsgStream & log){
    
    // std::cout<<"ILIJA write VxTrackAtVertexCnv_p2 "<<std::endl;
    
  // persObj->m_fitQuality = toPersistent (&m_fqCnv, &(transObj->m_fitQuality), log);
  
	m_fitQualityCnv.transToPers(&transObj->m_fitQuality,  &persObj->m_fitQuality, log);
	
  persObj->m_trkWeight  = transObj->m_trkWeight;
  persObj->m_VertexCompatibility = transObj->m_VertexCompatibility;

  // ITPConverterFor < Trk::ParametersBase > *paramsCnv = 0;
  // persObj->m_perigeeAtVertex = toPersistent (&paramsCnv, transObj->m_perigeeAtVertex, log);
  persObj->m_perigeeAtVertex = toPersistent (&m_paramsCnv, transObj->m_perigeeAtVertex, log);

  Trk::LinkToTrack * trLink = dynamic_cast < Trk::LinkToTrack * >(const_cast <Trk::ITrackLink *>(transObj->trackOrParticleLink ()));
  if (trLink != 0) {
    persObj->m_typeOfLink = 0;
    // m_elementLinkConverterForTrack.resetForCnv(persObj->m_origTrackNames);
    ElementLink < TrackCollection > *el = dynamic_cast < ElementLink < TrackCollection > *>(trLink);
	m_elementLinkConverterForTrack.transToPers (el, &persObj->m_origTrack, log);
  } else {
	Trk::LinkToTrackParticleBase * trPBLink = dynamic_cast < Trk::LinkToTrackParticleBase * >(const_cast <Trk::ITrackLink *>(transObj->trackOrParticleLink()));
    if (trPBLink != 0) {
      persObj->m_typeOfLink = 1;
      // m_elementLinkConverterForTrackParticle.resetForCnv(persObj->m_origTrackNames);
      ElementLink < Trk::TrackParticleBaseCollection > *el = dynamic_cast < ElementLink < Trk::TrackParticleBaseCollection > *>(trPBLink);
      m_elementLinkConverterForTrackParticle.transToPers (el, &persObj->m_origTrack, log);
    }
  }
  // log << "transToPers() Type of link is: " << persObj->m_typeOfLink << endreq;
//  log << "link saved at: " << persObj->m_origTrackNames.m_names.front() 
//                           << "\tcontIndex: " << persObj->m_origTrack.m_contIndex 
//                           << "\telIndex : " << persObj->m_origTrack.m_elementIndex << endreq; 
}
