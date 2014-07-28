/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/muFast.h"
#include "GaudiKernel/ThreadGaudi.h"

#include "TrigMuonEvent/MuonFeatureDetails.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigmuFast/MuData.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "TrigmuFast/LVL1emu.h"
#include "TrigmuFast/RpcPatRec.h"
#include "TrigmuFast/MdtPatRec.h"
#include "TrigmuFast/MuonFex.h"
#include "TrigmuFast/PtLUTSvc.h"

#include "TrigmuFast/TriggerMap.h"

#include "MuCalDecode/CalibEvent.h"
#include "MuCalDecode/CalibUti.h"

#include "PathResolver/PathResolver.h"

#include "Identifier/IdentifierHash.h"

#include "RPCcablingInterface/RpcPadIdHash.h" 

#include "TrigT1Result/RoIBResult.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#ifdef INSTRUMENTATION
//#include "instrumentation/Instrumentation.h"
#endif

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"

#include "GeneratorObjects/McEventCollection.h"  // include for retrieving the Montecarlo
#include "HepMC/GenEvent.h"                      // thru for LUT computing

#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "eformat/DetectorMask.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"

#include "TrigConfL1Data/CTPConfig.h"

#include "circ/Circ.h"
#include "circ/Circservice.h"

#include <sstream>
#include <sys/stat.h>

#include <string.h>

const int MAX_TGC_HITS = 100;
const int MAX_RPC_HITS = 100;
const int MAX_MDT_HITS = 100;
const int MAX_ROB = 10;
const int MAX_CSM = 30;

void TgcRdo2ByteStream(TgcRawData*, MsgStream&,
                       LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTHIT&,
		       LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRIPLETSTRIP&,
		       LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRACKLET&,
		       LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_HIPT&,
		       LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_SL&);
		       
// double const FIFTEEN_DEG    = 15.0 * M_PI / 180.0;
// double const THIRTY_DEG     = 30.0 * M_PI / 180.0;
// double const FORTY_FIVE_DEG = 45.0 * M_PI / 180.0;
double const PI_OVER_4 = M_PI / 4.0;
double const PI_OVER_8 = M_PI / 8.0;
double const PHI_RANGE = 12.0 / PI_OVER_8;

const int MAX_ERRORS= 20;

muFast::muFast(const std::string& name, ISvcLocator* pSvcLocator): 
   HLT::FexAlgo(name, pSvcLocator),
    m_rpc_converter("Muon__RPC_RawDataProviderTool"),
    m_mdt_converter("Muon__MDT_RawDataProviderTool"),
    m_tgc_converter("Muon__TGC_RawDataProviderTool"),
    m_csc_converter("Muon__CSC_RawDataProviderTool"),
    m_rpc_geometry("RPCgeometrySvc",""),
    m_tgc_geometry("TGCgeometrySvc",""),
    m_csc_geometry("CSCgeometrySvc",""),
    m_mdt_cabling ("MDTcablingSvc",""),
    m_backExtrapolator("TrigMuonBackExtrapolator"),
    m_lvl1ConfigSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc",""),
    m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
    m_recTGCRoiSvc("LVL1TGC::TGCRecRoiSvc",""),
    m_CalBufferNameDetermined(""), m_TestBufferNameDetermined("")
{
    m_RobIds.clear();
    m_MdtId.clear();
    
    declareProperty( "RPC_Geometry", m_rpc_geometry);
    declareProperty( "TGC_Geometry", m_tgc_geometry);
    declareProperty( "CSC_Geometry", m_csc_geometry);
    declareProperty( "MDT_Cabling", m_mdt_cabling);
    
    declareProperty( "RPC_RawDataProvider", m_rpc_converter);
    declareProperty( "MDT_RawDataProvider", m_mdt_converter);
    declareProperty( "TGC_RawDataProvider", m_tgc_converter);
    declareProperty( "CSC_RawDataProvider", m_csc_converter);
    
    declareProperty( "BackExtrapolator", m_backExtrapolator, "public tool for back extrapolating the muon tracks to the IV");
    
    declareProperty( "LVL1ConfigSvc", m_lvl1ConfigSvc, "LVL1 Config Service");
    declareProperty( "RPCRecRoiSvc",  m_recRPCRoiSvc, "Reconstruction of RPC RoI");
    declareProperty( "TGCRecRoiSvc",  m_recTGCRoiSvc, "Reconstruction of TGC RoI");


    // standard configuration
    declareProperty("ParamSetFile", m_paramSet);
    declareProperty("ROBmapperFile",m_ROBmapper_filename="RoiROBmap.data");
    declareProperty("ROImapperFile",m_ROImapper_filename="MuonRoImapping_M7.data");
    declareProperty("LUTFile",m_LUT_filename="lut.data");
    declareProperty("UseCalibration",m_use_calib=false);
    declareProperty("UseTiltedBField",m_tilted_B_field=false);
    declareProperty("BetaSlowParticles",m_particle_beta=1.);
    declareProperty("MuonCalBufferName",m_CalBufferName="");
    declareProperty("MuonCalBufferSize",m_CalBufferSize=1024*1024);

    declareProperty("UseLUTForMC",   m_use_mcLUT=true);
    declareProperty("UsePol2ForEPt", m_usePol2ForEPt=true);

    // configure offline like analysis
    declareProperty("Timing", m_timing=false);
    declareProperty("FEXntuple", m_fexntp=false);
    declareProperty("LUTntuple", m_lutntp=false);
    declareProperty("MONntuple", m_monntp=false);
    declareProperty("KeyForTruth", m_key_for_truth="TruthEvent");
    
    // Configure the run environment
    declareProperty("OnlineRun",m_OnlineRun=false);
    declareProperty("CreateRoIFromDescriptor",m_create_roi=false);
    declareProperty("DetMaskCheck", m_check_detector_mask=true);
    declareProperty("DetMaskCheckOnEvent", mp_detector_mask_on_event=true);
    declareProperty("MaskUncCMAch",m_maskUncCMAch=true);
    declareProperty("TrigOnPhiOnly",m_trigOnPhiOnly=false);
    declareProperty("UseRpcTriggerPattern",m_useRpcTrigPatt=false);
    declareProperty("RpcTimeDelay",m_RpcTimeDelay=0.);
    declareProperty("DoMuonCalStream",m_doCalStream=false);
    declareProperty("DoMuonCalBuild",m_doCalBuild=false);
    declareProperty("PEBInfoSGKey",m_pEBInfoSGKey="MuonCalibration");
    declareProperty("ConfirmCalTrack",m_confirmCalTrack=true);
    declareProperty("histoPathBase",m_histo_path_base="/EXPERT/");
    declareProperty("DomuFastESD",m_output_details=true);
    declareProperty("ESD_EXT_size",m_esd_ext_size=100);
    declareProperty("ESD_ROB_size",m_esd_rob_size=10);
    declareProperty("ESD_CSM_size",m_esd_csm_size=30);
    declareProperty("ESD_LV1_size",m_esd_lv1_size=20);
    declareProperty("ESD_RPC_size",m_esd_rpc_size=100);
    declareProperty("ESD_TGC_size",m_esd_tgc_size=50);
    declareProperty("ESD_MDT_size",m_esd_mdt_size=100);
    
    // printouts configuration
    declareProperty("EvtToShow",m_evtToShow=100);
    declareProperty("MUlvl1INFO",m_MUlvl1_info=false);
    declareProperty("MUtrackINFO",m_MUtrack_info=false);
    declareProperty("MUroadsINFO",m_MUroads_info=false);
    declareProperty("MUdecoINFO",m_MUdeco_info=false);
    declareProperty("MUcontINFO",m_MUcont_info=false);
    declareProperty("MUfitINFO",m_MUfit_info=false);
    declareProperty("MUsagINFO",m_MUsag_info=false);
    declareProperty("MUptINFO",m_MUpt_info=false);
    declareProperty("MUtgcDecoINFO",m_MUtgcDeco_info=false);
    declareProperty("EsdINFO",m_Esd_info=false);
    declareProperty("FITdebug",m_FITdebug_info=false);
    declareProperty("TestString",m_test_string=""); 
    
    // Cosmic environment configuration
    declareProperty("CosmicRun",m_CosmicRun=false);
    declareProperty("BfieldOn",m_BfieldOn=true);
    declareProperty("RemoveTgcTriggers",m_remove_tgc_trigger=false);
    declareProperty("doCSCdataConversion",m_csc_data_conversion=true);
    //declareProperty("TBmultipleRoIs",m_TBmultipleRoIs=false);
    declareProperty("RobId", m_RobIds);          // additional ROBId for run
    declareProperty("MdtId", m_MdtId);           // additional MDT hash Ids for RoI 
    //declareProperty("Mdt2id", m_Mdt2id);         // MDT hash Ids for RoI set 2
    declareProperty("SubsystemId",m_subsysID=1); // corresponding to |eta| > 0
    declareProperty("RoiNumber",m_RoINumber=10); // corresponding to PAD = 2
    declareProperty("SectorId",m_sectorID=31);   // cabled sector at H8

    // TGC Road parameters
    declareProperty("TGCEtaLowMidDelta",    m_TGC_ETA_LOW_MID_DELTA  = 0.10);
    declareProperty("TGCEtaHighMidDelta",   m_TGC_ETA_HIGH_MID_DELTA = 0.05);
    declareProperty("TGCPhiMidDelta",       m_TGC_PHI_MID_DELTA      = 0.05);
    declareProperty("TGCEtaLowInnerDelta",  m_TGC_ETA_LOW_INN_DELTA  = 0.15);
    declareProperty("TGCEtaHighInnerDelta", m_TGC_ETA_HIGH_INN_DELTA = 0.10);
    declareProperty("TGCPhiInnerDelta",     m_TGC_PHI_INN_DELTA      = 0.10);
    // TGC Fit parameters
    declareProperty("TGCChi2Test",          m_TGC_CHI2_TEST          = 10.0);
    declareProperty("TGCMinWirePoints",     m_TGC_MIN_WIRE_POINTS    = 4);
    declareProperty("TGCMinStripPoints",    m_TGC_MIN_STRIP_POINTS   = 3);
    
    declareMonitoredVariable("InnMdtHits", m_inner_mdt_hits);
    declareMonitoredVariable("MidMdtHits", m_middle_mdt_hits);
    declareMonitoredVariable("OutMdtHits", m_outer_mdt_hits);
    declareMonitoredStdContainer("FitResiduals", m_fit_residuals);
    declareMonitoredVariable("Efficiency", m_efficiency);
    declareMonitoredVariable("SagInv", m_sag_inverse);
    declareMonitoredVariable("Address", m_address);
    declareMonitoredVariable("AbsPt", m_absolute_pt);
    declareMonitoredVariable("Sagitta", m_sagitta);
    declareMonitoredVariable("CalEvtSize", m_calib_event_size);
    declareMonitoredVariable("BufFreeCnt", m_buffer_free_count);
    declareMonitoredVariable("PADhit", m_barrel_pad_hit);
    declareMonitoredVariable("SEChit", m_barrel_sector_hit);
    declareMonitoredVariable("PADrec", m_barrel_pad_rec);
    declareMonitoredVariable("SECrec", m_barrel_sector_rec);
    declareMonitoredVariable("PADeff", m_barrel_pad_track_eff);
    declareMonitoredVariable("SECeff", m_barrel_sector_track_eff);
    declareMonitoredStdContainer("ResInner", m_res_inner);
    declareMonitoredStdContainer("ResOuter", m_res_outer);
    declareMonitoredStdContainer("ResMiddle", m_res_middle);
    // AS
    declareMonitoredStdContainer("LVL1Eta", m_lvl1_eta);
    declareMonitoredStdContainer("LVL1Phi", m_lvl1_phi);
    declareMonitoredStdContainer("LVL1Pt", m_lvl1_pt);
    declareMonitoredStdContainer("EmuEta", m_emu_eta);
    declareMonitoredStdContainer("EmuPhi", m_emu_phi);
    declareMonitoredStdContainer("EmuPt", m_emu_pt);
    //=AI added
    declareMonitoredStdContainer("TGCEtaHit", m_endcap_eta_hit);
    declareMonitoredStdContainer("TGCSecHit", m_endcap_phi_hit);
    declareMonitoredStdContainer("TGCEtaRec", m_endcap_eta_rec);
    declareMonitoredStdContainer("TGCSecRec", m_endcap_phi_rec);
    declareMonitoredStdContainer("TGCEtaEff", m_endcap_eta_eff);
    declareMonitoredStdContainer("TGCSecEff", m_endcap_phi_eff);

    declareMonitoredStdContainer("TrackEta", m_track_eta);
    declareMonitoredStdContainer("TrackPhi", m_track_phi);
    //=end

    declareMonitoredStdContainer("L1EmuFailedEta",    m_l1emuFailed_eta);
    declareMonitoredStdContainer("L1EmuFailedPhi",    m_l1emuFailed_phi);
    declareMonitoredStdContainer("BackExtrFailedEta", m_backextrFailed_eta);
    declareMonitoredStdContainer("BackExtrFailedPhi", m_backextrFailed_phi);
}


