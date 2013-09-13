/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureContainer_tlp2
 *
 * @brief "top-level" persistent partner for MuonFeatureContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeatureContainer_tlp2.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_TLP2_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_TLP2_H

#include "TrigMuonEventTPCnv/MuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeature_p2.h"

class MuonFeatureContainer_tlp2 
{
   
 public:
   
  MuonFeatureContainer_tlp2(){}
 
  std::vector<MuonFeatureContainer_p2>       m_muonFeatureContainerVec;
  std::vector<MuonFeature_p2>                m_muonFeatureVec;
     
};//end of class definitions
 
#endif
