/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#define private public
//#define protected public
#include "AFP_SimEv/AFP_SIDSimHit.h"
//#undef private
//#undef protected

#include "Identifier/Identifier.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCnv_p1.h"

void AFP_SIDSimHitCnv_p1::persToTrans(const AFP_SIDSimHit_p1* persObj, AFP_SIDSimHit* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_SIDSimHitCnv_p1::persToTrans called " << endmsg;

	transObj->m_nHitID=persObj->m_nHitID;
	transObj->m_nTrackID=persObj->m_nTrackID;
	transObj->m_nParticleEncoding=persObj->m_nParticleEncoding;
	transObj->m_fKineticEnergy=persObj->m_fKineticEnergy;
	transObj->m_fEnergyDeposit=persObj->m_fEnergyDeposit;
	transObj->m_fPreStepX=persObj->m_fPreStepX;
	transObj->m_fPreStepY=persObj->m_fPreStepY;
	transObj->m_fPreStepZ=persObj->m_fPreStepZ;
	transObj->m_fPostStepX=persObj->m_fPostStepX;
	transObj->m_fPostStepY=persObj->m_fPostStepY;
	transObj->m_fPostStepZ=persObj->m_fPostStepZ;
	transObj->m_fGlobalTime=persObj->m_fGlobalTime;

	transObj->m_nStationID=persObj->m_nStationID;
	transObj->m_nDetectorID=persObj->m_nDetectorID;
	
	transObj->m_nPixelRow=persObj->m_nPixelRow;
	transObj->m_nPixelCol=persObj->m_nPixelCol;

	transObj->m_bIsAuxVSID=persObj->m_bIsAuxVSID;
}

void AFP_SIDSimHitCnv_p1::transToPers(const AFP_SIDSimHit* transObj, AFP_SIDSimHit_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_SIDSimHitCnv_p1::transToPers called " << endmsg;

	persObj->m_nHitID=transObj->m_nHitID;
	persObj->m_nTrackID=transObj->m_nTrackID;
	persObj->m_nParticleEncoding=transObj->m_nParticleEncoding;
	persObj->m_fKineticEnergy=transObj->m_fKineticEnergy;
	persObj->m_fEnergyDeposit=transObj->m_fEnergyDeposit;
	persObj->m_fPreStepX=transObj->m_fPreStepX;
	persObj->m_fPreStepY=transObj->m_fPreStepY;
	persObj->m_fPreStepZ=transObj->m_fPreStepZ;
	persObj->m_fPostStepX=transObj->m_fPostStepX;
	persObj->m_fPostStepY=transObj->m_fPostStepY;
	persObj->m_fPostStepZ=transObj->m_fPostStepZ;
	persObj->m_fGlobalTime=transObj->m_fGlobalTime;

	persObj->m_nStationID=transObj->m_nStationID;
	persObj->m_nDetectorID=transObj->m_nDetectorID;
	
	persObj->m_nPixelRow=transObj->m_nPixelRow;
	persObj->m_nPixelCol=transObj->m_nPixelCol;

	
	persObj->m_bIsAuxVSID=transObj->m_bIsAuxVSID;
 }
