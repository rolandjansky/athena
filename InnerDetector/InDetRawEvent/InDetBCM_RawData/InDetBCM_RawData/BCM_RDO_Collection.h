/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RDO_Collection.h
//   Header file for class BCM_RDO_Collection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Collection class for BCM raw data objects
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
// Version 1.1 19/03/2008 Daniel Dobos for BCM
// Version 1.2 14/05/2008 Daniel Dobos
// Version 1.3 23/05/2008 Dominique Tardif
// Version 1.4 03/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef INDETBCM_RAWDATA_BCM_RDO_COLLECTION_H
#define INDETBCM_RAWDATA_BCM_RDO_COLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "InDetBCM_RawData/BCM_RawData.h"

class BCM_RDO_Collection : public DataVector<BCM_RawData> {

 public:
  BCM_RDO_Collection() : m_chanId(0) {}
  BCM_RDO_Collection(unsigned int chanId) : m_chanId(chanId) {}
  virtual ~BCM_RDO_Collection() {}
  virtual unsigned int getChannel() const { return m_chanId; }
  virtual void setChannel(unsigned int chanId) { m_chanId = chanId; }

 private:
  BCM_RDO_Collection(const BCM_RDO_Collection&);
  BCM_RDO_Collection &operator=(const BCM_RDO_Collection&);
  unsigned int m_chanId;

};

CLASS_DEF(BCM_RDO_Collection,2591,1)

#endif // INDETBCM_RAWDATA_BCM_RDO_COLLECTION_H
