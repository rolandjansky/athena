/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/// Name    : MultivariateTagManager
/// Package : JetTagTools
/// Author  : Dan Guest (UC Irvine), Marie Lanfermann (U Geneva)
/// Created : October 2015
///
/// DESCRIPTION:
///
/// This class handles the inputs to MV based taggers like DL1.
///
/////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IToolSvc.h"

#include "JetTagCalibration/CalibrationBroker.h"

#include "JetTagTools/JetTagUtils.h"
#include "JetTagTools/MultivariateTagManager.h"
#include "JetTagTools/BTagVariables.h"

#include "TObjString.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <exception>
#include <limits> // check for NAN
#include <cmath>  // NAN


namespace {
  static_assert(std::numeric_limits<double>::has_quiet_NaN,
    "no NaN defined, but we require one");
  // int values use -1 for a placeholder,
  const int INT_MISSING = -1;
  double nan_if_placeholder(int);
}

namespace Analysis {
  MultivariateTagManager::MultivariateTagManager(const std::string& name, const std::string& n, const IInterface* p)
    : AthAlgTool(name, n, p),
      m_MultivariateTaggerHandleArray()
  {
    declareInterface<ITagTool>(this);

    // List of the tagging tools to be used
    declareProperty("MVTagToolList", m_MultivariateTaggerHandleArray);

    declareProperty("inputSV0SourceName", m_sv0_infosource = "SV0");
    declareProperty("inputSV1SourceName", m_sv1_infosource = "SV1");
    declareProperty("inputIP2DSourceName", m_ip2d_infosource = "IP2D");
    declareProperty("inputIP3DSourceName", m_ip3d_infosource = "IP3D");
    declareProperty("inputJFSourceName", m_jftNN_infosource = "JetFitter");
    declareProperty("inputSoftMuonSourceName", m_softmuon_infosource = "SMT");
    declareProperty("arbitraryAuxData", m_arbitrary_aux_data);
    declareProperty("auxDataNameMap", m_aux_data_name_map);
  }

  StatusCode MultivariateTagManager::initialize()
  {
    if ( m_MultivariateTaggerHandleArray.retrieve().isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to retreive " << m_MultivariateTaggerHandleArray);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved " << m_MultivariateTaggerHandleArray);
    }

    // get multivariate jet tagger
    StatusCode sc = StatusCode::FAILURE;
    for (auto& itr : m_MultivariateTaggerHandleArray) {
      sc = itr.retrieve(); //initialize the tagger from the array
      if(sc.isFailure()){
  ATH_MSG_WARNING("Retrieving in the initialization of MultivariateTagManager failed.");
      }
    }

