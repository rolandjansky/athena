/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_TrackSegmentsFinder_H
#define TRT_TrackSegmentsFinder_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "InDetRecToolInterfaces/ITRT_DetElementsRoadMaker.h" 
#include "StoreGate/DataHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
namespace InDet {


  // Class-algorithm for track finding in TRT
  // 
  class TRT_TrackSegmentsFinder : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      TRT_TrackSegmentsFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_TrackSegmentsFinder() {};
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     
      bool                                m_useCaloSeeds     ;
      int                                 m_outputlevel      ;  // Print level for debug
      int                                 m_nprint           ; // Kind of  print
      int                                 m_nsegments        ; // Number segments
      int                                 m_nsegmentsTotal   ; // Number segments
      int                                 m_minNumberDCs     ; // Min. number of DriftCircles

      SG::WriteHandle<Trk::SegmentCollection> m_foundSegments; // Name of segments location
      SG::ReadHandle<CaloClusterROI_Collection> m_calo;        // Calo clusters collection Handle
      
      ToolHandle<ITRT_TrackSegmentsMaker> m_segmentsMakerTool; // TRT segments maker
      ToolHandle<ITRT_DetElementsRoadMaker>     m_roadtool   ; // TRT road maker tool


      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const TRT_TrackSegmentsFinder&);
  std::ostream& operator << (std::ostream&,const TRT_TrackSegmentsFinder&); 
}
#endif // TRT_TrackSegmentsFinder_H
