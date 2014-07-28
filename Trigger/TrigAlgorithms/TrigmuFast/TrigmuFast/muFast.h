/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUFAST_H
#define MUFAST_H

#include <fstream>
#include <vector>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInterfaces/FexAlgo.h"
#include "RegionSelector/IRegSelSvc.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/MuRoadsSvc.h"
#include "TrigmuFast/MuLUTSvc.h"

#include "RPCgeometry/IRPCgeometrySvc.h"
#include "TGCgeometry/TGCgeometrySvc.h"
#include "CSCgeometry/CSCgeometrySvc.h"
#include "MDTcabling/IMDTcablingSvc.h"

#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuServiceInterface.h"

#include "TrigmuFast/OnlineSurvey.h"

#include "TrigmuFast/ROBmapper.h"
#include "TrigmuFast/ROImapper.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "TrigConfigSvc/ILVL1ConfigSvc.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"

#include "EventInfo/EventID.h"


class MuonFeature;
class MuonFeatureDetails;
class RpcPad;
class RpcPatRec;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace MufastConsts {
enum ECRegions{ Bulk, WeakBFieldA, WeakBFieldB };
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class muFast : public HLT::FexAlgo
{
    public:
    muFast(const std::string&, ISvcLocator*);
    ~muFast(){};
        
    
    //////////////////////////  STEERING INTERFACES ///////////////////////////
    
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltBeginRun();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement*,HLT::TriggerElement*);
    HLT::ErrorCode hltFinalize();
    // 2012.09.18 KN : necessary to move muon calibration from hltFinalize to hltEndRun 
    HLT::ErrorCode hltEndRun();
    
    private:
    

    ///////////////////////////////// PROCESSING //////////////////////////////

    bool RpcRoIDim(unsigned short int,unsigned short int,unsigned short int,
                   unsigned short int, float&, float&); 
    HLT::ErrorCode muFastSeed(const unsigned int, 
                             const HLT::TriggerElement*,HLT::TriggerElement*,
                             std::vector<uint32_t>*,
			     MuData*, LVL1emu*, const DigiPointer reco_digit,
			     MuonFex*,MuonFeature*);
    void fillNtp(const unsigned int, std::vector<uint32_t>*, MuData*, LVL1emu*, 
	 	 const DigiPointer reco_digit, MuonFex*,MuonFeature*);
    
    
    void extractCalData(uint16_t, uint16_t, uint16_t, uint16_t, unsigned int,
             std::vector<uint32_t>, const MuData*,
             const LVL1emu*, const RpcPatRec*, MdtPatRec*,
             std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>&,
             MuonFeature*);
    
    void streamOutCalData(uint16_t, uint16_t, uint16_t, uint16_t, 
             std::vector<uint32_t>, const RpcPad*, 
	     const LUTDigitVec&, const TrackFit, const DigiPointer,
	     const DigiPointer, 
	     std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& cscRobs,
	     MuonFeature*);
    
    void setup_detector_mask(const EventID* eventId);
    
    bool isRobReadout(uint32_t);
    bool isRobEnabled(uint32_t);
    void screen_robs(std::vector<uint32_t> , std::vector<uint32_t>&);
    
    void checkForOpening(std::string filename);
    bool checkForClosing(std::string filename);
    
    MufastConsts::ECRegions whichECRegion(const float eta, const float phi) const;
    float getRoiSizeForID(bool isEta, MuonFeature* muon_feature);

    //=AI added
    void TGCRoI2etaphi(unsigned short int systenID, unsigned short int subsystemID, 
                       unsigned short int sectorID, unsigned short int RoInumber, 
                       short int& eta, short int& phi);
    //=end
    ////////////////////////////////// VARIABLES //////////////////////////////
    
    StoreGateSvc*        m_pStoreGate;      // pointer to Storegate
    IRegSelSvc*          m_pRegionSelector; // pointer to Region Selector
    MUON::ROBmapper*     m_pROBmapper;      // pointer to the ROB mapper
    MUON::ROImapper*     m_pROImapper;      // pointer to the ROI mapper

    ROBDataProviderSvc*  m_pROBDataProvider; // pointer to the ROB data p.
    
    //new tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_rpc_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_mdt_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_tgc_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_csc_converter;
    
