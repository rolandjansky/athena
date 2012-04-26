/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKATHTRACKCONTAINER_H
#define FTKATHTRACKCONTAINER_H

#include "DataModel/DataVector.h"
#include "TrigFTKPool/FTKAthTrack.h"

class FTKAthTrackContainer : public DataVector<FTKAthTrack> {
public :
  FTKAthTrackContainer(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS) :
    DataVector<FTKAthTrack>(ownPolicy) {};
  virtual ~FTKAthTrackContainer() {};
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(FTKAthTrackContainer,67458,1)

#endif // FTKATHTRACKCONTAINER_H
