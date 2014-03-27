/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToTrackTool_H
#define IMuonboyToTrackTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkTrack/TrackStateOnSurface.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MB_TrackStoreAth;
class MB_TrackAth;

static const InterfaceID IID_IMuonboyToTrackTool("Trk::IMuonboyToTrackTool", 1, 0);

namespace Trk {

  /**
   @class IMuonboyToTrackTool

   This tools converts Muonboy tracks into Trk::Track
   
  @author samusog@cern.ch
  
  */

class IMuonboyToTrackTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Convert collection */
   virtual StatusCode convertCollection(
                                const MB_TrackStoreAth* pMB_TrackStoreAth,
                                TrackCollection* pTrackCollection,
                                TrackCollection* pTrackCollection_MSonly
                               ) = 0 ;
   virtual StatusCode convertIndividualAtIP(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               ) = 0 ;
   virtual StatusCode convertIndividualAtMS(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               ) = 0 ;
   /** Get set of TOS  */
   virtual DataVector<const Trk::TrackStateOnSurface>* GiveAllTosForFullTrack(const MB_TrackAth* pMB_TrackAth,int AddCaloEntranceToMain,int AddMuonSystemEntranceToMain) = 0 ;

   /** Get set of TOS  at Spectro entrance only*/
   virtual DataVector<const Trk::TrackStateOnSurface>* GiveAllTosForMuSysTrack(const MB_TrackAth* pMB_TrackAth,int AddUnMeasuredPerigeeeToMSOnly) = 0 ;

   
};

inline const InterfaceID& Trk::IMuonboyToTrackTool::interfaceID() { return IID_IMuonboyToTrackTool;}

}

#endif

