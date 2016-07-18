/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONFILLNTUPLETOOL_H
#define MUONALIGNGENTOOLS_MUONFILLNTUPLETOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignInterfaces/IFillNtupleTool.h"
#include "TrkSegment/TrackSegment.h"
#include "TrkSegment/SegmentCollection.h"
#include "TMatrixD.h"
#include "TVector3.h"

/**
   @file MuonFillNtupleTool.h
   @class MuonFillNtupleTool

   @brief Tool for writing information to ntuple for debugging.

   @author Robert Harrington <roberth@bu.edu>, Steffen Kaiser <Steffen.Kaiser@cern.ch>
   @date 10/14/2008
*/

class TFile;
class TTree;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class MuonEDMHelperTool;
  class MuonTrackHistTool;
  class MuonIdHelperTool;
}

class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

namespace Trk {
  class AlignTrack;
  class IAlignModuleTool;
  class IResidualPullCalculator;
}

class Identifier;

namespace MuonCalib {
  class IIdToFixedIdTool;
}

namespace Muon {
  class MuonFillNtupleTool : virtual public Trk::IFillNtupleTool, public AthAlgTool {
    
  public:
    MuonFillNtupleTool(const std::string& type, const std::string& name,
			    const IInterface* parent);
    virtual ~MuonFillNtupleTool();
    
    StatusCode initialize();
    StatusCode finalize();

    /** dumps track information to ntuple */
    void dumpTrack(int itrk, const Trk::AlignTrack* alignTrack);

    void storeHitmap();
    void fillHitmap();
    void fillSummary();
    void showStatistics();

    void setNtuple(TFile* ntuple) { m_ntuple=ntuple; }
    StatusCode fillNtuple();

  private:

    void createChamberTrees();

    // ToolHandle<Muon::MuonTrackHistTool>   m_trackHistTool; //!< Pointer to MuonTrackHistTool
    ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator; //!< Pointer to pull calculator
    //ToolHandle<Muon::MuonEDMHelperTool>   m_helperTool; //!< Pointer to EDM helper tool
    ToolHandle<MuonCalib::IIdToFixedIdTool> m_idTool;   //!< Pointer to IdToFixedIdTool
    ToolHandle<Trk::IAlignModuleTool> m_alignModuleTool; //!< Pointer to AlignModuleTool


    ToolHandle<MuonIdHelperTool>   m_muonIdHelperTool;
 
    const MuonGM::MuonDetectorManager* p_muonMgr; //!< Pointer to muon detector manager

    // access to Id Helpers
    const MdtIdHelper* p_MdtIdHelper; //!< Pointer to MDT Id helper
    const CscIdHelper* p_CscIdHelper; //!< Pointer to CSC Id helper
    const RpcIdHelper* p_RpcIdHelper; //!< Pointer to RPC Id helper
    const TgcIdHelper* p_TgcIdHelper; //!< Pointer to TGC Id helper

    std::string m_segmentcollName;

    TFile* m_ntuple;
    TTree* m_tree;

    // Reconstructed variables
    //int      m_runNumber;
    //int      m_evtNumber;
    
    //int      m_nMuonOutliers;
    //int      m_nTgcOutliers;

    /** number of associated precision hits [MDT+CSC] (etah in ntuple)  */
    //int m_etah;
    /** number of trigger chamber hits measuring phi (phih in ntuple)  */
    //int m_phih;
    /** number of associated MDT hits [including tube hits] (mdth in ntuple)  */
    //int m_mdth;
    /** number of associated CSC eta hits [precision coordinate] (cscetah in ntuple)  */
    //int m_cscetah;
    /** number of associated CSC phi hits [second coordinate] (cscphih in ntuple)  */
    //int m_cscphih;
    /** number of associated RPC eta hits [precision coordinate] (rpcetah in ntuple)  */
    //int m_rpcetah;
    /** number of associated RPC phi hits [second coordinate] (rpcphih in ntuple)  */
    //int m_rpcphih;
    /** number of associated TGC eta hits [precision coordinate] (tgcetah in ntuple)  */
    //int m_tgcetah;
    /** number of associated TGC phi hits [second coordinate] (tgcphih in ntuple)  */
    //int m_tgcphih;
    /** number of associated stations (stati in ntuple)  */
    //int m_stati;
    /** track fit status:  1=road, 2=merged_road, 3=road_wire, 4=road_time, 5=final_wire, 6=final_time, 7=final_track (star in ntuple)  */
    //int m_statr;