    ATH_MSG_INFO(" #BTAG# Initialization of MultivariateTagManager successfull" );
    return StatusCode::SUCCESS;
  }

  StatusCode MultivariateTagManager::finalize() { // all taken care of in destructor
    ATH_MSG_INFO(" #BTAG# Finalization of MultivariateTagManager successfull" );
    return StatusCode::SUCCESS;
  }


  // _______________________________________________________________________
  // MultivariateTagManager functions

  StatusCode MultivariateTagManager::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag, const std::string jetName) {

    std::string jetauthor = jetName; // determine the jet's channel
    ATH_MSG_DEBUG("#BTAG# Jet author: " << jetauthor );

    if ( jetauthor.empty() ) {
      ATH_MSG_WARNING(" #BTAG# Hypothesis or jetauthor is empty."
          " No likelihood value given back. ");
    }

    double jetpT  = jetToTag.pt();
    double jeteta = jetToTag.eta();

    // Fill Input Information
    var_map inputs;                  // map of input information
    inputs[btagvar::JET_ETA] = jeteta;
    inputs[btagvar::JET_ABS_ETA] = std::fabs(jeteta);
    inputs[btagvar::JET_PT]  = jetpT;
    fill_ip2d(inputs, BTag);         // fill IP2D variables
    fill_ip3d(inputs, BTag);         // fill IP3D variables
    fill_jetfitter(inputs, BTag);    // fill JetFitter variables
    fill_sv0(inputs, BTag);          // fill sv0 variables
    fill_sv1(inputs, BTag);          // fill sv1 variables
    fill_mvb(inputs, jetToTag, BTag);// fill MVb variables
    fill_mv2cl100(inputs, BTag); // fill MV2cl100 variables
    fill_trkSum(inputs,BTag);
    fill_softmuon(inputs,BTag);

    fill_arbitrary_aux_data(inputs, BTag);

    if (msgLvl(MSG::VERBOSE)) {
      for (const auto& input: inputs) {
        ATH_MSG_VERBOSE("  " << input.first << ": " << input.second);
      }
    }

    ATH_MSG_DEBUG(" #BTAG# Retrieving of inputs successfull" );

    /* ----------------------------------------------------------------------------------- */
    /*       Call all the tag tools specified in m_MultivariateTaggerHandleArray           */
    /* ----------------------------------------------------------------------------------- */

    for (auto& itr: m_MultivariateTaggerHandleArray) {

      itr->assignProbability(BTag, inputs, jetauthor);

    }

    ATH_MSG_DEBUG(" #BTAG# Retrieving of output successfull" );
    return StatusCode::SUCCESS;
  }

  // fill functions
  void MultivariateTagManager::fill_softmuon(var_map& inputs, xAOD::BTagging* BTag) const {
    float sm_mu_pt           = NAN;
    float sm_dR              = NAN;
    float sm_qOverPratio     = NAN;
    float sm_mombalsignif    = NAN;
    float sm_scatneighsignif = NAN;
    float sm_pTrel           = NAN;
    float sm_mu_d0           = NAN;
    float sm_mu_z0           = NAN;
    float sm_ID_qOverP       = NAN;

    BTag->variable<float>(m_softmuon_infosource, "mu_pt"           , sm_mu_pt          );
    if(sm_mu_pt>0){
        BTag->variable<float>(m_softmuon_infosource, "dR"              , sm_dR             );
        BTag->variable<float>(m_softmuon_infosource, "qOverPratio"     , sm_qOverPratio    );
        BTag->variable<float>(m_softmuon_infosource, "mombalsignif"    , sm_mombalsignif   );
        BTag->variable<float>(m_softmuon_infosource, "scatneighsignif" , sm_scatneighsignif);
        BTag->variable<float>(m_softmuon_infosource, "pTrel"           , sm_pTrel          );
        BTag->variable<float>(m_softmuon_infosource, "mu_d0"           , sm_mu_d0          );
        BTag->variable<float>(m_softmuon_infosource, "mu_z0"           , sm_mu_z0          );
        BTag->variable<float>(m_softmuon_infosource, "ID_qOverP"       , sm_ID_qOverP      );
    }else{
      sm_mu_pt= NAN;
    }

    inputs[btagvar::SM_MU_PT]     = sm_mu_pt;
    inputs[btagvar::SM_DR]        = sm_dR;
    inputs[btagvar::SM_QOVERP]    = sm_qOverPratio;
    inputs[btagvar::SM_MOMBALSIG] = sm_mombalsignif;
    inputs[btagvar::SM_SCATNEIGH] = sm_scatneighsignif;
    inputs[btagvar::SM_PTREL]     = sm_pTrel;
    inputs[btagvar::SM_MU_D0]     = sm_mu_d0;
    inputs[btagvar::SM_MU_Z0]     = sm_mu_z0;
    inputs[btagvar::SM_ID_QOVERP] = sm_ID_qOverP;


  }
  void MultivariateTagManager::fill_trkSum(var_map& inputs, xAOD::BTagging* BTag) const {
    float trkSum_ntrk = NAN;
    float trkSum_sPt = NAN;
    float trkSum_vPt = NAN;
    float trkSum_vAbsEta =NAN;


    trkSum_ntrk   = BTag->isAvailable<unsigned>("trkSum_ntrk") ? BTag->auxdata<unsigned>("trkSum_ntrk") : NAN;
    trkSum_sPt    = BTag->isAvailable<float   >("trkSum_SPt" ) ? BTag->auxdata<float   >("trkSum_SPt" ) : NAN;

    if (!std::isnan(trkSum_ntrk)){
      trkSum_vPt    = BTag->isAvailable<float>("trkSum_VPt" ) ?      BTag->auxdata<float>("trkSum_VPt" )  : NAN;
      trkSum_vAbsEta= BTag->isAvailable<float>("trkSum_VEta") ? fabs(BTag->auxdata<float>("trkSum_VEta")) : NAN;
    }

    inputs[btagvar::TRKSUM_NTRK]   = trkSum_ntrk;
    inputs[btagvar::TRKSUM_SPT]    = trkSum_sPt;
    inputs[btagvar::TRKSUM_VPT]    = trkSum_vPt;
    inputs[btagvar::TRKSUM_ABSETA] = trkSum_vAbsEta;


  }

  void MultivariateTagManager::fill_jetfitter(var_map& inputs, xAOD::BTagging* BTag) const {
    // default values
    int jf_nvtx      = INT_MISSING;
    int jf_nvtx1t    = INT_MISSING;
    int jf_ntrkAtVx  = INT_MISSING;
    int jf_n2tv      = INT_MISSING;
    float jf_dphi    = NAN;
    float jf_deta    = NAN;
    float jf_dR      = NAN;
    float jf_dR_flight = NAN;
    float jf_efrc    = NAN;
    float jf_mass    = NAN;
    float jf_mass_uncor = NAN;
    float jf_sig3d   = NAN;

    // check if we have vertices
    int jf_nvtx_tmp(INT_MISSING), jf_nvtx1t_tmp(INT_MISSING); bool jfitter_ok(false);
    std::vector< ElementLink< xAOD::BTagVertexContainer > > jf_vertices;
    BTag->variable<std::vector<ElementLink<xAOD::BTagVertexContainer> > >(m_jftNN_infosource, "JFvertices", jf_vertices);
    if("JetFitter" == m_jftNN_infosource) {
      BTag->taggerInfo(jf_nvtx_tmp, xAOD::BTagInfo::JetFitter_nVTX);
      BTag->taggerInfo(jf_nvtx1t_tmp, xAOD::BTagInfo::JetFitter_nSingleTracks);
    }
    else {
      BTag->variable<int>(m_jftNN_infosource, "nVTX", jf_nvtx_tmp  );
      BTag->variable<int>(m_jftNN_infosource, "nSingleTracks", jf_nvtx1t_tmp);
    }
    if(jf_vertices.size()>0 && jf_vertices[0].isValid() && (jf_nvtx_tmp > 0 || jf_nvtx1t_tmp > 0)) jfitter_ok = true;

    if(jfitter_ok) {
      // Get values from the xAOD
      if("JetFitter" == m_jftNN_infosource) { // check if JetFitter is known by the xAOD?
  BTag->taggerInfo(jf_nvtx,       xAOD::BTagInfo::JetFitter_nVTX);
  BTag->taggerInfo(jf_nvtx1t,     xAOD::BTagInfo::JetFitter_nSingleTracks);
  BTag->taggerInfo(jf_ntrkAtVx,   xAOD::BTagInfo::JetFitter_nTracksAtVtx);
  BTag->taggerInfo(jf_n2tv,       xAOD::BTagInfo::JetFitter_N2Tpair);
  BTag->taggerInfo(jf_efrc,     xAOD::BTagInfo::JetFitter_energyFraction);
  BTag->taggerInfo(jf_mass,     xAOD::BTagInfo::JetFitter_mass);
  BTag->taggerInfo(jf_sig3d,    xAOD::BTagInfo::JetFitter_significance3d);
  BTag->taggerInfo(jf_dphi,     xAOD::BTagInfo::JetFitter_deltaphi);
  BTag->taggerInfo(jf_deta,     xAOD::BTagInfo::JetFitter_deltaeta);
      }
      else { // get variables explicitely
  BTag->variable<int>(m_jftNN_infosource,    "nVTX",           jf_nvtx);
  BTag->variable<int>(m_jftNN_infosource,    "nSingleTracks",  jf_nvtx1t);
  BTag->variable<int>(m_jftNN_infosource,    "nTracksAtVtx",   jf_ntrkAtVx);
  BTag->variable<int>(m_jftNN_infosource,    "N2Tpair",        jf_n2tv);
  BTag->variable<float>(m_jftNN_infosource,  "energyFraction", jf_efrc);
  BTag->variable<float>(m_jftNN_infosource,  "mass",           jf_mass);
  BTag->variable<float>(m_jftNN_infosource,  "significance3d", jf_sig3d);
  BTag->variable<float>(m_jftNN_infosource,  "deltaphi",       jf_dphi);
  BTag->variable<float>(m_jftNN_infosource,  "deltaeta",       jf_deta);
      }
      // NOTE: no need to check for NAN here, it should do the right thing
      // http://en.cppreference.com/w/cpp/numeric/math/hypot#Error_handling
      jf_dR = std::hypot(jf_dphi,jf_deta);
      //new jf variables
      BTag->variable<float>(m_jftNN_infosource, "massUncorr" , jf_mass_uncor);
      BTag->variable<float>(m_jftNN_infosource, "dRFlightDir", jf_dR_flight);
    }
    // add variables to input map
    inputs[btagvar::JF_NVTX]   = nan_if_placeholder(jf_nvtx);
    inputs[btagvar::JF_NVTX1T] = nan_if_placeholder(jf_nvtx1t);
    inputs[btagvar::JF_NTRKV]  = nan_if_placeholder(jf_ntrkAtVx);
    inputs[btagvar::JF_EFRC]   = jf_efrc;
    inputs[btagvar::JF_MASS]   = jf_mass;
    inputs[btagvar::JF_MASS_UNCOR]   = jf_mass_uncor;
    inputs[btagvar::JF_N2TV]   = nan_if_placeholder(jf_n2tv);
    inputs[btagvar::JF_SIG3D]  = jf_sig3d;
    inputs[btagvar::JF_DR]     = jf_dR;
    inputs[btagvar::JF_DR_FLIGHT]= jf_dR_flight;
    inputs[btagvar::JF_DPHI]   = jf_dphi;
    inputs[btagvar::JF_DETA]   = jf_deta;
  }



  void MultivariateTagManager::fill_ip2d(var_map& inputs, xAOD::BTagging* BTag) const {
    // default values
    double ip2d_pb = NAN;
    double ip2d_pc = NAN;
    double ip2d_pu = NAN;

    float ip2   = NAN;
    float ip2_c = NAN;
    float ip2_cu= NAN;

    float ip2_nan   = NAN;
    float ip2_c_nan = NAN;
    float ip2_cu_nan= NAN;

    std::vector<float> weightBofTracksIP2D;
    BTag->variable<std::vector<float> >(m_ip2d_infosource, "weightBofTracks", weightBofTracksIP2D);
    int ntrk_ip2 = weightBofTracksIP2D.size();

    if(ntrk_ip2>0) {

      if( m_ip2d_infosource == "IP2D" ) {
	ip2d_pb = BTag->IP2D_pb();
	ip2d_pc = BTag->IP2D_pc();
	ip2d_pu = BTag->IP2D_pu();
      }
      else {
	BTag->variable<double>(m_ip2d_infosource, "pb", ip2d_pb);
	BTag->variable<double>(m_ip2d_infosource, "pc", ip2d_pc);
	BTag->variable<double>(m_ip2d_infosource, "pu", ip2d_pu);
      }
      
      ip2    = BTag->calcLLR(ip2d_pb,ip2d_pu);
      ip2_c  = BTag->calcLLR(ip2d_pb,ip2d_pc);
      ip2_cu = BTag->calcLLR(ip2d_pc,ip2d_pu);

      if(ip2d_pb<=0. or ip2d_pu<=0. or ip2d_pb==NAN or ip2d_pu==NAN) {
	ip2_nan = NAN;
      } else {
	ip2_nan = log(ip2d_pb/ip2d_pu);
      }

      if(ip2d_pb<=0. or ip2d_pc<=0. or ip2d_pb==NAN or ip2d_pc==NAN) {
	ip2_c_nan = NAN;
      } else {
	ip2_c_nan = log(ip2d_pb/ip2d_pc);
      }

      if(ip2d_pc<=0. or ip2d_pu<=0. or ip2d_pc==NAN or ip2d_pu==NAN) {
	ip2_cu_nan = NAN;
      } else {
	ip2_cu_nan = log(ip2d_pc/ip2d_pu);
      }
    }

    // add variables to input map
    inputs[btagvar::IP2D_PB] = ip2d_pb;
    inputs[btagvar::IP2D_PC] = ip2d_pc;
    inputs[btagvar::IP2D_PU] = ip2d_pu;
    inputs[btagvar::IP2]     = ip2;
    inputs[btagvar::IP2_C]   = ip2_c;
    inputs[btagvar::IP2_CU]  = ip2_cu;
    inputs[btagvar::IP2_NAN]     = ip2_nan;
    inputs[btagvar::IP2_C_NAN]   = ip2_c_nan;
    inputs[btagvar::IP2_CU_NAN]  = ip2_cu_nan;

  }

  void MultivariateTagManager::fill_ip3d(var_map& inputs, xAOD::BTagging* BTag) const {
    // default values
    double ip3d_pb = NAN;
    double ip3d_pc = NAN;
    double ip3d_pu = NAN;

    float ip3   = NAN;
    float ip3_c = NAN;
    float ip3_cu= NAN;

    float ip3_nan    = NAN;
    float ip3_c_nan  = NAN;
    float ip3_cu_nan = NAN;

    std::vector<float> weightBofTracksIP3D;
    BTag->variable<std::vector<float> >(m_ip3d_infosource, "weightBofTracks", weightBofTracksIP3D);
    int ntrk_ip3= weightBofTracksIP3D.size();
    if(ntrk_ip3>0) {
      if( m_ip3d_infosource == "IP3D" ) {
	ip3d_pb = BTag->IP3D_pb();
	ip3d_pc = BTag->IP3D_pc();
	ip3d_pu = BTag->IP3D_pu();
      }
      else {
	BTag->variable<double>(m_ip3d_infosource, "pb", ip3d_pb);
	BTag->variable<double>(m_ip3d_infosource, "pc", ip3d_pc);
	BTag->variable<double>(m_ip3d_infosource, "pu", ip3d_pu);
      }

      ip3    = BTag->calcLLR(ip3d_pb,ip3d_pu);
      ip3_c  = BTag->calcLLR(ip3d_pb,ip3d_pc);
      ip3_cu = BTag->calcLLR(ip3d_pc,ip3d_pu);

      if(ip3d_pb<=0. or ip3d_pu<=0. or ip3d_pb==NAN or ip3d_pu==NAN) {
	ip3_nan = NAN;
      } else {
	ip3_nan = log(ip3d_pb/ip3d_pu);
      }

      if(ip3d_pb<=0. or ip3d_pc<=0. or ip3d_pb==NAN or ip3d_pc==NAN) {
	ip3_c_nan = NAN;
      } else {
	ip3_c_nan = log(ip3d_pb/ip3d_pc);
      }

      if(ip3d_pc<=0. or ip3d_pu<=0. or ip3d_pc==NAN or ip3d_pu==NAN) {
	ip3_cu_nan = NAN;
      } else {
	ip3_cu_nan = log(ip3d_pc/ip3d_pu);
      }
    }

    // add variables to input map
    inputs[btagvar::IP3D_PB] = ip3d_pb;
    inputs[btagvar::IP3D_PC] = ip3d_pc;
    inputs[btagvar::IP3D_PU] = ip3d_pu;
    inputs[btagvar::IP3]     = ip3;
    inputs[btagvar::IP3_C]   = ip3_c;
    inputs[btagvar::IP3_CU]  = ip3_cu;
    inputs[btagvar::IP3_NAN]     = ip3_nan;
    inputs[btagvar::IP3_C_NAN]   = ip3_c_nan;
    inputs[btagvar::IP3_CU_NAN]  = ip3_cu_nan;

  }


  void MultivariateTagManager::fill_sv0(var_map& inputs, xAOD::BTagging* BTag) const {
    // default values
    int    sv0_n2t    = INT_MISSING;
    int    sv0_ntrkv  = INT_MISSING;
    float  sv0_efrc   = NAN;
    float  sv0_mass   = NAN;
    float  sv0_radius = NAN;
    double sv0_sig3d  = NAN;
    double sv0_pv_x = NAN, sv0_pv_y = NAN;

    // get vertex information
    bool sv0_ok(false);
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices_SV0;
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", myVertices_SV0);

    if ( myVertices_SV0.size() > 0 && myVertices_SV0[0].isValid() ) {
      // if we found a vertex, then sv0 is okay to use
      sv0_ok = true;
    }

    if (sv0_ok) {
      if (m_sv0_infosource == "SV0") {
  BTag->taggerInfo(sv0_mass, xAOD::BTagInfo::SV0_masssvx);
  BTag->taggerInfo(sv0_efrc, xAOD::BTagInfo::SV0_efracsvx);
  BTag->taggerInfo(sv0_n2t,  xAOD::BTagInfo::SV0_N2Tpair);
  BTag->taggerInfo(sv0_ntrkv, xAOD::BTagInfo::SV0_NGTinSvx);
  //BTag->taggerInfo(sv0_sig3d, xAOD::BTagInfo::SV0_normdist);
      }
      else {
  BTag->variable<float>(m_sv0_infosource,  "masssvx", sv0_mass);
  BTag->variable<float>(m_sv0_infosource,  "efracsvx", sv0_efrc);
  BTag->variable<int>(m_sv0_infosource,    "N2Tpair", sv0_n2t);
  BTag->variable<int>(m_sv0_infosource,    "NGTinSvx", sv0_ntrkv);
  //BTag->variable<double>(m_sv0_infosource, "significance3D", sv0_sig3d);
      }
      BTag->variable<double>(m_sv0_infosource, "significance3D", sv0_sig3d);

      if(m_priVtx) {
  sv0_pv_x=m_priVtx->x();
  sv0_pv_y=m_priVtx->y();
      }
      sv0_radius = sqrt(pow(sv0_pv_x,2)+pow(sv0_pv_y,2));
    }

    // add variables to input map
    inputs[btagvar::SV0_MASS]   = sv0_mass;
    inputs[btagvar::SV0_EFRC]   = sv0_efrc;
    inputs[btagvar::SV0_N2T]    = nan_if_placeholder(sv0_n2t);
    inputs[btagvar::SV0_NTRKV]  = nan_if_placeholder(sv0_ntrkv);
    inputs[btagvar::SV0_SIG3D]  = sv0_sig3d;
    inputs[btagvar::SV0_RADIUS] = sv0_radius;
  }


  void MultivariateTagManager::fill_sv1(var_map& inputs, xAOD::BTagging* BTag) const {
    // default values
    double sv1_pb = NAN, sv1_pc = NAN, sv1_pu = NAN;
    float     sv1 = NAN, sv1_c  = NAN, sv1_cu = NAN;
    float     sv1_nan = NAN, sv1_c_nan  = NAN, sv1_cu_nan = NAN;

    int   sv1_n2t    = INT_MISSING;
    int   sv1_ntrkv  = INT_MISSING;
    float sv1_efrc   = NAN;
    float sv1_mass   = NAN;
    float sv1_Lxy    = NAN;
    float sv1_L3d    = NAN;
    float sv1_sig3d  = NAN;
    float sv1_dR     = NAN;
    float sv1_distmatlay = NAN;

    // get vertex information
    bool sv1_ok(false);
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices_SV1;
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", myVertices_SV1);
    if ( myVertices_SV1.size() > 0 && myVertices_SV1[0].isValid() ) {
      // if we found a vertex, then sv1 is okay to use
      sv1_ok = true;
    }

    if (sv1_ok) {
      if (m_sv1_infosource == "SV1") {
	sv1_pb=BTag->SV1_pb();
	sv1_pu=BTag->SV1_pu();
	sv1_pc=BTag->SV1_pc();
	
	BTag->taggerInfo(sv1_mass, xAOD::BTagInfo::SV1_masssvx);
	BTag->taggerInfo(sv1_efrc, xAOD::BTagInfo::SV1_efracsvx);
	BTag->taggerInfo(sv1_n2t,  xAOD::BTagInfo::SV1_N2Tpair);
	BTag->taggerInfo(sv1_ntrkv, xAOD::BTagInfo::SV1_NGTinSvx);
	BTag->taggerInfo(sv1_sig3d, xAOD::BTagInfo::SV1_normdist);
      }
      else {
	BTag->variable<double>(m_sv1_infosource, "pu", sv1_pu);
	BTag->variable<double>(m_sv1_infosource, "pb", sv1_pb);
	BTag->variable<double>(m_sv1_infosource, "pc", sv1_pc);
	
	BTag->variable<float>(m_sv1_infosource, "masssvx",  sv1_mass);
	BTag->variable<float>(m_sv1_infosource, "efracsvx", sv1_efrc);
	BTag->variable<int>(m_sv1_infosource,   "N2Tpair",  sv1_n2t);
	BTag->variable<int>(m_sv1_infosource,   "NGTinSvx", sv1_ntrkv);
	BTag->variable<float>(m_sv1_infosource, "normdist", sv1_sig3d);
      }
      BTag->variable<float>(m_sv1_infosource, "dstToMatLay" , sv1_distmatlay);
      BTag->variable<float>(m_sv1_infosource, "deltaR", sv1_dR);
      BTag->variable<float>(m_sv1_infosource, "Lxy",    sv1_Lxy);
      BTag->variable<float>(m_sv1_infosource, "L3d",    sv1_L3d);

      sv1    = BTag->calcLLR(sv1_pb,sv1_pu);
      sv1_c  = BTag->calcLLR(sv1_pb,sv1_pc);
      sv1_cu = BTag->calcLLR(sv1_pc,sv1_pu);

      if(sv1_pb<=0. or sv1_pu<=0. or sv1_pb==NAN or sv1_pu==NAN) {
	sv1_nan = NAN;
      } else {
	sv1_nan = log(sv1_pb/sv1_pu);
      }

      if(sv1_pb<=0. or sv1_pc<=0. or sv1_pb==NAN or sv1_pc==NAN) {
	sv1_c_nan = NAN;
      } else {
	sv1_c_nan = log(sv1_pb/sv1_pc);
      }

      if(sv1_pc<=0. or sv1_pu<=0. or sv1_pc==NAN or sv1_pu==NAN) {
	sv1_cu_nan = NAN;
      } else {
	sv1_cu_nan = log(sv1_pc/sv1_pu);
      }
    }

    // add variables to input map
    inputs[btagvar::SV1_PB] = sv1_pb;
    inputs[btagvar::SV1_PC] = sv1_pc;
    inputs[btagvar::SV1_PU] = sv1_pu;
    inputs[btagvar::SV1]    = sv1;
    inputs[btagvar::SV1_C]  = sv1_c;
    inputs[btagvar::SV1_CU] = sv1_cu;
    inputs[btagvar::SV1_NAN]    = sv1_nan;
    inputs[btagvar::SV1_C_NAN]  = sv1_c_nan;
    inputs[btagvar::SV1_CU_NAN] = sv1_cu_nan;

    inputs[btagvar::SV1_EFRC]  = sv1_efrc;
    inputs[btagvar::SV1_MASS]  = sv1_mass;
    inputs[btagvar::SV1_N2T]   = nan_if_placeholder(sv1_n2t);
    inputs[btagvar::SV1_NTRKV] = nan_if_placeholder(sv1_ntrkv);
    inputs[btagvar::SV1_LXY]   = sv1_Lxy;
    inputs[btagvar::SV1_L3D]   = sv1_L3d;
    inputs[btagvar::SV1_SIG3D] = sv1_sig3d;
    inputs[btagvar::SV1_DR]    = sv1_dR;
    inputs[btagvar::SV1_DISTMATLAY]    = sv1_distmatlay;
  }

  void MultivariateTagManager::fill_mvb(var_map& inputs,xAOD::Jet& jet, xAOD::BTagging* BTag) const {
    // Generating MVb variables (as in MV2Tag.cxx)
    std::vector< ElementLink< xAOD::TrackParticleContainer > > IP3DTracks;
    bool trksOK= BTag->variable<std::vector<ElementLink<xAOD::TrackParticleContainer> > > (m_ip3d_infosource, "TrackParticleLinks", IP3DTracks );

    // TODO: improve this part?
    std::vector<float> vectD0, vectD0Signi, vectZ0, vectZ0Signi;
    vectD0.clear(), vectD0Signi.clear(), vectZ0.clear(), vectZ0Signi.clear();

    trksOK &= BTag->variable< std::vector<float> > ("IP3D", "valD0wrtPVofTracks", vectD0     );
    trksOK &= BTag->variable< std::vector<float> > ("IP3D", "sigD0wrtPVofTracks", vectD0Signi);
    trksOK &= BTag->variable< std::vector<float> > ("IP3D", "valZ0wrtPVofTracks", vectZ0     );
    trksOK &= BTag->variable< std::vector<float> > ("IP3D", "sigZ0wrtPVofTracks", vectZ0Signi);

    if (vectD0.size() and vectD0Signi.size() and vectZ0.size() and vectZ0Signi.size()) {
      trksOK &= IP3DTracks.size() == vectD0.size();
      trksOK &= IP3DTracks.size() == vectZ0.size();
      trksOK &= IP3DTracks.size() == vectD0Signi.size();
      trksOK &= IP3DTracks.size() == vectZ0Signi.size();
    }

    int ntrks = IP3DTracks.size();
    float width = NAN;
    int   n_trk_d0cut = INT_MISSING;
    float trk3_d0sig = NAN;
    float trk3_z0sig = NAN;
    float sv_scaled_efc = NAN;
    float jf_scaled_efc = NAN;

    if (trksOK) {
      ATH_MSG_VERBOSE("#BTAG# MV2: calculating MVb inputs.");

      float sum_pt = NAN, sum_pt_dr = NAN;

      std::vector<std::pair<float, float> > trk_d0_z0;
      trk_d0_z0.reserve(IP3DTracks.size());

      unsigned trkIndex=0;
      for(auto trkIter = IP3DTracks.begin(); trkIter != IP3DTracks.end(); ++trkIter) {
  const xAOD::TrackParticle* aTemp = **trkIter;
  TLorentzVector trk;
  trk.SetPtEtaPhiM(aTemp->pt(), aTemp->eta(), aTemp->phi(), 0.);

  // no need for a dedicated selection here, the tracks are already
  // selected by the IP3D algorithm
  const float d0sig = vectD0Signi.at(trkIndex);
  const float z0sig = vectZ0Signi.at(trkIndex);
  trkIndex++;

  if (std::fabs(d0sig) > 1.8){
    if(n_trk_d0cut==INT_MISSING) n_trk_d0cut = 0;
          n_trk_d0cut++;
        }

  // track width components
  if (std::isnan(sum_pt) && std::isnan(sum_pt_dr)) {
    sum_pt = 0., sum_pt_dr = 0.;
  }


        sum_pt += trk.Pt();
  const float dRtoJet = trk.DeltaR(jet.p4());
  sum_pt_dr += dRtoJet * trk.Pt();
  // for 3rd higest d0/z0 significance
  trk_d0_z0.push_back(std::make_pair(d0sig, z0sig));
      } //end of trk loop

      // sort by highest signed d0 sig
      std::sort(trk_d0_z0.begin(), trk_d0_z0.end());
      std::reverse(trk_d0_z0.begin(), trk_d0_z0.end());

      // look up some variables from JF and SV1
      double sv1_efrc = inputs.at(btagvar::SV1_EFRC);
      double sv1_ntrkv = inputs.at(btagvar::SV1_NTRKV);
      double jf_nvtx1t = inputs.at(btagvar::JF_NVTX1T);
      double jf_ntrkv = inputs.at(btagvar::JF_NTRKV);
      double jf_efrc = inputs.at(btagvar::JF_EFRC);

      //Assign MVb variables
      if (sum_pt > 0) width = sum_pt_dr / sum_pt;
      if (trk_d0_z0.size() > 2) trk3_d0sig = trk_d0_z0[2].first;
      if (trk_d0_z0.size() > 2) trk3_z0sig = trk_d0_z0[2].second;
      if (sv1_ntrkv>0) sv_scaled_efc  =  sv1_efrc * (static_cast<float>(ntrks) / sv1_ntrkv);
      if (jf_ntrkv + jf_nvtx1t>0) jf_scaled_efc  =  jf_efrc * (static_cast<float>(ntrks) / (jf_ntrkv + jf_nvtx1t));
    }

    inputs[btagvar::JET_WIDTH]  = width;
    inputs[btagvar::JET_N_TRK3_SIGD0CUT]  = n_trk_d0cut;
    inputs[btagvar::JET_TRK3_D0SIG]  = trk3_d0sig;
    inputs[btagvar::JET_TRK3_Z0SIG]  = trk3_z0sig;
    inputs[btagvar::JET_SV_SCALED_EFC]  = sv_scaled_efc;
    inputs[btagvar::JET_JF_SCALED_EFC]  = jf_scaled_efc;
  }

  void MultivariateTagManager::fill_mv2cl100(var_map& inputs, xAOD::BTagging* BTag) const {
    // Generating MV2cl100 variables
    std::vector< ElementLink< xAOD::TrackParticleContainer > > assocTracks;
    try{
      assocTracks = BTag->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >("BTagTrackToJetAssociator");
    } catch (std::exception& e) {
      ATH_MSG_WARNING("problem loading associated tracks,"
          "skipping this jet");
      return;
    }

    std::vector<ElementLink<xAOD::BTagVertexContainer> > jfvertices;
    try {
      jfvertices =  BTag->auxdata<std::vector<ElementLink<xAOD::BTagVertexContainer> > >("JetFitter_JFvertices");
    } catch (std::exception& e) {
      ATH_MSG_WARNING("problem loading JF vertices,"
          " skipping this jet");
    }

    std::vector<float> fittedPosition = BTag->auxdata<std::vector<float> >("JetFitter_fittedPosition");

    //MV2cl100 default values
    int nTrk_vtx1 = INT_MISSING;
    float mass_first_vtx = NAN;
    float e_first_vtx = NAN;
    float e_frac_vtx1 = NAN;
    float closestVtx_L3D = NAN;
    float JF_Lxy1 = NAN;
    float vtx1_MaxTrkRapidity_jf_path = NAN;
    float vtx1_AvgTrkRapidity_jf_path = NAN;
    float vtx1_MinTrkRapidity_jf_path = NAN;
    float MaxTrkRapidity_jf_path = NAN;
    float MinTrkRapidity_jf_path = NAN;
    float AvgTrkRapidity_jf_path = NAN;

    if (assocTracks.size() > 0 && fittedPosition.size() > 0) {

      ATH_MSG_VERBOSE("#BTAG# MV2: calculating MV2cl100 inputs.");

      float jf_phi = fittedPosition[3];
      float jf_theta = fittedPosition[4];
      TVector3 flightDir(0,0,0);
      flightDir.SetMagThetaPhi(1., jf_theta, jf_phi ); //flight directon of JF decay chain

      //loop over position of JF vertices, find index of secondary vertex
      int secondary_vertex_index = INT_MISSING;
      for (unsigned int jfv = 0; jfv < jfvertices.size(); jfv++) {

  float tmpL3D = fittedPosition[jfv + 5];


  if (tmpL3D > 0 && (closestVtx_L3D > tmpL3D || std::isnan(closestVtx_L3D)) ){

    closestVtx_L3D = tmpL3D;
    secondary_vertex_index = jfv;
  }
      }

      //loop over tracks, collect total 4 momentum, and 4 momentum of secondary vertex, calculate pseudo rapidity of track
      TLorentzVector tracksTot4Mom(0,0,0,0);
      TLorentzVector tracksTot4Mom_firstVtx(0,0,0,0);
      float sumTrackRapidity = 0;
      float vtx1_sumTrackRapidity = 0;
      int vtx1_first_track =0;
      float track_mass = 139.570;
      int trkIndex=0;
      //track loop
      for(auto trkIter = assocTracks.begin(); trkIter != assocTracks.end(); ++trkIter) {
         const xAOD::TrackParticle* aTemp = **trkIter;
         uint8_t getInt(0);
         aTemp->summaryValue(getInt, xAOD::numberOfPixelHits);
         int nSi = getInt;
         aTemp->summaryValue(getInt, xAOD::numberOfSCTHits);
         nSi += getInt;
         if (nSi < 2) continue;

  TLorentzVector trk;
  trk.SetPtEtaPhiM(aTemp->pt(), aTemp->eta(), aTemp->phi(), track_mass);
  tracksTot4Mom += trk;

  TVector3 trkvector(0,0,0);
  trkvector = trk.Vect();

  float trackRapidity = (trkvector.Mag2()>0 ? tan( 0.5*trkvector.Angle(flightDir) ) : 0); // steps to protect against log(0)

  trackRapidity = (trackRapidity < 0.000001 ? (-1)*log(0.000001) : (-1)*log(trackRapidity) ); // value of 0.000001 should provide enough margin for typical values of trackRapidity


  sumTrackRapidity += trackRapidity;

  if(trkIndex==0){
    MaxTrkRapidity_jf_path = trackRapidity;
    MinTrkRapidity_jf_path = trackRapidity;
  }else{
    MaxTrkRapidity_jf_path = trackRapidity > MaxTrkRapidity_jf_path ? trackRapidity : MaxTrkRapidity_jf_path;
    MinTrkRapidity_jf_path = trackRapidity < MinTrkRapidity_jf_path ? trackRapidity : MinTrkRapidity_jf_path;
  }

  if(secondary_vertex_index >= 0){
    //get track links to secondary vertex
    const xAOD::BTagVertex *tmpVertex = *(jfvertices.at(secondary_vertex_index));
    const std::vector< ElementLink<xAOD::TrackParticleContainer> > tmpVect = tmpVertex->track_links();
    //check association to JF vertex
    int particleInCollection = 0;

    for (unsigned int iT = 0; iT < tmpVect.size(); iT++) {
      if (aTemp == *(tmpVect.at(iT))) particleInCollection=1;
    }
      if (particleInCollection){

        if(nTrk_vtx1 < 0){
          nTrk_vtx1 = 0;
        }
        nTrk_vtx1 += 1;

        tracksTot4Mom_firstVtx += trk;
        vtx1_sumTrackRapidity += trackRapidity;

        if(!vtx1_first_track){
          vtx1_MaxTrkRapidity_jf_path = trackRapidity;
          vtx1_MinTrkRapidity_jf_path = trackRapidity;
          vtx1_first_track=1;
        }else{
          vtx1_MaxTrkRapidity_jf_path = trackRapidity > vtx1_MaxTrkRapidity_jf_path ? trackRapidity : vtx1_MaxTrkRapidity_jf_path;
          vtx1_MinTrkRapidity_jf_path = trackRapidity < vtx1_MinTrkRapidity_jf_path ? trackRapidity : vtx1_MinTrkRapidity_jf_path;
        }

      }
  }

  trkIndex++;
      } //end of trk loop

        // assign the remaining MV2cl100 variables
      AvgTrkRapidity_jf_path = trkIndex > 0 ? sumTrackRapidity/trkIndex : 0;

    if(nTrk_vtx1 > 0){
      JF_Lxy1 = closestVtx_L3D*sin(jf_theta);
      mass_first_vtx = tracksTot4Mom_firstVtx.M();
      e_first_vtx = tracksTot4Mom_firstVtx.E();
      e_frac_vtx1 = e_first_vtx/tracksTot4Mom.E();
      vtx1_AvgTrkRapidity_jf_path = vtx1_sumTrackRapidity/nTrk_vtx1;
    }

    } // end of if (assocTracks.size() > 0 && fittedPosition.size() > 0

    inputs[btagvar::JF_NTRK_VTX1]           = nan_if_placeholder(nTrk_vtx1);
    inputs[btagvar::JF_MASS_VTX1 ]          = mass_first_vtx ;
    inputs[btagvar::JF_E_VTX1 ]             = e_first_vtx ;
    inputs[btagvar::JF_E_FRAC_VTX1 ]        = e_frac_vtx1;
    inputs[btagvar::JF_L3D_VTX1 ]           = closestVtx_L3D ;
    inputs[btagvar::JF_Lxy_VTX1 ]           = JF_Lxy1 ;
    inputs[btagvar::JF_RAPIDITY_VTX1_MAX ]  = vtx1_MaxTrkRapidity_jf_path;
    inputs[btagvar::JF_RAPIDITY_VTX1_AVG ]  = vtx1_AvgTrkRapidity_jf_path;
    inputs[btagvar::JF_RAPIDITY_VTX1_MIN ]  = vtx1_MinTrkRapidity_jf_path;
    inputs[btagvar::JF_RAPIDITY_MAX ]       = MaxTrkRapidity_jf_path;
    inputs[btagvar::JF_RAPIDITY_MIN ]       = MinTrkRapidity_jf_path;
    inputs[btagvar::JF_RAPIDITY_AVG ]       = AvgTrkRapidity_jf_path;

  } // end of fill_mv2cl100

  void MultivariateTagManager::fill_arbitrary_aux_data(
    var_map& inputs, xAOD::BTagging* BTag) const {

    for (const auto& raw_key: m_arbitrary_aux_data) {
      auto key = raw_key;
      if (m_aux_data_name_map.count(raw_key)) {
        key = m_aux_data_name_map.at(raw_key);
      }
      // note: we should extend this to data types beyond double at
      // some point
      std::string valid_key = raw_key + "IsValid";
      if ( ! BTag->isAvailable<double>(raw_key) ) {
        ATH_MSG_WARNING("aux data '" + raw_key + "' is missing,"
                        " tagger inputs may be incomplete");
      } else if (!BTag->isAvailable<char>(valid_key)) {
        ATH_MSG_WARNING("no key '" + valid_key + "' found, invalid inputs"
                        " may be interperated incorrectly");
        inputs[key] = BTag->auxdata<double>(raw_key);
      } else if (!BTag->auxdata<char>(valid_key)) {
        inputs[key] = NAN;
      } else {
        inputs[key] = BTag->auxdata<double>(raw_key);
      }
    }

  }


} // end vp namespace

namespace {
  double nan_if_placeholder(int in) {
    if (in == INT_MISSING) return NAN;
    return in;
  }
}

//  LocalWords:  vertices
