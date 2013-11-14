/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/PropertyMgr.h"

//----------------------------------------------------------------//
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
 
//----------------------------------------------------------------//
#include "Particle/TrackParticleContainer.h"
#include "MuonSegment/MuonSegment.h"

#include "MuonCondData/MdtDeadChamberStatus.h"
#include "MuonIdHelpers/MdtIdHelper.h" 
#include "MuonIdHelpers/RpcIdHelper.h" 
#include "MuonIdHelpers/TgcIdHelper.h" 
#include "MuonIdHelpers/CscIdHelper.h" 
        
/////////////////////////////////////////////////////////
#include "MboySvc/MboySvc.h"

//----------------------------------------------------------------//
#include "MuonboyCore/MuonboyInterface.h"

#include "TrackRecord/TrackRecordCollection.h"

#include "MboyCoreEvt/MboyAlineStore.h"
#include "MboyCoreEvt/MboyBlineStore.h"
#include "MboyCoreEvt/MboyIlineStore.h"

#include "AmdcCore/AmdcAlineStore.h"
#include "AmdcCore/AmdcBlineStore.h"
#include "AmdcCore/AmdcIlineStore.h"

//----------------------------------------------------------------//
#include "AmdcAth/AmdcsimrecAthenaSvc.h"

#include "MboyEDMToolInterfaces/IMboyPrepDataToDigiStoreTool.h"

#include "MboyCoreEvt/MboyPosDigiChoice.h"
#include "MboyCoreEvt/MboyVarByName.h"
#include "MboyCoreEvt/MuonboyBFAbstract.h"
#include "MboyCoreEvt/MuonboyRTAbstract.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"
#include "MboyAthEvt/MB_TrackStoreAth.h"
#include "MboyAthEvt/MB_SegmentStoreAth.h"
#include "MboyCoreEvt/MboyGenTrackStore.h"

#include "MboyEDMToolInterfaces/IMboyDumpTrackCollectionTool.h"
#include "MboyEDMToolInterfaces/IMboyDumpSegmentCollectionTool.h"
#include "MboyEDMToolInterfaces/IMboyDumpTrackParticleContainerTool.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "MboyCoreEvt/MbProtoTrack.h"

#include "MboyCoreEvt/MuonboyInterfaceAbstract.h"

#include "MboyEDMToolInterfaces/IMboyGenTrackTool.h"
#include "MboyEDMToolInterfaces/IMboyRTTool.h"
#include "MboyEDMToolInterfaces/IMboyBFTool.h"
#include "MboyEDMToolInterfaces/IMuonboyToParticleTool.h"

#include "MboyEDMToolInterfaces/IMuonboyToTrackTool.h"
#include "MboyEDMToolInterfaces/IMuonboyToSegmentTool.h"

//Dead channel svc
#include "MuonCondInterface/IMDTConditionsSvc.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"
#include "MuonCondInterface/ITGCConditionsSvc.h"
#include "MuonCondInterface/ICSCConditionsSvc.h"

#include "MboyAthEvt/MboyROAStoreAth.h"

MboySvc::MboySvc(const std::string& name,ISvcLocator* svc):
  AthService(name,svc)
