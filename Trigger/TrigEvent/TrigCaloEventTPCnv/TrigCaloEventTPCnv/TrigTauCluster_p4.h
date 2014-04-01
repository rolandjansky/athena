/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauCluster_p4
 *
 * @brief persistent partner for TrigTauCluster
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P4_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P4_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

//need this for MAXSIZE, NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"

class TrigTauCluster_p4 {
	friend class TrigTauClusterCnv_p4;

public:
	TrigTauCluster_p4() : m_valid(false) {}
	~TrigTauCluster_p4() {}

private:
	float m_allTheFloats[11];
//	float m_allTheFloats[9];
	int m_allTheInts[2];

	bool m_valid;
	/** ElementLink to TrigTauClusterDetailsContainer **/
	ElementLinkInt_p3 m_details;
	TrigCaloCluster_p2 m_trigCaloCluster;
};

#endif