    //Standalone geometry
    ServiceHandle<IRPCgeometrySvc> m_rpc_geometry;
    ServiceHandle<TGCgeometrySvc>  m_tgc_geometry;
    ServiceHandle<CSCgeometrySvc>  m_csc_geometry;
    ServiceHandle<IMDTcablingSvc>  m_mdt_cabling;

    
    //BackExtrapolator
    ToolHandle<ITrigMuonBackExtrapolator> m_backExtrapolator;
    
    
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1ConfigSvc;
    ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;
    ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;
    
    BooleanProperty m_check_detector_mask;
    BooleanProperty mp_detector_mask_on_event;

    bool m_detector_mask_on_event;
    bool m_detector_mask_not_checked;
    bool m_hasMdtBarrelA;
    bool m_hasMdtBarrelC;
    bool m_hasMdtEndcapA;
    bool m_hasMdtEndcapC;
    bool m_hasRpcBarrelA;
    bool m_hasRpcBarrelC;
    bool m_hasTgcEndcapA;
    bool m_hasTgcEndcapC;
    bool m_hasCscEndcapA;
    bool m_hasCscEndcapC;
    
    bool m_gotL1Config;

    //quantities relative to the processed RoIs
    uint32_t m_current_run_id;
    uint32_t m_current_lbk_id;
    uint32_t m_current_bcg_id;
    
    uint32_t m_current_lvl1_id;
    
    const LVL1::RecMuonRoI* m_pMuonRoI;
    
    int m_currentRoIId;

    ITrigTimerSvc*       m_pTimerService;   // pointer to Timer Service
    
    TrigTimer* m_lvl1_emulation_time;  // time the Level-1 emulation

    TrigTimer* m_rpc_track_time;       // time the RPC track fit
    TrigTimer* m_muon_roads_time;      // time the definition of Muon Roads
    TrigTimer* m_mdt_deco_time;        // time the MDT data decoding 
    TrigTimer* m_contiguity_time;      // time the contiguity algorithm

    TrigTimer* m_fit_time;             // time the station fitting procedure
    TrigTimer* m_sagitta_time;         // time the sagitta computing
    TrigTimer* m_estimate_time;        // time the Pt estimation

    TrigTimer* m_pat_rec_time;         // time the pattern recognition
    TrigTimer* m_muon_fex_time;        // time the features extraction

    TrigTimer* m_RoIDimension_time;    // time the extrapolation of the RoI
                                       // region in terms of eta-phi coordinates
				       
    TrigTimer* m_RegionSelector_time;  // time the Region Selector retrieving
    
    TrigTimer* m_PadId_time;           // time the retrieving of the PAD Id
    TrigTimer* m_PadGet_time;          // time the retrieving of the PAD data
    TrigTimer* m_PadGetK_time;         // time the retrieving of the PAD Key
    TrigTimer* m_MdtGet_time;          // time the retrieving of the MDT data

    TrigTimer* m_monitor_time;         // time the muFast monitoring
    TrigTimer* m_muAlgo_time;          // time only the algorithmic part
    TrigTimer* m_muFast_time;          // time muFast + data access
    
    TrigTimer* m_calAccess_time;       // time the MDT cal. access
    TrigTimer* m_idHelper_time;        // time the IdHelper class
    TrigTimer* m_mdtDigi_time;         // time the creation of MDT digits
    TrigTimer* m_calRetrieve_time;     // time the retrieving of the space cir.
    TrigTimer* m_calSend_time;         // time the output of the MDT cal. data
    
    TrigTimer* m_tgc_decoder_time;     // time the decoding and fit of the TGC
        

    std::vector<int> m_RobIds; //list of additional ROBs to be accessed for a run
    std::vector<int> m_MdtId;  //list of additional MDTs to be accessed for RoIs
    //std::vector<int> m_Mdt2id;  //list of additional MDTs to be accessed for RoIs set 2   
    
    std::vector<uint32_t>       m_robIds;        // store the additional ROB Ids
    std::vector<uint32_t>       m_rob_disabled;  // store the statically disabled ROB
    std::set<uint32_t>          m_rob_removed;   // store the removed ROBs
    std::set<unsigned int>      m_mdt_unavailable;   //
    std::set<unsigned int>      m_mdt_disabledOnFly; //
    std::vector<IdentifierHash> m_listOfMDTIds;  // store the additional MDT has Ids
    
    
    MuonFeatureDetails* m_details;
    
    uint32_t m_data_access_error;
    
    uint16_t m_l2pu_id;
    
    OnlineSurvey*  m_monitor;
    
