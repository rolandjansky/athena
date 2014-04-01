/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetails_p1
 *
 * @brief persistent partner for TrigTauClusterDetails
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
**********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILS_P1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILS_P1_H

//need this for NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigTauClusterDetails.h"

class TrigTauClusterDetails_p1
{
 public:
  
  TrigTauClusterDetails_p1() {}
  friend class TrigTauClusterDetailsCnv_p1;
  friend class TrigTauClusterDetailsCnv_p2;
  
 private:
  
	/**  EM Radius per sampling */
	float m_EMRadius[NUMEMSAMP];
	/**  HAD Radius per sampling */
	float m_HADRadius[NUMHADSAMP];
	/**  Width in EM samplings */
	float m_EMenergyWidth[NUMEMSAMP];
	/**  Width in HAD samplings */
        float m_HADenergyWidth[NUMHADSAMP];

	/**  EM Energy in the mormalization region (0.3x0.3) */
        float m_EMenergyWide[NUMEMSAMP];
	/**  EM Energy in the Nor region (0.2x0.2) */
        float m_EMenergyMedium[NUMEMSAMP];
	/**  EM Energy in the Nar region (0.1x0.1) */
        float m_EMenergyNarrow[NUMEMSAMP];

	/**  HAD Energy in the normalization region (0.3x0.3) */
        float m_HADenergyWide[NUMHADSAMP];
	/**  HAD Energy in the Nor region (0.2x0.2) */
        float m_HADenergyMedium[NUMHADSAMP];
	/**  HAD Energy in the Nar region (0.1x0.1) */
        float m_HADenergyNarrow[NUMHADSAMP];

};

#endif
