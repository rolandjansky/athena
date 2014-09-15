/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToTrackTool_H
#define MuonboyToTrackTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "MboyEDMToolInterfaces/IMuonboyToTrackTool.h"

#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"

#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class  FitQuality;
  class  RIO_OnTrack;
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

class MB_Track_MboyCyl ;

namespace Muon {
  class IMboyTrackUndressingTool ;
  class IMboySortingTool ;
  class IMboyTrackRepresentationTransformTool ;
  class IMboyProperTrackRepresentationTransformTool ;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

namespace Trk {
  class IMuonboyToTrackParametersTool;
  class IMbMuScToTSOSTool ;
}

class IMboyEDMHelper ;
class MB_TrackAth ;
class MB_TrackStoreAth;
class MbPOT;
class MbIOObject ;

namespace Trk {

  /**
   @class MuonboyToTrackTool

   This tools converts Muonboy tracks into Trk::Track
   
  @author samusog@cern.ch
  
  */

class MuonboyToTrackTool: public AthAlgTool, virtual public IMuonboyToTrackTool{
public:
    MuonboyToTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToTrackTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Convert collection */
   virtual StatusCode convertCollection(
                                const MB_TrackStoreAth* pMB_TrackStoreAth,
                                TrackCollection* pTrackCollection,
                                TrackCollection* pTrackCollection_MSonly
                               );
   virtual StatusCode convertIndividualAtIP(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               );
   virtual StatusCode convertIndividualAtMS(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               );
   /** Get set of TOS  */
   virtual DataVector<const Trk::TrackStateOnSurface>* GiveAllTosForFullTrack(const MB_TrackAth* pMB_TrackAth,int AddCaloEntranceToMain,int AddMuonSystemEntranceToMain);

   /** Get set of TOS  at Spectro entrance only*/
   virtual DataVector<const Trk::TrackStateOnSurface>* GiveAllTosForMuSysTrack(const MB_TrackAth* pMB_TrackAth,int AddUnMeasuredPerigeeeToMSOnly);


private:
///////////////////////////////////
//Functions
   
   /** Convert single object */
   Trk::Track* convert (const MB_TrackAth* pMB_TrackAth);

   /** Convert single object keeping only representation at Spectro entrance*/
   Trk::Track* convertAtMuonSystemEntrance  (const MB_TrackAth* pMB_TrackAth) ;

   /** Building set of TOS for the ROT */
   void AddRotToss(int Selection, DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurfaceS, const MB_TrackAth* pMB_TrackAth);
   
   /** Return TOS */
   const Trk::TrackStateOnSurface* GiveTSOS(const MB_TrackAth* pMB_TrackAth, int iDig);
   const Trk::TrackStateOnSurface* GiveTSOSforMbROT (const MB_TrackAth* pMB_TrackAth,int iDig);
   const Trk::TrackStateOnSurface* GiveTSOSforMbHole(const MbPOT* pMbPOT);

   /** Return a RIO_OnTrack */
   const Trk::RIO_OnTrack* GiveROTforMbROT(const MB_TrackAth* pMB_TrackAth,int iDig,const TrackParameters*& pTrackParameters);
   const Trk::RIO_OnTrack* MdtROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::MdtPrepData*      pPrepData);
   const Trk::RIO_OnTrack* RpcROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::RpcPrepData*      pPrepData);
   const Trk::RIO_OnTrack* TgcROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::TgcPrepData*      pPrepData);
   const Trk::RIO_OnTrack* CscROT   (const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscStripPrepData* pPrepData);
   const Trk::RIO_OnTrack* CscCluROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscPrepData*      pPrepData);

   /** check distance to the surface of the PrepRawData */
   void CheckAdequation( const Trk::Surface* pSurface, const Amg::Vector3D& GP ) ;
                                  
   /** determine if the track is on the Spectro entrance cylinder */
   int IsOnTheCylinder(const MB_TrackAth* pMB_TrackAth);
   int IsOnTheCylinder(const MB_Track_MboyCyl* pMB_Track_MboyCyl) ;

   /** return FitQuality */
   const Trk::FitQuality*  GiveFitQuality(const MB_TrackAth* pMB_TrackAth);

   /** Sort */
   void SortElements(
                  const MB_TrackAth* pMB_TrackAth,
                  std::vector<int>&  LinearIndexSorted ,
                  std::vector<int>&  AreaTagSorted ,
                  double XrefStart,
                  double YrefStart,
                  double ZrefStart
                 );
     
     
