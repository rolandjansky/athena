/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for MuonSimData map
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: MuonSimDataCollection.h,v 1.1 2004-05-29 15:17:52 ketevi Exp $

#ifndef MUONSIMDATA_MUONSIMDATACOLLECTION_H
# define MUONSIMDATA_MUONSIMDATACOLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "MuonSimData/MuonSimData.h"
#include "Identifier/Identifier.h"
#include <map>

class MuonSimDataCollection : public std::map<Identifier,MuonSimData> {
 public:
  MuonSimDataCollection();
  virtual ~MuonSimDataCollection();

  // Helpers for easy access from python / debugger.
  Identifier id (size_t i) const;
  const MuonSimData* data (size_t i) const;
};
CLASS_DEF(MuonSimDataCollection, 41658935, 1)


#endif // MUONSIMDATA_MUONSIMDATACLASS_DEF_H

