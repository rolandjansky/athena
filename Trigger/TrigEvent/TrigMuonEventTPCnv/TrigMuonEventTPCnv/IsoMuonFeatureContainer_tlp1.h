/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureContainer_tlp1
 *
 * @brief "top-level" persistent partner for IsoMuonFeatureContainer
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureContainer_tlp1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"

class IsoMuonFeatureContainer_tlp1 
{
   
 public:
   
  IsoMuonFeatureContainer_tlp1(){}
 
  std::vector<IsoMuonFeatureContainer_p1>       m_isoMuonFeatureContainerVec;
  std::vector<IsoMuonFeature_p1>                m_isoMuonFeatureVec;
     
};//end of class definitions
 
#endif
