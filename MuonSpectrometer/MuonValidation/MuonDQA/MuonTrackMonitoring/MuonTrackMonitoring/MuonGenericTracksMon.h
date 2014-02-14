/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonGenericTracksMon_H
#define MuonGenericTracksMon_H

// **********************************************************************
// MuonGenericTracksMon.cxx
// AUTHORS: N. Benekos, E. Christidi, A. Eppig, Tony Liss
// **********************************************************************
 
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "StoreGate/StoreGate.h"
 
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
 
#include "TrkParameters/TrackParameters.h"
 
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
 
#include "TrkToolInterfaces/ITrackSummaryTool.h" 
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
 
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
 	
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
 
#include "MuonDQAUtils/MuonDQAFitFunc.h"
#include "TProfile.h"
#include <vector>

class ActiveStoreSvc;
 
namespace Trk {
  class IResidualPullCalculator;
  class ITrackSummaryTool;
  class Track;
}

namespace Muon {
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  class IMuonHitSummaryTool;
}

namespace Trk {
  class IUpdator;
  class IPropagator;
  class RIO_OnTrack;
  class Track;
  class TrackStateOnSurface;
  class ITrackSummaryHelperTool;
}
 
class MuonGenericTracksMon : public ManagedMonitorToolBase
{
 
 public:
 /** Constructor */
  MuonGenericTracksMon( const std::string & type, const std::string & name, const IInterface* parent ); 

  /** Destructor */
  virtual ~MuonGenericTracksMon();
  
  StatusCode initialize();
 
  /** Histogram booking method */
  virtual StatusCode bookHistograms();   
  /** Histogram filling method */
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms(); 
 
  StatusCode finalize();
  float   RadsToDegrees(float Radians); 
 
 protected:
 
 
 private:
 
            
  StatusCode setupTools();
         
          
  StoreGateSvc* m_storeGate;
  ActiveStoreSvc* m_activeStore;
	 
  std::string m_stream;
 
  //std::vector<std::string> m_MuonTriggerChainName;
  std::string m_MuonTriggerChainName;

  int m_checkrate, m_nphi_bins;


  AtlasDetectorID*                      m_idHelper;          //!< Used to find out the sub-det from PRD->identify().

  const MuonGM::MuonDetectorManager* m_muonMgr;
  const MdtIdHelper* m_mdtIdHelper;  
  const RpcIdHelper* m_rpcIdHelper;
  const TgcIdHelper* m_tgcIdHelper;  
  
  std::vector<std::string> m_trackCollectionName;   //!< Reconstructed track collection name
  std::vector<int> m_trackCollectionFlag;
  const DataVector<Trk::Track> *m_tracks;           //container for tracks
  
  int               m_eventCounter;
  int               m_eventNumber;
  static const int  s_maxEvents;            //!< Maximum number of events per run
  static const int  s_maxComponents;        //!< Maximum number of components in a Trk::MultiComponentState
  static const int  s_maxSoS;               //!< Maximum number of TrackStateOnSurface objects associated to the track

  mutable MsgStream      m_log;                  //!< Gaudi message stream - msgstream as private member (-> speed)
  bool                   m_debuglevel;           //!< private member to control debug messages

  // Handle for the trig decision tool
  ToolHandle<ITrigDecisionTool> m_trigDecTool;
  
  /** Pointer to Trk::ITrackSummaryTool */
  ToolHandle< Trk::ITrackSummaryTool > m_trackSumTool;
 
  
  // Trigger items
  std::vector<std::string> m_muon_triggers;
  
  // user flag to switch trigger on or off
  bool m_useTrigger;
 
  int m_rec_nSoS;
  int m_rec_nComponents;
  
  // Muon Monitoring Histograms 

  //  std::vector<TH1F*> m_rec_nComponents;
  //std::vector<TH1F*>     muon_rec_weights;
  std::vector<TH1F*>     muon_rec_1overpT;
  std::vector<TH1F*>     muon_rec_charge;
  std::vector<TH1F*>     muon_rec_nStations;
  //std::vector<TH1F*>   muon_num_tracks;

