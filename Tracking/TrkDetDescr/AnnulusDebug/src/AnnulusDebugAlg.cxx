#include "AnnulusDebug/AnnulusDebugAlg.h"
#include "StoreGate/StoreGateSvc.h"

AnnulusDebugAlg::AnnulusDebugAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm (name, pSvcLocator),
  m_tracks("Tracks")
{  
}

AnnulusDebugAlg::~AnnulusDebugAlg(void)
{}

StatusCode
AnnulusDebugAlg::initialize()
{
  ATH_MSG_INFO( "AnnulusDebugAlg::initialize(). " );
  return StatusCode::SUCCESS;
}

StatusCode
AnnulusDebugAlg::execute()
{
  ATH_MSG_VERBOSE ("AnnulusDebugAlg::execute()");
  const TrackCollection& tracks = *m_tracks;

  for(auto it = tracks.begin(); it != tracks.end(); ++it) {
    ATH_MSG_VERBOSE("I AM A TRACK!");
    const Trk::Track* track = *it;
    ATH_MSG_VERBOSE(*track);
  }
  

  return StatusCode::SUCCESS;
}

StatusCode
AnnulusDebugAlg::finalize()
{
  ATH_MSG_VERBOSE ("AnnulusDebugAlg::finalize()");
  return StatusCode::SUCCESS;
}

