/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : CosmicMuonCollection_tlp1
 *
 * @brief "top-level" persistent partner for CosmicMuonCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: CosmicMuonCollection_tlp1.h,v 1.2 2009-04-01 22:02:51 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_COSMICMUONCONTAINER_TLP1_H
#define TRIGCOSMICEVENTTPCNV_COSMICMUONCONTAINER_TLP1_H

#include "TrigCosmicEventTPCnv/CosmicMuonCollection_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuon_p1.h"

class CosmicMuonCollection_tlp1 
{
   
 public:
   
  CosmicMuonCollection_tlp1(){}
 
  std::vector<CosmicMuonCollection_p1>      m_cosmicMuonCollectionVec;
  std::vector<CosmicMuon_p1>                m_cosmicMuonVec;
     
};//end of class definitions
 
#endif
