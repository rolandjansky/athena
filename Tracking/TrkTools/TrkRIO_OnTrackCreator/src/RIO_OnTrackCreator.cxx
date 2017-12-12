/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RIO_OnTrackCreator.cxx
//   AlgTool for adapting a RIO to a track candidate. It
//   automatically selects the corresponding subdet correction.
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 2004 - now: Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
///////////////////////////////////////////////////////////////////

// --- the base class
#include "TrkRIO_OnTrackCreator/RIO_OnTrackCreator.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
// --- Gaudi stuff
#include "GaudiKernel/ListItem.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"

/** il costruttore
 */
Trk::RIO_OnTrackCreator::RIO_OnTrackCreator(const std::string& t,
			      const std::string& n,
			      const IInterface* p)
  :  AthAlgTool(t,n,p),
     m_idHelper(nullptr),
     m_PixClusCor        ("InDet::PixelClusterOnTrackTool/PixelClusterOnTrackTool"),
     m_SctClusCor        ("InDet::SCT_ClusterOnTrackTool/SCT_ClusterOnTrackTool"),
     m_TRT_Cor           ("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"),
     m_MuonDriftCircleCor("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackTool"),
     m_MuonClusterCor    ("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackTool"),
     m_doPixel(true),
     m_doSCT(true),
     m_doTRT(true)
 {
   m_mode     = "all";
   m_nwarning = new int(0);
   declareInterface<IRIO_OnTrackCreator>(this);
   declareProperty("ToolPixelCluster"   ,m_PixClusCor);
   declareProperty("ToolSCT_Cluster"    ,m_SctClusCor);
   declareProperty("ToolTRT_DriftCircle",m_TRT_Cor);
   declareProperty("ToolMuonDriftCircle",m_MuonDriftCircleCor);
   declareProperty("ToolMuonCluster"    ,m_MuonClusterCor);
   declareProperty("Mode"               ,m_mode);
 }

// destructor
Trk::RIO_OnTrackCreator::~RIO_OnTrackCreator()
{
  delete m_nwarning;
} 

// initialise
StatusCode Trk::RIO_OnTrackCreator::initialize()
{
  if (AlgTool::initialize().isFailure()) return StatusCode::FAILURE; 

  if (m_mode != "all" && m_mode != "indet" &&m_mode != "muon") {
    msg(MSG::FATAL) << "Mode is set to wrong value " << m_mode << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << " RIO_OnTrackCreator job configuration:" << std::endl
		 << std::endl << " (i) The following RIO correction "
		 << "tools configured (depends on mode = "<< m_mode <<"):" << std::endl
		 << "     Pixel      : " << m_PixClusCor << std::endl
		 << "     SCT        : " << m_SctClusCor << std::endl
		 << "     TRT        : " << m_TRT_Cor       << std::endl
		 << "     MDT        : " << m_MuonDriftCircleCor << std::endl
		 << "     CSC/RPC/TGC: " << m_MuonClusterCor << std::endl
		 << endmsg;

  // Get the correction tool to create Pixel/SCT/TRT RIO_onTrack

  if (m_mode == "all" || m_mode == "indet") {

    if (!m_PixClusCor.empty()) {
      if ( m_PixClusCor.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_PixClusCor << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::INFO) << "Retrieved tool " << m_PixClusCor << endmsg;
      }
    } else {
      m_doPixel = false;
    }

    if (!m_SctClusCor.empty()) {
      if ( m_SctClusCor.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_SctClusCor << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::INFO) << "Retrieved tool " << m_SctClusCor << endmsg;
      }
    } else {
      m_doSCT = false;
    }

    if (!m_TRT_Cor.empty()) {
      if ( m_TRT_Cor.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_TRT_Cor << endmsg;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::INFO) << "Retrieved tool " << m_TRT_Cor << endmsg;
      }
    } else {
      m_doTRT = false;
    }
  }
  else{
    m_TRT_Cor.disable();
    m_PixClusCor.disable();
    m_SctClusCor.disable();
  }

  
  if (m_mode == "all" || m_mode == "muon") {

    if ( m_MuonDriftCircleCor.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_MuonDriftCircleCor << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_MuonDriftCircleCor << endmsg;
    }

    if ( m_MuonClusterCor.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_MuonClusterCor << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_MuonClusterCor << endmsg;
    }
  }
  else{
    m_MuonClusterCor.disable();
    m_MuonDriftCircleCor.disable();
  }
  
  // Set up ATLAS ID helper to be able to identify the RIO's det-subsystem.
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO)  << "initialize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}

