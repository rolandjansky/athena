// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPData_v1.h 693858 2015-09-09 10:30:15Z lnozka $
#ifndef XAODFORWARD_VERSIONS_AFPDATA_V1_H
#define XAODFORWARD_VERSIONS_AFPDATA_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD
{

	/// Class containing AFP information
	///
	/// For information how to retrive, and to some level use this object,
	/// see the detailed package description.
	///
	/// @author Libor Nozka <Libor.Nozka@cern.ch>
	///
	/// $Revision: 693858 $
	/// $Date: 2015-09-09 12:30:15 +0200 (Wed, 09 Sep 2015) $
	///
	class AFPData_v1 : public SG::AuxElement
	{

	public:
		/// Default constructor
		AFPData_v1();

		//AfpSIDDigiCollection ---------------------------------------------------
		int pixelTotNum() const;
		void setPixelTotNum(int value);
		const std::vector<int>& pixelStID() const;
		void setPixelStID(const std::vector<int>& value);
		const std::vector<int>& pixelLayerID() const;
		void setPixelLayerID(const std::vector<int>& value);
		const std::vector<int>& pixelRow() const;
		void setPixelRow(const std::vector<int>& value);
		const std::vector<int>& pixelCol() const;
		void setPixelCol(const std::vector<int>& value);
		const std::vector<float>& pixelE() const;
		void setPixelE(const std::vector<float>& value);


		int layerTotNum() const;
		void setLayerTotNum(int value);
		const std::vector<int>& layerStID() const;
		void setLayerStID(const std::vector<int>& value);
		const std::vector<int>& layerLayerID() const;
		void setLayerLayerID(const std::vector<int>& value);
		const std::vector<int>& layerNpix() const;
		void setLayerNpix(const std::vector<int>& value);
		const std::vector<float>& layerEtot() const;
		void setLayerEtot(const std::vector<float>& value);

		int statTotNum() const;
		void setStatTotNum(int value);
		const std::vector<int>& statStID() const;
		void setStatStID(const std::vector<int>& value);
		const std::vector<int>& statNpix() const;
		void setStatNpix(const std::vector<int>& value);
		const std::vector<float>& statEtot() const;
		void setStatEtot(const std::vector<float>& value);


		//AfpSIDLocRecoEvCollection -----------------------------------------------
		int trkTotNumTracks() const;
		void setTrkTotNumTracks(int value);
		const std::vector<int>& trkStID() const;
		void setTrkStID(const std::vector<int>& value);
		const std::vector<float>& trkX() const;
		void setTrkX(const std::vector<float>& value);
		const std::vector<float>& trkY() const;
		void setTrkY(const std::vector<float>& value);
		const std::vector<float>& trkZ() const;
		void setTrkZ(const std::vector<float>& value);
		const std::vector<float>& trkXslope() const;
		void setTrkXslope(const std::vector<float>& value);
		const std::vector<float>& trkYslope() const;
		void setTrkYslope(const std::vector<float>& value);
		const std::vector<int>& trkNpix() const;
		void setTrkNpix(const std::vector<int>& value);
		const std::vector<int>& trkNholes() const;
		void setTrkNholes(const std::vector<int>& value);
		const std::vector<float>& trkQuality() const;
		void setTrkQuality(const std::vector<float>& value);

		//AfpSIDSimHitCollection -----------------------------------------------
		int simSidTotNumHits() const;
		void setSimSidTotNumHits(int value);
		const std::vector<int>& simSidHitID() const;
		void setSimSidHitID(const std::vector<int>& value);
		const std::vector<int>& simSidTrackID() const;
		void setSimSidTrackID(const std::vector<int>& value);
		const std::vector<int>& simSidEncoding() const;
		void setSimSidEncoding(const std::vector<int>& value);
		const std::vector<float>& simSidKineticE() const;
		void setSimSidKineticE(const std::vector<float>& value);
		const std::vector<float>& simSidDepE() const;
		void setSimSidDepE(const std::vector<float>& value);
		const std::vector<float>& simSidPreStepX() const;
		void setSimSidPreStepX(const std::vector<float>& value);
		const std::vector<float>& simSidPreStepY() const;
		void setSimSidPreStepY(const std::vector<float>& value);
		const std::vector<float>& simSidPreStepZ() const;
		void setSimSidPreStepZ(const std::vector<float>& value);
		const std::vector<float>& simSidPostStepX() const;
		void setSimSidPostStepX(const std::vector<float>& value);
		const std::vector<float>& simSidPostStepY() const;
		void setSimSidPostStepY(const std::vector<float>& value);
		const std::vector<float>& simSidPostStepZ() const;
		void setSimSidPostStepZ(const std::vector<float>& value);
		const std::vector<float>& simSidTime() const;
		void setSimSidTime(const std::vector<float>& value);
		const std::vector<int>& simSidStID() const;
		void setSimSidStID(const std::vector<int>& value);
		const std::vector<int>& simSidLayerID() const;
		void setSimSidLayerID(const std::vector<int>& value);
		const std::vector<int>& simSidIsVacLayer() const;
		void setSimSidIsVacLayer(const std::vector<int>& value);
		const std::vector<int>& simSidPixelRow() const;
		void setSimSidPixelRow(const std::vector<int>& value);
		const std::vector<int>& simSidPixelCol() const;
		void setSimSidPixelCol(const std::vector<int>& value);

		//AfpTDDigiCollection --------------------------------------------------

		//AfpTDLocRecoEvCollection ---------------------------------------------
		int tidTotNumTracks() const;
		void setTidTotNumTracks(int value);
		const std::vector<int>& tidStID() const;
		void setTidStID(const std::vector<int>& value);
		const std::vector<int>& tidQID() const;
		void setTidQID(const std::vector<int>& value);
		const std::vector<int>& tidTrainID() const;
		void setTidTrainID(const std::vector<int>& value);
		const std::vector<float>& tidTime() const;
		void setTidTime(const std::vector<float>& value);
		const std::vector<float>& tidAmplitude() const;
		void setTidAmplitude(const std::vector<float>& value);
		const std::vector<int>& tidNumSaturatedBars() const;
		void setTidNumSaturatedBars(const std::vector<int>& value);

		//AfpTDSimHitCollection ------------------------------------------------
		int simTidTotNumHits() const;
		void setSimTidTotNumHits(int value);
		const std::vector<int>& simTidHitID() const;
		void setSimTidHitID(const std::vector<int>& value);
		const std::vector<int>& simTidTrackID() const;
		void setSimTidTrackID(const std::vector<int>& value);
		const std::vector<int>& simTidEncoding() const;
		void setSimTidEncoding(const std::vector<int>& value);
		const std::vector<float>& simTidKineticE() const;
		void setSimTidKineticE(const std::vector<float>& value);
		const std::vector<float>& simTidDepE() const;
		void setSimTidDepE(const std::vector<float>& value);
		const std::vector<float>& simTidPreStepX() const;
		void setSimTidPreStepX(const std::vector<float>& value);
		const std::vector<float>& simTidPreStepY() const;
		void setSimTidPreStepY(const std::vector<float>& value);
		const std::vector<float>& simTidPreStepZ() const;
		void setSimTidPreStepZ(const std::vector<float>& value);
		const std::vector<float>& simTidPostStepX() const;
		void setSimTidPostStepX(const std::vector<float>& value);
		const std::vector<float>& simTidPostStepY() const;
		void setSimTidPostStepY(const std::vector<float>& value);
		const std::vector<float>& simTidPostStepZ() const;
		void setSimTidPostStepZ(const std::vector<float>& value);
		const std::vector<float>& simTidTime() const;
		void setSimTidTime(const std::vector<float>& value);
		const std::vector<int>& simTidStID() const;
		void setSimTidStID(const std::vector<int>& value);
		const std::vector<int>& simTidLayerID() const;
		void setSimTidLayerID(const std::vector<int>& value);
		const std::vector<int>& simTidSensElID() const;
		void setSimTidSensElID(const std::vector<int>& value);

	}; // class AFPData_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::AFPData_v1, SG::AuxElement);

#endif // XAODFORWARD_VERSIONS_AFPDATA_V1_H
