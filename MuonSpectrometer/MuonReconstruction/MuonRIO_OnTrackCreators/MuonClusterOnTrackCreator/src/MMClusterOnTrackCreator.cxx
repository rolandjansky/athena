/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "MMClusterOnTrackCreator.h"


#include "MuonRIO_OnTrack/MMClusterOnTrack.h"

Muon::MMClusterOnTrackCreator::MMClusterOnTrackCreator
  (const std::string& ty, const std::string& na, const IInterface* pa)
    : AthAlgTool(ty, na, pa) {

    // algtool interface - necessary!
    declareInterface<IMuonClusterOnTrackCreator>(this);
    declareInterface<IRIO_OnTrackCreator>(this);
}


StatusCode Muon::MMClusterOnTrackCreator::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
}




const Muon::MuonClusterOnTrack* Muon::MMClusterOnTrackCreator::createRIO_OnTrack(const Trk::PrepRawData& RIO,
                                                    const Amg::Vector3D& GP) const {
    MuonClusterOnTrack* MClT = nullptr;

    // check whether PrepRawData has detector element, if not there print warning
    const Trk::TrkDetElementBase* EL = RIO.detectorElement();
    if ( !EL ) {
      ATH_MSG_WARNING("RIO does not have associated detectorElement!, cannot produce ROT");
      return nullptr;
    }

    // MuClusterOnTrack production
    //
    // in RIO_OnTrack the local param and cov should have the same dimension
    Trk::LocalParameters locpar(RIO.localPosition());

    if (RIO.localCovariance().cols() != RIO.localCovariance().rows()) {
      ATH_MSG_FATAL("Rows and colums not equal!");
    }

    if (RIO.localCovariance().cols() > 1) {
      ATH_MSG_VERBOSE("Making 2-dim local parameters: " << m_idHelperSvc->toString(RIO.identify()));
    } else {
      Trk::DefinedParameter  radiusPar(RIO.localPosition().x(), Trk::locX);
      locpar = Trk::LocalParameters(radiusPar);
      ATH_MSG_VERBOSE("Making 1-dim local parameters: "  << m_idHelperSvc->toString(RIO.identify()));
    }

    Amg::Vector2D lp;
    double positionAlongStrip = 0;

    if ( !EL->surface(RIO.identify()).globalToLocal(GP, GP, lp) ) {
      Amg::Vector3D lpos = RIO.detectorElement()->surface(RIO.identify()).transform().inverse()*GP;
      ATH_MSG_WARNING("Extrapolated GlobalPosition not on detector surface! Distance " << lpos.z());
      lp[Trk::locX]  = lpos.x();
      lp[Trk::locY]  = lpos.y();
    }
    positionAlongStrip = lp[Trk::locY];

    Amg::MatrixX loce = RIO.localCovariance();
    ATH_MSG_DEBUG("All: new err matrix is " << loce);




    if ( m_idHelperSvc->isMM(RIO.identify()) ) {
      // cast to MMPrepData
      const MMPrepData* MClus   = dynamic_cast<const MMPrepData*> (&RIO);
      if (!MClus) {
        ATH_MSG_WARNING("RIO not of type MMPrepData, cannot create ROT");
        return nullptr;
      }
      ATH_MSG_VERBOSE("generating MMClusterOnTrack in MMClusterBuilder");
      MClT = new MMClusterOnTrack(MClus, locpar, loce, positionAlongStrip);
     } else {
      ATH_MSG_WARNING("MMClusterOnTrackCreator called with an non MM identifier");
    }

    return MClT;
}



const Muon::MuonClusterOnTrack* Muon::MMClusterOnTrackCreator::createRIO_OnTrack(const Trk::PrepRawData& RIO,
                                                    const Amg::Vector3D& GP,
                                                    const Amg::Vector3D&) const {
    return createRIO_OnTrack(RIO, GP);
}



const Muon::MuonClusterOnTrack* Muon::MMClusterOnTrackCreator::correct(const Trk::PrepRawData& RIO, const Trk::TrackParameters& TP) const {
    return createRIO_OnTrack(RIO, TP.position());
}