    std::ofstream* m_CalibrationStream;
    int m_cid;
    int m_calib_event;
    
    
    int m_barrel_sector_hit;
    int m_barrel_pad_hit;
    int m_barrel_sector_rec;
    int m_barrel_pad_rec;
    int m_barrel_sector_track_eff;
    int m_barrel_pad_track_eff;
    
    float m_inner_mdt_hits;
    float m_middle_mdt_hits;
    float m_outer_mdt_hits;
    std::vector<float> m_fit_residuals;
    std::vector<float> m_res_inner;
    std::vector<float> m_res_middle;
    std::vector<float> m_res_outer;
    float m_efficiency;
    float m_sag_inverse;
    float m_sagitta;
    float m_address;
    float m_absolute_pt;
    int m_calib_event_size;
    int m_buffer_free_count;
    
    std::vector<float> m_lvl1_eta;
    std::vector<float> m_lvl1_phi;
    std::vector<float> m_lvl1_pt;
    
    //=AI added
    std::vector<float> m_endcap_eta_hit;
    std::vector<float> m_endcap_phi_hit;
    std::vector<float> m_endcap_eta_rec;
    std::vector<float> m_endcap_phi_rec;
    std::vector<float> m_endcap_eta_eff;
    std::vector<float> m_endcap_phi_eff;

    std::vector<float> m_track_eta;
    std::vector<float> m_track_phi;
    //=end

    std::vector<float> m_l1emuFailed_eta;
    std::vector<float> m_l1emuFailed_phi;
    std::vector<float> m_backextrFailed_eta;
    std::vector<float> m_backextrFailed_phi;

    std::vector<float> m_emu_eta;
    std::vector<float> m_emu_phi;
    std::vector<float> m_emu_pt;
    
    std::vector<float> m_lvl1_id;
    
    
    NTuple::Tuple* m_FEXntp;
    NTuple::Item<float> m_fex_genPt;
    NTuple::Item<float> m_fex_genEta;
    NTuple::Item<float> m_fex_genPhi;
    NTuple::Item<long>  m_fex_address;
    NTuple::Item<float> m_fex_pt;
    NTuple::Item<float> m_fex_radius;
    NTuple::Item<float> m_fex_eta;
    NTuple::Item<float> m_fex_phi;
    NTuple::Item<float> m_fex_dir_phi;
    NTuple::Item<float> m_fex_zeta;
    NTuple::Item<float> m_fex_dir_zeta;
    NTuple::Item<float> m_fex_beta;
    

    NTuple::Tuple* m_LUTntp;
    NTuple::Item<long>  m_lut_lvl1Id;
    NTuple::Item<long>  m_lut_roiType;
    NTuple::Item<long>  m_lut_address;
    NTuple::Item<float> m_lut_muEnergy;
    NTuple::Item<float> m_lut_muPt;
    NTuple::Item<float> m_lut_muEta;
    NTuple::Item<float> m_lut_muPhi;
    NTuple::Item<float> m_lut_etaMap;
    NTuple::Item<float> m_lut_phiMap;
    NTuple::Item<float> m_lut_sagitta;
    NTuple::Item<float> m_lut_radius;
    
    

    //Monitoring ntuple
    
    //Global data
    NTuple::Item<long>   m_ALGO_id;
    NTuple::Item<long>   m_TE_id;
    NTuple::Item<long>   m_error_code;
    
    NTuple::Item<long>   m_event_number;
    NTuple::Item<long>   m_roi_id_steer;
    NTuple::Item<long>   m_roi_system;
    NTuple::Item<long>   m_roi_subsystem;
    NTuple::Item<long>   m_roi_sector;
    NTuple::Item<long>   m_roi_number;
    NTuple::Item<long>   m_roi_threshold;
    NTuple::Item<float>  m_roi_eta;
    NTuple::Item<float>  m_roi_phi;
    
    // MDT data access
    NTuple::Item<long>  m_rob_count;
    NTuple::Array<long> m_rob_id;
    
    NTuple::Item<long>  m_csm_count;
    NTuple::Array<long> m_csm_id;
    NTuple::Array<long> m_csm_size;
   
    NTuple::Item<long>  m_removed_rob_count;
    NTuple::Array<long> m_removed_rob_id;
    
