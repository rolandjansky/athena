#include "MMClusterOnTrackCreator.h"


#include "MuonRIO_OnTrack/MMClusterOnTrack.h"

Muon::MMClusterOnTrackCreator::MMClusterOnTrackCreator
  (const std::string& ty, const std::string& na, const IInterface* pa)
    : AthAlgTool(ty, na, pa),
      m_muonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
  {
    // algtool interface - necessary!
    declareInterface<IMuonClusterOnTrackCreator>(this);
    declareInterface<IRIO_OnTrackCreator>(this);

    // --- some job options here.
    declareProperty("MuonIdHelperTool",  m_muonIdHelperTool);
}


StatusCode Muon::MMClusterOnTrackCreator::initialize() {
    ATH_CHECK(m_muonIdHelperTool.retrieve());
    return StatusCode::SUCCESS;
}




const Muon::MuonClusterOnTrack* Muon::MMClusterOnTrackCreator::createRIO_OnTrack(const Trk::PrepRawData& RIO,
                                                    const Amg::Vector3D& GP) const {
    MuonClusterOnTrack* MClT = 0;

    //  check whether PrepRawData has detector element, if not there print warning
    const Trk::TrkDetElementBase* EL = RIO.detectorElement();
    if ( !EL ) {
      ATH_MSG_WARNING("RIO does not have associated detectorElement!, cannot produce ROT");
      return 0;
    }

    Trk::LocalParameters locpar(RIO.localPosition());

    Amg::Vector2D lp;
    double positionAlongStrip = 0;
    double positionAlongz = 0;

    if ( !EL->surface(RIO.identify()).globalToLocal(GP, GP, lp) ) {
      Amg::Vector3D lpos = RIO.detectorElement()->surface(RIO.identify()).transform().inverse()*GP;
      ATH_MSG_WARNING("Extrapolated GlobalPosition not on detector surface! Distance " << lpos.z());
      lp[Trk::locX]  = lpos.x();
      lp[Trk::locY]  = lpos.y();
      positionAlongz = lpos.z();
    }
    positionAlongStrip = lp[Trk::locY];
    Amg::MatrixX loce = RIO.localCovariance();


    if ( m_muonIdHelperTool->isMM(RIO.identify()) ) {
      // cast to MMPrepData
      const MMPrepData* MClus   = dynamic_cast<const MMPrepData*> (&RIO);
      if (!MClus) {
        ATH_MSG_WARNING("RIO not of type MMPrepData, cannot create ROT");
        return 0;
      }
      MClT = new MMClusterOnTrack(MClus, locpar, loce, positionAlongStrip);
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