    /** x coord of track perigee (xvtxr in ntuple)  */
    //double m_xvtxr;
    /** y coord of track perigee (yvtxr in ntuple)  */
    //double m_yvtxr;
    /** z coord of track perigee (zvtxr in ntuple)  */
    //double m_zvtxr;
    /** a0 perigee parameter of reconstructed track (a0r in ntuple)  */
    //double m_a0r;
    /** z0 perigee parameter of reconstructed track (z0r in ntuple)  */
    //double m_z0r;
    /** phi perigee parameter of reconstructed track (phir in ntuple)  */
    //double m_phir;
    /** cot(theta) perigee parameter of reconstructed track (cotthr in ntuple)  */
    //double m_cotthr;
    /** charge/pt perigee parameter of reconstructed track (ptir in ntuple)  */
    //double m_ptir;
    /** eta of perigee of reconstructed track (etar in ntuple)  */
    //double m_etar;
    /** Chi2 per degree of freedom of the track fit (chi2 in ntuple)  */
    //double m_chi2;
    /** Chi2 probability of the track fit (chi2pr in ntuple)  */
    //double m_chi2pr;
    /** x coord of vertex w.r.t which track perigee parameters are given (origx in ntuple)  */
    //double m_vertexx;
    /** y coord of vertex w.r.t which track perigee parameters are given (origy in ntuple)  */
    //double m_vertexy;
    /** z coord of vertex w.r.t which track perigee parameters are given (origz in ntuple)  */
    //double m_vertexz;

    /** covariance matrix element [a0,a0] (covr11 in ntuple)  */
    //double m_covr11;
    /** covariance matrix element [a0,z0] (covr12 in ntuple)  */
    //double m_covr12;
    /** covariance matrix element [z0,z0] (covr22 in ntuple)  */
    //double m_covr22;
    /** covariance matrix element [a0,phi] (covr13 in ntuple)  */
    //double m_covr13;
    /** covariance matrix element [z0,phi] (covr23 in ntuple)  */
    //double m_covr23;
    /** covariance matrix element [phi,phi] (covr33 in ntuple)  */
    //double m_covr33;
    /** covariance matrix element [a0,cot(theta)] (covr14 in ntuple)  */
    //double m_covr14;
    /** covariance matrix element [z0,cot(theta)] (covr24 in ntuple)  */
    //double m_covr24;
    /** covariance matrix element [phi,cot(theta)] (covr34 in ntuple)  */
    //double m_covr34;
    /** covariance matrix element [cot(theta),cot(theta)] (covr44 in ntuple)  */
    //double m_covr44;
    /** covariance matrix element [a0,q/pt] (covr15 in ntuple)  */
    //double m_covr15;
    /** covariance matrix element [z0,q/pt] (covr25 in ntuple)  */
    //double m_covr25;
    /** covariance matrix element [phi,q/pt] (covr35 in ntuple)  */
    //double m_covr35;
    /** covariance matrix element [cot(theta),q/pt] (covr45 in ntuple)  */
    //double m_covr45;
    /** covariance matrix element [q/pt,q/pt] (covr55 in ntuple)  */
    //double m_covr55;
  
   /** HIT INFORMATION */

    /** number of hits on track [including scattering centers] (nhits in ntuple) */
    //int m_nhits;

    /** hit station name index [see MuonIdHelper] (statname in ntuple) */
    int*  m_hit_statname;
    /** hit station eta index [see MuonIdHelper] (stateta in ntuple) */
    int*  m_hit_stateta;
    /** hit station phi index [see MuonIdHelper] (statphi in ntuple) */
    int*  m_hit_statphi;
    /** hit station region index: 0=Inner, 1=BEE or EE, 2=Middle, 3=Outer (statreg in ntuple) */
    int*  m_hit_statreg;
    /** hit technology [see MuonIdHelper] (tech in ntuple) */
    int*  m_hit_tech;
    /** hit detector technology: 1=RPCeta, 2=RPCphi, 3=TGCeta, 4=TGCphi, 5=CSCeta, 6=CSCphi, 7=MDT, 10=scatt.ctr (hit_dtyp in ntuple) */
    int*  m_hit_dtyp;
    
    /**  MuonFixedId in integer form (fixedid in ntuple) */
    int*  m_hit_fixedid;
    /**  hit station name index from MuonFixedId (fixedstatname in ntuple) */
    int*  m_hit_fixedstatname;
      
    /** doubletR number for RPC hit [see RpcIdHelper] (hit_dblr in ntuple) */
    double*  m_hit_dblr;
    /** doubletZ number for RPC hit [see RpcIdHelper] (hit_dblz in ntuple) */
    double*  m_hit_dblz;
    /** doubletPhi number for RPC hit [see RpcIdHelper] (hit_dblp in ntuple) */
    double*  m_hit_dblp;

