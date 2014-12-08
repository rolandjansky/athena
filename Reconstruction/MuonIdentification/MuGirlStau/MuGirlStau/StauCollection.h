/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STAUCOLLECTION_H
#define STAUCOLLECTION_H

#include "DataModel/DataVector.h"
#include "MuGirlStau/StauContainer.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"

/**This typedef represents a collection of Trk::Track objects. 
 It is a DataVector. It can be saved
 to storegate and persistified using POOL*/

/*using namespace MuGirlNS;

 typedef DataVector<MuGirlNS::StauContainer> StauCollection;
 */
//namespace MuGirlNS
//{
class StauCollection: public DataVector<MuGirlNS::StauContainer>
{
};
//////{
//////public:
//////   StauCollection();
//////   virtual ~StauCollection();//{};
//////};
//}
//CLASS_DEF(MuGirlNS::StauCollection,661236666, 1)
SG_BASE( StauCollection, DataVector< MuGirlNS::StauContainer > );
CLASS_DEF(StauCollection,144456789, 1)
//CLASS_DEF(MuGirlNS::StauCollection,"EDD81879-EAD9-4B8E-974A-F1E73DE361F7", 1)

#endif
