/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigTauClusterDetails_p1
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
        float m_EMRadius[NUMEMSAMP] = {0};
	/**  HAD Radius per sampling */
        float m_HADRadius[NUMHADSAMP] = {0};
	/**  Width in EM samplings */
	float m_EMenergyWidth[NUMEMSAMP] = {0};
	/**  Width in HAD samplings */
        float m_HADenergyWidth[NUMHADSAMP] = {0};

	/**  EM Energy in the mormalization region (0.3x0.3) */
        float m_EMenergyWide[NUMEMSAMP] = {0};
	/**  EM Energy in the Nor region (0.2x0.2) */
        float m_EMenergyMedium[NUMEMSAMP] = {0};
	/**  EM Energy in the Nar region (0.1x0.1) */
        float m_EMenergyNarrow[NUMEMSAMP] = {0};

	/**  HAD Energy in the normalization region (0.3x0.3) */
        float m_HADenergyWide[NUMHADSAMP] = {0};
	/**  HAD Energy in the Nor region (0.2x0.2) */
        float m_HADenergyMedium[NUMHADSAMP] = {0};
	/**  HAD Energy in the Nar region (0.1x0.1) */
        float m_HADenergyNarrow[NUMHADSAMP] = {0};

};

#endif