    /** gasGap number for RPC or TGC hit [see RpcIdHelper or TgcIdHelper] (hit_ggap in ntuple) */
    int*  m_hit_ggap;
    /** strip number for RPC or CSC hit [see RpcIdHelper or CscIdHelper] (hit_strp in ntuple) */
    int*  m_hit_strp;
    /** channel number for TGC hit [see TgcIdHelper] (hit_chnl in ntuple) */
    int*  m_hit_chnl;
    /** chamberLayer number for CSC hit [see CscIdHelper] (hit_clyr in ntuple) */
    int*  m_hit_clyr;
    /** wireLayer number for CSC hit [see CscIdHelper] (hit_wlyr in ntuple) */
    int*  m_hit_wlyr;
    /** multiLayer number for MDT hit [see MdtIdHelper] (hit_mlyr in ntuple) */
    int*  m_hit_mlyr;
    /** tubeLayer number for MDT hit [see MdtIdHelper] (hit_tlyr in ntuple) */
    int*  m_hit_tlyr;
    /** tube number for MDT hit [see MdtIdHelper] (hit_tube in ntuple) */
    int*  m_hit_tube;
    
    /** radius of hit (hit_rho in ntuple) */
    double*  m_hit_rho;
    /** phi angle of hit (hit_phi in ntuple) */
    double*  m_hit_phi;
    /** z coord of hit (hit_z in ntuple) */
    double*  m_hit_z;
    /** cot(theta) of hit (hit_cotth in ntuple) */
    double*  m_hit_cotth;
    /** eta angle of hit (hit_eta in ntuple) */
    double*  m_hit_eta;
    /** local x coord of hit (hit_xloc in ntuple) */
    double*  m_hit_xloc;
    /** local y coord of hit (hit_yloc in ntuple) */
    double*  m_hit_yloc;
    /** local z coord of hit (hit_zloc in ntuple) */
    double*  m_hit_zloc;
    /** drift distance to wire for hit [MDT only] (hit_drd in ntuple) */
    double*  m_hit_drd;
    /** hit error (hit_sig in ntuple) */
    double*  m_hit_sig;
    /** hit normalized residual (hit_res in ntuple) */
    double*  m_hit_res;

    /** global x-position of first tube in 1st tubelayer */
    double* m_hit_t0_x;

    /** global y-position of first tube in 1st tubelayer */
    double* m_hit_t0_y;

    /** global z-position of first tube in 1st tubelayer */
    double* m_hit_t0_z;

    /** global x-position of 12th tube in 1st tubelayer */
    double* m_hit_t1_x;

    /** global y-position of 12th tube in 1st tubelayer */
    double* m_hit_t1_y;

    /** global z-position of 12th tube in 1st tubelayer */
    double* m_hit_t1_z;

    /** global x-position of first tube in 3rd tubelayer */
    double* m_hit_t2_x;

    /** global y-position of first tube in 3rd tubelayer */
    double* m_hit_t2_y;

    /** global z-position of first tube in 3rd tubelayer */
    double* m_hit_t2_z;

    /** TSOS track parameter locX or locY */
    double* m_hit_tpar;
        
    /** number of radiation lengths [scatt.ctr only] (hit_x0 in ntuple) */
    int*  m_hit_x0;

    /** TSOS type */
    //double*  m_hit_type; 
    
    /** derivatives */
    /*
    double*  m_hit_deriv_transx;
    double*  m_hit_deriv_transy;
    double*  m_hit_deriv_transz;
    double*  m_hit_deriv_rotx;
    double*  m_hit_deriv_roty;
    double*  m_hit_deriv_rotz;
    */

    const int m_max_nchambers;
    //int       m_nchambers;
    //int       m_nshifts;
    long long int* m_chamberid;
    long long int* m_chamberstatname;
    double*   m_transx;
    double*   m_transy;
    double*   m_transz;
    double*   m_rotx;
    double*   m_roty;
    double*   m_rotz;

