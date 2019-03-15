/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RIO_OnTrackErrorScalingTool.cxx
//   AlgTool for modifying (inflating) the measurement errors of
//   RIO_onTrack objects directly in the detector-specific creators.
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkRIO_OnTrackCreator/RIO_OnTrackErrorScalingTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ListItem.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/errorcheck.h"
#include "InDetIdentifier/PixelID.h"

#include "xAODEventInfo/EventInfo.h"

#include <cmath>
#include <stdexcept>
#include <sstream>

const char *Trk::RIO_OnTrackErrorScalingTool::s_pix_names[Trk::RIO_OnTrackErrorScalingTool::kNPixParamTypes]={
    "PixPhi Barrel",
    "PixEta Barrel",
    "PixPhi IBL",
    "PixEta IBL",
    "PixPhi Endcap",
    "PixEta Endcap",
  };

const int Trk::RIO_OnTrackErrorScalingTool::s_pix_idx[Trk::RIO_OnTrackErrorScalingTool::kNPixParamTypes]={
    kPixBarrelPhi,
    kPixBarrelEta,
    kPixIBLPhi,
    kPixIBLEta,
    kPixEndcapPhi,
    kPixEndcapEta};


// constructor
Trk::RIO_OnTrackErrorScalingTool::RIO_OnTrackErrorScalingTool(const std::string& t,
            const std::string& n,
            const IInterface* p)
  :  AthAlgTool(t,n,p),
     m_do_pix(false),
     m_do_sct(false),
     m_do_trt(false),
     m_do_mdt(false),
     m_do_tgc(false),
     m_do_rpc(false),
     m_do_csc(false),
     m_scaling_sct_barrel(std::vector<double>(0)),
     m_scaling_sct_endcap(std::vector<double>(0)),
     m_scaling_trt_barrel(std::vector<double>(0)),
     m_scaling_trt_endcap(std::vector<double>(0)),
     m_scaling_mdt_barrel(std::vector<double>(0)),
     m_scaling_mdt_endcap(std::vector<double>(0)),
     m_scaling_tgcPhi(std::vector<double>(0)),
     m_scaling_tgcEta(std::vector<double>(0)),
     m_scaling_rpcPhi(std::vector<double>(0)),
     m_scaling_rpcEta(std::vector<double>(0)),
     m_scaling_cscPhi(std::vector<double>(0)),
     m_scaling_cscEta(std::vector<double>(0)),
     m_override_database_id_errors(false),
     m_doTRTScaling(true),
     m_override_scale_inflation_pix_bar_x(10.),
     m_override_scale_inflation_pix_bar_y(10.),
     m_override_scale_inflation_pix_ecs_x(10.),
     m_override_scale_inflation_pix_ecs_y(10.),
     m_override_scale_inflation_sct_bar(10.),
     m_override_scale_inflation_sct_ecs(10.),
     m_override_scale_inflation_trt_bar(10.),
     m_override_scale_inflation_trt_ecs(10.),
     m_override_constant_term_pix_bar_x(0.0044),
     m_override_constant_term_pix_bar_y(0.0312),
     m_override_constant_term_pix_ecs_x(0.026),
     m_override_constant_term_pix_ecs_y(0.0),
     m_override_constant_term_sct_bar(0.0065),
     m_override_constant_term_sct_ecs(0.0071),
     m_override_constant_term_trt_bar(0.0),
     m_override_constant_term_trt_ecs(0.0),
     m_override_mu_term_trt_bar(0.0),
     m_override_mu_term_trt_ecs(0.0),
     m_IncidentSvc("IncidentSvc",n),
     m_idFolder("/Indet/TrkErrorScaling"),
     m_muonFolder("/MUON/TrkErrorScaling")
 {
   declareInterface<IRIO_OnTrackErrorScalingTool>(this);
   declareProperty("overrideDatabaseID",m_override_database_id_errors,"inflate ID errors by multiplicative scale factor, overriding any database values");
   declareProperty("doTRTErrorScaling",m_doTRTScaling,"Turn off or on the TRT Error scaling");
   declareProperty("overrideScalePix",m_override_scale_inflation_pix_bar_x,"factor to inflate pixel errors (barrel local x)");
   declareProperty("overrideScalePixBarX",m_override_scale_inflation_pix_bar_x,"factor to inflate pixel errors (barrel local x)");
   declareProperty("overrideScalePixBarY",m_override_scale_inflation_pix_bar_y,"factor to inflate pixel errors (barrel local y)");
   declareProperty("overrideScalePixECsX",m_override_scale_inflation_pix_ecs_x,"factor to inflate pixel errors (endcaps local x)");
   declareProperty("overrideScalePixECsY",m_override_scale_inflation_pix_ecs_y,"factor to inflate pixel errors (endcaps local y)");
   declareProperty("overrideScaleSCT",m_override_scale_inflation_sct_bar,"factor to inflate SCT errors in the barrel");
   declareProperty("overrideScaleSCTBar",m_override_scale_inflation_sct_bar,"factor to inflate SCT errors in the barrel");
   declareProperty("overrideScaleSCTECs",m_override_scale_inflation_sct_ecs,"factor to inflate SCT errors in the end caps");
   declareProperty("overrideScaleTRT",m_override_scale_inflation_trt_bar,"factor to inflate TRT errors");
   declareProperty("overrideScaleTRTBar",m_override_scale_inflation_trt_bar,"factor to inflate TRT errors (barrel)");
   declareProperty("overrideScaleTRTECs",m_override_scale_inflation_trt_ecs,"factor to inflate TRT errors (end caps)");
   declareProperty("overrideConstantPixBarX",m_override_constant_term_pix_bar_x,"factor to change the constant term of pixel errors (barrel local x)");
   declareProperty("overrideConstantPixBarY",m_override_constant_term_pix_bar_y,"factor to change the constant term of pixel errors (barrel local y)");
   declareProperty("overrideConstantPixECsX",m_override_constant_term_pix_ecs_x,"factor to change the constant term of pixel errors (endcaps local x)");
   declareProperty("overrideConstantPixECsY",m_override_constant_term_pix_ecs_y,"factor to change the constant term of pixel errors (endcaps local y)");
   declareProperty("overrideConstantSCTBar",m_override_constant_term_sct_bar,"factor to change the constant term of SCT errors (barrel)");
   declareProperty("overrideConstantSCTECs",m_override_constant_term_sct_ecs,"factor to change the constant term of SCT errors (end caops)");
   declareProperty("overrideConstantTRTBar",m_override_constant_term_trt_bar,"factor to change the constant term of TRT errors (barrel)");
   declareProperty("overrideConstantTRTECs",m_override_constant_term_trt_ecs,"factor to change the constant term of TRT errors (endcaps)");
   declareProperty("overrideMuTRTBar",m_override_mu_term_trt_bar,"factor to change the mu dependent term of TRT errors (barrel)");
   declareProperty("overrideMuTRTECs",m_override_mu_term_trt_ecs,"factor to change the mu dependent term of TRT errors (endcaps)");
   declareProperty("IncidentService",m_IncidentSvc);
   m_scaling_pix.resize(kNPixParamTypes);

   m_mu = 0;
   m_hasBeenCalledThisEvent = false;
}

