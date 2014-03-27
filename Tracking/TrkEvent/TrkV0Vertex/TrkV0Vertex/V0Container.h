/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *   @class V0Container.h  
 *  begin   : 20-07-2005
 * A top level object of the TrkV0Vertex package.
 * This container is normally written to the store gate after 
 * the V0 reconstruction is finished.
 *   @authors  Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University), e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
 */

#ifndef TRKV0VERTEX_V0CONTAINER_H
#define TRKV0VERTEX_V0CONTAINER_H

#include "DataModel/DataVector.h"
#include "TrkV0Vertex/V0Candidate.h"
#include "CLIDSvc/CLASS_DEF.h"

  class V0Container : public DataVector<Trk::V0Candidate> 
  {
    public:
    V0Container(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<Trk::V0Candidate>(own) {} 
    virtual ~V0Container() {}
  };

CLASS_DEF( V0Container, 1317684620, 1 )

#endif