  std::vector<TH1F*> muon_rec_pT;  
  std::vector<TH1F*> muon_rec_pos_pT;
  std::vector<TH1F*> muon_rec_neg_pT;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT;

  std::vector<TH1F*> muon_rec_HighpT;
  std::vector<TH1F*> muon_rec_pos_HighpT;
  std::vector<TH1F*> muon_rec_neg_HighpT;

  //std::vector<TH1F*> muon_numHits_per_track;
  //std::vector<TH2F*> muon_numHits_per_track_vrs_phi;
  std::vector<TH2F*>   muon_rec_eta_phi;
  std::vector<TH2F*>   muon_rec_eta_phi_3moreStations;
  std::vector<TH2F*>   muon_rec_eta_phi_2lessStations;
  
  std::vector<TH1F*> muon_rec_locX;
  std::vector<TH1F*> muon_rec_error_locX;

  std::vector<TH1F*> muon_rec_locY;
  std::vector<TH1F*> muon_rec_error_locY;

  std::vector<TH1F*> muon_rec_locR;
  std::vector<TH1F*> muon_rec_error_locR;

  std::vector<TH1F*> muon_rec_phi;
  std::vector<TH1F*> muon_rec_error_phi;

  std::vector<TH1F*> muon_rec_theta;
  std::vector<TH1F*> muon_rec_error_theta;

  std::vector<TH1F*> muon_rec_qOverP;
  std::vector<TH1F*> muon_rec_error_qOverP;

  std::vector<TH1F*> muon_rec_posX;
  std::vector<TH1F*> muon_rec_posY;
  std::vector<TH1F*> muon_rec_posZ;
  std::vector<TH1F*> muon_rec_posR;

  std::vector<TH1F*> muon_glbl_residual;
  std::vector<TH1F*> muon_MDT_residual;
  std::vector<TH1F*> muon_RPC_residual;
  std::vector<TH1F*> muon_TGC_residual;
  std::vector<TH1F*> muon_CSC_residual;

  std::vector<TH1F*> muon_glbl_pull;
  std::vector<TH1F*> muon_MDT_pull;
  std::vector<TH1F*> muon_RPC_pull;
  std::vector<TH1F*> muon_TGC_pull;
  std::vector<TH1F*> muon_CSC_pull;

  std::vector<TH1F*> muon_RPC_eta_residual;
  std::vector<TH1F*> muon_RPC_phi_residual;
  std::vector<TH1F*> muon_TGC_eta_residual;
  std::vector<TH1F*> muon_TGC_phi_residual;
  std::vector<TH1F*> muon_CSC_eta_residual; 
  std::vector<TH1F*> muon_CSC_phi_residual; 
    
  std::vector<TH1F*> muon_RPC_pull_eta;
  std::vector<TH1F*> muon_RPC_pull_phi;
  std::vector<TH1F*> muon_TGC_pull_eta;
  std::vector<TH1F*> muon_TGC_pull_phi;
  std::vector<TH1F*> muon_CSC_pull_eta; 
  std::vector<TH1F*> muon_CSC_pull_phi; 
  
  /** Muon Residuals Histograms */
  std::vector<TH2F*>     muon_MDT_residual_pT;
  std::vector<TProfile*> muon_MDT_residual_pT_RMS;
    
  //Track Parameters at Perigee and their error
  std::vector<TH1F*> muon_perigee_qOverP;    
  std::vector<TH1F*> muon_perigee_P;    
  std::vector<TH1F*> muon_perigee_d0;     
  std::vector<TH1F*> muon_perigee_z0;     
  std::vector<TH1F*> muon_perigee_phi0;     
  std::vector<TH1F*> muon_perigee_theta;     
  std::vector<TH1F*> muon_perigee_eta; 
  
  std::vector<TH1F*> muon_perigee_error_d0;     
  std::vector<TH1F*> muon_perigee_error_z0;     
  std::vector<TH1F*> muon_perigee_error_phi0;     
  std::vector<TH1F*> muon_perigee_error_theta0;     
  std::vector<TH1F*> muon_perigee_error_qOverP;
      