, p_IMboyRTTool                         ( "MboyRTTool"                                                            )
, p_IMboyBFTool                         ( "MboyBFTool"                                                            )
, p_IMboyGenTrackTool                   ( "MboyGenTrackTool"                                                      )
, p_IMboyPrepDataToDigiStoreTool        ( "MboyPrepDataToDigiStoreTool"                                           )
, p_IMuonboyToTrackToolAdjustableT0     ( "Trk::MuonboyToTrackTool/MuonboyToTrackToolAdjustableT0"                )
, p_IMuonboyToSegmentToolAdjustableT0   ( "Trk::MuonboyToSegmentTool/MuonboyToSegmentToolAdjustableT0"            )
, p_IMuonboyToTrackTool                 ( "Trk::MuonboyToTrackTool/MuonboyToTrackTool"                            )
, p_IMuonboyToSegmentTool               ( "Trk::MuonboyToSegmentTool/MuonboyToSegmentTool"                        )
, p_IMuonboyToParticleTool              ( "MuonboyToParticleTool"                                                 )
, p_IMboyDumpSegmentCollectionTool      ( "MboyDumpSegmentCollectionTool"                                         )
, p_IMboyDumpTrackCollectionTool        ( "MboyDumpTrackCollectionTool"                                           )
, p_IMboyDumpTrackParticleContainerTool ( "MboyDumpTrackParticleContainerTool"                                    )
, p_AmdcsimrecAthenaSvc                 ( "AmdcsimrecAthenaSvc",name ) 
, p_MDTSummarySvc                       ( "MDTCondSummarySvc",  name )
, p_RPCSummarySvc                       ( "RPCCondSummarySvc",  name )
, p_TGCSummarySvc                       ( "TGCCondSummarySvc",  name )
, p_CSCSummarySvc                       ( "CSCCondSummarySvc",  name )
{


  m_mdtId = 0 ; 
  m_cscId = 0 ; 
  m_rpcId = 0 ; 
  m_tgcId = 0 ; 

//Set Default values

  m_MultiLayerOut.clear();
  declareProperty("MultiLayerOut"  , m_MultiLayerOut ) ;

  declareProperty("MboyRTTool"                         , p_IMboyRTTool                         ) ;
  declareProperty("MboyBFTool"                         , p_IMboyBFTool                         ) ;
  declareProperty("MboyGenTrackTool"                   , p_IMboyGenTrackTool                   ) ;
  declareProperty("MboyPrepDataToDigiStoreTool"        , p_IMboyPrepDataToDigiStoreTool        ) ;
  declareProperty("MuonboyToTrackToolAdjustableT0"     , p_IMuonboyToTrackToolAdjustableT0     ) ;
  declareProperty("MuonboyToSegmentToolAdjustableT0"   , p_IMuonboyToSegmentToolAdjustableT0   ) ;
  declareProperty("MuonboyToTrackTool"                 , p_IMuonboyToTrackTool                 ) ;
  declareProperty("MuonboyToSegmentTool"               , p_IMuonboyToSegmentTool               ) ;
  declareProperty("MuonboyToParticleTool"              , p_IMuonboyToParticleTool              ) ;
  declareProperty("MboyDumpSegmentCollectionTool"      , p_IMboyDumpSegmentCollectionTool      ) ;
  declareProperty("MboyDumpTrackCollectionTool"        , p_IMboyDumpTrackCollectionTool        ) ;
  declareProperty("MboyDumpTrackParticleContainerTool" , p_IMboyDumpTrackParticleContainerTool ) ;

  declareProperty("AmdcsimrecAthenaSvc", p_AmdcsimrecAthenaSvc);

  declareProperty( "PrintAlignmentCorr"                , m_PrintAlignmentCorr                = 0 ) ;
  declareProperty( "AlignementLock"                    , m_AlignementLock                    = 0 ) ;
  
  declareProperty( "PrintTagInfoXML"                   , m_PrintTagInfoXML                   = 0 ) ;
  declareProperty( "PrintAGDD2GeoSwitches"             , m_PrintAGDD2GeoSwitches             = 0 ) ;
  declareProperty( "PrintFieldStatus"                  , m_PrintFieldStatus                  = 0 ) ;
  
  m_TrackRecordCollectionLocationList.clear();
  m_TrackRecordCollectionLocationList.push_back("MuonEntryLayerFilter") ;
  m_TrackRecordCollectionLocationList.push_back("MuonEntryRecordFilter") ;
  declareProperty( "TrackRecordCollectionLocationList" , m_TrackRecordCollectionLocationList     ) ;
  declareProperty( "ShortSummaryGenTrack"              , m_ShortSummaryGenTrack              = 0 ) ;
  
  declareProperty( "GiveRsLineToDigit"                 , m_GiveRsLineToDigit              = 0 ) ;

  declareProperty( "WireSagOn"                         , m_WireSagOn                      = 0 ) ;

  declareProperty( "UseDeadChannelSvc"                            , m_UseDeadChannelSvc             = 1 ) ;

  declareProperty( "UseDeadChannelSvcMDTStation"                  , m_UseDeadChannelSvcMDTStation   = 1 ) ;
  declareProperty( "UseDeadChannelSvcMDTMulLayer"                 , m_UseDeadChannelSvcMDTMulLayer  = 0 ) ;
  declareProperty( "UseDeadChannelSvcMDTTube"                     , m_UseDeadChannelSvcMDTTube      = 0 ) ;

  declareProperty( "UseDeadChannelSvcRPCStation"                  , m_UseDeadChannelSvcRPCStation   = 0 ) ;
  declareProperty( "UseDeadChannelSvcRPStrip"                     , m_UseDeadChannelSvcRPStrip      = 0 ) ;

  declareProperty( "UseDeadChannelSvcTGCStation"                  , m_UseDeadChannelSvcTGCStation   = 0 ) ;

  declareProperty( "UseDeadChannelSvcCSCStation"                  , m_UseDeadChannelSvcCSCStation   = 0 ) ;
  declareProperty( "UseDeadChannelSvcCSWireLayer"                 , m_UseDeadChannelSvcCSWireLayer  = 0 ) ;

  declareProperty( "MDTCondSummarySvc", p_MDTSummarySvc );
  declareProperty( "RPCCondSummarySvc", p_RPCSummarySvc );
  declareProperty( "TGCCondSummarySvc", p_TGCSummarySvc );
  declareProperty( "CSCCondSummarySvc", p_CSCSummarySvc );


//Create a Variables carrier
  m_NberOfProprieties = 0 ;
  p_MboyVarByName = new MboyVarByName ;


//Define Variables 
//-----------------------------------------------------------------
// LevelOfRecons (MBSto/ISOF)  : 
//   At what "level" must MuonBoy stop (Nominal is 3) 
//     -2 ===> nothing (i.e. only prints out digis)
//     -1 ===> straight track segments in one multilayer only
//      0 ===> "Strict" straight track segments only
//      1 ===> "Strict" + "Loose" straight track segments
//      2 ===> Simple fit of straight track segments
//      3 ===> Full fit with matter
//-----------------------------------------------------------------
// SimulationBug (SBUG) : 
//          Index to treat bug in input
//          0 ===> no bug
//          8 ===> Digitization without eloss, nor scattering
//-----------------------------------------------------------------
// NegBarMDTtubeNubering (MDTnu) : 
//          Negative Barrel MDT tube Numbering Scheme
//         -1 ===> increasing with |Z| and symetrical (/Z=0) tube staggering
//          1 ===> increasing with Z and uniform tube staggering

//Define Variables
  DefineVariables( p_MboyVarByName , "EXternalCLusters"       , 1      , "EXCL"      , "Use External Clusters.............................................  " ); 
  DefineVariables( p_MboyVarByName , "PrintLevel"             , -1     , "MBPri"     , "MuonBoy print level for debug (from -1 to 6) ...................... " );
  DefineVariables( p_MboyVarByName , "MuonboyAcceleration"    , 11     , "MBAcc"     , "MuonBoy 'acceleration' flag ....................................... " );
  DefineVariables( p_MboyVarByName , "CheckDigits"            , 0      , "MBChkDig"  , "Check Digits adresses and positions (0: No, 1:Yes)................. " );
  DefineVariables( p_MboyVarByName , "LevelOfRecons"          , 3      , "MBSto"     , "At what 'level' must MuonBoy stop ................................. " );
  DefineVariables( p_MboyVarByName , "RequireRoA"             , 1      , "UseRoA"    , "=2 (1) Start all(loose) segments only inside RoA; =0 everywhere.... " ); 
  DefineVariables( p_MboyVarByName , "NegBarMDTtubeNubering"  , -1     , "MDTnu"     , "Negative Barrel MDT tube nubering scheme .......................... " );
  DefineVariables( p_MboyVarByName , "SeverityReco"           , 1      , "XSeve"     , "Reconstruction 'Severity' ......................................... " );
  DefineVariables( p_MboyVarByName , "MDTTubeResolution"      , 0.0085 , "ReTub"     , "'Typical' MDT tube resolution (cm) ................................ " );
  DefineVariables( p_MboyVarByName , "MDTTubeEfficiency"      , 1      , "TubEf"     , "Efficiency (%) to be applied in the MDT digitisation .............. " );
  DefineVariables( p_MboyVarByName , "RPCTGCStripEfficiency"  , 1      , "StrEf"     , "Efficiency (%) to be applied in the RPC/TGC digitisation .......... " );
  DefineVariables( p_MboyVarByName , "StationAlignementError" , 0      , "AerSt"     , "Alignement error (cm) to be applied to a MDT station .............. " );
  DefineVariables( p_MboyVarByName , "MLayerAlignementError"  , 0      , "AerML"     , "Alignement error (cm) to be applied to a MDT multilayer ........... " );
  DefineVariables( p_MboyVarByName , "CSCAlignementError"     , 0      , "CerSt"     , "Alignement error (cm) to be applied to a CSC station .............. " );
  DefineVariables( p_MboyVarByName , "CSCLayerAlignError"     , 0      , "CerML"     , "Alignement error (cm) to be applied to a CSC layer ................ " );
  DefineVariables( p_MboyVarByName , "FixedMomentum"          , 0      , "FixMo"     , "/= 0, do not fit momentum but fix it to a (signed) value (GeV)..... " );
  DefineVariables( p_MboyVarByName , "FixedMomWhenNoField"    , 6      , "MomNoField", "Momentum to be used when no field ................................. " );  
  DefineVariables( p_MboyVarByName , "MboyElossOption"        , 4      , "MEloss"    , "Muonboy Eloss options control: = 0 no action ...................... " );
  DefineVariables( p_MboyVarByName , "SimulationBug"          , 0      , "SimBug"    , "Bugs in simulation : +1 = shift of 1/2 CSC strip .................. " );
  DefineVariables( p_MboyVarByName , "AtlasSimulationSample"  , -1     , "IsSimu"    , "1 for Atlas simulation sample, 0 for real data .................... " );
  DefineVariables( p_MboyVarByName , "AtlasCosmicsSample"     , 0      , "AtCosm"    , "1 for Atlas cosmics sample, 2 or 3 for beam halo .................. " );
  DefineVariables( p_MboyVarByName , "WireLengthOfConstRT"    , 6      , "DsFastRT"  , "/= 0, length along tube over which R<->T rel.considered constant (in cm)" );
  DefineVariables( p_MboyVarByName , "MinRCHAMI"              , 0      , "RCHAMImi"  , "Min of RCHAMI (cm) Radius from which the tracking starts .......... " );
  DefineVariables( p_MboyVarByName , "MaxRCHAMA"              , 999999 , "RCHAMAma"  , "Max of RCHAMA (cm) Radius at which the tracking ends .............. " );
  DefineVariables( p_MboyVarByName , "TubeDeadTime"           , 600    , "DeTub"     , "'Typical' dead time (ns) of a MDT tube ............................ " );
  DefineVariables( p_MboyVarByName , "MdtAdcRejectionCut"     , 40.    , "MdtCut"    , "cut on ADC of MDT hits to reject hits   ........................... " );
  DefineVariables( p_MboyVarByName , "MdtAdcCut"              , 50.    , "MdtAdc"    , "cut on ADC of MDT hits   .......................................... " );
  DefineVariables( p_MboyVarByName , "SegAdcCut"              , 80.    , "SegAdc"    , "cut on ADC at segment level   ..................................... " );
  DefineVariables( p_MboyVarByName , "RimpfromRTSvc"          , 0      , "RimRTSvc"  , "Use RTSvc to compute Impact parameter    .......................... " );
  DefineVariables( p_MboyVarByName , "MaxConsStrips"          , 50     , "MaxConStr" , "Maximum number of allowed consecutives strips for ROA    .......... " );
  DefineVariables( p_MboyVarByName , "UseAllSegments"         , 0      , "UseAllSeg" , "=1 Starts tracks with all segments, =0 use strict segments only ... " );
  DefineVariables( p_MboyVarByName , "UseRoaFromMdt"          , 1      , "UseRoaMdt" , "=1 use also Roa made from MDT, =0 do not use them ................. " );
  DefineVariables( p_MboyVarByName , "MinuitFileOut"          , 1      , "MinuiFOut" , "=1 let the Minuit file to be produced, 0 otherwise ................ " );
  DefineVariables( p_MboyVarByName , "DontUseCutOut"          , 0      , "NoCutOut"  , "=1 do not use cutouts not implemented in simulation  .............. " );
  DefineVariables( p_MboyVarByName , "GeomDumpUnit"           , 6      , "LunGeo"    , "Fortran unit where to dump the geometry (6 = no dump) ............. " );
  DefineVariables( p_MboyVarByName , "MaxNberOfScattering"    , -1     , "MaxNberMS" , "Maximum nber of scattering points (-1 = 3-4-5 depending on P) ..... " );
  DefineVariables( p_MboyVarByName , "FinalChi2Cut"           , -1     , "FinalCut"  , "Chi2 cut to be applied on final fit  .............................. " );
  DefineVariables( p_MboyVarByName , "DoT0Fit"                , 0      , "DoT0Fit"   , "Do the fit on T0 for cosmics  ...... .............................. " );
  DefineVariables( p_MboyVarByName , "UseT0Fit"               , 0      , "UseT0Fit"  , "Use the result of the T0 fit during reconstruction  ............... " );
  DefineVariables( p_MboyVarByName , "StepT0Fit"              , 5.     , "StepT0Fit" , "Step (in ns) for T0 fit ........................................... " );
  DefineVariables( p_MboyVarByName , "NstepT0Fit"             , 30     , "NsteT0Fit" , "Number of steps for T0 fit (from -Nste to Nste) ................... " );
  DefineVariables( p_MboyVarByName , "TubeResolutionWhenNoT0" , 0.03   , "ReTubNoT0" , "Contribution to tube resolution when no T0 for cosmics ............ " );
  DefineVariables( p_MboyVarByName , "CheckCscALines"         , 0      , "MBChkCsc"  , "Compare use of Alines between Amdc and Mboy (0: No, n:Yes on lun=n) " );
  DefineVariables( p_MboyVarByName , "TStripMin"              , 999.   , "TStripMin" , "Lower limit of RPC strip time window .............................. " );
  DefineVariables( p_MboyVarByName , "TStripMax"              ,-999.   , "TStripMax" , "Upper limit of RPC strip time window .............................. " );
  DefineVariables( p_MboyVarByName , "FracTubStric2ML"        , 0.95   , "TubStr2ML" , "Cut on fraction of tubes hit for strict segments with 2ML ......... " );
  DefineVariables( p_MboyVarByName , "FracTubStric1ML"        , 0.90   , "TubStr1ML" , "Cut on fraction of tubes hit for strict segments with 1ML ......... " );
  DefineVariables( p_MboyVarByName , "FracTubLoose2ML"        , 0.70   , "TubLoo2ML" , "Cut on fraction of tubes hit for loose  segments with 2ML ......... " );
  DefineVariables( p_MboyVarByName , "FracTubLoose1ML"        , 0.50   , "TubLoo1ML" , "Cut on fraction of tubes hit for loose  segments with 1ML ......... " );
  DefineVariables( p_MboyVarByName , "ResBadlyAlignedBIS"     , 0.     , "ResBadBIS" , "Additionnal contribution to tube resolution for misaligned BIS .... " );
  DefineVariables( p_MboyVarByName , "ResBadlyAlignedBEE"     , 0.     , "ResBadBEE" , "Additionnal contribution to tube resolution for misaligned BEE .... " );
  DefineVariables( p_MboyVarByName , "ResBadlyAlignedEIL"     , 0.     , "ResBadEIL" , "Additionnal contribution to tube resolution for misaligned EIL .... " );
  DefineVariables( p_MboyVarByName , "ResBadlyAlignedEEL"     , 0.     , "ResBadEEL" , "Additionnal contribution to tube resolution for misaligned EEL .... " );
  DefineVariables( p_MboyVarByName , "ResBadlyAligned"        , 0.     , "ResBadAli" , "Additionnal contribution to tube resolution for other misaligned .. " );
  DefineVariables( p_MboyVarByName , "ResBadTomo"             , 0.     , "ResBadTomo", "Additionnal contribution to tube resolution for bad tomo .......... " );
  DefineVariables( p_MboyVarByName , "RemoveDeadCscLayers"    , 0      , "DeadCscLay", "=1 to remove (i.e. no penalty) the dead layers in the CSC ......... " );
  DefineVariables( p_MboyVarByName , "AligRPCeta"             , 0.4    , "AligRPCeta", "alignment tolerance (in cm) for RPC eta strips .................... " );
  DefineVariables( p_MboyVarByName , "AligRPCphi"             , 0.4    , "AligRPCphi", "alignment tolerance (in cm) for RPC phi strips .................... " );
  DefineVariables( p_MboyVarByName , "AligTGCeta"             , 0.4    , "AligTGCeta", "alignment tolerance (in cm) for TGC eta strips .................... " );
  DefineVariables( p_MboyVarByName , "AligTGCphi"             , 0.4    , "AligTGCphi", "alignment tolerance (in cm) for TGC phi strips .................... " );
  DefineVariables( p_MboyVarByName , "DoAlignementFit"        , 0      , "DoAligFit" , "1= do station alignement fit in matcov, 2 = do it inside main fit . " );
  DefineVariables( p_MboyVarByName , "AligStationBarrel"      , 0.0020 , "AligStBa"  , "Alignement error (cm) to be applied to a MDT barrel station ....... " );
  DefineVariables( p_MboyVarByName , "AligStationEndcap"      , 0.0020 , "AligStEc"  , "Alignement error (cm) to be applied to a MDT endcap station ....... " );
  DefineVariables( p_MboyVarByName , "AligStationCSC"         , 0.0020 , "AligStCs"  , "Alignement error (cm) to be applied to a CSC station .............. " );
  DefineVariables( p_MboyVarByName , "AligBarrelEndcap"       , 0.     , "AligBaEc"  , "Relative alignement error (cm) to be applied to barrel/endcap ..... " );
  DefineVariables( p_MboyVarByName , "BarrelAngularAlig"      , 0.00005, "ResAngBa"  , "Angular resolution of alignement of barrel stations  .............. " );
  DefineVariables( p_MboyVarByName , "EndcapAngularAlig"      , 0.00005, "ResAngEc"  , "Angular resolution of alignement of endcap stations  .............. " );
  DefineVariables( p_MboyVarByName , "RecoverDeadChannels"    , 0      , "RecovDead" , "1 = Recover dead channels with hits  .............................. " );
  DefineVariables( p_MboyVarByName , "ErrZMsIdBarrel"         , 0.     , "ErrZMsIdBa", "Uncertainty on z0 due to Ms-ID for barrel tracks  ................. " );
  DefineVariables( p_MboyVarByName , "ErrZMsIdEndcap"         , 0.     , "ErrZMsIdEc", "Uncertainty on z0 due to Ms-ID for endcap tracks  ................. " );


//Create p_MuonboyInterfaceAbstract
  p_MuonboyInterfaceAbstract = new MuonboyInterface;

//Declare Variables as Proprieties
  DeclareVariablesAsProprieties(p_MboyVarByName) ;

//Initial value of Volatility Status of Alignment corrections
   m_AmdcABlinesStamp = -9999 ;

//Initial value of Volatility Status of TagInfo infos
  m_TagInfoXMLStamp = -9999 ;
  m_TagInfoXML.clear();

//Initial value of Volatility Status of AGDD2GeoSwitches Data
  m_AGDD2GeoSwitchesStamp = -9999 ;
  m_AGDD2GeoSwitches.clear();

//Initial value  
  m_IMboyBFToolUpdatedToolDONE = false ;
  m_AmdcsimrecAthenaSvcUpdatedSvcDONE = false ;

   m_IsInitialized = false;
   m_IsUsable      = false;

   m_FieldStatusStamp = -1;

   p_StoreGateSvc = 0 ;
   p_detStore     = 0 ;

}
 
