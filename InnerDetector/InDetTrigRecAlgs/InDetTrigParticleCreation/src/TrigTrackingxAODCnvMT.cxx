/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTrackingxAODCnvMT.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <cmath>
#include <algorithm>


namespace InDet
{


  TrigTrackingxAODCnvMT::TrigTrackingxAODCnvMT(const std::string &name,
					   ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm (name, pSvcLocator),
      m_particleCreatorTool("Trk::ParticleCreatorTool"),
      m_trackKey(""),
      m_trackParticleKey("")
  {
    declareProperty("ParticleCreatorTool", m_particleCreatorTool);
    declareProperty("TrackParticlesName", 
                  m_trackParticleKey = std::string("TrackParticles"),
                  "TrackParticle container");
    declareProperty("TrackName", 
                  m_trackKey = std::string("Tracks"),
                  "Track Collection");
  }

  TrigTrackingxAODCnvMT::~TrigTrackingxAODCnvMT()
  {}

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  StatusCode TrigTrackingxAODCnvMT::initialize() {

    ATH_CHECK(m_particleCreatorTool.retrieve());

    ATH_CHECK( m_trackKey.initialize() );
    ATH_CHECK( m_trackParticleKey.initialize() );

    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Execute HLT Algorithm
  ///////////////////////////////////////////////////////////////////
  StatusCode TrigTrackingxAODCnvMT::execute(const EventContext& ctx) const {


    auto tpHandle = SG::makeHandle (m_trackParticleKey, ctx);
    
    ATH_CHECK( tpHandle.record (std::make_unique<xAOD::TrackParticleContainer>(),
                           std::make_unique<xAOD::TrackParticleAuxContainer>()) );
    xAOD::TrackParticleContainer* tpCont=tpHandle.ptr();

    auto tracks = SG::makeHandle(m_trackKey, ctx);

    ATH_MSG_VERBOSE(" Input track collection has size " << tracks->size());
    if ( tracks->size() == 0 ) {
      ATH_MSG_DEBUG(" Input track collection has 0 size. Algorithm not executed!");
      return StatusCode::SUCCESS;
    }
    
    //convert tracks
    tpCont->reserve(tracks->size());                                                     
    for(unsigned int idtr=0; idtr< tracks->size(); ++idtr) {
      const ElementLink<TrackCollection> trackLink(*tracks, idtr);

      xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, tpCont);

      if(msgLvl(MSG::DEBUG) && (tp != 0)){
        int npix, nsct, ntrt, npixh, nscth;
        npix = nsct = ntrt = npixh = nscth = -1;
        const Trk::Track *tr = tp->track();
        if (tr){
          const Trk::TrackSummary *ts = tr->trackSummary();
          if (ts){
            npix = ts->get(Trk::numberOfPixelHits);
            nsct = ts->get(Trk::numberOfSCTHits);
            ntrt = ts->get(Trk::numberOfTRTHits);
            nscth= ts->get(Trk::numberOfSCTHoles);
            npixh= ts->get(Trk::numberOfPixelHoles);

          }
        }

        msg() << MSG::DEBUG << "REGTEST: " << std::setw(5) << idtr
          << "  pT:  " << std::setw(10) << tp->pt()
          << "  eta: " << tp->eta()
          << "  phi: " << tp->phi()
          << "  d0:  " << tp->d0()
          << "  z0:  " << tp->z0()
          << "\t" << npix << "/" << nsct << "/" << ntrt << "//" << npixh << "/" << nscth
          << endmsg;

      }
    }


    ATH_MSG_DEBUG("REGTEST container size = " << tpCont->size());
    return StatusCode::SUCCESS;
  }
} // end namespace