  std::vector<TH1F*> muon_rec_chi2;     
  std::vector<TH1F*> muon_rec_DegreesOfFreedom;     

  std::vector<TH2F*> muon_rec_pT_vs_eta;
  std::vector<TH2F*> muon_rec_pT_vs_phi; 
  
  // pT histograms to understand charge asymmetry
  std::vector<TH2F*> muon_rec_charge_vs_eta;  
  std::vector<TH2F*> muon_rec_charge_vs_phi;  

  std::vector<TH1F*> muon_rec_pos_pT_Sector01;
  std::vector<TH1F*> muon_rec_neg_pT_Sector01;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector01;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector02;
  std::vector<TH1F*> muon_rec_neg_pT_Sector02;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector02;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector03;
  std::vector<TH1F*> muon_rec_neg_pT_Sector03;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector03;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector04;
  std::vector<TH1F*> muon_rec_neg_pT_Sector04;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector04;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector05;
  std::vector<TH1F*> muon_rec_neg_pT_Sector05;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector05;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector06;
  std::vector<TH1F*> muon_rec_neg_pT_Sector06;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector06;

  std::vector<TH1F*> muon_rec_pos_pT_Sector07;
  std::vector<TH1F*> muon_rec_neg_pT_Sector07;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector07;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector08;
  std::vector<TH1F*> muon_rec_neg_pT_Sector08;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector08;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector09;
  std::vector<TH1F*> muon_rec_neg_pT_Sector09;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector09;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector10;
  std::vector<TH1F*> muon_rec_neg_pT_Sector10;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector10;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector11;
  std::vector<TH1F*> muon_rec_neg_pT_Sector11;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector11;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector12;
  std::vector<TH1F*> muon_rec_neg_pT_Sector12;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector12;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector13;
  std::vector<TH1F*> muon_rec_neg_pT_Sector13;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector13;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector14;
  std::vector<TH1F*> muon_rec_neg_pT_Sector14;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector14;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector15;
  std::vector<TH1F*> muon_rec_neg_pT_Sector15;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector15;
  
  std::vector<TH1F*> muon_rec_pos_pT_Sector16;
  std::vector<TH1F*> muon_rec_neg_pT_Sector16;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_Sector16;
  
  std::vector<TH1F*> muon_rec_pos_pT_A;
  std::vector<TH1F*> muon_rec_neg_pT_A;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_A;
  
  std::vector<TH1F*> muon_rec_pos_pT_C;
  std::vector<TH1F*> muon_rec_neg_pT_C;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_C;
  
  std::vector<TH1F*> muon_rec_pos_pT_Up;
  std::vector<TH1F*> muon_rec_neg_pT_Up;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_up;
  
  std::vector<TH1F*> muon_rec_pos_pT_Down;
  std::vector<TH1F*> muon_rec_neg_pT_Down;
  std::vector<TH1F*> muon_rec_ChargeRatio_pT_dw;  
 
  /// Momentum differences between up-down sectors
  std::vector<TH1F*> muon_rec_pos_pT_up_dw_diff;
  std::vector<TH1F*> muon_rec_neg_pT_up_dw_diff;
  std::vector<TH1F*> muon_rec_pos_pT_A_C_diff;
  std::vector<TH1F*> muon_rec_neg_pT_A_C_diff;
    
  
  /** TProfile: MDT Pull per phi sector, per side*/
  std::vector<TH2F*> muon_MDT_residual_BA_2stations;
  std::vector<TH2F*> muon_MDT_residual_BC_2stations;
  std::vector<TH2F*> muon_MDT_residual_EA_2stations;
  std::vector<TH2F*> muon_MDT_residual_EC_2stations;
  
  std::vector<TH2F*> muon_MDT_residual_BA;
  std::vector<TH2F*> muon_MDT_residual_BC;
  std::vector<TH2F*> muon_MDT_residual_EA;
  std::vector<TH2F*> muon_MDT_residual_EC;
    
