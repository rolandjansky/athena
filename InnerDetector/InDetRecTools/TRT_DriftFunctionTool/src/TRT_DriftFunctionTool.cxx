/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DriftFunctionTool.cxx
//   Implementation file for class TRT_DriftFunctionTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used to go from drift time to drift distance
///////////////////////////////////////////////////////////////////


#include "TRT_DriftFunctionTool/TRT_DriftFunctionTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetReadoutGeometry/Version.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <math.h> 
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
//
// Constructor
TRT_DriftFunctionTool::TRT_DriftFunctionTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_TRTCalDbTool("TRT_CalDbTool",this),
    m_TRTCalDbTool2("",this),
    m_drifttimeperbin(3.125 * CLHEP::ns),
    m_error(0.17),
    m_drifttimeperhalfbin(0.), // set later
    m_isdata(false),
    m_ismc(true),
    m_isoverlay(false),
    m_istestbeam(false),
    m_allow_data_mc_override(true),
    m_forcedata(true),
    m_dummy(false),
    m_err_fudge(1.0),
    m_allow_digi_version_override(false),
    m_forced_digiversion(11),
    m_override_simcal(false),
    m_force_universal_errors(false),
    m_uni_error(0.136),
    m_inputfile(""),
    m_key(""),
    m_trt_mgr_location("TRT"),
    m_ht_correction_barrel_Xe(0.0), // initialised from python
    m_ht_correction_endcap_Xe(0.0), // initialised from python
    m_ht_correction_barrel_Ar(0.0), // initialised from python
    m_ht_correction_endcap_Ar(0.0), // initialised from python
    m_tot_corrections_barrel_Xe(20, 0.), // initialised from python
    m_tot_corrections_endcap_Xe(20, 0.), // initialised from python
    m_tot_corrections_barrel_Ar(20, 0.), // initialised from python
    m_tot_corrections_endcap_Ar(20, 0.) // initialised from python
{
  declareInterface<ITRT_DriftFunctionTool>(this);
  m_drifttimeperhalfbin = m_drifttimeperbin/2.;
  declareProperty("IsMC",m_ismc);
  declareProperty("AllowDigiVersionOverride",m_allow_digi_version_override);
  declareProperty("ForcedDigiVersion",m_forced_digiversion);
  declareProperty("AllowDataMCOverride",m_allow_data_mc_override);
  declareProperty("ForceData",m_forcedata);
  declareProperty("IsOverlay",m_isoverlay=false);
  declareProperty("OverrideSimulationCalibration",m_override_simcal);
  declareProperty("ForceUniversalErrors",m_force_universal_errors);
  declareProperty("UniversalError",m_uni_error);
  declareProperty("DummyMode",m_dummy);
  declareProperty("ErrorFudgeFactor",m_err_fudge);
  declareProperty("TRTCalDbTool", m_TRTCalDbTool);
  declareProperty("TRTCalDbTool2", m_TRTCalDbTool2);
  declareProperty("DriftFunctionFile", m_inputfile);
  declareProperty("TrtDescrManageLocation",m_trt_mgr_location);
  declareProperty("ToTCorrectionsBarrelXe",m_tot_corrections_barrel_Xe);
  declareProperty("ToTCorrectionsEndcapXe",m_tot_corrections_endcap_Xe);
  declareProperty("ToTCorrectionsBarrelAr",m_tot_corrections_barrel_Xe);
  declareProperty("ToTCorrectionsEndcapAr",m_tot_corrections_endcap_Xe);
  declareProperty("HTCorrectionBarrelXe",m_ht_correction_barrel_Xe);
  declareProperty("HTCorrectionEndcapXe",m_ht_correction_endcap_Xe);
  declareProperty("HTCorrectionBarrelAr",m_ht_correction_barrel_Ar);
  declareProperty("HTCorrectionEndcapAr",m_ht_correction_endcap_Ar);

  // make sure all arrays are initialized - use DC3version2 as default
  for (int i=0; i<3; i++) m_t0_barrel[i] = 15.625;
  for (int i=0; i<14; i++) m_t0_endcap[i] = 14.2;
  m_t0_shift=0.;
  for(size_t i=0;i<MaxTimeBin;++i) {
    m_errors[i]=m_uni_error;
    m_radius[i]=2.;
  }
  m_radius[0] = 0.;      m_errors[0] = 0.15;
  m_radius[1] = 0.0;     m_errors[1] = 0.15;
  m_radius[2] = 0.100;   m_errors[2] = 0.20;
  m_radius[3] = 0.262;   m_errors[3] = 0.23;
  m_radius[4] = 0.466;   m_errors[4] = 0.21;
  m_radius[5] = 0.607;   m_errors[5] = 0.18;
  m_radius[6] = 0.796;   m_errors[6] = 0.17;
  m_radius[7] = 0.931;   m_errors[7] = 0.16;
  m_radius[8] = 1.065;   m_errors[8] = 0.15;
  m_radius[9] = 1.212;   m_errors[9] = 0.15;
  m_radius[10] = 1.326;  m_errors[10] = 0.14;
  m_radius[11] = 1.466;  m_errors[11] = 0.13;
  m_radius[12] = 1.585;  m_errors[12] = 0.12;
  m_radius[13] = 1.689;  m_errors[13] = 0.11;
  m_radius[14] = 1.809;  m_errors[14] = 0.11;
  m_radius[15] = 1.880;  m_errors[15] = 0.11;
  m_radius[16] = 1.940;  m_errors[16] = 0.13;
  m_radius[17] = 1.950;  m_errors[17] = 0.20;
  m_radius[18] = 1.955;  m_errors[18] = 0.20;

}

