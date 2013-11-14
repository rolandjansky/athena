/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboySvc_H
#define IMboySvc_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/IOVSvcDefs.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Can not be forwarded due to typedef
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Rec {
  class  TrackParticleContainer;
}

namespace Muon {
  class  MuonSegment;
}

namespace Trk {
  class MeasuredPerigee;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboySvcInterfaces/IMboySvcMisc.h"

class MboyPosDigiChoice;
class MboyVarByName;
class MuonboyBFAbstract;
class MuonboyRTAbstract;
class MuonRecDigiStoreAth;
class MB_TrackStoreAth;
class MB_SegmentStoreAth;
class MboyGenTrackStore;
class MB_TrackAth;
class MB_Track;

class MbDRAT;
class MbProtoTrack;

class MbMuSc;

class MboyROAStoreAth;

static const InterfaceID IID_IIMboySvc(1001, 1, 0);

  /**
   @class IMboySvc

    IMboySvc provides methods to perform
    the Muon reconstruction in the Muon Spectrometer

  @author samusog@cern.ch
  
  */

class IMboySvc: virtual public IInterface {
public:
///////////////////////////////////

  static const InterfaceID& interfaceID() { return IID_IIMboySvc; }

  virtual bool       InitializedSvc() = 0 ;
  virtual bool       UsableSvc() = 0 ;
  virtual StatusCode UpdatedSvc(IOVSVC_CALLBACK_ARGS) = 0 ;

  virtual StatusCode AmdcsimrecAthenaSvcUpdatedSvc(IOVSVC_CALLBACK_ARGS) = 0 ;
  virtual StatusCode IMboyBFToolUpdatedTool(IOVSVC_CALLBACK_ARGS) = 0 ;
   
  
   /**Do reconstruction of Track/Segment  
   meth 1: uses meth 4 using configuration set provided by the service */
   virtual StatusCode DoRec(
                   MuonboyBFAbstract*	    pMuonboyBFAbstract,
                   MuonboyRTAbstract*	    pMuonboyRTAbstract,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   const MboyGenTrackStore* pMboyGenTrackStore,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  ) = 0 ;


   /**Do reconstruction of Track/Segment
   meth 2: uses meth 5  using configuration set provided by the service */
   virtual StatusCode DoRec(
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  ) = 0 ;

   /**Do reconstruction of Segment only 
   meth 3: uses meth 6  using configuration set provided by the service */
   virtual StatusCode DoRec(
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  ) = 0 ;
                  

   /**Do reconstruction of Track/Segment 
   meth 4:  main of the DoRec methods */
   virtual StatusCode DoRec(
                   MboyVarByName*           pMboyVarByName,
                   MuonboyBFAbstract*	    pMuonboyBFAbstract,
                   MuonboyRTAbstract*	    pMuonboyRTAbstract,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   const MboyGenTrackStore* pMboyGenTrackStore,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  ) = 0 ;

   /**Do reconstruction of Track/Segment 
   meth 5: uses meth 4 with RT and Field connections  provided by the service*/
   virtual StatusCode DoRec(
                   MboyVarByName*           pMboyVarByName,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  ) = 0 ;

   /**Do reconstruction of Segment only 
   meth 6: uses meth 5 */
   virtual StatusCode DoRec(
                   MboyVarByName*           pMboyVarByName,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  ) = 0 ;

   /**Fill a MuonRecDigiStoreAth with  PrepRawData set */
   virtual StatusCode FillMuonRecDigiStoreWithVectorOfPrepData_Base(
                                         const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                         ) = 0 ;

   /**Update digits (add drift time) */
   virtual StatusCode DoDigiUpdate(
                          MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                         ) = 0 ;

   /**Convert Mboy tracks in Trk::Track */
   virtual StatusCode GetTrackCollection(
                                const MB_TrackStoreAth* pMB_TrackStoreAth,
                                TrackCollection* pTrackCollection,
                                TrackCollection* pTrackCollection_MSonly
                               ) = 0 ;
   virtual StatusCode GetTrackIP(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               ) = 0 ;
   virtual StatusCode GetTrackMS(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               ) = 0 ;