    NTuple::Item<long>  m_removed_csm_count;
    NTuple::Array<long> m_removed_csm_id;
    
   
   //RPC reconstruction data    
    NTuple::Item<float> m_rpc1_x;
    NTuple::Item<float> m_rpc1_y;
    NTuple::Item<float> m_rpc1_z;
    NTuple::Item<float> m_rpc1_r;
    NTuple::Item<float> m_rpc2_x;
    NTuple::Item<float> m_rpc2_y;
    NTuple::Item<float> m_rpc2_z;
    NTuple::Item<float> m_rpc2_r;
    NTuple::Item<float> m_rpc3_x;
    NTuple::Item<float> m_rpc3_y;
    NTuple::Item<float> m_rpc3_z;
    NTuple::Item<float> m_rpc3_r;
    
    NTuple::Item<long>  m_eta_pivot_lay0;
    NTuple::Item<long>  m_eta_pivot_lay1;
    NTuple::Item<long>  m_eta_low_0_lay0;
    NTuple::Item<long>  m_eta_low_1_lay0;
    NTuple::Item<long>  m_eta_low_0_lay1;
    NTuple::Item<long>  m_eta_low_1_lay1;
    NTuple::Item<long>  m_eta_high_0_lay0;
    NTuple::Item<long>  m_eta_high_1_lay0;
    NTuple::Item<long>  m_eta_high_0_lay1;
    NTuple::Item<long>  m_eta_high_1_lay1;
    
    NTuple::Item<long>  m_phi_pivot_lay0;
    NTuple::Item<long>  m_phi_pivot_lay1;
    NTuple::Item<long>  m_phi_low_0_lay0;
    NTuple::Item<long>  m_phi_low_1_lay0;
    NTuple::Item<long>  m_phi_low_0_lay1;
    NTuple::Item<long>  m_phi_low_1_lay1;
    NTuple::Item<long>  m_phi_high_0_lay0;
    NTuple::Item<long>  m_phi_high_1_lay0;
    NTuple::Item<long>  m_phi_high_0_lay1;
    NTuple::Item<long>  m_phi_high_1_lay1;
    
    NTuple::Item<long>  m_pad_hit_data;
    
    NTuple::Array<long>  m_pad_hit_onlineId;
    NTuple::Array<long>  m_pad_hit_code;
    NTuple::Array<float> m_pad_hit_x;
    NTuple::Array<float> m_pad_hit_y;
    NTuple::Array<float> m_pad_hit_z;
    NTuple::Array<float> m_pad_hit_r;
    NTuple::Array<float> m_pad_hit_p;
    
    
    //TGC reconstruction data
    NTuple::Item<float> m_tgc_Mid1_eta;
    NTuple::Item<float> m_tgc_Mid1_phi;
    NTuple::Item<float> m_tgc_Mid1_r;
    NTuple::Item<float> m_tgc_Mid1_z;
    NTuple::Item<float> m_tgc_Mid2_eta;
    NTuple::Item<float> m_tgc_Mid2_phi;
    NTuple::Item<float> m_tgc_Mid2_r;
    NTuple::Item<float> m_tgc_Mid2_z;
    NTuple::Item<float> m_tgc_Mid_rho_chi2;
    NTuple::Item<long>  m_tgc_Mid_rho_N;
    NTuple::Item<long>  m_tgc_Mid_rho_N_in;
    NTuple::Item<float> m_tgc_Mid_phi_chi2;
    NTuple::Item<long>  m_tgc_Mid_phi_N;
    NTuple::Item<long>  m_tgc_Mid_phi_N_in;
    NTuple::Item<float> m_tgc_Inn_eta;
    NTuple::Item<float> m_tgc_Inn_phi;
    NTuple::Item<float> m_tgc_Inn_r;
    NTuple::Item<float> m_tgc_Inn_z;
    NTuple::Item<float> m_tgc_Inn_rho_std;
    NTuple::Item<long>  m_tgc_Inn_rho_N;
    NTuple::Item<long>  m_tgc_Inn_rho_N_in;
    NTuple::Item<float> m_tgc_Inn_phi_std;
    NTuple::Item<long>  m_tgc_Inn_phi_N;
    NTuple::Item<long>  m_tgc_Inn_phi_N_in;
    NTuple::Item<float> m_tgc_PT;

