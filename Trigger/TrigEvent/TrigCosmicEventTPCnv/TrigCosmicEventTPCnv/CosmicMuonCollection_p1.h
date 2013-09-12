/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : CosmicMuonCollection_p1
 *
 * @brief persistent partner for CosmicMuonCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: CosmicMuonCollection_p1.h,v 1.2 2009-04-01 22:02:51 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_COSMICMUONCONTAINER_P1_H
#define TRIGCOSMICEVENTTPCNV_COSMICMUONCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class CosmicMuonCollection_p1 : public std::vector<TPObjRef>
{
 public:
    
  CosmicMuonCollection_p1(){}
  friend class CosmicMuonCollectionCnv_p1;

 private:

  std::vector<TPObjRef> m_cosmicMuon;

};// end of class definitions
 
 
#endif