   /**Convert Mboy segments in Trk::Segment */
   virtual StatusCode GetSegmentCollection(
  			          const MB_SegmentStoreAth* pMB_SegmentStoreAth,
        		          Trk::SegmentCollection* pSegmentCollection
                                 , int Clean = 1
  			         ) = 0 ;
   /**Convert Mboy segments in Trk::Segment */
   virtual std::vector<const Muon::MuonSegment*>* GetSegmentCollection(
                                  const MB_SegmentStoreAth* pMB_SegmentStoreAth
                                 , int Clean = 1
                                 ) = 0 ;

   /**Convert Mboy tracks in Rec::TrackParticle */
   virtual StatusCode GetParticleCollection(
                                   const TrackCollection*       pTrackCollection,
                                   Rec::TrackParticleContainer* pTrackParticleContainer
                                  ) = 0 ;

   /**Dump Segment collection */
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump(
                                                  std::string SegmentCollectionLocation,
                                                  std::string TrackCollectionLocation,
		                                  std::ofstream* pOutFile
		                                 ) = 0 ;

   /**Dump Trk::Segment collection */
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump(
                                                  std::string SegmentCollectionLocation,
		                                  std::ofstream* pOutFile
		                                 ) = 0 ;

   /**Dump Trk::Track collection */
   virtual StatusCode MboyDumpTrackCollectionToolDoDump(
                                                std::string TrackCollectionLocation,
		                                std::ofstream* pOutFile
		                               ) = 0 ;

   /**Dump Rec::TrackParticle collection */
   virtual StatusCode MboyDumpTrackParticleContainerToolDoDump(
                                                       std::string TrackParticleContainerLocation,
		                                       std::ofstream* pOutFile
		                                      ) = 0 ;


   /**Collect Matter providing the Field connection */
   virtual StatusCode DoCollectMatter( 
                              MuonboyBFAbstract*	pMuonboyBFAbstract,
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
                             ) = 0 ;

   /**Collect Matter the Field connection is provided by the service) */
   virtual StatusCode DoCollectMatter( 
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
                             ) = 0 ;


    /** find track in the muon system */
   virtual MbProtoTrack* find(
        MboyVarByName* pMboyVarByName,
        MbProtoTrack*  pMbProtoTrack ,
        MbProtoTrack*  pTargetSegmentsProtoTrack 
                                            ) = 0 ;
    /** refine */
   virtual MbProtoTrack* refine(
        MboyVarByName*        pMboyVarByName,
        MbProtoTrack*         pMbProtoTrack,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth,
        MB_TrackStoreAth*     pMB_TrackStoreAth
                                            ) = 0 ;
   /** Set a MbDRAT */
   virtual void SetAMbDRAT (
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
                                            ) = 0 ;
   /** Load the Event */
   virtual void LoadEvent(
        MboyVarByName* pMboyVarByName,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth 
                                            ) = 0 ;
   /**Set default configuarion parameters values */
   virtual void SetVariablesDefault(MboyVarByName*  pMboyVarByName) = 0 ;
   
   /**Get Service configuarion parameters */
   virtual const MboyVarByName* GetSvcSetting() = 0 ;
   
   /**Get Agdd Xml variables */
   virtual void GetAgddXmlVarNb    (int& NbVar )                                   = 0 ;
   virtual void GetAgddXmlVarName  (int  IVar , std::string& NameVar )             = 0 ;
   virtual void GetAgddXmlVarValue (std::string  NameVar, int& Iok, double& Value) = 0 ;

   /**BackTrack */
   virtual MB_Track* MboyMuonBackTrack(MboyVarByName* pMboyVarByName,MB_Track* pMB_Track ) = 0  ;

};

#endif