HLT::ErrorCode muFast::hltInitialize()
{
  msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	<< PACKAGE_VERSION << endreq;


  m_detector_mask_on_event = mp_detector_mask_on_event.value();


  StatusCode sc;
  m_CalBufferNameDetermined  = m_CalBufferName;
  m_TestBufferNameDetermined = "";
  
  m_hasMdtBarrelA = true;
  m_hasMdtBarrelC = true;
  m_hasMdtEndcapA = true;
  m_hasMdtEndcapC = true;
  m_hasRpcBarrelA = true;
  m_hasRpcBarrelC = true;
  m_hasTgcEndcapA = true;
  m_hasTgcEndcapC = true;
  m_hasCscEndcapA = true;
  m_hasCscEndcapC = true; 
  
  // setting up the list of the statically disabled ROBs
  m_rob_disabled.clear();
  
  // ========== SET THE ENVIRONMENT FOR THE ONLINE/OFFLINE RUN ===============
  if(m_OnlineRun)
  {
     // removed the forcing on DetMask for the time being
    
     m_detector_mask_on_event = false;  // detector
     //m_timing = false;  // no internal timing when run in L2PU
     m_fexntp = false;  // no ntuple production when run in the L2PU
     m_maskUncCMAch = false;  // uncheck the masking of the RPC uncabled channels
     // for the time being
    
     // try to get calibration buffer name and size from oks
     IJobOptionsSvc* jobOptionsSvc;
     sc = service("JobOptionsSvc", jobOptionsSvc);
     if (sc.isFailure()) {
        msg() << MSG::ERROR << "Could not find JobOptionsSvc" << endreq;
        return HLT::BAD_JOB_SETUP;
     } else {
        IService* svc = dynamic_cast<IService*>(jobOptionsSvc);
        if(svc != 0 ) {
           msg() << MSG::INFO << " Algorithm = " << name() 
                 << " is connected to JobOptionsSvc Service = "
                 << svc->name() << endreq;
        }

        // Overwrite the properties with values from the catalogue using the non existing algorithm
        // 'MuonL2CalibrationConfig'
        sc = jobOptionsSvc->setMyProperties("MuonL2CalibrationConfig",this);
        if (sc.isFailure()) {
           msg() << MSG::WARNING 
                 << "Could not set properties from JobOptionsSvc catalogue; " 
                 << "use the defaults" << endreq;
        } else {
           msg() << MSG::INFO << "MuonCalBufferName: " << m_CalBufferName 
                 << endreq;
           msg() << MSG::INFO << "MuonCalBufferSize: " << m_CalBufferSize 
                 << endreq;
        }
      
        // Find the Data Flow application name
        bool df_found = false;
        std::string DF_ApplicationName  = "";
        std::string DF_MachineName      = "";
        std::string DF_SegmentName      = "";
        std::string DF_TemplateInstance = "";
        std::string DF_TemplateName     = "";
        std::string DF_Pid              = "";
        std::string nameSep = "_";
      
        const std::vector<const Property*>* dataFlowProps = jobOptionsSvc->getProperties("DataFlowConfig");
        if(dataFlowProps!=0) {
           for ( std::vector<const Property*>::const_iterator cur = dataFlowProps->begin();
                 cur != dataFlowProps->end(); cur++) {
              if ( (*cur)->name() == "DF_ApplicationName" ) {
                 msg() << MSG::INFO << " DF_ApplicationName = " << (*cur)->toString() << endreq;
                 DF_ApplicationName  = (*cur)->toString();
                 df_found = true;
                 int pos = DF_ApplicationName.find_last_of("-");
                 m_l2pu_id = atoi(
                                  DF_ApplicationName.substr(pos+1,DF_ApplicationName.length()).c_str());
              }
              if ( (*cur)->name() == "DF_MachineName" ) {
                 msg() << MSG::INFO << " DF_MachineName= " << (*cur)->toString() << endreq;
                 DF_MachineName  = (*cur)->toString();
              }
              if ( (*cur)->name() == "DF_SegmentName" ) {
                 msg() << MSG::INFO << " DF_SegmentName= " << (*cur)->toString() << endreq;
                 DF_SegmentName  = (*cur)->toString();
              }
              if ( (*cur)->name() == "DF_TemplateInstance" ) {
                 msg() << MSG::INFO << " DF_TemplateInstance= " << (*cur)->toString() << endreq;
                 DF_TemplateInstance  = (*cur)->toString();
              }
              if ( (*cur)->name() == "DF_TemplateName" ) {
                 msg() << MSG::INFO << " DF_TemplateName= " << (*cur)->toString() << endreq;
                 DF_TemplateName  = (*cur)->toString();
              }
              if ( (*cur)->name() == "DF_Pid" ) {
                 msg() << MSG::INFO << " DF_Pid= " << (*cur)->toString() << endreq;
                 DF_Pid  = (*cur)->toString();
              }
              if ( (*cur)->name() == "DF_Statically_Disabled_ROB_IDs" ) {
                 SimpleProperty< std::vector<uint32_t> > disabledROB;
                 disabledROB.assign(**cur);
                 msg() << MSG::INFO << "Read from OKS = " << disabledROB.value().size() 
                       << " disabled ROB IDs; filtering the muon ones ..." << endreq;
          
                 for (unsigned int i=0; i<disabledROB.value().size(); i++) {
                    uint32_t rob = disabledROB.value()[i];
                    uint32_t subsystem = rob >> 16;
	      
                    if(subsystem == 0x61 || subsystem == 0x62 || subsystem == 0x63 ||
                       subsystem == 0x64 || subsystem == 0x65 || subsystem == 0x66 ||
                       subsystem == 0x67 || subsystem == 0x68 || subsystem == 0x69 ||
                       subsystem == 0x6a) {
              
                       msg() << MSG::INFO << MSG::hex << "0x" << rob << MSG::dec
                             << " belong to the muon system; added into the list."
                             << endreq;
                       m_rob_disabled.push_back(rob);
	      
                    } else {
                       msg() << MSG::INFO << MSG::hex << "0x" << rob << MSG::dec
                             << " does not belong to the muon system."
                             << endreq;
                    } 
                 }

              }
           }
        }

      // 2012.11.13 EP test on buffer name added to avoid
      // fake errors in offline tests

      std::string buf_name = m_CalBufferName;

      // 2012.04.04 KN mods
      // if ( !df_found ) {
      if ( !df_found || ! m_doCalStream || (buf_name.size() == 0)) {
        if( ! df_found )  msg() << MSG::WARNING << " Property for DF_ApplicationName not found." << endreq;
        msg() << MSG::INFO << " Not opening the Calibration Buffer" << endreq;
        m_CalBufferName = "";
	m_CalBufferNameDetermined = "";
      } else {
        std::string url_conv = "buffer://";
        std::string tmp = (buf_name.find_first_of(':')!=std::string::npos)?
	                   buf_name : url_conv + buf_name;
	
           if(DF_Pid!="") {
              m_CalBufferNameDetermined = tmp + nameSep + DF_Pid;
              msg() << MSG::INFO << " Complete buffer name: " << m_CalBufferNameDetermined << endreq;
              m_TestBufferNameDetermined = "TestBuffer" + nameSep + DF_Pid;
              msg() << MSG::INFO << " Test file for buffer: " << m_TestBufferNameDetermined << endreq;
           } else if(DF_TemplateInstance!="") {
              m_CalBufferNameDetermined = tmp + nameSep + DF_TemplateInstance;
              msg() << MSG::INFO << " Complete buffer name: " << m_CalBufferNameDetermined << endreq;
              m_TestBufferNameDetermined = "TestBuffer" + nameSep + DF_TemplateInstance;
              msg() << MSG::INFO << " Test file for buffer: " << m_TestBufferNameDetermined << endreq;
           } else {
              int pos = DF_ApplicationName.find_last_of(":");
              std::string instance = DF_ApplicationName.substr(pos+1,
                                                               DF_ApplicationName.length());
              m_CalBufferNameDetermined = tmp + nameSep + instance;
              msg() << MSG::INFO << " Complete buffer name: " << m_CalBufferNameDetermined << endreq;
              m_TestBufferNameDetermined = "TestBuffer" + nameSep + instance;
              msg() << MSG::INFO << " Test file for buffer: " << m_TestBufferNameDetermined << endreq;
           }
        }
      
        // release JobOptionsSvc
        unsigned long mjcounter = jobOptionsSvc->release();
        msg() << MSG::INFO << " --> Release JobOptionsSvc Service, Counter = " 
              << mjcounter << endreq;
     }
  }
  
  
  msg() << MSG::INFO << "==== Global  configuration ====" << endreq;
  msg() << MSG::INFO << m_OnlineRun << endreq;
  msg() << MSG::INFO << m_CosmicRun << endreq;
  msg() << MSG::INFO << m_create_roi << endreq;
  msg() << MSG::INFO << m_check_detector_mask << endreq;
  msg() << MSG::INFO << m_detector_mask_on_event << endreq;
  msg() << MSG::INFO << m_remove_tgc_trigger << endreq;
  msg() << MSG::INFO << m_csc_data_conversion << endreq;
  msg() << MSG::INFO << m_BfieldOn << endreq;
  msg() << MSG::INFO << m_use_calib << endreq;
  msg() << MSG::INFO << m_tilted_B_field << endreq;  
  msg() << MSG::INFO << m_LUT_filename << endreq;
  msg() << MSG::INFO << m_doCalStream << endreq;  
  msg() << MSG::INFO << m_doCalBuild << endreq; 
  msg() << MSG::INFO << m_confirmCalTrack << endreq;
  msg() << MSG::INFO << m_pEBInfoSGKey << endreq; 
  msg() << MSG::INFO << m_timing << endreq;
  
  msg() << MSG::INFO << "==== Monitor configuration ====" << endreq;
  msg() << MSG::INFO << m_output_details << endreq;
  msg() << MSG::INFO << m_esd_ext_size << endreq;
  msg() << MSG::INFO << m_esd_rob_size << endreq;
  msg() << MSG::INFO << m_esd_csm_size << endreq;
  msg() << MSG::INFO << m_esd_lv1_size << endreq;
  msg() << MSG::INFO << m_esd_rpc_size << endreq;
  msg() << MSG::INFO << m_esd_tgc_size << endreq;
  msg() << MSG::INFO << m_esd_mdt_size << endreq;
  
  msg() << MSG::INFO << "== RPC trigger configuration ==" << endreq;
  msg() << MSG::INFO << m_maskUncCMAch  << endreq;
  msg() << MSG::INFO << m_trigOnPhiOnly << endreq; 
  msg() << MSG::INFO << m_useRpcTrigPatt << endreq;
  msg() << MSG::INFO << m_RpcTimeDelay << endreq;
  
  msg() << MSG::INFO << "== TGC trigger configuration ==" << endreq;
  msg() << MSG::INFO << m_TGC_ETA_LOW_MID_DELTA  << endreq;
  msg() << MSG::INFO << m_TGC_ETA_HIGH_MID_DELTA << endreq;
  msg() << MSG::INFO << m_TGC_PHI_MID_DELTA      << endreq;
  msg() << MSG::INFO << m_TGC_ETA_LOW_INN_DELTA  << endreq;
  msg() << MSG::INFO << m_TGC_ETA_HIGH_INN_DELTA << endreq;
  msg() << MSG::INFO << m_TGC_PHI_INN_DELTA      << endreq;


  m_detector_mask_not_checked = m_check_detector_mask;


  sc = m_rpc_geometry.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " 
                        << m_rpc_geometry << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved service " << m_rpc_geometry << endreq;
  }
  
  sc = m_tgc_geometry.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " 
                        << m_tgc_geometry << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved service " << m_tgc_geometry << endreq;
  }
  
  sc = m_csc_geometry.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " 
                        << m_csc_geometry << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved service " << m_csc_geometry << endreq;
  }
  
  sc = m_mdt_cabling.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " 
                        << m_mdt_cabling << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved service " << m_mdt_cabling << endreq;
  }

  sc = m_backExtrapolator.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " 
                        << m_backExtrapolator << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_backExtrapolator << endreq;
  }
  
  sc = m_rpc_converter.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " << m_rpc_converter << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_rpc_converter << endreq;
  }
  
  sc = m_mdt_converter.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " << m_mdt_converter << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_mdt_converter << endreq;
  }
  
   sc = m_tgc_converter.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " << m_tgc_converter << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_tgc_converter << endreq;
  }

  sc = m_csc_converter.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " << m_csc_converter << endreq;
    return HLT::BAD_JOB_SETUP;
  } else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_csc_converter << endreq;
  }

  sc = m_lvl1ConfigSvc.retrieve();
  m_gotL1Config = true;
  if ( sc.isFailure() ) 
  {
    msg() << MSG::WARNING << "Couldn't connect to " 
                          << m_lvl1ConfigSvc.typeAndName() << endreq;
    m_gotL1Config = false;
  } else
  {
    msg() << MSG::INFO << "Retrieved Service " << m_lvl1ConfigSvc << endreq;
  }

  sc = m_recRPCRoiSvc.retrieve();
  if ( sc.isFailure() ) 
  {
    msg() << MSG::ERROR << "Couldn't connect to " << m_recRPCRoiSvc << endreq;
    return HLT::BAD_JOB_SETUP;
  } else
  {
     msg() << MSG::INFO << "Retrieved Service " << m_recRPCRoiSvc << endreq;
  }
  
  sc = m_recTGCRoiSvc.retrieve();
  if ( sc.isFailure() ) 
  {
    msg() << MSG::ERROR << "Couldn't connect to " << m_recTGCRoiSvc << endreq;
    return HLT::BAD_JOB_SETUP;
  } else
  {
     msg() << MSG::INFO << "Retrieved Service " << m_recTGCRoiSvc << endreq;
  }

  // ==================== GET THE REGION SELECTOR TOOL =======================
  //sc = toolSvc()->retrieveTool("RegionSelector", m_pRegionSelector,this);
  sc = service("RegSelSvc", m_pRegionSelector);
  if(sc.isFailure()){
      msg() << MSG::FATAL
            << "Unable to retrieve RegionSelector"
            << endreq;
      return HLT::BAD_JOB_SETUP;
  }



  // ======================== GET ROBDATAPROVIDER ============================

  IService* svc ;
  if(StatusCode::SUCCESS != service("ROBDataProviderSvc",svc)){
    msg() << MSG::ERROR << " Can't get ROBDataProviderSvc " << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  m_pROBDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
  if(m_pROBDataProvider == 0 ) {
    msg()<<MSG::ERROR<< " Cant cast to TrigL2ROBDataProviderSvc " <<endreq;
    return HLT::BAD_JOB_SETUP ;
  }

  // ====================== INITIALIZE THE ROBMAPPER =========================

  // implement the search of RODFile trought the pathresolver utility.
  std::string ROBmapper_filename;
  ROBmapper_filename = PathResolver::find_file (m_ROBmapper_filename, "DATAPATH");
    
  m_pROBmapper = new MUON::ROBmapper(ROBmapper_filename);  
  msg() << MSG::INFO << "ROB mapper loaded from " 
        << ROBmapper_filename << endreq;

#ifndef NDEBUG
  for(int i=0;i<2;++i)
      for(int j=0;j<32;++j)
          for(int k=1;k<28;++k)
	  {
	      std::vector<uint32_t> robs;
              //robs.reserve(2);
	      m_pROBmapper->lookup(i,j,k,robs);
	      if(robs.size())
	      {
	          if( robs.size() == 2)
	          {
	              msg() << MSG::DEBUG << "ROBMAPPER. Sub, Sec, RoI =" 
		                          << i << "," << j << "," << k 
			  	          << "|-> " << std::setiosflags(std::ios::showbase)
					  << MSG::hex << robs[0] << " " 
					  << robs[1] << MSG::dec << endreq;
                  } else 
		  { 
	              msg() << MSG::DEBUG << "ROBMAPPER. Sub, Sec, RoI =" 
		                          << i << "," << j << "," << k 
				          << "|-> " << std::setiosflags(std::ios::showbase)
					  << MSG::hex << robs[0] << MSG::dec
					  << endreq;		  
		  }
	      }
	  }  
#endif

  if ( m_CosmicRun )
  {
      std::string ROImapper_filename;
      ROImapper_filename = PathResolver::find_file (m_ROImapper_filename, "DATAPATH");
      m_pROImapper = new MUON::ROImapper(ROImapper_filename);
      msg() << MSG::DEBUG << *m_pROImapper << endreq;
      msg() << MSG::INFO << "ROI mapper loaded from "
            << ROImapper_filename << endreq;  
  } else
  {
      m_pROImapper = 0;
  }
  // ========= SETTING UP THE ADDITIONAL ROBLIST AND MDT LIST FOR RUN =======

  m_robIds.clear();
  std::vector<int>::const_iterator it = m_RobIds.begin();
  while (it != m_RobIds.end())
  {
       m_robIds.push_back(*it);
       ++it;
  }
   
  // Collect the full ROB list //////////////////////////////////////////////
#ifndef NDEBUG                                                             //
  msg() << MSG::DEBUG                                                      //
        << " Additional ROBs to be accessed = "                            //
        << m_robIds.size() << endreq;                                      //
  msg() << MSG::DEBUG                                                      //
        << " List of ROBs: "  << endreq;                                   //
  for (std::vector<uint32_t>::const_iterator i=m_robIds.begin();           //
       i != m_robIds.end(); ++i)                                           //
  {                                                                        //
       msg() << MSG::DEBUG << *i << endreq;                                //
  }                                                                        //
#endif                                                                     //
  ///////////////////////////////////////////////////////////////////////////





  // ============= FILL THE INTERFACES TOWARDS THE MUON SERVICES =============
  unsigned short int info = ((1 & m_MUlvl1_info)    << 0) |
                            ((1 & m_MUtrack_info)   << 1) |
			    ((1 & m_MUroads_info)   << 2) |
			    ((1 & m_MUdeco_info)    << 3) |
			    ((1 & m_MUcont_info)    << 4) |
			    ((1 & m_MUfit_info)     << 5) |
			    ((1 & m_MUsag_info)     << 6) |
			    ((1 & m_MUpt_info)      << 7) |
			    ((1 & m_MUtgcDeco_info) << 8) |
			    ((1 & m_FITdebug_info) << 15);
  

  TGCRoadParameters tgc_road_parameters;
  tgc_road_parameters.ETA_LOW_MID_DELTA  = m_TGC_ETA_LOW_MID_DELTA;
  tgc_road_parameters.ETA_HIGH_MID_DELTA = m_TGC_ETA_HIGH_MID_DELTA;
  tgc_road_parameters.PHI_MID_DELTA      = m_TGC_PHI_MID_DELTA;
  tgc_road_parameters.ETA_LOW_INN_DELTA  = m_TGC_ETA_LOW_INN_DELTA;
  tgc_road_parameters.ETA_HIGH_INN_DELTA = m_TGC_ETA_HIGH_INN_DELTA;
  tgc_road_parameters.PHI_INN_DELTA      = m_TGC_PHI_INN_DELTA;

  TGCFitParameters tgc_fit_parameters;
  tgc_fit_parameters.CHI2_TEST           = m_TGC_CHI2_TEST;
  tgc_fit_parameters.MIN_WIRE_POINTS     = m_TGC_MIN_WIRE_POINTS;
  tgc_fit_parameters.MIN_STRIP_POINTS    = m_TGC_MIN_STRIP_POINTS;
  m_msg = new MuMessageInterface(msg(),0,info,m_evtToShow,m_test_string);
  m_svc = new MuServiceInterface(serviceLocator(),
                                 &m_rpc_geometry,
				 &m_tgc_geometry,
                                 &m_csc_geometry,
				 &m_mdt_cabling,
				 &m_backExtrapolator,name(),
                                 m_use_calib,m_CosmicRun,m_BfieldOn,
				 m_use_mcLUT, m_usePol2ForEPt,
				 m_particle_beta, &tgc_road_parameters, 
				 &tgc_fit_parameters);
  if(!m_svc->isLoaded())
  {
      msg() << MSG::FATAL
	    << "unable to set the Muon Service Interface"
	    << endreq;
      return HLT::BAD_JOB_SETUP;
  }

  // ======================= GET THE TIMER SERVICE ===========================
  m_pTimerService = 0;

  m_lvl1_emulation_time = 0;
  m_rpc_track_time      = 0;
  m_muon_roads_time     = 0;
  m_mdt_deco_time       = 0;
  m_contiguity_time     = 0;

  m_fit_time            = 0;
  m_sagitta_time        = 0;
  m_estimate_time       = 0;

  m_pat_rec_time        = 0;
  m_muon_fex_time       = 0;
  m_monitor_time        = 0;
  m_muAlgo_time         = 0;
  m_muFast_time         = 0;
  
  m_RoIDimension_time    = 0;
  m_RegionSelector_time  = 0;
  m_PadId_time           = 0;
  m_PadGet_time          = 0;
  m_PadGetK_time         = 0;
  m_MdtGet_time          = 0;
  
  m_tgc_decoder_time     = 0;

  if(m_timing)
  {
      StatusCode tc;
      tc = serviceLocator()->service( "TrigTimerSvc", m_pTimerService,true); 
      if( tc.isFailure() ) 
      {
         msg() << MSG::WARNING << name() << ": Unable to locate TrigTimerSvc" 
               << endreq;
      }
      else
      {

          m_lvl1_emulation_time = addTimer("MUlvl1");
	  //m_lvl1_emulation_time->propName("muFast.RpcCha");
          m_svc->load_timer(m_lvl1_emulation_time,0);

          m_rpc_track_time = addTimer("MUtrack");
          m_svc->load_timer(m_rpc_track_time,1);

          m_muon_roads_time = addTimer("MUroads");
          m_svc->load_timer(m_muon_roads_time,2);

          m_mdt_deco_time = addTimer("MUdeco");
          //m_mdt_deco_time->propName("muFast.MdtDig");
          m_svc->load_timer(m_mdt_deco_time,3);

          m_contiguity_time = addTimer("MUcont");
          //m_contiguity_time->propName("muFast.RoadDig");
          m_svc->load_timer(m_contiguity_time,4);

          m_fit_time = addTimer("MUfit");
          m_svc->load_timer(m_fit_time,5);

          m_sagitta_time = addTimer("MUsag");
          m_svc->load_timer(m_sagitta_time,6);

          m_estimate_time = addTimer("MUpt");
          m_svc->load_timer(m_estimate_time,7);
	  
	  m_tgc_decoder_time = addTimer("MUtgcdeco");
	  //m_tgc_decoder_time->propName("muFast.TgcCha");
	  m_svc->load_timer(m_tgc_decoder_time,8);

          m_calAccess_time = addTimer("MUcalAccess");
          m_svc->load_timer(m_calAccess_time,9);

          m_idHelper_time = addTimer("MUidHelper");
          m_svc->load_timer(m_idHelper_time,10);

          m_mdtDigi_time = addTimer("MUmdtDigi");
          m_svc->load_timer(m_mdtDigi_time,11);

          m_calRetrieve_time = addTimer("MUcalRetrieve");
          m_svc->load_timer(m_calRetrieve_time,12);
	  
	  m_calSend_time = addTimer("MUcalSend");
          m_svc->load_timer(m_calSend_time,13);


          m_pat_rec_time = addTimer("Upatrec");

          m_muon_fex_time = addTimer("MUfex");

	  m_RoIDimension_time = addTimer("MUroidim");
	  
	  m_RegionSelector_time = addTimer("MUregsel");

	  m_PadId_time = addTimer("MUpadid");
	  
	  m_PadGet_time = addTimer("MUgetPad");
          //m_PadGet_time->propName("muFast.CMAColl");	  
	  
	  m_PadGetK_time = addTimer("MUPadKey");
	  
	  m_MdtGet_time = addTimer("MUgetMdt");
          //m_MdtGet_time->propName("muFast.MdtColl");

          m_muAlgo_time = addTimer("MUalgo");
	  //m_muAlgo_time->propName("muFast.FexPt");

          m_monitor_time = addTimer("MUmoni");

          m_muFast_time = addTimer("MUtotal");
      }
  }
  
  
  // ====================== CREATE THE ALGO MONITOR =========================
  ITHistSvc *rootHistSvc;
  sc = service("THistSvc", rootHistSvc, true);
  if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to locate THistSvc" << endreq;
      return HLT::BAD_JOB_SETUP;
  }
  
  std::string base = m_histo_path_base;
  if (base!="") {   
      std::string path = base + getGaudiThreadGenericName(name()) + "/";
  
      m_monitor = new OnlineSurvey(rootHistSvc,path);
  } else m_monitor = new OnlineSurvey();
  
  
  // ========================= CREATE FEX NTUPLE ============================
  if(m_fexntp)
  {
  
      SmartDataPtr<NTuple::Directory>//gets the item from the store if it exists
          ntdir(ntupleSvc(),"/NTUPLES/FILE1/MUFAST");
      if ( !ntdir )
      { 
          //    otherwise create the directory
          ntdir = ntupleSvc()->createDirectory("/NTUPLES/FILE1/MUFAST");
      }
  
      // Check if the directory is valid
      if ( ! ntdir )  
      {
          msg() << MSG::ERROR << " failed to get ntuple directory" << endreq;
          return HLT::BAD_JOB_SETUP;
      }
    
      
      m_FEXntp = ntupleSvc()->book (ntdir.ptr(), 10, 
		                            CLID_ColumnWiseTuple,"MUFAST");
      
      m_FEXntp->addItem("GenerationPt", m_fex_genPt);
      m_FEXntp->addItem("GenerationEta",m_fex_genEta);
      m_FEXntp->addItem("GenerationPhi",m_fex_genPhi);
      m_FEXntp->addItem("Address",m_fex_address);
      m_FEXntp->addItem("pt",m_fex_pt);
      m_FEXntp->addItem("radius",m_fex_radius);
      m_FEXntp->addItem("eta",m_fex_eta);
      m_FEXntp->addItem("phi",m_fex_phi);
      m_FEXntp->addItem("dir_phi",m_fex_dir_phi);
      m_FEXntp->addItem("zeta",m_fex_zeta);
      m_FEXntp->addItem("dir_zeta",m_fex_dir_zeta);
      m_FEXntp->addItem("beta",m_fex_beta);
     
  } else m_FEXntp = 0;


 // ========================= CREATE LUT NTUPLE ============================
  if(m_lutntp)
  {
        
      SmartDataPtr<NTuple::Directory>//gets the item from the store if it exists
          ntdir(ntupleSvc(),"/NTUPLES/FILE1/MUFAST");
      if ( !ntdir )
      { 
          //    otherwise create the directory
          ntdir = ntupleSvc()->createDirectory("/NTUPLES/FILE1/MUFAST");
      }
  
      // Check if the directory is valid
      if ( ! ntdir )  
      {
          msg() << MSG::ERROR << " failed to get ntuple directory" << endreq;
          return HLT::BAD_JOB_SETUP;
      }
    
      
      m_LUTntp = ntupleSvc()->book (ntdir.ptr(), 11, 
		                            CLID_ColumnWiseTuple,"LUT");
      

      m_LUTntp->addItem("Level1_Id",m_lut_lvl1Id);
      m_LUTntp->addItem("RoI",m_lut_roiType);
      m_LUTntp->addItem("Address",m_lut_address);
      m_LUTntp->addItem("Energy",m_lut_muEnergy);
      m_LUTntp->addItem("Pt",m_lut_muPt);
      m_LUTntp->addItem("Eta",m_lut_muEta);
      m_LUTntp->addItem("Phi",m_lut_muPhi);
      m_LUTntp->addItem("Eta_Map",m_lut_etaMap);
      m_LUTntp->addItem("Phi_Map",m_lut_phiMap);
      m_LUTntp->addItem("Sagitta",m_lut_sagitta);
      m_LUTntp->addItem("Radius",m_lut_radius);
  } else m_LUTntp = 0;
  
  
  // ======================= CREATE Monitor NTUPLE ============================
  if(m_monntp)
  {
      int algo_id = AlgoMap(name());
            
      SmartDataPtr<NTuple::Directory>//gets the item from the store if it exists
          ntdir(ntupleSvc(),"/NTUPLES/FILE1/MUFAST");
      if ( !ntdir )
      { 
          //    otherwise create the directory
          ntdir = ntupleSvc()->createDirectory("/NTUPLES/FILE1/MUFAST");
      }
  
      // Check if the directory is valid
      if ( ! ntdir )  
      {
          msg() << MSG::ERROR << " failed to get ntuple directory" << endreq;
          return HLT::BAD_JOB_SETUP;
      }
    
      m_MONntp = ntupleSvc()->book (ntdir.ptr(), 50+algo_id, 
		                            CLID_ColumnWiseTuple,"MONITOR");
      
      m_MONntp->addItem("AlgoId"    , m_ALGO_id).ignore();
      m_MONntp->addItem("TE_Id"     , m_TE_id).ignore();
      m_MONntp->addItem("ErrorCode" , m_error_code).ignore();
      
      
      m_MONntp->addItem("LVL1Id"      , m_event_number).ignore();
      m_MONntp->addItem("RoINumber"   , m_roi_id_steer).ignore();
      m_MONntp->addItem("RoISystem"   , m_roi_system).ignore();
      m_MONntp->addItem("RoISubSystem", m_roi_subsystem).ignore();
      m_MONntp->addItem("RoISector"   , m_roi_sector).ignore();
      m_MONntp->addItem("RoIId"       , m_roi_number).ignore();
      m_MONntp->addItem("RoIThreshold", m_roi_threshold).ignore();
      m_MONntp->addItem("RoIEta"      , m_roi_eta).ignore();
      m_MONntp->addItem("RoIPhi"      , m_roi_phi).ignore();
      
      m_MONntp->addItem("RobCount"     , m_rob_count, 0, MAX_ROB).ignore();
      m_MONntp->addIndexedItem("RobId" , m_rob_count, m_rob_id).ignore();
      
      m_MONntp->addItem("CsmCount"       , m_csm_count, 0,MAX_CSM).ignore();
      m_MONntp->addIndexedItem("CsmId"   , m_csm_count, m_csm_id).ignore();
      m_MONntp->addIndexedItem("CsmSize" , m_csm_count, m_csm_size).ignore();
      
      m_MONntp->addItem("RemovedRobCount"     , m_removed_rob_count, 0, MAX_ROB).ignore();
      m_MONntp->addIndexedItem("RemovedRobId" , m_removed_rob_count, m_removed_rob_id).ignore();
      
      m_MONntp->addItem("RemovedCsmCount"       , m_removed_csm_count, 0,MAX_CSM).ignore();
      m_MONntp->addIndexedItem("RemovedCsmId"   , m_removed_csm_count, m_removed_csm_id).ignore();
      
            
      m_MONntp->addItem("Rpc1_X"      , m_rpc1_x).ignore();
      m_MONntp->addItem("Rpc1_Y"      , m_rpc1_y).ignore();
      m_MONntp->addItem("Rpc1_Z"      , m_rpc1_z).ignore();
      m_MONntp->addItem("Rpc1_R"      , m_rpc1_r).ignore();
      m_MONntp->addItem("Rpc2_X"      , m_rpc2_x).ignore();
      m_MONntp->addItem("Rpc2_Y"      , m_rpc2_y).ignore();
      m_MONntp->addItem("Rpc2_Z"      , m_rpc2_z).ignore();
      m_MONntp->addItem("Rpc2_R"      , m_rpc2_r).ignore();
      m_MONntp->addItem("Rpc3_X"      , m_rpc3_x).ignore();
      m_MONntp->addItem("Rpc3_Y"      , m_rpc3_y).ignore();
      m_MONntp->addItem("Rpc3_Z"      , m_rpc3_z).ignore();
      m_MONntp->addItem("Rpc3_R"      , m_rpc3_r).ignore();
      
      m_MONntp->addItem("EtaPivot_lay0"  , m_eta_pivot_lay0).ignore();
      m_MONntp->addItem("EtaPivot_lay1"  , m_eta_pivot_lay1).ignore();
      m_MONntp->addItem("EtaLow_0_lay0"  , m_eta_low_0_lay0).ignore();
      m_MONntp->addItem("EtaLow_1_lay0"  , m_eta_low_1_lay0).ignore();
      m_MONntp->addItem("EtaLow_0_lay1"  , m_eta_low_0_lay1).ignore();
      m_MONntp->addItem("EtaLow_1_lay1"  , m_eta_low_1_lay1).ignore();
      m_MONntp->addItem("EtaHigh_0_lay0" , m_eta_high_0_lay0).ignore();
      m_MONntp->addItem("EtaHigh_1_lay0" , m_eta_high_1_lay0).ignore();
      m_MONntp->addItem("EtaHigh_0_lay1" , m_eta_high_0_lay1).ignore();
      m_MONntp->addItem("EtaHigh_1_lay1" , m_eta_high_1_lay1).ignore();
      m_MONntp->addItem("PhiPivot_lay0"  , m_phi_pivot_lay0).ignore();
      m_MONntp->addItem("PhiPivot_lay1"  , m_phi_pivot_lay1).ignore();
      m_MONntp->addItem("PhiLow_0_lay0"  , m_phi_low_0_lay0).ignore();
      m_MONntp->addItem("PhiLow_1_lay0"  , m_phi_low_1_lay0).ignore();
      m_MONntp->addItem("PhiLow_0_lay1"  , m_phi_low_0_lay1).ignore();
      m_MONntp->addItem("PhiLow_1_lay1"  , m_phi_low_1_lay1).ignore();
      m_MONntp->addItem("PhiHigh_0_lay0" , m_phi_high_0_lay0).ignore();
      m_MONntp->addItem("PhiHigh_1_lay0" , m_phi_high_1_lay0).ignore();
      m_MONntp->addItem("PhiHigh_0_lay1" , m_phi_high_0_lay1).ignore();
      m_MONntp->addItem("PhiHigh_1_lay1" , m_phi_high_1_lay1).ignore();

      m_MONntp->addItem("Pad_data_hits"  , m_pad_hit_data, 0, MAX_RPC_HITS).ignore();
    
      m_MONntp->addIndexedItem("Pad_hit_onlineId", m_pad_hit_data, m_pad_hit_onlineId).ignore();
      m_MONntp->addIndexedItem("Pad_hit_code"    , m_pad_hit_data, m_pad_hit_code).ignore();
      m_MONntp->addIndexedItem("Pad_hit_X"       , m_pad_hit_data, m_pad_hit_x).ignore();
      m_MONntp->addIndexedItem("Pad_hit_Y"       , m_pad_hit_data, m_pad_hit_y).ignore();
      m_MONntp->addIndexedItem("Pad_hit_Z"       , m_pad_hit_data, m_pad_hit_z).ignore();
      m_MONntp->addIndexedItem("Pad_hit_R"       , m_pad_hit_data, m_pad_hit_r).ignore();
      m_MONntp->addIndexedItem("Pad_hit_P"       , m_pad_hit_data, m_pad_hit_p).ignore();
    
      m_MONntp->addItem("TgcMid1Eta"   , m_tgc_Mid1_eta).ignore();
      m_MONntp->addItem("TgcMid1Phi"   , m_tgc_Mid1_phi).ignore();
      m_MONntp->addItem("TgcMid1R"     , m_tgc_Mid1_r).ignore();
      m_MONntp->addItem("TgcMid1Z"     , m_tgc_Mid1_z).ignore();
      m_MONntp->addItem("TgcMid2Eta"   , m_tgc_Mid2_eta).ignore();
      m_MONntp->addItem("TgcMid2Phi"   , m_tgc_Mid2_phi).ignore();
      m_MONntp->addItem("TgcMid2R"     , m_tgc_Mid2_r).ignore();
      m_MONntp->addItem("TgcMid2Z"     , m_tgc_Mid2_z).ignore();
      m_MONntp->addItem("MidRhoNpIn"   , m_tgc_Mid_rho_N_in, 0, MAX_TGC_HITS).ignore();
      m_MONntp->addItem("MidRhoChi2"   , m_tgc_Mid_rho_chi2).ignore();
      m_MONntp->addItem("MidRhoNpOut"  , m_tgc_Mid_rho_N).ignore();
      m_MONntp->addItem("MidPhiNpIn"   , m_tgc_Mid_phi_N_in, 0, MAX_TGC_HITS).ignore();
      m_MONntp->addItem("MidPhiChi2"   , m_tgc_Mid_phi_chi2).ignore();
      m_MONntp->addItem("MidPhiNpOut"  , m_tgc_Mid_phi_N).ignore();
      m_MONntp->addItem("TgcInnEta"    , m_tgc_Inn_eta).ignore();
      m_MONntp->addItem("TgcInnPhi"    , m_tgc_Inn_phi).ignore();
      m_MONntp->addItem("TgcInnR"      , m_tgc_Inn_r).ignore();
      m_MONntp->addItem("TgcInnZ"      , m_tgc_Inn_z).ignore();
      m_MONntp->addItem("InnRhoStd"    , m_tgc_Inn_rho_std).ignore();
      m_MONntp->addItem("InnRhoNpIn"   , m_tgc_Inn_rho_N_in, 0, MAX_TGC_HITS).ignore();
      m_MONntp->addItem("InnRhoNpOut"  , m_tgc_Inn_rho_N).ignore();
      m_MONntp->addItem("InnPhiStd"    , m_tgc_Inn_phi_std).ignore();
      m_MONntp->addItem("InnPhiNpIn"   , m_tgc_Inn_phi_N_in, 0, MAX_TGC_HITS).ignore();
      m_MONntp->addItem("InnPhiNpOut"  , m_tgc_Inn_phi_N).ignore();
      m_MONntp->addItem("TgcPt"        , m_tgc_PT).ignore();
      
      m_MONntp->addIndexedItem("Tgc_InnRhoHitPhi"   , m_tgc_Inn_rho_N_in, m_tgc_Inn_rho_hit_phi).ignore();
      m_MONntp->addIndexedItem("Tgc_InnRhoHitR"     , m_tgc_Inn_rho_N_in, m_tgc_Inn_rho_hit_r).ignore();
      m_MONntp->addIndexedItem("Tgc_InnRhoHitZ"     , m_tgc_Inn_rho_N_in, m_tgc_Inn_rho_hit_z).ignore();
      m_MONntp->addIndexedItem("Tgc_InnRhoHitWidth" , m_tgc_Inn_rho_N_in, m_tgc_Inn_rho_hit_width).ignore();
      m_MONntp->addIndexedItem("Tgc_InnRhoHitInSeg" , m_tgc_Inn_rho_N_in, m_tgc_Inn_rho_hit_in_seg).ignore();
      m_MONntp->addIndexedItem("Tgc_InnPhiHitPhi"   , m_tgc_Inn_phi_N_in, m_tgc_Inn_phi_hit_phi).ignore();
      m_MONntp->addIndexedItem("Tgc_InnPhiHitR"     , m_tgc_Inn_phi_N_in, m_tgc_Inn_phi_hit_r).ignore();
      m_MONntp->addIndexedItem("Tgc_InnPhiHitZ"     , m_tgc_Inn_phi_N_in, m_tgc_Inn_phi_hit_z).ignore();
      m_MONntp->addIndexedItem("Tgc_InnPhiHitWidth" , m_tgc_Inn_phi_N_in, m_tgc_Inn_phi_hit_width).ignore();
      m_MONntp->addIndexedItem("Tgc_InnPhiHitInSeg" , m_tgc_Inn_phi_N_in, m_tgc_Inn_phi_hit_in_seg).ignore();
      m_MONntp->addIndexedItem("Tgc_MidRhoHitPhi"   , m_tgc_Mid_rho_N_in, m_tgc_Mid_rho_hit_phi).ignore();
      m_MONntp->addIndexedItem("Tgc_MidRhoHitR"     , m_tgc_Mid_rho_N_in, m_tgc_Mid_rho_hit_r).ignore();
      m_MONntp->addIndexedItem("Tgc_MidRhoHitZ"     , m_tgc_Mid_rho_N_in, m_tgc_Mid_rho_hit_z).ignore();
      m_MONntp->addIndexedItem("Tgc_MidRhoHitWidth" , m_tgc_Mid_rho_N_in, m_tgc_Mid_rho_hit_width).ignore();
      m_MONntp->addIndexedItem("Tgc_MidRhoHitInSeg" , m_tgc_Mid_rho_N_in, m_tgc_Mid_rho_hit_in_seg).ignore();
      m_MONntp->addIndexedItem("Tgc_MidPhiHitPhi"   , m_tgc_Mid_phi_N_in, m_tgc_Mid_phi_hit_phi).ignore();
      m_MONntp->addIndexedItem("Tgc_MidPhiHitR"     , m_tgc_Mid_phi_N_in, m_tgc_Mid_phi_hit_r).ignore();
      m_MONntp->addIndexedItem("Tgc_MidPhiHitZ"     , m_tgc_Mid_phi_N_in, m_tgc_Mid_phi_hit_z).ignore();
      m_MONntp->addIndexedItem("Tgc_MidPhiHitWidth" , m_tgc_Mid_phi_N_in, m_tgc_Mid_phi_hit_width).ignore();
      m_MONntp->addIndexedItem("Tgc_MidPhiHitInSeg" , m_tgc_Mid_phi_N_in, m_tgc_Mid_phi_hit_in_seg).ignore();
      
      m_MONntp->addItem("Mdt_data_hits", m_mdt_hit_tubes, 0, MAX_MDT_HITS).ignore();
      
      m_MONntp->addIndexedItem("Mdt_onlineId"      , m_mdt_hit_tubes, m_mdt_onlineId).ignore();
      m_MONntp->addIndexedItem("Mdt_offlineId"     , m_mdt_hit_tubes, m_mdt_offlineId).ignore();
      m_MONntp->addIndexedItem("Mdt_tube_R"        , m_mdt_hit_tubes, m_mdt_tube_r).ignore();
      m_MONntp->addIndexedItem("Mdt_tube_Z"        , m_mdt_hit_tubes, m_mdt_tube_z).ignore();
      m_MONntp->addIndexedItem("Mdt_tube_residual" , m_mdt_hit_tubes, m_mdt_tube_residual).ignore();
      m_MONntp->addIndexedItem("Mdt_tube_time"     , m_mdt_hit_tubes, m_mdt_tube_time).ignore();
      m_MONntp->addIndexedItem("Mdt_tube_space"    , m_mdt_hit_tubes, m_mdt_tube_space).ignore();
      m_MONntp->addIndexedItem("Mdt_tube_sigma"    , m_mdt_hit_tubes, m_mdt_tube_sigma).ignore();
      
      
      m_MONntp->addItem("MdtInnerSlope"      ,m_mdt_Inner_slope).ignore();
      m_MONntp->addItem("MdtInnerIntercept"  ,m_mdt_Inner_intercept).ignore();
      m_MONntp->addItem("MdtInnerZ"          ,m_mdt_Inner_Z).ignore();
      m_MONntp->addItem("MdtInnerR"          ,m_mdt_Inner_R).ignore();
      m_MONntp->addItem("MdtInnerFitChi2"    ,m_mdt_Inner_fit_chi).ignore();
      m_MONntp->addItem("MdtMiddleSlope"     ,m_mdt_Middle_slope).ignore();
      m_MONntp->addItem("MdtMiddleIntercept" ,m_mdt_Middle_intercept).ignore();
      m_MONntp->addItem("MdtMiddleZ"         ,m_mdt_Middle_Z).ignore();
      m_MONntp->addItem("MdtMiddleR"         ,m_mdt_Middle_R).ignore();
      m_MONntp->addItem("MdtMiddleFitChi2"   ,m_mdt_Middle_fit_chi).ignore();
      m_MONntp->addItem("MdtOuterSlope"      ,m_mdt_Outer_slope).ignore();
      m_MONntp->addItem("MdtOuterIntercept"  ,m_mdt_Outer_intercept).ignore();
      m_MONntp->addItem("MdtOuterZ"          ,m_mdt_Outer_Z).ignore();
      m_MONntp->addItem("MdtOuterR"          ,m_mdt_Outer_R).ignore();
      m_MONntp->addItem("MdtOuterFitChi2"    ,m_mdt_Outer_fit_chi).ignore();
      
      m_MONntp->addItem("Sagitta"      , m_Sagitta).ignore();
      m_MONntp->addItem("Radius"       , m_Radius).ignore();
      m_MONntp->addItem("Slope"        , m_Slope).ignore();
      m_MONntp->addItem("Intercept"    , m_Intercept).ignore();
      m_MONntp->addItem("Alpha"        , m_Alpha).ignore();
      m_MONntp->addItem("Beta"         , m_Beta).ignore();
      m_MONntp->addItem("DeltaR"       , m_DeltaR).ignore();
      m_MONntp->addItem("Speed_over_c" , m_Speed_over_c).ignore();
      m_MONntp->addItem("EtaMap"       , m_EtaMap).ignore();
      m_MONntp->addItem("PhiMap"       , m_PhiMap).ignore();
      m_MONntp->addItem("Phi"          , m_Phi).ignore();
      m_MONntp->addItem("PhiDir"       , m_PhiDir).ignore();
      m_MONntp->addItem("Pt"           , m_Pt).ignore();
      m_MONntp->addItem("Charge"       , m_Charge).ignore();
      
  } else m_MONntp = 0;
 
  
  msg() << MSG::INFO
        << "Parameter file name from jobOptions set to: "
        << m_paramSet
        << endreq;

  // Here one is supposed to open the file if needed and do the setup...

  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endreq;

  return HLT::OK;
}

