/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetails_p2
 *
 * @brief persistent partner for TrigTauClusterDetails
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
**********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILS_P2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILS_P2_H

//need this for NUMEMSAMP, and NUMHADSAMP
#include "TrigCaloEvent/TrigTauClusterDetails.h"

class TrigTauClusterDetails_p2
{
 public:
  
  TrigTauClusterDetails_p2() {}
  friend class TrigTauClusterDetailsCnv_p2;
  
 private:
  
        float m_allEMFloats[20];
	/**  EM Radius per sampling */
//	  float m_EMRadius[NUMEMSAMP];
	/**  Width in EM samplings */
//	  float m_EMenergyWidth[NUMEMSAMP];
	/**  EM Energy in the mormalization region (0.3x0.3) */
//        float m_EMenergyWide[NUMEMSAMP];
	/**  EM Energy in the Nor region (0.2x0.2) */
//        float m_EMenergyMedium[NUMEMSAMP];
	/**  EM Energy in the Nar region (0.1x0.1) */
//        float m_EMenergyNarrow[NUMEMSAMP];

        float m_allHADFloats[15];
	/**  HAD Radius per sampling */
//	  float m_HADRadius[NUMHADSAMP];
	/**  Width in HAD samplings */
//        float m_HADenergyWidth[NUMHADSAMP];
	/**  HAD Energy in the normalization region (0.3x0.3) */
//        float m_HADenergyWide[NUMHADSAMP];
	/**  HAD Energy in the Nor region (0.2x0.2) */
//        float m_HADenergyMedium[NUMHADSAMP];
	/**  HAD Energy in the Nar region (0.1x0.1) */
//        float m_HADenergyNarrow[NUMHADSAMP];

};

#endif
