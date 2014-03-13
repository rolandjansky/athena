/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef VXVERTEX_VXCONTAINER_H
#define VXVERTEX_VXCONTAINER_H

#include "DataModel/DataVector.h"
#include "VxVertex/VxCandidate.h"
#include "CLIDSvc/CLASS_DEF.h"
/**
 * @class VxContainer.h  
 *      
 * A container for Trk::VxCandidates and extensions 
 * to be written in the StoreGate. This is a top level
 * object of the VxVertex package.
 *                     
 *   begin   : 29-01-2004
 *   <br>
 *   changes : 17-02-2004 AW Moved this file from
 *                          InnerDetector/InDetRecAlgs/InDetPriVxFinder
 *                           to
 *                           InnerDetector/InDetRecEvent/InDetPriVx
 *  @authors Andreas Wildauer (CERN PH-ATC) andreas.wildauer@cern.ch
 */
  class VxContainer : public DataVector<Trk::VxCandidate> 
  {
    public:
    VxContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<Trk::VxCandidate>(own) {} 
    virtual ~VxContainer() {}
  };

/* Define CLID and version (not used for now) for VxContainer
   the CLID for "Tutorial objects" must be in the range 9900-9999
   http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/documentation/CLID1.txt */

/* CLID number as generated with
clid.py InDetPriVx
in RecExCommon 01-58 
9327453 InDetPriVx
*/
  
CLASS_DEF( VxContainer, 9327453, 1 )

#endif
