/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigTrackCollectionMerger_H
#define TrigTrackCollectionMerger_H

#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"

//trigger stuff
#include "TrigInterfaces/FexAlgo.h"



namespace Trk {

  //forward declarations
  class IPRD_AssociationTool;
  class ITrackSummaryTool;

  /** Class-algorithm for track collection merging and removalof potential duplicate tracks. */
  class TrigTrackCollectionMerger : public HLT::FexAlgo 
    {
    
      ///////////////////////////////////////////////////////////////////
      /** Public methods:                                              */
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** Standard Algotithm methods:                                  */
      ///////////////////////////////////////////////////////////////////

      TrigTrackCollectionMerger(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TrigTrackCollectionMerger() {}
      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
      HLT::ErrorCode hltFinalize();

      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status.                   */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      /** Protected data:                                              */
      ///////////////////////////////////////////////////////////////////
      ToolHandle< Trk::IPRD_AssociationTool >                      m_assoTool          ;  /** association tool for PRDs */ 
      ToolHandle< Trk::ITrackSummaryTool >                         m_trkSummaryTool    ;  /** summary tool with shared hits enabled */

      ///////////////////////////////////////////////////////////////////
      /** Protected methods:                                           */
      ///////////////////////////////////////////////////////////////////

      /** A routine that merges the track collections. */
      HLT::ErrorCode    mergeTrack(const TrackCollection* trackCol, TrackCollection* outputCol);

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    private:

    };
  MsgStream&    operator << (MsgStream&   ,const TrigTrackCollectionMerger&);
  std::ostream& operator << (std::ostream&,const TrigTrackCollectionMerger&); 
}
#endif // TrigTrackCollectionMerger_H