//
// Destructor--------------------------------------------------
TRT_DriftFunctionTool::~TRT_DriftFunctionTool(){}

const InterfaceID& TRT_DriftFunctionTool::interfaceID( )
{ return IID_ITRT_DriftFunctionTool; }

//
// Initialize--------------------------------------------------
StatusCode TRT_DriftFunctionTool::initialize()
{
  ATH_MSG_DEBUG( "initialize()");

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    {
      ATH_MSG_FATAL("Cannot initialize AthAlgTool!");
      return StatusCode::FAILURE;
    } 

  if(m_dummy){
    ATH_MSG_INFO(" Drift time information ignored ");
  }

  // Retrieve TRT_DetectorManager and helper
  sc = AthAlgTool::detStore()->retrieve(m_manager, m_trt_mgr_location);
  if (sc.isFailure() || !m_manager)
  {
    ATH_MSG_FATAL("Could not find the Manager: "
		  << m_trt_mgr_location << " !");
    return sc;
  }

  // Get TRT ID helper
  sc = detStore()->retrieve(m_trtid,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not retrieve TRT ID helper." );
    return sc;
  }

  // Check that ToT corrections have the correct length
  if (m_tot_corrections_barrel_Xe.size() != 20) {
    ATH_MSG_FATAL( "Length of ToTCorrectionsBarrelXe is not 20." );
    return sc;
  }
  if (m_tot_corrections_endcap_Xe.size() != 20) {
    ATH_MSG_FATAL( "Length of ToTCorrectionsEndcapXe is not 20." );
    return sc;
  }
  if (m_tot_corrections_barrel_Ar.size() != 20) {
    ATH_MSG_FATAL( "Length of ToTCorrectionsBarrelAr is not 20." );
    return sc;
  }
  if (m_tot_corrections_endcap_Ar.size() != 20) {
    ATH_MSG_FATAL( "Length of ToTCorrectionsEndcapAr is not 20." );
    return sc;
  }

  if(m_allow_data_mc_override)
    {
      ATH_MSG_DEBUG(" Constants from conddb, code or file allowed "); 
    }

  //
  // Get GeoModel version key
  IGeoModelSvc *geomodel;
  sc=service("GeoModelSvc",geomodel);
  if(sc.isFailure()){
    ATH_MSG_FATAL(" Could not locate GeoModelSvc ");
    return sc;
  }

  DecodeVersionKey versionKey(geomodel,"TRT");
  m_key=versionKey.tag();

  int numB = m_manager->getNumerology()->getNBarrelPhi();
  ATH_MSG_DEBUG(" Number of Barrel elements "<< numB);      
      
  if (numB==2) {
      m_istestbeam = true;
  } else {
      m_istestbeam = false;
  }


  bool choosedata = false;
  bool choosemc = false;

  if(m_ismc) {
    ATH_MSG_INFO(" TRT_DriftFunctionTool initialized for simulation");
    choosemc = true;
    m_isdata=false;
  } else {
    ATH_MSG_INFO("  TRT_DriftFunctionTool initialized for data");
    choosedata = true;
    m_isdata=true;
  }

  if(m_allow_data_mc_override) { 
    choosedata = false;
    choosemc = false;
    if (m_forcedata) {
      ATH_MSG_INFO(" Constants will be read from conddb ");
      choosedata=true;
    } else {
      ATH_MSG_INFO(" Constants will be read from code ");
      choosemc=true;
    }
  }

  if(choosemc&&choosedata) ATH_MSG_FATAL("Trying to init MC and data setup both!");
  if(!choosemc&&!choosedata) ATH_MSG_FATAL("Neither MC nor data setup selected!");
  if(choosemc) {
    m_isdata=false;
    setupRtRelationMC();
  } else {
    m_isdata=true;
    setupRtRelationData();
  }

  return sc;
}

