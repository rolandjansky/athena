/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureContainer_p1
 *
 * @brief persistent partner for IsoMuonFatureContainer
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureContainer_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class IsoMuonFeatureContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  IsoMuonFeatureContainer_p1(){}
  friend class IsoMuonFeatureContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_isoMuonFeature;

};// end of class definitions
 
 
#endif
