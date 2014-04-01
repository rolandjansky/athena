/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauCluster_p3
 *
 * @brief persistent partner for TrigTauCluster
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P3_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P3_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

//need this for MAXSIZE, NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"

class TrigTauCluster_p3 {
  friend class TrigTauClusterCnv_p3;

 public:
  
  TrigTauCluster_p3() :  m_valid(false) {}
  ~TrigTauCluster_p3(){}

 private:
  
        float m_allTheFloats[8];
        int m_allTheInts[2];

	/**  EM Energy */
//        float m_EMenergy;
	/**  HAD Energy */
//        float m_HADenergy;
	/**  Calibrated Energy */
//        float m_eCalib;
	/**  EM Radius */
//	  float m_EMRadius2;
	/**  Total Calo Radius */
//	  float m_CaloRadius;
	/**  Isolation Fraction */
//	  float m_IsoFrac;
	/**  Number of fired strips */
//	int  m_numStripCells;
	/**  Total number of cells in the cluster */
//	int  m_numTotCells;
	/**  Width in the strip sampling */
//        float m_stripWidth;
	/**  Width as in Offline */
//        float m_stripWidthOffline;
	
	bool m_valid;
	/** ElementLink to TrigTauClusterDetailsContainer **/
	ElementLinkInt_p3 m_details;
        TrigCaloCluster_p2 m_trigCaloCluster;
};

#endif
 
