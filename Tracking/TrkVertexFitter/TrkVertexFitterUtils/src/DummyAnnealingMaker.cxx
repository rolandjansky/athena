/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          DummyAnnealingMaker.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitterUtils/DummyAnnealingMaker.h"

namespace Trk
{
  
  StatusCode DummyAnnealingMaker::initialize() 
  { 
    ATH_MSG_DEBUG( "Initialize successful"  );
    return StatusCode::SUCCESS;
  }

  StatusCode DummyAnnealingMaker::finalize() 
  {
    ATH_MSG_DEBUG( "Initialize successful"  );
    return StatusCode::SUCCESS;
  }

  void DummyAnnealingMaker::reset(AnnealingState& state) const {
    state = 0;
  }

  void DummyAnnealingMaker::anneal(AnnealingState& /*state*/) const {
  }

  double DummyAnnealingMaker::getWeight(const AnnealingState& /*state*/,
                                        double /* chisq */) const {
    return 0.5;
  }

  double DummyAnnealingMaker::getWeight(const AnnealingState& /*state*/,
                                        double /* chisq */,
                                        const std::vector<double>& /* allchisq */) const {
    return 0.5;
  }


}

	
