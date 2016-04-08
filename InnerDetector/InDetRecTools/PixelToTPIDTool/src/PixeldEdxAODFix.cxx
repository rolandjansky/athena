/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelToTPIDTool/PixeldEdxAODFix.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"

namespace InDet
{
PixeldEdxAODFix::PixeldEdxAODFix(const std::string &name, ISvcLocator *pSvcLocator)
        : AthAlgorithm (name, pSvcLocator),
          m_summarytool("") {

  declareProperty("TrackParticleContainers",m_tpcontainernames);
  declareProperty("SummaryHelperTool",m_summarytool);
}

PixeldEdxAODFix::~PixeldEdxAODFix() {
}

StatusCode PixeldEdxAODFix::initialize() {
  msg(MSG::INFO) << "PixeldEdxAODFix::initialize()" << endreq;
  if ( m_summarytool.retrieve().isFailure() ) {
    msg(MSG:: FATAL) << m_summarytool.propertyName() << ": Failed to retrieve tool " << m_summarytool.type() << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << m_summarytool.propertyName() << ": Retrieved tool " << m_summarytool.type() << endreq;
  }
  return StatusCode::SUCCESS;

}

StatusCode PixeldEdxAODFix::execute() {

  for (int i=0;i<(int)m_tpcontainernames.size();i++){
    const Rec::TrackParticleContainer *tpcontainer=0;
    StatusCode sc=evtStore()->retrieve(tpcontainer,m_tpcontainernames[i]);
    if (sc.isFailure()){
      msg(MSG::WARNING) << "Could not retrieve " << m_tpcontainernames[i] << endreq;
      return sc;
    }

    for (Rec::TrackParticleContainer::const_iterator it = tpcontainer->begin(); it != tpcontainer->end(); it++) {
      const Rec::TrackParticle* particle = *it;
      Trk::TrackSummary *summ=const_cast<Trk::TrackSummary *>(particle->trackSummary());
      if (!summ->indetTrackSummary()) continue;
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
      typePattern.set(Trk::TrackStateOnSurface::Perigee); 
      const Trk::TrackStateOnSurface *pertsos=new Trk::TrackStateOnSurface(0,particle->perigee()->clone(),0,0,typePattern); 
      DataVector<const Trk::TrackStateOnSurface>*dv=new DataVector<const Trk::TrackStateOnSurface>;
      dv->push_back(pertsos);
      Trk::TrackInfo info(Trk::TrackInfo::GlobalChi2Fitter,Trk::pion);
   
      Trk::Track tmptrack(info,dv,0);
      m_summarytool->addDetailedTrackSummary(tmptrack,*summ);

    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PixeldEdxAODFix::finalize() {

  return StatusCode::SUCCESS;

}

}