HLT::ErrorCode muFast::hltFinalize()
{
    delete m_monitor;
    delete m_svc;
    delete m_msg;
    if (m_pROBmapper) delete m_pROBmapper; 
    
    return HLT::OK;
}

// 2012.09.18 KN : necessary to move muon calibration from hltFinalize to hltEndRun 
HLT::ErrorCode muFast::hltEndRun()
{
   // close calibration stream
   // 2012.09.18 KN : moved from hltFinalize to hltEndRun

   if (m_CalibrationStream  && 
       checkForClosing(m_CalBufferName))  m_CalibrationStream->close();
   if (m_CalibrationStream) delete m_CalibrationStream;
    
   if (m_cid>-1) {
      // Old buffer close obsolete
      //std::string name = m_CalBufferName;
      //std::string tag("._CIRC_SHM");
      //if(checkForClosing(name+tag)) CircClose(m_cid);
      if(CircCloseCircConnection (0,(char*)m_stream_name.c_str(),m_cid)!= 0) 
	 return HLT::ERROR;
   }
    
   return HLT::OK;
}

HLT::ErrorCode muFast::hltBeginRun() {
  
  msg() << MSG::INFO << "At BeginRun of " << name() << " - package version " 
	<< PACKAGE_VERSION << endreq;
  
  m_TgcGeometry_NoConf_Erros = 0;
  
  // set storegate variable
  m_pStoreGate = store(); 
  
  // get EventInfo
  const EventInfo* p_EventInfo(0);
  StatusCode sc = m_pStoreGate->retrieve(p_EventInfo);
  if(sc.isFailure()){
    msg() << MSG::FATAL << "Can't get EventInfo object in beginRun()" << endreq;
    return HLT::SG_ERROR;
  }
  
  // setup the detector mask if required by the configuration
  if(m_detector_mask_not_checked) setup_detector_mask(p_EventInfo->event_ID());

  // 
  // open calibration stream
  // 2012.09.18 KN : moved from hltInitialize to hltBeginRun 
  //
  //std::string Name = CalBufferName;
  m_cid = -1;
  m_CalibrationStream = 0;
  m_calib_event = 0;
  m_stream_name = "";
  if(m_CalBufferNameDetermined!="") {      
      std::string::size_type pos = m_CalBufferNameDetermined.find_first_of(':');
      std::string type = m_CalBufferNameDetermined.substr(0,pos);
      std::string location = m_CalBufferNameDetermined.substr(pos+3,m_CalBufferNameDetermined.length());
      m_stream_name = location;// + getGaudiThreadIDfromName(name());
      
      msg() << MSG::INFO << "Calibration stream type: " << type << endreq;
      msg() << MSG::INFO << "Calibration stream name: " << m_stream_name 
            << endreq;
      
      m_CalBufferName = m_stream_name;
            
      if(type=="file") {
          checkForOpening(m_stream_name);
	  m_CalibrationStream = new std::ofstream(m_stream_name.c_str());
	  if ( !m_CalibrationStream->is_open() ) {
	      msg() << MSG::WARNING << "Unable to open the calibration stream "
	            << location << endreq;
	      m_CalibrationStream = 0;
	  }
      } else if (type=="buffer") {
          
	  // The check of the buffer opening is obsolete
	  //std::string tag1("._CIRC_SHM");
	  //std::string tag2("._CIRC_SEM");
          //checkForOpening(stream_name+tag1);
	  //checkForOpening(stream_name+tag2);
          //m_cid = CircOpen (NULL, (char*)stream_name.c_str(), m_CalBufferSize);
          
	  m_cid = CircOpenCircConnection
	                 (0, (char*)m_stream_name.c_str(), m_CalBufferSize);
	  // 2012.04.04 KN added
          // 2012.11.13 EP message type demoted to WARNING
	  if( m_cid == -1 ) {
	     msg() << MSG::WARNING << "Error in opening muon calibration buffer: name="
		   << m_stream_name << " buffer size=" << m_CalBufferSize << endreq;
	  }
      
          //output the stream into a file for testing purposes
	  if( char * env = getenv("TEST_LVL2_MUON_CALIBRATION") ) {
              std::string check_for_test ( env );
	      if ( check_for_test!="" ) {
		  
	          std::string file_name = check_for_test + "/" + m_TestBufferNameDetermined + ".data";
		  
	          checkForOpening(file_name);
	          m_CalibrationStream = new std::ofstream(file_name.c_str());
	          if ( !m_CalibrationStream->is_open() ) {
	              msg() << MSG::WARNING 
		            << "Unable to open the calibration stream test file"
	                    << location << endreq;
	              m_CalibrationStream = 0;
	          }
	      }
          }
      }
      
  }


  return HLT::OK;
}