/// Standard Destructor
MboySvc::~MboySvc()  {
 delete p_MuonboyInterfaceAbstract;
 delete p_MboyVarByName;

}
 
/// Service initialisation
StatusCode MboySvc::initialize() {

   StatusCode sc;
   ATH_MSG_INFO( "Initialisation started     " );

   sc=AthService::initialize();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL( "Service::initialize() failed" );
     return StatusCode::FAILURE;
   }


//Update Variables 
  ProprietiesValues2VariablesValues(p_MboyVarByName) ;


//ReDefine Variables defaults
  SetVariablesDefault(p_MboyVarByName) ;
  

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );
  PrintOutVariables(p_MboyVarByName) ;
  ATH_MSG_INFO( "= PrintAlignmentCorr                             " << m_PrintAlignmentCorr );
  ATH_MSG_INFO( "= AlignementLock                                 " << m_AlignementLock );
  ATH_MSG_INFO( "= PrintTagInfoXML                                " << m_PrintTagInfoXML );
  ATH_MSG_INFO( "= PrintAGDD2GeoSwitches                          " << m_PrintAGDD2GeoSwitches );
  ATH_MSG_INFO( "= PrintFieldStatus                               " << m_PrintFieldStatus );
  ATH_MSG_INFO( "= TrackRecordCollectionLocationList              " );
  int TrackRecordCollectionLocationListSize =  m_TrackRecordCollectionLocationList.size() ;
  for (int TrackRecordCollectionLocationListItem=0; TrackRecordCollectionLocationListItem<TrackRecordCollectionLocationListSize ; TrackRecordCollectionLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_TrackRecordCollectionLocationList[TrackRecordCollectionLocationListItem] );
  }
  ATH_MSG_INFO( "= ShortSummaryGenTrack                           " << m_ShortSummaryGenTrack );
  ATH_MSG_INFO( "= GiveRsLineToDigit                              " << m_GiveRsLineToDigit );

  ATH_MSG_INFO( "= UseDeadChannelSvc                              " << m_UseDeadChannelSvc            );
  ATH_MSG_INFO( "=  UseDeadChannelSvcMDTStation                   " << m_UseDeadChannelSvcMDTStation  );

  ATH_MSG_INFO( "=  UseDeadChannelSvcMDTMulLayer                  " << m_UseDeadChannelSvcMDTMulLayer );
  ATH_MSG_INFO( "=  UseDeadChannelSvcMDTTube                      " << m_UseDeadChannelSvcMDTTube     );

  ATH_MSG_INFO( "=  UseDeadChannelSvcRPCStation                   " << m_UseDeadChannelSvcRPCStation  );
  ATH_MSG_INFO( "=  UseDeadChannelSvcRPStrip                      " << m_UseDeadChannelSvcRPStrip     );

  ATH_MSG_INFO( "=  UseDeadChannelSvcTGCStation                   " << m_UseDeadChannelSvcTGCStation  );

  ATH_MSG_INFO( "=  UseDeadChannelSvcCSCStation                   " << m_UseDeadChannelSvcCSCStation  );
  ATH_MSG_INFO( "=  UseDeadChannelSvcCSWireLayer                  " << m_UseDeadChannelSvcCSWireLayer );
  ATH_MSG_INFO( "= MultiLayerOut              " );
  int MultiLayerOutSize =  m_MultiLayerOut.size() ;
  for (int MultiLayerOutItem=0; MultiLayerOutItem<MultiLayerOutSize ; MultiLayerOutItem++){
   ATH_MSG_INFO( "=                                                " << m_MultiLayerOut[MultiLayerOutItem] );
  }

  ATH_MSG_INFO( "================================" );
  
  
//Set pointer on StoreGateSvc
  sc = service("StoreGateSvc", p_StoreGateSvc);
  if ( sc.isFailure() ) {
     ATH_MSG_FATAL( "StoreGate service not found" );
    return StatusCode::FAILURE;
  }

//Set pointer on DetectorStore 
  sc = service("DetectorStore",p_detStore);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DetectorStore service not found !" );
    return StatusCode::FAILURE;
  }

//Retreive p_AmdcsimrecAthenaSvc and set up call back
  if ( p_AmdcsimrecAthenaSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << p_AmdcsimrecAthenaSvc );
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "Retrieved service " << p_AmdcsimrecAthenaSvc );

  if (p_AmdcsimrecAthenaSvc->InitializedSvc()) {
    ATH_MSG_INFO( "p_AmdcsimrecAthenaSvc->InitializedSvc() is true " );
    m_AmdcsimrecAthenaSvcUpdatedSvcDONE = true ; 

  }else{
    ATH_MSG_INFO( "p_AmdcsimrecAthenaSvc->InitializedSvc() is false " );

    sc=regFcnAmdcsimrecAthenaSvcUpdatedSvc();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("regFcnAmdcsimrecAthenaSvcUpdatedSvc failed" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Done: regFcnAmdcsimrecAthenaSvcUpdatedSvc " );

  }

//Get FromEvt Tools
  sc = GetFromEvtTools();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " GetFromEvtTools failed " );
    return( StatusCode::FAILURE );
  }

//Get ToEDM Tools
  sc = GetToEDMTools();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " GetToEDMTools failed " );
    return( StatusCode::FAILURE );
  }

//Get Dumper Tools
  sc = GetDumperTools();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " GetDumperTools failed " );
    return( StatusCode::FAILURE );
  }

//Dead channel Svc
  sc = p_MDTSummarySvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Could not retrieve the MDT summary service");
  }
  sc = p_RPCSummarySvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Could not retrieve the RPC summary service");
  }
  sc = p_TGCSummarySvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Could not retrieve the TGC summary service");
  }
  sc = p_CSCSummarySvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Could not retrieve the CSC summary service");
  }

  sc = p_detStore->retrieve(m_mdtId,"MDTIDHELPER"); 
  if (sc.isFailure()){ 
    ATH_MSG_FATAL( "Cannot retrieve MDTIDHELPER" ); 
    return( StatusCode::FAILURE ); 
  } 
  sc = p_detStore->retrieve(m_cscId,"CSCIDHELPER"); 
  if (sc.isFailure()){ 
    ATH_MSG_FATAL( "Cannot retrieve CSCIDHELPER" ); 
    return( StatusCode::FAILURE ); 
  } 
  sc = p_detStore->retrieve(m_rpcId,"RPCIDHELPER"); 
  if (sc.isFailure()){ 
    ATH_MSG_FATAL( "Cannot retrieve RPCIDHELPER" ); 
    return( StatusCode::FAILURE ); 
  } 
  sc = p_detStore->retrieve(m_tgcId,"TGCIDHELPER"); 
  if (sc.isFailure()){ 
    ATH_MSG_FATAL( "Cannot retrieve TGCIDHELPER" ); 
    return( StatusCode::FAILURE ); 
  } 
 

  ATH_MSG_INFO( "Initialisation ended     " );
  return StatusCode::SUCCESS;

}
 
