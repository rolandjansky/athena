/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterContainer_p3
 *
 * @brief persistent partner for TrigEMClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterContainer_p3.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_P3_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_P3_H

#include "TrigCaloEventTPCnv/TrigEMCluster_p3.h"
  
class TrigEMClusterContainer_p3 : public std::vector<TrigEMCluster_p3> { };
    
#endif
