// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPDataAuxContainer_v1.h 693858 2015-09-09 10:30:15Z lnozka $
#ifndef XAODFORWARD_VERSIONS_AFPDATAAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPDATAAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD
{

	/// Auxiliary store for xAOD::AFPDataContainer_v1
	///
	/// This is a custom auxiliary container for xAOD::AFPDataContainer_v1,
	/// to be used in the main reconstruction code.
	///
	/// @author Libor Nozka <Libor.Nozka@cern.ch>
	///
	/// $Revision: 693858 $
	/// $Date: 2015-09-09 12:30:15 +0200 (Wed, 09 Sep 2015) $
	///
	class AFPDataAuxContainer_v1 : public AuxContainerBase
	{

	public:
		/// Default constructor
		AFPDataAuxContainer_v1();

	private:

		//AfpSIDDigiCollection ---------------------------------------------------
		std::vector<int> pixelTotNum;
		std::vector<std::vector<int> > pixelStID;
		std::vector<std::vector<int> > pixelLayerID;
		std::vector<std::vector<int> > pixelRow;
		std::vector<std::vector<int> > pixelCol;
		std::vector<std::vector<float> > pixelE;

		std::vector<int> layerTotNum;
		std::vector<std::vector<int> > layerStID;
		std::vector<std::vector<int> > layerLayerID;
		std::vector<std::vector<int> > layerNpix;
		std::vector<std::vector<float> > layerEtot;

		std::vector<int> statTotNum;
		std::vector<std::vector<int> > statStID;
		std::vector<std::vector<int> > statNpix;
		std::vector<std::vector<float> > statEtot;

		//AfpSIDLocRecoEvCollection -----------------------------------------------
		std::vector<int> trkTotNumTracks;
		std::vector<std::vector<int> > trkStID;
		std::vector<std::vector<float> > trkX;
		std::vector<std::vector<float> > trkY;
		std::vector<std::vector<float> > trkZ;
		std::vector<std::vector<float> > trkXslope;
		std::vector<std::vector<float> > trkYslope;
		std::vector<std::vector<int> > trkNpix;
		std::vector<std::vector<int> > trkNholes;
		std::vector<std::vector<float> > trkQuality;

		//AfpSIDSimHitCollection --------------------------------------------------
		std::vector<int> simSidTotNumHits;
		std::vector<std::vector<int> > simSidHitID;
		std::vector<std::vector<int> > simSidTrackID;
		std::vector<std::vector<int> > simSidEncoding;
		std::vector<std::vector<float> > simSidKineticE;
		std::vector<std::vector<float> > simSidDepE;
		std::vector<std::vector<float> > simSidPreStepX;
		std::vector<std::vector<float> > simSidPreStepY;
		std::vector<std::vector<float> > simSidPreStepZ;
		std::vector<std::vector<float> > simSidPostStepX;
		std::vector<std::vector<float> > simSidPostStepY;
		std::vector<std::vector<float> > simSidPostStepZ;
		std::vector<std::vector<float> > simSidTime;
		std::vector<std::vector<int> > simSidStID;
		std::vector<std::vector<int> > simSidLayerID;
		std::vector<std::vector<int> > simSidIsVacLayer;
		std::vector<std::vector<int> > simSidPixelRow;
		std::vector<std::vector<int> > simSidPixelCol;

		//AfpTDDigiCollection --------------------------------------------------

		//AfpTDLocRecoEvCollection ---------------------------------------------
		std::vector<int> tidTotNumTracks;
		std::vector<std::vector<int> > tidStID;
		std::vector<std::vector<int> > tidQID;
		std::vector<std::vector<int> > tidTrainID;
		std::vector<std::vector<float> > tidTime;
		std::vector<std::vector<float> > tidAmplitude;
		std::vector<std::vector<int> > tidNumSaturatedBars;

		//AfpTDSimHitCollection ------------------------------------------------
		std::vector<int> simTidTotNumHits;
		std::vector<std::vector<int> > simTidHitID;
		std::vector<std::vector<int> > simTidTrackID;
		std::vector<std::vector<int> > simTidEncoding;
		std::vector<std::vector<float> > simTidKineticE;
		std::vector<std::vector<float> > simTidDepE;
		std::vector<std::vector<float> > simTidPreStepX;
		std::vector<std::vector<float> > simTidPreStepY;
		std::vector<std::vector<float> > simTidPreStepZ;
		std::vector<std::vector<float> > simTidPostStepX;
		std::vector<std::vector<float> > simTidPostStepY;
		std::vector<std::vector<float> > simTidPostStepZ;
		std::vector<std::vector<float> > simTidTime;
		std::vector<std::vector<int> > simTidStID;
		std::vector<std::vector<int> > simTidLayerID;
		std::vector<std::vector<int> > simTidSensElID;

	}; // class AFPDataAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPDataAuxContainer_v1, xAOD::AuxContainerBase );

#endif //XAODFORWARD_VERSIONS_AFPDATAAUXCONTAINER_V1_H
