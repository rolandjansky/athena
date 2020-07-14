/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXMULTIVERTEX_VXCONTAINER_H
#define VXMULTIVERTEX_VXCONTAINER_H

#include "AthContainers/DataVector.h"
#include "VxMultiVertex/MVFVxCandidate.h"
#include "AthenaKernel/CLASS_DEF.h"


/**
 *  @class   MVFVxContainer.h 
 *
 *  A top level class of the VxMultiVertex package.
 *  This container is designed for the case when 
 *  MVFVxCandidate objects alone would beed to be 
 *  stored. In the resent reconstruction, the 
 *  VxContainer is used to store any kind of
 *  Trk::VxCandidate.
 *
 *  begin   : 18-08-2006
 *  authors  Giacinto Piacquadio (Freiburg University)
 *             Kirill Prokofiev , Andreas Wildauer giacinto.piacquadio@physik.uni-freiburg.de 
 */



  class MVFVxContainer : public DataVector<Trk::MVFVxCandidate> 
  {
    public:
    MVFVxContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<Trk::MVFVxCandidate>(own) {} 
    virtual ~MVFVxContainer() {}
  };

/* Define CLID and version (not used for now) for VxContainer
   the CLID for "Tutorial objects" must be in the range 9900-9999
   http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/documentation/CLID1.txt */

//obtained through ./clid MVxContainer on 18-08-2006 
CLASS_DEF( MVFVxContainer,1943140, 1 )

#endif