//
// Finalize-----------------------------------------------------------------
StatusCode TRT_DriftFunctionTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

// Drift time in ns for any non negative drift radius; Not calibrated for
// individual straws and run range, but otherwise adapted to any
// setup.
double TRT_DriftFunctionTool::approxDriftTime(double driftradius) const
{
  double t = 0.;
  int i=0;
  if(driftradius<0.100) {
    t = 2.5*m_drifttimeperbin*driftradius/0.1;
  } else if(driftradius<1.99) {
    while(driftradius>=m_radius[i]) ++i;
    if(i>0) i--;
    t=(i+0.5+(driftradius-m_radius[i])/(m_radius[i+1]-m_radius[i]))*m_drifttimeperbin;
  } else {
    t = m_drifttimeperbin*( 19. + (driftradius-1.99)/0.08 );
  }

  return t;
}

// Drift radius in mm for valid drift time in MC; zero otherwise.----------
double TRT_DriftFunctionTool::driftRadius(double drifttime) const
{
  if( !isValidTime(drifttime) ) return 0;
  int drifttimebin = std::max(int(drifttime/m_drifttimeperbin),0); 

  // Interpolate linearly 
  if(drifttime < (drifttimebin+0.5)*m_drifttimeperbin) {
    if (drifttimebin-1 > -1)
        return m_radius[drifttimebin-1]+
         (m_radius[drifttimebin]-m_radius[drifttimebin-1])*
	  (drifttime - (drifttimebin-0.5)*m_drifttimeperbin)/m_drifttimeperbin;
  } else if (drifttimebin+1 < 20) {
        return m_radius[drifttimebin]+
         (m_radius[drifttimebin+1]-m_radius[drifttimebin])*
	  (drifttime - (drifttimebin+0.5)*m_drifttimeperbin)/m_drifttimeperbin;
  }

  return m_radius[drifttimebin];
}

