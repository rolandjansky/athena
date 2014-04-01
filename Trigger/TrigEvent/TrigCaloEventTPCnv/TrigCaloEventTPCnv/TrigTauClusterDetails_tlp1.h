/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetails_tlp1
 *
 * @brief "top level" persistent partner for TrigTauClusterDetails
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILS_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILS_TLP1_H

#include "TrigCaloEventTPCnv/TrigTauClusterDetails_p1.h"

class TrigTauClusterDetails_tlp1
{
 public:
  
  TrigTauClusterDetails_tlp1() {}
  friend class TrigTauClusterDetailsCnv_tlp1; 
  
 private:
  
  std::vector< TrigTauClusterDetails_p1 >    m_trigTauCluster;

};

#endif
