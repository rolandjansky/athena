/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibExtraTreeAlg.h"

#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "AthContainers/DataVector.h"
#include "MuonCalibNtuple/RootFileManager.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPattern.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "CxxUtils/sincos.h"

#include "TTree.h"
#include "TDirectory.h"
#include <vector>

namespace MuonCalib{
  
MuonCalibExtraTreeAlg::MuonCalibExtraTreeAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_patterns(0),
  m_dir(nullptr),
  m_tree(nullptr),
  m_init(false) {
}  //end MuonCalibExtraTreeAlg::MuonCalibExtraTreeAlg

StatusCode MuonCalibExtraTreeAlg::initialize() {
    
  return StatusCode::SUCCESS;
}  // end MuonCalibExtraTreeAlg::initialize

StatusCode MuonCalibExtraTreeAlg::execute() {
  if(!m_init) {
    m_dir = RootFileManager::getInstance()->getDirectory(m_ntupleName);
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

  MuonPatternCombinationCollection::const_iterator it = m_patterns->begin();
  MuonPatternCombinationCollection::const_iterator it_end = m_patterns->end();
  unsigned int index = 0;
  for ( ; it!=it_end; ++it) {
    const std::vector< Muon::MuonPatternChamberIntersect > mpcivec = (*it)->chamberData();
    std::vector< Muon::MuonPatternChamberIntersect >::const_iterator pat_it = mpcivec.begin();
    for (;pat_it!=mpcivec.end();++pat_it) {
      const std::vector< const Trk::PrepRawData* > prdvec = (*pat_it).prepRawDataVec();
      std::vector< const Trk::PrepRawData* >::const_iterator prd_it = prdvec.begin();
      for( ; prd_it!= prdvec.end() ;++prd_it ) {

	const Trk::PrepRawData* hit = *prd_it;
	MuonFixedId fId = m_idToFixedIdTool->idToFixedId( hit->identify() );

	const Trk::TrkDetElementBase* detEl = hit->detectorElement() ;
	Amg::Vector3D gPos = detEl->center( hit->identify() );
	double error = std::sqrt(hit->localCovariance()(0,0));
	  
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
  ATH_MSG_DEBUG("finish eventready... ");
}  //end MuonCalibExtraTreeAlg::finishEvent

}//namespace MuonCalib