  std::vector<TH1F*> muon_MDT_residual_BA_fit;
  std::vector<TH1F*> muon_MDT_residual_BC_fit;
  std::vector<TH1F*> muon_MDT_residual_EA_fit;
  std::vector<TH1F*> muon_MDT_residual_EC_fit;
  
  std::vector<TProfile*> muon_MDT_pull_sideA_RMS;
  std::vector<TProfile*> muon_MDT_pull_sideC_RMS;

   
  /** TProfile: Residuals per signficant "sector" */
  std::vector<TProfile*> muon_MDT_residual_sideA_RMS;
  std::vector<TProfile*> muon_MDT_residual_sideC_RMS;
  
  std::vector<TProfile*> muon_MDT_residual_BA_RMS_2stations;
  std::vector<TProfile*> muon_MDT_residual_BC_RMS_2stations;
  std::vector<TProfile*> muon_MDT_residual_EA_RMS_2stations;
  std::vector<TProfile*> muon_MDT_residual_EC_RMS_2stations;
  
  std::vector<TProfile*> muon_MDT_residual_BA_RMS;
  std::vector<TProfile*> muon_MDT_residual_BC_RMS;
  std::vector<TProfile*> muon_MDT_residual_EA_RMS;
  std::vector<TProfile*> muon_MDT_residual_EC_RMS;

  std::vector<TProfile*> muon_RPC_eta_residual_RMS;
  std::vector<TProfile*> muon_RPC_phi_residual_RMS;
  std::vector<TProfile*> muon_TGC_residual_wire_RMS;
  std::vector<TProfile*> muon_TGC_residual_strip_RMS;
  std::vector<TProfile*> muon_CSC_eta_residual_RMS;
  std::vector<TProfile*> muon_CSC_phi_residual_RMS;

    
  /** TProfile: TGC Residuals vs phi sector*/
  
  std::vector<TProfile*> muon_TGC_residual_EndCapWire_phi;
  std::vector<TProfile*> muon_TGC_residual_EndCapStrip_phi;
  std::vector<TProfile*> muon_TGC_residual_ForwardWire_phi;
  std::vector<TProfile*> muon_TGC_residual_ForwardStrip_phi;
  
  

  /** TProfile: MDT Residuals per phi sector*/

  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi1;
  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi2;
  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi3;
  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi4;
  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi5;
  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi6;
  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi7;
  std::vector<TProfile*> muon_MDT_residual_barrel_SectorPhi8;

  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi1;
  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi2;
  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi3;
  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi4;
  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi5;
  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi6;
  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi7;
  std::vector<TProfile*> muon_MDT_residual_eca_SectorPhi8;

  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi1;
  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi2;
  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi3;
  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi4;
  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi5;
  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi6;
  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi7;
  std::vector<TProfile*> muon_MDT_residual_ecc_SectorPhi8;  
  
  /** TH1F MDT RMS from Residuals TProfiles per phi sector*/ 

  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi1;
  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi2;
  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi3;
  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi4;
  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi5;
  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi6;
  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi7;
  std::vector<TH1F*> muon_MDT_residualRMS_barrel_SectorPhi8;

  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi1;
  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi2;
  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi3;
  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi4;
  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi5;
  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi6;
  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi7;
  std::vector<TH1F*> muon_MDT_residualRMS_eca_SectorPhi8;

  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi1;
  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi2;
  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi3;
  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi4;
  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi5;
  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi6;
  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi7;
  std::vector<TH1F*> muon_MDT_residualRMS_ecc_SectorPhi8;
     
  // Tool handles
  ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;     //<! tool to calculate residuals and pulls
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
  //const Muon::MuonDQAFitFunc *m_muondqafitfunc;
  ToolHandle<Muon::MuonDQAFitFunc> m_muondqafitfunc;
  ToolHandle<Muon::IMuonHitSummaryTool> m_muonHitSummaryTool;
  ToolHandle<Trk::ITrackSummaryHelperTool>         m_trackSummaryTool; //<! muon track summary helper 
};

#endif
 


