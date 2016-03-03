/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPData_v1.cxx 693858 2015-12-01 10:30:15Z lnozka $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPData_v1.h"

namespace xAOD {

	AFPData_v1::AFPData_v1()
		: SG::AuxElement() {}


	//AfpSIDDigiCollection ---------------------------------------------------
	AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(AFPData_v1,int,pixelTotNum,setPixelTotNum)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1, std::vector<int>,pixelStID,setPixelStID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,pixelLayerID,setPixelLayerID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,pixelRow,setPixelRow)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,pixelCol,setPixelCol)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,pixelE,setPixelE)

	AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(AFPData_v1,int,layerTotNum,setLayerTotNum)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,layerStID,setLayerStID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,layerLayerID,setLayerLayerID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,layerNpix,setLayerNpix)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,layerEtot,setLayerEtot)

	AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(AFPData_v1,int,statTotNum,setStatTotNum)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,statStID,setStatStID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,statNpix,setStatNpix)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,statEtot,setStatEtot)

	//AfpSIDLocRecoEvCollection -----------------------------------------------
	AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(AFPData_v1,int,trkTotNumTracks,setTrkTotNumTracks)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,trkStID,setTrkStID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,trkX,setTrkX)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,trkY,setTrkY)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,trkZ,setTrkZ)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,trkXslope,setTrkXslope)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,trkYslope,setTrkYslope)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,trkNpix,setTrkNpix)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,trkNholes,setTrkNholes)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,trkQuality,setTrkQuality)

	//AfpSIDSimHitCollection --------------------------------------------------
	AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(AFPData_v1,int,simSidTotNumHits,setSimSidTotNumHits)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidHitID,setSimSidHitID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidTrackID,setSimSidTrackID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidEncoding,setSimSidEncoding)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidKineticE,setSimSidKineticE)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidDepE,setSimSidDepE)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidPreStepX,setSimSidPreStepX)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidPreStepY,setSimSidPreStepY)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidPreStepZ,setSimSidPreStepZ)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidPostStepX,setSimSidPostStepX)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidPostStepY,setSimSidPostStepY)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidPostStepZ,setSimSidPostStepZ)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simSidTime,setSimSidTime)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidStID,setSimSidStID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidLayerID,setSimSidLayerID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidIsVacLayer,setSimSidIsVacLayer)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidPixelRow,setSimSidPixelRow)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simSidPixelCol,setSimSidPixelCol)

	//AfpTDDigiCollection --------------------------------------------------

	//AfpTDLocRecoEvCollection ---------------------------------------------
	AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(AFPData_v1,int,tidTotNumTracks,setTidTotNumTracks)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,tidStID,setTidStID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,tidQID,setTidQID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,tidTrainID,setTidTrainID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,tidTime,setTidTime)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,tidAmplitude,setTidAmplitude)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,tidNumSaturatedBars,setTidNumSaturatedBars)

	//AfpTDSimHitCollection ------------------------------------------------
	AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(AFPData_v1,int,simTidTotNumHits,setSimTidTotNumHits)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simTidHitID,setSimTidHitID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simTidTrackID,setSimTidTrackID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simTidEncoding,setSimTidEncoding)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidKineticE,setSimTidKineticE)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidDepE,setSimTidDepE)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidPreStepX,setSimTidPreStepX)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidPreStepY,setSimTidPreStepY)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidPreStepZ,setSimTidPreStepZ)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidPostStepX,setSimTidPostStepX)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidPostStepY,setSimTidPostStepY)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidPostStepZ,setSimTidPostStepZ)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<float>,simTidTime,setSimTidTime)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simTidStID,setSimTidStID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simTidLayerID,setSimTidLayerID)
	AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPData_v1,std::vector<int>,simTidSensElID,setSimTidSensElID)

} // namespace xAOD

