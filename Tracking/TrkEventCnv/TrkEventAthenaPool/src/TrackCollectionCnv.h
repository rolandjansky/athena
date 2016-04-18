/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef TRKEVENTATHENAPOOL_TRACKCOLLECTIONCNV_H
#define TRKEVENTATHENAPOOL_TRACKCOLLECTIONCNV_H

// Hack so we can access the private data. EJWM
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/AthenaPoolCnvTPExtension.h"

#include "TrkTrack/TrackCollection.h"

#include "TrkEventTPCnv/TrackCollectionCnv_tlp1.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp2.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp3.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp4.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"

//-----------------------------------------------------------------------------
// Base class definition - defined which is the to-be-used persistent version
//-----------------------------------------------------------------------------

typedef Trk::TrackCollection_tlp5   TrackCollection_PERS;
typedef TrackCollectionCnv_tlp5   TrackCollectionCNV_PERS;

typedef T_AthenaPoolCustomCnv<TrackCollection, TrackCollection_PERS>  TrackCollectionCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class TrackCollectionCnv
   : public TrackCollectionCnvBase,
     public AthenaPoolCnvTPExtension
{
friend class CnvFactory<TrackCollectionCnv>;

protected:
  TrackCollectionCnv( ISvcLocator *svcloc );
  ~TrackCollectionCnv();
  virtual StatusCode initialize();

  virtual TrackCollection_PERS *createPersistent( TrackCollection *transCont);
  virtual TrackCollection      *createTransient();

  virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return & m_TPConverterForPER; }



private:
  void    updateLog(); //!< This method modifies m_log to indicate the current key being converted
    
  IMessageSvc*              m_msgSvc;
  MsgStream                 m_log;

  TrackCollectionCnv_tlp1   m_TPConverter_tlp1;
  TrackCollectionCnv_tlp2   m_TPConverter_tlp2;
  TrackCollectionCnv_tlp3   m_TPConverter_tlp3;
  TrackCollectionCnv_tlp4   m_TPConverter_tlp4;
  TrackCollectionCNV_PERS   m_TPConverterForPER;
};

#endif // TRKEVENTATHENAPOOL_TRACKCOLLECTIONCNV_H
