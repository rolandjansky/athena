/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauCluster_p2
 *
 * @brief persistent partner for TrigTauCluster
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p1.h"

//need this for MAXSIZE, NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigTauCluster.h"

class TrigTauCluster_p2
{
 public:
  
  TrigTauCluster_p2() :
    m_EMenergy(0.0),
    m_HADenergy(0.0),
    m_eCalib(0.0),
    m_EMRadius2(0.0),
    m_CaloRadius(0.0),
    m_IsoFrac(0.0),
    m_numStripCells(0),
    m_numTotCells(0),
    m_stripWidth(0.0),
    m_stripWidthOffline(0.0),
    m_valid(false)
      {}
  friend class TrigTauClusterCnv_p2;
  
 private:
  
	/**  EM Energy */
        float m_EMenergy;
	/**  HAD Energy */
        float m_HADenergy;
	/**  Calibrated Energy */
        float m_eCalib;
	/**  EM Radius */
	float m_EMRadius2;
	/**  Total Calo Radius */
	float m_CaloRadius;
	/**  Isolation Fraction */
	float m_IsoFrac;
	/**  Number of fired strips */
	int  m_numStripCells;
	/**  Total number of cells in the cluster */
	int  m_numTotCells;
	/**  Width in the strip sampling */
        float m_stripWidth;
	/**  Width as in Offline */
        float m_stripWidthOffline;
	
	bool m_valid;
	/** ElementLink to TrigTauClusterDetailsContainer **/
	ElementLinkInt_p1 m_details;
  
        //the TrigCaloCluster base class
        TPObjRef m_trigCaloCluster;

};

#endif
