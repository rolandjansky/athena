/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboySvc_H
#define MboySvc_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "AthenaKernel/IOVSvcDefs.h" 

class StoreGateSvc;

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Can not be forwarded due to typedef
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

//Dead channel svc
class IMDTConditionsSvc;
class IRPCConditionsSvc;
class ITGCConditionsSvc;
class ICSCConditionsSvc;

namespace Rec {
  class  TrackParticleContainer;
}

namespace Muon {
  class  MuonSegment;
}

class MdtDeadChamberStatus;

/////////////////////////////////////////////////////////////////////////////
#include "MboySvc/MboySvcMisc.h"

class AmdcsimrecAthenaSvc;

class IMboyPrepDataToDigiStoreTool;

class IMboyDumpTrackCollectionTool;
class IMboyDumpSegmentCollectionTool;
class IMboyDumpTrackParticleContainerTool;

class MuonboyInterfaceAbstract;

class IMboyGenTrackTool;
class IMboyRTTool;
class IMboyBFTool;
class IMuonboyToParticleTool;

class MdtIdHelper ; 
class RpcIdHelper ; 
class TgcIdHelper ; 
class CscIdHelper ; 

namespace Trk {
  class IMuonboyToTrackTool;
  class IMuonboyToSegmentTool;
}

template <class TYPE> class SvcFactory;

#include "MboySvcInterfaces/IMboySvc.h"

  /**
   @class MboySvc

    MboySvc provides methods to perform
    the Muon reconstruction in the Muon Spectrometer
    using tools and class from MboyCoreEvt, MboyAthEvt
    and MboyEDMTool packages

  @author samusog@cern.ch
  
  */

class MboySvc : public AthService, virtual public IMboySvc  {
public:
///////////////////////////////////

  static const InterfaceID& interfaceID() { return IID_IIMboySvc; }

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  
  virtual bool       InitializedSvc();
  virtual bool       UsableSvc();
  virtual StatusCode UpdatedSvc(IOVSVC_CALLBACK_ARGS);

  virtual StatusCode AmdcsimrecAthenaSvcUpdatedSvc(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode IMboyBFToolUpdatedTool(IOVSVC_CALLBACK_ARGS);
   
  
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
                  );


   /**Do reconstruction of Track/Segment
   meth 2: uses meth 5  using configuration set provided by the service */
   virtual StatusCode DoRec(
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  );

