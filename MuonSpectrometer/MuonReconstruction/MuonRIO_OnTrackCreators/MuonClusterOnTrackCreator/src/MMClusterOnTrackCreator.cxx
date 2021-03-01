/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "MMClusterOnTrackCreator.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "NSWCalibTools/INSWCalibTool.h"


#include "MuonRIO_OnTrack/MMClusterOnTrack.h"

Muon::MMClusterOnTrackCreator::MMClusterOnTrackCreator
  (const std::string& ty, const std::string& na, const IInterface* pa)
    : AthAlgTool(ty, na, pa),
      m_muonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_clusterBuilderTool("Muon::SimpleMMClusterBuilderTool/SimpleMMClusterBuilderTool",this),
      m_calibTool("Muon::NSWCalibTool/NSWCalibTool", this)      
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
    MuonClusterOnTrack* MClT = nullptr;

    if ( !m_muonIdHelperTool->isMM(RIO.identify()) ) {
      ATH_MSG_ERROR("MMClusterOnTrackCreator called with an non MM identifier");
      return MClT;
    }
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
      ATH_MSG_VERBOSE("Making 2-dim local parameters: " << m_muonIdHelperTool->toString(RIO.identify()));
    } else {
      Trk::DefinedParameter  radiusPar(RIO.localPosition().x(), Trk::locX);
      locpar = Trk::LocalParameters(radiusPar);
      ATH_MSG_VERBOSE("Making 1-dim local parameters: "  << m_muonIdHelperTool->toString(RIO.identify()));
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

    // cast to MMPrepData
    const MMPrepData* MClus   = dynamic_cast<const MMPrepData*> (&RIO);
    ATH_MSG_VERBOSE("generating MMClusterOnTrack in MMClusterBuilder");
    MClT = new MMClusterOnTrack(MClus, locpar, loce, positionAlongStrip);

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

const Muon::MuonClusterOnTrack* Muon::MMClusterOnTrackCreator::calibratedCluster(const Trk::PrepRawData& RIO, const Amg::Vector3D& GP) const 
{
  MuonClusterOnTrack* cluster = nullptr;
    
  if ( !m_muonIdHelperTool->isMM(RIO.identify()) ) {
    ATH_MSG_ERROR("MMClusterOnTrackCreator called with an non MM identifier");
    return cluster;
  }
  
  // check whether PrepRawData has detector element, if not there print warning
  const Trk::TrkDetElementBase* EL = RIO.detectorElement();
  if ( !EL ) {
    ATH_MSG_WARNING("RIO does not have associated detectorElement!, cannot produce ROT");
    return nullptr;
  }
  
  // MuClusterOnTrack production
  //
  Trk::LocalParameters locpar(RIO.localPosition());
  
  if (RIO.localCovariance().cols() != RIO.localCovariance().rows()) {
    ATH_MSG_FATAL("Rows and colums not equal!");
  }

  if (RIO.localCovariance().cols() > 1) {
    ATH_MSG_VERBOSE("Making 2-dim local parameters: " << m_muonIdHelperTool->toString(RIO.identify()));
  } else {
    Trk::DefinedParameter  radiusPar(RIO.localPosition().x(), Trk::locX);
    locpar = Trk::LocalParameters(radiusPar);
    ATH_MSG_VERBOSE("Making 1-dim local parameters: "  << m_muonIdHelperTool->toString(RIO.identify()));
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

  /// correct the local position based on the stereo angle                                                                                        
  if ( m_muonIdHelperTool->mmIdHelper().isStereo(RIO.identify())) {
    const MuonGM::MMReadoutElement* mmEL = dynamic_cast<const MuonGM::MMReadoutElement*>(EL);
    double sAngle = mmEL->getDesign(RIO.identify())->sAngle;
    locpar[Trk::locX] = locpar[Trk::locX]+positionAlongStrip*tan(sAngle);
  }

  /// calibrate the input   
  const MMPrepData* MClus = dynamic_cast<const MMPrepData*> (&RIO);
  std::vector<NSWCalib::CalibratedStrip> calibratedStrips; 
  StatusCode sc = m_calibTool->calibrateClus(MClus,GP,calibratedStrips);
  if ( sc != StatusCode::SUCCESS ) {
    ATH_MSG_WARNING("Could not calibrate the MM Cluster in the RIO on track creator");
    return cluster;
  } 
  Amg::Vector2D localposition;
  Amg::MatrixX loce = RIO.localCovariance();
  localposition[Trk::locX] = locpar[Trk::locX];
  localposition[Trk::locY] = 0.0;

  /// calibrate the cluster position along the precision coordinate
  sc = m_clusterBuilderTool->getCalibratedClusterPosition(MClus,calibratedStrips,localposition,loce);
  if ( sc != StatusCode::SUCCESS ) {
    ATH_MSG_WARNING("Could not calibrate the MM Cluster in the RIO on track creator");
    return cluster;
  } 
  /// set the value of the local parameter after the calibration
  locpar[Trk::locX] = localposition[Trk::locX];

  ATH_MSG_VERBOSE("generating MMClusterOnTrack in MMClusterBuilder");
  cluster = new MMClusterOnTrack(MClus, locpar, loce, positionAlongStrip);


  return cluster;
}