// finalise
StatusCode Trk::RIO_OnTrackCreator::finalize()
{
  msg(MSG::INFO)  << "finalize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}

// The sub-detector brancher algorithm
const Trk::RIO_OnTrack* 
Trk::RIO_OnTrackCreator::correct(const Trk::PrepRawData& rio,
                                 const TrackParameters& trk) const
{
  /* How to select the sub-detector type?
     --> a) via detector identifier 
     --> b) via dynamic cast. This is not acceptable any more since it creates
            dependencies to the cast-class (InDet, Muons...)
  */
  Identifier id;
  //  if (rio) {
  id = rio.identify();

  // --- print RIO
  ATH_MSG_VERBOSE ("RIO ID prints as "<<m_idHelper->print_to_string(id));
  ATH_MSG_VERBOSE ("RIO.locP = ("<<rio.localPosition().x()<<","<<rio.localPosition().y()<<")");

  if (m_doPixel && m_idHelper->is_pixel(id)) {
    if (m_mode == "muon") {
      msg(MSG::WARNING)<<"I have no tool to correct the current Pixel hit! - Giving back nil."<<endmsg;
      return 0;
    } else {
      ATH_MSG_DEBUG ("RIO identified as PixelCluster.");
      return m_PixClusCor->correct(rio, trk);
    }
  }

  if (m_doSCT && m_idHelper->is_sct(id)) {
    if (m_mode == "muon") {
      msg(MSG::WARNING)<<"I have no tool to correct the current SCT hit! - Giving back nil."<<endmsg;
      return 0;
    } else {
      ATH_MSG_DEBUG ("RIO identified as SCT_Cluster.");
      return m_SctClusCor->correct(rio, trk);
    }
  }

  if (m_doTRT && m_idHelper->is_trt(id)) {
    if (m_mode == "muon") {
      msg(MSG::WARNING)<<"I have no tool to correct this TRT driftcircle! - Giving back nil."<<endmsg;
      return 0;
    } else {
      ATH_MSG_DEBUG ("RIO identified as TRT_DriftCircle.");
      return m_TRT_Cor->correct(rio, trk);
    }
  }

  if (m_idHelper->is_mdt(id)){
    if (m_mode == "indet") {
      msg(MSG::WARNING)<<"I have no tool to correct this MDT driftcircle! - Giving back nil."<<endmsg;
      return 0;
    } else {
      ATH_MSG_DEBUG ("RIO identified as MuonDriftCircle.");
      return m_MuonDriftCircleCor->correct(rio, trk);
    }
  }
  if ( (m_idHelper->is_csc(id)) || (m_idHelper->is_rpc(id)) 
       || (m_idHelper->is_tgc(id)) ) {
    if (m_mode == "indet") {
      msg(MSG::WARNING)<<"I have no tool to correct a CSC/RPC/TGC hit! - Giving back nil."<<endmsg;
      return 0;
    } else {
      ATH_MSG_DEBUG ("RIO identified as MuonCluster.");
      return m_MuonClusterCor->correct(rio, trk);
    }
  }

  int ROTCreator_maxwarn = 50;
  if (*m_nwarning < 10*ROTCreator_maxwarn) (*m_nwarning)++;
  if (*m_nwarning <= ROTCreator_maxwarn) {
    msg(MSG::ERROR) << "idHelper could not identify sub-detector! Return nil RIO_OnTrack*"<<endmsg;
    if ((*m_nwarning)==ROTCreator_maxwarn) {
      msg(MSG::ERROR) << " --> will skip this warning in the future..."<<endmsg;
    }
  }

  return 0;
}