void muFast::setup_detector_mask(const EventID* eventId) {  
  
  // Retrieve run number and detector mask
  
  m_current_run_id = eventId->run_number();
  
  uint64_t dmask = eventId->detector_mask();
    
  if(!m_detector_mask_on_event && msgLvl() <= MSG::INFO) {
      msg() << MSG::INFO << "Retrieved the detector mask:" << endreq;
      msg() << MSG::INFO << "  mask = 0x" << MSG::hex << dmask 
                         << MSG::dec << endreq;
  }
  
  if(m_detector_mask_on_event && msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Retrieved the detector mask:" << endreq;
      msg() << MSG::DEBUG << "  mask = 0x" << MSG::hex << dmask 
                          << MSG::dec << endreq;
  }
  
  if(dmask==0) {
      if(!m_detector_mask_on_event && msgLvl() <= MSG::INFO) {
          msg() << MSG::INFO << "detector mask == 0: do nothing!" << endreq;
          
      }
      if(m_detector_mask_on_event && msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "detector mask == 0: do nothing!" << endreq;
      }
      return;
  }
  
  eformat::helper::DetectorMask dm(dmask);
    
  m_hasMdtBarrelA = dm.is_set(eformat::MUON_MDT_BARREL_A_SIDE);
  m_hasMdtBarrelC = dm.is_set(eformat::MUON_MDT_BARREL_C_SIDE);
  m_hasMdtEndcapA = dm.is_set(eformat::MUON_MDT_ENDCAP_A_SIDE);
  m_hasMdtEndcapC = dm.is_set(eformat::MUON_MDT_ENDCAP_C_SIDE);
  m_hasRpcBarrelA = dm.is_set(eformat::MUON_RPC_BARREL_A_SIDE);
  m_hasRpcBarrelC = dm.is_set(eformat::MUON_RPC_BARREL_C_SIDE);
  m_hasTgcEndcapA = dm.is_set(eformat::MUON_TGC_ENDCAP_A_SIDE);
  m_hasTgcEndcapC = dm.is_set(eformat::MUON_TGC_ENDCAP_C_SIDE);
  m_hasCscEndcapA = dm.is_set(eformat::MUON_CSC_ENDCAP_A_SIDE);
  m_hasCscEndcapC = dm.is_set(eformat::MUON_CSC_ENDCAP_C_SIDE);
  
  if(!m_detector_mask_on_event && msgLvl() <= MSG::INFO) {
      msg() << MSG::INFO << " ==== Detector Mask settings ==== " << endreq;           
      msg() << MSG::INFO << " ---> MDT Barrel side A: "                 
                         << ((m_hasMdtBarrelA)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> MDT Barrel side C: "                 
                         << ((m_hasMdtBarrelC)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> MDT Endcap side A: "                 
                         << ((m_hasMdtEndcapA)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> MDT Endcap side C: "                 
                         << ((m_hasMdtEndcapC)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> RPC Barrel side A: "                 
                         << ((m_hasRpcBarrelA)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> RPC Barrel side C: "                 
                         << ((m_hasRpcBarrelC)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> TGC Endcap side A: "                 
                         << ((m_hasTgcEndcapA)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> TGC Endcap side C: "                 
                         << ((m_hasTgcEndcapC)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> CSC Endcap side A: "                 
                         << ((m_hasCscEndcapA)? "ON" : "OFF") << endreq;
      msg() << MSG::INFO << " ---> CSC Endcap side C: "                 
                         << ((m_hasCscEndcapC)? "ON" : "OFF") << endreq;
  }
  
  if(m_detector_mask_on_event && msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " ==== Detector Mask settings ==== " << endreq;           
      msg() << MSG::DEBUG << " ---> MDT Barrel side A: "                 
                          << ((m_hasMdtBarrelA)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> MDT Barrel side C: "                 
                          << ((m_hasMdtBarrelC)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> MDT Endcap side A: "                 
                          << ((m_hasMdtEndcapA)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> MDT Endcap side C: "                 
                          << ((m_hasMdtEndcapC)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> RPC Barrel side A: "                 
                          << ((m_hasRpcBarrelA)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> RPC Barrel side C: "                 
                          << ((m_hasRpcBarrelC)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> TGC Endcap side A: "                 
                          << ((m_hasTgcEndcapA)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> TGC Endcap side C: "                 
                          << ((m_hasTgcEndcapC)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> CSC Endcap side A: "                 
                          << ((m_hasCscEndcapA)? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> CSC Endcap side C: "                 
                          << ((m_hasCscEndcapC)? "ON" : "OFF") << endreq;
  }
  
  if (!m_detector_mask_on_event) m_detector_mask_not_checked = false;
}


bool muFast::isRobReadout (uint32_t robId) {
    uint16_t SubDetectorId = (robId >> 16)&0xff;
    
    switch (SubDetectorId) {
        case 0x61:
	if(!m_hasMdtBarrelA) m_data_access_error |= (1<<31);
	return (m_hasMdtBarrelA);
	break;
	
	case 0x62:
	if(!m_hasMdtBarrelC) m_data_access_error |= (1<<29);
	return (m_hasMdtBarrelC);
	break;
	
	case 0x63:
	if(!m_hasMdtEndcapA) m_data_access_error |= (1<<27);
	return (m_hasMdtEndcapA);
	break;
	
	case 0x64:
	if(!m_hasMdtEndcapC) m_data_access_error |= (1<<25);
	return (m_hasMdtEndcapC);
	break;
	
	case 0x65:
	if(!m_hasRpcBarrelA) m_data_access_error |= (1<<23);
	return (m_hasRpcBarrelA);
	break;
	
	case 0x66:
	if(!m_hasRpcBarrelC) m_data_access_error |= (1<<21);
	return (m_hasRpcBarrelC);
	break;
	
	case 0x67:
	if(!m_hasTgcEndcapA) m_data_access_error |= (1<<19);
	return (m_hasTgcEndcapA);
	break;
	
	case 0x68:
	if(!m_hasTgcEndcapC) m_data_access_error |= (1<<17);
	return (m_hasTgcEndcapC);
	break;
	
	case 0x69:
	if(!m_hasCscEndcapA) m_data_access_error |= (1<<15);
	return (m_hasCscEndcapA);
	break;
	
	case 0x6a:
	if(!m_hasCscEndcapC) m_data_access_error |= (1<<13);
	return (m_hasCscEndcapC);
	break;
	
	
	default:
	msg() << MSG::WARNING << "ROB ID " << std::hex
	      << robId << std::dec << " is not a Muon Spectrometer ROB"
	      << endreq;
	return true;
    }
    return true;
}

bool muFast::isRobEnabled (uint32_t robId) {
    for(unsigned int i=0;i<m_rob_disabled.size();i++) {
        if(m_rob_disabled[i]==robId) {
	    
	    uint16_t SubDetectorId = (robId >> 16) & 0xff;
            switch (SubDetectorId) {
        
	        case 0x61:
	        m_data_access_error |= (1<<30);
	        break;
	
	        case 0x62:
	        m_data_access_error |= (1<<28);
	        break;
	
	        case 0x63:
	        m_data_access_error |= (1<<26);
	        break;
	
	        case 0x64:
  	        m_data_access_error |= (1<<24);
	        break;
	
	        case 0x65:
	        m_data_access_error |= (1<<22);
	        break;
	
	        case 0x66:
	        m_data_access_error |= (1<<20);
	        break;
	
	        case 0x67:
	        m_data_access_error |= (1<<18);
	        break;
	
	        case 0x68:
	        m_data_access_error |= (1<<16);
	        break;
	
	        case 0x69:
	        m_data_access_error |= (1<<14);
 	        break;
	
	        case 0x6a:
	        m_data_access_error |= (1<<12);
	        break;
	
	
	        default:
	        msg() << MSG::WARNING << "ROB ID " << std::hex
	           << robId << std::dec << " is not a Muon Spectrometer ROB"
	           << endreq;
	    }
	    return false;
        }
    }
    
    return true;
}

void
muFast::screen_robs(std::vector<uint32_t> in, std::vector<uint32_t>& out) {
    for(unsigned int i=0;i<in.size();++i) {
        if(this->isRobReadout(in[i]) && this->isRobEnabled(in[i]) )
	    out.push_back(in[i]);
	else m_rob_removed.insert(in[i]);
    }
}


////////////////////////////////////////////////////////////////////////////

/** @fn bool muFast::execHLTAlgorithm(TriggerElement* outputTE)
 * @brief Execute(TE) method.
 * @return bool
 * This is an example to illustrate several aspects of the HLTSSW framework:

 * - Seeding mechanism: how to navigate the following structure at input:
 *
 *         +-------------+   uses     +------------+
 *         |  InputTe    | ---------> |  EmTauRoI  |
 *         +-------------+            +------------+
 *               ^
 *               | seeded by
 *         +-------------+
 *         |  OutputTE   |
 *         +-------------+
 *
 * - Region lookup
 * - RawDataObject access (from BS)
 * - Linking a new object into the TEOutput for subsequent use
 * - OutputTE activation to signal a positive decision
 * All is done using the LAr as an example detector.
 */
HLT::ErrorCode muFast::hltExecute(const HLT::TriggerElement* inputTE,
                                        HLT::TriggerElement* outputTE)
{
  if(m_muFast_time) m_muFast_time->start();

  m_pStoreGate = store();                        // retrieve the store

  //MsgStream Mlog(messageService(), name());      // retrieve Message Service

  StatusCode sc = StatusCode::FAILURE;

  m_msg->next();            // increase the number of output events

  // Retrieve the EventInfo Objects //////////////////////////////////////////
  const EventInfo* pEvent(0);                                               //
  sc = m_pStoreGate->retrieve(pEvent);                                      //
  if ( sc.isFailure() ) {                                                   //
      msg() << MSG::ERROR << "Could not find EventInfo object" << endreq;   //
      return HLT::SG_ERROR;                                                 //
  }                                                                         //
                                                                            //
  const EventID* pEventId = pEvent->event_ID();                             //
  if(pEventId==0) {                                                         //
      msg() << MSG::ERROR << "Could not find EventID object" << endreq;     //
      return HLT::SG_ERROR;                                                 //
  }                                                                         //
  m_current_run_id = pEventId->run_number();                                //
  m_current_lbk_id = pEventId->lumi_block();                                //
  m_current_bcg_id = pEventId->bunch_crossing_id();                         //
                                                                            //
  const TriggerInfo* pTriggerInfo = pEvent->trigger_info();                 //
  if(pTriggerInfo==0) {                                                     //
      msg() << MSG::ERROR << "Could not find TriggerInfo object" << endreq; //
      return HLT::SG_ERROR;                                                 //
  }                                                                         //
                                                                            //
  if(m_detector_mask_not_checked &&                                         //
        m_detector_mask_on_event) setup_detector_mask(pEventId);            //
                                                                            //
  m_current_lvl1_id = pTriggerInfo->extendedLevel1ID();                     //
                                                                            //
  if(msgLvl() <= MSG::DEBUG) {                                              //
      msg() << MSG::DEBUG << " ---> Run Number       : "                    //
                          << m_current_run_id << endreq;                    //
      msg() << MSG::DEBUG << " ---> Luminosity Block : " << std::hex        //
                          << m_current_lbk_id << std::dec << endreq;        //
      msg() << MSG::DEBUG << " ---> Bunch Crossing ID: " << std::hex        //
                          << m_current_bcg_id << std::dec << endreq;        //
      msg() << MSG::DEBUG << " ---> Extended Lvl1 ID : " << std::hex        //
                          << m_current_lvl1_id << std::dec << endreq;       //  
  }                                                                         //
  ////////////////////////////////////////////////////////////////////////////
  

#ifdef INSTRUMENTATION  
  INSTRUMENT("PESA_muFast_BEGIN_PROCESSING", m_current_lvl1_id);
#endif


  if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId()<<endreq;


  m_data_access_error = 0;
  DigiPointer dummy_digit;
  dummy_digit.digi = 0;
  
  
  // *************************************************************************
  // ****************** START TRIGGER ELEMENT PROCESSING *********************
  // *************************************************************************
  
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(inputTE, roiDescriptor, "");
  
  if (status != HLT::OK) {
      msg() << MSG::WARNING  << "Could not find the LVL1 roi descriptor." 
            << endreq;
  }

  ////////////////////////////////////////////////////////////////////////////
  if (roiDescriptor) {                                                      //
      m_currentRoIId = roiDescriptor->roiId();                              //
  } else if ( !roiDescriptor && m_create_roi) {                             //
      msg() << MSG::ERROR                                                   //
	        << "Could not find roiDescriptor, MuonRoI cannot be created"//
		<< endreq;                                                  //
          return HLT::NAV_ERROR;                                            //
  } else {                                                                  //
      msg() << MSG::WARNING  << "Could not set the RoI Id" << endreq;       //
      m_currentRoIId = -1;                                                  //
  }                                                                         //
  ////////////////////////////////////////////////////////////////////////////
  
  std::vector<TrigConf::TriggerThreshold*> muonConfig;
  LVL1::RecMuonRoI local = LVL1::RecMuonRoI(0x0,
                           &(*m_recRPCRoiSvc),&(*m_recTGCRoiSvc),&muonConfig);
  
  if (!m_create_roi) {

      // Get vector of of pointers to all RecMuonRoIs with label "uses" //////
      std::vector<const LVL1::RecMuonRoI*> vectorOfRecMuonRoI;              //
      status = getFeatures(inputTE, vectorOfRecMuonRoI, "");                //
      if(status!=HLT::OK) {                                                 //
          msg() << MSG::ERROR                                               //
                << "Retrieval of RoI vector failed!"                        //
                << endreq;                                                  //
          msg() << MSG::ERROR                                               //
                << "Status=" << status << ",  vector size="                 //
	        << vectorOfRecMuonRoI.size() << endreq;                     //
          return HLT::NAV_ERROR;                                            //
      }                                                                     //
                                                                            //
      /** Check that there is only one RecMuonRoI.                          //
       *  We only expect ONE input RoI.                                     //
       */                                                                   //
      if (vectorOfRecMuonRoI.size() != 1){                                  //
          msg() << MSG::ERROR                                               //
                << "Size of RoI vector is "                                 //
                << vectorOfRecMuonRoI.size() << endreq;                     //
          fillNtp(MUFAST::BAD_ROI_DELIVERED, 0, 0, 0, dummy_digit, 0, 0);   //
          return HLT::NAV_ERROR;                                            //
      }                                                                     //
      ////////////////////////////////////////////////////////////////////////

      // Get first (and only) RoI ////////////////////////////////////////////
      m_pMuonRoI = vectorOfRecMuonRoI.front();                              //
      if(!m_pMuonRoI){                                                      //
          msg() << MSG::ERROR                                               //
                << "Retrieval of RoI from vector failed" << endreq;         //
          fillNtp(MUFAST::EMPTY_ROIVEC_DELIVERED,0,0,0,dummy_digit,0,0);    //
          return HLT::NAV_ERROR;                                            //
      } else {                                                              //
          msg() << MSG::DEBUG                                               //
	        << "Muon RoI with word 0x"                                  //
		<< MSG::hex << m_pMuonRoI->roiWord() << MSG::dec            //
		<< " retrieved from StoreGate" << endreq;                   //
      }                                                                     //
      ////////////////////////////////////////////////////////////////////////

  } else {
  
      // Get LVL1 menu configuration  ////////////////////////////////////////
      std::vector<TrigConf::TriggerThreshold*> muonConfig;                  //
      if (m_gotL1Config) {                                                  //
          std::vector<TrigConf::TriggerThreshold*> thresholds =             //
	           m_lvl1ConfigSvc->ctpConfig()->menu().thresholdVector(); //
          for (std::vector<TrigConf::TriggerThreshold*>::const_iterator     //
	      it = thresholds.begin(); it != thresholds.end(); ++it) {      //
              if ( (*it)->type() == TrigConf::L1DataDef::muonType() )       //
	          muonConfig.push_back(*it);                                //
          }                                                                 //
      }                                                                     //
      ////////////////////////////////////////////////////////////////////////
      
      // Create RoI  /////////////////////////////////////////////////////////
      uint32_t roiWord = roiDescriptor->roiWord();                          //
      local = LVL1::RecMuonRoI(roiWord,                                     //
                       &(*m_recRPCRoiSvc),&(*m_recTGCRoiSvc),&muonConfig);  //
      m_pMuonRoI = &local;                                                  //
      msg() << MSG::DEBUG                                                   //
            << "Muon RoI with word 0x" << MSG::hex << roiWord << MSG::dec   //
	    << " created from RoI descriptor" << endreq;                    //
      ////////////////////////////////////////////////////////////////////////

  }       
       
  
  
  if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG 
            << "The position of the RoI is phi = " << m_pMuonRoI->phi() 
	    << " / eta = " << m_pMuonRoI->eta() << endreq;                                                       //
      msg() << MSG::DEBUG 
           << "RoI code = " << m_pMuonRoI->getRoINumber()
           << " ,  RoI Threshold = " << m_pMuonRoI->getThresholdNumber()
           << endreq;
      msg() << MSG::DEBUG
           << "RoI sectorID = " << m_pMuonRoI->sectorID() << endreq;
      msg() << MSG::DEBUG << "SystemID = " << m_pMuonRoI->sysID()
            << ",  SubsystemId = " << m_pMuonRoI->subsysID() << endreq;
  }

  
  
  m_barrel_sector_hit = -1;
  m_barrel_pad_hit    = -99;
  m_barrel_sector_rec = -1;
  m_barrel_pad_rec    = -99;
  
  m_barrel_sector_track_eff = -1;
  m_barrel_pad_track_eff    = -99;
  
  m_inner_mdt_hits  = -1;
  m_middle_mdt_hits = -1;
  m_outer_mdt_hits  = -1;
  
  m_fit_residuals.clear();
  m_res_inner.clear();
  m_res_middle.clear();
  m_res_outer.clear();
  
  m_efficiency  = 0;
  m_sag_inverse = 9999.;
  m_sagitta     = 9999.;
  m_address     = 9999;
  m_absolute_pt = 9999.;
  m_calib_event_size  = -1;
  m_buffer_free_count = -1;

  m_lvl1_id.clear();
  m_lvl1_eta.clear();
  m_lvl1_phi.clear();
  m_lvl1_pt.clear();
  m_lvl1_id.push_back(0);
  m_lvl1_eta.push_back(m_pMuonRoI->eta());
  float lvl1_roi_phi =  (m_pMuonRoI->phi()>0.)? m_pMuonRoI->phi():m_pMuonRoI->phi()+2*PIGR;
  m_lvl1_phi.push_back(lvl1_roi_phi); 
  m_lvl1_pt.push_back(m_pMuonRoI->getThresholdValue());
   
  Identifier rpc_pad_id;       // identifier for accessing the RPC PAD
  // To avoid problems with 64-bit identifiers, we make the tgc_rdo_id
  // an unsigned int directly since the IdHelpers are not used to
  // create it.
  unsigned int tgc_rdo_id;    // identifier for accessing the TGC data
  
  unsigned short int systemID    = m_pMuonRoI->sysID();
  unsigned short int subsystemID = m_pMuonRoI->subsysID();
  unsigned short int sectorID    = m_pMuonRoI->sectorID();
  unsigned short int RoInumber   = m_pMuonRoI->getRoINumber();
  unsigned short int threshold   = m_pMuonRoI->getThresholdNumber();

  float EtaRoI =  m_pMuonRoI->eta();
  float PhiRoI = (m_pMuonRoI->phi()>0.)? m_pMuonRoI->phi():m_pMuonRoI->phi()+2*PIGR;
  
  //= AI added
  // CID 47062: UNINIT
  // short int TGCRoIeta;
  // short int TGCRoIphi;
  short int TGCRoIeta = 0;
  short int TGCRoIphi = 0;

  // systemID 1:endcap 2:forward
  // subsystemID 1:eta>0 0:eta<0
  if(systemID!=0)TGCRoI2etaphi(systemID, subsystemID, sectorID, RoInumber,TGCRoIeta, TGCRoIphi);

  m_endcap_eta_hit.clear();
  m_endcap_phi_hit.clear();
  m_endcap_eta_rec.clear();
  m_endcap_phi_rec.clear();
  m_endcap_eta_eff.clear();
  m_endcap_phi_eff.clear();

  m_track_eta.clear();
  m_track_phi.clear();

  //=end


  // create the ESD
  m_details = 0;
  if(m_output_details) {
     // CID 24145: CONSTANT_EXPRESSION_RESULT
     /*
    uint32_t muondetmask =  m_hasMdtBarrelA       &
                           (m_hasMdtBarrelC << 1) &
                           (m_hasMdtEndcapA << 2) &
                           (m_hasMdtEndcapC << 3) &
                           (m_hasRpcBarrelA << 4) &
                           (m_hasRpcBarrelC << 5) &
                           (m_hasTgcEndcapA << 6) &
                           (m_hasTgcEndcapC << 7) &
                           (m_hasCscEndcapA << 8) &
                           (m_hasCscEndcapC << 9);
     */
    uint32_t muondetmask = 0;
    if( m_hasMdtBarrelA ) muondetmask += 0x1;
    if( m_hasMdtBarrelC ) muondetmask += 0x2;
    if( m_hasMdtEndcapA ) muondetmask += 0x4;
    if( m_hasMdtEndcapC ) muondetmask += 0x8;
    if( m_hasRpcBarrelA ) muondetmask += 0x10;
    if( m_hasRpcBarrelC ) muondetmask += 0x20;
    if( m_hasTgcEndcapA ) muondetmask += 0x40;
    if( m_hasTgcEndcapC ) muondetmask += 0x80;
    if( m_hasCscEndcapA ) muondetmask += 0x100;
    if( m_hasCscEndcapC ) muondetmask += 0x200;

    m_details = new MuonFeatureDetails( AlgoMap(name()),
                                        inputTE->getId(),
					m_current_lvl1_id,
					m_current_lbk_id,
					muondetmask,
					m_currentRoIId,
					systemID,
					subsystemID,
					sectorID,
					RoInumber,
                                        threshold,
					m_pMuonRoI->eta(),
					m_pMuonRoI->phi(),
					m_esd_ext_size,
                                        m_esd_rob_size,
                                        m_esd_csm_size,
                                        m_esd_lv1_size,
                                        m_esd_rpc_size,
                                        m_esd_tgc_size,
                                        m_esd_mdt_size );
  }
						

  //RoI remapping for cosmic run
  if ( m_CosmicRun ) {

      m_pROImapper->remap(systemID,subsystemID,sectorID,EtaRoI,PhiRoI,threshold);
      // patch for M5
      if ( RoInumber >= 148 )  RoInumber -= 8;

      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "SystemID remapped to "
                                       << systemID << ", SectorID remapped to "
                                       << sectorID
				       << " and SubsystemID remapped to "
				       << subsystemID << endreq;   
  }
  
  int FeatureAdd = (systemID==0)? 0 : -1;
  
  std::vector<IdentifierHash> listOfMDTIds;

  float EtaMin = 0.;    // EtaMin of the barrel RoI
  float EtaMax = 0.;    // EtaMax of the barrel RoI
  float PhiMin = 0.;    // PhiMin of the barrel RoI
  float PhiMax = 0.;    // Phimax of the barrel RoI



  // *************************************************************************
  // ********************* START ROI DATA PROCESSING *************************
  // *************************************************************************  
  
  unsigned short int PADId  = 0;
  
  if ( systemID != 0 )         // PROCESSING THE ENDCAP RoI 
  {
      // Processing a TGC RoI to get the TGC RDO identifier //////////////////
      EtaMin = EtaRoI - 0.1;                                                //
      EtaMax = EtaRoI + 0.1;                                                //
                                                                            //
      double phi = PhiRoI;                                                  //
      PhiMin = phi - 0.1;                                                   //
      PhiMax = phi + 0.1;                                                   //
      if ( PhiMin<0. )     PhiMin += 2*M_PI;                                //
      if ( PhiMax>2*M_PI ) PhiMax -= 2*M_PI;                                //
                                                                            //
      if(!m_svc->tgcLutSvc()->isConfigured()) {                             //
          if (m_TgcGeometry_NoConf_Erros <= MAX_ERRORS) {                   //
	      ++m_TgcGeometry_NoConf_Erros;                                 //
	      msg() << MSG::ERROR << "Error Code "                          //
	            << MUFAST::TGC_GEOMETRY_MISCONFIGURED << ": attemp to " //
	            << "use TGCgeometrySvc while it is not configured"      //
	            << endreq;                                              //
	      if (m_TgcGeometry_NoConf_Erros == MAX_ERRORS) {               //
	          msg() << MSG::ERROR                                       //
		        << "Suppressing any further messages of error code "//
	                << MUFAST::TGC_GEOMETRY_MISCONFIGURED << endreq;    //
	      }                                                             //
	  }                                                                 //
	                                                                    //
	  MuData data;                                                      //
	  std::vector<uint32_t> robIds;                                     //
	  MuonFeature* muon_feature =                                       //
                      new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0); //
          return muFastSeed(MUFAST::TGC_GEOMETRY_MISCONFIGURED, inputTE,    //
	         outputTE, &robIds, &data, 0, dummy_digit,0, muon_feature); //
      }                                                                     //
                                                                            //
      tgc_rdo_id = m_svc->tgcLutSvc()->rdoID(EtaRoI, phi);                  //
                                                                            //
      if(msgLvl() <= MSG::DEBUG) {                                          //
          msg() << MSG::DEBUG  << "tgc_rdo_id="                             //
	       << tgc_rdo_id << endreq;                       //
          msg() << MSG::DEBUG << "TGC RoI processing works!" << endreq;     //
      }                                                                     //
      //=AI added                                                           //
      m_endcap_eta_hit.push_back(TGCRoIeta);                                //
      m_endcap_phi_hit.push_back(TGCRoIphi);                                //
      //=end                                                                //
      ////////////////////////////////////////////////////////////////////////

  } else               // PROCESSING THE BARREL RoI 
  {
  
      if(m_RoIDimension_time) m_RoIDimension_time->start();
      // Processing the RPC RoI to get its Eta Phi dimension /////////////////
          if(!RpcRoIDim(subsystemID,sectorID,threshold,RoInumber,           //
                        EtaMin,EtaMax))                                     //
          {                                                                 //
              msg() << MSG::DEBUG                                           //
	           << "Cannot retrieve the RPC RoI dimension in eta:"       //
	           << " using the nominal ones" << endreq;                  //
	                                                                    //
	      EtaMin = EtaRoI - 0.2;                                        //
	      EtaMax = EtaRoI + 0.2;                                        // 
          }                                                                 //
                                                                            //
          PhiMin = PhiRoI - 0.02;                                           //
          PhiMax = PhiRoI + 0.02;                                           //
	  PhiMin = PhiRoI - 0.1;                                            //
          PhiMax = PhiRoI + 0.1;                                            //
          if(PhiMin < 0.) PhiMin += 2*M_PI;                                 //
          if(PhiMax < 0.) PhiMax += 2*M_PI;                                 //
	                                                                    //
      ////////////////////////////////////////////////////////////////////////


      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG
             << "RoI EtaMin,EtaMax = (" << EtaMin << "," << EtaMax << ")  "
             << "RoI PhiMin,PhiMax = (" << PhiMin << "," << PhiMax << ")"
             << endreq;
      }
      if(m_RoIDimension_time) m_RoIDimension_time->stop();


      if(m_PadId_time) m_PadId_time->start();
      // Retrieve the PAD identifiers using the cabling service //////////////
      unsigned int logic_sector = 0;                                        //
                                                                            //
      bool success = m_svc->rpccab()->give_PAD_address(subsystemID,sectorID,//
		                  RoInumber,logic_sector,PADId,rpc_pad_id); //
      if(!success)                                                          //
      {                                                                     //
          msg() << MSG::DEBUG                                               //
           << "Cannot retrieve the PAD identifier from the Cabling Service" //
           << endreq;                                                       //
	  msg() << MSG::DEBUG << "Send dummy MuonFeature " << endreq;       //
	                                                                    //
          MuData data;                                                      //
	  std::vector<uint32_t> robIds;                                     //
	  MuonFeature* muon_feature =                                       //
                    new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0);   //
          return muFastSeed(MUFAST::BAD_RPC_ROI_INDEXING, inputTE,          //
	         outputTE, &robIds, &data, 0, dummy_digit,0, muon_feature); //
		                                                            //
          //msg() << MSG::ERROR                                               //
          // << "Cannot retrieve the PAD identifier from the Cabling Service" //
          // << endreq;                                                       //
	  //fillNtp(MUFAST::BAD_RPC_ROI_INDEXING, 0, 0, 0, dummy_digit, 0, 0);//
          //return HLT::TOOL_FAILURE;                                         //
      }                                                                     //
                                                                            //
      m_barrel_sector_hit = sectorID;                                       //
      m_barrel_pad_hit    = (subsystemID==1)? PADId+1 : -(PADId+1);         //
      ////////////////////////////////////////////////////////////////////////
      if(m_PadId_time) m_PadId_time->stop();
  }

  
  // include the additional MDTs to be collected
  std::vector<int>::const_iterator hs = m_MdtId.begin();
  while (hs != m_MdtId.end())
  {
      listOfMDTIds.push_back(*hs);
      ++hs;
  }



  // crate the data holder ///////////////////////////////////////////////////
  MuData data;                                                              //
  data.get_roi(m_pMuonRoI);                                                 //
  ////////////////////////////////////////////////////////////////////////////
  
  

  // *************************************************************************
  // ************************** PRELOAD ROB DATA *****************************
  // *************************************************************************

  unsigned int trigger_subsys_id = 0;  
  unsigned int trigger_rob_id = 0; 
  if( systemID == 0 )  // compute the RPC ROB Id
  {
      // compute the ROB ID for the RPC data and preload the ROBDataProvider//
      trigger_subsys_id = (subsystemID==1)? 0x65 : 0x66;                    //  
      trigger_rob_id = ( trigger_subsys_id    << 16) | (sectorID/2);        //
      if(msgLvl() <= MSG::DEBUG)                                            //
          msg() << MSG::DEBUG                                               //
                << " Rpc ROBId = "                                          //
	        << std::hex << trigger_rob_id << std::dec << endreq;        //
      ////////////////////////////////////////////////////////////////////////  
  }
  
  if( systemID != 0 )  // compute the TGC ROB Id
  {   
      // compute the ROB ID for the RPC data and preload the ROBDataProvider//
      double phi = PhiRoI;                                                  //
      if (phi < 0) phi += 2.0 * M_PI;                                       //
                                                                            //
      trigger_rob_id = m_svc->tgcLutSvc()->robNumber(EtaRoI, phi);          //
                                                                            //
      if(msgLvl() <= MSG::DEBUG)                                            //
          msg() << MSG::DEBUG                                               //
                << " Tgc ROBId = "                                          //
	        << std::hex << trigger_rob_id << std::dec << endreq;        //
      ////////////////////////////////////////////////////////////////////////  
  }
  
 
  std::vector<uint32_t> robIds;
  std::vector<uint32_t> mdt_rob_ids;
  robIds.clear();
  if( systemID == 0 )
  {

      // Preload the data into the ROBdataProvider /////////////////////////// 
      robIds = m_svc->mdtgeo()->getRobId(EtaMin,EtaMax,PhiMin,PhiMax);      //
      if(msgLvl() <= MSG::DEBUG) {                                          //
          msg() << MSG::DEBUG                                               //
                << " Muon MDT Cabling: Number of ROBIds = "                 //
                << robIds.size() << endreq;                                 //
          msg() << MSG::DEBUG                                               //
                << " Muon MDT Cabling: List of ROBIds   = "  << endreq;     //
          for (std::vector<uint32_t>::const_iterator i=robIds.begin();      //
               i != robIds.end(); ++i)                                      //
          {                                                                 //
              msg() << MSG::DEBUG << std::hex << *i << std::dec << endreq;  //
          }                                                                 //
      }                                                                     //
                                                                            //
      mdt_rob_ids = robIds;                                                 //
      robIds.insert(robIds.begin(),trigger_rob_id);                         //
                                                                            //
      ////////////////////////////////////////////////////////////////////////
  }
  else if ( systemID != 0 ) 
  {
      // Preload endcap data /////////////////////////////////////////////////
      if( m_CosmicRun )                                                     //
      {                                                                     //
          EtaMin = (EtaMin<0.)? -2.5  : 1.2;                                //
	  EtaMax = (EtaMax<0.)? -1.2 : 2.5;                                 //
      }                                                                     //
      robIds = m_svc->mdtgeo()->getRobId(EtaMin,EtaMax,PhiMin,PhiMax);      //
                                                                            //
      if(msgLvl() <= MSG::DEBUG) {                                          //
          msg() << MSG::DEBUG                                               //
                << " Muon MDT Cabling: Number of ROBIds = "                 //
                << robIds.size() << endreq;                                 //
          msg() << MSG::DEBUG                                               //
                << " Muon MDT Cabling: List of ROBIds   = "  << endreq;     //
          for (std::vector<uint32_t>::const_iterator i=robIds.begin();      //
               i != robIds.end(); ++i)                                      //
          {                                                                 //
              msg() << MSG::DEBUG << std::hex << *i << std::dec << endreq;  //
          }                                                                 //
      }                                                                     //
                                                                            //
      mdt_rob_ids = robIds;                                                 //                                                                      //
      robIds.insert(robIds.begin(),trigger_rob_id);                         //
                                                                            //      
      ////////////////////////////////////////////////////////////////////////   
  }

  // add the additional ROBs to the list
  std::vector<uint32_t>::const_iterator rob = m_robIds.begin();
  while (rob != m_robIds.end())
  {
       robIds.push_back(*rob);
       mdt_rob_ids.push_back(*rob);
       ++rob;
  }
  
  
  
  //get the infos for accessing the CSC detector
  CSCgeometrySvc::CSCIDInfo CSC_data_access_infos;
  std::vector<uint32_t> csc_rob_ids;
  std::vector<IdentifierHash> listOfCSCIds;
  if( fabs(EtaMax)>=2.0 && m_csc_data_conversion ) {
      m_svc->cscLutSvc()->CSCIdList(EtaRoI,PhiRoI,&CSC_data_access_infos);
      
      if(CSC_data_access_infos.size()!=0)
      {
          if(msgLvl() <= MSG::DEBUG)
	  {
              msg() << MSG::DEBUG << "Needed to retrieve " 
	            << CSC_data_access_infos.size() << " CSC collections: "
		    << endreq;
              for (unsigned int i=0;i<CSC_data_access_infos.size();i++)
	          msg() << MSG::DEBUG << "  Collection " << i 
		        << ": ROB ID = " << std::hex 
			<< CSC_data_access_infos[i].robID << std::dec
			<< ",  hash ID = " << CSC_data_access_infos[i].hashID
			<< endreq;
          }
      }
  }

  //add CSC ROBs to this list
  for (unsigned int i=0;i<CSC_data_access_infos.size();i++)
  {
      uint32_t robId = CSC_data_access_infos[i].robID;
      robIds.push_back(robId);
      if(isRobReadout(robId) && isRobEnabled(robId)) {
          csc_rob_ids.push_back(robId);
          listOfCSCIds.push_back(CSC_data_access_infos[i].hashID);
      }
  }

  
  //screen the list of requested ROB, remove unavailable robs
  m_rob_removed.clear();
  std::vector<uint32_t> vec;
  vec.clear();
  
  if(m_details!=0) {
      for(std::vector<uint32_t>::const_iterator it=robIds.begin();
          it!=robIds.end();++it) m_details->setRequestedRob(*it);
  }
  
  this->screen_robs(robIds,vec);
  std::set<uint32_t>::const_iterator rem;
  for (rem=m_rob_removed.begin(); rem!=m_rob_removed.end();rem++) {
      
      if(m_details) m_details->setRemovedRob(*rem);
      
      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "removed rob = " << std::hex << *rem 
	        << std::dec << endreq;
      }
      std::vector<uint32_t>::iterator rob = mdt_rob_ids.begin();
      while (rob!=mdt_rob_ids.end()) {
          if(*rob == *rem) rob = mdt_rob_ids.erase(rob);
	  else rob++;
      }
  }
  
  if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " List of requested ROBs = "  << endreq;
      for(unsigned int i=0;i<vec.size();++i) {
          msg() << MSG::DEBUG << std::hex << vec[i] << std::dec << endreq;
      }
  }

  m_pROBDataProvider->addROBData(vec);

  
  
  
  // *************************************************************************
  // ************************* ACCESS TRIGGER DATA ***************************
  // *************************************************************************

#ifdef INSTRUMENTATION
  INSTRUMENT("PESA_muFast_REQUEST_TRIGGER_DATA",m_current_lvl1_id);
#endif    
  
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobs;

  if ( systemID != 0 )      // ACCESSING TGC DATA 
  {
  // Get the TGC RDOs ////////////////////////////////////////////////////////
      const TgcRdoContainer* TGC =                                          //
           Muon::MuonRdoContainerAccess::retrieveTgcRdo("TGCRDO");          //
                                                                            //
      vecRobs.clear();                                                      //
                                                                            //
      if(TGC==0)                                                            //
      {                                                                     //
          if ( msgLvl() <= MSG::DEBUG )                                     //
              msg() << MSG::DEBUG << "Tgc RDO container not registered by " //
	                      << "MuonRdoContainerManager; retrieving it "  //
			      << "from the store! " << endreq;              //
                                                                            //
          sc = m_pStoreGate->retrieve(TGC, "TGCRDO");                       //
          if (sc.isFailure()){                                              //
              msg() << MSG::ERROR << "Retrieval of TgcRdoContainer failed"  //
                                  << endreq;                                //
              fillNtp(MUFAST::TGC_CONTAINER_UNAVAILABLE,&robIds,&data,0,    //
	                                                  dummy_digit,0,0); //
	      return HLT::SG_ERROR;                                         //
          }                                                                 // 
      }                                                                     //
                                                                            //
      if( isRobReadout(trigger_rob_id) && isRobEnabled(trigger_rob_id) ) {  //
                                                                            //
          m_pROBDataProvider->                                              //
              getROBData(std::vector<uint32_t>(1,trigger_rob_id),vecRobs);  // 
                                                                            //
          IdentifierHash tgc_id = TGC->idToHash(tgc_rdo_id);                //
                                                                            //
          if(m_tgc_converter->convert(vecRobs).isFailure())                 //
          {                                                                 //
	     // CID 26647: UNREACHABLE                                      //
              msg() << MSG::ERROR << "Failed to convert TGC RDO Id "        //
                                  << tgc_rdo_id << endreq;                  //
              MuonFeature* muon_feature =                                   //
                     new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0);  //
              return muFastSeed(MUFAST::TGC_CONVERSION_FAILURE, inputTE,    //
	                        outputTE, &robIds, &data, 0, dummy_digit,   //
	      			                       0, muon_feature);    //
						                            //
              //fillNtp(MUFAST::TGC_CONVERSION_FAILURE,&robIds,&data,0,       //
	      //                                            dummy_digit,0,0); //
	      //return HLT::SG_ERROR;                                         //
          }                                                                 //
                                                                            //
          TgcRdoContainer::const_iterator itTGC =  TGC->indexFind(tgc_id);  //
                                                                            //
          if(itTGC==TGC->end())                                             //
          {                                                                 //
              msg() << MSG::DEBUG << "Failed to retrieve TGC hash Id "      // 
	                          << tgc_id << endreq;                      //
	      msg() << MSG::DEBUG << "Send dummy MuonFeature " << endreq;   //
	                                                                    //
	      MuonFeature* muon_feature =                                   //
                    new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0);   //
              return muFastSeed(MUFAST::TGC_COLLECTION_MISSING, inputTE,    //
	                        outputTE, &robIds, &data, 0, dummy_digit,   //
	                                0,muon_feature);                    //
						                            //
              //msg() << MSG::ERROR << "Failed to retrieve TGC hash Id "      // 
	      //                    << tgc_id << endreq;                      //
              //fillNtp(MUFAST::TGC_COLLECTION_MISSING,&robIds,&data,0,       //
	      //                                            dummy_digit,0,0); //
	      //return HLT::SG_ERROR;                                         //
          }                                                                 //
          data.get_tgc(*itTGC);                                             //
      }                                                                     //
  ////////////////////////////////////////////////////////////////////////////
  }
  else                        // ACCESSING RPC DATA 
  {
      if(m_PadGet_time) m_PadGet_time->start();
      // Get the Data for the RPC PADs ///////////////////////////////////////
      const RpcPadContainer* PAD =                                          //
           Muon::MuonRdoContainerAccess::retrieveRpcPad("RPCPAD");          //
	                                                                    //
      vecRobs.clear();                                                      //
                                                                            //
      if(PAD==0)                                                            //
      {                                                                     //
          if ( msgLvl() <= MSG::DEBUG )                                     //
              msg() << MSG::DEBUG << "Rpc PAD container not registered by " //
	                      << "MuonRdoContainerManager; retrieving it "  //
			      << "from the store! " << endreq;              //
                                                                            //
          sc = m_pStoreGate->retrieve(PAD, "RPCPAD");                       //
          if (sc.isFailure()){                                              //
              msg() << MSG::ERROR << "Retrieval of RpcPadContainer failed"  //
                                  << endreq;                                //
              fillNtp(MUFAST::RPC_CONTAINER_UNAVAILABLE, &robIds, &data, 0, //
	                                               dummy_digit, 0, 0);  //
	      return HLT::SG_ERROR;                                         //
          }                                                                 // 
      }                                                                     //
                                                                            //
      const RpcPad* rpc_pad = 0;                                            //
                                                                            //
      if( isRobReadout(trigger_rob_id) && isRobEnabled(trigger_rob_id) ) {  //
                                                                            //
          m_pROBDataProvider->                                              //
              getROBData(std::vector<uint32_t>(1,trigger_rob_id),vecRobs);  //
	                                                                    //
          // IdentifierHash pad_id = PAD->idToHash(rpc_pad_id);                //
	  RpcPadIdHash padHashFunction = *(m_svc->iRpcCablingSvc()->padHashFunction());
	  IdentifierHash pad_id = padHashFunction(rpc_pad_id);
                                                                            //
          if(m_rpc_converter->convert(vecRobs,                              //
                        std::vector<IdentifierHash>(1,pad_id)).isFailure()) //
          {                                                                 //
                MuonFeature* muon_feature =                                 //
                       new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0);//
                return muFastSeed(MUFAST::RPC_CONVERSION_FAILURE, inputTE,  //
	                        outputTE, &robIds, &data, 0, dummy_digit,   //
                                      0, muon_feature);                     //
	                                                                    //
              msg() << MSG::ERROR << "Failed to convert PAD offline Id "    //
                    << rpc_pad_id.get_compact()                             //
	            << endreq;                                              //
              //fillNtp(MUFAST::RPC_CONVERSION_FAILURE, &robIds, &data, 0,    //
	      //                                         dummy_digit, 0, 0);  //
	      //return HLT::SG_ERROR;                                         //
          }                                                                 //
                                                                            //
          RpcPadContainer::const_iterator itPAD = PAD->indexFind(pad_id);   //
          if(itPAD==PAD->end())                                             //
          {                                                                 //
              msg() << MSG::DEBUG << "Failed to retrieve PAD hash Id "      //
	                          << pad_id << endreq;                      //
	      msg() << MSG::DEBUG << "Send dummy MuonFeature " << endreq;   //
	                                                                    //
	      MuonFeature* muon_feature =                                   //
                    new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0);   //
              return muFastSeed(MUFAST::RPC_COLLECTION_MISSING, inputTE,    //
	                      outputTE, &robIds, &data, 0, dummy_digit,     //
              		                       0, muon_feature);            //
						                            //
              //msg() << MSG::ERROR << "Failed to retrieve PAD hash Id "      //
	      //                    << pad_id << endreq;                      //
              //fillNtp(MUFAST::RPC_COLLECTION_MISSING, &robIds, &data, 0,    //
	      //                                         dummy_digit, 0, 0);  //
	      //return HLT::SG_ERROR;                                         //
          }                                                                 //
                                                                            //
          rpc_pad = *itPAD;                                                 //
                                                                            //
                                                                            //
          if(m_PadGet_time)                                                 //
          {                                                                 //
              m_PadGet_time->stop();                                        //
              int Cma = 0;                                                  //
              RpcPad::const_iterator it3 = rpc_pad->begin();                //
              RpcPad::const_iterator it4 = rpc_pad->end();                  //
              for (; it3!=it4; ++it3)                                       //
              {                                                             //
                  if((*it3)->size()) ++Cma;                                 //
              }                                                             //
              m_PadGet_time->propVal(Cma);                                  //
          }                                                                 //
                                                                            //
          data.get_pad(rpc_pad);                                            //
      }                                                                     //
      ////////////////////////////////////////////////////////////////////////

  }
  
#ifdef INSTRUMENTATION
  INSTRUMENT("PESA_muFast_RECEIVED_TRIGGER_DATA",m_current_lvl1_id);
#endif





  // *************************************************************************
  // *********************** MUFAST ALGORITHM START **************************
  // *************************************************************************
  
  if(m_muAlgo_time) m_muAlgo_time->start();
  
  m_l1emuFailed_eta.clear();
  m_l1emuFailed_phi.clear();

  m_backextrFailed_eta.clear();
  m_backextrFailed_phi.clear();

  // Level-1 emulation ///////////////////////////////////////////////////////
  LVL1emu  lvl1_emu(*m_msg,*m_svc,m_maskUncCMAch,m_trigOnPhiOnly,           //
                                    m_useRpcTrigPatt,m_RpcTimeDelay,data);  //
  MUFAST::ErrorCode code = lvl1_emu.start(m_details);                       //
  if( code!=0 )                                                             //
  {                                                                         //
      MuonFeature* muon_feature =                                           //
                      new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0); //

      std::string algoName = m_svc->name();
      if( (algoName.find("_Muon")     != std::string::npos && 
	   algoName.find("_MuonEcut") == std::string::npos) ||
	  algoName.find("_900GeV") != std::string::npos ) {
	 m_l1emuFailed_eta.push_back( m_pMuonRoI->eta() );
	 m_l1emuFailed_phi.push_back( m_pMuonRoI->phi() );
      }

      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobs; //
      extractCalData(systemID,subsystemID,sectorID,RoInumber,tgc_rdo_id,    //
                         robIds, &data, 0, 0, 0, vecRobs, muon_feature);    //
                                                                            //
      return muFastSeed(code, inputTE, outputTE, &robIds, &data, 0,         //
                                         dummy_digit,0, muon_feature);      //
  }                                                                         //
                                                                            //
  if(systemID == 0)                                                         //
  {                                                                         //
      m_barrel_sector_rec = sectorID;                                       //
      m_barrel_pad_rec    = (subsystemID==1)? PADId+1 : -(PADId+1);         //
  }else{                                                                    //
      //=AI added                                                           //
      m_endcap_eta_rec.push_back(TGCRoIeta);                                //
      m_endcap_phi_rec.push_back(TGCRoIphi);                                //
      //=end                                                                //
  }                                                                         //
  const TriggerData& LVL1trig_data = lvl1_emu.trigger_data();               //
  ////////////////////////////////////////////////////////////////////////////

  m_emu_pt.clear();
  m_emu_phi.clear();
  m_emu_eta.clear();

  m_emu_pt.push_back(LVL1trig_data.threshold+1);
  double emu_phi = -999.;
  if(LVL1trig_data.rpc2[0]!=0)
    emu_phi=atan2(LVL1trig_data.rpc2[1],LVL1trig_data.rpc2[0]);

  m_emu_phi.push_back(emu_phi);
  float emu_r = sqrt(LVL1trig_data.rpc2[1]*LVL1trig_data.rpc2[1]+LVL1trig_data.rpc2[0]*LVL1trig_data.rpc2[0]);
  double emu_eta = 0.;
  if(LVL1trig_data.rpc2[2]!=0){
    float emu_theta = atan(emu_r/LVL1trig_data.rpc2[2]);
    //if(emu_theta<0) emu_theta+=2*M_PI;
    emu_eta=-log(tan(fabs(emu_theta)/2))*emu_theta/fabs(emu_theta);
  } else 
    emu_eta = -999.;

  m_emu_eta.push_back(emu_eta);
  
//  TrackFit rpc_track;
//  RoadRegions muon_roads;

  
  if(m_pat_rec_time) m_pat_rec_time->start(); 
  // RPC Pattern Recognition /////////////////////////////////////////////////
  RpcPatRec RPCpatternRecognition(*m_msg,*m_svc,m_tilted_B_field,           //
				  m_usePol2ForEPt, LVL1trig_data);          //
  code = RPCpatternRecognition.start(m_details);                            //
  if( code!=0 )                                                             //
  {                                                                         //
      MuonFeature* muon_feature =                                           //
                      new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0); //
                                                                            //
      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobs; //                                                                     //
      extractCalData(systemID,subsystemID,sectorID,RoInumber,tgc_rdo_id,    //
                         robIds, &data, &lvl1_emu, 0, 0,                    //
			 vecRobs, muon_feature);                            //
                                                                            //
      return muFastSeed(code, inputTE, outputTE, &robIds, &data, &lvl1_emu, //
                                              dummy_digit,0,muon_feature);  //
  }                                                                         //
                                                                            //
  const TrackFit rpc_track     = RPCpatternRecognition.trig_track();        //
  const RoadRegions muon_roads = RPCpatternRecognition.muon_roads();        //
  ////////////////////////////////////////////////////////////////////////////
  if(m_muAlgo_time) m_muAlgo_time->pause();
  


#ifdef INSTRUMENTATION
  INSTRUMENT("PESA_muFast_REQUEST_MDT_DATA",m_current_lvl1_id);
#endif
  
  
  
  if(m_RegionSelector_time) m_RegionSelector_time->start();

  m_mdt_unavailable.clear();
  m_mdt_disabledOnFly.clear();

  // Ask for Identifiers of the Collections using Muon Roads /////////////////
                                                                            //
  std::vector<IdentifierHash>* idList=new (std::vector<IdentifierHash>);    //
                                                                            //
  for (int station=0;station<3;++station)                                   //
  {                                                                         //
    float EtaMin = muon_roads.etaMin[0][station];                           //
    float EtaMax = muon_roads.etaMax[0][station];                           //
    const int& ty1 = muon_roads.ChamberTypes[0][station][0];                //
    const int& ty2 = muon_roads.ChamberTypes[0][station][1];                //
                                                                            //
    /// create the roi descriptor for the region selector                   //
    TrigRoiDescriptor _roi( 0.5*(EtaMin+EtaMax), EtaMin, EtaMax,            //
			    HLT::phiMean(PhiMin,PhiMax), PhiMin, PhiMax );  // 
                                                                            //
    std::vector<IdentifierHash>::const_iterator it;                         //
    if(ty1 != -1)                                                           //
    {                                                                       //
                                                                            //
      m_pRegionSelector->DetHashIDList(MDT,static_cast<TYPEID>(ty1),        //
				       _roi, *idList);                      //
       it = (*idList).begin();                                              //
       while(it != (*idList).end()) listOfMDTIds.push_back(*it++);          //
       (*idList).clear();                                                   //
    }                                                                       //
    if(ty2 != -1)                                                           //
    {                                                                       //
      m_pRegionSelector->DetHashIDList(MDT,static_cast<TYPEID>(ty2),        //
				       _roi,*idList);                       //
       it = (*idList).begin();                                              //
       while(it != (*idList).end()) listOfMDTIds.push_back(*it++);          //
       (*idList).clear();                                                   //
    }                                                                       //
  }                                                                         //
  delete idList;                                                            //
                                                                            //
  if(msgLvl() <= MSG::DEBUG) {                                              //
      msg() << MSG::DEBUG                                                   //
            << "List of MDT Chambers from Region Selector ..." << endreq;   //
      for (unsigned int i=0;i<listOfMDTIds.size();i++)                      //
          msg() << MSG::DEBUG                                               //
                << "MDT chamber n.  " << i                                  //
                << ": hash id " << listOfMDTIds[i]                          //
                << endreq;                                                  //
  }                                                                         //
                                                                            //
  for (rem=m_rob_removed.begin(); rem!=m_rob_removed.end();rem++) {         //
      std::vector<IdentifierHash>::iterator mdt=listOfMDTIds.begin();       //
      while (mdt!=listOfMDTIds.end()) {                                     //
          bool result = m_svc->mdtgeo()->checkHashIdForROB(*mdt,*rem);      //
	  if(result) {                                                      //
	      unsigned int hash = *mdt;                                     //
	      m_mdt_unavailable.insert(hash);                               //
	      int station = m_svc->mdtgeo()->hash2Station(*mdt);            //
	      m_data_access_error |= (1 << (station+7) );                   //
	      mdt = listOfMDTIds.erase(mdt);                                //
	      if(m_details!=0) m_details->setRemovedCsm(hash);              //
	  } else mdt++;                                                     //
      }                                                                     //
  }                                                                         //
                                                                            //
                                                                            //
  if(msgLvl() <= MSG::DEBUG) {                                              //
      msg() << MSG::DEBUG                                                   //
             << "List of MDT Chambers requested from the converter ..."     //
	     << endreq;                                                     //
      for (unsigned int i=0;i<listOfMDTIds.size();i++)                      //
          msg() << MSG::DEBUG                                               //
                << "MDT chamber n.  " << i                                  //
                << ": hash id " << listOfMDTIds[i]                          //
                << endreq;                                                  //
  }                                                                         //
  ////////////////////////////////////////////////////////////////////////////
  if(m_RegionSelector_time) m_RegionSelector_time->stop();


  if(m_MdtGet_time) m_MdtGet_time->start();  
  // Get the Container for the CSMs //////////////////////////////////////////
  const MdtCsmContainer* CSM =                                              //
           Muon::MuonRdoContainerAccess::retrieveMdtCsm("MDTCSM");          //
	                                                                    //
  vecRobs.clear();                                                          //
                                                                            //
  if(CSM==0)                                                                //
  {                                                                         //
     if ( msgLvl() <= MSG::DEBUG )                                          //
          msg() << MSG::DEBUG << "MDT CSM container not registered by "     //
	                      << "MuonRdoContainerManager; retrieving it "  //
			      << "from the store! " << endreq;              //
                                                                            //
      sc = m_pStoreGate->retrieve(CSM, "MDTCSM");                           //
      if (sc.isFailure()){                                                  //
          msg() << MSG::ERROR << "Retrieval of MdtCsmContainer failed"      //
                              << endreq;                                    //
          return HLT::SG_ERROR;                                             //
      }                                                                     // 
  }                                                                         //
                                                                            //
  m_pROBDataProvider->getROBData(mdt_rob_ids,vecRobs);                      //
                                                                            //
  if(m_mdt_converter->convert(vecRobs,listOfMDTIds).isFailure())            //
  {                                                                         //
        MuonFeature* muon_feature =                                         //
                      new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0); //
      return muFastSeed(MUFAST::MDT_CONVERSION_FAILURE, inputTE, outputTE,  //
                        &robIds, &data, 0, dummy_digit,0,muon_feature);     //
      msg() << MSG::ERROR                                                   //
            << "Failed to convert MDT CSM hash Ids: " << endreq;            //
      for(unsigned int i = 0; i < listOfMDTIds.size(); i++)                 //
      {                                                                     //
          msg() << MSG::ERROR << listOfMDTIds[i] << endreq;                 //
      }                                                                     //
      //fillNtp(MUFAST::MDT_CONVERSION_FAILURE, &robIds, &data, &lvl1_emu,    //
	//                                               dummy_digit, 0, 0);  //
      //return HLT::SG_ERROR;                                                 //
  }                                                                         //
                                                                            //
  for(unsigned int i = 0; i < listOfMDTIds.size(); i++)                     //
  {                                                                         //
      MdtCsmContainer::const_iterator pCsmIt =                              //
      CSM->indexFind(listOfMDTIds[i]);                                      //
                                                                            //
      if(pCsmIt!=CSM->end())                                                //
      {                                                                     //
          if(m_details) m_details->setConvertedCsm(listOfMDTIds[i],         //
	                                             (*pCsmIt)->size());    //
          if(msgLvl() <= MSG::DEBUG) {                                      //
	      msg() << MSG::DEBUG                                           //
	            << "MDT Collection hash " << listOfMDTIds[i]            //
		    << " associated to:  SubDet 0x" << MSG::hex             //
		    << (*pCsmIt)->SubDetId() << " MRod 0x"                  //
		    << (*pCsmIt)->MrodId() << " Link 0x"                    //
		    << (*pCsmIt)->CsmId() << MSG::dec << endreq;            //
              msg() << MSG::DEBUG                                           //
	            << "Number of digit in  MDT Collection "                //
		    << listOfMDTIds[i] << ": " << (*pCsmIt)->size()         //
	            << endreq;                                              //
          }                                                                 //
          data.get_csm(*pCsmIt);                                            //
      }                                                                     //
  }                                                                         //
  ////////////////////////////////////////////////////////////////////////////
  if(m_MdtGet_time) m_MdtGet_time->stop();
  if(m_MdtGet_time) m_MdtGet_time->propVal(listOfMDTIds.size());
  
  if(LVL1trig_data.MDT_Sector_for_overlap != 99)
  {
    listOfMDTIds.clear();
    if(m_RegionSelector_time) m_RegionSelector_time->start();
    // Ask for Identifiers of the Collections using Muon Roads ///////////////
                                                                            //
    std::vector<IdentifierHash>* idList=new (std::vector<IdentifierHash>);  //
                                                                            //
    for (int station=0;station<3;++station)                                 //
    {                                                                       //
      float EtaMin = muon_roads.etaMin[1][station];                         //
      float EtaMax = muon_roads.etaMax[1][station];                         //
      const int& ty1 = muon_roads.ChamberTypes[1][station][0];              //
      const int& ty2 = muon_roads.ChamberTypes[1][station][1];              //
                                   
      /// create the roi descriptor for the region selector                 //
      TrigRoiDescriptor _roi( 0.5*(EtaMin+EtaMax), EtaMin, EtaMax,          //
			      HLT::phiMean(PhiMin,PhiMax), PhiMin, PhiMax );// 
                                                                            //
      std::vector<IdentifierHash>::const_iterator it;                       //
      if(ty1 != -1)                                                         //
      {                                                                     //
         m_pRegionSelector->DetHashIDList(MDT,static_cast<TYPEID>(ty1),     //
					  _roi, *idList);                   //
	 it = (*idList).begin();                                            //
         while(it != (*idList).end()) listOfMDTIds.push_back(*it++);        //
         (*idList).clear();                                                 //
      }                                                                     //
      if(ty2 != -1)                                                         //
      {                                                                     //
         m_pRegionSelector->DetHashIDList(MDT,static_cast<TYPEID>(ty2),     //
					  _roi, *idList);                   //
         it = (*idList).begin();                                            //
         while(it != (*idList).end()) listOfMDTIds.push_back(*it++);        //
         (*idList).clear();                                                 //
      }                                                                     //
    }                                                                       //
    delete idList;                                                          //
                                                                            //
    for (rem=m_rob_removed.begin(); rem!=m_rob_removed.end();rem++) {       //
      std::vector<IdentifierHash>::iterator mdt=listOfMDTIds.begin();       //
      while (mdt!=listOfMDTIds.end()) {                                     //
          bool result = m_svc->mdtgeo()->checkHashIdForROB(*mdt,*rem);      //
	  if(result) mdt = listOfMDTIds.erase(mdt);                         //
	  else mdt++;                                                       //
      }                                                                     //
    }                                                                       //
                                                                            //
    if(m_mdt_converter->convert(vecRobs,listOfMDTIds).isFailure())          //
    {                                                                       //
      msg() << MSG::ERROR                                                   //
            << "Failed to convert overlap MDT CSM hash Ids: " << endreq;    //
      for(unsigned int i = 0; i < listOfMDTIds.size(); i++)                 //
      {                                                                     //
          msg() << MSG::ERROR << listOfMDTIds[i] << endreq;                 //
      }                                                                     //
    }                                                                       //
    if(msgLvl() <= MSG::DEBUG) {                                            //
        for (unsigned int i=0;i<listOfMDTIds.size();i++)                    //
            msg() << MSG::DEBUG                                             //
                  << "MDT overlap chamber n.  " << i                        //
                  << ": hash id " << listOfMDTIds[i]                        //
                  << endreq;                                                //
    }                                                                       //
    //////////////////////////////////////////////////////////////////////////
    if(m_RegionSelector_time) m_RegionSelector_time->stop();


    if(m_MdtGet_time) m_MdtGet_time->start();
    // Get the Container for the CSMs ////////////////////////////////////////
    for(unsigned int i = 0; i < listOfMDTIds.size(); i++)                   //
    {                                                                       //
        MdtCsmContainer::const_iterator pCsmIt =                            //
        CSM->indexFind(listOfMDTIds[i]);                                    //
                                                                            //
        if(pCsmIt!=CSM->end())                                              //
        {                                                                   //
            if(msgLvl() <= MSG::DEBUG) {                                    //
                msg() << MSG::DEBUG                                         //
	            << "MDT overlap Collection hash " << listOfMDTIds[i]    //
		    << " associated to:  SubDet 0x" << MSG::hex             //
		    << (*pCsmIt)->SubDetId() << " MRod 0x"                  //
		    << (*pCsmIt)->MrodId() << " Link 0x"                    //
		    << (*pCsmIt)->CsmId() << MSG::dec << endreq;            //
		msg() << MSG::DEBUG                                         //
	              << "Number of digit in  MDT overlap Collection "      //
		      << listOfMDTIds[i] << ": " << (*pCsmIt)->size()       //
	              << endreq;                                            //
            }                                                               //
            data.get_ovl(*pCsmIt);                                          //
        }                                                                   //
    }                                                                       //
    //////////////////////////////////////////////////////////////////////////
  }
  if(m_MdtGet_time) m_MdtGet_time->stop();
  if(m_MdtGet_time) m_MdtGet_time->propVal(listOfMDTIds.size());
  


#ifdef INSTRUMENTATION
 INSTRUMENT("PESA_muFast_RECEIVED_MDT_DATA",m_current_lvl1_id);
#endif

  if(m_muAlgo_time) m_muAlgo_time->resume();
  vecRobs.clear();
  ////////////////////////////////////////////////////////////////////////////
  if (csc_rob_ids.size()!=0)                                                //
  {                                                                         //
      const CscRawDataContainer* CSC =                                      //
           Muon::MuonRdoContainerAccess::retrieveCscRaw("CSCRDO");          //
	                                                                    //
     if(CSC==0)                                                             //
     {                                                                      //
        if ( msgLvl() <= MSG::DEBUG )                                       //
             msg() << MSG::DEBUG << "CSC RAW container not registered by "  //
	                      << "MuonRdoContainerManager; retrieving it "  //
			      << "from the store! " << endreq;              //
                                                                            //
         sc = m_pStoreGate->retrieve(CSC, "CSCRDO");                        //
         if (sc.isFailure()){                                               //
             msg() << MSG::ERROR << "Retrieval of CscRawContainer failed"   //
                                 << endreq;                                 //
             return HLT::SG_ERROR;                                          //
         }                                                                  // 
     }                                                                      //
                                                                            //
     m_pROBDataProvider->getROBData(csc_rob_ids,vecRobs);                   //
     if(m_csc_converter->convert(vecRobs,listOfCSCIds).isFailure())         //
     {                                                                      //
         msg() << MSG::WARNING                                              //
               << "Failed to convert CSC Raw hash Ids: " << endreq;         //
         msg() << MSG::WARNING                                              //
               << "Data delivered into the Muon Calibration Stream"         // 
	       << endreq;                                                   //
	 //for(unsigned int i = 0; i < listOfCSCIds.size(); i++)              //
         //{                                                                  //
         //    msg() << MSG::ERROR << listOfCSCIds[i] << endreq;              //
         //}                                                                  //
         //fillNtp(MUFAST::CSC_CONVERSION_FAILURE, &robIds, &data, &lvl1_emu, //
	 //                                 dummy_digit, 0, 0);               //
         //return HLT::SG_ERROR;                                              //
     }                                                                      //
                                                                            //
     for(unsigned int i = 0; i < listOfCSCIds.size(); i++)                  //
     {                                                                      //
         CscRawDataContainer::const_iterator pCscIt =                       //
         CSC->indexFind(listOfCSCIds[i]);                                   //
                                                                            //
         if(pCscIt!=CSC->end())                                             //
         {                                                                  //
             if(msgLvl() <= MSG::DEBUG)                                     //
                 msg() << MSG::DEBUG                                        //
	               << "Number of hit in CSC Raw Collection "            //
		       << listOfCSCIds[i] << ": "                           //
                       << (*pCscIt)->size()                                 //
	               << endreq;                                           //
                                                                            //
          //data.get_csm(*pCscIt);                                          //
         }                                                                  //
     }                                                                      //
  }                                                                         //
  ////////////////////////////////////////////////////////////////////////////
  
  
  // MDT Pattern Recognition /////////////////////////////////////////////////
  MdtPatRec MDTpatternRecognition(*m_msg,*m_svc,rpc_track,muon_roads,data); //
  code = MDTpatternRecognition.start(m_details);                            //
  if( code!=0 )                                                             //
  {                                                                         //
      MuonFeature* muon_feature =                                           //
                      new MuonFeature(FeatureAdd,0.,0.,0.,0.,0.,0.,0.,1.0); //
                                                                            //
      extractCalData(systemID,subsystemID,sectorID,RoInumber,tgc_rdo_id,    //
                         robIds, &data, &lvl1_emu, &RPCpatternRecognition,  //
			 0, vecRobs, muon_feature);                         //
                                                                            //
      return muFastSeed(code, inputTE, outputTE,                            //
                       &robIds,&data,&lvl1_emu,dummy_digit,0,muon_feature); //
  }                                                                         //
                                                                            //
  const DigiPointer reco_digits = MDTpatternRecognition.digits();           //
  ////////////////////////////////////////////////////////////////////////////
  if(m_pat_rec_time) m_pat_rec_time->stop();
  
  // Features extraction /////////////////////////////////////////////////////
  if(m_muon_fex_time) m_muon_fex_time->start();                             //
  MuonFex fexExtraction(*m_msg,*m_svc,m_tilted_B_field,LVL1trig_data,       //
                         rpc_track,reco_digits.digi);                       //
  if( m_usePol2ForEPt ) fexExtraction.setEPtStrategy(MuonFexConsts::EPtStrategy_pol2);
  code = fexExtraction.start(m_details);                                    //
  if( code == MUFAST::EST_ALPHABETA_ERROR )
  { // just to monitor
      std::string algoName = m_svc->name();
      if( algoName.find("_OutBCID") == std::string::npos ) {
	 m_backextrFailed_eta.push_back( m_pMuonRoI->eta() );
	 m_backextrFailed_phi.push_back( m_pMuonRoI->phi() );
      }
  }
  else if( code!=0 ) 
  { 
      MuonFeature* muon_feature =                                           //
                      new MuonFeature(FeatureAdd,0.,0.,EtaRoI,PhiRoI,0.,0., //
		                                                   0.,1.0); //
                                                                            //
      if(!m_confirmCalTrack) {                                              //
          MuonFeature* dummy = new MuonFeature(FeatureAdd,0.,0.,            //
	                                       EtaRoI,PhiRoI,0.,0.,0.,1.0); //
          extractCalData(systemID,subsystemID,sectorID,RoInumber,tgc_rdo_id,//
                         robIds, &data, &lvl1_emu, &RPCpatternRecognition,  //
		         &MDTpatternRecognition, vecRobs, muon_feature);    //
          delete dummy;                                                     //
      }                                                                     //
                                                                            //
      return muFastSeed(code, inputTE, outputTE, &robIds, &data, &lvl1_emu, //
                                 reco_digits, &fexExtraction,muon_feature); //
  }                                                                         //
  if(m_muon_fex_time) m_muon_fex_time->stop();                              //
                                                                            //
  MuonFeature* muon_feature = fexExtraction.muonFeatures();                 //
  ////////////////////////////////////////////////////////////////////////////  
  
  
  extractCalData(systemID,subsystemID,sectorID,RoInumber,tgc_rdo_id,
    robIds, &data, &lvl1_emu, &RPCpatternRecognition, &MDTpatternRecognition,
    vecRobs, muon_feature);
  
  
  if(m_muAlgo_time) m_muAlgo_time->stop();
  if(m_muAlgo_time) m_muAlgo_time->propVal((int)muon_feature->pt());



  // *************************************************************************
  // ********************** START MONITORING SECTION *************************
  // *************************************************************************
  
  if(m_monitor_time) m_monitor_time->start();
  //m_monitor->update(lvl1_emu,MDTpatternRecognition,fexExtraction);
  m_monitor->update(MDTpatternRecognition,fexExtraction);
  m_inner_mdt_hits  = m_monitor->innHits();
  m_middle_mdt_hits = m_monitor->midHits();
  m_outer_mdt_hits  = m_monitor->outHits();
  m_fit_residuals   = m_monitor->residuals();
  m_efficiency      = m_monitor->eff();
  m_sag_inverse     = m_monitor->saginv();
  m_address         = m_monitor->pos();
  m_absolute_pt     = m_monitor->aPt();
  m_sagitta         = m_monitor->sagitta();
  m_res_inner       = m_monitor->resInn();
  m_res_middle      = m_monitor->resMid();
  m_res_outer       = m_monitor->resOut();

  //=AI added
  if( fabs(muon_feature->eta()) > 1e-5 || fabs(muon_feature->phi()) > 1e-5 ) {
     m_track_eta.push_back(muon_feature->eta());
     m_track_phi.push_back(muon_feature->phi());
  }
  //=end

  if(systemID == 0)
  {
      if(m_efficiency==1)
      {
          m_barrel_sector_track_eff = sectorID;
          m_barrel_pad_track_eff    = (subsystemID==1)? PADId+1 : -(PADId+1);
      }
  }else{
    //=AI added                                                           
    if(m_efficiency==1){

      m_endcap_eta_eff.push_back(TGCRoIeta);                                //
      m_endcap_phi_eff.push_back(TGCRoIphi);                                //
    }
    //=end
  }
  
  if(m_monitor_time) m_monitor_time->stop();

  if(m_muFast_time) m_muFast_time->stop();
  
  
#ifdef INSTRUMENTATION
    INSTRUMENT("PESA_muFast_END_PROCESSING",m_current_lvl1_id);
#endif    
  

  
  // output the Muon Feature to seed next Algorithm
  return muFastSeed(MUFAST::NO_ERROR,inputTE,outputTE,
           &robIds,&data,&lvl1_emu,reco_digits,&fexExtraction, muon_feature);
}


void
muFast::extractCalData(
    uint16_t systemID,  uint16_t subsystemID,  uint16_t sectorID, 
    uint16_t RoInumber, unsigned int tgc_rdo_id,
    std::vector<uint32_t> robIds, const MuData* data,
    const LVL1emu* lvl1_rec, const RpcPatRec* rpc_rec, MdtPatRec* mdt_rec,
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& cscRobs,
    MuonFeature* muon_feature) {

  if (m_doCalStream && (m_CalibrationStream || m_cid!=-1) &&
      lvl1_rec != 0 && rpc_rec != 0 && mdt_rec != 0)
  {
      uint16_t subsys = 0x0;
      uint16_t rod_id = 0x0;

      if (systemID==0) subsys = (subsystemID==1) ? 0x65 : 0x66;
      else             subsys = (subsystemID==1) ? 0x67 : 0x68;

      if (systemID==0) rod_id = sectorID/2;
      else             rod_id = tgc_rdo_id;
      
      
      const TrackFit rpc_track         = rpc_rec->trig_track();     
      const DigiPointer reco_digits    = mdt_rec->digits();
      const DigiPointer overlap_digits = mdt_rec->overlap();
      streamOutCalData(systemID, subsys,
		       rod_id, RoInumber, robIds, data->rpc_pad(),
		       lvl1_rec->tgc_digits(), rpc_track, reco_digits, 
		       overlap_digits, cscRobs,muon_feature);
  }
  
  if (m_doCalBuild)
  {
     PartialEventBuildingInfo* pEBInfo = config()->getPEBI();
      
      if (pEBInfo) {
          // add ROBs
          pEBInfo->addROBs(robIds);
      } else {
          msg() << MSG::ERROR 
	  << "Error creating Partial Event Building Info object. Pointer = 0x" 
	  << MSG::hex << pEBInfo << MSG::dec << endreq;
          msg() << MSG::ERROR << "Calibration data will not be streamed" 
	            << endreq;
      }
  }
}


void
muFast::streamOutCalData(uint16_t system, uint16_t subsystem, uint16_t rod_id,
			 uint16_t roi_number, std::vector<uint32_t> robs,
                         const RpcPad* pad, const LUTDigitVec& tgc_rod_hit,
			 const TrackFit track_fit, const DigiPointer mdtDigits,
			 const DigiPointer ovlDigits,
			 std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& cscRobs,
			 MuonFeature* fea) {
    
    uint32_t trigger_mrod = robs[0];
    robs.erase(robs.begin());
    uint32_t mrods[4] = {0x0,0x0,0x0,0x0};
    for (unsigned int i=0;i<robs.size();++i) if (i<4) mrods[i] = robs[i];
    
    LVL2_MUON_CALIBRATION::CalibEvent  event(1,m_current_run_id,
                                       m_current_lvl1_id,m_l2pu_id,
                                       trigger_mrod, mrods, name().c_str(), 
				       fea->eta(),fea->phi(),fea->pt());

    float phi  = track_fit.phi;

    LVL2_MUON_CALIBRATION::MdtCalibFragment MdtFragment;
    
    LayerDigits (*digits)[MAXLAYER] = mdtDigits.digi;
    for(int st=0;st<MAXSTATION;st++)
    { 
	for(int ly=0;ly<MAXLAYER;ly++)
	{ 
	    DigitData* firstDig = digits[st][ly].first;
	    DigitData *nextDig = 0;
            if(firstDig)
	    {
	        nextDig = firstDig;
	        do {
		
		    int StationName  = nextDig->IdMDT->name;
		    int StationEta   = nextDig->IdMDT->StationEta;
		    int StationPhi   = nextDig->IdMDT->StationPhi;
		    int Multilayer   = nextDig->IdMDT->Multilayer;
		    int Layer        = nextDig->IdMDT->TubeLayer;
		    int Tube         = nextDig->IdMDT->Tube;
                    
		    //std::cout << "/StationName=" << StationName
                    //          << "/StationEta="  << StationEta
                    //          << "/StationPhi="  << StationPhi
                    //          << "/Multilayer="  << Multilayer
                    //          << "/Layer="       << Layer
                    //          << "/Tube="        << Tube << std::endl;
			      		    
		    uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(StationName,
		                  StationEta,StationPhi,Multilayer,Layer,Tube);
		    
		    uint16_t Lc = nextDig->LeadingCoarseTime;
		    uint16_t Lf = nextDig->LeadingFineTime; 
		    uint16_t Tc = nextDig->TrailingCoarseTime; 
		    uint16_t Tf = nextDig->TrailingFineTime; 
		    uint16_t Wt = nextDig->Adc;
		    
		    LVL2_MUON_CALIBRATION::MdtCalibData mdt(ID,Lc,Lf,Tc,Tf,Wt,phi);
                    MdtFragment << mdt;
                    nextDig = nextDig->ring;
		} while(nextDig!=firstDig);
	    }
        }
    }
    
    LayerDigits (*overlap)[MAXLAYER] = ovlDigits.digi;
    for(int st=0;st<MAXSTATION;st++)
    { 
	for(int ly=0;ly<MAXLAYER;ly++)
	{ 
	    DigitData* firstDig = overlap[st][ly].first;
	    DigitData *nextDig = 0;
            if(firstDig)
	    {
	        nextDig = firstDig;
	        do {
		
		    int StationName  = nextDig->IdMDT->name;
		    int StationEta   = nextDig->IdMDT->StationEta;
		    int StationPhi   = nextDig->IdMDT->StationPhi;
		    int Multilayer   = nextDig->IdMDT->Multilayer;
		    int Layer        = nextDig->IdMDT->TubeLayer;
		    int Tube         = nextDig->IdMDT->Tube;
			      		    
		    uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(StationName,
		                  StationEta,StationPhi,Multilayer,Layer,Tube);
		    
		    uint16_t Lc = nextDig->LeadingCoarseTime;
		    uint16_t Lf = nextDig->LeadingFineTime; 
		    uint16_t Tc = nextDig->TrailingCoarseTime; 
		    uint16_t Tf = nextDig->TrailingFineTime; 
		    uint16_t Wt = nextDig->Adc;
		    
		    LVL2_MUON_CALIBRATION::MdtCalibData mdt(ID,Lc,Lf,Tc,Tf,Wt,phi);
                    MdtFragment << mdt;
                    nextDig = nextDig->ring;
		} while(nextDig!=firstDig);
	    }
        }
    }
    
    
    std::list<LVL2_MUON_CALIBRATION::MdtCalibData> MDThit = MdtFragment.data();
    /*
    if(MDThit.size()==0) {
        uint32_t MdtID = LVL2CALIBRATION::MdtIdIntoWord(12,2,1,1,2,3);
	LVL2CALIBRATION::MdtCalibData mdt(MdtID,20,10,1,2,35,2.3);
	for (int i=0;i<35;++i) MdtFragment << mdt;
    } 
    */
    
    if(pad) {
        uint16_t sector = pad->sector();
	uint16_t sysId  = (sector<32)? 0x66 : 0x65;
	uint16_t secId  = sector%32;
        uint16_t padId  = pad->onlineId();
	uint16_t status = pad->status();
	uint16_t error  = pad->errorCode();
	
	LVL2_MUON_CALIBRATION::RpcCalibFragment RpcFragment(sysId,secId,padId,status,error);
	RpcPad::const_iterator it3 = pad->begin(); 
        RpcPad::const_iterator it4 = pad->end();
        for (; it3!=it4; ++it3) 
        {
            const RpcCoinMatrix * cma = (*it3);

            uint16_t cmaId  = cma->onlineId();
            uint16_t fel1Id = cma->fel1Id();
	    uint16_t febcId = cma->febcId();
	    uint16_t crc    = cma->crc();
	    
	    LVL2_MUON_CALIBRATION::RpcCalibData matrix(cmaId,fel1Id,febcId,crc);
            
	    //for each cma for over fired channels
            RpcCoinMatrix::const_iterator it5 = (*it3)->begin(); 
            RpcCoinMatrix::const_iterator it6 = (*it3)->end();
            for (; it5!=it6; ++it5) 
            {
	        const RpcFiredChannel * fChannel = (*it5);

                uint16_t bcid    = fChannel->bcid();
		uint16_t time    = fChannel->time();
                uint16_t ijk     = fChannel->ijk();
                uint16_t channel = fChannel->channel();
		uint16_t ovl     = fChannel->ovl();
		uint16_t thr     = fChannel->thr();
		
		if(ijk!=7) matrix.getHit(bcid,time,ijk,channel);
		else       matrix.getHit(bcid,time,ijk,ovl,thr);
            }
	    RpcFragment << matrix;
	}
	event << RpcFragment;
    }
    
    if(tgc_rod_hit.size()!=0) 
    {    
	LVL2_MUON_CALIBRATION::TgcCalibFragment TgcFragment(system,subsystem,rod_id,roi_number);
	LVL2_MUON_CALIBRATION::TgcCalibData tgc_payload;
	
	std::vector<tgcLUTDigit>::const_iterator it;
	for(it=tgc_rod_hit.begin();it!=tgc_rod_hit.end();++it)
	{
	    if( (*it).inRoad ) 
	    {
	        LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTHIT roh;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRIPLETSTRIP tps;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRACKLET rot;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_HIPT hpt;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_SL sl;
    
                TgcRdo2ByteStream((*it).rawData,msg(),roh,tps,rot,hpt,sl);
		tgc_payload.getHit(roh,tps,rot,hpt,sl);
	    }
	    
	    if( (*it).isCoincidence ) 
	    {
	        LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTHIT roh;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRIPLETSTRIP tps;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRACKLET rot;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_HIPT hpt;
                LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_SL sl;
    
                TgcRdo2ByteStream((*it).rawData,msg(),roh,tps,rot,hpt,sl);
		tgc_payload.getHit(roh,tps,rot,hpt,sl);
	    }
        }
	
	TgcFragment << tgc_payload;
	event << TgcFragment;
     }
    
    event << MdtFragment;

    if(cscRobs.size() != 0 ) {
        LVL2_MUON_CALIBRATION::CscCalibFragment CscFragment;
        std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator it;
        for (it=cscRobs.begin();it!=cscRobs.end();it++) {
	    
	    LVL2_MUON_CALIBRATION::CscCalibData CscData;
	    uint32_t rod_words     = (**it).rod_fragment_size_word(); 
	    
	    OFFLINE_FRAGMENTS_NAMESPACE::PointerType wr;
	    (*it)->rod_start(wr);

	    for (uint32_t i=0;i<rod_words;++i) {
	       // CID 22907: CAST_TO_QUALIFIED_TYPE
	       // CscData << static_cast<const uint32_t>(*(wr+i));
	       CscData << static_cast<uint32_t>(*(wr+i));
	    }
	   
	    CscFragment << CscData;
	}
	event << CscFragment;
    }

    if(m_CalibrationStream) {
        uint16_t size = event.size();
	uint8_t* buff = new uint8_t[size];
	event.dumpWords(buff,size);
	m_CalibrationStream->write( (char*) buff, event.size() );
	// CID 22892: DELETE_ARRAY
	// delete buff;
	delete [] buff;
    }
    
    // use exactly the same code to output the data into the file
    //*m_CalibrationStream << 
    //              dynamic_cast<LVL2CALIBRATION::CalibData*>(&event);

    if(m_cid != -1) {
        uint16_t size = event.size();
	m_calib_event_size = size;
        char* p;
        if ((p = CircReserve (m_cid, m_calib_event, event.size())) != (char *) -1) {
	    uint8_t* buff = reinterpret_cast<uint8_t*>(p);
	    event.dumpWords(buff,size);
	    CircValidate (m_cid, m_calib_event, p, event.size() );
	    m_calib_event++;
	    m_buffer_free_count = 1;
	} else m_buffer_free_count = 0;
    }
}


void
muFast::fillNtp(const unsigned int error, std::vector<uint32_t>* robIds,
		MuData* data, LVL1emu* lvl1_emu, const DigiPointer reco_digits,
		MuonFex* muon_fex, MuonFeature* muon_feature)
{      
  // ************************************************************************
  // ******************** RETRIEVING MONTECARLO THRUTH **********************
  // ************************************************************************

  float muEnergy = 0.;
  float muPt  = 0.;
  float muPhi = 0.;
  float muEta = 0.;

  if(m_lutntp || m_FEXntp )
  {
      // retrieving the Montecarlo thru from the Storegate
      const DataHandle<McEventCollection> mcCollptr;
      //std::string	key = "GEN_EVENT";
      std::string	key = m_key_for_truth;

      StatusCode cont = m_pStoreGate->contains<McEventCollection>(key);
      StatusCode retr = m_pStoreGate->retrieve(mcCollptr,key);
      if ( cont.isSuccess() && retr.isSuccess() )
      {    
          McEventCollection::const_iterator itr;
          for(itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) 
          {  
              HepMC::GenEvent::particle_const_iterator Part;
	  
	      for (Part = (*itr)->particles_begin(); 
                   Part!=(*itr)->particles_end(); 
                   ++Part ) 
              {
                  // store MCthru only for muon particles
	          if ( ((*Part)->pdg_id() == -13 || (*Part)->pdg_id() == 13) ) 
                  {

                      muEnergy = (*Part)->momentum().e();
                      muPhi = (*Part)->momentum().phi();
		      muPt = (*Part)->momentum().perp();
		      float theta = atan(muPt/fabs((*Part)->momentum().z())); 
		      float eta = -log (tan(theta/2.) );
                      muEta = ((*Part)->momentum().z() >= 0.)? eta : -eta;
	          }
              }
          }
      }
      else {
          msg() << MSG::WARNING << "Could not retrieve McEventCollection" 
	        << endreq;
      }
  }
  
  
  // ************************************************************************
  // ************************ ROI VARIABLES OUTPUT **************************
  // ************************************************************************
 
 enum TriggerType {NONE,BARREL,ENDCAP};
 TriggerType trigger_type = NONE;  
 if(m_MONntp) {
      m_event_number  = m_current_lvl1_id;
      m_roi_id_steer  = m_currentRoIId;
      m_roi_system    = (m_pMuonRoI)? m_pMuonRoI->sysID()    : 0;
      m_roi_subsystem = (m_pMuonRoI)? m_pMuonRoI->subsysID() : 0;
      m_roi_sector    = (m_pMuonRoI)? m_pMuonRoI->sectorID() : 0;
      m_roi_number    = (m_pMuonRoI)? m_pMuonRoI->getRoINumber()       : 0;
      m_roi_threshold = (m_pMuonRoI)? m_pMuonRoI->getThresholdNumber() : 0;
      m_roi_eta       = (m_pMuonRoI)? m_pMuonRoI->eta() : 0;
      m_roi_phi       = (m_pMuonRoI)? m_pMuonRoI->phi() : 0;
      trigger_type    = (m_pMuonRoI && m_pMuonRoI->sysID()==0)?  BARREL : ENDCAP; 
      
      if(robIds != 0) 
      {
	  m_rob_count = ( (int)robIds->size()<MAX_ROB)? robIds->size() : MAX_ROB;
	  for(int i=0; i<m_rob_count; ++i) m_rob_id[i] = (*robIds)[i];
      } else m_rob_count = 0;
      
      if(data != 0) 
      {
          MuData::CSMlist csm = data->mdt_csm();
	  MuData::CSMlist::const_iterator it = csm.begin();
	  m_csm_count = ( (int)csm.size()<MAX_CSM)? csm.size() : MAX_CSM;
	  
	  for(int i=0; i<m_csm_count; ++i, ++it) 
	  {
	      m_csm_id[i]   = (*it)->identifyHash();
	      m_csm_size[i] = (*it)->size();
          }
      } else m_csm_count = 0;
      
      if( m_rob_removed.size() != 0) {
          m_removed_rob_count = ( (int)m_rob_removed.size()<MAX_ROB)? m_rob_removed.size() : MAX_ROB;
          std::set<uint32_t>::const_iterator it = m_rob_removed.begin();
	  int i = 0;
	  while(it!=m_rob_removed.end() && i<m_removed_rob_count ) {
	      m_removed_rob_id[i] = *it;
	      ++i;
	  }
      }
      
      if( m_mdt_unavailable.size() != 0) {
          m_removed_csm_count = ( (int)m_mdt_unavailable.size()<MAX_CSM)? m_mdt_unavailable.size() : MAX_CSM;
          std::set<unsigned int>::const_iterator it = m_mdt_unavailable.begin();
	  int i = 0;
	  while(it!=m_mdt_unavailable.end() && i<m_removed_csm_count ) {
	      m_removed_csm_id[i] = *it;
	      ++i;
	  }
      }
      
      
  }
  
  // ************************************************************************
  // ************************* LUT NTUPLE OUTPUT ****************************
  // ************************************************************************

  // CID 26430: FORWARD_NULL
  // if(m_LUTntp && !m_pMuonRoI->sysID() && muon_fex)
  if(m_LUTntp && m_pMuonRoI && !m_pMuonRoI->sysID() && muon_fex)
  {   
      StatusCode sc;
      const MuFeature& feature = muon_fex->feature();    
      
      m_lut_lvl1Id   = m_current_lvl1_id;
      m_lut_roiType  = m_pMuonRoI->getThresholdNumber();
      m_lut_address  = feature.Saddress;
      m_lut_muEnergy = muEnergy;
      m_lut_muPt     = muPt;
      m_lut_muEta    = muEta;
      m_lut_muPhi    = muPhi;
      m_lut_etaMap   = feature.EtaMap;
      m_lut_phiMap   = feature.PhiMap;
      m_lut_sagitta  = feature.Sagitta;
      m_lut_radius   = feature.Radius;

      sc = ntupleSvc()->writeRecord("/NTUPLES/FILE1/MUFAST/11");
      if(sc.isFailure()) {
          msg() << MSG::WARNING << "Could not write tree for MuFast LUT ntuple" 
	        << endreq;
      }
  }  
  

  // ************************************************************************
  // ************************* MON NTUPLE OUTPUT ****************************
  // ************************************************************************ 
  
  if(m_MONntp )
  {
      StatusCode sc;
            
      const TriggerData* LVL1trig_data = (lvl1_emu)? &lvl1_emu->trigger_data() :
                                                     0;
      const LUTDigitVec* LVL1digits    = (lvl1_emu)? &lvl1_emu->tgc_digits() :
                                                     0;
      const RpcTrigPatt* rpc_patterns  = (lvl1_emu)? &lvl1_emu->rpc_patterns() :
                                                     0;
      const SpointData* super_points = (muon_fex)? muon_fex->super_points() :
      					0;
      const MuFeature* feature = (muon_fex)? &muon_fex->feature() : 0; 
      
      const RpcPad* pad_data = (data)? data->rpc_pad() : 0;
      
      int algo_id = AlgoMap(name());
      
      m_error_code = error | m_data_access_error;
      
      m_tgc_Mid1_eta    =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid1[0]    :99999.;
      m_tgc_Mid1_phi    =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid1[1]    :99999.;
      m_tgc_Mid1_r      =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid1[2]    :99999.;
      m_tgc_Mid1_z      =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid1[3]    :99999.;
      m_tgc_Mid2_eta    =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid2[0]    :99999.;
      m_tgc_Mid2_phi    =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid2[1]    :99999.;
      m_tgc_Mid2_r      =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid2[2]    :99999.;
      m_tgc_Mid2_z      =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMid2[3]    :99999.;
      m_tgc_Mid_rho_N_in=(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMidRhoNin  :0;
      m_tgc_Mid_rho_chi2=(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMidRhoChi2 :99999.;
      m_tgc_Mid_rho_N   =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMidRhoN    :0;
      m_tgc_Mid_phi_N_in=(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMidPhiNin  :0;
      m_tgc_Mid_phi_chi2=(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMidPhiChi2 :99999.;
      m_tgc_Mid_phi_N   =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcMidPhiN    :0;
      m_tgc_Inn_eta     =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInn[0]     :99999.;
      m_tgc_Inn_phi     =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInn[1]     :99999.;
      m_tgc_Inn_r       =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInn[2]     :99999.;
      m_tgc_Inn_z       =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInn[3]     :99999.;
      m_tgc_Inn_rho_std =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInnRhoStd  :99999.;
      m_tgc_Inn_rho_N_in=(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInnRhoNin  :0;
      m_tgc_Inn_rho_N   =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInnRhoN    :0;
      m_tgc_Inn_phi_std =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInnPhiStd  :99999.;
      m_tgc_Inn_phi_N_in=(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInnPhiNin  :0;
      m_tgc_Inn_phi_N   =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcInnPhiN    :0;
      m_tgc_PT          =(LVL1trig_data&&trigger_type==ENDCAP)? LVL1trig_data->tgcPT         :99999.;

        // Hit data
        for (int i = 0; i < MAX_TGC_HITS; i++)
        {
            m_tgc_Inn_rho_hit_phi   [i] = 99999.0;
            m_tgc_Inn_rho_hit_r     [i] = 99999.0;
            m_tgc_Inn_rho_hit_z     [i] = 99999.0;
            m_tgc_Inn_rho_hit_width [i] = 99999.0;
            m_tgc_Inn_rho_hit_in_seg[i] = 99999;
            m_tgc_Inn_phi_hit_phi   [i] = 99999.0;
            m_tgc_Inn_phi_hit_r     [i] = 99999.0;
            m_tgc_Inn_phi_hit_z     [i] = 99999.0;
            m_tgc_Inn_phi_hit_width [i] = 99999.0;
            m_tgc_Inn_phi_hit_in_seg[i] = 99999;
            m_tgc_Mid_rho_hit_phi   [i] = 99999.0;
            m_tgc_Mid_rho_hit_r     [i] = 99999.0;
            m_tgc_Mid_rho_hit_z     [i] = 99999.0;
            m_tgc_Mid_rho_hit_width [i] = 99999.0;
            m_tgc_Mid_rho_hit_in_seg[i] = 99999;
            m_tgc_Mid_phi_hit_phi   [i] = 99999.0;
            m_tgc_Mid_phi_hit_r     [i] = 99999.0;
            m_tgc_Mid_phi_hit_z     [i] = 99999.0;
            m_tgc_Mid_phi_hit_width [i] = 99999.0;
            m_tgc_Mid_phi_hit_in_seg[i] = 99999;
        }
        if (LVL1digits != NULL && !LVL1digits->empty())
        {
            int iInnRho = 0, iInnPhi = 0, iMidRho = 0, iMidPhi = 0;
            for (LUTDigitVec::const_iterator itDigit = LVL1digits->begin(); itDigit != LVL1digits->end(); itDigit++)
            {
                const tgcLUTDigit& digit = *itDigit;
                if (!digit.inRoad) continue;
                if (digit.isStrip && digit.sta == 3 && iInnPhi < m_tgc_Inn_phi_N_in)
                {
                    if (m_tgc_Inn_phi_N_in == 0 || iInnPhi >= m_tgc_Inn_phi_N_in)
                    {
                        msg() << MSG::WARNING << "Monitoring NTuple: iInnPhi=" << iInnPhi
                            << " but m_tgc_Inn_phi_N_in=" << m_tgc_Inn_phi_N_in << endreq;
                        continue;
                    }
		    
		    if(iInnPhi >= MAX_TGC_HITS) {
		        m_tgc_Inn_phi_N_in = MAX_TGC_HITS;
		        continue; //stop filling when limit is reached
                    }
		    
		    m_tgc_Inn_phi_hit_phi   [iInnPhi] = digit.phi;
                    m_tgc_Inn_phi_hit_r     [iInnPhi] = digit.r;
                    m_tgc_Inn_phi_hit_z     [iInnPhi] = digit.z;
                    m_tgc_Inn_phi_hit_width [iInnPhi] = digit.width;
                    m_tgc_Inn_phi_hit_in_seg[iInnPhi] = digit.inSegment ? 1 : 0;
                    iInnPhi++;
                }
                else if (!digit.isStrip && digit.sta == 3 && iInnRho < m_tgc_Inn_rho_N_in)
                {
                    if (m_tgc_Inn_rho_N_in == 0 || iInnRho >= m_tgc_Inn_rho_N_in)
                    {
                        msg() << MSG::WARNING << "Monitoring NTuple: iInnRho=" << iInnRho
                            << " but m_tgc_Inn_rho_N_in=" << m_tgc_Inn_rho_N_in << endreq;
                        continue;
                    }
		    
		    if(iInnRho >= MAX_TGC_HITS) {
		        m_tgc_Inn_rho_N_in = MAX_TGC_HITS;
		        continue; //stop filling when limit is reached
		    }
		     
		    m_tgc_Inn_rho_hit_phi   [iInnRho] = digit.phi;
                    m_tgc_Inn_rho_hit_r     [iInnRho] = digit.r;
                    m_tgc_Inn_rho_hit_z     [iInnRho] = digit.z;
                    m_tgc_Inn_rho_hit_width [iInnRho] = digit.width;
                    m_tgc_Inn_rho_hit_in_seg[iInnRho] = digit.inSegment ? 1 : 0;
                    iInnRho++;
                }
                if (digit.isStrip && digit.sta < 3 && iMidPhi < m_tgc_Mid_phi_N_in)
                {
                    if (m_tgc_Mid_phi_N_in == 0 || iMidPhi >= m_tgc_Mid_phi_N_in)
                    {
                        msg() << MSG::WARNING << "Monitoring NTuple: iMidPhi=" << iMidPhi
                            << " but m_tgc_Mid_phi_N_in=" << m_tgc_Mid_phi_N_in << endreq;
                        continue;
                    }
		    
		    if(iMidPhi >= MAX_TGC_HITS) {
		        m_tgc_Mid_phi_N_in = MAX_TGC_HITS;
		        continue; //stop filling when limit is reached
                    } 
		    m_tgc_Mid_phi_hit_phi   [iMidPhi] = digit.phi;
                    m_tgc_Mid_phi_hit_r     [iMidPhi] = digit.r;
                    m_tgc_Mid_phi_hit_z     [iMidPhi] = digit.z;
                    m_tgc_Mid_phi_hit_width [iMidPhi] = digit.width;
                    m_tgc_Mid_phi_hit_in_seg[iMidPhi] = digit.inSegment ? 1 : 0;
                    iMidPhi++;
                }
                else if (!digit.isStrip && digit.sta < 3 && iMidRho < m_tgc_Mid_rho_N_in)
                {
                    if (m_tgc_Mid_rho_N_in == 0 || iMidRho >= m_tgc_Mid_rho_N_in)
                    {
                        msg() << MSG::WARNING << "Monitoring NTuple: iMidRho=" << iMidRho
                            << " but m_tgc_Mid_rho_N_in=" << m_tgc_Mid_rho_N_in << endreq;
                        continue;
                    }
		    
		    if(iMidRho >= MAX_TGC_HITS) {
		        m_tgc_Mid_rho_N_in = MAX_TGC_HITS;
		        continue; //stop filling when limit is reached
		    } 
                    m_tgc_Mid_rho_hit_phi   [iMidRho] = digit.phi;
                    m_tgc_Mid_rho_hit_r     [iMidRho] = digit.r;
                    m_tgc_Mid_rho_hit_z     [iMidRho] = digit.z;
                    m_tgc_Mid_rho_hit_width [iMidRho] = digit.width;
                    m_tgc_Mid_rho_hit_in_seg[iMidRho] = digit.inSegment ? 1 : 0;
                    iMidRho++;
                }
            }
            if (iInnPhi != m_tgc_Inn_phi_N_in && (iInnPhi != MAX_TGC_HITS) )
                msg() << MSG::WARNING << "Monitoring NTuple: iInnPhi=" << iInnPhi
                    << " but m_tgc_Inn_phi_N_in=" << m_tgc_Inn_phi_N_in << endreq;
            if (iInnRho != m_tgc_Inn_rho_N_in && (iInnRho != MAX_TGC_HITS) )
                msg() << MSG::WARNING << "Monitoring NTuple: iInnRho=" << iInnRho
                    << " but m_tgc_Inn_rho_N_in=" << m_tgc_Inn_rho_N_in << endreq;
            if (iMidPhi != m_tgc_Mid_phi_N_in && (iMidPhi != MAX_TGC_HITS) )
                msg() << MSG::WARNING << "Monitoring NTuple: iMidPhi=" << iMidPhi
                    << " but m_tgc_Mid_phi_N_in=" << m_tgc_Mid_phi_N_in << endreq;
            if (iMidRho != m_tgc_Mid_rho_N_in && (iMidRho != MAX_TGC_HITS) )
                msg() << MSG::WARNING << "Monitoring NTuple: iMidRho=" << iMidRho
                    << " but m_tgc_Mid_rho_N_in=" << m_tgc_Mid_rho_N_in << endreq;
            msg() << MSG::DEBUG << "Monitoring NTuple:"
                << " iInnRho=" << iInnRho << " iInnPhi=" << iInnPhi
                << " iMidRho=" << iMidRho << " iMidPhi=" << iMidPhi << endreq;
        }
      
      m_rpc1_x = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc1[0] : 99999.;
      m_rpc1_y = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc1[1] : 99999.;
      m_rpc1_z = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc1[2] : 99999.;
      m_rpc1_r = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc1[3] : 99999.;
      m_rpc2_x = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc2[0] : 99999.;
      m_rpc2_y = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc2[1] : 99999.;
      m_rpc2_z = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc2[2] : 99999.;
      m_rpc2_r = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc2[3] : 99999.;
      m_rpc3_x = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc3[0] : 99999.;
      m_rpc3_y = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc3[1] : 99999.;
      m_rpc3_z = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc3[2] : 99999.;
      m_rpc3_r = (LVL1trig_data&&trigger_type==BARREL)? LVL1trig_data->rpc3[3] : 99999.;
     
      m_eta_pivot_lay0 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_pivot_lay0  : 0;
      m_eta_pivot_lay1 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_pivot_lay1  : 0;
      m_eta_low_0_lay0 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_0_lay0  : 0;
      m_eta_low_1_lay0 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_1_lay0  : 0;
      m_eta_low_0_lay1 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_0_lay1  : 0;
      m_eta_low_1_lay1 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_1_lay1  : 0;
      m_eta_high_0_lay0=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_0_lay0 : 0;
      m_eta_high_1_lay0=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_1_lay0 : 0;
      m_eta_high_0_lay1=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_0_lay1 : 0;
      m_eta_high_1_lay1=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_1_lay1 : 0;
      
      m_phi_pivot_lay0 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_pivot_lay0  : 0;
      m_phi_pivot_lay1 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_pivot_lay1  : 0;
      m_phi_low_0_lay0 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_0_lay0  : 0;
      m_phi_low_1_lay0 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_1_lay0  : 0;
      m_phi_low_0_lay1 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_0_lay1  : 0;
      m_phi_low_1_lay1 =(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_low_1_lay1  : 0;
      m_phi_high_0_lay0=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_0_lay0 : 0;
      m_phi_high_1_lay0=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_1_lay0 : 0;
      m_phi_high_0_lay1=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_0_lay1 : 0;
      m_phi_high_1_lay1=(rpc_patterns&&trigger_type==BARREL)? rpc_patterns->eta_high_1_lay1 : 0;
	
      
      m_pad_hit_onlineId[0]= 0xffffffff;
      m_pad_hit_code[0]    = 0xffffffff;
      m_pad_hit_x[0]       = 99999.;
      m_pad_hit_y[0]       = 99999.;
      m_pad_hit_z[0]       = 99999.;
      m_pad_hit_r[0]       = 99999.;
      m_pad_hit_p[0]       = 99999.;
      
      uint32_t pad_subSys      = 0xf;
      uint32_t pad_sector      = 0xffff;
      uint32_t pad_id          = 0xffff;
      uint32_t pad_hit_bcid    = 0xf;
      uint32_t pad_hit_time    = 0xf;
      uint32_t pad_hit_cmaid   = 0xf;
      uint32_t pad_hit_ijk     = 0xf;
      uint32_t pad_hit_channel = 0xff;

      m_pad_hit_data = 0;
      
      if(pad_data) {
	uint16_t Sector = pad_data->sector();
	uint16_t PADId  = pad_data->onlineId();
	uint16_t SecId  = Sector%32;
        uint16_t SubId  = (Sector < 32)? 0 : 1;
	
	pad_sector   = Sector;
        pad_subSys   = (Sector<32)? 0x0 : 0x1;
        pad_id       = PADId;
	
	
	RpcPad::const_iterator it3 = pad_data->begin(); 
        RpcPad::const_iterator it4 = pad_data->end();
        for (; it3!=it4; ++it3) {
	
	  if(m_pad_hit_data >= MAX_RPC_HITS) continue;
	  
          const RpcCoinMatrix * cma = (*it3);

	  pad_hit_cmaid  = cma->onlineId();
	  uint16_t CMAId = cma->onlineId();
          uint16_t LH    = (CMAId >> 2) & 1;
	  
	  const CMAparameters* CMA = 
	                m_svc->rpccab()->give_CMA(SubId,SecId,PADId,CMAId);
			
	  std::list <unsigned int> CodeList;
	    
	  //for each cma for over fired channels
          RpcCoinMatrix::const_iterator it5 = (*it3)->begin(); 
          RpcCoinMatrix::const_iterator it6 = (*it3)->end();
          for (; it5!=it6; ++it5) 
          {  
	    if(m_pad_hit_data >= MAX_RPC_HITS) continue;
	    
	    CodeList.clear();
	    const RpcFiredChannel * fChannel = (*it5);
	    
	    pad_hit_bcid    = fChannel->bcid() & 0x7;
            pad_hit_time    = fChannel->time() & 0x7;
            pad_hit_ijk     = fChannel->ijk() & 0x7;
            pad_hit_channel = fChannel->channel() & 0x1f;
	    
	    m_pad_hit_onlineId[m_pad_hit_data] = pad_hit_channel      |
	                                         (pad_hit_ijk << 5)   |
						 (pad_hit_cmaid << 8) |
						 (pad_id << 11)       |
						 (pad_subSys << 15)   |
						 (pad_sector << 16);
	    		
	    CMA->give_strip_code(Sector,LH,fChannel->ijk(),
		                       fChannel->channel(),CodeList);
				       
	    if(CodeList.size()!=0) {
	      std::list <unsigned int>::const_iterator it7 = CodeList.begin();
              m_pad_hit_code[m_pad_hit_data] = *it7;
		 
              if(m_svc->rpcgeo()->existCode(*it7)) {
		float xyz[3];
		float radius;
		float pitch;
		bool  r = true;
		r &= m_svc->rpcgeo()->give_strip_coordinates(*it7, xyz);
                r &= m_svc->rpcgeo()->give_strip_radius(*it7,radius);
		if( (*it7)/100000000) r |= m_svc->rpcgeo()->give_phi_strip_size(*it7,pitch);
                else                  r |= m_svc->rpcgeo()->give_eta_strip_size(*it7,pitch);
                if (r) {
		  m_pad_hit_x[m_pad_hit_data] = xyz[0];
                  m_pad_hit_y[m_pad_hit_data] = xyz[1];
                  m_pad_hit_z[m_pad_hit_data] = xyz[2];
                  m_pad_hit_r[m_pad_hit_data] = radius;
		  m_pad_hit_p[m_pad_hit_data] = pitch;
		} 
              }
              
	      m_pad_hit_data++;
	      
              if(m_pad_hit_data<MAX_RPC_HITS) {
		m_pad_hit_onlineId[m_pad_hit_data]= 0xffffffff;
	        m_pad_hit_code[m_pad_hit_data]    = 0xffffffff;
	        m_pad_hit_x[m_pad_hit_data]       = 99999.;
                m_pad_hit_y[m_pad_hit_data]       = 99999.;
                m_pad_hit_z[m_pad_hit_data]       = 99999.;
                m_pad_hit_r[m_pad_hit_data]       = 99999.;
                m_pad_hit_p[m_pad_hit_data]       = 99999.;
              }
	      
	    }
	  }
	}
      }
    
      m_mdt_hit_tubes = 0;
      if( reco_digits.digi != 0) { 
        LayerDigits (*digits)[MAXLAYER] = reco_digits.digi;
        
	for(int st=0;st<MAXSTATION;st++) {
	  if( m_mdt_hit_tubes >= MAX_MDT_HITS) continue;  
	  
	  for(int ly=0;ly<MAXLAYER;ly++) { 
	    if( m_mdt_hit_tubes >= MAX_MDT_HITS) continue;  
	  
	    DigitData* firstDig = digits[st][ly].first;
            if(firstDig)
	    {
              int StationName  = firstDig->IdMDT->name;
              int StationEta   = firstDig->IdMDT->StationEta;
              int StationPhi   = firstDig->IdMDT->StationPhi;
              int Multilayer   = firstDig->IdMDT->Multilayer;
              int Layer        = firstDig->IdMDT->TubeLayer;
              int Tube         = firstDig->IdMDT->Tube;
                    
              uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(StationName,
                  StationEta,StationPhi,Multilayer,Layer,Tube);
				  
              m_mdt_onlineId     [m_mdt_hit_tubes] = firstDig->OnlineId;
              m_mdt_offlineId    [m_mdt_hit_tubes] = ID;
              m_mdt_tube_r       [m_mdt_hit_tubes] = firstDig->R;
              m_mdt_tube_z       [m_mdt_hit_tubes] = firstDig->Z;
              m_mdt_tube_residual[m_mdt_hit_tubes] = firstDig->Residual;
              m_mdt_tube_time    [m_mdt_hit_tubes] = firstDig->DriftTime;
              m_mdt_tube_space   [m_mdt_hit_tubes] = firstDig->DriftSpace;
              m_mdt_tube_sigma   [m_mdt_hit_tubes] = firstDig->DriftSigma;
              
	      m_mdt_hit_tubes++;
	    }
          }
        }

	for(int st=0;st<MAXSTATION;st++) {
	  if( m_mdt_hit_tubes >= MAX_MDT_HITS) continue;  
	  
	  for(int ly=0;ly<MAXLAYER;ly++) { 
	    if( m_mdt_hit_tubes >= MAX_MDT_HITS) continue;  
	  
	    DigitData* firstDig = digits[st][ly].first;
	    DigitData *nextDig = 0;
            if(firstDig)
	    {
	      nextDig = firstDig->ring;
	      if(nextDig && nextDig != firstDig) {
	        do {
		
		    int StationName  = nextDig->IdMDT->name;
		    int StationEta   = nextDig->IdMDT->StationEta;
		    int StationPhi   = nextDig->IdMDT->StationPhi;
		    int Multilayer   = nextDig->IdMDT->Multilayer;
		    int Layer        = nextDig->IdMDT->TubeLayer;
		    int Tube         = nextDig->IdMDT->Tube;
                    
		    uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(StationName,
		                  StationEta,StationPhi,Multilayer,Layer,Tube);
				  
                    m_mdt_onlineId     [m_mdt_hit_tubes] = nextDig->OnlineId;
                    m_mdt_offlineId    [m_mdt_hit_tubes] = ID;
                    m_mdt_tube_r       [m_mdt_hit_tubes] = nextDig->R;
                    m_mdt_tube_z       [m_mdt_hit_tubes] = nextDig->Z;
                    m_mdt_tube_residual[m_mdt_hit_tubes] = nextDig->Residual;
                    m_mdt_tube_time    [m_mdt_hit_tubes] = nextDig->DriftTime;
		    m_mdt_tube_space   [m_mdt_hit_tubes] = nextDig->DriftSpace;
		    m_mdt_tube_sigma   [m_mdt_hit_tubes] = nextDig->DriftSigma;
		    
                    nextDig = nextDig->ring;
		    m_mdt_hit_tubes++;
		} while(nextDig!=firstDig && m_mdt_hit_tubes < MAX_MDT_HITS);
	      }
            }
	  }
        }

      }
    
  
    
      m_mdt_Inner_slope      = 99999.;
      m_mdt_Inner_intercept  = 99999.;
      m_mdt_Inner_Z          = 99999.;
      m_mdt_Inner_R          = 99999.;
      m_mdt_Inner_fit_chi    = 99999.;
      m_mdt_Middle_slope     = 99999.;
      m_mdt_Middle_intercept = 99999.;
      m_mdt_Middle_Z         = 99999.;
      m_mdt_Middle_R         = 99999.;
      m_mdt_Middle_fit_chi   = 99999.;
      m_mdt_Outer_slope      = 99999.;
      m_mdt_Outer_intercept  = 99999.;
      m_mdt_Outer_Z          = 99999.;
      m_mdt_Outer_R          = 99999.;
      m_mdt_Outer_fit_chi    = 99999.;
      
      if(super_points) {
          for (int st=0;st<MAXSTATION;++st) {
      
              if( super_points[st].Npoint>2 ) {
	          if(st%3==0) { 
	              m_mdt_Inner_slope = super_points[st].Alin;
	              m_mdt_Inner_intercept = super_points[st].R -
		                        m_mdt_Middle_slope*super_points[st].Z;
		      m_mdt_Inner_Z = super_points[st].Z;
		      m_mdt_Inner_R = super_points[st].R;
		      m_mdt_Inner_fit_chi = super_points[st].Chi2;
		  }
	          if(st%3==1) { 
	              m_mdt_Middle_slope = super_points[st].Alin;
	              m_mdt_Middle_intercept = super_points[st].R -
		                        m_mdt_Middle_slope*super_points[st].Z;
		      m_mdt_Middle_Z = super_points[st].Z;
		      m_mdt_Middle_R = super_points[st].R;
		      m_mdt_Middle_fit_chi = super_points[st].Chi2;
		  }
	          if(st%3==2) {
	              m_mdt_Outer_slope = super_points[st].Alin;
                      m_mdt_Outer_intercept = super_points[st].R -
		                        m_mdt_Outer_slope*super_points[st].Z;
                      m_mdt_Outer_Z = super_points[st].Z;
		      m_mdt_Outer_R = super_points[st].R;
		      m_mdt_Outer_fit_chi = super_points[st].Chi2;
	          }
	      }
	  
          }
      }
      
      m_Sagitta      = (feature)? feature->Sagitta          : 99999.;
      m_Radius       = (feature)? feature->Radius           : -99999.;
      m_Slope        = (feature)? feature->slope            : 99999.;
      m_Intercept    = (feature)? feature->intercept        : 99999.;
      m_Alpha        = (feature)? feature->alpha            : 99999.;
      m_Beta         = (feature)? feature->beta             : 99999.;
      m_DeltaR       = (feature)? feature->deltaR           : 99999.;
      m_Speed_over_c = (LVL1trig_data)? LVL1trig_data->beta : 99999.; 
      m_EtaMap       = (feature)? feature->EtaMap           : 99999.;
      m_PhiMap       = (feature)? feature->PhiMap           : 99999.;
      m_Phi          = (feature)? feature->phi              : 99999.;
      m_PhiDir       = (feature)? feature->dir_phi          : 99999.;
      m_Pt           = (feature)? feature->Pt               : 99999.;
      m_Charge       = (feature)? feature->Charge           : 99999.;
      
      m_ALGO_id = algo_id;
      m_TE_id   = 0;
      
      int ID = 50 + algo_id;
      std::stringstream tag;
      tag << "/NTUPLES/FILE1/MUFAST/" << ID << std::ends; 
      sc = ntupleSvc()->writeRecord(tag.str().c_str());
      if(sc.isFailure()) {
          msg() << MSG::WARNING << "Could not write tree for MuFast MON ntuple" 
	        << endreq;
      }
  }
 
  
  // ************************************************************************
  // ************************* FEX NTUPLE OUTPUT ****************************
  // ************************************************************************
  
  if(m_FEXntp )
  {
      StatusCode sc;
      
      m_fex_genPt    = muPt;
      m_fex_genEta   = muEta;
      m_fex_genPhi   = muPhi;
      m_fex_address  = (muon_feature)? muon_feature->saddress() : 0;
      m_fex_pt       = (muon_feature)? muon_feature->pt()       : 0;
      m_fex_radius   = (muon_feature)? muon_feature->radius()   : 0;
      m_fex_eta      = (muon_feature)? muon_feature->eta()      : 0;
      m_fex_phi      = (muon_feature)? muon_feature->phi()      : 0;
      m_fex_dir_phi  = (muon_feature)? muon_feature->dir_phi()  : 0;
      m_fex_zeta     = (muon_feature)? muon_feature->zeta()     : 0;
      m_fex_dir_zeta = (muon_feature)? muon_feature->dir_zeta() : 0;
      m_fex_beta     = (muon_feature)? muon_feature->beta()     : 1.;

      sc = ntupleSvc()->writeRecord("/NTUPLES/FILE1/MUFAST/10");
      if(sc.isFailure()) {
          msg() << MSG::WARNING << "Could not write tree for MuFast FEX ntuple" 
	        << endreq;
      }
  }
}

HLT::ErrorCode
muFast::muFastSeed(const unsigned int error, const HLT::TriggerElement* inputTE,
                   HLT::TriggerElement* outputTE,
		   std::vector<uint32_t>* robIds, MuData* data,
		   LVL1emu* lvl1_emu,const DigiPointer reco_digits,
		   MuonFex* muon_fex,MuonFeature* muon_feature)
{
  // CID 26822: REVERSE_INULL
  // muon_feature->set_algoId(MuonFeature::AlgoId(AlgoMap(name())));
  if(muon_feature) muon_feature->set_algoId(MuonFeature::AlgoId(AlgoMap(name())));

  //bool status;
  //StatusCode sc;
  uint32_t code = error|m_data_access_error;
  if(m_details!=0) m_details->setError(code);
  
  int currentRoIId = -1;      
  
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(inputTE, roiDescriptor, "");
  
  if (status != HLT::OK) {
      msg() << MSG::WARNING  << "Could not find the LVL1 roi descriptor." 
            << endreq;
  }

  ///////////////////////////////////////////////////////////////////////////
  if (roiDescriptor) {                                                     //
      currentRoIId = roiDescriptor->roiId();                               //
  } else {                                                                 //
      msg() << MSG::WARNING  << "Could not set the RoI Id" << endreq;      //
  }                                                                        //
  muon_feature->set_roiId(currentRoIId);                                   //
  ///////////////////////////////////////////////////////////////////////////

  if(!muon_feature) {
      msg() << MSG::INFO << "muFast has an empty feature! Set TE to false."
            << endreq; 
      outputTE->setActiveState(false);
  }

  fillNtp(error, robIds, data, lvl1_emu, reco_digits,muon_fex, muon_feature);

  // update the TrigRoiDescriptor ///////////////////////////////////////////
  // CID 26431: FORWARD_NULL                                               //
  // if (muon_feature->pt() != 0.) {                                       //
  if (muon_feature && muon_feature->pt() != 0.) {                          //
                                                                           //
      double EtaVtx = 0.;                                                  //
      double PhiVtx = 0.;                                                  //
      double SigEta = 0.;                                                  //
      double SigPhi = 0.;                                                  //
      if ( (m_backExtrapolator->give_eta_phi_at_vertex (muon_feature,      //
                 EtaVtx,SigEta,PhiVtx,SigPhi,0.)).isFailure() )            //
      {                                                                    //
          msg() << MSG::WARNING << "BackExtrapolator problem: "            //
	        << "Pt of Muon Feature out of BackExtrapolator range."     // 
		<< endreq;                                                 //
	  msg() << MSG::WARNING << "Use Muon Feature position to fill the" //
	        << " TrigRoiDescriptor for IDSCAN." << endreq;             //
          EtaVtx = (muon_fex->feature()).EtaMap;                           //
	  PhiVtx = (muon_fex->feature()).phi;                              //
	  SigEta = 0.1;                                                    //
	  SigPhi = 0.1;                                                    //
      }                                                                    //
                                                                           //
      if (roiDescriptor) {                                                 //

	  bool isStrangeBackExtrapolated = false; 
	  float mf_eta = (muon_fex->feature()).EtaMap;
	  float mf_phi = (muon_fex->feature()).phi;
	  const float PHI_LIMIT       = 3.141592;
	  const float ETA_LIMIT       = 5.0;
	  const float DELTA_ETA_LIMIT = 3.0;
	  bool isEtaStrange = false;
	  bool isPhiStrange = false;
	  if( fabs(EtaVtx) > ETA_LIMIT || fabs(EtaVtx-mf_eta) > DELTA_ETA_LIMIT ) isEtaStrange = true;
	  if( fabs(PhiVtx) > PHI_LIMIT ) isPhiStrange = true;
	  if( isEtaStrange || isPhiStrange ) {
	     isStrangeBackExtrapolated = true;
	     if( isEtaStrange ) msg() << MSG::WARNING << "Strange BackExtrapolated eta value" << endreq;
	     if( isPhiStrange ) msg() << MSG::WARNING << "Strange BackExtrapolated phi value" << endreq;
	     msg() << MSG::WARNING << "   MF: pT/Eta/Phi/Saddress/Radius="
		   << muon_feature->pt() << "/" << muon_feature->eta() << "/" << muon_feature->phi()
		   << "/" << muon_feature->saddress() << "/" << muon_feature->radius() << endreq;
	     msg() << MSG::WARNING << "   Extrapolated: Eta/Phi=" << EtaVtx << "/" << PhiVtx << endreq;
	     msg() << MSG::WARNING << "Use MuonFeature eta/phi to fill the TrigRoiDescriptor for IDSCAN." << endreq;
	     EtaVtx = mf_eta;
	     PhiVtx = mf_phi;
	     m_backextrFailed_eta.push_back( m_pMuonRoI->eta() );
	     m_backextrFailed_phi.push_back( m_pMuonRoI->phi() );
	  }


          // patch for the ID RoI descriptor ////////////////////////////////
	  float phiHalfWidth = 0.1;                                        //
	  float etaHalfWidth = 0.1;                                        //
	  // 2010 runs                                                     //
          // if ( std::fabs(EtaVtx)>1 && std::fabs(EtaVtx)<1.5 ) {         //
	  //    phiHalfWidth = 0.25;                                       //
	  //    etaHalfWidth = 0.4;                                        //
	  // }                                                             //
	  // else {                                                        //
	  //    phiHalfWidth = 0.1;                                        //
	  //    etaHalfWidth = 0.15;                                       //
	  // }                                                             //
	  // 2011a tuning
	  if( ! isStrangeBackExtrapolated ) {
	     phiHalfWidth = getRoiSizeForID(false,muon_feature);
	     etaHalfWidth = getRoiSizeForID(true, muon_feature);
	  }
	  else {
	     phiHalfWidth = 0.25;
	     etaHalfWidth = 0.40;
	  }


	  /// calculate RoI descriptor sizes - are these correct for the MSroi?  
     
	  double _eta    = (muon_fex->feature()).EtaMap;
	  double _etaMin = (muon_fex->feature()).EtaMap - etaHalfWidth;
	  double _etaMax = (muon_fex->feature()).EtaMap + etaHalfWidth;

	  double _phi    = (muon_fex->feature()).phi;
	  double _phiMin = HLT::wrapPhi((muon_fex->feature()).phi - phiHalfWidth); 
	  double _phiMax = HLT::wrapPhi((muon_fex->feature()).phi + phiHalfWidth); 
      
	  TrigRoiDescriptor* MSroiDescriptor =                             //
	           new TrigRoiDescriptor(roiDescriptor->l1Id(),            //
                                         roiDescriptor->roiId(),           //
					 _eta, _etaMin, _etaMax,           // 
					 _phi, _phiMin, _phiMax );         //
				         


	  double EtaVtxMin = (EtaVtx - etaHalfWidth);
	  double EtaVtxMax = (EtaVtx + etaHalfWidth);

	  double PhiVtxMin = HLT::wrapPhi(PhiVtx - phiHalfWidth); 
	  double PhiVtxMax = HLT::wrapPhi(PhiVtx + phiHalfWidth); 
	                                                                   //
	  TrigRoiDescriptor* IDroiDescriptor =                             //
	           new TrigRoiDescriptor(roiDescriptor->l1Id(),            //
                                         roiDescriptor->roiId(),           //
                                         EtaVtx, EtaVtxMin, EtaVtxMax,     //
					 PhiVtx, PhiVtxMin, PhiVtxMax );   //
                                                                           //
          ///////////////////////////////////////////////////////////////////
                                                                           //
          HLT::ErrorCode attached;                                         //
          // attach roi descriptor for TrigMoore                           //
          attached = attachFeature(outputTE,MSroiDescriptor, "forMS");     //
          if ( attached!=HLT::OK) {                                        //
	      msg() << MSG::WARNING  << "Could not attach the "            //
	            <<"roi descriptor for TrigMoore." << endreq;           //
	  } else {                                                         //
	      if (msgLvl() <= MSG::DEBUG){                                 //
	        msg() << MSG::DEBUG  << "Recorded an RoiDescriptor for "   //
		      << "TrigMoore: phi=" << MSroiDescriptor->phi()      //
		      << ",  eta="         << MSroiDescriptor->eta()      //
		      << endreq;                                           //
              }                                                            //
	  }                                                                //
	                                                                   //
	  // attach roi descriptor for ID                                  //
          attached = attachFeature(outputTE,IDroiDescriptor, "forID");     //
          if ( attached!=HLT::OK) {                                        //
	      msg() << MSG::WARNING  << "Could not attach the "            //
	            <<"roi descriptor for Inner Detector." << endreq;      //
	  } else {                                                         //
	      if (msgLvl() <= MSG::DEBUG)                                  //
	        msg() << MSG::DEBUG  << "Recorded an RoiDescriptor for "   //
		     << "Inner Detector: phi=" << IDroiDescriptor->phi()  //
		     << ",  eta="         << IDroiDescriptor->eta()       //
		     << endreq;                                            //
	  }                                                                //
                                                                           //
      }                                                                    //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////

  
  
  
  // seeding the next Algorithm /////////////////////////////////////////////  
  std::string Key = "";                                                    //
  status = attachFeature( outputTE, muon_feature);                         //
  if(status!=HLT::OK){                                                     //
      msg() << MSG::ERROR                                                  //
           << "Record of Muon Feature in TriggerElement failed" << endreq; //
      return status;                                                       //
                                                                           //
  } else {                                                                 //
      if(msgLvl() <= MSG::DEBUG)                                           //
          msg() << MSG::DEBUG                                              //
                << "Muon Feature attached to the TriggerElement"           //
                << endreq;                                                 //
	                                                                   //
  }                                                                        //
                                                                           //
                                                                           //
  outputTE->setActiveState(true);                                          //
  ///////////////////////////////////////////////////////////////////////////
  
  if(m_details) {
  // attaching ESD object ///////////////////////////////////////////////////
    status = attachFeature( outputTE, m_details);                          //
    if(status!=HLT::OK){                                                   //
        msg() << MSG::ERROR                                                //
              << "Record of Muon Feature Details failed" << endreq;        //
        return status;                                                     //
                                                                           //
    } else {                                                               //
        if(msgLvl() <= MSG::DEBUG) {                                       //
          msg() << MSG::DEBUG                                              //
                << "Muon Feature Details attached to the TriggerElement"   //
                << endreq;                                                 //
		                                                           //
          if (m_Esd_info) msg() << MSG::INFO << *m_details << endreq;      //
        }                                                                  //
	                                                                   //
    }                                                                      //
  ///////////////////////////////////////////////////////////////////////////
  }
  return HLT::OK;
}

bool
muFast::RpcRoIDim(unsigned short int subsystem,unsigned short int sector,
                  unsigned short int threshold,unsigned short int roi,
		  float& EtaMin, float& EtaMax)
{

  unsigned int EtaLowBorder = 0;
  unsigned int EtaHighBorder = 0;
  unsigned int PhiLowBorder = 0;
  unsigned int PhiHighBorder = 0;

  if(threshold > 3)
  {
      if(!m_svc->rpccab()->give_HighPt_borders(subsystem,sector,roi,
					       EtaLowBorder,EtaHighBorder,
					       PhiLowBorder,PhiHighBorder))
      {
          msg() << MSG::DEBUG
          << "Cannot retrieve the RoI border code of the RPC strips "
          << endreq;
	  return false;
      }
  }
  else
  {
       if(!m_svc->rpccab()->give_LowPt_borders(subsystem,sector,roi,
					       EtaLowBorder,EtaHighBorder,
					       PhiLowBorder,PhiHighBorder))
      {
          msg() << MSG::DEBUG
          << "Cannot retrieve the RoI border code of the RPC strips "
          << endreq;
	  return false;
      }
  }


  float firstEta[3] = {0.,0.,0.};
  float lastEta[3]  = {0.,0.,0.};
  //float firstPhi[3] = {0.,0.,0.};
  //float lastPhi[3]  = {0.,0.,0.};

  float RadMin = 0.;
  float RadMax = 0.;

  while (EtaLowBorder%100 && !m_svc->rpcgeo()->existCode(EtaLowBorder)) {EtaLowBorder += 1;}
  while (EtaHighBorder%100 && !m_svc->rpcgeo()->existCode(EtaHighBorder)) {EtaHighBorder -= 1;}

  if(!m_svc->rpcgeo()->give_strip_coordinates(EtaLowBorder,firstEta))
  {
      msg() << MSG::DEBUG
            << "Cannot retrieve the coordinates "
	    << "for EtaLowBorder=" << EtaLowBorder << endreq;
      msg() << MSG::DEBUG << firstEta[0] << " " << firstEta[1]
	    << " " << firstEta[2] << endreq;
      return false;
  }

  if(!m_svc->rpcgeo()->give_strip_coordinates(EtaHighBorder,lastEta))
  {
      msg() << MSG::DEBUG
            << "Cannot retrieve the coordinates "
	    << "for EtaHighBorder=" << EtaHighBorder << endreq;
      return false;
  }

  if(!m_svc->rpcgeo()->give_strip_radius(EtaLowBorder,RadMin))
  {
      msg() << MSG::DEBUG
            << "Cannot retrieve the gas Layer Radius "
	    << "for EtaLowBorder=" << EtaLowBorder << endreq;
      return false;
  }

  if(!m_svc->rpcgeo()->give_strip_radius(EtaHighBorder,RadMax))
  {
      msg() << MSG::DEBUG
            << "Cannot retrieve the gas Layer Radius "
	    << "for EtaHighBorder=" << EtaHighBorder << endreq;
      return false;
  }

  float Zmin = firstEta[2];
  float ThMin  = (Zmin)? atan(RadMin/fabsf(Zmin)): asin(1.);
  EtaMin = (Zmin>0.)?  -log(tan(ThMin/2.)) : log(tan(ThMin/2.));

  float Zmax = lastEta[2];
  float ThMax  = (Zmax)? atan(RadMax/fabsf(Zmax)): asin(1.);
  EtaMax = (Zmax>0.)?  -log(tan(ThMax/2.)) : log(tan(ThMax/2.));

  if (EtaMin > EtaMax)
  {
      float tmp = EtaMin;
      EtaMin = EtaMax;
      EtaMax = tmp;
  }
  
  return true;
}

void
muFast::checkForOpening(std::string filename) {

    struct stat stFileInfo;
    int intStat;
    
    //try to get the filname attribute
    intStat = stat(filename.c_str(),&stFileInfo);
    
    // if file exist then remove it
    if(intStat == 0) {
        remove(filename.c_str());
    }
}

bool
muFast::checkForClosing(std::string filename) {

    struct stat stFileInfo;
    int intStat;
    
    //try to get the filname attribute
    intStat = stat(filename.c_str(),&stFileInfo);
    
    // if file exist then remove it
    if(intStat == 0) {
        return true;
    }
    
    return false;
}

void 
muFast::TGCRoI2etaphi(unsigned short int systemID, unsigned short int subsystemID, 
                      unsigned short int sectorID, unsigned short int RoInumber, 
                      short int& eta, short int& phi)
{
  //systemID     1:endcap 2:forward
  //subsystemID  1:eta>0 0:eta<0
  //sectorID     [0:47]
  //RoInumber    [0:147]:endcap [0:64]:forwad

  //eta [0:52]:A [-53:-1]:C
  //phi [0:191]:endcap [0,2,4,..,190]:forward

  int localeta=RoInumber/4;

  if(systemID==2){//forward

    int localphi=RoInumber%4;
    //eta=roi/4;
    eta=localeta+36;
    phi=(localphi+sectorID*4)*2;

  }else if(systemID==1){//endcap

    int sector=(sectorID+1)/4+1;
    phi=sectorID*4 + ( sector%2==0 ? RoInumber%4 : 3-(RoInumber%4)   );
    eta=localeta;

  }

  if(subsystemID==0){
    eta+=1;
    eta*=-1;
  }

  return;

}

float muFast::getRoiSizeForID(bool isEta, MuonFeature* muon_feature)
{
   bool  isBarrel = (muon_feature->saddress()==-1) ? true : false;
   float eta = muon_feature->eta();
   float phi = muon_feature->phi();
   float pt  = muon_feature->pt();

   //
   const int N_PARAMS = 2;

   //
   const float etaMinWin_brl = 0.10;
   const float etaMinWin_ec1 = 0.10;
   const float etaMinWin_ec2 = 0.10;
   const float etaMinWin_ec3 = 0.10;
   const float etaMinWin_ecA = 0.10;
   const float etaMinWin_ecB = 0.10;

   const float etaMaxWin_brl = 0.20;
   const float etaMaxWin_ec1 = 0.20;
   const float etaMaxWin_ec2 = 0.20;
   const float etaMaxWin_ec3 = 0.20;
   const float etaMaxWin_ecA = 0.20;
   const float etaMaxWin_ecB = 0.20;

   const float etaParams_brl[N_PARAMS] = { 0.038,  0.284};
   const float etaParams_ec1[N_PARAMS] = { 0.011,  0.519};
   const float etaParams_ec2[N_PARAMS] = { 0.023,  0.253};
   const float etaParams_ec3[N_PARAMS] = { 0.018,  0.519};
   const float etaParams_ecA[N_PARAMS] = { 0.010,  0.431};
   const float etaParams_ecB[N_PARAMS] = { 0.023,  0.236};

   //
   const float phiMinWin_brl = 0.125;
   const float phiMinWin_ec1 = 0.125;
   const float phiMinWin_ec2 = 0.125;
   const float phiMinWin_ec3 = 0.10;
   const float phiMinWin_ecA = 0.15;
   const float phiMinWin_ecB = 0.15;

   const float phiMaxWin_brl = 0.20;
   const float phiMaxWin_ec1 = 0.20;
   const float phiMaxWin_ec2 = 0.20;
   const float phiMaxWin_ec3 = 0.20;
   const float phiMaxWin_ecA = 0.25;
   const float phiMaxWin_ecB = 0.20;

   const float phiParams_brl[N_PARAMS] = { 0.000,  0.831};
   const float phiParams_ec1[N_PARAMS] = { 0.000,  0.885};
   const float phiParams_ec2[N_PARAMS] = { 0.015,  0.552};
   const float phiParams_ec3[N_PARAMS] = { 0.008,  0.576};
   const float phiParams_ecA[N_PARAMS] = { 0.000,  0.830};
   const float phiParams_ecB[N_PARAMS] = { 0.006,  1.331};

   //
   float minWin;
   float maxWin;
   float params[N_PARAMS];
   if( isBarrel ) {
      if( isEta ) {
	 memcpy(params,etaParams_brl,sizeof(params));
	 minWin = etaMinWin_brl;
	 maxWin = etaMaxWin_brl;
      }
      else {
	 memcpy(params,phiParams_brl,sizeof(params));
	 minWin = phiMinWin_brl;
	 maxWin = phiMaxWin_brl;
      }
   }
   else { // endcap
      MufastConsts::ECRegions reg = whichECRegion(eta,phi);
      if( reg == MufastConsts::WeakBFieldA ) {
	 if( isEta ) {
	    memcpy(params,etaParams_ecA,sizeof(params));
	    minWin = etaMinWin_ecA;
	    maxWin = etaMaxWin_ecA;
	 }
	 else {
	    memcpy(params,phiParams_ecA,sizeof(params));
	    minWin = phiMinWin_ecA;
	    maxWin = phiMaxWin_ecA;
	 }
      }
      else if( reg == MufastConsts::WeakBFieldB ) {
	 if( isEta ) {
	    memcpy(params,etaParams_ecB,sizeof(params));
	    minWin = etaMinWin_ecB;
	    maxWin = etaMaxWin_ecB;
	 }
	 else {
	    memcpy(params,phiParams_ecB,sizeof(params));
	    minWin = phiMinWin_ecB;
	    maxWin = phiMaxWin_ecB;
	 }
      }
      else {
	 if( fabs(eta) < 1.5 ) {
	    if( isEta ) {
	       memcpy(params,etaParams_ec1,sizeof(params));
	       minWin = etaMinWin_ec1;
	       maxWin = etaMaxWin_ec1;
	    }
	    else {
	       memcpy(params,phiParams_ec1,sizeof(params));
	       minWin = phiMinWin_ec1;
	       maxWin = phiMaxWin_ec1;
	    }
	 }
	 else if( fabs(eta) < 2.0 ) {
	    if( isEta ) {
	       memcpy(params,etaParams_ec2,sizeof(params));
	       minWin = etaMinWin_ec2;
	       maxWin = etaMaxWin_ec2;
	    }
	    else {
	       memcpy(params,phiParams_ec2,sizeof(params));
	       minWin = phiMinWin_ec2;
	       maxWin = phiMaxWin_ec2;
	    }
	 }
	 else {
	    if( isEta ) {
	       memcpy(params,etaParams_ec3,sizeof(params));
	       minWin = etaMinWin_ec3;
	       maxWin = etaMaxWin_ec3;
	    }
	    else {
	       memcpy(params,phiParams_ec3,sizeof(params));
	       minWin = phiMinWin_ec3;
	       maxWin = phiMaxWin_ec3;
	    }
	 }
      }
   }
   
   //
   float x = params[0] + params[1] / pt;
   float retval = x;
   if( x < minWin ) retval = minWin;
   if( x > maxWin ) retval = maxWin;
   
   return retval;
}

MufastConsts::ECRegions muFast::whichECRegion( const float eta, const float phi ) const
{
   float absEta = fabs(eta);
   
   if(      ( 1.3 <= absEta && absEta < 1.45) &&
	    ( (0                 <= fabs(phi) && fabs(phi) < M_PI/48. )     ||
	      (M_PI*11./48. <= fabs(phi) && fabs(phi) < M_PI*13./48. ) ||
	      (M_PI*23./48. <= fabs(phi) && fabs(phi) < M_PI*25./48. ) ||
	      (M_PI*35./48. <= fabs(phi) && fabs(phi) < M_PI*37./48. ) ||
	      (M_PI*47./48. <= fabs(phi) && fabs(phi) < M_PI )
	       )
      ) return MufastConsts::WeakBFieldA;
   
   else if( ( 1.5 <= absEta && absEta < 1.65 ) &&
	    ( (M_PI*3./32.  <= fabs(phi) && fabs(phi) < M_PI*5./32. ) ||
	      (M_PI*11./32. <= fabs(phi) && fabs(phi) < M_PI*13./32.) ||
	      (M_PI*19./32. <= fabs(phi) && fabs(phi) < M_PI*21./32.) ||
	      (M_PI*27./32. <= fabs(phi) && fabs(phi) < M_PI*29./32.)
	       )
      ) return MufastConsts::WeakBFieldB;
   
   else return MufastConsts::Bulk;
}

