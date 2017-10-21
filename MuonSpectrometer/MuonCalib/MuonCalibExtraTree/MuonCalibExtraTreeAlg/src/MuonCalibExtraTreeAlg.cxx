/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibExtraTreeAlg.h"
#include "MuonCalibExtraTreeAlg/IExtraTreeFillerTool.h"
#include "MuonCalibExtraTreeAlg/ISegmentOnTrackSelector.h"

// Extra Tree
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

#include "AthContainers/DataVector.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonCalibNtuple/RootFileManager.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPattern.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
/*#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "TrkDetElementBase/TrkDetElementBase.h"

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/Perigee.h"
#include "TrkParameters/MeasuredPerigee.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"*/

#include "CxxUtils/sincos.h"


#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include <vector>

namespace MuonCalib{
  
MuonCalibExtraTreeAlg::MuonCalibExtraTreeAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_muonIdHelper(0),
  m_patterns(0),
  m_doPhi(false),
  m_ntupleName(""), m_patternLocation(""),m_delayFinish(false),
  m_idToFixedIdTool("MuonCalib::IdToFixedIdTool/MuonCalib_IdToFixedIdTool"),
  m_segmentOnTrackSelector(""),
  m_dir(0), m_tree(0),
  m_init(false) {
  m_ntupleName = "PatternNtupleMaker";
  declareProperty("doPhi",m_doPhi);
    
  declareProperty("NtupleName",m_ntupleName);
  declareProperty("PatternLocation",m_patternLocation);
//  Run TrackAnalysis in RTT 
  declareProperty("DelayFinish",m_delayFinish);
  //  Properties for TrackAnalysis passed
  //  flip hit truth radius of MDT drift radius 
  //  needed for truth matching in e.g. rel 12 RDO data sets 
  declareProperty("TrackFillerTools", m_track_fillers);
  declareProperty("IdToFixedIdTool", m_idToFixedIdTool);
  declareProperty("SegmentOnTrackSelector", m_segmentOnTrackSelector);
  m_detMgr=NULL;    
}  //end MuonCalibExtraTreeAlg::MuonCalibExtraTreeAlg
  
MuonCalibExtraTreeAlg::~MuonCalibExtraTreeAlg() {
}

StatusCode MuonCalibExtraTreeAlg::initialize() {
  StatusCode sc = detStore()->retrieve( m_detMgr );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(" Cannot retrieve MuonDetDescrMgr " << endmsg);
    return StatusCode::FAILURE;
  } 
  m_muonIdHelper = m_detMgr->mdtIdHelper();
    
  return StatusCode::SUCCESS;
}  // end MuonCalibExtraTreeAlg::initialize

StatusCode MuonCalibExtraTreeAlg::execute() {
  if(!m_init) {
    m_dir = RootFileManager::getInstance()->getDirectory( m_ntupleName.c_str() );
    m_dir->cd();
      
    //    m_tree = new TTree("Segments", "my first Tree");  
    ATH_MSG_DEBUG("Try the GetObject call");
    m_dir->GetObject("Segments",m_tree);
    ATH_MSG_DEBUG("retrieved tree " << m_tree);
    
    if(m_doPhi) createPhiHitBranch(m_tree);
    if(m_track_fillers.size()) {
      createHitBranch(m_tree);
      createTrackBranch(m_tree);
    }
    for (ToolHandleArray<IExtraTreeFillerTool>::iterator it=m_track_fillers.begin(); it!=m_track_fillers.end(); it++) {
      if(!it->retrieve().isSuccess()) {
	ATH_MSG_FATAL("Failed to retrieve "<<it->name());
	return StatusCode::FAILURE;
      }
      if(!m_segmentOnTrackSelector.empty()) {
	if(!m_segmentOnTrackSelector.retrieve().isSuccess()) {
	  ATH_MSG_FATAL("Failed to retieve SegmentOnTrackSelector!");
	  return StatusCode::FAILURE;
	}
      }
      (*it)->SetBranches(&m_trackBranch, &m_hitBranch, &m_trackSegmentBranch, m_segmentOnTrackSelector);
    }
    m_init = true;
  }

  // Reset branches
  if(m_doPhi) m_phiHitBranch.reset();
  if(m_track_fillers.size()) {
    m_hitBranch.reset();
    m_trackBranch.reset();
    m_trackSegmentBranch.reset();
    if(!m_segmentOnTrackSelector.empty() && !m_segmentOnTrackSelector->beginEvent().isSuccess()) {
      ATH_MSG_FATAL("SegmentOnTrackSelecor failed!");
      return StatusCode::FAILURE;
    }
  }
   
  if(m_doPhi) {
    retrievePatterns();
    handlePatterns();
  }
  unsigned int index = 0;
  for (ToolHandleArray<IExtraTreeFillerTool>::iterator it=m_track_fillers.begin(); it!=m_track_fillers.end(); it++) {
    if(!(*it)->writeTracks(index).isSuccess()) {
      return StatusCode::FAILURE;
    }
  }
  finishEvent();
  return  StatusCode::SUCCESS; 
}  // end MuonCalibExtraTreeAlg::execute

