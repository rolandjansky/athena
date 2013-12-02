/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for CscSimData map
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: CscSimDataCollection.h,v 1.1 2007-03-27 15:51:48 dadams Exp $

#ifndef MUONSIMDATA_CSCSIMDATACOLLECTION_H
#define MUONSIMDATA_CSCSIMDATACOLLECTION_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "CLIDSvc/CLASS_DEF.h"
#include "MuonSimData/CscSimData.h"
#include "Identifier/Identifier.h"
#include <map>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class CscSimDataCollection : public std::map<Identifier,CscSimData> {
public:
  CscSimDataCollection();
  virtual ~CscSimDataCollection();
};
CLASS_DEF(CscSimDataCollection, 1095642046, 1)


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // MUONSIMDATA_CSCSIMDATACOLLECTION_H

