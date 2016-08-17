/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPDataAuxContainer_v1.cxx 693858 2015-12-01 10:30:15Z lnozka $

// Local include(s):
#include "xAODForward/versions/AFPDataAuxContainer_v1.h"

namespace xAOD
{

   AFPDataAuxContainer_v1::AFPDataAuxContainer_v1()
	  : AuxContainerBase()
   {

	   //AfpSIDDigiCollection ---------------------------------------------------
	   AUX_VARIABLE(pixelTotNum);
	   AUX_VARIABLE(pixelStID);
	   AUX_VARIABLE(pixelLayerID);
	   AUX_VARIABLE(pixelRow);
	   AUX_VARIABLE(pixelCol);
	   AUX_VARIABLE(pixelE);

	   AUX_VARIABLE(layerTotNum);
	   AUX_VARIABLE(layerStID);
	   AUX_VARIABLE(layerLayerID);
	   AUX_VARIABLE(layerNpix);
	   AUX_VARIABLE(layerEtot);

	   AUX_VARIABLE(statTotNum);
	   AUX_VARIABLE(statStID);
	   AUX_VARIABLE(statNpix);
	   AUX_VARIABLE(statEtot);

	   //AfpSIDLocRecoEvCollection -----------------------------------------------
	   AUX_VARIABLE(trkTotNumTracks);
	   AUX_VARIABLE(trkStID);
	   AUX_VARIABLE(trkX);
	   AUX_VARIABLE(trkY);
	   AUX_VARIABLE(trkZ);
	   AUX_VARIABLE(trkXslope);
	   AUX_VARIABLE(trkYslope);
	   AUX_VARIABLE(trkNpix);
	   AUX_VARIABLE(trkNholes);
	   AUX_VARIABLE(trkQuality);

	   //AfpSIDSimHitCollection --------------------------------------------------
	   AUX_VARIABLE(simSidTotNumHits);
	   AUX_VARIABLE(simSidHitID);
	   AUX_VARIABLE(simSidTrackID);
	   AUX_VARIABLE(simSidEncoding);
	   AUX_VARIABLE(simSidKineticE);
	   AUX_VARIABLE(simSidDepE);
	   AUX_VARIABLE(simSidPreStepX);
	   AUX_VARIABLE(simSidPreStepY);
	   AUX_VARIABLE(simSidPreStepZ);
	   AUX_VARIABLE(simSidPostStepX);
	   AUX_VARIABLE(simSidPostStepY);
	   AUX_VARIABLE(simSidPostStepZ);
	   AUX_VARIABLE(simSidTime);
	   AUX_VARIABLE(simSidStID);
	   AUX_VARIABLE(simSidLayerID);
	   AUX_VARIABLE(simSidIsVacLayer);
	   AUX_VARIABLE(simSidPixelRow);
	   AUX_VARIABLE(simSidPixelCol);

	   //AfpTDDigiCollection --------------------------------------------------

	   //AfpTDLocRecoEvCollection ---------------------------------------------
	   AUX_VARIABLE(tidTotNumTracks);
	   AUX_VARIABLE(tidStID);
	   AUX_VARIABLE(tidQID);
	   AUX_VARIABLE(tidTrainID);
	   AUX_VARIABLE(tidTime);
	   AUX_VARIABLE(tidAmplitude);
	   AUX_VARIABLE(tidNumSaturatedBars);

	   //AfpTDSimHitCollection ------------------------------------------------
	   AUX_VARIABLE(simTidTotNumHits);
	   AUX_VARIABLE(simTidHitID);
	   AUX_VARIABLE(simTidTrackID);
	   AUX_VARIABLE(simTidEncoding);
	   AUX_VARIABLE(simTidKineticE);
	   AUX_VARIABLE(simTidDepE);
	   AUX_VARIABLE(simTidPreStepX);
	   AUX_VARIABLE(simTidPreStepY);
	   AUX_VARIABLE(simTidPreStepZ);
	   AUX_VARIABLE(simTidPostStepX);
	   AUX_VARIABLE(simTidPostStepY);
	   AUX_VARIABLE(simTidPostStepZ);
	   AUX_VARIABLE(simTidTime);
	   AUX_VARIABLE(simTidStID);
	   AUX_VARIABLE(simTidLayerID);
	   AUX_VARIABLE(simTidSensElID);

   }

} // namespace xAOD