//Get FromEvt Tools
StatusCode MboySvc::GetFromEvtTools() {

//Retrieve p_IMboyPrepDataToDigiStoreTool
  if ( p_IMboyPrepDataToDigiStoreTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyPrepDataToDigiStoreTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyPrepDataToDigiStoreTool );


//Retrieve p_IMboyGenTrackTool
  if ( p_IMboyGenTrackTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyGenTrackTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyGenTrackTool );  


//Retrieve p_IMboyRTTool
  if ( p_IMboyRTTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyRTTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyRTTool );


//Retrieve p_IMboyBFTool and set up call back
  if ( p_IMboyBFTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyBFTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyBFTool );

  StatusCode sc = regFcnIMboyBFToolUpdatedTool();
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "regFcnIMboyBFToolUpdatedTool failed " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: regFcnIMboyBFToolUpdatedTool " );
  
  return StatusCode::SUCCESS;

}
 
//Get ToEDM Tools
StatusCode MboySvc::GetToEDMTools() {

//Retrieve p_IMuonboyToTrackToolAdjustableT0
  if ( p_IMuonboyToTrackToolAdjustableT0.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToTrackToolAdjustableT0 );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToTrackToolAdjustableT0 );


//Retrieve p_IMuonboyToSegmentToolAdjustableT0
  if ( p_IMuonboyToSegmentToolAdjustableT0.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToSegmentToolAdjustableT0 );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToSegmentToolAdjustableT0 );  


//Retrieve p_IMuonboyToTrackTool
  if ( p_IMuonboyToTrackTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToTrackTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToTrackTool );


//Retrieve p_IMuonboyToSegmentTool
  if ( p_IMuonboyToSegmentTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToSegmentTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToSegmentTool );  


//Retrieve p_IMuonboyToParticleTool
  if ( p_IMuonboyToParticleTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToParticleTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToParticleTool );

  return StatusCode::SUCCESS;

}
 
//Get Dumper Tools
StatusCode MboySvc::GetDumperTools() {

//Retrieve p_IMboyDumpTrackCollectionTool
  if ( p_IMboyDumpTrackCollectionTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDumpTrackCollectionTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyDumpTrackCollectionTool );


//Retrieve p_IMboyDumpSegmentCollectionTool
  if ( p_IMboyDumpSegmentCollectionTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDumpSegmentCollectionTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyDumpSegmentCollectionTool );


//Retrieve p_IMboyDumpTrackParticleContainerTool
  if ( p_IMboyDumpTrackParticleContainerTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDumpTrackParticleContainerTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyDumpTrackParticleContainerTool );

  return StatusCode::SUCCESS;

}

// Service finalisation
StatusCode MboySvc::finalize() {return StatusCode::SUCCESS;}
 
StatusCode MboySvc::queryInterface( const InterfaceID& riid, void** ppvInterface ) {
  if ( IID_IIMboySvc == riid )    {
    *ppvInterface = (IMboySvc*)this;
  }
  else  {
    return Service::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

//meth 1
StatusCode MboySvc::DoRec(
                   MuonboyBFAbstract*	    pMuonboyBFAbstract,
                   MuonboyRTAbstract*	    pMuonboyRTAbstract,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   const MboyGenTrackStore* pMboyGenTrackStore,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth 
){

  StatusCode sc = StatusCode::SUCCESS;

  sc = DoRec(
             p_MboyVarByName,
             pMuonboyBFAbstract,
             pMuonboyRTAbstract,
             pMboyPosDigiChoice,
             pMuonRecDigiStoreAth,
             pMboyGenTrackStore,
             pMB_TrackStoreAth,
             pMB_SegmentStoreAth,
             pMboyROAExternalStoreAth
            );

  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " DoRec failed " );
    return( StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;

}

//meth 2
StatusCode MboySvc::DoRec(
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth
){
  
  StatusCode sc = StatusCode::SUCCESS;

  sc = DoRec(
             p_MboyVarByName,
             pMboyPosDigiChoice,
             pMuonRecDigiStoreAth,
             pMB_TrackStoreAth,
             pMB_SegmentStoreAth,
             pMboyROAExternalStoreAth
            );

  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " DoRec failed " );
    return( StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;

}


//meth 3
StatusCode MboySvc::DoRec(
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth
){
  
  StatusCode sc = StatusCode::SUCCESS;

  sc = DoRec(
             p_MboyVarByName,
             pMboyPosDigiChoice,
             pMuonRecDigiStoreAth,
             pMB_SegmentStoreAth,
             pMboyROAExternalStoreAth
            );

  if ( sc.isFailure() ){
    ATH_MSG_FATAL( " DoRec failed " );
    return( StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;

}

//meth 4
StatusCode MboySvc::DoRec(
                   MboyVarByName*           pMboyVarByName,
                   MuonboyBFAbstract*	    pMuonboyBFAbstract,
                   MuonboyRTAbstract*	    pMuonboyRTAbstract,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   const MboyGenTrackStore* pMboyGenTrackStore,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth
){

//Print Gen track 
  if (m_ShortSummaryGenTrack == 1) pMboyGenTrackStore->PrintSummary() ;
  
//Load matiere data
  SetMatiere();
  
//Set Field Status
  SetFieldStatus() ;
  
  int StatusMuonboyInterface = p_MuonboyInterfaceAbstract->DoRec(
                                                                 pMboyVarByName,
								 pMuonboyBFAbstract,
								 pMuonboyRTAbstract,
								 pMboyPosDigiChoice,
								 pMuonRecDigiStoreAth,
								 pMboyGenTrackStore,
								 pMB_TrackStoreAth,
								 pMB_SegmentStoreAth,
                                                                 pMboyROAExternalStoreAth
								 );

  if ( StatusMuonboyInterface !=0 ){
    ATH_MSG_FATAL( " DoRec failed " );
    return( StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;

}

//meth 5
StatusCode MboySvc::DoRec(
                   MboyVarByName*           pMboyVarByName,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_TrackStoreAth*	    pMB_TrackStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth
){
  
  StatusCode sc = StatusCode::SUCCESS;

//Get BF, RT and pMboyGenTrackStore
  MuonboyBFAbstract* pMuonboyBFAbstract = 0;
  MuonboyRTAbstract* pMuonboyRTAbstract = 0 ;
  MboyGenTrackStore* pMboyGenTrackStore = 0 ;  
  sc = GetBFRTGen(
                  pMuonboyBFAbstract,
                  pMuonboyRTAbstract,
                  pMboyGenTrackStore,
                  pMuonRecDigiStoreAth
                 );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " GetBFRTGen failed " );
    return( StatusCode::FAILURE );
  }

  sc = DoRec(
             pMboyVarByName,
             pMuonboyBFAbstract,
             pMuonboyRTAbstract,
             pMboyPosDigiChoice,
             pMuonRecDigiStoreAth,
             pMboyGenTrackStore,
             pMB_TrackStoreAth,
             pMB_SegmentStoreAth,
             pMboyROAExternalStoreAth
            );

  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " DoRec failed " );
    return( StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;

}
//meth 6
StatusCode MboySvc::DoRec(
                   MboyVarByName*           pMboyVarByName,
                   const MboyPosDigiChoice* pMboyPosDigiChoice,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth,
                   MB_SegmentStoreAth*      pMB_SegmentStoreAth,
                   MboyROAStoreAth*         pMboyROAExternalStoreAth
){
  
  StatusCode sc = StatusCode::SUCCESS;

  MB_TrackStoreAth aMB_TrackStoreAth;
  MB_TrackStoreAth* pMB_TrackStoreAth = &(aMB_TrackStoreAth) ;
  pMB_TrackStoreAth->ResetMB_TrackStoreAth();

  sc = DoRec(
             pMboyVarByName,
             pMboyPosDigiChoice,
             pMuonRecDigiStoreAth,
             pMB_TrackStoreAth,
             pMB_SegmentStoreAth,
             pMboyROAExternalStoreAth
            );

  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " DoRec failed " );
    return( StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;

}


StatusCode MboySvc::FillMuonRecDigiStoreWithVectorOfPrepData_Base(
                                         const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  StatusCode sc = StatusCode::SUCCESS;

  const DataHandle<EventInfo> pEventInfo;
  sc = p_StoreGateSvc->retrieve(pEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " retrieve pEventInfo failed" );
    return( StatusCode::FAILURE );
  }

  pMuonRecDigiStoreAth->SetRunNber(pEventInfo->event_ID()->run_number());
  pMuonRecDigiStoreAth->SetEventNber(pEventInfo->event_ID()->event_number());

  p_IMboyPrepDataToDigiStoreTool->FillMuonRecDigiStoreWithVectorOfPrepData_Base(
                                   VectorOfpPrepData,
                                   pMuonRecDigiStoreAth
                                  );
  if ( m_GiveRsLineToDigit == 1 ){
    sc = DoDigiUpdate(pMuonRecDigiStoreAth);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "DoDigiUpdate failed" );
      return( StatusCode::FAILURE);
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode MboySvc::DoDigiUpdate(
                   MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                  ){

  StatusCode sc = StatusCode::SUCCESS;


//Get RT
  MuonboyRTAbstract* pMuonboyRTAbstract = 0 ;
  // double DoT0Fit = GetVariableByName(p_MboyVarByName,"DoT0Fit");
  // if ( DoT0Fit < 0.5 ){
  //   sc=p_IMboyRTTool->GetMuonboyRTAbstractWithDigiUpdate(pMuonboyRTAbstract,pMuonRecDigiStoreAth,m_WireSagOn);
  //   if ( sc.isFailure() ) {
  //     ATH_MSG_FATAL( " GetMuonboyRTAbstractWithDigiUpdate failed " );
  //     return( StatusCode::FAILURE );
  //   }
  // }else{
    sc=p_IMboyRTTool->GetMuonboyRTAbstractWithDigiUpdateT0Ctrl(pMuonboyRTAbstract,pMuonRecDigiStoreAth,m_WireSagOn);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( " GetMuonboyRTAbstractWithDigiUpdateT0Ctrl failed " );
      return( StatusCode::FAILURE );
    }
  // }

  return StatusCode::SUCCESS;

}

StatusCode MboySvc::GetTrackCollection(
                                const MB_TrackStoreAth* pMB_TrackStoreAth,
                                TrackCollection* pTrackCollection,
                                TrackCollection* pTrackCollection_MSonly
){

  StatusCode sc(StatusCode::SUCCESS,false);


  // double DoT0Fit = GetVariableByName(p_MboyVarByName,"DoT0Fit");
  // if ( DoT0Fit < 0.5 ){
  //   sc = p_IMuonboyToTrackTool->convertCollection(
  //                                                pMB_TrackStoreAth,
  // 		                                 pTrackCollection,
  // 		                                 pTrackCollection_MSonly
  // 		                                );
  // }else{
    sc = p_IMuonboyToTrackToolAdjustableT0->convertCollection(
                                                 pMB_TrackStoreAth,
		                                 pTrackCollection,
		                                 pTrackCollection_MSonly
		                                );
  // }
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "convertCollection failed" );
  }
  
   
  return sc;
   
}
StatusCode MboySvc::GetTrackIP(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
){

  StatusCode sc(StatusCode::SUCCESS,false);


  // double DoT0Fit = GetVariableByName(p_MboyVarByName,"DoT0Fit");
  // if ( DoT0Fit < 0.5 ){
  //   sc = p_IMuonboyToTrackTool->convertIndividualAtIP(
  //                                                pMB_TrackAth,
  // 		                                 pTrack
  // 		                                );
  // }else{
    sc = p_IMuonboyToTrackToolAdjustableT0->convertIndividualAtIP(
                                                 pMB_TrackAth,
		                                 pTrack
		                                );
  // }
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GetTrackIP failed" );
  }
  
   
  return sc;
   
}
StatusCode MboySvc::GetTrackMS(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
){

  StatusCode sc(StatusCode::SUCCESS,false);


  // double DoT0Fit = GetVariableByName(p_MboyVarByName,"DoT0Fit");
  // if ( DoT0Fit < 0.5 ){
  //   sc = p_IMuonboyToTrackTool->convertIndividualAtMS(
  //                                                pMB_TrackAth,
  // 		                                 pTrack
  // 		                                );
  // }else{
    sc = p_IMuonboyToTrackToolAdjustableT0->convertIndividualAtMS(
                                                 pMB_TrackAth,
		                                 pTrack
		                                );
  // }
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GetTrackMS failed" );
  }
  
   
  return sc;
   
}

