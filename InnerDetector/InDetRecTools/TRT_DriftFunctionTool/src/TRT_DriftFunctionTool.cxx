/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolFactory.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetReadoutGeometry/Version.h"


#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"

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
  :  AthAlgTool(type, name, parent),
    m_TRTCalDbSvc("TRT_CalDbSvc",name),
    m_TRTCalDbSvc2("",name),
    m_IncidentSvc("IncidentSvc",name),
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
    m_trt_mgr_location("TRT")

{
  declareInterface<ITRT_DriftFunctionTool>(this);
  declareProperty("IncidentService",m_IncidentSvc);
  m_drifttimeperhalfbin = m_drifttimeperbin/2.;
  declareProperty("TRT_Calibration",m_isdata);
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
  declareProperty("TRTCalDbTool", m_TRTCalDbSvc);
  declareProperty("TRTCalDbTool2", m_TRTCalDbSvc2);
  declareProperty("DriftFunctionFile", m_inputfile);
  declareProperty("TrtDescrManageLocation",m_trt_mgr_location);

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

  //ToT drift time corrections  --  Barrel (0) --  Endcap(1)
  m_tot_corrections[0][0] = 0.;           m_tot_corrections[1][0] = 0.;
  m_tot_corrections[0][1] = 4.358121;     m_tot_corrections[1][1] = 5.514777;
  m_tot_corrections[0][2] = 3.032195;     m_tot_corrections[1][2] = 3.342712;
  m_tot_corrections[0][3] = 1.631892;     m_tot_corrections[1][3] = 2.056626;
  m_tot_corrections[0][4] = 0.7408397;    m_tot_corrections[1][4] = 1.08293693;
  m_tot_corrections[0][5] = -0.004113;    m_tot_corrections[1][5] = 0.3907979;
  m_tot_corrections[0][6] = -0.613288;    m_tot_corrections[1][6] = -0.082819;
  m_tot_corrections[0][7] = -0.73758;     m_tot_corrections[1][7] = -0.457485;
  m_tot_corrections[0][8] = -0.623346;    m_tot_corrections[1][8] = -0.599706; 
  m_tot_corrections[0][9] = -0.561229;    m_tot_corrections[1][9] = -0.427493;
  m_tot_corrections[0][10] = -0.29828;    m_tot_corrections[1][10] = -0.328962;
  m_tot_corrections[0][11] = -0.21344;    m_tot_corrections[1][11] = -0.403399;
  m_tot_corrections[0][12] = -0.322892;   m_tot_corrections[1][12] = -0.663656;
  m_tot_corrections[0][13] = -0.386718;   m_tot_corrections[1][13] = -1.029428;
  m_tot_corrections[0][14] = -0.534751;   m_tot_corrections[1][14] = -1.46008;
  m_tot_corrections[0][15] = -0.874178;   m_tot_corrections[1][15] = -1.919092;
  m_tot_corrections[0][16] = -1.231799;   m_tot_corrections[1][16] = -2.151582;
  m_tot_corrections[0][17] = -1.503689;   m_tot_corrections[1][17] = -2.285481;
  m_tot_corrections[0][18] = -1.896464;   m_tot_corrections[1][18] = -2.036822;
  m_tot_corrections[0][19] = -2.385958;   m_tot_corrections[1][19] = -2.15805;
  
  //HT drift time correction
  m_ht_corrections[0] = 1.5205;  //barrel
  m_ht_corrections[1] = 1.2712;  //endcap
  
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
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "initialize()" << endreq;

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    {
      msg(MSG::FATAL) << "Cannot initialize AthAlgTool!" << endreq;
      return StatusCode::FAILURE;
    } 

  if(m_dummy){
    msg(MSG::INFO) << " Drift time information ignored "<< endreq;
  }

  //Incident service (to check for MC/data and setup accordingly)
  if ( m_IncidentSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_IncidentSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Retrieved service " << m_IncidentSvc << endreq;

  // call handle in case of BeginRun
  m_IncidentSvc->addListener( this, std::string("BeginRun"));

  // Retrieve TRT_DetectorManager and helper
  sc = AthAlgTool::detStore()->retrieve(m_manager, m_trt_mgr_location);
  if (sc.isFailure() || !m_manager)
  {
    msg(MSG::FATAL) << "Could not find the Manager: "
       	  << m_trt_mgr_location << " !" << endreq;
    return sc;
  }

  // Get TRT ID helper
  sc = detStore()->retrieve(m_trtid,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not retrieve TRT ID helper." );
    return sc;
  }

  if(m_allow_data_mc_override)
    {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Constants from conddb, code or file allowed " << endreq; 
    }

  //
  // Get GeoModel version key
  IGeoModelSvc *geomodel;
  sc=service("GeoModelSvc",geomodel);
  if(sc.isFailure()){
    msg(MSG::FATAL) << " Could not locate GeoModelSvc " << endreq;
    return sc;
  }

  DecodeVersionKey versionKey(geomodel,"TRT");
  m_key=versionKey.tag();


  // Register callback function for cache updates - ToT:
  const DataHandle<CondAttrListCollection> aptr;
  if (StatusCode::SUCCESS == detStore()->regFcn(&TRT_DriftFunctionTool::update,this, aptr, "/TRT/Calib/ToTCalib" )) {
    ATH_MSG_INFO ("Registered callback for TRT_DriftFunctionTool - ToT Correction.");
  } else {
    ATH_MSG_ERROR ("Callback registration failed for TRT_DriftFunctionTool - ToT!  Using default correction values.");
  }

  // Register callback function for cache updates - HT:
  const DataHandle<CondAttrListCollection> aptrHT;
  if (StatusCode::SUCCESS == detStore()->regFcn(&TRT_DriftFunctionTool::update,this, aptrHT, "/TRT/Calib/HTCalib" )) {
    ATH_MSG_INFO ("Registered callback for TRT_DriftFunctionTool - HT Correction.");
  } else {
    ATH_MSG_ERROR ("Callback registration failed for TRT_DriftFunctionTool - HT!  Using default correction values.");
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

//
// handle BeginRun incidents------------------------------------------------
void TRT_DriftFunctionTool::handle(const Incident& inc)
{

  //Find out what type of run
  if (inc.type() == "BeginRun") 
    {
      const EventInfo* pevt = 0; // pointer for the event
      StatusCode status = evtStore()->retrieve(pevt); // retrieve the pointer to the event
      if(!status.isSuccess() || pevt==0) {
         msg(MSG::FATAL) <<"Couldn't get EventInfo object from StoreGate"<<endreq;
         return;
      }

      int numB = m_manager->getNumerology()->getNBarrelPhi();
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Number of Barrel elements "<< numB << endreq;      
      
      if (numB==2) {
          m_istestbeam = true;
        } else {
          m_istestbeam = false;
        }

      if(pevt->event_type()->test(EventType::IS_CALIBRATION))
	{
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Run reports itself as calibration"<<endreq;
	} else {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Run reports itself as physics"<<endreq;
	}

      bool choosedata = false;
      bool choosemc = false;

      if(pevt->event_type()->test(EventType::IS_SIMULATION))
	{
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Run reports itself as simulation"<<endreq;
	  choosemc = true;
          m_ismc=true;
	} else {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Run reports itself as data"<<endreq;
	  choosedata = true;
          m_ismc=false;
	}

      if(m_allow_data_mc_override)
	{ 
	  choosedata = false;
	  choosemc = false;
	  if (m_forcedata)
	    {
	      msg(MSG::INFO) << " Constants will be read from conddb " << endreq;
	      choosedata=true;
	    } else {
	      msg(MSG::INFO) << " Constants will be read from code " << endreq;
	      choosemc=true;
	    }
	}

      if(choosemc&&choosedata) msg(MSG::FATAL) << "Trying to init MC and data setup both!" << endreq;
      if(!choosemc&&!choosedata) msg(MSG::FATAL) << "Neither MC nor data setup selected!" << endreq;
      if(choosemc)
	{
	  m_isdata=false;
          setupRtRelationMC();
	} else {
	  m_isdata=true;
          setupRtRelationData();
	}
    }
  return;
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
	radius = m_TRTCalDbSvc->driftRadius(crawtime,ft0,cid,isOK);
	t0=ft0 + m_t0_shift;
      }
      else{ //overlay case
	radius = m_TRTCalDbSvc->driftRadius(rawtime,ft0,cid,isOK);// no m_t0_shift in rawtime, and use data TRTCalDbSvc
	t0=ft0;
	bool mcdigit = word & (1<<31);
	if (mcdigit ){
	  //check if it's a MC digit, and if so apply other calibration
	  ATH_MSG_DEBUG ("Overlay TRTCalDbSvc  gave  radius: "<<radius<<", t0: "<<t0);
	  radius = m_TRTCalDbSvc2->driftRadius(crawtime,ft0,cid,isOK);//t0_shift in crawtime, and use MC TRTCalDbSvc(2)
	  t0=ft0 + m_t0_shift;
	  ATH_MSG_DEBUG ("Overlay TRTCalDbSvc2 gives radius: "<<radius<<", t0: "<<t0);                                                                
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
  double error = m_TRTCalDbSvc->driftError(drifttime,id,founderr);
  double slope = m_TRTCalDbSvc->driftSlope(drifttime,id,foundslope);
  bool mcdigit = word & (1<<31);
  if (m_isoverlay && mcdigit ){
    //check if it's a MC digit, and if so apply other calibration
    ATH_MSG_DEBUG ("Overlay TRTCalDbSvc gave error: "<<error<<", found="<<founderr);
    error = m_TRTCalDbSvc2->driftError(drifttime,id,founderr);
    ATH_MSG_DEBUG ("Overlay TRTCalDbSvc2 gives error: "<<error<<", found="<<founderr);
    ATH_MSG_DEBUG ("Overlay TRTCalDbSvc gave slope: "<<slope<<", found="<<foundslope);
    slope = m_TRTCalDbSvc2->driftSlope(drifttime,id,foundslope);
    ATH_MSG_DEBUG ("Overlay TRTCalDbSvc2 gives slope: "<<slope<<", found="<<foundslope);
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
double TRT_DriftFunctionTool::driftTimeToTCorrection(double tot, Identifier id) const
{

  int tot_index = tot/3.125;
  if (tot_index < 0) tot_index = 0;
  if (tot_index > 19) tot_index = 19;
  
  int bec_index = abs(m_trtid->barrel_ec(id)) - 1;
  
  return m_tot_corrections[bec_index][tot_index];
}

// Returns high threshold correction to the drift time (ns) 
double TRT_DriftFunctionTool::driftTimeHTCorrection(Identifier id) const 
{

  int bec_index = abs(m_trtid->barrel_ec(id)) - 1;
  
  return m_ht_corrections[bec_index];
}

//
// Initialise R-t relation for data ------------------------------------------
void TRT_DriftFunctionTool::setupRtRelationData()
{     

  //Setting up for data
  ATH_MSG_DEBUG(" Setting up for data ");

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Using TRTCalDbSvc "<<endreq;
  if ( m_TRTCalDbSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << m_TRTCalDbSvc.propertyName() <<
      ": Failed to retrieve service " << m_TRTCalDbSvc.type() << endreq;
    return;
    
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << m_TRTCalDbSvc.propertyName() <<
      ": Retrieved service " << m_TRTCalDbSvc.type() << endreq;
  }

  if (m_isoverlay){
    msg(MSG::INFO) <<" Using TRTCalDbSvc2 for overlay ! "<<endreq;
    if ( m_TRTCalDbSvc2.retrieve().isFailure() ) {
      msg(MSG::FATAL) << m_TRTCalDbSvc2.propertyName() <<": Failed to retrieveservice " << m_TRTCalDbSvc2.type() << endreq;
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
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT detector manager returned digitization version "<< type <<
                            " corresponding to "<< m_manager->digitizationVersionName() << endreq;
    } else {
      msg(MSG::WARNING) << "Digitization version chosen by user for global t0 correction: "<<type<<endreq;
    }


    if(type>10) {
      m_t0_shift=-8.;
      msg(MSG::INFO) << " Digitization version " << type << " - T0 for barrel is shifted by "
                     << m_t0_shift << endreq;
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

  msg(MSG::INFO) <<" Setting up for MC "<<endreq;
  int nBarrelRings = m_manager->getNumerology()->getNBarrelRings();
  int nEndcapWheels = m_manager->getNumerology()->getNEndcapWheels();

  if(!m_override_simcal) // take constants from this code
	{
	  int type;
	  if(!m_allow_digi_version_override){      //use the digitization version on tape
	    type = m_manager->digitizationVersion();
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT detector manager returned digitization version "<< type <<
                         " corresponding to "<< m_manager->digitizationVersionName() << endreq;

	  } else {                                 // use another digitization version than the one on tape
	    type = m_forced_digiversion;
	    msg(MSG::WARNING) << "Digitization version chosen by user: "<<type<<endreq;
	  }


	  if(type==0||type==1) //Old gas (DC1 + DC2)
	  {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Old gas selected (DC1 + DC2)"<<endreq;

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
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"New gas without time corrections selected (Rome)"<<endreq;

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
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"New gas with time corrections selected (DC3)"<<std::endl;

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
	    msg(MSG::INFO) <<"New gas with time corrections + t_0 selected (DC3version2)"<< endreq;
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
	    msg(MSG::INFO) <<"New gas with time corrections + t_0 selected (DC3version2-Cosmic)"<<endreq;

            if(m_manager->getLayout()!="SR1-EndcapC" && type!=10) {

              msg(MSG::INFO) <<" The specific layout is "
                                 << m_manager->getLayout() << endreq;

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
              msg(MSG::INFO) <<" The specific layout is "
                                  << m_manager->getLayout() << endreq;

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
		
	    msg(MSG::INFO) <<"CTB: New gas with time corrections + t_0 selected (DC3version2-CTB)"<<endreq;
		  
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

	     msg(MSG::FATAL) <<
             " Non-supported digitization version - revert to default" << endreq;
          }
	
       } else if( m_inputfile!="") { // Overriding sim settings from text file or defaults
	  msg(MSG::WARNING)<<" Simulation constants from file "
                <<  m_inputfile  << endreq;

          std::ifstream infile;
          infile.open(m_inputfile.c_str());
          if(infile) {
            int index=0;
            while ((infile >> m_radius[index] >> m_errors[index]) && index<20)
              index++;
         
	  } else {
	     msg(MSG::WARNING) <<
             " File does not exist - revert to default" << endreq;
          }

	  if(msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG)<<"  Radii are: \n";
	    for (unsigned int i = 0; i!=20 ; i++)
               msg(MSG::DEBUG)<<"["<<i<<"]: "<<m_radius[i]<<"\n";
	    msg(MSG::DEBUG)<<endreq;

	    msg(MSG::DEBUG)<<"  Errors are: \n";
	    for (unsigned int i = 0; i!=20 ; i++)
	      msg(MSG::DEBUG)<<"["<<i<<"]: "<<m_errors[i]<<"\n";
	    msg(MSG::DEBUG)<<endreq;

	    msg(MSG::DEBUG)<<"  Universal error is : " << m_error << endreq;

	    msg(MSG::DEBUG)<<"  Barrel T0s are: \n";
 	    for (int i=0; i<nBarrelRings; i++) msg(MSG::DEBUG) << m_t0_barrel[i] << endreq;

	    msg(MSG::DEBUG)<<"  Endcap T0s are: \n";
 	    for (int i=0; i<nEndcapWheels; i++)
            msg(MSG::DEBUG)<< m_t0_endcap[i] << endreq;
          }

       }
}

/* ----------------------------------------------------------------------------------- */
// Callback function to update constants from database: 
/* ----------------------------------------------------------------------------------- */
StatusCode TRT_DriftFunctionTool::update( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {

  ATH_MSG_INFO ("Updating constants for the TRT_DriftFunctionTool! ");

 // Callback function to update Drift Time Correction parameters when condDB data changes:
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
   ATH_MSG_DEBUG("IOVCALLBACK for key " << *key << " number " << I);

  StatusCode status = StatusCode::SUCCESS;
  
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key) {

    const CondAttrListCollection* atrlistcol;
    
    if (*key == "/TRT/Calib/ToTCalib") {
      ATH_MSG_INFO ("Updating ToT constants for the TRT_DriftFunctionTool! ");

      // Read the ToT Correction parameters:
      // ----------------------------------------------------------------------------------- //
      if (StatusCode::SUCCESS == detStore()->retrieve(atrlistcol, "/TRT/Calib/ToTCalib" ) && atrlistcol != 0) {

	 int channel;
	 std::ostringstream var_name;
	 for (CondAttrListCollection::const_iterator citr=atrlistcol->begin();
              citr!=atrlistcol->end();++citr) {

	    //get Barrel (1) or Endcap (2)
	   channel = citr->first;

	   if ((channel == 1) || (channel == 2)) {
             const coral::AttributeList& atrlist = citr->second;

	     for (int i = 0; i < 20; ++i ) {
       	       var_name << "TRT_ToT_" << std::dec << i;
	       m_tot_corrections[channel-1][i] = atrlist[var_name.str()].data<float>();
	       var_name.str("");
	     }	 
	   } 
	 }
	
      } else {
	ATH_MSG_ERROR ("Problem reading condDB object for ToT correction constants.");
	status = StatusCode::FAILURE;
      }
    }
    
    
    if (*key == "/TRT/Calib/HTCalib") {
      ATH_MSG_INFO ("Updating HT constants for the TRT_DriftFunctionTool! ");

      // Read the HT Correction parameters:
      // ----------------------------------------------------------------------------------- //
      if (StatusCode::SUCCESS == detStore()->retrieve(atrlistcol, "/TRT/Calib/HTCalib" ) && atrlistcol != 0) {

	 int channel;
	 std::ostringstream var_name;
	 for (CondAttrListCollection::const_iterator citr=atrlistcol->begin();
              citr!=atrlistcol->end();++citr) {
	      
	   channel = citr->first;

	   if (channel == 1) {
             const coral::AttributeList& atrlist = citr->second;

	     for (int i = 0; i < 2; ++i ) {
	       var_name << "TRT_HT_" << std::dec << i;
       	       m_ht_corrections[i] = atrlist[var_name.str()].data<float>();
	       var_name.str("");
	     }	 
	   } 
	 }

       } else {
	 ATH_MSG_ERROR ("Problem reading condDB object for HT correction constants.");
	 status = StatusCode::FAILURE;
       }
    }
   
  }
  
  return status;
}
