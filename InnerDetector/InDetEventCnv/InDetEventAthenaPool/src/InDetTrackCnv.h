/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_TRACKCNV_H
#define INDET_TRACKCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.h"

#include "InDetEventTPCnv/InDetTrackCnv_tlp1.h"
#include "InDetEventTPCnv/InDetTrackCnv_tlp2.h"

#include "InDetTrack.h"


//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef InDet::Track_tlp2	InDetTrack_PERS;
typedef T_AthenaPoolExtendingCnv<InDetTrack, InDetTrack_PERS>       InDetTrackCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class InDetTrackCnv : public InDetTrackCnvBase
{
friend class CnvFactory<InDetTrackCnv>;

protected:
  InDetTrackCnv( ISvcLocator *svcloc );

  virtual StatusCode initialize();


  virtual AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv() { return &m_TPConverter2; }

  virtual AthenaPoolCnvTPExtension*  clone() { return new InDetTrackCnv(0); }

  virtual void       readObjectFromPool( const std::string& );

  
private:  
  InDetTrackCnv_tlp1		m_TPConverter1;
  InDetTrackCnv_tlp2		m_TPConverter2;
};

#endif

