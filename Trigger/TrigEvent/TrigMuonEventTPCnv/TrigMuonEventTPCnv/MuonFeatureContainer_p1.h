/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureContainer_p1
 *
 * @brief persistent partner for MuonFeatureContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeatureContainer_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class MuonFeatureContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  MuonFeatureContainer_p1(){}
  friend class MuonFeatureContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_muonFeature;

};// end of class definitions
 
 
#endif
