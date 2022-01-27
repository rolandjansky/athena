/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>



#include "TrkTruthAlgs/TrackTruthSimilaritySelector.h"
#include "TrkToolInterfaces/IDetailedTrackTruthSimilarity.h"

TrackTruthSimilaritySelector::TrackTruthSimilaritySelector(const std::string &name,ISvcLocator *pSvcLocator)
  : AthAlgorithm(name,pSvcLocator),
  m_detailed("DetailedTrackTruth"), m_out("TrackTruthNew")
{
  declareProperty("DetailedTrackTruthName",  m_detailed);
  declareProperty("OutputName",  m_out);
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackTruthSimilaritySelector::initialize()
{
  ATH_MSG_INFO ("TrackTruthSimilaritySelector::initialize(), output " << m_out.name());
  ATH_CHECK( m_matchTool.retrieve() );
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

  if (!m_detailed.isValid()){
    ATH_MSG_ERROR ("DetailedTrackTruthCollection "<<m_detailed.name()<<" NOT found");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("Got DetailedTrackTruthCollection "<<m_detailed.name());
  }

  //----------------------------------------------------------------
  // Produce and store the output.

  m_out = std::make_unique<TrackTruthCollection>
               (m_detailed->trackCollectionLink());

  fillOutput(m_out.ptr(), m_detailed.cptr());
  
  ATH_MSG_DEBUG ("TrackTruthCollection '"<<m_out.name()<<"' is registered in StoreGate, size="<<m_out->size());
  
  
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
