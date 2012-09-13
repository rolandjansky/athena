/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for InDetSimData map
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: InDetSimDataCollection.h,v 1.3 2004-04-05 23:16:07 costanzo Exp $

#ifndef INDETSIMDATA_INDETSIMDATACOLLECTION_H
# define INDETSIMDATA_INDETSIMDATACOLLECTION_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "CLIDSvc/CLASS_DEF.h"
#include "InDetSimData/InDetSimData.h"
#include "Identifier/Identifier.h"
#include <map>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class InDetSimDataCollection : public std::map<Identifier,InDetSimData> {
 public:
  InDetSimDataCollection();
  virtual ~InDetSimDataCollection();
};
CLASS_DEF(InDetSimDataCollection, 2543, 1)


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // INDETSIMDATA_INDETSIMDATACLASS_DEF_H
