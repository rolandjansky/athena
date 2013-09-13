/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureContainer_p2
 *
 * @brief persistent partner for MuonFeatureContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeatureContainer_p2.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_P2_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class MuonFeatureContainer_p2 : public std::vector<TPObjRef>
{
 public:
    
  MuonFeatureContainer_p2(){}
  friend class MuonFeatureContainerCnv_p2;

 private:

  std::vector<TPObjRef> m_muonFeature;

};// end of class definitions
 
 
#endif
