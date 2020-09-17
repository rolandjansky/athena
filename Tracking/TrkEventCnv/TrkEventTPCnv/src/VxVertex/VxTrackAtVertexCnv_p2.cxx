/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertexCnv_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrackLink/ITrackLink.h"

#include "TrkParameters/TrackParameters.h"

static const FitQualityCnv_p1 fitQualityCnv;

void VxTrackAtVertexCnv_p2::persToTrans (const Trk::VxTrackAtVertex_p2 * persObj, Trk::VxTrackAtVertex * transObj, MsgStream & log) {
      // std::cout<<"ILIJA read VxTrackAtVertexCnv_p2 "<<std::endl;

  std::unique_ptr<Trk::ITrackLink> trackLink;
  if (persObj->m_typeOfLink == 0) {
    // m_elementLinkConverterForTrack.resetForCnv(persObj->m_origTrackNames); // for element link change - Gerhard !?
    Trk::LinkToTrack* tmpLink = new Trk::LinkToTrack;
    trackLink  = std::unique_ptr<Trk::ITrackLink>(tmpLink);
    m_elementLinkConverterForTrack.persToTrans(&persObj->m_origTrack,tmpLink,log);
  } else if (persObj->m_typeOfLink == 1) {
    // m_elementLinkConverterForTrackParticle.resetForCnv(persObj->m_origTrackNames);
    Trk::LinkToTrackParticleBase * tmpLink = new Trk::LinkToTrackParticleBase;
    trackLink  = std::unique_ptr<Trk::ITrackLink>(tmpLink);
    m_elementLinkConverterForTrackParticle.persToTrans (  &persObj->m_origTrack, tmpLink, log);
  }

 Trk::FitQuality fitq;
 fitQualityCnv.persToTrans(&persObj->m_fitQuality,  &fitq, log);

 Trk::TrackParameters* perigeeAtVertex = createTransFromPStore(&m_paramsCnv, persObj->m_perigeeAtVertex,log );

 *transObj = Trk::VxTrackAtVertex(fitq.chiSquared(),
                                  perigeeAtVertex,
                                  static_cast<Trk::NeutralParameters*>(nullptr),
                                  fitq.doubleNumberDoF());
 transObj->setWeight (persObj->m_trkWeight);
 transObj->setVtxCompatibility (persObj->m_VertexCompatibility);
 transObj->setOrigTrackNoCache (trackLink.release());
}

void VxTrackAtVertexCnv_p2::transToPers (const Trk::VxTrackAtVertex * transObj, Trk::VxTrackAtVertex_p2 * persObj, MsgStream & log){
    
    // std::cout<<"ILIJA write VxTrackAtVertexCnv_p2 "<<std::endl;
    
  // persObj->m_fitQuality = toPersistent (&m_fqCnv, &(transObj->m_fitQuality), log);
  
  const Trk::FitQuality fq = transObj->trackQuality();
  fitQualityCnv.transToPers(&fq,  &persObj->m_fitQuality, log);
	
  persObj->m_trkWeight  = transObj->weight();
  persObj->m_VertexCompatibility = transObj->vtxCompatibility();

  // ITPConverterFor < Trk::ParametersBase > *paramsCnv = 0;
  // persObj->m_perigeeAtVertex = toPersistent (&paramsCnv, transObj->m_perigeeAtVertex, log);
  persObj->m_perigeeAtVertex = toPersistent (&m_paramsCnv, transObj->perigeeAtVertex(), log);

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
  // log << "transToPers() Type of link is: " << persObj->m_typeOfLink << endmsg;
//  log << "link saved at: " << persObj->m_origTrackNames.m_names.front() 
//                           << "\tcontIndex: " << persObj->m_origTrack.m_contIndex 
//                           << "\telIndex : " << persObj->m_origTrack.m_elementIndex << endmsg; 
}