StatusCode MboySvc::GetSegmentCollection(
  			           const MB_SegmentStoreAth* pMB_SegmentStoreAth,
        		           Trk::SegmentCollection* pSegmentCollection
                                   , int clean
){

  StatusCode sc = StatusCode::SUCCESS;

  // double DoT0Fit = GetVariableByName(p_MboyVarByName,"DoT0Fit");
  // if ( DoT0Fit < 0.5 ){
  //   sc = p_IMuonboyToSegmentTool->convertCollection(
  //                                                  pMB_SegmentStoreAth,
  // 		                                   pSegmentCollection ,clean
  // 		                                  );
  // }else{
    sc = p_IMuonboyToSegmentToolAdjustableT0->convertCollection(
                                                   pMB_SegmentStoreAth,
		                                   pSegmentCollection ,clean
		                                  );
  // }

  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "convertCollection failed" );
    return( StatusCode::FAILURE );
  }


  return StatusCode::SUCCESS;
   
}

std::vector<const Muon::MuonSegment*>* MboySvc::GetSegmentCollection(
  			           const MB_SegmentStoreAth* pMB_SegmentStoreAth
                                   , int clean
){

  // double DoT0Fit = GetVariableByName(p_MboyVarByName,"DoT0Fit");
  // if ( DoT0Fit < 0.5 ){
  //   return p_IMuonboyToSegmentTool->convertCollection( pMB_SegmentStoreAth  ,clean);
  // }else{
    return p_IMuonboyToSegmentToolAdjustableT0->convertCollection( pMB_SegmentStoreAth,clean );
  // }

}

StatusCode MboySvc::GetParticleCollection(
                                   const TrackCollection*       pTrackCollection,
                                   Rec::TrackParticleContainer* pTrackParticleContainer
){

  StatusCode sc = StatusCode::SUCCESS;


  sc = p_IMuonboyToParticleTool->convertCollection(
                                                  pTrackCollection,
		                                  pTrackParticleContainer
		                                 );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "convertCollection failed" );
    return( StatusCode::FAILURE );
  }


  return StatusCode::SUCCESS;
   
}

StatusCode MboySvc::MboyDumpSegmentCollectionToolDoDump(
                                                  std::string SegmentCollectionLocation,
                                                  std::string TrackCollectionLocation,
		                                  std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;


  sc = p_IMboyDumpSegmentCollectionTool->DoDump(
  					       SegmentCollectionLocation,
        				       TrackCollectionLocation,
        				       pOutFile
					      );
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "DoDump failed " );
  }


  return StatusCode::SUCCESS;
   
}
StatusCode MboySvc::MboyDumpSegmentCollectionToolDoDump(
                                                  std::string SegmentCollectionLocation,
		                                  std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;


  sc = p_IMboyDumpSegmentCollectionTool->DoDump(
  					       SegmentCollectionLocation,
        				       pOutFile
					      );
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "DoDump failed " );
  }


  return StatusCode::SUCCESS;
   
}
StatusCode MboySvc::MboyDumpTrackCollectionToolDoDump(
                                                std::string TrackCollectionLocation,
		                                std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;


  sc = p_IMboyDumpTrackCollectionTool->DoDump(
  					     TrackCollectionLocation,
        				     pOutFile
        				    );
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "DoDump failed " );
  }


  return StatusCode::SUCCESS;
   
}
StatusCode MboySvc::MboyDumpTrackParticleContainerToolDoDump(
                                                       std::string TrackParticleContainerLocation,
		                                       std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;


  sc = p_IMboyDumpTrackParticleContainerTool->DoDump(
  						    TrackParticleContainerLocation,
  						    pOutFile
  						   );
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "DoDump failed " );
  }


  return StatusCode::SUCCESS;
   
}

StatusCode MboySvc::DoCollectMatter( 
                              MuonboyBFAbstract*	pMuonboyBFAbstract,
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
)
{

  double Xpos	= (pTrackParameters->position()).x() ;
  double Ypos	= (pTrackParameters->position()).y() ;
  double Zpos	= (pTrackParameters->position()).z() ;
  double Xmom	= (pTrackParameters->momentum()).x() ;
  double Ymom	= (pTrackParameters->momentum()).y() ;
  double Zmom	= (pTrackParameters->momentum()).z() ;
  double Charge =  pTrackParameters->charge()	     ;

  p_MuonboyInterfaceAbstract->DoCollectMatter(
                                pMuonboyBFAbstract,
                                AtCosmMatter,
                                Xpos,
		                Ypos,
		                Zpos,
                                Xmom,
		                Ymom,
		                Zmom,
		                Charge,
				pVectorOfMbMuSc
		               );

  return StatusCode::SUCCESS;
   
}
StatusCode MboySvc::DoCollectMatter( 
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
)
{

  StatusCode sc = StatusCode::SUCCESS;


//Get BF
  MuonboyBFAbstract* pMuonboyBFAbstract = 0;
  sc=p_IMboyBFTool->GetMuonboyBFAbstract(pMuonboyBFAbstract);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GetMuonboyBFAbstract failed" );
    return( StatusCode::FAILURE );
  }

  return DoCollectMatter(
               pMuonboyBFAbstract,
               AtCosmMatter,
               pTrackParameters ,
               pVectorOfMbMuSc
              );
}


void MboySvc::SetVariablesDefault(MboyVarByName*  pMboyVarByName){
  p_MuonboyInterfaceAbstract->SetVariablesDefault(pMboyVarByName) ;
  p_MuonboyInterfaceAbstract->Reset() ;
  
}

const MboyVarByName* MboySvc::GetSvcSetting(){ return p_MboyVarByName ;}

void MboySvc::GetAgddXmlVarNb    (int& NbVar )                                   {p_MuonboyInterfaceAbstract->GetAgddXmlVarNb    ( NbVar )              ;}
void MboySvc::GetAgddXmlVarName  (int  IVar , std::string& NameVar )             {p_MuonboyInterfaceAbstract->GetAgddXmlVarName  ( IVar , NameVar )     ;}
void MboySvc::GetAgddXmlVarValue (std::string  NameVar, int& Iok, double& Value) {p_MuonboyInterfaceAbstract->GetAgddXmlVarValue ( NameVar , Iok, Value);}

//---------------------------------------------------------------------------------------------------------------//
MB_Track* MboySvc::MboyMuonBackTrack(MboyVarByName* pMboyVarByName,MB_Track* pMB_Track )
{
 return p_MuonboyInterfaceAbstract->MboyMuonBackTrack(pMboyVarByName,pMB_Track);
}

//---------------------------------------------------------------------------------------------------------------//

StatusCode MboySvc::GetBFRTGen(
                         MuonboyBFAbstract*& pMuonboyBFAbstract,
                         MuonboyRTAbstract*& pMuonboyRTAbstract,
                         MboyGenTrackStore*& pMboyGenTrackStore,
                         MuonRecDigiStoreAth*  pMuonRecDigiStoreAth
){
  StatusCode sc = StatusCode::SUCCESS;

  pMuonboyBFAbstract = 0;
  pMuonboyRTAbstract = 0 ;
  pMboyGenTrackStore = 0 ;  

//Get BF
  sc=p_IMboyBFTool->GetMuonboyBFAbstract(pMuonboyBFAbstract);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GetMuonboyBFAbstract failed" );
    return( StatusCode::FAILURE );
  }


//Get RT
  // double DoT0Fit = GetVariableByName(p_MboyVarByName,"DoT0Fit");
  // if ( DoT0Fit < 0.5 ){
  //   sc=p_IMboyRTTool->GetMuonboyRTAbstract(pMuonboyRTAbstract,pMuonRecDigiStoreAth);
  //   if ( sc.isFailure() ) {
  //     ATH_MSG_FATAL( " GetMuonboyRTAbstract failed " );
  //     return( StatusCode::FAILURE );
  //   }
  // }else{
    sc=p_IMboyRTTool->GetMuonboyRTAbstractT0Ctrl(pMuonboyRTAbstract,pMuonRecDigiStoreAth);
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( " GetMuonboyRTAbstractT0Ctrl failed " );
      return( StatusCode::FAILURE );
    }
  // }


