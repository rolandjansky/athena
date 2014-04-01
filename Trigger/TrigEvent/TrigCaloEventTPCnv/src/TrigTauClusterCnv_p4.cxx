/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigCaloEvent/TrigTauCluster.h"
#undef private

#include "TrigCaloEventTPCnv/TrigTauCluster_p4.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p4.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"

// pre-allocate converters
static TrigCaloClusterCnv_p2 trigCaloClusterCnv;

void TrigTauClusterCnv_p4 :: persToTrans(const TrigTauCluster_p4 *persObj, TrigTauCluster *transObj, MsgStream& log)
{
	log << MSG::DEBUG << "TrigTauClusterCnv_p4::persToTrans" << endreq;

	transObj->m_EMenergy = persObj->m_allTheFloats[0];
	transObj->m_HADenergy = persObj->m_allTheFloats[1];
	transObj->m_eCalib = persObj->m_allTheFloats[2];
	transObj->m_EMRadius2 = persObj->m_allTheFloats[3];
	transObj->m_CaloRadius = persObj->m_allTheFloats[4];
	transObj->m_IsoFrac = persObj->m_allTheFloats[5];
	transObj->m_stripWidth = persObj->m_allTheFloats[6];
	transObj->m_stripWidthOffline = persObj->m_allTheFloats[7];
	transObj->m_EMRadius3S = persObj->m_allTheFloats[8];
	transObj->m_CoreFrac = persObj->m_allTheFloats[9];
	transObj->m_EMFrac = persObj->m_allTheFloats[10];

	transObj->m_numStripCells = persObj->m_allTheInts[0];
	transObj->m_numTotCells = persObj->m_allTheInts[1];
	//transObj->m_numStripCells = persObj->m_numStripCells;
	//transObj->m_numTotCells = persObj->m_numTotCells;

	m_ELinkTauClusterDetailsCnv.persToTrans(&persObj->m_details, &transObj->m_clusterDetails, log);
	trigCaloClusterCnv.persToTrans(&persObj->m_trigCaloCluster, transObj, log);
}

void TrigTauClusterCnv_p4 :: transToPers(const TrigTauCluster *transObj, TrigTauCluster_p4 *persObj, MsgStream& log)
{
	log << MSG::DEBUG << "TrigTauClusterCnv_p4::transToPers" << endreq;

	persObj->m_allTheFloats[0] = transObj->m_EMenergy;
	persObj->m_allTheFloats[1] = transObj->m_HADenergy;
	persObj->m_allTheFloats[2] = transObj->m_eCalib;
	persObj->m_allTheFloats[3] = transObj->m_EMRadius2;
	persObj->m_allTheFloats[4] = transObj->m_CaloRadius;
	persObj->m_allTheFloats[5] = transObj->m_IsoFrac;
	persObj->m_allTheFloats[6] = transObj->m_stripWidth;
	persObj->m_allTheFloats[7] = transObj->m_stripWidthOffline;
	persObj->m_allTheFloats[8] = transObj->m_EMRadius3S;
	persObj->m_allTheFloats[9] = transObj->m_CoreFrac;
	persObj->m_allTheFloats[10] = transObj->m_EMFrac;

	persObj->m_allTheInts[0] = transObj->m_numStripCells;
	persObj->m_allTheInts[1] = transObj->m_numTotCells;
	//persObj->m_numStripCells = transObj->m_numStripCells;
	//persObj->m_numTotCells = transObj->m_numTotCells;

	log << MSG::DEBUG << "TrigTauClusterCnv_p4::transToPers - fill EL" << endreq;

	m_ELinkTauClusterDetailsCnv.transToPers(&transObj->m_clusterDetails, &persObj->m_details, log);

	log << MSG::DEBUG << "TrigTauClusterCnv_p4::transToPers - fill trigcalocluster" << endreq;

	trigCaloClusterCnv.transToPers(transObj, &persObj->m_trigCaloCluster, log);

	log << MSG::DEBUG << "TrigTauClusterCnv_p4::transToPers - filled" << endreq;
}
