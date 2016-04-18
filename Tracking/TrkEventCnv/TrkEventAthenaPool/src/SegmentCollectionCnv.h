/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SegmentCollectionCnv.h
// author: Andreas Wildauer <andreas.wildauer@cern.ch>
// 
//-----------------------------------------------------------------------------

#ifndef TRKEVENTATHENAPOOL_SEGMENTCOLLECTIONCNV_H
#define TRKEVENTATHENAPOOL_SEGMENTCOLLECTIONCNV_H

// Hack so we can access the private data. EJWM
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/AthenaPoolCnvTPExtension.h"

#include "TrkSegment/SegmentCollection.h"

#include "TrkEventTPCnv/SegmentCollectionCnv_tlp1.h"
#include "TrkEventTPCnv/SegmentCollectionCnv_tlp2.h"
#include "TrkEventTPCnv/SegmentCollectionCnv_tlp3.h"

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Trk::SegmentCollection_tlp3 SegmentCollection_PERS;
typedef SegmentCollectionCnv_tlp3 SegmentCollectionCNV_PERS;

typedef T_AthenaPoolCustomCnv<Trk::SegmentCollection, SegmentCollection_PERS> SegmentCollectionCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class SegmentCollectionCnv
   : public SegmentCollectionCnvBase,
     public AthenaPoolCnvTPExtension
{
friend class CnvFactory<SegmentCollectionCnv>;

protected:
  SegmentCollectionCnv( ISvcLocator *svcloc );
  ~SegmentCollectionCnv();
  virtual StatusCode initialize();

  virtual SegmentCollection_PERS *createPersistent( Trk::SegmentCollection *transCont);
  virtual Trk::SegmentCollection *createTransient();

  virtual AthenaPoolTopLevelTPCnvBase* getTopLevelTPCnv() { return &m_TPConverterForPER; }

private:
  void    updateLog(); //!< This method modifies m_log to indicate the current key being converted

  IMessageSvc           *m_msgSvc;
  MsgStream             m_log;

  SegmentCollectionCnv_tlp1 m_TPConverter_tlp1;
  SegmentCollectionCnv_tlp2 m_TPConverter_tlp2;
  SegmentCollectionCNV_PERS m_TPConverterForPER;
};

#endif // TRKEVENTATHENAPOOL_SEGMENTCOLLECTIONCNV_H