//Get pMboyGenTrackStore
  int TrackRecordCollectionLocationListSize =  m_TrackRecordCollectionLocationList.size() ;
  int FoundTrackRecordCollectionLocationListItem = 0 ;
  for (int TrackRecordCollectionLocationListItem=0; TrackRecordCollectionLocationListItem<TrackRecordCollectionLocationListSize ; TrackRecordCollectionLocationListItem++){
    if ( FoundTrackRecordCollectionLocationListItem == 0 ){
      std::string TheCurrentTrackRecordCollectionLocation = m_TrackRecordCollectionLocationList[TrackRecordCollectionLocationListItem] ;
      if (p_StoreGateSvc->contains<TrackRecordCollection>(TheCurrentTrackRecordCollectionLocation)) {
        FoundTrackRecordCollectionLocationListItem = 1;
        sc=p_IMboyGenTrackTool->GetMboyGenTrackStore(pMboyGenTrackStore,TheCurrentTrackRecordCollectionLocation);
        if ( sc.isFailure() ) {
          ATH_MSG_FATAL( "GetMboyGenTrackStore failed" );
          return( StatusCode::FAILURE );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
   
}

// Variables and Proprieties
void MboySvc::DefineVariables(
           MboyVarByName* pMboyVarByName,
           std::string Name1,std::string Name2,std::string Comment
){

  pMboyVarByName->Define(Name1,Name2,Comment) ;

}

void MboySvc::DefineVariables(
           MboyVarByName* pMboyVarByName,
           std::string Name1, double Value,std::string Name2,std::string Comment
){

  pMboyVarByName->Define(Name1,Name2,Comment,Value) ;

}

void MboySvc::DeclareVariablesAsProprieties(MboyVarByName*  pMboyVarByName){

  int TheSizeVar = pMboyVarByName->Size();
  for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){    
    std::string Name1 = pMboyVarByName->GetName1(IndexVar);
    double Value = pMboyVarByName->GetValue(IndexVar) ;
    m_ProprityName.push_back(Name1);
    m_ProprityValue[m_NberOfProprieties] = Value ;
    declareProperty(Name1, m_ProprityValue[m_NberOfProprieties]);
    m_NberOfProprieties = m_NberOfProprieties + 1;
  }

}

void MboySvc::ProprietiesValues2VariablesValues(MboyVarByName* pMboyVarByName){

  int TheSizeVar = pMboyVarByName->Size();
  for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){    
    std::string Name1 = pMboyVarByName->GetName1(IndexVar);
    int TheSizeProp = m_ProprityName.size();
    for (int IndexProp= 0 ; IndexProp <TheSizeProp ; IndexProp++){ 
      std::string NameProp = m_ProprityName[IndexProp];
      if ( NameProp == Name1 ){
        double Value = m_ProprityValue[IndexProp] ;
        pMboyVarByName->SetValue(IndexVar,Value);
      }
    }   
  }

}

void MboySvc::PrintOutVariables(MboyVarByName*  pMboyVarByName){

  int TheSizeVar = pMboyVarByName->Size();
  for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){    
    std::string Name1   = pMboyVarByName->GetName1(IndexVar);
    double      Value   = pMboyVarByName->GetValue(IndexVar) ;
    std::string Comment = pMboyVarByName->GetComment(IndexVar);
    Name1.resize(30,' ');
    Comment.resize(70,' ');
    ATH_MSG_INFO( "= " << Name1  << " " << Comment << " " << Value );
  }

}
double MboySvc::GetVariableByName(MboyVarByName*  pMboyVarByName, std::string Name){

  int TheSizeVar = pMboyVarByName->Size();
  for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){    
    std::string Name1 = pMboyVarByName->GetName1(IndexVar);
    if( Name1 == Name ) {
      double Value = pMboyVarByName->GetValue(IndexVar) ;
      return Value;
    }
  }
  return -999;

}

StatusCode MboySvc::SetAliLine(){

  if ( m_AlignementLock == 0 ) {

    int   AmdcABlinesStampReceived = 0 ;
    const AmdcAlineStore* pAmdcAlineStore = 0 ;
    const AmdcBlineStore* pAmdcBlineStore = 0 ;
    const AmdcIlineStore* pAmdcIlineStore = 0 ;

    StatusCode sc = p_AmdcsimrecAthenaSvc->GetAmdcAliStores( AmdcABlinesStampReceived, pAmdcAlineStore, pAmdcBlineStore, pAmdcIlineStore );
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( " GetAmdcABlineStore failed " );
      return( StatusCode::FAILURE );
    }
    if ( pAmdcAlineStore == 0 ) {
      ATH_MSG_WARNING( " pAmdcAlineStore from GetAmdcABlineStore is NULL " );
      return StatusCode::SUCCESS;
    }
    if ( pAmdcBlineStore == 0 ) {
      ATH_MSG_WARNING( " pAmdcBlineStore from GetAmdcABlineStore is NULL " );
      return StatusCode::SUCCESS;
    }

    if ( AmdcABlinesStampReceived == m_AmdcABlinesStamp ) return StatusCode::SUCCESS;

    m_AmdcABlinesStamp = AmdcABlinesStampReceived ;
    
    MboyAlineStore aMboyAlineStore ; aMboyAlineStore.Reset(); aMboyAlineStore.SetActive(1);
    MboyBlineStore aMboyBlineStore ; aMboyBlineStore.Reset(); aMboyBlineStore.SetActive(1);
    MboyIlineStore aMboyIlineStore ; aMboyIlineStore.Reset(); aMboyIlineStore.SetActive(1);
  
    int TheSize = 0;
    TheSize = pAmdcAlineStore->NberOfObjects();
    for (int Index= 0 ; Index <TheSize ; Index++){
      MboyAline aMboyAline;
      const AmdcAline* pAmdcAline = pAmdcAlineStore->GetAmdcAline(Index);
      aMboyAline.SetStationType( pAmdcAline->GetStationType() );
      aMboyAline.Setjff        ( pAmdcAline->Getjff        () );
      aMboyAline.Setjzz        ( pAmdcAline->Getjzz        () );
      aMboyAline.Setjob        ( pAmdcAline->Getjob        () );
      aMboyAline.Sets          ( pAmdcAline->Gets          () );
      aMboyAline.Setz          ( pAmdcAline->Getz          () );
      aMboyAline.Sett          ( pAmdcAline->Gett          () );
      aMboyAline.SetrotS       ( pAmdcAline->GetrotS       () );
      aMboyAline.SetrotZ       ( pAmdcAline->GetrotZ       () );
      aMboyAline.SetrotT       ( pAmdcAline->GetrotT       () );
      aMboyAlineStore.Add(aMboyAline);
    }

    TheSize = pAmdcBlineStore->NberOfObjects();
    for (int Index= 0 ; Index <TheSize ; Index++){
      MboyBline aMboyBline;
      const AmdcBline* pAmdcBline = pAmdcBlineStore->GetAmdcBline(Index);
      aMboyBline.SetStationType( pAmdcBline->GetStationType() );
      aMboyBline.Setjff        ( pAmdcBline->Getjff        () );
      aMboyBline.Setjzz        ( pAmdcBline->Getjzz        () );
      aMboyBline.Setjob        ( pAmdcBline->Getjob        () );
      aMboyBline.Setbz         ( pAmdcBline->Getbz         () );
      aMboyBline.Setbp         ( pAmdcBline->Getbp         () );
      aMboyBline.Setbn         ( pAmdcBline->Getbn         () );
      aMboyBline.Setsp         ( pAmdcBline->Getsp         () );
      aMboyBline.Setsn         ( pAmdcBline->Getsn         () );
      aMboyBline.Settw         ( pAmdcBline->Gettw         () );
      aMboyBline.Setpg         ( pAmdcBline->Getpg         () );
      aMboyBline.Settr         ( pAmdcBline->Gettr         () );
      aMboyBline.Seteg         ( pAmdcBline->Geteg         () );
      aMboyBline.Setep         ( pAmdcBline->Getep         () );
      aMboyBline.Seten         ( pAmdcBline->Geten         () );
      aMboyBlineStore.Add(aMboyBline);
    }

    TheSize = pAmdcIlineStore->NberOfObjects();
    for (int Index= 0 ; Index <TheSize ; Index++){
      MboyIline aMboyIline;
      const AmdcIline* pAmdcIline = pAmdcIlineStore->GetAmdcIline(Index);
      aMboyIline.SetStationType( pAmdcIline->GetStationType() );
      aMboyIline.Setjff        ( pAmdcIline->Getjff        () );
      aMboyIline.Setjzz        ( pAmdcIline->Getjzz        () );
      aMboyIline.Setjob        ( pAmdcIline->Getjob        () );
      aMboyIline.Setjlay       ( pAmdcIline->Getjlay       () );
      aMboyIline.Sets          ( pAmdcIline->Gets          () );
      aMboyIline.Setz          ( pAmdcIline->Getz          () );
      aMboyIline.Sett          ( pAmdcIline->Gett          () );
      aMboyIline.SetrotS       ( pAmdcIline->GetrotS       () );
      aMboyIline.SetrotZ       ( pAmdcIline->GetrotZ       () );
      aMboyIline.SetrotT       ( pAmdcIline->GetrotT       () );
      aMboyIlineStore.Add(aMboyIline);
    }

    int StatusSetAliLine = p_MuonboyInterfaceAbstract->SetAliLine(&aMboyAlineStore,&aMboyBlineStore,&aMboyIlineStore);
    if ( StatusSetAliLine !=0 ) ATH_MSG_INFO( "p_MuonboyInterfaceAbstract->SetAliLine failed " );

    if ( m_PrintAlignmentCorr == 1 ) {
      std::cout << " aMboyAlineStore activity " << aMboyAlineStore.GetActive() << std::endl;
      aMboyAlineStore.Print();
      std::cout << " aMboyBlineStore activity " << aMboyBlineStore.GetActive() << std::endl;
      aMboyBlineStore.Print();
      std::cout << " aMboyIlineStore activity " << aMboyIlineStore.GetActive() << std::endl;
      aMboyIlineStore.Print();
    }

  }

  return StatusCode::SUCCESS;
  
}
  
void MboySvc::SetMatiere(){

//Load the Tag Info
  SetMatiereTagInfo();
  
//Load the AGDD2GeoSwitches Data
  SetMatiereAGDD2GeoSwitches();

}