   /**Do reconstruction of Segment only 
   meth 3: uses meth 6  using configuration set provided by the service */
   virtual StatusCode DoRec(
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  );
                  

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
                  );

   /**Do reconstruction of Track/Segment 
   meth 5: uses meth 4 with RT and Field connections  provided by the service*/
   virtual StatusCode DoRec(
                   MboyVarByName*           pMboyVarByName,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  );

   /**Do reconstruction of Segment only 
   meth 6: uses meth 5 */
   virtual StatusCode DoRec(
                   MboyVarByName*           pMboyVarByName,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth = 0
                  );

   /**Fill a MuonRecDigiStoreAth with  PrepRawData set */
   virtual StatusCode FillMuonRecDigiStoreWithVectorOfPrepData_Base(
                                         const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                         );

   /**Update digits (add drift time) */
   virtual StatusCode DoDigiUpdate(
                          MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                         );

   /**Convert Mboy tracks in Trk::Track */
   virtual StatusCode GetTrackCollection(
                                const MB_TrackStoreAth* pMB_TrackStoreAth,
                                TrackCollection* pTrackCollection,
                                TrackCollection* pTrackCollection_MSonly
                               );
   virtual StatusCode GetTrackIP(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               );
   virtual StatusCode GetTrackMS(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
                               );

   /**Convert Mboy segments in Trk::Segment */
   virtual StatusCode GetSegmentCollection(
  			          const MB_SegmentStoreAth* pMB_SegmentStoreAth,
        		          Trk::SegmentCollection* pSegmentCollection
                                 , int Clean = 1
  			         );
   /**Convert Mboy segments in Trk::Segment */
   virtual std::vector<const Muon::MuonSegment*>* GetSegmentCollection(
                                  const MB_SegmentStoreAth* pMB_SegmentStoreAth
                                 , int Clean = 1
                                 );

   /**Convert Mboy tracks in Rec::TrackParticle */
   virtual StatusCode GetParticleCollection(
                                   const TrackCollection*       pTrackCollection,
                                   Rec::TrackParticleContainer* pTrackParticleContainer
                                  );

   /**Dump Segment collection */
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump(
                                                  std::string SegmentCollectionLocation,
                                                  std::string TrackCollectionLocation,
		                                  std::ofstream* pOutFile
		                                 );

   /**Dump Trk::Segment collection */
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump(
                                                  std::string SegmentCollectionLocation,
		                                  std::ofstream* pOutFile
		                                 );

   /**Dump Trk::Track collection */
   virtual StatusCode MboyDumpTrackCollectionToolDoDump(
                                                std::string TrackCollectionLocation,
		                                std::ofstream* pOutFile
		                               );

   /**Dump Rec::TrackParticle collection */
   virtual StatusCode MboyDumpTrackParticleContainerToolDoDump(
                                                       std::string TrackParticleContainerLocation,
		                                       std::ofstream* pOutFile
		                                      );


   /**Collect Matter providing the Field connection */
   virtual StatusCode DoCollectMatter( 
                              MuonboyBFAbstract*	pMuonboyBFAbstract,
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
                             );

   /**Collect Matter the Field connection is provided by the service) */
   virtual StatusCode DoCollectMatter( 
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
                             );


    /** find track in the muon system */
   virtual MbProtoTrack* find(
        MboyVarByName* pMboyVarByName,
        MbProtoTrack*  pMbProtoTrack ,
        MbProtoTrack*  pTargetSegmentsProtoTrack 
                                            ) ;
    /** refine */
   virtual MbProtoTrack* refine(
        MboyVarByName*        pMboyVarByName,
        MbProtoTrack*         pMbProtoTrack,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth,
        MB_TrackStoreAth*     pMB_TrackStoreAth
                                            ) ;
    /** Set a MbDRAT */
   virtual void SetAMbDRAT (
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
                                            ) ;
   /** Load the Event */
   virtual void LoadEvent(
        MboyVarByName* pMboyVarByName,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth 
                                            ) ;
   /**Set default configuarion parameters values */
   virtual void SetVariablesDefault(MboyVarByName*  pMboyVarByName) ;
   
   /**Get Service configuarion parameters */
   virtual const MboyVarByName* GetSvcSetting() ;
   
   /**Get Agdd Xml variables */
   virtual void GetAgddXmlVarNb    (int& NbVar )                                   ;
   virtual void GetAgddXmlVarName  (int  IVar , std::string& NameVar )             ;
   virtual void GetAgddXmlVarValue (std::string  NameVar, int& Iok, double& Value) ;

   /**BackTrack */
   virtual MB_Track* MboyMuonBackTrack(MboyVarByName* pMboyVarByName,MB_Track* pMB_Track ) ;

   
  friend class SvcFactory<MboySvc>;

protected:
///////////////////////////////////

  // Standard Constructor
  MboySvc(const std::string& name, ISvcLocator* svc);


  // Standard Destructor
  virtual ~MboySvc();

private:
///////////////////////////////////