//Data

   int m_DoMdtConvertion ; //!< Control conversion of Mdt
   int m_DoRpcConvertion ; //!< Control conversion of Rpc
   int m_DoTgcConvertion ; //!< Control conversion of Tgc
   int m_DoCscConvertion ; //!< Control conversion of Csc
   
   int m_DoMbMuSc ; //!< Control production of Scattering planes
   
   int m_DoMdtHole ; //!< Control production of Mdt hole
   int m_DoRpcHole ; //!< Control production of Rpc hole
   int m_DoTgcHole ; //!< Control production of Tgc hole
   int m_DoCscHole ; //!< Control production of Csc hole
   
   int m_AddMuonSystemEntranceToMain   ; //!< Control addition of Muon at Spectro entrance on main track
   int m_AddCaloEntranceToMain         ; //!< Control addition of Muon at Calo    entrance on main track
   int m_AddUnMeasuredPerigeee         ; //!< Control addition of Muon Perigeee representatuion on main track
   int m_AddUnMeasuredPerigeeeToMSOnly ; //!< Control addition of Muon Perigeee representatuion on Spectro entrance only track
   
   int m_AddTrackParametersOnRotTSOS ; //!< Control addition of TrackParameters On Rot TSOSs
   
   int m_DoCorrectPosition ; //!< Control correction for small distance to PreRawData
   
   int m_DoDbg ; //!< Control special debug
   
   ToolHandle< Trk::IMbMuScToTSOSTool > p_IMbMuScToTSOSTool ; //!< Pointer On IMbMuScToTSOSTool
   ToolHandle< Muon::IMboyProperTrackRepresentationTransformTool > p_IMboyProperTrackRepresentationTransformTool ; //!< Pointer On IMboyProperTrackRepresentationTransformTool
   ToolHandle< Muon::IMboyTrackRepresentationTransformTool > p_IMboyTrackRepresentationTransformTool ; //!< Pointer On IMboyTrackRepresentationTransformTool
   ToolHandle< Muon::IMboySortingTool > p_IMboySortingTool ; //!< Pointer On IMboySortingTool
   ToolHandle< Muon::IMboyTrackUndressingTool > p_IMboyTrackUndressingTool ; //!< Pointer On IMboyTrackUndressingTool
   ToolHandle< Muon::IMdtDriftCircleOnTrackCreator > p_IMdtDriftCircleOnTrackCreator ; //!< Pointer On IMdtDriftCircleOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorRPC        ; //!< Pointer On IMuonClusterOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorTGC        ; //!< Pointer On IMuonClusterOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorCSCStrip   ; //!< Pointer On IMuonClusterOnTrackCreator
   ToolHandle< Muon::IMuonClusterOnTrackCreator >    p_IMuonClusterOnTrackCreatorCSCCluster ; //!< Pointer On IMuonClusterOnTrackCreator

   int m_AddPerigeeAtBeamSpot ; //!< Control addition of Perigee at beam spot
   ToolHandle< IMboyEDMHelper > p_OwnEDMHelper ; //!< Pointer On IMboyEDMHelper
   ToolHandle< Trk::IMuonboyToTrackParametersTool > p_IMuonboyToTrackParametersTool ; //!< Pointer On IMuonboyToTrackParametersTool

   int m_TEMPORARYNewTrackInfoEnumUse ; //!< Control use of new TrackInfo enum
   
   ToolHandle<Muon::MuonIdHelperTool>   m_idHelper;
   
};
}

#endif