StatusCode MuonCalibExtraTreeAlg::finalize() {
  if(m_doPhi) m_phiHitBranch.reset();
  if(m_track_fillers.size()) {
    m_hitBranch.reset();
    m_trackBranch.reset();
  }

//    m_tree->Write();      
  return StatusCode::SUCCESS;
}  // end MuonCalibExtraTreeAlg::finalize

bool MuonCalibExtraTreeAlg::retrievePatterns(){
  if(m_patternLocation.size()==0) return true;
  StatusCode  sc = evtStore()->retrieve(m_patterns,m_patternLocation);
  if( !sc.isSuccess() || 0 == m_patterns)
    ATH_MSG_WARNING("Could not find Pattern Combination  " << m_patternLocation);
  else
    ATH_MSG_DEBUG("Found Pattern Combination "<< m_patternLocation << " with size " << m_patterns->size());
  return true;
}  //end MuonCalibExtraTreeAlg::retrievePatterns
  
bool MuonCalibExtraTreeAlg::createPhiHitBranch(TTree* tree) {
  bool created = false; 
  if(tree) created = m_phiHitBranch.createBranch( tree );
  return created;
}  // end MuonCalibExtraTreeAlg::createPhiHitBranch
  
bool MuonCalibExtraTreeAlg::createHitBranch(TTree* tree) {
  bool created = false; 
  if(tree) created = m_hitBranch.createBranch( tree );
  return created;
}  // end MuonCalibExtraTreeAlg::createHitBranch 

bool MuonCalibExtraTreeAlg::createTrackBranch(TTree* tree) {
  bool created = false; 
  if(tree) created = m_trackBranch.createBranch( tree );
  if(tree) created &= m_trackSegmentBranch.createBranch( tree );
  return created;
}  // end MuonCalibExtraTreeAlg::createTrackBranch

bool MuonCalibExtraTreeAlg::handlePatterns() {
  //loop over pattern combination
  if( !m_patterns ) return false; 

//    bool m_debug = false;
  MuonPatternCombinationCollection::const_iterator it = m_patterns->begin();
  MuonPatternCombinationCollection::const_iterator it_end = m_patterns->end();
  unsigned int index = 0;
  for ( ; it!=it_end; ++it) {
    const std::vector< Muon::MuonPatternChamberIntersect > mpcivec = (*it)->chamberData();
    std::vector< Muon::MuonPatternChamberIntersect >::const_iterator pat_it = mpcivec.begin();
//     if (m_debug) std::cout << " Pattern nr " << index << std::endl;
//     int nhit = -1;
    for (;pat_it!=mpcivec.end();++pat_it) {
      const std::vector< const Trk::PrepRawData* > prdvec = (*pat_it).prepRawDataVec();
      std::vector< const Trk::PrepRawData* >::const_iterator prd_it = prdvec.begin();
      for( ; prd_it!= prdvec.end() ;++prd_it ) {

//    for( unsigned int index = 0; index < m_patterns->size()  ; ++index){
//      const Muon::MuonPattern* pattern = (*m_patterns)[index] ;
//      for( unsigned int hitNr=0; hitNr < pattern->numberOfContainedPrds(); ++hitNr ){
//      const Trk::PrepRawData* hit = pattern->prd(hitNr);
//        nhit++;
//        if(m_debug) std::cout << " Pattern hit with PrepData " << nhit << std::endl;
	const Trk::PrepRawData* hit = *prd_it;
	MuonFixedId fId = m_idToFixedIdTool->idToFixedId( hit->identify() );

	//is this hit a phi hit??
// 	if( fId.is_tgc() ) {
// 	  if( fId.tgcIsStrip() != 1 ) continue ;
// 	} else if ( fId.is_rpc() ) {
// 	  if( fId.rpcMeasuresPhi() != 1) continue ;
// 	} else if( fId.is_csc() ) {
// 	  if( fId.cscMeasuresPhi() != 1) continue ;
// 	}
	const Trk::TrkDetElementBase* detEl = hit->detectorElement() ;
	Amg::Vector3D gPos = detEl->center( hit->identify() );
	double error = std::sqrt(hit->localCovariance()(0,0));
//        double error = hit->localErrorMatrix().error(Trk::loc1); 
//        double errorY = hit->localErrorMatrix().error(Trk::locY); 
	  
	ATH_MSG_DEBUG("writing phiHit with index " << index << " error " << error);
	const MuonCalibHit_E phiHit( fId, gPos , 0., error);
	//write hit to phiHitBranch.
	m_phiHitBranch.fillBranch( phiHit, index );
      } // end prep data
    } // end Chamber data
    index++;
  } // end pattern combination
    
  return true;
}  //end MuonCalibExtraTreeAlg::handlePatterns

void MuonCalibExtraTreeAlg::finishEvent() {
  if (!m_delayFinish) {
    ATH_MSG_DEBUG("in finishEvent, filling tree " << m_tree);
    m_tree->Fill();
    ATH_MSG_DEBUG("tree Filled ");
  }
//    if(m_doPhi) m_phiHitBranch.reset();
//    if(m_doTracks) {
//      m_hitBranch.reset();
//      m_trackBranch.reset();
//    }
  ATH_MSG_DEBUG("finish eventready... ");
}  //end MuonCalibExtraTreeAlg::finishEvent

}//namespace MuonCalib
