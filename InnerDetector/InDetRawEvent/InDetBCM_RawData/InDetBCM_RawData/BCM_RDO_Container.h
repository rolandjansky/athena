/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RDO_Container.h
//   Header file for class BCM_RDO_Container
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container class for BCM raw data objects
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
// Version 1.1 19/03/2008 Daniel Dobos for BCM
// Version 1.2 14/05/2008 Daniel Dobos
// Version 1.3 23/05/2008 Dominique Tardif
// Version 1.4 03/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef INDETBCM_RAWDATA_BCM_RDO_CONTAINER_H
#define INDETBCM_RAWDATA_BCM_RDO_CONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"

class BCM_RDO_Container : public DataVector<BCM_RDO_Collection> {

 public:
  BCM_RDO_Container() {}
  ~BCM_RDO_Container() {}

 private:
  BCM_RDO_Container(const BCM_RDO_Container&);
  BCM_RDO_Container &operator=(const BCM_RDO_Container&);

};

CLASS_DEF(BCM_RDO_Container,2590,1)

#endif // INDETBCM_RAWDATA_BCM_RDO_CONTAINER_H
