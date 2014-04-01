/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterCnv_p5
 *
 *
 * @author M. Pilar Casado <casado@ifae.es>  - IFAE/UAB - Barcelona
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_P5_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p5.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

class MsgStream;

class TrigTauClusterCnv_p5 : public T_AthenaPoolTPCnvBase<TrigTauCluster, TrigTauCluster_p5>
{
public:
	TrigTauClusterCnv_p5() { }

	virtual void persToTrans(const TrigTauCluster_p5* persObj, TrigTauCluster* transObj, MsgStream &log);
	virtual void transToPers(const TrigTauCluster* transObj, TrigTauCluster_p5* persObj, MsgStream &log);

protected:
	// to persistify ElementLinks to cluster and track 
	ElementLinkCnv_p3<ElementLink<TrigTauClusterDetailsContainer> > m_ELinkTauClusterDetailsCnv;
};
#endif