//
// Drift radius in mm for valid drift time (rawtime-t0) in data; --------------
// zero otherwise; truncated to at most 2mm.
double TRT_DriftFunctionTool::driftRadius(double rawtime, Identifier id, double& t0, bool& isOK, unsigned int word) const
{
  isOK = true;
  const double crawtime=rawtime - m_t0_shift; // const cast
  const Identifier cid=id;       // const cast
  t0   = 0.;
  float ft0=t0;                  //float cast

  //case of no drifttime information wanted
  if (m_dummy) return 0.;

  //case of real data or forced database lookup for mc
  if(m_isdata)
    {
      double radius = 0.; 
      if (!m_isoverlay){ //standard case
	radius = m_TRTCalDbTool->driftRadius(crawtime,ft0,cid,isOK);
	t0=ft0 + m_t0_shift;
      }
      else{ //overlay case
	radius = m_TRTCalDbTool->driftRadius(rawtime,ft0,cid,isOK);// no m_t0_shift in rawtime, and use data TRTCalDbSvc
	t0=ft0;
	bool mcdigit = word & (1<<31);
	if (mcdigit ){
	  //check if it's a MC digit, and if so apply other calibration
	  ATH_MSG_DEBUG ("Overlay TRTCalDbTool  gave  radius: "<<radius<<", t0: "<<t0);
	  radius = m_TRTCalDbTool2->driftRadius(crawtime,ft0,cid,isOK);//t0_shift in crawtime, and use MC TRTCalDbSvc(2)
	  t0=ft0 + m_t0_shift;
	  ATH_MSG_DEBUG ("Overlay TRTCalDbTool2 gives radius: "<<radius<<", t0: "<<t0);                                                                
	}   
      }
      double drifttime = rawtime-t0;
      if( !isValidTime(drifttime) ) isOK=false;
      
      /*
	if(isOK) {
	std::cout <<" Found a radius " << radius << " for drifttime " << drifttime <<
                    " t0 " << t0 << " bin " << std::max(int(drifttime/m_drifttimeperbin),0)
                     << " for bec "<< m_trtid->barrel_ec(id) <<
                     " layer_or_wheel " << m_trtid->layer_or_wheel(id) <<
	             " strawlayer " << m_trtid->straw_layer(id)  << std::endl;
	std::cout << " drift time from radius " << approxDriftTime(driftRadius(drifttime)) << std::endl;     
      } else {
	std::cout  <<" No good drift time "  << rawtime-t0 <<  " t0 " << t0 <<
                     " bin " << std::max(int(drifttime/m_drifttimeperbin),0)
                     << " for bec "<< m_trtid->barrel_ec(id) <<
                     " layer_or_wheel " << m_trtid->layer_or_wheel(id) <<
	             " strawlayer " << m_trtid->straw_layer(id)  <<std::endl;
      }
      */

      return radius;
    } 

  //case of MC
  bool barl = m_trtid->is_barrel(cid);
  int lay = m_trtid->layer_or_wheel(cid);
  if(barl) {
     t0=m_t0_barrel[lay];
  } else {
     t0=m_t0_endcap[lay];
  }

  double drifttime = rawtime-t0;

  if(!isValidTime(drifttime)) isOK=false;

  /*
  if(isOK) {
    double radius = driftRadius(drifttime);
    std::cout <<  " good drifttime " << drifttime << " t0 " << t0 <<
                     " bin " << std::max(int(drifttime/m_drifttimeperbin),0)
                     << " for bec "<< m_trtid->barrel_ec(id) <<
                     " layer_or_wheel " << m_trtid->layer_or_wheel(id) <<
                     " strawlayer " << m_trtid->straw_layer(id)  <<
                     " radius " << radius <<  std::endl;
    std::cout << " drift time from radius " << approxDriftTime(radius) << std::endl;     
  } else {
    std::cout << " no good drifttime " << drifttime << " for Identifier "<< id <<std::endl;
  }
  */

  return driftRadius(drifttime);
}
 
// Error of drift radius in mm -----------------------------------------------
double TRT_DriftFunctionTool::errorOfDriftRadius(double drifttime, Identifier id, float mu, unsigned int word) const
{
  if( m_dummy ) return 4./sqrt(12.);
  if(m_force_universal_errors && m_uni_error!=0) return m_uni_error;
  bool founderr=true;
  bool foundslope=true;
  double error = m_TRTCalDbTool->driftError(drifttime,id,founderr);
  double slope = m_TRTCalDbTool->driftSlope(drifttime,id,foundslope);
  bool mcdigit = word & (1<<31);
  if (m_isoverlay && mcdigit ){
    //check if it's a MC digit, and if so apply other calibration
    ATH_MSG_DEBUG ("Overlay TRTCalDbTool gave error: "<<error<<", found="<<founderr);
    error = m_TRTCalDbTool2->driftError(drifttime,id,founderr);
    ATH_MSG_DEBUG ("Overlay TRTCalDbTool2 gives error: "<<error<<", found="<<founderr);
    ATH_MSG_DEBUG ("Overlay TRTCalDbTool gave slope: "<<slope<<", found="<<foundslope);
    slope = m_TRTCalDbTool2->driftSlope(drifttime,id,foundslope);
    ATH_MSG_DEBUG ("Overlay TRTCalDbTool2 gives slope: "<<slope<<", found="<<foundslope);
  }

  if(founderr && foundslope) {
    return error+mu*slope;
//to add condition for old setup
  }
  else if ((founderr && !foundslope)  || (mu<0)) {
		return error; }
  else {  //interpolate
    if(drifttime<=0.) {
      return m_errors[0];
    } else if(drifttime >= 18.*m_drifttimeperbin) {
      return m_errors[18];
    } else {
      float drifttimeinbins = 	drifttime/m_drifttimeperbin;
      int drifttimebin = (int)drifttimeinbins;
      float fracbin = drifttimeinbins-drifttimebin;
      return (1-fracbin)*m_errors[drifttimebin]+fracbin*m_errors[drifttimebin+1];
    }
  }
}

