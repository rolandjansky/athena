/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToSegmentTool_H
#define MuonboyToSegmentTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//Can not be forwarded due to typedef
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

#include "MboyEDMToolInterfaces/IMuonboyToSegmentTool.h"

#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"

#include "Identifier/Identifier.h"

namespace Trk {
  class  FitQuality;
  class  RIO_OnTrack;
}

namespace Muon {
  class  MuonSegment;
}

namespace Muon {
  class  IMdtDriftCircleOnTrackCreator;
  class  IMuonClusterOnTrackCreator;
  class  CscPrepData;
  class  MdtPrepData;
  class  RpcPrepData;
  class  TgcPrepData;
  class  CscStripPrepData;
  class MuonIdHelperTool;
}

namespace Muon {
class IMboySortingTool ;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

namespace Trk {
  class IMuonboyToTrackParametersTool ;
}
class IMuonboyToIdentifier;

class MB_SegmentAth;
class MB_SegmentStoreAth;
class MbPOT;
class MbIOObject ;

namespace Trk {

  /**
   @class MuonboyToSegmentTool

   This tool converts Muonboy segments into Trk::Segment
   
  @author samusog@cern.ch
  
  */

class MuonboyToSegmentTool:public AthAlgTool, virtual public IMuonboyToSegmentTool{
public:
    MuonboyToSegmentTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToSegmentTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Convert collection */
   virtual StatusCode convertCollection(
                                const MB_SegmentStoreAth* pMB_SegmentStoreAth,
			        Trk::SegmentCollection* pSegmentCollection
                                 , int Clean = 1
                               );


   /** Convert collection */
   virtual std::vector<const Muon::MuonSegment*>* convertCollection(
                                const MB_SegmentStoreAth* pMB_SegmentStoreAth
                                 , int Clean = 1
                               );

private:
///////////////////////////////////
//Functions

   /** Convert single object */
   Trk::Segment* convert (const MB_SegmentAth* pMB_SegmentAth);

   /** Convert single object */
   Muon::MuonSegment* convertToMuonSegment (const MB_SegmentAth* pMB_SegmentAth);

   /** Return a RIO_OnTrack */
   const Trk::RIO_OnTrack* GiveRots(const MB_SegmentAth* pMB_SegmentAth,int iDig);

   /** Return a RIO_OnTrack */
   const Trk::RIO_OnTrack* GiveROTforMbROT(const MB_SegmentAth* pMB_SegmentAth,int iDig,const TrackParameters*& pTrackParameters);
   const Trk::RIO_OnTrack* MdtROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::MdtPrepData*      pPrepData);
   const Trk::RIO_OnTrack* RpcROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::RpcPrepData*      pPrepData);
   const Trk::RIO_OnTrack* TgcROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::TgcPrepData*      pPrepData);
   const Trk::RIO_OnTrack* CscROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscStripPrepData* pPrepData);
   const Trk::RIO_OnTrack* CscCluROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscPrepData*      pPrepData);

   /** check distance to the surface of the PrepRawData */
   void CheckAdequation( const Trk::Surface* pSurface, const Amg::Vector3D& GP ) ;
                                  
   /** return FitQuality */
   Trk::FitQuality*  GiveFitQuality(const MB_SegmentAth* pMB_SegmentAth, std::vector<Identifier>& holeVec);

   /** Check validity of segment */
   int   RecSeg_IsOk(const MB_SegmentAth* pMB_SegmentAth);

   /** Sort */
   void SortElements(
                  const MB_SegmentAth* pMB_SegmentAth,
                  std::vector<int>&  LinearIndexSorted ,
                  std::vector<int>&  AreaTagSorted ,
                  double XrefStart,
                  double YrefStart,
                  double ZrefStart
                 );

   /** Compute CSC segmet t0 */
   void T0CSCSegment(const Muon::MuonSegment* pMuonSegment, float& t0, float& t0Error);
    
//Data

   int m_DoMdtConvertion ; //!< Control conversion of Mdt
   int m_DoRpcConvertion ; //!< Control conversion of Rpc
   int m_DoTgcConvertion ; //!< Control conversion of Tgc
   int m_DoCscConvertion ; //!< Control conversion of Csc
   
   int m_DoMdtHole ; //!< Control production of Mdt hole
   int m_DoRpcHole ; //!< Control production of Rpc hole
   int m_DoTgcHole ; //!< Control production of Tgc hole
   int m_DoCscHole ; //!< Control production of Csc hole
   
// Cut on segments

   int   m_LetCSCGo ; //!< Control production of CSC segment

   int   m_ApplyQualityFactorCut ; //!< Control cut of quality factor
   float m_QualityFactorCut      ; //!< Value of cut of quality factor

   int   m_ApplyNberofDigitsCut ; //!< Control cut of nber of digits
   int   m_NberofDigitsCut      ; //!< Value of cut of nber of digits
   
   int   m_ApplyNberofPrecisonDigitsCut ; //!< Control cut of nber of precision digits
   int   m_NberofPrecisonDigitsCut      ; //!< Value of cut of nber of precision digits
   
   int m_DoCorrectPosition ; //!< Control correction for small distance to PreRawData
   
   int m_DoDbg ; //!< Control special debug
   
   ToolHandle< Muon::IMboySortingTool > p_IMboySortingTool ; //!< Pointer On IMboySortingTool
   ToolHandle< Muon::IMdtDriftCircleOnTrackCreator > p_IMdtDriftCircleOnTrackCreator        ; //!< Pointer On IMdtDriftCircleOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorRPC        ; //!< Pointer On IMuonClusterOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorTGC        ; //!< Pointer On IMuonClusterOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorCSCStrip   ; //!< Pointer On IMuonClusterOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorCSCCluster ; //!< Pointer On IMuonClusterOnTrackCreator

   ToolHandle< Trk::IMuonboyToTrackParametersTool > p_IMuonboyToTrackParametersTool ; //!< Pointer On IMuonboyToTrackParametersTool
   ToolHandle< IMuonboyToIdentifier > p_IMuonboyToIdentifier ; //!< Pointer On IMuonboyToIdentifier
   
   ToolHandle<Muon::MuonIdHelperTool>   m_idHelper;   
   int   m_Clean ; //!< Internal control on cleaning
   
};
}

#endif

