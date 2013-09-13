/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureContainer_tlp1
 *
 * @brief "top-level" persistent partner for MuonFeatureContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeatureContainer_tlp1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/MuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigMuonEventTPCnv/MuonFeature_p2.h"

class MuonFeatureContainer_tlp1 
{
   
 public:
   
  MuonFeatureContainer_tlp1(){}
 
  std::vector<MuonFeatureContainer_p1>       m_muonFeatureContainerVec;
  std::vector<MuonFeature_p1>                m_muonFeatureVec;
  std::vector<MuonFeature_p2>                m_muonFeatureVec_p2;
     
};//end of class definitions
 
#endif
