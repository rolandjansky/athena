/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

//#include "TrkTrackSummaryTool/TrackSummaryTool.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/ResidualPull.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
//#include "MuonHistTools/MuonTrackHistTool.h"

//for truth info
#include "HepMC/GenEvent.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecordCollection.h"

#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"
#include "TrkSurfaces/StraightLineSurface.h"

//#include "TrkAlignGenAlgs/AlignAlg.h"
#include "MuonAlignGenTools/MuonFillNtupleTool.h"

#include "muonEvent/MuonContainer.h"
#include "Particle/TrackParticle.h"
#include "MuonSegment/MuonSegment.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

#include <iostream>

using namespace std;

namespace Muon {

  //________________________________________________________________________
  MuonFillNtupleTool::MuonFillNtupleTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
    : AthAlgTool(type,name,parent)
      // , m_trackSumTool("Trk::TrackSummaryTool", this)
      //    , m_trackHistTool("Muon::MuonTrackHistTool/MuonTrackHistTool")
    , m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
    , m_idTool("MuonCalib::IdToFixedIdTool")
    , m_alignModuleTool("Muon::MuonAlignModuleTool")
    , m_muonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
    , p_muonMgr(0)
    , p_MdtIdHelper(0)
    , p_CscIdHelper(0)
    , p_RpcIdHelper(0)
    , p_TgcIdHelper(0)
    , m_ntuple(0)
    , m_tree(0)
    , m_max_nchambers(10)
    , m_totaltrks(0)
    , m_totalhits(0)
    , m_max_hits(100)
    , m_csc_nMaxHits(40)
    , m_storeGate(0)
    , m_chi2VAlignParamQuality(new double[6])  
  {

    declareInterface<Trk::IFillNtupleTool>(this);
  
    declareProperty("AlignModuleTool",        m_alignModuleTool);
    declareProperty("ResidualPullCalculator", m_pullCalculator);
    declareProperty("MuonIdHelperTool",            m_muonIdHelperTool);
    //declareProperty("MuonContainer",          m_muonContainer="");
    declareProperty("SegmentCollectionName",    m_segmentcollName="MooreSegments");
    //declareProperty("BigNtuple",              m_bigNtuple=false);
    /*
    m_csc_eta_hit_residual= new double[m_csc_nMaxHits];
    m_csc_phi_hit_residual= new double[m_csc_nMaxHits];  
    m_csc_eta_hit_error= new double[m_csc_nMaxHits]; 
    m_csc_phi_hit_error= new double[m_csc_nMaxHits];  
    m_csc_eta_hit_layer=new int[m_csc_nMaxHits];
    m_csc_phi_hit_layer=new int[m_csc_nMaxHits];
    m_csc_eta_hit_sector=new int[m_csc_nMaxHits]; 
    m_csc_phi_hit_sector=new int[m_csc_nMaxHits];
    */

    //  declareProperty("TrackSummaryTool",m_trackSumTool,
    //	  "tool to extract track info");
  
    m_hit_statname=new int[m_max_hits];
    m_hit_stateta =new int[m_max_hits];
    m_hit_statphi =new int[m_max_hits];
    m_hit_statreg =new int[m_max_hits];
    m_hit_tech    =new int[m_max_hits];
    m_hit_dtyp    =new int[m_max_hits];

    m_hit_fixedid =new int[m_max_hits];
    m_hit_fixedstatname=new int[m_max_hits];

    m_hit_dblr    =new double[m_max_hits];
    m_hit_dblz    =new double[m_max_hits];
    m_hit_dblp    =new double[m_max_hits];

    m_hit_ggap    =new int[m_max_hits];
    m_hit_strp    =new int[m_max_hits];
    m_hit_chnl    =new int[m_max_hits];
    m_hit_clyr    =new int[m_max_hits];
    m_hit_wlyr    =new int[m_max_hits];
    m_hit_mlyr    =new int[m_max_hits];
    m_hit_tlyr    =new int[m_max_hits];
    m_hit_tube    =new int[m_max_hits];
    m_hit_rho     =new double[m_max_hits];
    m_hit_phi     =new double[m_max_hits];
    m_hit_z       =new double[m_max_hits];
    m_hit_cotth   =new double[m_max_hits];
    m_hit_eta     =new double[m_max_hits];
    m_hit_xloc    =new double[m_max_hits];
    m_hit_yloc    =new double[m_max_hits];
    m_hit_zloc    =new double[m_max_hits];
    m_hit_drd     =new double[m_max_hits];
    m_hit_sig     =new double[m_max_hits];
    m_hit_res     =new double[m_max_hits];
    m_hit_tpar    =new double[m_max_hits];
    m_hit_x0      =new int[m_max_hits];
    m_hit_t0_x    =new double[m_max_hits];
    m_hit_t0_y    =new double[m_max_hits];
    m_hit_t0_z    =new double[m_max_hits];
    m_hit_t1_x    =new double[m_max_hits];
    m_hit_t1_y    =new double[m_max_hits];
    m_hit_t1_z    =new double[m_max_hits];
    m_hit_t2_x    =new double[m_max_hits];
    m_hit_t2_y    =new double[m_max_hits];
    m_hit_t2_z    =new double[m_max_hits];

    //m_hit_type      =new double[m_max_hits];
    /*
      m_hit_deriv_transx =new double[m_max_hits];
      m_hit_deriv_transy =new double[m_max_hits];
      m_hit_deriv_transz =new double[m_max_hits];
      m_hit_deriv_rotx   =new double[m_max_hits];
      m_hit_deriv_roty   =new double[m_max_hits];
      m_hit_deriv_rotz   =new double[m_max_hits];
    */

    m_chi2_transx = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz   = new TMatrixD(1,m_max_nchambers);
    m_chi2_transx_X = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy_X = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz_X = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx_X   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty_X   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz_X   = new TMatrixD(1,m_max_nchambers);
    //mdt
    m_chi2_transx_mdt = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy_mdt = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz_mdt = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx_mdt   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty_mdt   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz_mdt   = new TMatrixD(1,m_max_nchambers);
    //tgc
    m_chi2_transx_tgc = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy_tgc = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz_tgc = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx_tgc   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty_tgc   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz_tgc   = new TMatrixD(1,m_max_nchambers);
    //rpc
    m_chi2_transx_rpc = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy_rpc = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz_rpc = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx_rpc   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty_rpc   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz_rpc   = new TMatrixD(1,m_max_nchambers);
    //csc
    m_chi2_transx_csc = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy_csc = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz_csc = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx_csc   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty_csc   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz_csc   = new TMatrixD(1,m_max_nchambers);
    //id
    m_chi2_transx_id = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy_id = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz_id = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx_id   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty_id   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz_id   = new TMatrixD(1,m_max_nchambers);
    //scat
    m_chi2_transx_scat = new TMatrixD(1,m_max_nchambers);
    m_chi2_transy_scat = new TMatrixD(1,m_max_nchambers);
    m_chi2_transz_scat = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotx_scat   = new TMatrixD(1,m_max_nchambers);
    m_chi2_roty_scat   = new TMatrixD(1,m_max_nchambers);
    m_chi2_rotz_scat   = new TMatrixD(1,m_max_nchambers);


    m_chamberid       = new long long int[m_max_nchambers];
    m_chamberstatname = new long long int[m_max_nchambers];
    m_transx          = new double[m_max_nchambers];
    m_transy          = new double[m_max_nchambers];
    m_transz          = new double[m_max_nchambers];
    m_rotx            = new double[m_max_nchambers];
    m_roty            = new double[m_max_nchambers];
    m_rotz            = new double[m_max_nchambers];
  }