    TMatrixD* m_chi2_transx;
    TMatrixD* m_chi2_transy;
    TMatrixD* m_chi2_transz;
    TMatrixD* m_chi2_rotx;
    TMatrixD* m_chi2_roty;
    TMatrixD* m_chi2_rotz;
    TMatrixD* m_chi2_transx_X;
    TMatrixD* m_chi2_transy_X;
    TMatrixD* m_chi2_transz_X;
    TMatrixD* m_chi2_rotx_X;
    TMatrixD* m_chi2_roty_X;
    TMatrixD* m_chi2_rotz_X;
    //mdt
    TMatrixD* m_chi2_transx_mdt;
    TMatrixD* m_chi2_transy_mdt;
    TMatrixD* m_chi2_transz_mdt;
    TMatrixD* m_chi2_rotx_mdt;
    TMatrixD* m_chi2_roty_mdt;
    TMatrixD* m_chi2_rotz_mdt;
    //tgc
    TMatrixD* m_chi2_transx_tgc;
    TMatrixD* m_chi2_transy_tgc;
    TMatrixD* m_chi2_transz_tgc;
    TMatrixD* m_chi2_rotx_tgc;
    TMatrixD* m_chi2_roty_tgc;
    TMatrixD* m_chi2_rotz_tgc;
    //rpc
    TMatrixD* m_chi2_transx_rpc;
    TMatrixD* m_chi2_transy_rpc;
    TMatrixD* m_chi2_transz_rpc;
    TMatrixD* m_chi2_rotx_rpc;
    TMatrixD* m_chi2_roty_rpc;
    TMatrixD* m_chi2_rotz_rpc;
    //csc
    TMatrixD* m_chi2_transx_csc;
    TMatrixD* m_chi2_transy_csc;
    TMatrixD* m_chi2_transz_csc;
    TMatrixD* m_chi2_rotx_csc;
    TMatrixD* m_chi2_roty_csc;
    TMatrixD* m_chi2_rotz_csc;
    //scat
    TMatrixD* m_chi2_transx_scat;
    TMatrixD* m_chi2_transy_scat;
    TMatrixD* m_chi2_transz_scat;
    TMatrixD* m_chi2_rotx_scat;
    TMatrixD* m_chi2_roty_scat;
    TMatrixD* m_chi2_rotz_scat;
    //id
    TMatrixD* m_chi2_transx_id;
    TMatrixD* m_chi2_transy_id;
    TMatrixD* m_chi2_transz_id;
    TMatrixD* m_chi2_rotx_id;
    TMatrixD* m_chi2_roty_id;
    TMatrixD* m_chi2_rotz_id;

    /** TRUTH INFORMATION (only filled in case of single muon MC) */ 
    
    /** generated pt of the truth muon */
    //double m_truth_pt_gen;
    /** pt of the truth muon at the muon spectrometer entrance */
    //double m_truth_pt_msentrance;

    int m_totaltrks;
    int m_totalhits;
    int m_max_hits; // max number of hits on track stored in ntuple

    const int m_csc_nMaxHits;
    StoreGateSvc* m_storeGate;

    double* m_chi2VAlignParamQuality; 
    /*
    double m_trackChi2;       //!> track chi2
    double m_trackChi2Dof;    //!> track chi2 per degree of freedom
    double m_idTrackChi2;     //!> inner detector track chi2
    double m_idTrackChi2Dof;  //!> inner detector track chi2 per degree of freedom
    double m_msTrackChi2;     //!> muon spectrometer track chi2
    double m_msTrackChi2Dof;  //!> muon spectrometer track chi2 per degree of freedom
    double m_trackEta;        //!> track eta
    double m_energyLoss;      //!> energy loss
    int    m_nSctHits;        //!> number silicon hits
    int    m_nPixHits;        //!> number pixel hits
    int    m_nTrtHits;        //!> number TRT hits
    double m_msidPtDiff;      //!> relative difference of pT between MS and ID track 
    double m_idPtAtIP;        //!> momentum of ID track at IP
    double m_idZ0AtIP;        //!> z0 of ID track at IP
    double m_idD0AtIP;        //!> d0 of ID track at IP
    double m_msPtAtIP;        //!> momentum of MS track at IP
    double m_msZ0AtIP;        //!> z0 of MS track at IP
    double m_msD0AtIP;        //!> d0 of MS track at IP
    int    m_nMdtHits;        //!> number MDT hits
    int    m_nRpcPhiHits;     //!> number RPC phi hits
    int    m_nTgcPhiHits;     //!> number TGC phi hits
    */
    //These are CSC specific variables
    /*
    int  m_csc_nphihits;
    int  m_csc_netahits; 
    double m_csc_phi_avg_residual;
    double m_csc_phi_avg_error; 
    double m_csc_eta_avg_residual;
    double m_csc_eta_avg_error; 
    double *m_csc_phi_hit_residual;
    double *m_csc_eta_hit_residual; 
    double *m_csc_phi_hit_error; 
    double *m_csc_eta_hit_error;
    int *m_csc_eta_hit_layer;
    int *m_csc_phi_hit_layer;
    int *m_csc_eta_hit_sector;
    int *m_csc_phi_hit_sector;

    double m_muon_track_sagitta;
    */

    //std::string m_muonContainer;   //!> name of muon container
    
    //bool m_bigNtuple;         //!> make ntuple with all branches
  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_MUONFILLNTUPLETOOL_H