void MboySvc::SetMatiereTagInfo(){

  int TagInfoXMLStampReceived = p_AmdcsimrecAthenaSvc->GetTagInfoXMLStamp( ) ;
  
  if ( TagInfoXMLStampReceived < 0 ) return; 
  
  if ( TagInfoXMLStampReceived == m_TagInfoXMLStamp ) return; 
  
  m_TagInfoXMLStamp = TagInfoXMLStampReceived ;
  
  std::vector< std::pair<std::string,std::string> > TheTagInfoXML = p_AmdcsimrecAthenaSvc->GetTagInfoXML() ;
  m_TagInfoXML.clear();
  m_TagInfoXML = TheTagInfoXML ;
  p_MuonboyInterfaceAbstract->SetMatiereTagInfo(m_TagInfoXML);
  
  if (m_PrintTagInfoXML == 1) {
    ATH_MSG_INFO( " One gets from GetTagInfoXML " << TheTagInfoXML.size() << " items "
        );
    for (unsigned int Item = 0; Item < TheTagInfoXML.size(); Item++) {
      std::string TheTagInfoXMLFirst  = TheTagInfoXML[Item].first;   
      std::string TheTagInfoXMLSecond = TheTagInfoXML[Item].second;   
      ATH_MSG_INFO( "  "
          << "  " << TheTagInfoXMLFirst
          << "  " << TheTagInfoXMLSecond
          );
    }
  }

}

void MboySvc::SetMatiereAGDD2GeoSwitches(){

  int AGDD2GeoSwitchesStampReceived = p_AmdcsimrecAthenaSvc->GetAGDD2GeoSwitchesStamp() ;
  
  if ( AGDD2GeoSwitchesStampReceived < 0 ) return; 
  
  if ( AGDD2GeoSwitchesStampReceived == m_AGDD2GeoSwitchesStamp ) return; 
  
  m_AGDD2GeoSwitchesStamp = AGDD2GeoSwitchesStampReceived ;
  
  std::vector<std::string> TheAGDD2GeoSwitches =  p_AmdcsimrecAthenaSvc->GetAGDD2GeoSwitches() ;
  m_AGDD2GeoSwitches.clear();
  m_AGDD2GeoSwitches = TheAGDD2GeoSwitches ;
  p_MuonboyInterfaceAbstract->SetMatiereAGDD2GeoSwitches(m_AGDD2GeoSwitches);
  
  if (m_PrintAGDD2GeoSwitches == 1) {
    ATH_MSG_INFO( " One gets from GetAGDD2GeoSwitches " << TheAGDD2GeoSwitches.size() << " items "
        );
    for (unsigned int Item = 0; Item < TheAGDD2GeoSwitches.size(); Item++) {
      std::string TheString = TheAGDD2GeoSwitches[Item] ;   
      ATH_MSG_INFO( "  "
          << "  " << TheString
          );
    }
  }

}

void MboySvc::SetFieldStatus(){

  int FieldStatusStampReceived = p_IMboyBFTool->UpdatedToolStamp() ;
  
  if ( FieldStatusStampReceived < 0 ) return; 
  
  if ( FieldStatusStampReceived == m_FieldStatusStamp ) return; 
  
  m_FieldStatusStamp = FieldStatusStampReceived ;
  
  int getToroidBarrelOn = p_IMboyBFTool->getToroidBarrelOn() ;
  int getToroidECTAOn   = p_IMboyBFTool->getToroidECTAOn()   ;
  int getToroidECTCOn   = p_IMboyBFTool->getToroidECTCOn()   ;
  int getSolenoidOn     = p_IMboyBFTool->getSolenoidOn()     ;
  int getAllToroidOn    = p_IMboyBFTool->getAllToroidOn()    ;
  int getFieldStatusOn  = p_IMboyBFTool->getFieldStatusOn()  ;

  p_MuonboyInterfaceAbstract->SetFieldStatus(
                                             getToroidBarrelOn ,
                                             getToroidECTAOn   ,
                                             getToroidECTCOn   ,
                                             getSolenoidOn     ,
                                             getAllToroidOn    ,
                                             getFieldStatusOn 
                                            );

  if (m_PrintFieldStatus == 1) {
    ATH_MSG_INFO( " One gets from p_IMboyBFTool " );
    ATH_MSG_INFO( "  getToroidBarrelOn = " << getToroidBarrelOn );
    ATH_MSG_INFO( "  getToroidECTAOn   = " << getToroidECTAOn   );
    ATH_MSG_INFO( "  getToroidECTCOn   = " << getToroidECTCOn   );
    ATH_MSG_INFO( "  getSolenoidOn     = " << getSolenoidOn     );
    ATH_MSG_INFO( "  getAllToroidOn    = " << getAllToroidOn    );
    ATH_MSG_INFO( "  getFieldStatusOn  = " << getFieldStatusOn  );
  }

}