  //________________________________________________________________________
  MuonFillNtupleTool::~MuonFillNtupleTool()
  { 
    ATH_MSG_DEBUG("in destructor of MuonFillNtupleTool");
    /*
    delete [] m_csc_eta_hit_residual;
    delete [] m_csc_phi_hit_residual;
    delete [] m_csc_eta_hit_error;
    delete [] m_csc_phi_hit_error;
    delete []  m_csc_eta_hit_layer;
    delete []  m_csc_phi_hit_layer;
    delete []  m_csc_eta_hit_sector;
    delete []  m_csc_phi_hit_sector;
    */
    delete [] m_chi2VAlignParamQuality;

    delete [] m_hit_statname;
    delete [] m_hit_stateta;
    delete [] m_hit_statphi;
    delete [] m_hit_statreg;
    delete [] m_hit_tech;
    delete [] m_hit_dtyp;

    delete [] m_hit_fixedstatname;
    delete [] m_hit_fixedid;

    delete [] m_hit_dblr;
    delete [] m_hit_dblz;
    delete [] m_hit_dblp;

    delete [] m_hit_ggap;
    delete [] m_hit_strp;
    delete [] m_hit_chnl;
    delete [] m_hit_clyr;
    delete [] m_hit_wlyr;
    delete [] m_hit_mlyr;
    delete [] m_hit_tlyr;
    delete [] m_hit_tube;
    delete [] m_hit_rho;
    delete [] m_hit_phi;
    delete [] m_hit_z;
    delete [] m_hit_cotth;
    delete [] m_hit_eta;
    delete [] m_hit_xloc;
    delete [] m_hit_yloc;
    delete [] m_hit_zloc;
    delete [] m_hit_drd;
    delete [] m_hit_sig;
    delete [] m_hit_res;
    delete [] m_hit_tpar;
    delete [] m_hit_x0;
    delete [] m_hit_t0_x;
    delete [] m_hit_t0_y;
    delete [] m_hit_t0_z;
    delete [] m_hit_t1_x;
    delete [] m_hit_t1_y;
    delete [] m_hit_t1_z;
    delete [] m_hit_t2_x;
    delete [] m_hit_t2_y;
    delete [] m_hit_t2_z;

    //delete [] m_hit_type;

    /*
      delete [] m_hit_deriv_transx;
      delete [] m_hit_deriv_transy;
      delete [] m_hit_deriv_transz;
      delete [] m_hit_deriv_rotx;
      delete [] m_hit_deriv_roty;
      delete [] m_hit_deriv_rotz;
    */

    delete m_chi2_transx;
    delete m_chi2_transy;
    delete m_chi2_transz;
    delete m_chi2_rotx;
    delete m_chi2_roty;
    delete m_chi2_rotz;
    delete m_chi2_transx_X;
    delete m_chi2_transy_X;
    delete m_chi2_transz_X;
    delete m_chi2_rotx_X;
    delete m_chi2_roty_X;
    delete m_chi2_rotz_X;
    //mdt
    delete m_chi2_transx_mdt;
    delete m_chi2_transy_mdt;
    delete m_chi2_transz_mdt;
    delete m_chi2_rotx_mdt;
    delete m_chi2_roty_mdt;
    delete m_chi2_rotz_mdt;
    //tgc
    delete m_chi2_transx_tgc;
    delete m_chi2_transy_tgc;
    delete m_chi2_transz_tgc;
    delete m_chi2_rotx_tgc;
    delete m_chi2_roty_tgc;
    delete m_chi2_rotz_tgc;
    //rpc
    delete m_chi2_transx_rpc;
    delete m_chi2_transy_rpc;
    delete m_chi2_transz_rpc;
    delete m_chi2_rotx_rpc;
    delete m_chi2_roty_rpc;
    delete m_chi2_rotz_rpc;
    //csc
    delete m_chi2_transx_csc;
    delete m_chi2_transy_csc;
    delete m_chi2_transz_csc;
    delete m_chi2_rotx_csc;
    delete m_chi2_roty_csc;
    delete m_chi2_rotz_csc;
    //id
    delete m_chi2_transx_id;
    delete m_chi2_transy_id;
    delete m_chi2_transz_id;
    delete m_chi2_rotx_id;
    delete m_chi2_roty_id;
    delete m_chi2_rotz_id;
    //scat
    delete m_chi2_transx_scat;
    delete m_chi2_transy_scat;
    delete m_chi2_transz_scat;
    delete m_chi2_rotx_scat;
    delete m_chi2_roty_scat;
    delete m_chi2_rotz_scat;

    delete [] m_chamberid;
    delete [] m_chamberstatname;
    delete [] m_transx;
    delete [] m_transy;
    delete [] m_transz;
    delete [] m_rotx;
    delete [] m_roty;
    delete [] m_rotz;

  }
  //________________________________________________________________________
  StatusCode MuonFillNtupleTool::initialize() 
  {

    ATH_MSG_DEBUG("initialize() of MuonFillNtupleTool");

    // set up MuonIdHelperTool
    if (m_muonIdHelperTool.retrieve().isSuccess()) 
      ATH_MSG_INFO("Retrieved  "<<m_muonIdHelperTool);
    else {
      ATH_MSG_FATAL("Could not get MuonIdHelperTool!");
      return StatusCode::FAILURE;
    }    


    // get TrackSummaryTool
    /*if ( m_trackSumTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_trackSumTool);
      return StatusCode::FAILURE;
      } 
      else
      ATH_MSG_INFO("Retrieved tool " << m_trackSumTool);
    */

    // check StoreGate service available
    if (StatusCode::SUCCESS != service("StoreGateSvc", m_storeGate)) {
      ATH_MSG_FATAL("StoreGate service not found");
      return StatusCode::FAILURE;
    }
  
    // if ( m_trackHistTool.retrieve().isFailure() ) {
    //   ATH_MSG_FATAL("Could not get " << m_trackHistTool);
    //   return StatusCode::FAILURE;
    // }
    // else
    //   ATH_MSG_INFO("Retrieved " << m_trackHistTool);

    if (m_pullCalculator.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_pullCalculator);
    else{
      ATH_MSG_FATAL("Could not get " << m_pullCalculator);
      return StatusCode::FAILURE;
    }
    /*
    if (m_helperTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_helperTool);
    else{
      ATH_MSG_FATAL("Could not get " << m_helperTool); 
      return StatusCode::FAILURE;
    }
    */
    if (m_idTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_idTool);
    else{
      ATH_MSG_FATAL("Could not get " << m_idTool ); 
      return StatusCode::FAILURE;
    }

    StoreGateSvc* detStore=0;
    if ( (serviceLocator()->service("DetectorStore", detStore)).isSuccess() ) {
      if ( detStore->retrieve( p_muonMgr ).isSuccess() ) {
	p_MdtIdHelper = p_muonMgr->mdtIdHelper();
	p_CscIdHelper = p_muonMgr->cscIdHelper();
	p_RpcIdHelper = p_muonMgr->rpcIdHelper();
	p_TgcIdHelper = p_muonMgr->tgcIdHelper();
      } 
      else 
	ATH_MSG_ERROR( "Cannot retrieve MuonDetDescrMgr " );    
    } 
    else 
      ATH_MSG_ERROR( "MuonDetDescrMgr not found in DetectorStore " );
     
    if (m_alignModuleTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_alignModuleTool ); 
    else{
      ATH_MSG_FATAL("Could not get " << m_alignModuleTool ); 
      return StatusCode::FAILURE;
    }
  
    //initialize variables to be written in ntuple
    for(int i = 0; i<m_max_hits ; i++) {
      m_hit_statname[i] = -999;
      m_hit_stateta[i]  = -999;
      m_hit_statphi[i]  = -999;
      m_hit_statreg[i]  = -999;
      m_hit_tech[i]     = -999;
      m_hit_dtyp[i]     = -999;
    
      m_hit_fixedid[i]  = -999;
      m_hit_fixedstatname[i] = -999;
    
      m_hit_dblr[i]     = -999.;
      m_hit_dblz[i]     = -999.;
      m_hit_dblp[i]     = -999.;
     
      m_hit_ggap[i]     = -999;
      m_hit_strp[i]     = -999;
      m_hit_chnl[i]     = -999;
      m_hit_clyr[i]     = -999;
      m_hit_wlyr[i]     = -999;
      m_hit_mlyr[i]     = -999;
      m_hit_tlyr[i]     = -999;
      m_hit_tube[i]     = -999;
      m_hit_rho[i]      = -999.;
      m_hit_phi[i]      = -999.;
      m_hit_z[i]        = -999.;
      m_hit_cotth[i]    = -999.;
      m_hit_eta[i]      = -999.;
      m_hit_xloc[i]     = -999.;
      m_hit_yloc[i]     = -999.;
      m_hit_zloc[i]     = -999.;
      m_hit_drd[i]      = -999.;
      m_hit_sig[i]      = -999.;
      m_hit_res[i]      = -999.;
      m_hit_tpar[i]     = -999.;
      m_hit_x0[i]       = -999;
      m_hit_t0_x[i] = -999;
      m_hit_t0_y[i] = -999;
      m_hit_t0_z[i] = -999;
      m_hit_t1_x[i] = -999;
      m_hit_t1_y[i] = -999;
      m_hit_t1_z[i] = -999;
      m_hit_t2_x[i] = -999;
      m_hit_t2_y[i] = -999;
      m_hit_t2_z[i] = -999;

      //m_hit_type[i] = -999;

      /*
	m_hit_deriv_transx[i] = -999.;
	m_hit_deriv_transy[i] = -999.;
	m_hit_deriv_transz[i] = -999.;
	m_hit_deriv_rotx[i]   = -999.;
	m_hit_deriv_roty[i]   = -999.;
	m_hit_deriv_rotz[i]   = -999.;
      */

    }

    return StatusCode::SUCCESS;

  }

