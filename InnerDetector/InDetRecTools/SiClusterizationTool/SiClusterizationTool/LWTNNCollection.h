/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LWTNNCollection_H_
#define _LWTNNCollection_H_

//#include <vector>
#include "lwtnn/LightweightGraph.hh"

class LWTNNCollection 
  :  public std::map<int, std::unique_ptr<lwt::LightweightGraph> >
{
public:

private:

};

// These values produced using clid script.
// clid LWTNNCollection
// 1196174442 LWTNNCollection None
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(LWTNNCollection, 1196174442, 1)
// clid -cs LWTNNCollection
// 1226994220
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(LWTNNCollection, 1226994220);

#endif