    // TGC Hit data
    NTuple::Array<float> m_tgc_Inn_rho_hit_phi;
    NTuple::Array<float> m_tgc_Inn_rho_hit_r;
    NTuple::Array<float> m_tgc_Inn_rho_hit_z;
    NTuple::Array<float> m_tgc_Inn_rho_hit_width;
    NTuple::Array<long>  m_tgc_Inn_rho_hit_in_seg;
    NTuple::Array<float> m_tgc_Inn_phi_hit_phi;
    NTuple::Array<float> m_tgc_Inn_phi_hit_r;
    NTuple::Array<float> m_tgc_Inn_phi_hit_z;
    NTuple::Array<float> m_tgc_Inn_phi_hit_width;
    NTuple::Array<long>  m_tgc_Inn_phi_hit_in_seg;
    NTuple::Array<float> m_tgc_Mid_rho_hit_phi;
    NTuple::Array<float> m_tgc_Mid_rho_hit_r;
    NTuple::Array<float> m_tgc_Mid_rho_hit_z;
    NTuple::Array<float> m_tgc_Mid_rho_hit_width;
    NTuple::Array<long>  m_tgc_Mid_rho_hit_in_seg;
    NTuple::Array<float> m_tgc_Mid_phi_hit_phi;
    NTuple::Array<float> m_tgc_Mid_phi_hit_r;
    NTuple::Array<float> m_tgc_Mid_phi_hit_z;
    NTuple::Array<float> m_tgc_Mid_phi_hit_width;
    NTuple::Array<long>  m_tgc_Mid_phi_hit_in_seg;
    
    
    //MDT hits
    NTuple::Item<long>  m_mdt_hit_tubes;
    
    NTuple::Array<float> m_mdt_onlineId;
    NTuple::Array<float> m_mdt_offlineId;
    NTuple::Array<float> m_mdt_tube_r;
    NTuple::Array<float> m_mdt_tube_z;
    NTuple::Array<float> m_mdt_tube_residual;
    NTuple::Array<float> m_mdt_tube_time;
    NTuple::Array<float> m_mdt_tube_space;
    NTuple::Array<float> m_mdt_tube_sigma;
    
    
    // output of pattern recognition
    NTuple::Item<float> m_mdt_Inner_slope;
    NTuple::Item<float> m_mdt_Inner_intercept;
    NTuple::Item<float> m_mdt_Inner_Z;
    NTuple::Item<float> m_mdt_Inner_R;
    NTuple::Item<float> m_mdt_Inner_fit_chi;
    NTuple::Item<float> m_mdt_Middle_slope;
    NTuple::Item<float> m_mdt_Middle_intercept;
    NTuple::Item<float> m_mdt_Middle_Z;
    NTuple::Item<float> m_mdt_Middle_R;
    NTuple::Item<float> m_mdt_Middle_fit_chi;
    NTuple::Item<float> m_mdt_Outer_slope;
    NTuple::Item<float> m_mdt_Outer_intercept;
    NTuple::Item<float> m_mdt_Outer_Z;
    NTuple::Item<float> m_mdt_Outer_R;
    NTuple::Item<float> m_mdt_Outer_fit_chi;
    
    // output of measurements
    NTuple::Item<double> m_Sagitta;
    NTuple::Item<double> m_Radius;
    NTuple::Item<double> m_Slope;
    NTuple::Item<double> m_Intercept;
    NTuple::Item<double> m_Alpha;
    NTuple::Item<double> m_Beta;
    NTuple::Item<double> m_DeltaR;
    NTuple::Item<double> m_Speed_over_c;
    NTuple::Item<double> m_EtaMap;
    NTuple::Item<double> m_PhiMap;
    NTuple::Item<double> m_Phi;
    NTuple::Item<double> m_PhiDir;
    NTuple::Item<double> m_Pt;
    NTuple::Item<float>  m_Charge;
    
    
    NTuple::Tuple* m_MONntp;
    
    

    MuMessageInterface* m_msg;
    MuServiceInterface* m_svc;

    std::string m_paramSet;
    std::string m_stream_name;

    private:
    BooleanProperty m_timing;         // switch on the timing measurements
    BooleanProperty m_fexntp;         // switch on the Feature ntuple production
    BooleanProperty m_lutntp;         // switch on the LUT ntuple production
    BooleanProperty m_tgcntp;         // switch on the TGC ntuple production
    BooleanProperty m_monntp;         // switch on the MON ntuple production
    