// destructor
Trk::RIO_OnTrackErrorScalingTool::~RIO_OnTrackErrorScalingTool()
{ }

// initialise
StatusCode Trk::RIO_OnTrackErrorScalingTool::initialize()
{
  if (StatusCode::SUCCESS!=AlgTool::initialize())
    return StatusCode::FAILURE;
 
  CHECK(  detStore()->retrieve(m_pixelID, "PixelID") );

  //Incident service (to check for MC/data and setup accordingly)
  if ( m_IncidentSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << m_IncidentSvc );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_DEBUG( "Retrieved service " << m_IncidentSvc);
  
  m_IncidentSvc->addListener( this, std::string("BeginEvent"));


    
  /* set up scaling factors for error matrices (use their **2 for cov)
   */

  assert( m_scaling_pix.size() == kNPixParamTypes );
  // --- check scaling factors
  
  assert( kNPixParamTypes%2 == 0);
  for (int i=0; i <kNPixParamTypes; ++i) {
    registerParameters(m_do_pix, &(m_scaling_pix[ s_pix_idx[i] ]), s_pix_names[i] );
  }
                   
 
  ATH_MSG_INFO( "Apply TRT error scaling? " << m_doTRTScaling );

  registerParameters(m_do_sct,
                    &m_scaling_sct_barrel,"SCT Barrel");
  registerParameters(m_do_sct,
                    &m_scaling_sct_endcap,"SCT Endcap");

  registerParameters(m_do_trt,
                    &m_scaling_trt_barrel,"TRT Barrel");
  registerParameters(m_do_trt,
                    &m_scaling_trt_endcap,"TRT Endcap");

  registerParameters(m_do_mdt,
                    &m_scaling_mdt_barrel,"MDT Barrel");
  registerParameters(m_do_mdt,
                    &m_scaling_mdt_endcap,"MDT Endcap");

  registerParameters(m_do_tgc,
                    &m_scaling_tgcPhi,"TGC Phi");
  registerParameters(m_do_tgc,
                    &m_scaling_tgcEta,"TGC Eta");

  registerParameters(m_do_rpc,
                    &m_scaling_rpcPhi,"RPC Phi");
  registerParameters(m_do_rpc,
                    &m_scaling_rpcEta,"RPC Eta");

  registerParameters(m_do_csc,
                    &m_scaling_cscPhi,"CSC Phi");
  registerParameters(m_do_csc,
                    &m_scaling_cscEta,"CSC Eta");

  // find and register callback function for ID
  const DataHandle<CondAttrListCollection> colptr;
  if (detStore()->contains<CondAttrListCollection>(m_idFolder)) {
    if (StatusCode::SUCCESS!=detStore()->
        regFcn(&Trk::RIO_OnTrackErrorScalingTool::callback,
               this,colptr,m_idFolder)) {
      msg(MSG::ERROR) << "Found the folder, but could not register a callback"
          << " on " << m_idFolder << endreq;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Registered callback on COOL folder " << m_idFolder << endreq;
  } else {
    msg(MSG::INFO) << "Folder " << m_idFolder << " is not loaded, "
          << "intrinsic meas't errors will be used for ID tracks." << endreq;
    m_do_pix = false;
    m_do_sct = false;
    m_do_trt = false;
  }
  if (m_override_database_id_errors) {
    msg(MSG::INFO) << " ignoring COOL; forcing scaling of intrinsic measurment errors in ID." << endreq;
    m_do_pix = true;
    m_do_sct = true;
    m_do_trt = true;
  }

  // find and register callback function for Muons
  if (detStore()->contains<CondAttrListCollection>(m_muonFolder)) {
    if (StatusCode::SUCCESS!=detStore()->
        regFcn(&Trk::RIO_OnTrackErrorScalingTool::callback,
               this,colptr,m_muonFolder)) {
      msg(MSG::ERROR) << "Found the folder, but could not register a callback"
          << " on " << m_muonFolder << endreq;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Registered callback on COOL folder " 
         << m_muonFolder << endreq;
  } else {
    msg(MSG::INFO) << "Folder " << m_muonFolder << " is not loaded, "
       << "intrinsic meas't errors will be used for Muon RIOs_OnTrack." << endreq;
    m_do_mdt = false;
    m_do_tgc = false;
    m_do_rpc = false;
    m_do_csc = false;
  }

  msg(MSG::INFO) << "initialize successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

MsgStream& Trk::RIO_OnTrackErrorScalingTool::dump( MsgStream& out ) const
{
  //--- need an sprintf here.
  out << std::endl << name() << " has found the following factors and constants to scale errors:"  <<std::endl;
  out << "| Detector System                factor   constant            |"           <<std::endl;
  out << "+-------------------------------------------------------------+"           <<std::endl;

  for (int i=0; i<kNPixParamTypes; ++i) {
    out << makeInfoString(s_pix_names[i],m_do_pix,m_scaling_pix[ s_pix_idx[i] ]) <<std::endl;
  }
  out << makeInfoString("SCT barrel    ",m_do_sct,m_scaling_sct_barrel) <<std::endl;
  out << makeInfoString("SCT barrel    ",m_do_sct,m_scaling_sct_barrel) <<std::endl;
  out << makeInfoString("TRT barrel    ",m_do_trt,m_scaling_trt_barrel) <<std::endl;
  out << makeInfoString("TRT endcap    ",m_do_trt,m_scaling_trt_endcap) <<std::endl;
  out << makeInfoString("MDT barrel    ",m_do_mdt,m_scaling_mdt_barrel) <<std::endl;
  out << makeInfoString("MDT endcap    ",m_do_mdt,m_scaling_mdt_endcap) <<std::endl;
  out << makeInfoString("RPC Phi barrel",m_do_rpc,m_scaling_rpcPhi)     <<std::endl;
  out << makeInfoString("RPC Eta barrel",m_do_rpc,m_scaling_rpcEta)     <<std::endl;
  out << makeInfoString("TGC Phi endcap",m_do_tgc,m_scaling_tgcPhi)     <<std::endl;
  out << makeInfoString("TGC Eta endcap",m_do_tgc,m_scaling_tgcEta)     <<std::endl;
  out << makeInfoString("CSC Phi endcap",m_do_csc,m_scaling_cscPhi)     <<std::endl;
  out << makeInfoString("CSC Eta endcap",m_do_csc,m_scaling_cscEta)     <<std::endl;
  out << "+-------------------------------------------------------------+"           <<std::endl;
  return out;
}

// handle BeginRun incidents------------------------------------------------
void Trk::RIO_OnTrackErrorScalingTool::handle(const Incident& inc)
{
  if (inc.type() == "BeginEvent") {
    m_hasBeenCalledThisEvent =  false;
    m_mu=0;
  }
}


const std::string Trk::RIO_OnTrackErrorScalingTool::makeInfoString 
(const std::string& sdet, const bool do_detSystem,
 const std::vector<double>& errscaler) const {
  
  const int nformat=62;
  std::string s1("| ");
  s1.append(sdet);
  if (do_detSystem && (errscaler.size()>0) ) {
      s1.append(" - scaled by ");
  } else {
      s1.append(" - unscaled. ");
  }
  if (errscaler.size() < 2 && do_detSystem)
    s1.append("WARNING, scaling active but empty vector of a,c values!");
  if (errscaler.size() > 1) {
    char s2[7];
    snprintf(s2,7,"%6.3g ",errscaler[0]);s1.append(s2);
    s1.append("* err (+) ");
    snprintf(s2,7,"%6.3g ",errscaler[1]);s1.append(s2);
  }

  if (errscaler.size() > 2){
    s1.append(" * (1 + mu *");
    char s2[7];
    snprintf(s2,7,"%6.3g ",errscaler[2]);s1.append(s2);
    s1.append(")");
  }

  int n = nformat-s1.size();
  for(int i=0; i<n; ++i) s1.append(" ");
  s1.append("|");
  return s1;
}

void Trk::RIO_OnTrackErrorScalingTool::registerParameters
           (bool&         do_detSystem,
            std::vector<double>* errscaler,
            const std::string& sdet) {
  // add this set of parameters to list, always set the parameters to be
  // 'used' for now
  do_detSystem=true;
  m_parammap.insert(ParamMap::value_type(sdet,errscaler));
  // set default parameters in case no info is read from condDB for this set
  errscaler->clear();
  errscaler->push_back(1.0);
  errscaler->push_back(0.0);
  errscaler->push_back(0.0);
  //    do_detSystem = false;
  //    if (errscaler[0] != 1.0) do_detSystem = true;
  //    if (errscaler[1] != 0.0) do_detSystem = true;
}


//finalise
StatusCode Trk::RIO_OnTrackErrorScalingTool::finalize()
{
  msg(MSG::INFO)  << "finalize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

void Trk::RIO_OnTrackErrorScalingTool::scale2by2
(              Amg::MatrixX& cov, 
               const std::vector<double>& phiTrafo,
               const std::vector<double>& etaTrafo) const {
  // careful not to change character of error
  double corr = (fabs(cov(0,1)) > 1.0e-20)             ?
    cov(0,1)/(cov(0,0)*cov(1,1)) :
                0.0;
  cov(0,0) *= phiTrafo[0]*phiTrafo[0];
  cov(0,0) += phiTrafo[1]*phiTrafo[1];
  cov(1,1) *= etaTrafo[0]*etaTrafo[0];
  cov(1,1) += etaTrafo[1]*etaTrafo[1];
  if (corr !=0.0) {
    cov(0,1) = corr*cov(0,0)*cov(1,1);
    cov(1,0) = cov(0,1);
  }
}

Amg::MatrixX* 
Trk::RIO_OnTrackErrorScalingTool::createScaledPixelCovariance
  (const Amg::MatrixX& inputCov, const Identifier& id) const
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  assert( m_pixelID );

  // from SiDetectorElement::isEndcap
  bool is_endcap = !(m_pixelID->is_barrel(id) || m_pixelID->is_dbm(id));
  unsigned int idx=kNPixParamTypes;
  
  if (is_endcap) {
    idx=kPixEndcapPhi;
  } else {

    // from  PixelDigitizationTool::getReadoutTech
    int barrel_ec = m_pixelID->barrel_ec(id);
    bool is_ibl = abs(barrel_ec)==4 || m_pixelID->eta_module_max(id)>6;
    if (is_ibl) {
      idx = kPixIBLPhi;
    }
    else {
      idx = kPixBarrelPhi;
    }
  }

  assert( m_scaling_pix.size()>idx+1);
  if (m_scaling_pix[idx].size()<2 ) {
    assert( idx < kNPixParamTypes);

    std::stringstream message;
    message << "ERROR " << name() << " createScaledPixelCovariance : No error scaling factors for  "  << s_pix_names[idx] 
            << " or " << s_pix_names[idx+1]
            << ".";
    throw std::runtime_error( message.str() );
  }

  assert(   m_scaling_pix[idx].size()>1
         && m_scaling_pix[idx+1].size()>1 );

  scale2by2(*newCov,m_scaling_pix[idx] /* phi */ ,m_scaling_pix[idx+1] /* eta */);
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "changing original Pix-" << (idx==kPixEndcapPhi ? "EC " : (idx==kPixIBLPhi ? "IBL" : "BR " )) << " cov " << endreq;
    msg(MSG::VERBOSE) << inputCov << " to "<< *newCov
                      <<" by:" << m_scaling_pix[idx][0]
                      <<", " << m_scaling_pix[idx][1]
                      <<" / "<< m_scaling_pix[idx+1][0]
                      <<", " << m_scaling_pix[idx+1][1] << endreq;
  }

  return newCov;
}

Amg::MatrixX* 
Trk::RIO_OnTrackErrorScalingTool::createScaledSctCovariance
(const Amg::MatrixX& inputCov, bool is_endcap,
double sinLocalAngle) const
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  if (is_endcap && newCov->rows() > 1) {
    double Sn      = sinLocalAngle; 
    double Sn2     = Sn*Sn;
    double Cs2     = (1.-Sn)*(1.+Sn);
    double SC      = Sn*sqrt(Cs2);
    double a       = m_scaling_sct_endcap[0];
    double b       = m_scaling_sct_endcap[1];
    double dV0     = (Cs2*(*newCov)(0,0)+Sn2*(*newCov)(1,1)
                      +2.*SC*(*newCov)(1,0))*(a*a-1.) + b*b;
    (*newCov)(0,0)+= (Cs2*dV0);
    (*newCov)(1,0)+= (SC *dV0);
    (*newCov)(0,1) = (*newCov)(1,0);
    (*newCov)(1,1)+= (Sn2*dV0);
  } else {
    double a = (is_endcap) ? m_scaling_sct_endcap[0] : m_scaling_sct_barrel[0];
    double b = (is_endcap) ? m_scaling_sct_endcap[1] : m_scaling_sct_barrel[1];
    (*newCov)(0,0) *= a*a;
    (*newCov)(0,0) += b*b;
  }
  return newCov;
}

Amg::MatrixX* 
Trk::RIO_OnTrackErrorScalingTool::createScaledTrtCovariance
  (const Amg::MatrixX& inputCov, bool is_endcap) const
{
  if(!m_hasBeenCalledThisEvent){
    const xAOD::EventInfo* eventInfo;
    if( evtStore()->retrieve(eventInfo).isFailure() ){
      ATH_MSG_ERROR("Cant retrieve EventInfo"); m_mu = 0;
    } else {
      m_mu = eventInfo->averageInteractionsPerCrossing();
    }
    m_hasBeenCalledThisEvent = true;
  }


  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  if (!m_doTRTScaling) 	return newCov;
  double a = (is_endcap) ? m_scaling_trt_endcap[0] : m_scaling_trt_barrel[0];
  double b = (is_endcap) ? m_scaling_trt_endcap[1] : m_scaling_trt_barrel[1];
  double c = (is_endcap) ? m_scaling_trt_endcap[2] : m_scaling_trt_barrel[2];
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  (*newCov)(0,0) *= (1. + m_mu * c); 
  return newCov;
}

Amg::MatrixX* 
Trk::RIO_OnTrackErrorScalingTool::createScaledMdtCovariance
  (const Amg::MatrixX& inputCov, bool is_endcap) const
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a = (is_endcap) ? m_scaling_mdt_endcap[0] : m_scaling_mdt_barrel[0];
  double b = (is_endcap) ? m_scaling_mdt_endcap[1] : m_scaling_mdt_barrel[1];
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}

