/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SegmentCollectionCnv.h
// author: Andreas Wildauer <andreas.wildauer@cern.ch>
// 
//-----------------------------------------------------------------------------

#ifndef TRKEVENTATHENAPOOL_SEGMENTCOLLECTIONCNV_H
#define TRKEVENTATHENAPOOL_SEGMENTCOLLECTIONCNV_H

#include "GaudiKernel/MsgStream.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/AthenaPoolCnvTPExtension.h"

#include "TrkSegment/SegmentCollection.h"

#include "TrkEventTPCnv/SegmentCollectionCnv_tlp1.h"
#include "TrkEventTPCnv/SegmentCollectionCnv_tlp2.h"
#include "TrkEventTPCnv/SegmentCollectionCnv_tlp3.h"
#include "TrkEventTopLevelCnv/SegmentCollectionCnv_tlp4.h"

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Trk::SegmentCollection_tlp4 SegmentCollection_PERS;
typedef SegmentCollectionCnv_tlp4 SegmentCollectionCNV_PERS;

typedef T_AthenaPoolCustomCnv<Trk::SegmentCollection, SegmentCollection_PERS> SegmentCollectionCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class SegmentCollectionCnv
   : public SegmentCollectionCnvBase,
     public AthenaPoolCnvTPExtension
{
friend class CnvFactory<SegmentCollectionCnv>;

public:
  SegmentCollectionCnv( ISvcLocator *svcloc );

protected:
  virtual SegmentCollection_PERS *createPersistent( Trk::SegmentCollection *transCont) override;
  virtual Trk::SegmentCollection *createTransient() override;

  virtual AthenaPoolTopLevelTPCnvBase* getTopLevelTPCnv() override { return &m_TPConverterForPER; }

private:
  void    initializeOldExtConverters();  //!< setup old extended converters when reading old data

  IMessageSvc           *m_msgSvc;
  MsgStream             m_log;
  bool                  m_oldExtCnvInitialized = false;

  SegmentCollectionCnv_tlp1 m_TPConverter_tlp1;
  SegmentCollectionCnv_tlp2 m_TPConverter_tlp2;
  SegmentCollectionCnv_tlp3 m_TPConverter_tlp3;
  SegmentCollectionCNV_PERS m_TPConverterForPER;
};

#endif // TRKEVENTATHENAPOOL_SEGMENTCOLLECTIONCNV_H



