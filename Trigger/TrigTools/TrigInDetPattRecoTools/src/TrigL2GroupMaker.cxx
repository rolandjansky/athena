/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetPattRecoTools/TrigL2HoughTransformClass.h"
#include "TrigInDetPattRecoTools/TrigL2HashKeyFunction.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/TrigL2GroupMakerBase.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigInDetPattRecoTools/TrigL2GroupMaker.h"

#include <cmath>

void TrigL2GroupMaker::makeTrackCandidates(std::vector<TrigSiSpacePointBase*>& vsp, 
					   std::vector<TrigL2TrackCandidate*>& vtr, 
					   double zVertex,
					   double layerThreshold, 
					   const IRoiDescriptor& roi ) {

  double etaHalfWidth = 0.5*(roi.etaPlus()-roi.etaMinus());
  double phiHalfWidth = 0.5*std::fabs(HLT::wrapPhi(roi.phiPlus()-roi.phiMinus()));
  
  makeTrackCandidates(vsp,vtr,
		      zVertex, layerThreshold,
		      roi.eta(), etaHalfWidth, roi.phi(), phiHalfWidth );
  
}