//
// returns the time over threshold correction in ns
double TRT_DriftFunctionTool::driftTimeToTCorrection(double tot, Identifier id, bool isArgonStraw)
{
  int tot_index = tot/m_drifttimeperbin;
  if (tot_index < 0) tot_index = 0;
  if (tot_index > 19) tot_index = 19;

  int bec_index = abs(m_trtid->barrel_ec(id)) - 1;

  if (isArgonStraw) {
    return (bec_index) ? m_tot_corrections_endcap_Ar[tot_index] : m_tot_corrections_barrel_Ar[tot_index];
  }
  return (bec_index) ? m_tot_corrections_endcap_Xe[tot_index] : m_tot_corrections_barrel_Xe[tot_index];
}

// Returns high threshold correction to the drift time (ns)
double TRT_DriftFunctionTool::driftTimeHTCorrection(Identifier id, bool isArgonStraw)
{
  int bec_index = abs(m_trtid->barrel_ec(id)) - 1;

  if (isArgonStraw) {
    return (bec_index) ? m_ht_correction_endcap_Ar : m_ht_correction_barrel_Ar;
  }
  return (bec_index) ? m_ht_correction_endcap_Xe : m_ht_correction_barrel_Xe;
}

//
// Initialise R-t relation for data ------------------------------------------
void TRT_DriftFunctionTool::setupRtRelationData()
{     

  //Setting up for data
  ATH_MSG_DEBUG(" Setting up for data ");

  ATH_MSG_DEBUG(" Using TRTCalDbTool ");
  if ( m_TRTCalDbTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(m_TRTCalDbTool.propertyName() <<
		  ": Failed to retrieve service " << m_TRTCalDbTool.type());
    return;
    
  } else {
    ATH_MSG_DEBUG(m_TRTCalDbTool.propertyName() <<
		  ": Retrieved service " << m_TRTCalDbTool.type());
  }

  if (m_isoverlay){
    ATH_MSG_INFO("Using TRTCalDbTool2 for overlay ! ");
    if ( m_TRTCalDbTool2.retrieve().isFailure() ) {
      ATH_MSG_FATAL(m_TRTCalDbTool2.propertyName() <<": Failed to retrieveservice " << m_TRTCalDbTool2.type());
      return;
    }
  }
  //temporary: we need some way to automatically link digi version with db tag
  //for now we make a hack in order always to get the right t0 after having centered the
  //drifttime spectrum better in the allowed time-window with digi version 12 in release 14.

  int type = m_forced_digiversion;
  if(m_ismc || m_isoverlay){
   
    if(!m_allow_digi_version_override) {
      type = m_manager->digitizationVersion();
      ATH_MSG_DEBUG("TRT detector manager returned digitization version "<< type <<
		    " corresponding to "<< m_manager->digitizationVersionName());
    } else {
      ATH_MSG_WARNING("Digitization version chosen by user for global t0 correction: "<<type);
    }


    if(type>10) {
      m_t0_shift=-8.;
      ATH_MSG_INFO(" Digitization version " << type << " - T0 for barrel is shifted by "
                   << m_t0_shift);
    }

  }   
  //temporary: we need to think about how to store the uncertainties in the db!!!

  if(m_key.compare(6,4,"Comm")==0) {

    m_radius[0] = 0.;
    m_radius[1] = 0.;
    m_radius[2] = 0.;
    m_radius[3] = 0.252054;
    m_radius[4] = 0.488319;
    m_radius[5] = 0.751514;
    m_radius[6] = 1.00173;
    m_radius[7] = 1.21851;
    m_radius[8] = 1.40886;
    m_radius[9] = 1.68368;
    m_radius[10] = 1.85363;
    m_radius[11] = 1.91764;
    m_radius[12] = 1.94114;

    m_errors[0] = 0.10440061;
    m_errors[1] = 0.1510298;
    m_errors[2] = 0.26130742;
    m_errors[3] = 0.260436;
    m_errors[4] = 0.246961;
    m_errors[5] = 0.226037;
    m_errors[6] = 0.18272;
    m_errors[7] = 0.195482;
    m_errors[8] = 0.213817;
    m_errors[9] = 0.157627;
    m_errors[10] = 0.0922559;
    m_errors[11] = 0.0463124;
    m_errors[12] = 0.0480864;

  } else {

    m_errors[0] = 0.15*m_err_fudge;
    m_errors[1] = 0.15*m_err_fudge;
    m_errors[2] = 0.20*m_err_fudge;
    m_errors[3] = 0.23*m_err_fudge;
    m_errors[4] = 0.21*m_err_fudge;
    m_errors[5] = 0.18*m_err_fudge;
    m_errors[6] = 0.17*m_err_fudge;
    m_errors[7] = 0.16*m_err_fudge;
    m_errors[8] = 0.15*m_err_fudge;
    m_errors[9] = 0.15*m_err_fudge;
    m_errors[10] = 0.14*m_err_fudge;
    m_errors[11] = 0.13*m_err_fudge;
    m_errors[12] = 0.12*m_err_fudge;
    m_errors[13] = 0.11*m_err_fudge;
    m_errors[14] = 0.11*m_err_fudge;
    m_errors[15] = 0.11*m_err_fudge;
    m_errors[16] = 0.13*m_err_fudge;
    m_errors[17] = 0.20*m_err_fudge;
    m_errors[18] = 0.20*m_err_fudge;
  }
    
  m_error = 0.136;
  return;
}
//
// Initialise R-t relation for MC --------------------------------------------
void TRT_DriftFunctionTool::setupRtRelationMC()
{     

  ATH_MSG_INFO(" Setting up for MC ");
  int nBarrelRings = m_manager->getNumerology()->getNBarrelRings();
  int nEndcapWheels = m_manager->getNumerology()->getNEndcapWheels();

  if(!m_override_simcal) // take constants from this code
	{
	  int type;
	  if(!m_allow_digi_version_override){      //use the digitization version on tape
	    type = m_manager->digitizationVersion();
	    ATH_MSG_DEBUG("TRT detector manager returned digitization version "<< type <<
			  " corresponding to "<< m_manager->digitizationVersionName());

	  } else {                                 // use another digitization version than the one on tape
	    type = m_forced_digiversion;
	    ATH_MSG_WARNING("Digitization version chosen by user: " << type);
	  }


	  if(type==0||type==1) //Old gas (DC1 + DC2)
	  {
	    ATH_MSG_DEBUG("Old gas selected (DC1 + DC2)");

	    m_radius[0] = 0.118643;
	    m_radius[1] = 0.155114;
	    m_radius[2] = 0.254119;
	    m_radius[3] = 0.387365;
	    m_radius[4] = 0.532817;
	    m_radius[5] = 0.709223;
	    m_radius[6] = 0.912180;
	    m_radius[7] = 1.13879;
	    m_radius[8] = 1.36224;
	    m_radius[9] = 1.58121;
	    m_radius[10] = 1.76968;
	    m_radius[11] = 1.87641;
	    m_radius[12] = 1.92448;
	    m_radius[13] = 1.99633;
	    m_radius[14] = 1.99990;
		  
	    for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 0.;
	    for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 0.;

	    m_error = 0.17;
	  }
	  else if (type==2||type==3) //New gas without time corrections (Rome)
	  {
	    ATH_MSG_DEBUG("New gas without time corrections selected (Rome)");

	    m_radius[0] = 0.125866;
	    m_radius[1] = 0.174001;
	    m_radius[2] = 0.273058;
	    m_radius[3] = 0.418042;
	    m_radius[4] = 0.573647;
	    m_radius[5] = 0.72763;
	    m_radius[6] = 0.885585;
	    m_radius[7] = 1.0433;	    m_radius[8] = 1.20297;
	    m_radius[9] = 1.36244;
	    m_radius[10] = 1.52331;
	    m_radius[11] = 1.67892;
	    m_radius[12] = 1.81663;
	    m_radius[13] = 1.89067;
	    m_radius[14] = 1.93317;
	    m_radius[15] = 1.96245;
	      
	    for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 0.;
	    for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 0.;

	    m_error = 0.17;
	  }
	  else if (type==4||type==5) //New gas with time corrections (DC3)
	  {
	    ATH_MSG_DEBUG("New gas with time corrections selected (DC3)");

	    m_radius[0] = 0.133702;
	    m_radius[1] = 0.133909;
	    m_radius[2] = 0.194929;
	    m_radius[3] = 0.293642;
	    m_radius[4] = 0.422019;
	    m_radius[5] = 0.560768;
	    m_radius[6] = 0.732412;
	    m_radius[7] = 0.902288;
	    m_radius[8] = 1.06672;
	    m_radius[9] = 1.22995;
	    m_radius[10] = 1.38251;
	    m_radius[11] = 1.5318;
	    m_radius[12] = 1.66725;
	    m_radius[13] = 1.78666;
	    m_radius[14] = 1.86107;
	    m_radius[15] = 1.9047;

	    for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 0.;
	    for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 0.;

	    m_error = 0.136;
	  }
	  else if (type==6 || type==11) //(DC3version2)
	  {
	    ATH_MSG_INFO("New gas with time corrections + t_0 selected (DC3version2)");
            m_radius[0] = 0.;
            m_radius[1] = 0.0;
            m_radius[2] = 0.100;
            m_radius[3] = 0.262;
            m_radius[4] = 0.466;
            m_radius[5] = 0.607;
            m_radius[6] = 0.796;
            m_radius[7] = 0.931;
            m_radius[8] = 1.065;
            m_radius[9] = 1.212;
            m_radius[10] = 1.326;
            m_radius[11] = 1.466;
            m_radius[12] = 1.585;
            m_radius[13] = 1.689;
            m_radius[14] = 1.809;
            m_radius[15] = 1.880;
            m_radius[16] = 1.940;
            m_radius[17] = 1.950;
            m_radius[18] = 1.955;

            m_errors[0] = 0.25;
            m_errors[1] = 0.25;
            m_errors[2] = 0.14;
            m_errors[3] = 0.15;
            m_errors[4] = 0.16;
            m_errors[5] = 0.14;
            m_errors[6] = 0.14;
            m_errors[7] = 0.12;
            m_errors[8] = 0.11;
            m_errors[9] = 0.11;
            m_errors[10] = 0.11;
            m_errors[11] = 0.10;
            m_errors[12] = 0.10;
            m_errors[13] = 0.10;
            m_errors[14] = 0.08;
            m_errors[15] = 0.08;
            m_errors[16] = 0.08;
            m_errors[17] = 0.08;
            m_errors[18] = 0.08;

            if(type==6) {
 	      for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 15.625;
	      for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 14.2;
            } else {
 	      for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 7.625;
	      for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 6.2;
            }

	    m_error = 0.136;


	  } 
	  else if (type==7 || type==9 || type==10) //(DC3version2-Cosmic)
	  {
	    ATH_MSG_INFO("New gas with time corrections + t_0 selected (DC3version2-Cosmic)");

            if(m_manager->getLayout()!="SR1-EndcapC" && type!=10) {

              ATH_MSG_INFO(" The specific layout is " << m_manager->getLayout());

 	      m_radius[0] = 0.;
	      m_radius[1] = 0.;
	      m_radius[2] = 0.;
	      m_radius[3] = 0.252054;
	      m_radius[4] = 0.488319;
	      m_radius[5] = 0.751514;
	      m_radius[6] = 1.00173;
	      m_radius[7] = 1.21851;
	      m_radius[8] = 1.40886;
	      m_radius[9] = 1.68368;
	      m_radius[10] = 1.85363;
	      m_radius[11] = 1.91764;
	      m_radius[12] = 1.94114;
                   
              m_errors[0] = 0.10440061;
              m_errors[1] = 0.1510298;
              m_errors[2] = 0.26130742;
              m_errors[3] = 0.260436;
              m_errors[4] = 0.246961;
              m_errors[5] = 0.226037;
              m_errors[6] = 0.18272;
              m_errors[7] = 0.195482;
              m_errors[8] = 0.213817;
              m_errors[9] = 0.157627;
              m_errors[10] = 0.0922559;
              m_errors[11] = 0.0463124;
              m_errors[12] = 0.0480864;

 	      for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 6.;
	      for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 6.;

	      m_error = 0.18;

		    
            } else {
              ATH_MSG_INFO(" The specific layout is " << m_manager->getLayout());

 	      m_radius[0] = 0.;
	      m_radius[1] = 0.071;
	      m_radius[2] = 0.270;
	      m_radius[3] = 0.554;
	      m_radius[4] = 0.821;
	      m_radius[5] = 1.079;
	      m_radius[6] = 1.321;
	      m_radius[7] = 1.567;
	      m_radius[8] = 1.781;
	      m_radius[9] = 1.901;
	      m_radius[10] = 1.93;
	      m_radius[11] = 1.96;
	      m_radius[12] = 1.97;
	      m_radius[13] = 1.98;
	      m_radius[14] = 1.99;
	      m_radius[15] = 2.00;
	      m_radius[16] = 2.00;


 	      for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 0.;
	      for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 0.;

  	      for (int i = 0; i < MaxTimeBin; ++i) m_errors[i]=0.2;
	      m_error = 0.2;
            }


	 } else if (type==8) { //(DC3version2-CTB)
		
	    ATH_MSG_INFO("CTB: New gas with time corrections + t_0 selected (DC3version2-CTB)");
		  
	    m_radius[0] = 0.;
	    m_radius[1] = 0.;
	    m_radius[2] = 0.121003;
	    m_radius[3] = 0.319269;
	    m_radius[4] = 0.489897;
	    m_radius[5] = 0.652101;
	    m_radius[6] = 0.824697;
	    m_radius[7] = 0.981399;
	    m_radius[8] = 1.14677;
	    m_radius[9] = 1.3075;
	    m_radius[10] = 1.46142;
	    m_radius[11] = 1.61662;
	    m_radius[12] = 1.75832;
	    m_radius[13] = 1.84887;
	    m_radius[14] = 1.89023;
	    m_radius[15] = 1.92232;
	    m_radius[16] = 1.93842;


	    m_errors[0] = 0.153*m_err_fudge;
	    m_errors[1] = 0.183*m_err_fudge;
	    m_errors[2] = 0.174*m_err_fudge;
	    m_errors[3] = 0.159*m_err_fudge;
	    m_errors[4] = 0.155*m_err_fudge;
	    m_errors[5] = 0.162*m_err_fudge;
	    m_errors[6] = 0.172*m_err_fudge;
	    m_errors[7] = 0.171*m_err_fudge;
	    m_errors[8] = 0.176*m_err_fudge;
	    m_errors[9] = 0.169*m_err_fudge;
	    m_errors[10] = 0.160*m_err_fudge;
	    m_errors[11] = 0.142*m_err_fudge;
	    m_errors[12] = 0.135*m_err_fudge;
	    m_errors[13] = 0.139*m_err_fudge;
	    m_errors[14] = 0.139*m_err_fudge;
	    m_errors[15] = 0.139*m_err_fudge;
	    m_errors[16] = 0.139*m_err_fudge;

 	    for (int i=0; i<nBarrelRings; i++) m_t0_barrel[i] = 15.625;
	    for (int i=0; i<nEndcapWheels; i++) m_t0_endcap[i] = 14.2;
	     
	    m_error = 0.136;

	  } else { //Non-supported digiversion

            ATH_MSG_FATAL(" Non-supported digitization version - revert to default");
          }
	
       } else if( m_inputfile!="") { // Overriding sim settings from text file or defaults
          ATH_MSG_WARNING(" Simulation constants from file " <<  m_inputfile);

          std::ifstream infile;
          infile.open(m_inputfile.c_str());
          if(infile) {
            int index=0;
            while ((infile >> m_radius[index] >> m_errors[index]) && index<20)
              index++;
         
	  } else {
	    ATH_MSG_WARNING(" File does not exist - revert to default");
          }

        }
}