    IntegerProperty m_evtToShow;      // set the maximum number of event to be
                                      // printout
    BooleanProperty m_MUlvl1_info;    // switch on the LVL1 emulation printouts
    BooleanProperty m_MUtrack_info;   // switch on the RPC tracking printouts
    BooleanProperty m_MUroads_info;   // switch on the Muon Roads printouts 
    BooleanProperty m_MUdeco_info;    // switch on the MDT deconding printouts
    BooleanProperty m_MUcont_info;    // switch on the MDT contiguity printouts
    BooleanProperty m_MUfit_info;     // switch on the MDT station fit printouts
    BooleanProperty m_MUsag_info;     // switch on the sagitta printouts
    BooleanProperty m_MUpt_info;      // switch on the pt estimate printouts
    BooleanProperty m_MUtgcDeco_info; // switch on the Tgc Deco printouts
    BooleanProperty m_FITdebug_info;  // switch on the fit debug printouts
    BooleanProperty m_Esd_info;       // switch on the printouts of ESD
    StringProperty  m_test_string;    // string to show for the regular tests
    
    BooleanProperty m_use_calib;      // switch on the use of calibration Svc
    BooleanProperty m_tilted_B_field; // switch on the inversion of the charge
    DoubleProperty  m_particle_beta;  // beta for the slow particles
    
    BooleanProperty m_OnlineRun;      // flag to set the online run environment:
                                      // no timing and no fex N-tuple production
    BooleanProperty  m_create_roi;    // flag for creating the muon RoI
    BooleanProperty  m_maskUncCMAch;  // flag the RPC masking
    BooleanProperty  m_trigOnPhiOnly; // flag to set the RPC trigger view
    BooleanProperty  m_useRpcTrigPatt;// flag to use the RPC trigger pattern

    BooleanProperty  m_CosmicRun;     // flag to set the Cosmic run 
                                      // environmment
    BooleanProperty  m_BfieldOn;      // flag to configure the reconstruction
                                      // according to the B-field status 
    
    BooleanProperty m_confirmCalTrack;// extract cal data with no confirm from
                                      // trigger reconstruction 
    BooleanProperty m_doCalStream;    // output calibration data into LVL2 stream
    BooleanProperty m_doCalBuild;     // output calibration data into Partial
                                      // build
    StringProperty  m_pEBInfoSGKey;   // the storegate key used to record the
                                      // PEB Info Object 		      
    BooleanProperty  m_remove_tgc_trigger;
    BooleanProperty  m_csc_data_conversion;
    
    BooleanProperty  m_use_mcLUT;
    BooleanProperty  m_usePol2ForEPt;
    
    BooleanProperty m_output_details; // switch on/off the production of mufast
                                      // ESD
    
    //BooleanProperty  m_TBmultipleRoIs;// flag to switch between single and
                                      // multiple RoI 
				      
    IntegerProperty  m_subsysID;      // set the RPC subsystem for TestBeam      
    IntegerProperty  m_RoINumber;     // set the RoI number for TestBeam
    IntegerProperty  m_sectorID;      // set the sector Logic for TestBeam
    
    StringProperty m_histo_path_base;     // set the histo path for Monitoring
    StringProperty m_ROBmapper_filename;  // set the ROB mapper file
    StringProperty m_ROImapper_filename;  // set the ROI mapper file
    StringProperty m_LUT_filename;        // set the LUT file
    StringProperty m_key_for_truth;       // key for retrieving the Montecarlo
                                          // truth
    StringProperty m_CalBufferName;
    IntegerProperty m_CalBufferSize;

    // 2012.09.18 KN : necessary to move muon calibration from hltInitialize to hltBeginRun 
    std::string m_CalBufferNameDetermined;
    std::string m_TestBufferNameDetermined;

    IntegerProperty m_esd_ext_size;
    IntegerProperty m_esd_rob_size;
    IntegerProperty m_esd_csm_size;
    IntegerProperty m_esd_lv1_size;
    IntegerProperty m_esd_rpc_size;
    IntegerProperty m_esd_tgc_size;
    IntegerProperty m_esd_mdt_size;

    DoubleProperty m_TGC_ETA_LOW_MID_DELTA;
    DoubleProperty m_TGC_ETA_HIGH_MID_DELTA;
    DoubleProperty m_TGC_PHI_MID_DELTA;
    DoubleProperty m_TGC_ETA_LOW_INN_DELTA;
    DoubleProperty m_TGC_ETA_HIGH_INN_DELTA;
    DoubleProperty m_TGC_PHI_INN_DELTA;
    DoubleProperty m_TGC_CHI2_TEST;
    UnsignedIntegerProperty m_TGC_MIN_WIRE_POINTS;
    UnsignedIntegerProperty m_TGC_MIN_STRIP_POINTS;
    
    DoubleProperty m_RpcTimeDelay;
    
    int m_TgcGeometry_NoConf_Erros;
};

#endif // MUFAST_H
