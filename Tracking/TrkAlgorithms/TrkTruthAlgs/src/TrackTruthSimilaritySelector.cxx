/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>



#include "TrkTruthAlgs/TrackTruthSimilaritySelector.h"
#include "TrkToolInterfaces/IDetailedTrackTruthSimilarity.h"

TrackTruthSimilaritySelector::TrackTruthSimilaritySelector(const std::string &name,ISvcLocator *pSvcLocator)
  : AthAlgorithm(name,pSvcLocator)
{
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackTruthSimilaritySelector::initialize()
{
  ATH_MSG_INFO ("TrackTruthSimilaritySelector::initialize(), output " << m_out.key());
  ATH_CHECK( m_matchTool.retrieve() );
  ATH_CHECK( m_detailed.initialize() );
  ATH_CHECK( m_out.initialize() );
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackTruthSimilaritySelector::finalize() {
  ATH_MSG_INFO ("TrackTruthSimilaritySelector finalized");
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackTruthSimilaritySelector::execute() {

  ATH_MSG_DEBUG ("TrackTruthSimilaritySelector::execute()");

  //----------------------------------------------------------------
  // Retrieve the input

  auto detailedHandle = SG::makeHandle( m_detailed );
  if (!detailedHandle.isValid()){
    ATH_MSG_ERROR ("DetailedTrackTruthCollection "<<m_detailed.key()<<" NOT found");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("Got DetailedTrackTruthCollection "<<m_detailed.key());
  }

  //----------------------------------------------------------------
  // Produce and store the output.

  auto outHandle = SG::makeHandle( m_out );
  outHandle = std::make_unique<TrackTruthCollection>
               (detailedHandle->trackCollectionLink());

  fillOutput(outHandle.ptr(), detailedHandle.cptr());
  
  ATH_MSG_DEBUG ("TrackTruthCollection '"<<m_out.key()<<"' is registered in StoreGate, size="<<outHandle->size());
  
  
  return StatusCode::SUCCESS;
}

//================================================================
void TrackTruthSimilaritySelector::fillOutput(TrackTruthCollection *out, 
					      const DetailedTrackTruthCollection *in)
{

  typedef DetailedTrackTruthCollection::const_iterator Iter;
  Iter itrackData=in->begin();
  while(itrackData!=in->end()) {
    std::pair<Iter,Iter> range = in->equal_range(itrackData->first);

    // We KNOW that the range is not empty - no need to check that.
    Iter selected = range.first;
    double bestProb = m_matchTool->trackTruthSimilarity(selected->second);
    ATH_MSG_VERBOSE ("track="<<selected->first.index()<<" prob="<<bestProb
		     <<" link: "<<*(selected->second.trajectory().rbegin()));

    for(Iter imatch = ++range.first; imatch != range.second; ++imatch) {
      double prob = m_matchTool->trackTruthSimilarity(imatch->second);
      ATH_MSG_VERBOSE ("track="<<imatch->first.index()<<" prob="<<prob
		       <<" link: "<<*(imatch->second.trajectory().rbegin()));
      if(prob>bestProb) {
	bestProb = prob;
	selected = imatch;
      }
    }

    // trajectory[0] is the LAST particle on the trajectory. The first is at trajectory.rbegin().
    const HepMcParticleLink& particleLink = *(selected->second.trajectory().rbegin());

    ATH_MSG_VERBOSE ("Truth selected for track="<<selected->first.index()<<" prob="<<bestProb<<" link: "<<particleLink);
    out->insert(std::make_pair(selected->first, TrackTruth(particleLink, bestProb, 0) ));
    itrackData=range.second;
  }

}

//================================================================