//Data

   bool m_IsUsable ; //!< Tell usuability state 

   bool m_IsInitialized ; //!< Tell initialisation state 

   int m_GiveRsLineToDigit ; //!< Option to add Rs line string to digit


   int m_ShortSummaryGenTrack ; //!< Print short summary


   int m_WireSagOn ; //!< =1 if data or simul with WireSag on, =0 otherwise

   StoreGateSvc* p_StoreGateSvc ; //!< Pointer On StoreGateSvc
   StoreGateSvc* p_detStore     ; //!< Pointer On detector store
  
  
   /** Get Tools */
    StatusCode GetFromEvtTools();
    StatusCode GetToEDMTools();
    StatusCode GetDumperTools();
    
  
   /** Get RT, B Field and gen tracks connections of the Service */
   StatusCode GetBFRTGen(
                         MuonboyBFAbstract*& pMuonboyBFAbstract,
                         MuonboyRTAbstract*& pMuonboyRTAbstract,
                         MboyGenTrackStore*& pMboyGenTrackStore,
                         MuonRecDigiStoreAth*  pMuonRecDigiStoreAth
                        );

   /** Get FromEvt Tools */
   ToolHandle< IMboyRTTool > p_IMboyRTTool ; //!< Pointer On IMboyRTTool

   ToolHandle< IMboyBFTool > p_IMboyBFTool ; //!< Pointer On IMboyBFTool

   std::vector<std::string> m_TrackRecordCollectionLocationList ; //!< Location of TrackRecordCollection 
   ToolHandle< IMboyGenTrackTool > p_IMboyGenTrackTool ; //!< Pointer On IMboyGenTrackTool

   ToolHandle< IMboyPrepDataToDigiStoreTool > p_IMboyPrepDataToDigiStoreTool ; //!< Pointer On IMboyPrepDataToDigiStoreTool

   
   /** Get ToEDM Tools */
   ToolHandle< Trk::IMuonboyToTrackTool >   p_IMuonboyToTrackToolAdjustableT0        ; //!< Pointer On IMuonboyToTrackTool
   ToolHandle< Trk::IMuonboyToSegmentTool > p_IMuonboyToSegmentToolAdjustableT0      ; //!< Pointer On IMuonboyToSegmentTool
   ToolHandle< Trk::IMuonboyToTrackTool >   p_IMuonboyToTrackTool                    ; //!< Pointer On IMuonboyToTrackTool
   ToolHandle< Trk::IMuonboyToSegmentTool > p_IMuonboyToSegmentTool                  ; //!< Pointer On IMuonboyToSegmentTool
   ToolHandle< IMuonboyToParticleTool >     p_IMuonboyToParticleTool                 ; //!< Pointer On IMuonboyToParticleTool


   /** Get Dumper Tools */
   ToolHandle< IMboyDumpSegmentCollectionTool >      p_IMboyDumpSegmentCollectionTool      ; //!< Pointer On IMboyDumpSegmentCollectionTool
   ToolHandle< IMboyDumpTrackCollectionTool >        p_IMboyDumpTrackCollectionTool        ; //!< Pointer On IMboyDumpTrackCollectionTool
   ToolHandle< IMboyDumpTrackParticleContainerTool > p_IMboyDumpTrackParticleContainerTool ; //!< Pointer On IMboyDumpTrackParticleContainerTool



   MuonboyInterfaceAbstract* p_MuonboyInterfaceAbstract ; //!< Pointer On MuonboyInterfaceAbstract


   /** Define configuration variables */
   void DefineVariables( MboyVarByName* pMboyVarByName,
           std::string Name1,std::string Name2,std::string Comment
   );
   void DefineVariables( MboyVarByName* pMboyVarByName,
           std::string Name1, double Value,std::string Name2,std::string Comment
   );
   void DeclareVariablesAsProprieties     (MboyVarByName* pMboyVarByName);
   void ProprietiesValues2VariablesValues (MboyVarByName* pMboyVarByName);
   void PrintOutVariables                 (MboyVarByName* pMboyVarByName);
   double GetVariableByName               (MboyVarByName* pMboyVarByName, std::string Name);
   
   MboyVarByName* p_MboyVarByName           ; //!< configuration variables
   int    m_NberOfProprieties               ; //!< configuration variables
   double m_ProprityValue[100]              ; //!< configuration variables
   std::vector<std::string>  m_ProprityName ; //!< configuration variables
  

   /** Get Alignement corrections */
   ServiceHandle<AmdcsimrecAthenaSvc> p_AmdcsimrecAthenaSvc;  //!< Pointer On AmdcsimrecAthenaSvc

   /** Set Alignement corrections */
   int m_AlignementLock ; //!< Do or nor Alignment
   int m_AmdcABlinesStamp ; //!< Volatility Status of Alignment corrections
   int m_PrintAlignmentCorr ; //!< Print Alignment Corrections
   StatusCode SetAliLine();

   /** Set matiere data */
   void SetMatiere();

   /** Set Tag Info data */
   int m_TagInfoXMLStamp   ; //!< Volatility Status 
   std::vector< std::pair<std::string,std::string> > m_TagInfoXML  ; //!< Store Data
   int m_PrintTagInfoXML ; //!< Print Data
   void SetMatiereTagInfo();
   
   /** Set AGDD2GeoSwitches data */
   int m_AGDD2GeoSwitchesStamp   ; //!< Volatility Status 
   std::vector<std::string> m_AGDD2GeoSwitches  ; //!< Store Data
   int m_PrintAGDD2GeoSwitches ; //!< Print Data
   void SetMatiereAGDD2GeoSwitches();
   
   /** Set Field Status data */
   int m_FieldStatusStamp   ; //!< Volatility Status 
   int m_PrintFieldStatus ; //!< Print Data
   void SetFieldStatus();
   
   /** Get dead chamber services*/
   ServiceHandle<IMDTConditionsSvc> p_MDTSummarySvc;  //!< Pointer On IMDTConditionsSvc
   ServiceHandle<IRPCConditionsSvc> p_RPCSummarySvc;  //!< Pointer On IRPCConditionsSvc
   ServiceHandle<ITGCConditionsSvc> p_TGCSummarySvc;  //!< Pointer On ITGCConditionsSvc
   ServiceHandle<ICSCConditionsSvc> p_CSCSummarySvc;  //!< Pointer On ICSCConditionsSvc

   const MdtIdHelper * m_mdtId ; //!< IdHelper 
   const CscIdHelper * m_cscId ; //!< IdHelper 
   const RpcIdHelper * m_rpcId ; //!< IdHelper 
   const TgcIdHelper * m_tgcId ; //!< IdHelper 

   /** Set DeadChannels data */
   int m_UseDeadChannelSvc            ; //!< =1/0 Use or not Monica Svc 

   int m_UseDeadChannelSvcMDTStation  ; //!< =1/0 Use or not Monica Svc for MDT Station 
   int m_UseDeadChannelSvcMDTMulLayer ; //!< =1/0 Use or not Monica Svc for MDT Multi Layer
   int m_UseDeadChannelSvcMDTTube     ; //!< =1/0 Use or not Monica Svc for MDT Tubes

   int m_UseDeadChannelSvcRPCStation  ; //!< =1/0 Use or not Monica Svc for RPC Station 
   int m_UseDeadChannelSvcRPStrip     ; //!< =1/0 Use or not Monica Svc for RPC Strip 

   int m_UseDeadChannelSvcTGCStation  ; //!< =1/0 Use or not Monica Svc for TGC Station 

   int m_UseDeadChannelSvcCSCStation  ; //!< =1/0 Use or not Monica Svc for CSC Station 
   int m_UseDeadChannelSvcCSWireLayer ; //!< =1/0 Use or not Monica Svc for CSC Wirelayer 

   void SetDeadChannels();

   std::vector<std::string> m_MultiLayerOut ; //!< Tell by joboptions which multilayer are removed
   
   /** Initialize core of the Svc */
   StatusCode InitializeCore();  

   bool m_IMboyBFToolUpdatedToolDONE ; //!< tell when updated magnetic field done
   bool m_AmdcsimrecAthenaSvcUpdatedSvcDONE ; //!< tell when updated geometry done

   StatusCode regFcnAmdcsimrecAthenaSvcUpdatedSvc();
   StatusCode regFcnIMboyBFToolUpdatedTool();

};

#endif