Amg::MatrixX* 
Trk::RIO_OnTrackErrorScalingTool::createScaledTgcCovariance
(const Amg::MatrixX& inputCov,
 const Trk::ParamDefs measuredCoord) const // exists only as end-cap thing
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a,b = 0.0;
  if (measuredCoord == Trk::distPhi) {
    a = m_scaling_tgcPhi[0];
    b = m_scaling_tgcPhi[1];
  } else if (measuredCoord == Trk::distEta) {
    a = m_scaling_tgcEta[0];
    b = m_scaling_tgcEta[1];
  } else {
    delete newCov;
    msg(MSG::WARNING) << "Wrong TGC measurement coordinate definition given to RIO_OnTrackErrorScalingTool." << endreq;
    msg(MSG::WARNING) << "Error scaling cancelled, cov=NULL." << endreq;
    msg(MSG::INFO) << "Allowed definitions: Trk::distPhi, Trk::distEta." << endreq;
    return 0;
  }
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}

Amg::MatrixX* 
Trk::RIO_OnTrackErrorScalingTool::createScaledRpcCovariance
(const Amg::MatrixX& inputCov,
 const Trk::ParamDefs measuredCoord ) const // exists only as barrel thing
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a,b = 0.0;
  if (measuredCoord == Trk::distPhi) {
    a = m_scaling_rpcPhi[0];
    b = m_scaling_rpcPhi[1];
  } else if (measuredCoord == Trk::distEta) {
    a = m_scaling_rpcEta[0];
    b = m_scaling_rpcEta[1];
  } else {
    delete newCov;
    msg(MSG::WARNING) << "Wrong RPC measurement coordinate definition given to RIO_OnTrackErrorScalingTool." << endreq;
    msg(MSG::WARNING) << "Error scaling cancelled, cov=NULL." << endreq;
    msg(MSG::INFO) << "Allowed definitions: Trk::distPhi, Trk::distEta." << endreq;
    return 0;
  }
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}

