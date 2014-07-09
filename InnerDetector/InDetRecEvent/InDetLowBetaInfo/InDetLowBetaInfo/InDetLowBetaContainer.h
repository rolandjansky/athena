/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETLOWBETACONTAINER_H
#define INDETLOWBETACONTAINER_H

#include "DataModel/DataVector.h"
#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"
#include "CLIDSvc/CLASS_DEF.h"


/**
 * @class InDetLowBetaContainer.h  
 *      
 * A container for InDet::InDetLowBetaInfo 
 * to be written in the StoreGate. 
 * 
 *  @authors Christopher Marino 
 */
namespace InDet {
  class InDetLowBetaContainer: public DataVector<InDet::InDetLowBetaCandidate> 
  {
  public:
    InDetLowBetaContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<InDet::InDetLowBetaCandidate>(own) {} 
      virtual ~InDetLowBetaContainer() {}
  };

}
/* CLID number as generated with
clid InDetLowBetaContainer
1286706128 InDetLowBetaContainer None 
*/
//   CLASS_DEF( InDetLowBetaContainer,1286706128  , 1 )
CLASS_DEF( InDet::InDetLowBetaContainer,1096598165, 1 )



#endif