void MboySvc::SetDeadChannels(){

  std::vector<std::string> MultiLayerOutTobeUsed = m_MultiLayerOut;
  
  if (m_UseDeadChannelSvc == 1){

//  Dead MDT Station
    if (m_UseDeadChannelSvcMDTStation == 1){
      int size = p_MDTSummarySvc->deadStationsId().size();
      ATH_MSG_DEBUG( "Number of dead MDT Stations = " << size );
      for (int k = 0; k < size; k++) {
      
        Identifier channelId = (p_MDTSummarySvc->deadStationsId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_mdtId->stationNameString(m_mdtId->stationName(channelId))  
                             << "/" << m_mdtId->stationEta(channelId) 
                             << "/" << m_mdtId->stationPhi(channelId) 
                             << "/" << "MDT" 
                             ;  
        ATH_MSG_DEBUG( "Adding MDT Dead Station (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );

        MultiLayerOutTobeUsed.push_back(StringDeadDevice.str()) ;
  
      }
    }
    
//  Dead MDTMulitlayers
    if (m_UseDeadChannelSvcMDTMulLayer == 1){
      int size = p_MDTSummarySvc->deadMultiLayersId().size();
      ATH_MSG_DEBUG( "Number of dead MDT Multilayers = " << size );
      for (int k = 0 ; k < size; k++) {

        Identifier channelId = (p_MDTSummarySvc->deadMultiLayersId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_mdtId->stationNameString(m_mdtId->stationName(channelId))  
                             << "/" << m_mdtId->stationEta(channelId) 
                             << "/" << m_mdtId->stationPhi(channelId) 
                             << "/" << "MDT" 
                             << "/" << m_mdtId->multilayer(channelId)
                             ;  
        ATH_MSG_DEBUG( "Adding MDT Dead Multilayer (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );

        MultiLayerOutTobeUsed.push_back(StringDeadDevice.str()) ;  
  
      }

    }
    
//  Dead MDTtubes
    if (m_UseDeadChannelSvcMDTTube == 1){
      int size = p_MDTSummarySvc->deadTubesId().size();
      ATH_MSG_DEBUG( "Number of dead MDT Tube = " << size );
      for (int k = 0; k < size; k++) {

        Identifier channelId = (p_MDTSummarySvc->deadTubesId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_mdtId->stationNameString(m_mdtId->stationName(channelId))  
                             << "/" << m_mdtId->stationEta(channelId) 
                             << "/" << m_mdtId->stationPhi(channelId) 
                             << "/" << "MDT" 
                             << "/" << m_mdtId->multilayer(channelId)
                             << "/" << m_mdtId->tubeLayer(channelId)
                             << "/" << m_mdtId->tube(channelId) 
                             ;  
        ATH_MSG_DEBUG( "Adding MDT Dead tube (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );
  
      }
    }

//  Dead RPC Station
    if (m_UseDeadChannelSvcRPCStation == 1){
      int size = p_RPCSummarySvc->EffPanelId().size();
      ATH_MSG_DEBUG( "Number of RPC dead Stations = " << size );
      for (int k = 0; k < size; k++) {
  
        Identifier channelId = (p_RPCSummarySvc->EffPanelId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_rpcId->stationNameString(m_rpcId->stationName(channelId))  
                             << "/" << m_rpcId->stationEta(channelId) 
                             << "/" << m_rpcId->stationPhi(channelId) 
                             << "/" << "RPC" 
                             ;  
        ATH_MSG_DEBUG( "Adding RPC Dead Station (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );

        MultiLayerOutTobeUsed.push_back(StringDeadDevice.str()) ; 
      }
    }

//  Dead RPC Strips
    if (m_UseDeadChannelSvcRPStrip == 1){
      int size = p_RPCSummarySvc->EffStripId().size();
      ATH_MSG_DEBUG( "Number of RPC dead Strip = " << size );
      for (int k = 0; k < size; k++) {
  
        Identifier channelId = (p_RPCSummarySvc->EffStripId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_rpcId->stationNameString(m_rpcId->stationName(channelId))  
                             << "/" << m_rpcId->stationEta(channelId) 
                             << "/" << m_rpcId->stationPhi(channelId) 
                             << "/" << "RPC" 
                             << "/" << m_rpcId->doubletR(channelId)   
                             << "/" << m_rpcId->doubletZ(channelId)   
                             << "/" << m_rpcId->doubletPhi(channelId) 
                             << "/" << m_rpcId->gasGap(channelId)     
                             << "/" << m_rpcId->measuresPhi(channelId)
                             << "/" << m_rpcId->strip(channelId)      
                             ;  
        ATH_MSG_DEBUG( "Adding RPC Dead Strips (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );

        MultiLayerOutTobeUsed.push_back(StringDeadDevice.str()) ; 
      }
    }

//  Dead TGC Station
    if (m_UseDeadChannelSvcTGCStation == 1){
      int size = p_TGCSummarySvc->deadStationsId().size();
      ATH_MSG_DEBUG( "Number of dead TGC Stations = " << size );
      for (int k = 0; k < size; k++) {
  
        Identifier channelId = (p_TGCSummarySvc->deadStationsId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_tgcId->stationNameString(m_tgcId->stationName(channelId))  
                             << "/" << m_tgcId->stationEta(channelId) 
                             << "/" << m_tgcId->stationPhi(channelId) 
                             << "/" << "TGC" 
                             ;  
        ATH_MSG_DEBUG( "Adding TGC Dead Station (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );

        MultiLayerOutTobeUsed.push_back(StringDeadDevice.str()) ; 

      }
    }

//  Dead CSC Station
    if (m_UseDeadChannelSvcCSCStation == 1){
      int size = p_CSCSummarySvc->deadStationsId().size();
      ATH_MSG_DEBUG( "Number of dead CSC Stations = " << size );
      for (int k = 0; k < size; k++) {

        Identifier channelId = (p_CSCSummarySvc->deadStationsId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_cscId->stationNameString(m_cscId->stationName(channelId))  
                             << "/" << m_cscId->stationEta(channelId) 
                             << "/" << m_cscId->stationPhi(channelId) 
                             << "/" << "CSC" 
                             ;  
        ATH_MSG_DEBUG( "Adding CSC Dead Station (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );

        MultiLayerOutTobeUsed.push_back(StringDeadDevice.str()) ; 

      }
    }

//  Dead CSC Station
    if (m_UseDeadChannelSvcCSWireLayer == 1){
      int size = p_CSCSummarySvc->deadWireLayersId().size();
      ATH_MSG_DEBUG( "Number of dead CSC Stations = " << size );
      for (int k = 0; k < size; k++) {

        Identifier channelId = (p_CSCSummarySvc->deadWireLayersId()[k]);

        std::stringstream StringDeadDevice;
        StringDeadDevice <<            m_cscId->stationNameString(m_cscId->stationName(channelId))  
                             << "/" << m_cscId->stationEta(channelId) 
                             << "/" << m_cscId->stationPhi(channelId) 
                             << "/" << "CSC" 
                             << "/" << m_cscId->chamberLayer(channelId) 
                             << "/" << m_cscId->wireLayer(channelId)    
                             ;  
        ATH_MSG_DEBUG( "Adding CSC Dead wire layer (Identifier/string): " << channelId << " / " << StringDeadDevice.str() );

        MultiLayerOutTobeUsed.push_back(StringDeadDevice.str()) ; 

      }
    }

  }

  p_MuonboyInterfaceAbstract->SetDeadChannels(MultiLayerOutTobeUsed);

}

// Initialize Core
StatusCode MboySvc::InitializeCore() {

  ATH_MSG_INFO( "InitializeCore() called " );

//InitializeCore should called only once
  if (m_IsUsable)return StatusCode::SUCCESS;
  
  if (!m_IMboyBFToolUpdatedToolDONE) {
    ATH_MSG_INFO( "InitializeCore() called BUT m_IMboyBFToolUpdatedToolDONE false    " );
    return StatusCode::SUCCESS;
  }

  if ( !m_AmdcsimrecAthenaSvcUpdatedSvcDONE ){
    ATH_MSG_INFO( "InitializeCore() called BUT m_AmdcsimrecAthenaSvcUpdatedSvcDONE is false    " );
    return StatusCode::SUCCESS;
  }

//Cache Amdc/Agdd strings
  std::string AmdcString = p_AmdcsimrecAthenaSvc->GetAmdcString();
  std::string AgddString = p_AmdcsimrecAthenaSvc->GetAgddString();

//Get BF
  MuonboyBFAbstract* pMuonboyBFAbstract = 0;
  StatusCode sc=p_IMboyBFTool->GetMuonboyBFAbstract(pMuonboyBFAbstract);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GetMuonboyBFAbstract failed" );
    return( StatusCode::FAILURE );
  }

//Load matiere data
  SetMatiere();

//Set Field Status
  SetFieldStatus() ;

//Initialize p_MuonboyInterfaceAbstract
  p_MuonboyInterfaceAbstract->initialize(AmdcString,AgddString,pMuonboyBFAbstract);

//Set configuration
  p_MuonboyInterfaceAbstract->SetConfiguration(p_MboyVarByName);

//Load Dead Channels
  SetDeadChannels();

//Set Alignement corrections
  sc=SetAliLine();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "SetAliLine failed" );
    return( StatusCode::FAILURE );
  }

  m_IsUsable = true ;

  ATH_MSG_INFO( "InitializeCore Done     " );
  
  return StatusCode::SUCCESS;

}
 
StatusCode MboySvc::IMboyBFToolUpdatedTool(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "IMboyBFToolUpdatedTool called     " );  

  m_IMboyBFToolUpdatedToolDONE = true ;

  if ( !(p_AmdcsimrecAthenaSvc->UsableSvc()) ) {
    ATH_MSG_INFO( "BUT p_AmdcsimrecAthenaSvc NOT usable yet " );  
    return StatusCode::SUCCESS;
  }
  
  StatusCode sc = InitializeCore() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "InitializeCore failed" ); 
    return StatusCode::FAILURE;
  }

//Return if not yet usuable
  if ( !UsableSvc() ) return StatusCode::SUCCESS;
  
//Set Field Status
  SetFieldStatus() ;

  return StatusCode::SUCCESS;

}
StatusCode MboySvc::AmdcsimrecAthenaSvcUpdatedSvc(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "AmdcsimrecAthenaSvcUpdatedSvc called     " );  

  if ( !(p_AmdcsimrecAthenaSvc->UsableSvc()) ) {
    ATH_MSG_INFO( "BUT p_AmdcsimrecAthenaSvc found NOT usable yet  " );  
    return StatusCode::SUCCESS;
  }else{
    m_AmdcsimrecAthenaSvcUpdatedSvcDONE = true ;
    ATH_MSG_INFO( "AND p_AmdcsimrecAthenaSvc found usable   " );  
  }

  StatusCode sc = InitializeCore() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "InitializeCore failed" ); 
    return StatusCode::FAILURE;
  }

//Return if not yet usuable
  if ( !UsableSvc() ) return StatusCode::SUCCESS;
  
//Load matiere data
  SetMatiere();

//Set Alignement corrections
  sc=SetAliLine();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "SetAliLine failed" );
    return( StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;

}

bool MboySvc::UsableSvc()      {return m_IsUsable     ;}
bool MboySvc::InitializedSvc() {return m_IsInitialized;}
StatusCode MboySvc::UpdatedSvc(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "UpdatedSvc done     " );
  return StatusCode::SUCCESS;
}

StatusCode MboySvc::regFcnIMboyBFToolUpdatedTool()
{

  StatusCode sc = p_detStore->regFcn(
                                     &IMboyBFTool::UpdatedTool,dynamic_cast<IMboyBFTool*>(&*p_IMboyBFTool),
                                     &IMboySvc::IMboyBFToolUpdatedTool,dynamic_cast<IMboySvc*>(this),true
                                    );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on MboySvc::IMboyBFToolUpdatedTool from IMboyBFTool::UpdatedTool " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on MboySvc::IMboyBFToolUpdatedTool from IMboyBFTool::UpdatedTool " );

  sc = p_detStore->regFcn(
                          &IMboySvc::IMboyBFToolUpdatedTool,dynamic_cast<IMboySvc*>(this),
                          &IMboySvc::UpdatedSvc,dynamic_cast<IMboySvc*>(this),true
                         );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on MboySvc::UpdatedSvc from MboySvc::IMboyBFToolUpdatedTool " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on MboySvc::UpdatedSvc from MboySvc::IMboyBFToolUpdatedTool " );

  return StatusCode::SUCCESS;
}
StatusCode MboySvc::regFcnAmdcsimrecAthenaSvcUpdatedSvc()
{

  StatusCode sc = p_detStore->regFcn(
                         &AmdcsimrecAthenaSvc::UpdatedSvc,(&*p_AmdcsimrecAthenaSvc),
                         &IMboySvc::AmdcsimrecAthenaSvcUpdatedSvc,dynamic_cast<IMboySvc*>(this),true
                         );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on MboySvc::AmdcsimrecAthenaSvcUpdatedSvc from AmdcsimrecAthenaSvc::UpdatedSvc " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on MboySvc::AmdcsimrecAthenaSvcUpdatedSvc from AmdcsimrecAthenaSvc::UpdatedSvc" );
  sc = p_detStore->regFcn(
              &IMboySvc::AmdcsimrecAthenaSvcUpdatedSvc,dynamic_cast<IMboySvc*>(this),
              &IMboySvc::UpdatedSvc,dynamic_cast<IMboySvc*>(this),true
              );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on MboySvc::UpdatedSvc from MboySvc::AmdcsimrecAthenaSvcUpdatedSvc " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on MboySvc::UpdatedSvc from MboySvc::AmdcsimrecAthenaSvcUpdatedSvc " );

  return StatusCode::SUCCESS;
  
}
//---------------------------------------------------------------------------------------------------------------//
// find track in the muon system 
MbProtoTrack* MboySvc::find(
        MboyVarByName* pMboyVarByName,
        MbProtoTrack*  pMbProtoTrack ,
        MbProtoTrack*  pTargetSegmentsProtoTrack 
){

  return p_MuonboyInterfaceAbstract->find(
                                         pMboyVarByName,
                                         pMbProtoTrack,
                                         pTargetSegmentsProtoTrack
					 );

}
MbProtoTrack* MboySvc::refine(
        MboyVarByName*        pMboyVarByName,
        MbProtoTrack*         pMbProtoTrack,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth,
        MB_TrackStoreAth*     pMB_TrackStoreAth
){

  return p_MuonboyInterfaceAbstract->refine(
                                         pMboyVarByName,
                                         pMbProtoTrack,
                                         pMuonRecDigiStoreAth,
                                         pMB_TrackStoreAth
					 );

}
void MboySvc::SetAMbDRAT (
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
){
  if ( pMbDRAT == 0 ) return ;

  int StatusMuonboyInterface = p_MuonboyInterfaceAbstract->SetAMbDRAT(
                                                                 pMboyVarByName,
                                                                 pMbDRAT
								 );

  if ( StatusMuonboyInterface !=0 ){
    ATH_MSG_FATAL( " SetAMbDRAT failed " );
    return ;
  }

}

void MboySvc::LoadEvent(
        MboyVarByName* pMboyVarByName,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth 
){
  if ( pMboyVarByName == 0 ) return ;
  if ( pMuonRecDigiStoreAth == 0 ) return ;

//Get BF, RT and pMboyGenTrackStore
  MuonboyBFAbstract* pMuonboyBFAbstract = 0;
  MuonboyRTAbstract* pMuonboyRTAbstract = 0 ;
  MboyGenTrackStore* pMboyGenTrackStore = 0 ;  
  StatusCode sc = GetBFRTGen(
                  pMuonboyBFAbstract,
                  pMuonboyRTAbstract,
                  pMboyGenTrackStore,
                  pMuonRecDigiStoreAth
                 );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " GetBFRTGen failed " );
    return ;
  }

  MboyPosDigiChoice aMboyPosDigiChoice ; 
  int StatusMuonboyInterface = p_MuonboyInterfaceAbstract->LoadEvent(
                                                                 pMboyVarByName,
								 pMuonboyBFAbstract,
								 pMuonboyRTAbstract,
								 &aMboyPosDigiChoice,
								 pMuonRecDigiStoreAth
								 );

  if ( StatusMuonboyInterface !=0 ){
    ATH_MSG_FATAL( " LoadEvent failed " );
    return ;
  }

}
