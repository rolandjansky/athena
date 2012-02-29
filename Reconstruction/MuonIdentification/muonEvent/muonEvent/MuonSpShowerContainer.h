/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENT_MUONSPSHOWERCONTAINER_H
#define MUONEVENT_MUONSPSHOWERCONTAINER_H
//////////////////////////////////////////////////////////////////////////////////////
///
/// Name:     MuonSpContainer.h
/// Package : offline/Reconstruction/MuonIdentification/muonEvent
///
/// Authors:  K. A. Assamagan
/// Created:  July 2006
///
/// Purpose:  This is a data object, containing a collection of MuonSpShower objects
/////////////////////////////////////////////////////////////////////////////////////

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "muonEvent/MuonSpShower.h"

namespace Rec {
class MuonSpShowerContainer : public DataVector<MuonSpShower> {
 public:
  MuonSpShowerContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
	  : DataVector<MuonSpShower>(own) {}
  virtual ~MuonSpShowerContainer() {};

};

}

CLASS_DEF(Rec::MuonSpShowerContainer, 1333253135, 1)

#endif


