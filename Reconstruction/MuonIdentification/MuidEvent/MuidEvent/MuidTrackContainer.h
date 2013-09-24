/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONIDENTIFICATION_MUIDTRACKCONTAINER_H
# define MUONIDENTIFICATION_MUIDTRACKCONTAINER_H


#ifndef CLIDSVC_CLASSDEF_H
# include "CLIDSvc/CLASS_DEF.h"
#endif
#include "DataModel/DataVector.h"
#include "MuidEvent/MuidTrack.h"

class MuidTrackContainer: public DataVector<MuidTrack>
{
public:
  MuidTrackContainer (void);
  
  ~MuidTrackContainer (void);
};

inline
MuidTrackContainer::MuidTrackContainer (void)
  :     DataVector<MuidTrack>  ()
{}

inline
MuidTrackContainer::~MuidTrackContainer (void)
{}

CLASS_DEF(MuidTrackContainer, 1117860785, 1)

#endif //
