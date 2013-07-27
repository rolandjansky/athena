/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_MDTDEADTUBESTATUSCONTAINER_H
#define MUONCONDDATA_MDTDEADTUBESTATUSCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "MuonCondData/MdtDeadTubeStatus.h"
class MdtDeadTubeStatusContainer : public DataVector<MdtDeadTubeStatus>
{
 public:
  MdtDeadTubeStatusContainer();
  ~MdtDeadTubeStatusContainer();

};

inline MdtDeadTubeStatusContainer::MdtDeadTubeStatusContainer():
  DataVector<MdtDeadTubeStatus>()
{ } 

inline MdtDeadTubeStatusContainer::~MdtDeadTubeStatusContainer()
{ } 

CLASS_DEF(MdtDeadTubeStatusContainer, 1229111234, 1)
#endif //MUONCONDCOOL_MDTDEADTUBESTATUSCONTAINER.H