Amg::MatrixX* 
Trk::RIO_OnTrackErrorScalingTool::createScaledCscCovariance
(const Amg::MatrixX& inputCov,
 const Trk::ParamDefs measuredCoord) const // is a far-fwd technology
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a,b = 0.0;
  if (measuredCoord == Trk::distPhi) {
    a = m_scaling_cscPhi[0];
    b = m_scaling_cscPhi[1];
  } else if (measuredCoord == Trk::distEta) {
    a = m_scaling_cscEta[0];
    b = m_scaling_cscEta[1];
  } else {
    delete newCov;
    msg(MSG::WARNING) << "Wrong CSC measurement coordinate definition given to RIO_OnTrackErrorScalingTool." << endreq;
    msg(MSG::WARNING) << "Error scaling cancelled, cov=NULL." << endreq;
    msg(MSG::INFO) << "Allowed definitions: Trk::distPhi, Trk::distEta." << endreq;
    return 0;
  }
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}


StatusCode Trk::RIO_OnTrackErrorScalingTool::callback( 
            IOVSVC_CALLBACK_ARGS_P(I,keys) ) {
  (void) I;

  // callback function when the conditions data object changes
  // loop over all the keys, action for the one we are interested in
  for (std::list<std::string>::const_iterator itr=keys.begin();itr!=keys.end();
       ++itr) {
    if (*itr==m_idFolder || *itr==m_muonFolder) {
      ATH_MSG_DEBUG ("Callback function invoked! (Folder: " << *itr << ")");
      const CondAttrListCollection* atrlistcol=0;
      if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol,*itr)) {
        // loop through elements of the attribute list collection
        for (CondAttrListCollection::const_iterator citr=atrlistcol->begin();
             citr!=atrlistcol->end();++citr) {
          // attribute list format is name(string), n(int) followed by n double
          const coral::AttributeList& alist=citr->second;
          try {
            const std::string& name=alist[0].data<std::string>();
            const int nvals=alist[1].data<int>();
            // locate pointer to parameters vector, based on name
            // note that names unknown to the service are ignored
            ParamMap::const_iterator loc=m_parammap.find(name);
            if (loc!=m_parammap.end()) {
              std::vector<double>* params=loc->second;
              ATH_MSG_DEBUG ("Setting " << nvals << " parameters for " << name << " at location " << params);
              params->clear();
              for (int i=0;i<nvals;++i){
                ATH_MSG_VERBOSE("Parameter " << i << " = " << alist[2+i].data<double>() );
                params->push_back(alist[2+i].data<double>());
              }
            } else {
              ATH_MSG_DEBUG ("Ignored unexpected parameter" << name);
            }
          }
          // catch problems with the attributelist accesses
          catch (coral::Exception& e) {
            msg(MSG::ERROR) << "Problem with AttributeList decoding: " << e.what() << endreq;
            return StatusCode::FAILURE;
          }
        }
      } else {
        msg(MSG::ERROR) << "Problem reading conditions object " << *itr << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  for (int i=0; i < kNPixParamTypes-1; i+=2 ) {
    bool has_eta_scaling=(m_scaling_pix[i+1].size()>0);
    bool has_phi_scaling=(m_scaling_pix[i].size()>0);
    if (has_eta_scaling != has_phi_scaling) {
      ATH_MSG_ERROR( "Missing pixel scaling factors for " 
                     << s_pix_names[i] << "[" << m_scaling_pix[i].size() << " params]"
                     << " or " << s_pix_names[i+1]  << "[" << m_scaling_pix[i+1].size() << " params]"
                     << ".");
      return StatusCode::FAILURE;
    }
  }

  // if requested, scale up the ID errors by the values in job options
  if (m_override_database_id_errors) {
    msg(MSG::WARNING) << "WARNING overriding database error scaling parameters with values specified by job options; see "
                      << __FILE__ << " at line " << __LINE__ << endreq;
    m_do_pix = true;
    m_do_sct = true;
    m_do_trt = true;
    std::vector<double> scale(3,0);

    // pixel barrel 
    // local x (phi -> old convetion)
    scale[0] = m_override_scale_inflation_pix_bar_x;
    scale[1] = m_override_constant_term_pix_bar_x; // 0.0044;
    m_scaling_pix[kPixBarrelPhi] = scale;
    m_scaling_pix[kPixIBLPhi] = scale;

    // local y (eta -> old convention)
    scale[0] = m_override_scale_inflation_pix_bar_y;
    scale[1] = m_override_constant_term_pix_bar_y; // 0.0312;
    m_scaling_pix[kPixBarrelEta] = scale;
    m_scaling_pix[kPixIBLEta] = scale;

    // pixel end caps
    // local x (phi -> old convetion)
    scale[0] = m_override_scale_inflation_pix_bar_x;
    scale[1] = m_override_constant_term_pix_ecs_x; // 0.0026;
    m_scaling_pix[kPixEndcapPhi] = scale;

    // local y (eta -> old convention)
    scale[0] = m_override_scale_inflation_pix_bar_y;
    scale[1] = m_override_constant_term_pix_ecs_y; // 0.;
    m_scaling_pix[kPixEndcapEta] = scale;

    // sct barrel
    scale[0] = m_override_scale_inflation_sct_bar;
    scale[1] = m_override_constant_term_sct_bar; // 0.0065
    m_scaling_sct_barrel = scale;

    // sct end caps
    scale[0] = m_override_scale_inflation_sct_ecs;
    scale[1] = m_override_constant_term_sct_ecs; // 0.0071;
    m_scaling_sct_endcap = scale;

    // trt barrel
    scale[0] = m_override_scale_inflation_trt_bar;
    scale[1] = m_override_constant_term_trt_bar; // 0.;
    scale[2] = m_override_mu_term_trt_bar;
    m_scaling_trt_barrel = scale;

    // trt end caps
    scale[0] = m_override_scale_inflation_trt_ecs;
    scale[1] = m_override_constant_term_trt_ecs; // 0.;
    scale[2] = m_override_mu_term_trt_ecs;
    m_scaling_trt_endcap = scale;
  }

  // printout new constants if in debug print mode
  ATH_MSG_DEBUG ( (*this) );
  return StatusCode::SUCCESS;
}