  //________________________________________________________________________
  StatusCode MuonFillNtupleTool::fillNtuple()
  {
    ATH_MSG_DEBUG("in fillNtuple");
    if (m_ntuple && m_tree) {
      m_ntuple->cd();
      ATH_MSG_DEBUG("writing tree");
      m_tree->Write();
      ATH_MSG_DEBUG("done");
    }
    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  StatusCode MuonFillNtupleTool::finalize() 
  {

    ATH_MSG_DEBUG("finalize() of MuonFillNtupleTool");
 
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  void MuonFillNtupleTool::dumpTrack(int /*itrk*/, const Trk::AlignTrack* /*alignTrack*/) 
  {
    ATH_MSG_ERROR("migrate MuonFillNtupleTool::dumpTrack to Eigen");
    return;

    /*
    ATH_MSG_DEBUG("In processTrack()");

    if (!m_tree && m_ntuple) {
      m_ntuple->cd();
      m_tree = new TTree("MuonAlign", "Muon Alignment Ntuple");
      
      m_tree->Branch("run",            &m_runNumber, "run/I");
      m_tree->Branch("evt",            &m_evtNumber, "evt/I");
      m_tree->Branch("nMuonOutliers",  &m_nMuonOutliers, "nMuonOutliers/I");
      m_tree->Branch("nTgcOutliers",   &m_nTgcOutliers,  "nTgcOutliers/I");

      m_tree->Branch("CscNumPhiHits",    &m_csc_nphihits,         "CscNumPhiHits/I");
      m_tree->Branch("CscPhiResidual",    m_csc_phi_hit_residual, "CscPhiResidual[CscNumPhiHits]/D");
      m_tree->Branch("CscPhiError",       m_csc_phi_hit_error,    "CscPhiError[CscNumPhiHits]/D");
      m_tree->Branch("CscPhiLayer",       m_csc_phi_hit_layer,    "CscPhiLayer[CscNumPhiHits]/I");
      m_tree->Branch("CscPhiSector",      m_csc_phi_hit_sector,   "CscPhiSector[CscNumPhiHits]/I");
      m_tree->Branch("CscNumEtaHits",    &m_csc_netahits,         "CscNumEtaHits/I");
      m_tree->Branch("CscEtaResidual",    m_csc_eta_hit_residual, "CscEtaResidual[CscNumEtaHits]/D");
      m_tree->Branch("CscEtaError",       m_csc_eta_hit_error,    "CscEtaError[CscNumEtaHits]/D");
      m_tree->Branch("CscEtaLayer",       m_csc_eta_hit_layer,    "CscEtaLayer[CscNumEtaHits]/I");
      m_tree->Branch("CscEtaSector",      m_csc_eta_hit_sector,   "CscEtaSector[CscNumEtaHits]/I");
      m_tree->Branch("CscAvgPhiResidual",&m_csc_phi_avg_residual, "CscAvgPhiResidual/D");
      m_tree->Branch("CscAvgPhiError",   &m_csc_phi_avg_error,    "CscAvgPhiError/D");
      m_tree->Branch("CscAvgEtaResidual",&m_csc_eta_avg_residual, "CscAvgEtaResidual/D");
      m_tree->Branch("CscAvgEtaError",   &m_csc_eta_avg_error,    "CscAvgEtaError/D");
      m_tree->Branch("MuonSagitta",      &m_muon_track_sagitta,   "MuonSagitta/D");


      if (m_bigNtuple) {
	m_tree->Branch("etah",       &m_etah, "etah/I");
	m_tree->Branch("phih",       &m_phih, "phih/I");
	m_tree->Branch("mdth",       &m_mdth, "mdth/I");
	m_tree->Branch("cscetah",    &m_cscetah, "cscetah/I");
	m_tree->Branch("cscphih",    &m_cscphih, "cscphih/I");
	m_tree->Branch("rpcetah",    &m_rpcetah, "rpcetah/I");
	m_tree->Branch("rpcphih",    &m_rpcphih, "rpcphih/I");
	m_tree->Branch("tgcetah",    &m_tgcetah, "tgcetah/I");
	m_tree->Branch("tgcphih",    &m_tgcphih, "tgcphih/I");
	m_tree->Branch("stati",      &m_stati, "stati/I");
	m_tree->Branch("statr",      &m_statr, "statr/I");
    
	m_tree->Branch("xvtxr",      &m_xvtxr, "xvtxr/D");
	m_tree->Branch("yvtxr",      &m_yvtxr, "yvtxr/D");
	m_tree->Branch("zvtxr",      &m_zvtxr, "zvtxr/D");
	m_tree->Branch("a0r",        &m_a0r, "a0r/D");
	m_tree->Branch("z0r",        &m_z0r, "z0r/D");
	m_tree->Branch("phir",       &m_phir, "phir/D");
	m_tree->Branch("cotthr",     &m_cotthr, "cotthr/D");
	m_tree->Branch("ptir",       &m_ptir, "ptir/D");
	m_tree->Branch("etar",       &m_etar, "etar/D");
	m_tree->Branch("chi2",       &m_chi2, "chi2/D");
	m_tree->Branch("chi2pr",     &m_chi2pr, "chi2pr/D");
	m_tree->Branch("vertexx",    &m_vertexx, "vertexx/D");
	m_tree->Branch("vertexy",    &m_vertexy, "vertexy/D");
	m_tree->Branch("vertexz",    &m_vertexz, "vertexz/D");
    
	m_tree->Branch("covr11",     &m_covr11, "covr11/D");
	m_tree->Branch("covr12",     &m_covr12, "covr12/D");
	m_tree->Branch("covr22",     &m_covr22, "covr22/D");
	m_tree->Branch("covr13",     &m_covr13, "covr13/D");
	m_tree->Branch("covr23",     &m_covr23, "covr23/D");
	m_tree->Branch("covr33",     &m_covr33, "covr33/D");
	m_tree->Branch("covr14",     &m_covr14, "covr14/D");
	m_tree->Branch("covr24",     &m_covr24, "covr24/D");
	m_tree->Branch("covr34",     &m_covr34, "covr34/D");
	m_tree->Branch("covr44",     &m_covr44, "covr44/D");
	m_tree->Branch("covr15",     &m_covr15, "covr15/D");
	m_tree->Branch("covr25",     &m_covr25, "covr25/D");
	m_tree->Branch("covr35",     &m_covr35, "covr35/D");
	m_tree->Branch("covr45",     &m_covr45, "covr45/D");
	m_tree->Branch("covr55",     &m_covr55, "covr55/D");
			       
	m_tree->Branch("nhits",      &m_nhits,       "nhits/I");
	m_tree->Branch("statname",    m_hit_statname,"statname[nhits]/I");
	m_tree->Branch("stateta",     m_hit_stateta, "stateta[nhits]/I");
	m_tree->Branch("statphi",     m_hit_statphi, "statphi[nhits]/I");
    
	m_tree->Branch("statreg",     m_hit_statreg, "statreg[nhits]/I");
	m_tree->Branch("tech",        m_hit_tech,    "tech[nhits]/I");
	m_tree->Branch("dtyp",        m_hit_dtyp,    "dtyp[nhits]/I");
    
	m_tree->Branch("fixedstatname", m_hit_fixedstatname,"fixedstatname[nhits]/I");
	m_tree->Branch("fixedid",     m_hit_fixedid, "fixedid[nhits]/I");
    
	m_tree->Branch("dblr",        m_hit_dblr,    "dblr[nhits]/D");
	m_tree->Branch("dblz",        m_hit_dblz,    "dblz[nhits]/D");
	m_tree->Branch("dblp",        m_hit_dblp,    "dblp[nhits]/D");
    
	m_tree->Branch("ggap",        m_hit_ggap,    "ggap[nhits]/I");
	m_tree->Branch("strp",        m_hit_strp,    "strp[nhits]/I");
	m_tree->Branch("chnl",        m_hit_chnl,    "chnl[nhits]/I");
	m_tree->Branch("clyr",        m_hit_clyr,    "clyr[nhits]/I");
	m_tree->Branch("wlyr",        m_hit_wlyr,    "wlyr[nhits]/I");
	m_tree->Branch("mlyr",        m_hit_mlyr,    "mlyr[nhits]/I");
	m_tree->Branch("tlyr",        m_hit_tlyr,    "tlyr[nhits]/I");
	m_tree->Branch("tube",        m_hit_tube,    "tube[nhits]/I");
    
	m_tree->Branch("rho",         m_hit_rho,     "rho[nhits]/D");
	m_tree->Branch("phi",         m_hit_phi,     "phi[nhits]/D");
	m_tree->Branch("z",           m_hit_z,       "z[nhits]/D");
	m_tree->Branch("cotth",       m_hit_cotth,   "cotth[nhits]/D");
	m_tree->Branch("eta",         m_hit_eta,     "eta[nhits]/D");
	m_tree->Branch("xloc",        m_hit_xloc,    "xloc[nhits]/D");
	m_tree->Branch("yloc",        m_hit_yloc,    "yloc[nhits]/D");
	m_tree->Branch("zloc",        m_hit_zloc,    "zloc[nhits]/D");
	m_tree->Branch("drd",         m_hit_drd,     "drd[nhits]/D");
	m_tree->Branch("sig",         m_hit_sig,     "sig[nhits]/D");
	m_tree->Branch("res",         m_hit_res,     "res[nhits]/D");
	m_tree->Branch("tpar",        m_hit_tpar,    "tpar[nhits]/D");
	m_tree->Branch("x0",          m_hit_x0,      "x0[nhits]/I");  
	m_tree->Branch("t0_xglob",    m_hit_t0_x,    "t0_xglob[nhits]/D");
	m_tree->Branch("t0_yglob",    m_hit_t0_y,    "t0_yglob[nhits]/D");
	m_tree->Branch("t0_zglob",    m_hit_t0_z,    "t0_zglob[nhits]/D");
	m_tree->Branch("t1_xglob",    m_hit_t1_x,    "t1_xglob[nhits]/D");
	m_tree->Branch("t1_yglob",    m_hit_t1_y,    "t1_yglob[nhits]/D");
	m_tree->Branch("t1_zglob",    m_hit_t1_z,    "t1_zglob[nhits]/D");
	m_tree->Branch("t2_xglob",    m_hit_t2_x,    "t2_xglob[nhits]/D");
	m_tree->Branch("t2_yglob",    m_hit_t2_y,    "t2_yglob[nhits]/D");
	m_tree->Branch("t2_zglob",    m_hit_t2_z,    "t2_zglob[nhits]/D");
    
	//m_tree->Branch("type",        m_hit_type,    "type[nhits]/I");
      }

      
	// m_tree->Branch("deriv_transx", m_hit_deriv_transx, "deriv_transx[nhits]/D");
	// m_tree->Branch("deriv_transy", m_hit_deriv_transy, "deriv_transy[nhits]/D");
	// m_tree->Branch("deriv_transz", m_hit_deriv_transz, "deriv_transz[nhits]/D");
	// m_tree->Branch("deriv_rotx",   m_hit_deriv_rotx,   "deriv_rotx[nhits]/D");
	// m_tree->Branch("deriv_roty",   m_hit_deriv_roty,   "deriv_roty[nhits]/D");
	// m_tree->Branch("deriv_rotz",   m_hit_deriv_rotz,   "deriv_rotz[nhits]/D");
      

      //stringstream brname; brname<<"["<<m_nshifts+1<<"][nchambers]/D";
      m_tree->Branch("nchambers",     &m_nchambers,      "nchambers/I");
      m_tree->Branch("chamberid",      m_chamberid,      "chamberid[nchambers]/L");
      m_tree->Branch("chamberstatname",m_chamberstatname,"chamberstatname[nchambers]/L");
      m_tree->Branch("transx",         m_transx,  "transx[nchambers]/D");
      m_tree->Branch("transy",         m_transy,  "transy[nchambers]/D");
      m_tree->Branch("transz",         m_transz,  "transz[nchambers]/D");
      m_tree->Branch("rotx",           m_rotx,    "rotx[nchambers]/D");
      m_tree->Branch("roty",           m_roty,    "roty[nchambers]/D");
      m_tree->Branch("rotz",           m_rotz,    "rotz[nchambers]/D");

      m_tree->Branch("chi2_transx",  &m_chi2_transx);
      m_tree->Branch("chi2_transy",  &m_chi2_transy);
      m_tree->Branch("chi2_transz",  &m_chi2_transz);
      m_tree->Branch("chi2_rotx",    &m_chi2_rotx);  
      m_tree->Branch("chi2_roty",    &m_chi2_roty);  
      m_tree->Branch("chi2_rotz",    &m_chi2_rotz);
      m_tree->Branch("chi2_transx_X",  &m_chi2_transx_X);
      m_tree->Branch("chi2_transy_X",  &m_chi2_transy_X);
      m_tree->Branch("chi2_transz_X",  &m_chi2_transz_X);
      m_tree->Branch("chi2_rotx_X",    &m_chi2_rotx_X);  
      m_tree->Branch("chi2_roty_X",    &m_chi2_roty_X);  
      m_tree->Branch("chi2_rotz_X",    &m_chi2_rotz_X);  
      m_tree->Branch("chi2VAlignParamQuality", m_chi2VAlignParamQuality, "chi2VAlignParamQuality[6]/D");
  
      if (m_bigNtuple) {
	//mdt
	m_tree->Branch("chi2_transx_mdt",  &m_chi2_transx_mdt);
	m_tree->Branch("chi2_transy_mdt",  &m_chi2_transy_mdt);
	m_tree->Branch("chi2_transz_mdt",  &m_chi2_transz_mdt);
	m_tree->Branch("chi2_rotx_mdt",    &m_chi2_rotx_mdt);  
	m_tree->Branch("chi2_roty_mdt",    &m_chi2_roty_mdt);  
	m_tree->Branch("chi2_rotz_mdt",    &m_chi2_rotz_mdt);
	//tgc
	m_tree->Branch("chi2_transx_tgc",  &m_chi2_transx_tgc);
	m_tree->Branch("chi2_transy_tgc",  &m_chi2_transy_tgc);
	m_tree->Branch("chi2_transz_tgc",  &m_chi2_transz_tgc);
	m_tree->Branch("chi2_rotx_tgc",    &m_chi2_rotx_tgc);  
	m_tree->Branch("chi2_roty_tgc",    &m_chi2_roty_tgc);  
	m_tree->Branch("chi2_rotz_tgc",    &m_chi2_rotz_tgc);
	//rpc
	m_tree->Branch("chi2_transx_rpc",  &m_chi2_transx_rpc);
	m_tree->Branch("chi2_transy_rpc",  &m_chi2_transy_rpc);
	m_tree->Branch("chi2_transz_rpc",  &m_chi2_transz_rpc);
	m_tree->Branch("chi2_rotx_rpc",    &m_chi2_rotx_rpc);  
	m_tree->Branch("chi2_roty_rpc",    &m_chi2_roty_rpc);  
	m_tree->Branch("chi2_rotz_rpc",    &m_chi2_rotz_rpc);
	//csc
	m_tree->Branch("chi2_transx_csc",  &m_chi2_transx_csc);
	m_tree->Branch("chi2_transy_csc",  &m_chi2_transy_csc);
	m_tree->Branch("chi2_transz_csc",  &m_chi2_transz_csc);
	m_tree->Branch("chi2_rotx_csc",    &m_chi2_rotx_csc);  
	m_tree->Branch("chi2_roty_csc",    &m_chi2_roty_csc);  
	m_tree->Branch("chi2_rotz_csc",    &m_chi2_rotz_csc);
	//id
	m_tree->Branch("chi2_transx_id",  &m_chi2_transx_id);
	m_tree->Branch("chi2_transy_id",  &m_chi2_transy_id);
	m_tree->Branch("chi2_transz_id",  &m_chi2_transz_id);
	m_tree->Branch("chi2_rotx_id",    &m_chi2_rotx_id);  
	m_tree->Branch("chi2_roty_id",    &m_chi2_roty_id);  
	m_tree->Branch("chi2_rotz_id",    &m_chi2_rotz_id);
	//scat
	m_tree->Branch("chi2_transx_scat",  &m_chi2_transx_scat);
	m_tree->Branch("chi2_transy_scat",  &m_chi2_transy_scat);
	m_tree->Branch("chi2_transz_scat",  &m_chi2_transz_scat);
	m_tree->Branch("chi2_rotx_scat",    &m_chi2_rotx_scat);  
	m_tree->Branch("chi2_roty_scat",    &m_chi2_roty_scat);  
	m_tree->Branch("chi2_rotz_scat",    &m_chi2_rotz_scat);
  
	m_tree->Branch("trackChi2",      &m_trackChi2,      "trackChi2/D");
	m_tree->Branch("trackChi2Dof",   &m_trackChi2Dof,   "trackChi2Dof/D");
	m_tree->Branch("idTrackChi2",    &m_idTrackChi2,    "idTrackChi2/D");
	m_tree->Branch("idTrackChi2Dof", &m_idTrackChi2Dof, "idTrackChi2Dof/D");
	m_tree->Branch("msTrackChi2",    &m_msTrackChi2,    "msTrackChi2/D");
	m_tree->Branch("msTrackChi2Dof", &m_msTrackChi2Dof, "msTrackChi2Dof/D");
	m_tree->Branch("trackEta",       &m_trackEta,       "trackEta/D");
	m_tree->Branch("energyLoss",     &m_energyLoss,     "energyLoss/D");
	m_tree->Branch("nSctHits",       &m_nSctHits,       "nSctHits/I");
	m_tree->Branch("nPixHits",       &m_nPixHits,       "nPixHits/I");
	m_tree->Branch("nTrtHits",       &m_nTrtHits,       "nTrtHits/I");
	m_tree->Branch("msidPtDiff",     &m_msidPtDiff,     "msidPtDiff/D");
	m_tree->Branch("idPtAtIP",       &m_idPtAtIP,        "idPtAtIP/D");
	m_tree->Branch("idZ0AtIP",       &m_idZ0AtIP,        "idZ0AtIP/D");
	m_tree->Branch("idD0AtIP",       &m_idD0AtIP,        "idD0AtIP/D");
	m_tree->Branch("msPtAtIP",       &m_msPtAtIP,        "msPtAtIP/D");
	m_tree->Branch("msZ0AtIP",       &m_msZ0AtIP,        "msZ0AtIP/D");
	m_tree->Branch("msD0AtIP",       &m_msD0AtIP,        "msD0AtIP/D");
	m_tree->Branch("nMdtHits",       &m_nMdtHits,        "nMdtHits/I");
	m_tree->Branch("nRpcPhiHits",    &m_nRpcPhiHits,     "nRpcPhiHits/I");
	m_tree->Branch("nTgcPhiHits",    &m_nTgcPhiHits,     "nTgcPhiHits/I");
    
	m_tree->Branch("truth_pt_gen",        &m_truth_pt_gen,        "truth_pt_gen/D");
	m_tree->Branch("truth_pt_msentrance", &m_truth_pt_msentrance, "truth_pt_msentrance/D");
      }
    }

    //These are CSC specific stuffs and muon track sagitta calculations
    for(int il=0;il<m_csc_nMaxHits;il++){
      m_csc_phi_hit_residual[il]=-100000.;
      m_csc_phi_hit_error[il]=1000.;
      m_csc_eta_hit_residual[il]=-100000.;
      m_csc_eta_hit_error[il]=1000.;
      m_csc_eta_hit_layer[il]=-1000; 
      m_csc_phi_hit_layer[il]=-1000; 
      m_csc_eta_hit_sector[il]=-1000; 
      m_csc_phi_hit_sector[il]=-1000; 

    }
    m_csc_nphihits=0;
    m_csc_netahits=0;
    m_csc_phi_avg_residual=0.;
    m_csc_phi_avg_error=0.;
    m_csc_eta_avg_residual=0.;
    m_csc_eta_avg_error=0.;
    const Trk::SegmentCollection *m_collecsegments;
    if( (m_storeGate->retrieve(m_collecsegments, m_segmentcollName)).isFailure()){ 
      ATH_MSG_FATAL(" could not open segments collection : MooreSegments");
      return;
    } else 
      ATH_MSG_DEBUG("Retrieved Segment Collection: "<<m_segmentcollName); 

    m_muon_track_sagitta=-9999.;
    int issegmentontrack=-1;
    int seg_match_rot_csc=1000;
    int seg_match_rot_m_mdt=1000;
    int seg_match_rot_o_mdt=1000;
    const Trk::Segment* seg_matched_m_mdt(0);
    const Trk::Segment* seg_matched_o_mdt(0);
    const Trk::Segment* seg_matched_csc(0);
    for (Trk::SegmentCollection::const_iterator it_seg=m_collecsegments->begin();it_seg!=m_collecsegments->end(); it_seg++) {
      const Trk::Segment* iSegment = *it_seg;
      issegmentontrack=m_mutagedmhelpertool->IsThisSegmentInThisTrack(iSegment, alignTrack);
      if(issegmentontrack<=1){
	ATH_MSG_DEBUG("Segment Author: "<<iSegment->author());
	//if (msgLvl(MSG::DEBUG)) iSegment->dump(msg());
	ATH_MSG_DEBUG("Number of ROT notshared: "<<issegmentontrack);
	std::string seg_station_name=m_mutagedmhelpertool->SegmentAssociatedStationName(iSegment);
	int seg_station_level=m_mutagedmhelpertool->StationLevel(seg_station_name);
	ATH_MSG_DEBUG("Associated Station Name: "<<seg_station_name);
	ATH_MSG_DEBUG("Associated Station Level: "<<seg_station_level);
	if(seg_station_name=="CSL"||seg_station_name=="CSS"){
	  if(issegmentontrack<seg_match_rot_csc){
	    seg_match_rot_csc=issegmentontrack;
	    seg_matched_csc=iSegment;
	  }
	}else if(seg_station_name=="EMS"||seg_station_name=="EML"){
	  if(issegmentontrack<seg_match_rot_m_mdt){
	    seg_match_rot_m_mdt=issegmentontrack;
	    seg_matched_m_mdt=iSegment;
	  }
	}else if(seg_station_name=="EOS"||seg_station_name=="EOL"){
	  if(issegmentontrack<seg_match_rot_o_mdt){
	    seg_match_rot_o_mdt=issegmentontrack;
	    seg_matched_o_mdt=iSegment;
	  }
	}else
	  continue;  
      }
    }
    
    const Muon::MuonSegment* m_seg_matched_m_mdt=(seg_matched_m_mdt)?
      dynamic_cast<const Muon::MuonSegment*>(seg_matched_m_mdt):0;
    
    if (m_seg_matched_m_mdt && seg_matched_o_mdt && seg_matched_csc) {
      const Amg::Vector3D csc_gp=seg_matched_csc->globalPosition();
      const Amg::Vector3D mdt_m_gp=seg_matched_m_mdt->globalPosition();
      const Amg::Vector3D mdt_o_gp=seg_matched_o_mdt->globalPosition();
      HepGeom::Vector3D<double> mdt_m_tubedirection;
      const std::vector<const  Trk::RIO_OnTrack*> pmdt_m_RIOSet = m_seg_matched_m_mdt->containedROTs();
      for (vector<const  Trk::RIO_OnTrack*>::const_iterator it_m_mdt=pmdt_m_RIOSet.begin();it_m_mdt!=pmdt_m_RIOSet.end(); it_m_mdt++) {
	const Trk::RIO_OnTrack* rot_m_mdt = (*it_m_mdt);
	const Identifier& id_m_mdt = rot_m_mdt->identify();
	if (p_MdtIdHelper->is_mdt(id_m_mdt)) {
	  if (!p_MdtIdHelper->valid(id_m_mdt)) {
	    ATH_MSG_WARNING("Skipping tube with invalid identifier");
	    mdt_m_tubedirection= HepGeom::Vector3D<double>(1.,0.,0.);
	  }
	  const Trk::PrepRawData* pPrepRawData_m_mdt =  rot_m_mdt->prepRawData();
	  const Trk::Surface* pSurface_m_mdt =
	    &(pPrepRawData_m_mdt->detectorElement()->surface(pPrepRawData_m_mdt->identify()));
	  const Trk::StraightLineSurface* pStraightLineSurface_m_mdt =
	    dynamic_cast<const Trk::StraightLineSurface*> (pSurface_m_mdt);
	  if (!pStraightLineSurface_m_mdt) {
	    ATH_MSG_WARNING("Skipping tube with invalid surface");
	    mdt_m_tubedirection=HepGeom::Vector3D<double>(1.,0.,0.);
	  }
	  else {
	    mdt_m_tubedirection=(pStraightLineSurface_m_mdt->transform()).getRotation().colZ();
	    break;
	  }
	}
      }
      
      TVector3 mdt_tubedirection(mdt_m_tubedirection[0], mdt_m_tubedirection[1],mdt_m_tubedirection[2]);
      //csc_gp=m_mutagedmhelpertool->GetPoint(seg_matched_csc);
      //mdt_m_gp=m_mutagedmhelpertool->GetPoint(seg_matched_m_mdt);
      //mdt_o_gp=m_mutagedmhelpertool->GetPoint(seg_matched_o_mdt);
      ATH_MSG_DEBUG("Found all three layer hits");
      TVector3 bi(csc_gp[0], csc_gp[1], csc_gp[2]);
      TVector3 bm(mdt_m_gp[0], mdt_m_gp[1], mdt_m_gp[2]);
      TVector3 bo(mdt_o_gp[0], mdt_o_gp[1], mdt_o_gp[2]);
      TVector3 IM = bm-bi;
      TVector3 IO = bo-bi;
      m_muon_track_sagitta = IM.Dot(mdt_tubedirection.Cross(IO).Unit());
      ATH_MSG_DEBUG("m_sagitta: "<<m_muon_track_sagitta);
    }
	
  
    // get run and event numbers
    ATH_MSG_DEBUG("Retrieving event info.");
    const EventInfo* eventInfo;
    StatusCode sc = m_storeGate->retrieve(eventInfo);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve event info.");
    }
    m_runNumber = eventInfo->event_ID()->run_number();
    m_evtNumber = eventInfo->event_ID()->event_number();

    // create chamber hitmap trees if they don't already exist
  
    // get info for reconstructed track
    Muon::MuonTrackHistTool::TrackData trackData;
    m_trackHistTool->extract(trackData, *alignTrack);
  
    ATH_MSG_DEBUG("extracted trackData from track");
  
    Muon::MuonHitHistTool::HitCountData& hcData = trackData.hitData.countData;
    m_phih  = hcData.rpcs.nphi + hcData.tgcs.nphi + hcData.cscs.nphi;
    m_etah  = hcData.mdts.nmdt + hcData.cscs.neta;
    m_stati = hcData.mdts.nmdtCh + hcData.cscs.netaCh;
  
    m_statr=0; // track fit status... 0?
  
    // global position of track perigee (probably worthless)
    m_xvtxr=trackData.gpos.x();
    m_yvtxr=trackData.gpos.y();
    m_zvtxr=trackData.gpos.z();
  
    ATH_MSG_DEBUG("global position of perigee" << m_xvtxr <<", "<<m_yvtxr
		  <<", "<<m_zvtxr);
  
    const Trk::Perigee* perigee = alignTrack->perigeeParameters();
    m_a0r    = perigee ? perigee->parameters()[Trk::d0] : -999.;
    m_z0r    = perigee ? perigee->parameters()[Trk::z0] : -999.;
    m_phir   = perigee ? perigee->momentum().phi()      : -999.;
    m_cotthr = perigee ? perigee->cotTheta()            : -999.;
    m_ptir   = perigee ? perigee->pT()                  : -999.;
    m_etar   = perigee ? perigee->eta()                 : -999.;
  
    m_chi2   = trackData.chi2;
    m_chi2pr = TMath::Prob(trackData.chi2, (int)trackData.ndof);
  
    ATH_MSG_DEBUG("perigee: "<<perigee);

    const Trk::MeasuredPerigee* measPars = dynamic_cast<const Trk::MeasuredPerigee*>(perigee);

    if( measPars ) {
      
      ATH_MSG_DEBUG("have measPars");
    
      const Trk::CovarianceMatrix& covMat = 
	measPars->localErrorMatrix().covariance();
    
      m_covr11=covMat[0][0];
      m_covr12=covMat[0][1];
      m_covr22=covMat[1][1];
      m_covr13=covMat[0][2];
      m_covr23=covMat[1][2];
      m_covr33=covMat[2][2];
      m_covr14=covMat[0][3];
      m_covr24=covMat[1][3];
      m_covr34=covMat[2][3];
      m_covr44=covMat[3][3];
      m_covr15=covMat[0][4];
      m_covr25=covMat[1][4];
      m_covr35=covMat[2][4];
      m_covr45=covMat[3][4];
      m_covr55=covMat[4][4];
    }
    else
      m_covr11=m_covr12=m_covr22=m_covr13=m_covr23=m_covr33=m_covr14=
	m_covr24=m_covr34=m_covr44=m_covr15=m_covr25=m_covr35=m_covr45= 
	m_covr55=-999;;
  
    m_vertexx=m_vertexy=m_vertexz=0.;
  
    // number of hits in different technologies
    m_mdth=hcData.mdts.nmdt;
    m_cscetah=hcData.cscs.neta;
    m_cscphih=hcData.cscs.nphi;
    m_rpcetah=hcData.rpcs.neta;
    m_rpcphih=hcData.rpcs.nphi;
    m_tgcetah=hcData.tgcs.neta;
    m_tgcphih=hcData.tgcs.nphi;
  
    // store information for all hits on this track, including 
    // scattering centers  
    //const DataVector<const Trk::TrackStateOnSurface>* states = 
    //alignTrack->trackStateOnSurfaces();
  
    //ATH_MSG_DEBUG("got states");
    //if( !states ) return;
    
    //int nhitsTotal = states->size();
    const Trk::AlignTSOSCollection* alignTSOSCollection = 
      alignTrack->alignTSOSCollection();
    int nhitsTotal = alignTSOSCollection->size();
    if (nhitsTotal > m_max_hits) nhitsTotal = m_max_hits;

    // loop over track and get information not on AlignTrack
    m_nMuonOutliers=m_nTgcOutliers=0;
    const DataVector<const Trk::TrackStateOnSurface>* tsosCollection=alignTrack->trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos = tsosCollection->begin();
    for (; iTsos != tsosCollection->end(); ++iTsos) {
      if (!(*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) continue;
      const Trk::MeasurementBase*             mesb       = (*iTsos)->measurementOnTrack();	
      const Trk::RIO_OnTrack*                 rio        = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack*        crio       = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!rio && crio) rio=crio->rioOnTrack(0);
      if (!rio) continue;
      if (!m_muonIdHelperTool->isMuon(rio->detectorElement()->identify())) continue;
    
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Outlier)) {
	m_nMuonOutliers++;    
	if (m_muonIdHelperTool->isTgc(rio->detectorElement()->identify())) m_nTgcOutliers++;
      }
    }

    ATH_MSG_DEBUG("have "<<nhitsTotal<<" hits total");
  
    int nhits=0;
    // loop over TSOSs
    //DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit    =states->begin();
    //DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end=states->end();
    //for( ; tsit!=tsit_end ; ++tsit) {
 
    // loop over AlignTSOSs
    std::vector<const Trk::AlignModule*> doneModules;
  
    m_nchambers=0;
    m_chi2_transx->Zero();
    m_chi2_transy->Zero();
    m_chi2_transz->Zero();
    m_chi2_rotx->Zero();
    m_chi2_roty->Zero();
    m_chi2_rotz->Zero();
    m_chi2_transx_X->Zero();
    m_chi2_transy_X->Zero();
    m_chi2_transz_X->Zero();
    m_chi2_rotx_X->Zero();
    m_chi2_roty_X->Zero();
    m_chi2_rotz_X->Zero();
    //mdt
    m_chi2_transx_mdt->Zero();
    m_chi2_transy_mdt->Zero();
    m_chi2_transz_mdt->Zero();
    m_chi2_rotx_mdt->Zero();
    m_chi2_roty_mdt->Zero();
    m_chi2_rotz_mdt->Zero();
    //tgc
    m_chi2_transx_tgc->Zero();
    m_chi2_transy_tgc->Zero();
    m_chi2_transz_tgc->Zero();
    m_chi2_rotx_tgc->Zero();
    m_chi2_roty_tgc->Zero();
    m_chi2_rotz_tgc->Zero();
    //rpc
    m_chi2_transx_rpc->Zero();
    m_chi2_transy_rpc->Zero();
    m_chi2_transz_rpc->Zero();
    m_chi2_rotx_rpc->Zero();
    m_chi2_roty_rpc->Zero();
    m_chi2_rotz_rpc->Zero();
    //csc
    m_chi2_transx_csc->Zero();
    m_chi2_transy_csc->Zero();
    m_chi2_transz_csc->Zero();
    m_chi2_rotx_csc->Zero();
    m_chi2_roty_csc->Zero();
    m_chi2_rotz_csc->Zero();
    //id
    m_chi2_transx_id->Zero();
    m_chi2_transy_id->Zero();
    m_chi2_transz_id->Zero();
    m_chi2_rotx_id->Zero();
    m_chi2_roty_id->Zero();
    m_chi2_rotz_id->Zero();
    //scat
    m_chi2_transx_scat->Zero();
    m_chi2_transy_scat->Zero();
    m_chi2_transz_scat->Zero();
    m_chi2_rotx_scat->Zero();
    m_chi2_roty_scat->Zero();
    m_chi2_rotz_scat->Zero();

    DataVector<Trk::AlignTSOS>::const_iterator atsit = alignTSOSCollection->begin();
    DataVector<Trk::AlignTSOS>::const_iterator atsit_end = alignTSOSCollection->end();
    for ( ;atsit!=atsit_end;++atsit) {
  
      ATH_MSG_DEBUG("in loop over AlignTSOS, nhits="<<nhits);
      if (nhits == m_max_hits) break;
    

      // don't keep scatterer info for now
      
	// if( (*tsit)->type(TrackStateOnSurface::scatterer) ) {
	// const Trk::TrackParameters* pars = (*tsit)->trackParameters();
	// HepGeom::Point3D<double> pos = pars ? pars->position() : HepGeom::Point3D<double>();
	// m_hit_dtyp[nhits]=10;
	// m_hit_rho[nhits]=pos.perp();
	// m_hit_z[nhits]=pos.z();      
	// ATH_MSG_DEBUG("m_hit_z"<<nhits<<"]: "<<m_hit_z[mnhits]);
	// }     
	// else {
      

      if ( !(*atsit)->type(Trk::TrackStateOnSurface::Measurement) ){
        continue;
      }

      //m_hit_type[nhits]=(*atsit)->type();

      const Trk::MeasurementBase* measurement = 
	dynamic_cast<const Trk::MeasurementBase*>((*atsit)->measurementOnTrack());
    
    
      ATH_MSG_DEBUG("have measurement");
      
      HepGeom::Point3D<double> pos = measurement->globalPosition();
      double locPos(0.);
      if( measurement->localParameters().contains(Trk::locX) ) 
	locPos = measurement->localParameters()[Trk::locX];      
      if( measurement->localParameters().contains(Trk::locY) ) 
	locPos = measurement->localParameters()[Trk::locY];

      double error = measurement->localErrorMatrix().error(Trk::locX); 
      ATH_MSG_DEBUG("error: "<<error);
      
      Identifier id = m_helperTool->getIdentifier(*measurement);
      ATH_MSG_DEBUG("id: "<<id.get_compact());
      
      double residual = -999.;
      if ( id.is_valid() && (p_MdtIdHelper->is_muon(id)|| p_CscIdHelper->is_muon(id))&& 
	   (p_MdtIdHelper->is_mdt(id)|| p_CscIdHelper->is_csc(id))) {

	const Trk::ResidualPull* resPull = 0;
	double pull = -999.;
	const Trk::TrackParameters* trackPars = (*atsit)->trackParameters();
	if ( trackPars ) {
	  resPull = m_pullCalculator->residualPull( measurement, trackPars,
						    Trk::ResidualPull::HitOnly,
						    (*atsit)->measType());
	  if ( resPull ) {
	    residual = resPull->residual().front();
	    pull = resPull->pull().front();
	    ATH_MSG_DEBUG("residual: "<<residual);
	  }
	}
	delete resPull; resPull=0;
   
	m_hit_tpar[nhits]=trackPars->parameters()[Trk::locX];

	ATH_MSG_DEBUG("have valid muon");

	MuonCalib::MuonFixedId fixedId = m_idTool->idToFixedId(id);

	m_hit_fixedid[nhits]        = fixedId.getIdInt();
	m_hit_fixedstatname[nhits]  = fixedId.stationName();
	ATH_MSG_DEBUG("hit in chamber with fixedid "<<m_hit_fixedid[nhits]
		      <<" and name "<<m_hit_fixedstatname[nhits]);

	// only do CSC/MDT for now
	if ( p_CscIdHelper->is_csc(id) ) {
	  m_hit_statname[nhits]=p_CscIdHelper->stationName(id);
	  m_hit_stateta[nhits]=p_CscIdHelper->stationEta(id);
	  m_hit_statphi[nhits]=p_CscIdHelper->stationPhi(id);
	  m_hit_statreg[nhits]=p_CscIdHelper->stationRegion(id);
	  m_hit_tech[nhits]=p_CscIdHelper->technology(id);
	  m_hit_dtyp[nhits]=p_CscIdHelper->measuresPhi(id) ? 6 : 5;
	  m_hit_clyr[nhits]=p_CscIdHelper->chamberLayer(id);
	  m_hit_wlyr[nhits]=p_CscIdHelper->wireLayer(id);
	  m_hit_strp[nhits]=p_CscIdHelper->strip(id);
	  ATH_MSG_DEBUG("m_hit_statname[nhits]: "<<m_hit_statname[nhits]
			<<"\tm_hit_statreg[nhits]: "<<m_hit_statreg[nhits]);
	       // Identifier Module = p_CscIdHelper->parentID(id);
		  // const MuonGM::CscReadoutElement* descriptor = p_muonMgr->getCscReadoutElement(Module);
		  // HepGeom::Point3D<double> LocalPos = descriptor->globalToLocalCoords(pos,Module);
		  // m_hit_xloc[nhits]=LocalPos.x();
		  // m_hit_yloc[nhits]=LocalPos.y();
		  // m_hit_zloc[nhits]=LocalPos.z();
	  
	  ATH_MSG_DEBUG("Done with module finding");
	  int csc_sector_num=-9999;
	  if(p_CscIdHelper->stationName(id)==50)
            csc_sector_num=p_CscIdHelper->stationEta(id)*(2*p_CscIdHelper->stationPhi(id)-1 + 1); 
	  else
            csc_sector_num=p_CscIdHelper->stationEta(id)*(2*p_CscIdHelper->stationPhi(id)-2 + 1);
	  if((*atsit)->measDir()==Trk::x){
	    if (m_csc_nphihits<m_csc_nMaxHits) {
	      m_csc_phi_hit_residual[m_csc_nphihits] = ((*atsit)->residuals())[0].residual();
	      m_csc_phi_hit_error[m_csc_nphihits] = ((*atsit)->residuals())[0].error();
	      m_csc_phi_hit_layer[m_csc_nphihits] = p_CscIdHelper->wireLayer(id);
	      m_csc_phi_hit_sector[m_csc_nphihits] = csc_sector_num;
	      ATH_MSG_DEBUG("In loop for CSC phi hit"); 
	      m_csc_nphihits++;
	    }
	  }else{
	    if (m_csc_netahits<m_csc_nMaxHits) {
	      m_csc_eta_hit_residual[m_csc_netahits] = ((*atsit)->residuals())[0].residual(); 
	      m_csc_eta_hit_error[m_csc_netahits] = ((*atsit)->residuals())[0].error(); 
	      m_csc_eta_hit_layer[m_csc_netahits] = p_CscIdHelper->wireLayer(id);
	      m_csc_eta_hit_sector[m_csc_netahits] =csc_sector_num;
	      ATH_MSG_DEBUG("In loop for CSC eta hit"); 
	      m_csc_netahits++; 
	    }
	  } 
	  ATH_MSG_DEBUG("Done with Variable filling");
	}else {

	  m_hit_statname[nhits]=p_MdtIdHelper->stationName(id);
	  m_hit_stateta [nhits]=p_MdtIdHelper->stationEta(id);
	  m_hit_statphi [nhits]=p_MdtIdHelper->stationPhi(id);
	  m_hit_statreg [nhits]=p_MdtIdHelper->stationRegion(id);
	  m_hit_tech[nhits]    =p_MdtIdHelper->technology(id);
	  m_hit_dtyp[nhits]=7; // MDT
	  m_hit_mlyr[nhits]=p_MdtIdHelper->multilayer(id);
	  m_hit_tlyr[nhits]=p_MdtIdHelper->tubeLayer(id);
	  m_hit_tube[nhits]=p_MdtIdHelper->tube(id);

	  Identifier Module = p_MdtIdHelper->parentID(id);
	  const MuonGM::MdtReadoutElement* descriptor = 
	    p_muonMgr->getMdtReadoutElement(Module);
	  HepGeom::Point3D<double> LocalPos = descriptor->globalToLocalCoords(pos,Module);
	  m_hit_xloc[nhits]=LocalPos.x();
	  m_hit_yloc[nhits]=LocalPos.y();
	  m_hit_zloc[nhits]=LocalPos.z();
	  m_hit_t0_x[nhits]=descriptor->tubePos(1,1,1).x();
	  m_hit_t0_y[nhits]=descriptor->tubePos(1,1,1).y();
	  m_hit_t0_z[nhits]=descriptor->tubePos(1,1,1).z();
	  m_hit_t1_x[nhits]=descriptor->tubePos(1,1,12).x();
	  m_hit_t1_y[nhits]=descriptor->tubePos(1,1,12).y();
	  m_hit_t1_z[nhits]=descriptor->tubePos(1,1,12).z();
	  m_hit_t2_x[nhits]=descriptor->tubePos(1,3,1).x();
	  m_hit_t2_y[nhits]=descriptor->tubePos(1,3,1).y();
	  m_hit_t2_z[nhits]=descriptor->tubePos(1,3,1).z();
      
	  ATH_MSG_DEBUG("hit_tube0_x/y/z="
			<<m_hit_t0_x[nhits]<<"/"<<m_hit_t0_y[nhits]<<"/"
			<<m_hit_t0_z[nhits]);


	  m_hit_rho[nhits]=pos.perp();
	  m_hit_phi[nhits]=pos.phi();
	  m_hit_z[nhits]=pos.z();
	  m_hit_sig[nhits]=error;
	  ATH_MSG_DEBUG("pushing back "<<residual<<" to m_hit_res");
	  m_hit_res[nhits]=residual;
	  double etaval=0;
	  if (p_MdtIdHelper->is_mdt(id)) {
	    m_hit_cotth[nhits]=1./tan(pos.theta());
	    etaval=-log(tan(atan(1./fabs(m_hit_cotth[nhits]))/2.));
	    if (m_hit_cotth[nhits] < 0.) { etaval *= -1.;}
	    m_hit_eta[nhits]=etaval;
	    m_hit_drd[nhits]=locPos;
	  } 
	  else {
	    m_hit_cotth[nhits]=pos.z()/pos.perp();
	    etaval=-log(tan(atan(1./fabs(m_hit_cotth[nhits]))/2.));
	    if (m_hit_cotth[nhits] < 0.) { etaval *= -1.;}
	    m_hit_eta[nhits]=etaval;
	  }
	} //is mdt

	
	  // if ( p_RpcIdHelper->is_rpc(id) ) {
	  // m_hit_statname[nhits]=p_RpcIdHelper->stationName(id);
	  // m_hit_stateta[nhits]=p_RpcIdHelper->stationEta(id);
	  // m_hit_statphi[nhits]=p_RpcIdHelper->stationPhi(id);
	  // m_hit_statreg[nhits]=p_RpcIdHelper->stationRegion(id);
	  // m_hit_tech[nhits]=p_RpcIdHelper->technology(id);
	  // m_hit_dtyp[nhits]=p_RpcIdHelper->measuresPhi(id) ? 2 : 1; 
	  // m_hit_dblr[nhits]=p_RpcIdHelper->doubletR(id);
	  // m_hit_dblz[nhits]=p_RpcIdHelper->doubletZ(id);
	  // m_hit_dblp[nhits]=p_RpcIdHelper->doubletPhi(id);
	  // m_hit_ggap[nhits]=p_RpcIdHelper->gasGap(id);
	  // m_hit_strp[nhits]=p_RpcIdHelper->strip(id);
	  // Identifier Module = p_RpcIdHelper->parentID(id);
	  // const MuonGM::RpcReadoutElement* descriptor = p_muonMgr->getRpcReadoutElement(Module);
	  // HepGeom::Point3D<double> LocalPos = descriptor->globalToLocalCoords(pos,Module);
	  // m_hit_xloc[nhits]=LocalPos.x();
	  // m_hit_yloc[nhits]=LocalPos.y();
	  // m_hit_zloc[nhits]=LocalPos.z();
	  // } 
	  // else if ( p_TgcIdHelper->is_tgc(id) ) {
	  // m_hit_statname[nhits]=p_TgcIdHelper->stationName(id);
	  // m_hit_stateta[nhits]=p_TgcIdHelper->stationEta(id);
	  // m_hit_statphi[nhits]=p_TgcIdHelper->stationPhi(id);
	  // m_hit_statreg[nhits]=p_TgcIdHelper->stationRegion(id);
	  // m_hit_tech[nhits]=p_TgcIdHelper->technology(id);
	  // m_hit_dtyp[nhits]=p_TgcIdHelper->isStrip(id) ? 4 : 3;
	  // m_hit_ggap[nhits]=p_TgcIdHelper->gasGap(id);
	  // m_hit_chnl[nhits]=p_TgcIdHelper->channel(id);
	  // Identifier Module = p_TgcIdHelper->parentID(id);
	  // const MuonGM::TgcReadoutElement* descriptor = p_muonMgr->getTgcReadoutElement(Module);
	  // HepGeom::Point3D<double> LocalPos = descriptor->globalToLocalCoords(pos,Module);
	  // m_hit_xloc[nhits]=LocalPos.x();
	  // m_hit_yloc[nhits]=LocalPos.y();
	  // m_hit_zloc[nhits]=LocalPos.z();
	  // } 
	  // else if ( p_CscIdHelper->is_csc(id) ) {
	  // m_hit_statname[nhits]=p_CscIdHelper->stationName(id);
	  // m_hit_stateta[nhits]=p_CscIdHelper->stationEta(id);
	  // m_hit_statphi[nhits]=p_CscIdHelper->stationPhi(id);
	  // m_hit_statreg[nhits]=p_CscIdHelper->stationRegion(id);
	  // m_hit_tech[nhits]=p_CscIdHelper->technology(id);
	  // m_hit_dtyp[nhits]=p_CscIdHelper->measuresPhi(id) ? 6 : 5;
	  // m_hit_clyr[nhits]=p_CscIdHelper->chamberLayer(id);
	  // m_hit_wlyr[nhits]=p_CscIdHelper->wireLayer(id);
	  // m_hit_strp[nhits]=p_CscIdHelper->strip(id);
	  // Identifier Module = p_CscIdHelper->parentID(id);
	  // const MuonGM::CscReadoutElement* descriptor = p_muonMgr->getCscReadoutElement(Module);
	  // HepGeom::Point3D<double> LocalPos = descriptor->globalToLocalCoords(pos,Module);
	  // m_hit_xloc[nhits]=LocalPos.x();
	  // m_hit_yloc[nhits]=LocalPos.y();
	  // m_hit_zloc[nhits]=LocalPos.z();
	  // } 
	  // else if ( p_MdtIdHelper->is_mdt(id) ) { 
	

      }//csc/mdt
      ATH_MSG_DEBUG("done with tech-specific");

      ++nhits;

      // get chi2 vs. align parameter information (if it exists)
      if (!(*atsit)->module()) continue;
    
      std::vector<const Trk::AlignModule*>::iterator it=find(doneModules.begin(),
							     doneModules.end(),
							     (*atsit)->module());
      if (it!=doneModules.end()) continue;
    
      ATH_MSG_DEBUG("working on nchamber "<<m_nchambers);
      if (m_nchambers<m_max_nchambers) {
	const Trk::AlignModule* module=(*atsit)->module();
	doneModules.push_back(module);
	ATH_MSG_DEBUG("module "<<module<<" (id "<<module->identify()<<")");
	Identifier identifier=module->identify();
	if (identifier.get_compact()>3) {
	  MuonCalib::MuonFixedId fixedId = 
	    m_idTool->idToFixedId(module->identify());
	  m_chamberid[m_nchambers]      =fixedId.getIdInt();
	  ATH_MSG_DEBUG("chamberid["<<m_nchambers<<"]="
			<<m_chamberid[m_nchambers]);
	  m_chamberstatname[m_nchambers]=fixedId.stationName();
	}
	else {
	  m_chamberid[m_nchambers]=identifier.get_compact();
	  ATH_MSG_DEBUG("chamberid["<<m_nchambers<<"]="
			<<m_chamberid[m_nchambers]);
	  m_chamberstatname[m_nchambers]=(int)identifier.get_compact();
	}
      
	double sigmas[6] = {1.,1.,1.,1.,1.,1.};
	DataVector<Trk::AlignPar>* alignPars=
	  m_alignModuleTool->getAlignPars(module);

	if (!alignPars) {
	  ATH_MSG_WARNING("no alignPars set for module!");
	  continue;
	}

	ATH_MSG_DEBUG("have alignpars");
	for (int iap=0;iap<(int)alignPars->size();iap++) {
	  int iparam=(*alignPars)[iap]->paramType();
	  sigmas[iparam]=(*alignPars)[iap]->sigma();
	}
      
	m_transx[m_nchambers]=sigmas[0];
	m_transy[m_nchambers]=sigmas[1];
	m_transz[m_nchambers]=sigmas[2];
	m_rotx  [m_nchambers]=sigmas[3];
	m_roty  [m_nchambers]=sigmas[4];
	m_rotz  [m_nchambers]=sigmas[5];
      
	ATH_MSG_DEBUG("m_rotx["<<m_nchambers<<"]="<<m_rotx[m_nchambers]);
	ATH_MSG_DEBUG("m_roty["<<m_nchambers<<"]="<<m_roty[m_nchambers]);
	ATH_MSG_DEBUG("m_rotz["<<m_nchambers<<"]="<<m_rotz[m_nchambers]);
      
	double** chi2AlignParam     = (*atsit)->module()->chi2VAlignParamArray();
	double** chi2AlignParamX    = (*atsit)->module()->chi2VAlignParamXArray();
	double** chi2AlignParamMdt  = (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::MDT);
	double** chi2AlignParamTgc  = (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::TGC);
	double** chi2AlignParamRpc  = (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::RPC);
	double** chi2AlignParamCsc  = (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::CSC);
	double** chi2AlignParamUnid = (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::unidentified);
	double** chi2AlignParamPixel= (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::Pixel);
	double** chi2AlignParamSCT  = (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::SCT);
	double** chi2AlignParamTRT  = (*atsit)->module()->chi2VAlignParamArrayMeasType(Trk::TrackState::TRT);

	if (chi2AlignParam==0) continue;
      
	if (m_nshifts<0) {
	  m_nshifts = module->nChamberShifts();
	  m_chi2_transx  ->ResizeTo(m_nshifts+1,m_chi2_transx  ->GetNcols());
	  for (int i=0;i<m_nshifts+1;i++)
	    for (int j=0;j<m_chi2_transx->GetNcols();j++)
	      ATH_MSG_DEBUG("m_chi2_transx["<<i<<"]["<<j<<"]="<<(*m_chi2_transx)[i][j]);
	  m_chi2_transy  ->ResizeTo(m_nshifts+1,m_chi2_transy  ->GetNcols());
	  m_chi2_transz  ->ResizeTo(m_nshifts+1,m_chi2_transz  ->GetNcols());
	  m_chi2_rotx    ->ResizeTo(m_nshifts+1,m_chi2_rotx    ->GetNcols());
	  m_chi2_roty    ->ResizeTo(m_nshifts+1,m_chi2_roty    ->GetNcols());
	  m_chi2_rotz    ->ResizeTo(m_nshifts+1,m_chi2_rotz    ->GetNcols());
	  m_chi2_transx_X->ResizeTo(m_nshifts+1,m_chi2_transx_X->GetNcols());
	  m_chi2_transy_X->ResizeTo(m_nshifts+1,m_chi2_transy_X->GetNcols());
	  m_chi2_transz_X->ResizeTo(m_nshifts+1,m_chi2_transz_X->GetNcols());
	  m_chi2_rotx_X  ->ResizeTo(m_nshifts+1,m_chi2_rotx_X  ->GetNcols());
	  m_chi2_roty_X  ->ResizeTo(m_nshifts+1,m_chi2_roty_X  ->GetNcols());
	  m_chi2_rotz_X  ->ResizeTo(m_nshifts+1,m_chi2_rotz_X  ->GetNcols());

	  // mdt
	  if (chi2AlignParamMdt) {
	    m_chi2_transx_mdt->ResizeTo(m_nshifts+1,m_chi2_transx_mdt->GetNcols());
	    m_chi2_transy_mdt->ResizeTo(m_nshifts+1,m_chi2_transy_mdt->GetNcols());
	    m_chi2_transz_mdt->ResizeTo(m_nshifts+1,m_chi2_transz_mdt->GetNcols());
	    m_chi2_rotx_mdt  ->ResizeTo(m_nshifts+1,m_chi2_rotx_mdt  ->GetNcols());
	    m_chi2_roty_mdt  ->ResizeTo(m_nshifts+1,m_chi2_roty_mdt  ->GetNcols());
	    m_chi2_rotz_mdt  ->ResizeTo(m_nshifts+1,m_chi2_rotz_mdt  ->GetNcols());
	  }

	  // tgc
	  if (chi2AlignParamTgc) {
	    ATH_MSG_DEBUG("resizing to "<<m_nshifts+1<<", "<<m_chi2_transx_tgc->GetNcols());
	    m_chi2_transx_tgc->ResizeTo(m_nshifts+1,m_chi2_transx_tgc->GetNcols());
	    for (int i=0;i<m_nshifts+1;i++)
	      for (int j=0;j<m_chi2_transx_tgc->GetNcols();j++)
		ATH_MSG_DEBUG("m_chi2_transx_tgc["<<i<<"]["<<j<<"]="<<(*m_chi2_transx_tgc)[i][j]);
	    m_chi2_transy_tgc->ResizeTo(m_nshifts+1,m_chi2_transy_tgc->GetNcols());
	    m_chi2_transz_tgc->ResizeTo(m_nshifts+1,m_chi2_transz_tgc->GetNcols());
	    m_chi2_rotx_tgc  ->ResizeTo(m_nshifts+1,m_chi2_rotx_tgc  ->GetNcols());
	    m_chi2_roty_tgc  ->ResizeTo(m_nshifts+1,m_chi2_roty_tgc  ->GetNcols());
	    m_chi2_rotz_tgc  ->ResizeTo(m_nshifts+1,m_chi2_rotz_tgc  ->GetNcols());
	  }

	  // rpc
	  if (chi2AlignParamRpc) {
	    m_chi2_transx_rpc->ResizeTo(m_nshifts+1,m_chi2_transx_rpc->GetNcols());
	    m_chi2_transy_rpc->ResizeTo(m_nshifts+1,m_chi2_transy_rpc->GetNcols());
	    m_chi2_transz_rpc->ResizeTo(m_nshifts+1,m_chi2_transz_rpc->GetNcols());
	    m_chi2_rotx_rpc  ->ResizeTo(m_nshifts+1,m_chi2_rotx_rpc  ->GetNcols());
	    m_chi2_roty_rpc  ->ResizeTo(m_nshifts+1,m_chi2_roty_rpc  ->GetNcols());
	    m_chi2_rotz_rpc  ->ResizeTo(m_nshifts+1,m_chi2_rotz_rpc  ->GetNcols());
	  }

	  // csc
	  if (chi2AlignParamCsc) {
	    m_chi2_transx_csc->ResizeTo(m_nshifts+1,m_chi2_transx_csc->GetNcols());
	    m_chi2_transy_csc->ResizeTo(m_nshifts+1,m_chi2_transy_csc->GetNcols());
	    m_chi2_transz_csc->ResizeTo(m_nshifts+1,m_chi2_transz_csc->GetNcols());
	    m_chi2_rotx_csc  ->ResizeTo(m_nshifts+1,m_chi2_rotx_csc  ->GetNcols());
	    m_chi2_roty_csc  ->ResizeTo(m_nshifts+1,m_chi2_roty_csc  ->GetNcols());
	    m_chi2_rotz_csc  ->ResizeTo(m_nshifts+1,m_chi2_rotz_csc  ->GetNcols());
	  }
	
	  // scat
	  if (chi2AlignParamUnid) {
	    m_chi2_transx_scat->ResizeTo(m_nshifts+1,m_chi2_transx_scat->GetNcols());
	    m_chi2_transy_scat->ResizeTo(m_nshifts+1,m_chi2_transy_scat->GetNcols());
	    m_chi2_transz_scat->ResizeTo(m_nshifts+1,m_chi2_transz_scat->GetNcols());
	    m_chi2_rotx_scat  ->ResizeTo(m_nshifts+1,m_chi2_rotx_scat  ->GetNcols());
	    m_chi2_roty_scat  ->ResizeTo(m_nshifts+1,m_chi2_roty_scat  ->GetNcols());
	    m_chi2_rotz_scat  ->ResizeTo(m_nshifts+1,m_chi2_rotz_scat  ->GetNcols());
	  }
	
	  // id
	  if (chi2AlignParamPixel || chi2AlignParamSCT || chi2AlignParamTRT) {
	    m_chi2_transx_id->ResizeTo(m_nshifts+1,m_chi2_transx_id->GetNcols());
	    m_chi2_transy_id->ResizeTo(m_nshifts+1,m_chi2_transy_id->GetNcols());
	    m_chi2_transz_id->ResizeTo(m_nshifts+1,m_chi2_transz_id->GetNcols());
	    m_chi2_rotx_id  ->ResizeTo(m_nshifts+1,m_chi2_rotx_id  ->GetNcols());
	    m_chi2_roty_id  ->ResizeTo(m_nshifts+1,m_chi2_roty_id  ->GetNcols());
	    m_chi2_rotz_id  ->ResizeTo(m_nshifts+1,m_chi2_rotz_id  ->GetNcols());
	  }
	}
	ATH_MSG_DEBUG("nshifts "<<m_nshifts<<", nchambers "<<m_nchambers);
      
	std::vector<TMatrixD*> chi2matrices;
	std::vector<TMatrixD*> chi2matricesX;
	std::vector<TMatrixD*> chi2matricesMdt;
	std::vector<TMatrixD*> chi2matricesTgc;
	std::vector<TMatrixD*> chi2matricesRpc;
	std::vector<TMatrixD*> chi2matricesCsc;
	std::vector<TMatrixD*> chi2matricesScat;
	std::vector<TMatrixD*> chi2matricesId;
      
	for (int ipar=0;ipar<(int)alignPars->size();ipar++) {
	  if      ((*alignPars)[ipar]->paramType()==Trk::AlignModule::TransX) {
	    chi2matrices.push_back(m_chi2_transx);
	    chi2matricesX.push_back(m_chi2_transx_X);	  
	    chi2matricesMdt.push_back(m_chi2_transx_mdt);
	    ATH_MSG_DEBUG("pushing back m_chi2_transx_tgc "<<m_chi2_transx_tgc);
	    chi2matricesTgc.push_back(m_chi2_transx_tgc);
	    chi2matricesRpc.push_back(m_chi2_transx_rpc);
	    chi2matricesCsc.push_back(m_chi2_transx_csc);
	    chi2matricesScat.push_back(m_chi2_transx_scat);
	    chi2matricesId.  push_back(m_chi2_transx_id);
	  }
	  else if ((*alignPars)[ipar]->paramType()==Trk::AlignModule::TransY) {
	    chi2matrices.push_back(m_chi2_transy);
	    chi2matricesX.push_back(m_chi2_transy_X);  
	    chi2matricesMdt.push_back(m_chi2_transy_mdt);
	    chi2matricesTgc.push_back(m_chi2_transy_tgc);
	    chi2matricesRpc.push_back(m_chi2_transy_rpc);
	    chi2matricesCsc.push_back(m_chi2_transy_csc);
	    chi2matricesScat.push_back(m_chi2_transy_scat);
	    chi2matricesId.  push_back(m_chi2_transy_id);
	  }
	  else if ((*alignPars)[ipar]->paramType()==Trk::AlignModule::TransZ) {
	    chi2matrices.push_back(m_chi2_transz);
	    chi2matricesX.push_back(m_chi2_transz_X);
	    chi2matricesMdt.push_back(m_chi2_transz_mdt);
	    chi2matricesTgc.push_back(m_chi2_transz_tgc);
	    chi2matricesRpc.push_back(m_chi2_transz_rpc);
	    chi2matricesCsc.push_back(m_chi2_transz_csc);
	    chi2matricesScat.push_back(m_chi2_transz_scat);
	    chi2matricesId.  push_back(m_chi2_transz_id);
	  }
	  else if ((*alignPars)[ipar]->paramType()==Trk::AlignModule::RotX) {
	    chi2matrices.push_back(m_chi2_rotx);
	    chi2matricesX.push_back(m_chi2_rotx_X);
	    chi2matricesMdt.push_back(m_chi2_rotx_mdt);
	    chi2matricesTgc.push_back(m_chi2_rotx_tgc);
	    chi2matricesRpc.push_back(m_chi2_rotx_rpc);
	    chi2matricesCsc.push_back(m_chi2_rotx_csc);
	    chi2matricesScat.push_back(m_chi2_rotx_scat);
	    chi2matricesId.  push_back(m_chi2_rotx_id);
	  }
	  else if ((*alignPars)[ipar]->paramType()==Trk::AlignModule::RotY) {
	    chi2matrices.push_back(m_chi2_roty);
	    chi2matricesX.push_back(m_chi2_roty_X);
	    chi2matricesMdt.push_back(m_chi2_roty_mdt);
	    chi2matricesTgc.push_back(m_chi2_roty_tgc);
	    chi2matricesRpc.push_back(m_chi2_roty_rpc);
	    chi2matricesCsc.push_back(m_chi2_roty_csc);
	    chi2matricesScat.push_back(m_chi2_roty_scat);
	    chi2matricesId.  push_back(m_chi2_roty_id);
	  }
	  else if ((*alignPars)[ipar]->paramType()==Trk::AlignModule::RotZ) {
	    chi2matrices.push_back(m_chi2_rotz);
	    chi2matricesX.push_back(m_chi2_rotz_X);
	    chi2matricesMdt.push_back(m_chi2_rotz_mdt);
	    chi2matricesTgc.push_back(m_chi2_rotz_tgc);
	    chi2matricesRpc.push_back(m_chi2_rotz_rpc);
	    chi2matricesCsc.push_back(m_chi2_rotz_csc);
	    chi2matricesScat.push_back(m_chi2_rotz_scat);
	    chi2matricesId.  push_back(m_chi2_rotz_id);
	  }
	}
      
	for (int ishift=0;ishift<m_nshifts;ishift++) {
	  ATH_MSG_DEBUG("ishift "<<ishift);
		
	  int jpar(0);
	  for (int iparam=0;iparam<(int)alignPars->size();iparam++) {
	    ATH_MSG_DEBUG("iparam="<<iparam<<"/"<<alignPars->size());
	    (*(chi2matrices[jpar]))[ishift][m_nchambers]   =chi2AlignParam[iparam][ishift];
	    (*(chi2matricesX[jpar]))[ishift][m_nchambers]  =chi2AlignParamX[iparam][ishift];

	    ATH_MSG_DEBUG("chi2matrices["<<jpar<<"]["<<ishift<<"]=chi2AlignParam["
			  <<iparam<<"]["<<ishift<<"]="<<chi2AlignParam[iparam][ishift]);

	    ATH_MSG_DEBUG("getting Mdt for jpar="<<jpar<<", ishift="
			  <<ishift<<", nchambers="<<m_nchambers<<", iparam="<<iparam);
	    if (chi2AlignParamMdt) 
	      (*(chi2matricesMdt[jpar]))[ishift][m_nchambers]=chi2AlignParamMdt[iparam][ishift];	    

	    ATH_MSG_DEBUG("getting Tgc for jpar="<<jpar<<", ishift="
			  <<ishift<<", nchambers="<<m_nchambers<<", iparam="<<iparam);
	    if (chi2AlignParamTgc) 
	      (*(chi2matricesTgc[jpar]))[ishift][m_nchambers]=chi2AlignParamTgc[iparam][ishift];

	    ATH_MSG_DEBUG("getting Rpc for jpar="<<jpar<<", ishift="
			  <<ishift<<", nchambers="<<m_nchambers<<", iparam="<<iparam);
	    if (chi2AlignParamRpc) 
	      (*(chi2matricesRpc[jpar]))[ishift][m_nchambers]=chi2AlignParamRpc[iparam][ishift];

	    ATH_MSG_DEBUG("getting Csc for jpar="<<jpar<<", ishift="
			  <<ishift<<", nchambers="<<m_nchambers<<", iparam="<<iparam);
	    if (chi2AlignParamCsc) 
	      (*(chi2matricesCsc[jpar]))[ishift][m_nchambers]=chi2AlignParamCsc[iparam][ishift];
	  
	    if (chi2AlignParamUnid)
	      (*(chi2matricesScat[jpar]))[ishift][m_nchambers]=chi2AlignParamUnid[iparam][ishift];

	    if (chi2AlignParamPixel || chi2AlignParamSCT || chi2AlignParamTRT) {
	      double val(0.);
	      if (chi2AlignParamPixel) val += chi2AlignParamPixel[iparam][ishift];
	      if (chi2AlignParamSCT)   val += chi2AlignParamSCT  [iparam][ishift];
	      if (chi2AlignParamTRT)   val += chi2AlignParamTRT  [iparam][ishift];
	      (*(chi2matricesId[jpar]))[ishift][m_nchambers]=val;
	    }
	  
	    ATH_MSG_DEBUG("done");
	    jpar++;
	  }
	}
      
	m_nchambers++;
      }
    } //atsos
    m_nhits=nhits;
    ATH_MSG_DEBUG("done with loop over TSOS");
  
  
    //fill truth info (variables are only filled for single muon MC)
    m_truth_pt_gen = -1;
    m_truth_pt_msentrance = -1;

    //generated truth variables  
    sc = StatusCode::SUCCESS;
    const McEventCollection* mcEventCollection(0);
    std::string mcEventCollectionName = "TruthEvent";
    if(m_storeGate->transientContains<McEventCollection>(mcEventCollectionName)) {
      sc = m_storeGate->retrieve(mcEventCollection,mcEventCollectionName);
    }
    if(sc.isFailure() || !mcEventCollection) {
      ATH_MSG_DEBUG("No McEventCollection: " << mcEventCollectionName 
		    << " found in store gate.");
    }
    else{
      ATH_MSG_DEBUG("have McEventCollection.");
    
      const HepMC::GenEvent*  genEvent  = *(mcEventCollection->begin());
      const HepMC::GenVertex* genVertex = *(genEvent->vertices_begin());
      HepMC::GenEvent::particle_const_iterator part = genEvent->particles_begin();
      HepMC::GenEvent::particle_const_iterator part_end = genEvent->particles_end();
    
      const HepMC::GenParticle* muon(0);
      for(;part!=part_end;++part){
	if( abs((*part)->pdg_id()) == 13
	    && (*part)->production_vertex() == genVertex){
	  if(!muon){   
	    muon = *part;
	  }
	  else{
	    ATH_MSG_DEBUG("More than one generated muon found."  << endmsg 
			  << "Generated truth info will not be filled.");
	    muon = 0;
	    break;
	  } 
	}
      }
    
      if(muon) m_truth_pt_gen = muon->momentum().perp();
    }
  
    //truth variables at muon spectrometer entrance  
    sc = StatusCode::SUCCESS;
    const TrackRecordCollection* trackRecordCollection(0);
    std::string trackRecordCollectionName = "MuonEntryLayer";
    if(m_storeGate->transientContains<TrackRecordCollection>(trackRecordCollectionName)) {
      sc = m_storeGate->retrieve(trackRecordCollection,trackRecordCollectionName);
    }
    if(sc.isFailure() || !trackRecordCollection){
      ATH_MSG_DEBUG("No TrackRecordCollection: " << trackRecordCollectionName
		    << " found in store gate.");
    }
    else{
      ATH_MSG_DEBUG("have TrackRecordCollection.");
      if(trackRecordCollection->size()==1){
	TrackRecordConstIterator trackRecord = trackRecordCollection->begin();
	CLHEP::HepLorentzVector vec((*trackRecord)->GetMomentum(),
			     (*trackRecord)->GetEnergy());
	m_truth_pt_msentrance = vec.perp();
      }else{
	ATH_MSG_DEBUG("More than one entry in trackRecordCollection found."<< endmsg 
		      << "Truth info at MS entrance will not be filled.");
      }
    }

  
    // chi2 vs. alignment parameter information from alignTrack
    for (int i=0;i<6;i++) 
      m_chi2VAlignParamQuality[i]=alignTrack->trackAlignParamQuality(i);
  

    // set default values
    m_trackChi2=m_trackChi2Dof=m_idTrackChi2=m_idTrackChi2Dof=
      m_msTrackChi2=m_msTrackChi2Dof=m_trackEta=m_energyLoss=m_msidPtDiff=-999.;
    m_nSctHits=m_nPixHits=m_nTrtHits=m_nMdtHits=m_nRpcPhiHits=m_nTgcPhiHits=-999;
    m_idPtAtIP=m_idZ0AtIP=m_idD0AtIP=-999.;

    //m_idD0AtIP=alignTrack->perigeeParameters()->parameters()[Trk::d0];
    //m_idZ0AtIP=alignTrack->perigeeParameters()->parameters()[Trk::z0];
    //alignTrack->perigeeParameters()->pT(); 
    m_trackChi2=(alignTrack->fitQuality())->chiSquared();
    m_trackChi2Dof=(alignTrack->fitQuality())->chiSquared()/((alignTrack->fitQuality())->numberDoF());  
    //m_trackEta= alignTrack->perigeeParameters()->eta();
  
    // muon container info
    const Analysis::MuonContainer* muons = 0;   
    if (m_muonContainer!="") {    
      if(evtStore()->contains<Analysis::MuonContainer>(m_muonContainer)) {
	StatusCode sc = evtStore()->retrieve(muons,m_muonContainer);
	if (sc.isFailure()) {	  
	  ATH_MSG_DEBUG("No Collection with name MuidMuonCollection found in StoreGate");
	}
      } 
      else {
	ATH_MSG_WARNING("no muon container "<<m_muonContainer);
      }
    }
  
    if (muons) {
      if (muons->size()>1) ATH_MSG_WARNING("Need to implement selection to get correct muon!");
    
      for (Analysis::MuonContainer::const_iterator muonItr=muons->begin(); 
	   muonItr != muons->end(); ++muonItr) {
      
	if (!(*muonItr)->hasCombinedMuonTrackParticle()) {
	  ATH_MSG_DEBUG("no combinedMuonTrackParticle!");
	  continue;
	}
      
	double idPt = (*muonItr)->hasInDetTrackParticle() ? 
	  (*muonItr)->inDetTrackParticle()->pt() : 0.;
	ATH_MSG_DEBUG("idPt: "<<idPt);
      
	double msExtrapPt = (*muonItr)->muonExtrapolatedTrackParticle() ? 
	  (*muonItr)->muonExtrapolatedTrackParticle()->pt() : 0.;
	ATH_MSG_DEBUG("msExtrapPt: "<<msExtrapPt);
      
	m_msidPtDiff = std::fabs(msExtrapPt/idPt-1.);
      
	// combined track
	const Trk::FitQuality* fitQual = 
	  (*muonItr)->combinedMuonTrackParticle()->fitQuality();	  
	if (fitQual) {
	  m_trackChi2    = fitQual->chiSquared();
	  m_trackChi2Dof = fitQual->chiSquared()/(double)fitQual->numberDoF();
	}

	// MS track
	if ((*muonItr)->muonExtrapolatedTrackParticle()) {
	  const Trk::FitQuality* msFitQual = 
	    (*muonItr)->muonExtrapolatedTrackParticle()->fitQuality();	  
	  if (msFitQual) {
	    m_msTrackChi2    = msFitQual->chiSquared();
	    m_msTrackChi2Dof = msFitQual->chiSquared()/(double)msFitQual->numberDoF();
	  }
	}

	// ID track
	const Trk::FitQuality* idFitQual = 
	  (*muonItr)->inDetTrackParticle()->fitQuality();	  
	if (idFitQual) {
	  m_idTrackChi2    = idFitQual->chiSquared();
	  m_idTrackChi2Dof = idFitQual->chiSquared()/(double)idFitQual->numberDoF();
	}

	// other variables
	m_trackEta   = (*muonItr)->eta();
	m_energyLoss = (*muonItr)->energyLoss().first;
	m_nSctHits   = (*muonItr)->numberOfSCTHits();
	m_nPixHits   = (*muonItr)->numberOfPixelHits();
	m_nTrtHits   = (*muonItr)->numberOfTRTHits();
      
	// ID track
	const Trk::Track* idtrack = (*muonItr)->inDetTrkTrack();
	if (idtrack) {
	  m_idPtAtIP=1./std::fabs(idtrack->perigeeParameters()->parameters()[Trk::qOverP]);
	  m_idZ0AtIP=idtrack->perigeeParameters()->parameters()[Trk::z0];
	  m_idD0AtIP=idtrack->perigeeParameters()->parameters()[Trk::d0];
	}
      
	// MS track at IP
	const Trk::Track* msatIP = (*muonItr)->muonExtrapolatedTrkTrack();
	if (msatIP) {
	  m_msPtAtIP=std::fabs(msatIP->perigeeParameters()->parameters()[Trk::qOverP])*1000.;
	  m_msZ0AtIP=msatIP->perigeeParameters()->parameters()[Trk::z0];
	  m_msD0AtIP=msatIP->perigeeParameters()->parameters()[Trk::d0];
	}
      
	m_nMdtHits    = (*muonItr)->numberOfMDTHits();
	m_nRpcPhiHits = (*muonItr)->numberOfRPCPhiHits();
	m_nTgcPhiHits = (*muonItr)->numberOfTGCPhiHits();
      }  
    }

    if(m_csc_nphihits>0){
      for(int il=0;il<m_csc_nphihits;il++){
        m_csc_phi_avg_residual += m_csc_phi_hit_residual[il];
        m_csc_phi_avg_error += (m_csc_phi_hit_error[il]*m_csc_phi_hit_error[il]);
      }
      m_csc_phi_avg_residual = (m_csc_phi_avg_residual/m_csc_nphihits);
      m_csc_phi_avg_error = (sqrt(m_csc_phi_avg_error)/m_csc_nphihits);
    }
    if(m_csc_netahits>0){ 
      for(int il=0;il<m_csc_netahits;il++){ 
        m_csc_eta_avg_residual += m_csc_eta_hit_residual[il]; 
        m_csc_eta_avg_error += (m_csc_eta_hit_error[il]*m_csc_eta_hit_error[il]); 
      } 
      m_csc_eta_avg_residual = (m_csc_eta_avg_residual/m_csc_netahits); 
      m_csc_eta_avg_error = (sqrt(m_csc_eta_avg_error)/m_csc_netahits); 
    } 

    for(int il=0;il<m_csc_nMaxHits;il++)
      ATH_MSG_VERBOSE("m_csc_phi_hit_residual[il]: "<<m_csc_phi_hit_residual[il]<<"   m_csc_eta_hit_residual[il]: "<< m_csc_eta_hit_residual[il]);  

    if (m_tree) m_tree->Fill();

    ++itrk;
    return;
    */
  }
  


  //________________________________________________________________________
  void MuonFillNtupleTool::storeHitmap() 
  {
  }

  //________________________________________________________________________
  void MuonFillNtupleTool::fillHitmap() 
  {
  }

  //________________________________________________________________________
  void MuonFillNtupleTool::fillSummary() 
  {
  }

  //________________________________________________________________________
  void MuonFillNtupleTool::showStatistics() 
  {
  }
  
} // end namespace
