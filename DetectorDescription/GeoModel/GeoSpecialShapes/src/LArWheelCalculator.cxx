/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArWheelCalculator 19-Apr-2001 Bill Seligman
// 26-May-2009 AMS: remove all previous comments from here as obsoleted

#include <cmath>
#include <climits>
#include <cassert>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "./LArWheelCalculator_Impl/RDBParamReader.h"
#include "./LArWheelCalculator_Impl/RDBParamRecords.h"

#include "./LArWheelCalculator_Impl/DistanceCalculatorFactory.h"
#include "./LArWheelCalculator_Impl/FanCalculatorFactory.h"

#include "AthenaKernel/Units.h"

using namespace Athena::Units;
using Gaudi::Units::twopi;


// The radial boundaries of the inner and outer wheels are defined
// by values of eta, the distance from z=0 to the front face of the
// wheel, and the thickness of the wheel.

// these numbers are taken from DB in constructor,
// hardcoded values here are just for reference
/*
  double LArWheelCalculator::zWheelRefPoint       = 3689.5*mm;  //=endg_z0
  double LArWheelCalculator::dMechFocaltoWRP      = 3691. *mm;  //=endg_z1
  double LArWheelCalculator::dElecFocaltoWRP      = 3689. *mm;  //=endg_dcf
  double LArWheelCalculator::rOuterCutoff                 = 2034. *mm;  //=endg_rlimit
  double LArWheelCalculator::HalfGapBetweenWheels = 0.15*cm;  // In DB EMECGEOMETRY.DCRACK
  float LArWheelCalculator::m_zShift = 4. *cm; // endg_zshift
  double LArWheelCalculator::eta_hi  = 3.2; // from EmecWheelParameters
  double LArWheelCalculator::eta_mid = 2.5;
  double LArWheelCalculator::eta_low = 1.375;
*/

// these values are taken from "EMECParams" DB
/*
  bool LArWheelCalculator::SaggingOn = false;
  bool LArWheelCalculator::phiRotation = false;
*/

// these were internal constants, now everything in DB
//const double LArWheelCalculator::s_dWRPtoFrontFace = 11.*mm;
//const double LArWheelCalculator::s_WheelThickness = 514.*mm;
// 2x2mm are to be substracted from value of wheel thickness
// - for straight 2-mm domains at start and finish of absorber
//const double LArWheelCalculator::s_StraightStartSection = 2.*mm;
//const double LArWheelCalculator::s_HalfWheelThickness = s_WheelThickness * 0.5;
//const double LArWheelCalculator::s_WheelThickness_wo_2sss = s_WheelThickness - 2.*s_StraightStartSection;

static const double default_slant_parametrization[2][5] = {
  { -50.069, 0.50073, -0.10127E-02, 0.10390E-05, -0.42176E-09 }, // inner
  { -34.254, 0.15528, -0.11670E-03, 0.45018E-07, -0.68473E-11 }  //outer
};

const char *LArWheelCalculator::LArWheelCalculatorTypeString(LArG4::LArWheelCalculator_t type)
{
  switch(type){
     case LArG4::InnerAbsorberWheel:           return("InnerAbsorberWheel");
     case LArG4::OuterAbsorberWheel:           return("OuterAbsorberWheel");
     case LArG4::InnerElectrodWheel:           return("InnerElectrodWheel");
     case LArG4::OuterElectrodWheel:           return("OuterElectrodWheel");
     case LArG4::InnerAbsorberModule:          return("InnerAbsorberModule");
     case LArG4::OuterAbsorberModule:          return("OuterAbsorberModule");
     case LArG4::InnerElectrodModule:          return("InnerElectrodModule");
     case LArG4::OuterElectrodModule:          return("OuterElectrodModule");
     case LArG4::BackOuterBarretteWheel:       return("BackOuterBarretteWheel");
     case LArG4::BackInnerBarretteWheel:       return("BackInnerBarretteWheel");
     case LArG4::BackOuterBarretteModule:      return("BackOuterBarretteModule");
     case LArG4::BackInnerBarretteModule:      return("BackInnerBarretteModule");
     case LArG4::BackOuterBarretteWheelCalib:  return("BackOuterBarretteWheelCalib");
     case LArG4::BackInnerBarretteWheelCalib:  return("BackInnerBarretteWheelCalib");
     case LArG4::BackOuterBarretteModuleCalib: return("BackOuterBarretteModuleCalib");
     case LArG4::BackInnerBarretteModuleCalib: return("BackInnerBarretteModuleCalib");
     case LArG4::InnerGlueWheel:               return("InnerGlueWheel");
     case LArG4::InnerLeadWheel:               return("InnerLeadWheel");
     case LArG4::OuterGlueWheel:               return("OuterGlueWheel");
     case LArG4::OuterLeadWheel:               return("OuterLeadWheel");
  }
  return("unknown");
}

LArWheelCalculator::~LArWheelCalculator() {
  delete m_distanceCalcImpl;
  m_distanceCalcImpl = 0;
  delete m_fanCalcImpl;
  m_fanCalcImpl = 0;
}

LArWheelCalculator::LArWheelCalculator(LArG4::LArWheelCalculator_t a_wheelType, int zside) :
  m_type(a_wheelType),
  m_AtlasZside(zside),
  m_distanceCalcImpl(0),
  m_fanCalcImpl(0)
{
  // Get pointer to the message service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* msgSvc;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if(status.isFailure()){
    throw std::runtime_error("LArWheelCalculator constructor: cannot initialze message service");
  }
  MsgStream msg(msgSvc, "LArWheelCalculator");
  msg << MSG::VERBOSE << "LArWheelCalculator constructor at " << this
      << " (type " << LArWheelCalculatorTypeString(m_type)
      << "):" << endmsg;

#ifdef LARWC_DTNF_NEW
  msg << MSG::VERBOSE << "compiled with new DTNF" << endmsg;
#endif

  // Access source of detector parameters.
  msg << MSG::VERBOSE
      << "initializing data members from DB..." << endmsg;

  IGeoModelSvc *geoModel;
  IRDBAccessSvc* rdbAccess;

  if(svcLocator->service("GeoModelSvc", geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error cannot access GeoModelSvc");
  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error cannot access RDBAccessSvc");

  DecodeVersionKey larVersionKey(geoModel, "LAr");

  RDBParamReader rdbr(rdbAccess);
  rdbr.
    data( "EmecGeometry", larVersionKey.tag(), larVersionKey.node() ).
    fallback_to( "EmecGeometry", "EmecGeometry-00" ).
    //     store to,                  name,   units
    param(m_zWheelRefPoint,            "Z0",  cm).
    param(m_dMechFocaltoWRP,           "Z1",  cm).
    param(m_dElecFocaltoWRP,          "DCF",  cm).
    param(m_HalfGapBetweenWheels,  "DCRACK",  cm).
    param(m_rOuterCutoff,          "RLIMIT",  cm).
    param(m_zShift,                "ZSHIFT",  cm)
    ;

  RDBParamRecords EmecWheelParameters_recs = rdbr.
    data( "EmecWheelParameters", larVersionKey.tag(), larVersionKey.node() ).
    fallback_to( "EmecWheelParameters", "EmecWheelParameters-00" );
  //        store to,     name,   units,   index(default=0)
  EmecWheelParameters_recs.
    param(m_eta_hi,    "ETAINT",  1.0).
    param(m_eta_mid,   "ETAEXT",  1.0).
    param(m_eta_low,   "ETAEXT",  1.0, 1)
    ;

  rdbr.
    data( "EmecMagicNumbers", larVersionKey.tag(), larVersionKey.node() ).
    fallback_to( "EmecMagicNumbers","EMECMagicNumbers-00" ).
    //          store to,                         name,     units
    param(m_ActiveLength,                   "ACTIVELENGTH",  mm).
    param(m_StraightStartSection,   "STRAIGHTSTARTSECTION",  mm).
    param(m_dWRPtoFrontFace,                 "REFTOACTIVE",  mm)
    ;

  m_WheelThickness = m_ActiveLength + 2.*m_StraightStartSection;
  m_HalfWheelThickness = m_WheelThickness * 0.5;

  std::string pr_opt_value;
  std::string sagging_opt_value;

  RDBParamRecords EMECParams_recs = rdbr.
    data( "EMECParams", larVersionKey.tag(), larVersionKey.node() ).
    fallback_to( "EMECParams", "EMECParams-00" );

  //          store to,                                             name
  EMECParams_recs.
    param(pr_opt_value,       "PHIROTATION").
    param(sagging_opt_value,      "SAGGING")
    ;

  m_phiRotation =  pr_opt_value == "g3"? true: false;

  m_zWheelFrontFace = m_dMechFocaltoWRP + m_dWRPtoFrontFace;
  m_zWheelBackFace = m_zWheelFrontFace + m_WheelThickness;

  msg << MSG::DEBUG << "... got these values:" << std::endl
      << "m_zWheelRefPoint       : " << m_zWheelRefPoint / cm << " [cm]" << std::endl
      << "m_dMechFocaltoWRP      : " << m_dMechFocaltoWRP / cm << " [cm]" << std::endl
      << "m_dElecFocaltoWRP      : " << m_dElecFocaltoWRP / cm << " [cm]" << std::endl
      << "m_HalfGapBetweenWheels : " << m_HalfGapBetweenWheels / cm << " [cm]" << std::endl
      << "m_rOuterCutoff         : " << m_rOuterCutoff / cm << " [cm]" << std::endl
      << "m_zWheelFrontFace      : " << m_zWheelFrontFace / cm << " [cm]" << std::endl
      << "m_zWheelBackFace       : " << m_zWheelBackFace / cm << " [cm]" << std::endl
      << "m_zShift               : " << m_zShift / cm << " [cm]" << std::endl
      << "Phi rotation           : " << (m_phiRotation? "true": "false") << std::endl
      << "eta wheels limits      : " << m_eta_low << ", " << m_eta_mid << ", " << m_eta_hi
      << endmsg;
  msg << MSG::VERBOSE << "hardcoded constants: " << std::endl
      << "m_WheelThickness       : " << m_WheelThickness / cm << " [cm]" << std::endl
      << "m_dWRPtoFrontFace      : " << m_dWRPtoFrontFace / cm << " [cm]"
      << endmsg;

  // Constructor initializes the geometry.

  m_isBarrette      = false;
  m_isBarretteCalib = false;
  m_isModule        = false;
  m_isElectrode     = false;
  m_isInner         = false;
  m_FirstFan        = 0;
  m_LastFan         = 0;

  switch(m_type){
    case LArG4::BackInnerBarretteWheelCalib:
      m_isBarretteCalib = true;
      /* FALLTHROUGH */
    case LArG4::BackInnerBarretteWheel:
      m_isBarrette = true;
      m_type = LArG4::InnerAbsorberWheel;
      /* FALLTHROUGH */
    case LArG4::InnerAbsorberWheel:
    case LArG4::InnerGlueWheel:
    case LArG4::InnerLeadWheel:
      inner_wheel_init(EmecWheelParameters_recs);
      m_ZeroFanPhi = m_FanStepOnPhi * 0.5;
      if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
      break;
    case LArG4::BackOuterBarretteWheelCalib:
      m_isBarretteCalib = true;
      /* FALLTHROUGH */
    case LArG4::BackOuterBarretteWheel:
      m_isBarrette = true;
      m_type = LArG4::OuterAbsorberWheel;
      /* FALLTHROUGH */
    case LArG4::OuterAbsorberWheel:
    case LArG4::OuterGlueWheel:
    case LArG4::OuterLeadWheel:
      outer_wheel_init(EmecWheelParameters_recs);
      m_ZeroFanPhi = m_FanStepOnPhi * 0.5;
      if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
      break;
    case LArG4::InnerElectrodWheel:
      inner_wheel_init(EmecWheelParameters_recs);
      m_ZeroFanPhi = 0;
      if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
      m_isElectrode = true;
      break;
    case LArG4::OuterElectrodWheel:
      outer_wheel_init(EmecWheelParameters_recs);
      m_ZeroFanPhi = 0;
      if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
      m_isElectrode = true;
      break;
    case LArG4::BackInnerBarretteModuleCalib:
      m_isBarretteCalib = true;
      /* FALLTHROUGH */
    case LArG4::BackInnerBarretteModule:
      m_isBarrette = true;
      m_type = LArG4::InnerAbsorberModule;
      /* FALLTHROUGH */
    case LArG4::InnerAbsorberModule:
      inner_wheel_init(EmecWheelParameters_recs);
      module_init();
      m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
      // later for all? m_ZeroFanPhi_ForDetNeaFan = m_ZeroFanPhi - m_FanStepOnPhi * 0.5;
      break;
    case LArG4::BackOuterBarretteModuleCalib:
      m_isBarretteCalib = true;
      /* FALLTHROUGH */
    case LArG4::BackOuterBarretteModule:
      m_isBarrette = true;
      m_type = LArG4::OuterAbsorberModule;
      /* FALLTHROUGH */
    case LArG4::OuterAbsorberModule:
      outer_wheel_init(EmecWheelParameters_recs);
      module_init();
      m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
      // later for all? m_ZeroFanPhi_ForDetNeaFan = m_ZeroFanPhi - m_FanStepOnPhi * 0.5;
      break;
    case LArG4::InnerElectrodModule:
      inner_wheel_init(EmecWheelParameters_recs);
      module_init();
      m_FirstFan ++;
      m_isElectrode = true;
      break;
    case LArG4::OuterElectrodModule:
      outer_wheel_init(EmecWheelParameters_recs);
      module_init();
      m_FirstFan ++;
      m_isElectrode = true;
      break;
    default:
      throw std::runtime_error("LArWheelCalculator constructor:unknown LArWheelCalculator_t");
  }
  m_ZeroFanPhi_ForDetNeaFan = m_ZeroFanPhi - m_FanStepOnPhi * 0.5;
  m_NumberOfHalfWaves = m_NumberOfWaves * 2;
  m_HalfWaveLength = m_ActiveLength / m_NumberOfHalfWaves;
  m_QuarterWaveLength = m_HalfWaveLength * 0.5;
  //m_HalfNumberOfFans = m_NumberOfFans / 2;
  m_FanHalfThickness = GetFanHalfThickness(m_type);

  // Init sagging
  // value read above
  // std::string sagging_opt_value = (*DB_EMECParams)[0]->getString("SAGGING");

  msg << MSG::VERBOSE << "SAGGING value = " << sagging_opt_value << endmsg;

  // the same condition is in DistanceCalculatorFactory::Create
  m_SaggingOn = (sagging_opt_value != "" && sagging_opt_value != "off")? true: false;

  m_distanceCalcImpl = LArWheelCalculator_Impl::DistanceCalculatorFactory::Create(
      sagging_opt_value, this, rdbAccess, larVersionKey);
  if (m_SaggingOn) {
    msg << MSG::VERBOSE << "Creating DistanceCalculatorSaggingOn = "  << this
        << ',' << m_distanceCalcImpl << endmsg;
  } else {
    msg << MSG::VERBOSE << "Creating DistanceCalculatorSaggingOff = " << this
        << ',' << m_distanceCalcImpl << endmsg;
  }

  m_fanCalcImpl = LArWheelCalculator_Impl::FanCalculatorFactory::Create(
      m_SaggingOn, m_isModule, this, rdbAccess, larVersionKey);

  //--------------------------
  // At this place there was the loading of sagging parameters
  // Transfered to DistanceCalculatorSaggingOn
  //--------------------------

  // Get option: Slant params.
  msg << MSG::VERBOSE << "Loading SlantAngle parameters ...";
  std::string slant_params;

  if (m_isInner) {
    EMECParams_recs.param(slant_params,  "INNERSLANTPARAM");
  } else {
    EMECParams_recs.param(slant_params,  "OUTERSLANTPARAM");
  }

  msg << (m_isInner?" InnerWheel ":" OuterWheel ") << slant_params << endmsg;

  if(slant_params != "" && slant_params != "default"){
    double a, b, c, d, e;
    if(sscanf(slant_params.c_str(), "%80le %80le %80le %80le %80le", &a, &b, &c, &d, &e) != 5){
      msg << MSG::ERROR
          << "LArWheelCalculator: ERROR: wrong value(s) "
          << "for EMEC slant angle parameters: "
          << slant_params << ", "
          << "defaults are used" << endmsg;
    } else {
      m_slant_parametrization[0] = a;
      m_slant_parametrization[1] = b;
      m_slant_parametrization[2] = c;
      m_slant_parametrization[3] = d;
      m_slant_parametrization[4] = e;
      m_slant_use_default = false;
    }
  } // else already initialized in inner/outer_wheel_init()

  fill_sincos_parameterization(); // initialize sin&cos parameterization

  msg << MSG::VERBOSE << "All params initialized. Print some internal variables" << endmsg;

  msg << MSG::VERBOSE << "Data members:" << std::endl
      << "m_AtlasZside              = " << m_AtlasZside << std::endl
      << "m_NumberOfFans            = " << m_NumberOfFans << std::endl
      << "m_ZeroFanPhi              = " << m_ZeroFanPhi << std::endl
      << "m_ZeroFanPhi_ForDetNeaFan = " << m_ZeroFanPhi_ForDetNeaFan << std::endl
      << "m_FanStepOnPhi            = " << m_FanStepOnPhi << std::endl
      << "m_FanHalfThickness        = " << m_FanHalfThickness << std::endl
      //<< "Sagging parameters        : " << m_sagging_parameter[0][0] << " " << m_sagging_parameter[0][1] << std::endl
      //<< "Sagging parameters        : " << m_sagging_parameter[1][0] << " " << m_sagging_parameter[1][1] << std::endl
      << "slant_params              = " << slant_params << std::endl
      << "Sagging option            = " << sagging_opt_value << std::endl
      << "SaggingOn                 = " << (m_SaggingOn? "true": "false") << std::endl
      << "Slant parameters          : ";
  for(int i = 0; i < 5; i ++) msg << " " << m_slant_parametrization[i];
  msg << endmsg;

  if(m_isModule){
    msg << MSG::VERBOSE
        << "module_init: FirstFan = " << m_FirstFan
        << ", LastFan = " << m_LastFan
        << ", ZeroFanPhi = " << m_ZeroFanPhi
        << endmsg;
  }

  //m_fan_number = -1000;

  // Is the following code fragment obsoleted? DM 2015-03-13
  /* to compare various methods of slant angle computation:
     if(isInner) return;
     FILE *O = fopen("slant_stat.table1.txt", "w");
     if(O == 0) abort();
     struct timeval t1, t2;
     struct timezone tz;
     std::vector<double> alpha;
     gettimeofday(&t1, &tz);
     for(double r = 600.; r < 2100.; r += .01){
     alpha.push_back(parameterized_slant_angle(r));
     }
     gettimeofday(&t2, &tz);

     fprintf(O, "%d.%06d %d.%06d\n", t1.tv_sec, t1.tv_usec, t2.tv_sec, t2.tv_usec);
     int i = 0;
     for(double r = 600.; r < 2100.; r += .01, i ++){
     fprintf(O, "%f %f\n", r, alpha[i]);
     }

     fclose(O);
     exit(0);
  */
}

/* converts module gap number into wheel gap number */
int LArWheelCalculator::PhiGapNumberForWheel(int i) const
{
	return m_fanCalcImpl->PhiGapNumberForWheel(i);
}

void LArWheelCalculator::inner_wheel_init(const RDBParamRecords & EmecWheelParameters_recs)
{
  for(int i = 0; i < 5; ++ i) m_slant_parametrization[i] = default_slant_parametrization[0][i];
  m_slant_use_default = true;

  EmecWheelParameters_recs.
    param(m_NumberOfFans,  "NABS", 0).
    param(m_NumberOfWaves, "NACC", 0)
    ;

  m_FanFoldRadius = 3.25*mm;
  m_ZeroGapNumber = 64; // internal constant, should not be taken from DB
  m_FanStepOnPhi = twopi / m_NumberOfFans;
  m_isInner = true;

}

void LArWheelCalculator::outer_wheel_init(const RDBParamRecords & EmecWheelParameters_recs)
{
  for(int i = 0; i < 5; ++ i) m_slant_parametrization[i] = default_slant_parametrization[1][i];
  m_slant_use_default = true;
  EmecWheelParameters_recs.
    param(m_NumberOfFans,  "NABS", 1).
    param(m_NumberOfWaves, "NACC", 1)
    ;

  m_FanFoldRadius = 3.0*mm;
  m_ZeroGapNumber = 192; // internal constant, should not be taken from DB
  m_FanStepOnPhi = twopi / m_NumberOfFans;
  m_isInner = false;

}

double LArWheelCalculator::GetFanHalfThickness(LArG4::LArWheelCalculator_t t)
{
  switch(t){
     case LArG4::BackInnerBarretteWheelCalib:
     case LArG4::BackInnerBarretteModuleCalib:
     case LArG4::BackInnerBarretteWheel:
     case LArG4::BackInnerBarretteModule:
     case LArG4::InnerAbsorberWheel:
     case LArG4::InnerAbsorberModule:
     //              return (2.2 / 2 + 0.2 + 0.15) * mm;
          return (2.2 / 2 + 0.2 + 0.1)*0.997 * mm; // new values, 02.11.06 J.T. with contraction in cold
     // lead / 2 + steel + glue
     case LArG4::InnerGlueWheel:
          return (2.2 / 2 + 0.1)*0.997 * mm;
     case LArG4::InnerLeadWheel:
          return 2.2 / 2 * 0.997 * mm;

     case LArG4::BackOuterBarretteWheelCalib:
     case LArG4::BackOuterBarretteModuleCalib:
     case LArG4::BackOuterBarretteWheel:
     case LArG4::BackOuterBarretteModule:
     case LArG4::OuterAbsorberWheel:
     case LArG4::OuterAbsorberModule:
          //return (1.7 / 2 + 0.2 + 0.15) * mm;
          return (1.69 / 2 + 0.2 + 0.1)*0.997 * mm;  // new values, 02.11.06 J.T.
     case LArG4::OuterGlueWheel:
          return (1.69 / 2 + 0.1)*0.997 * mm;
     case LArG4::OuterLeadWheel:
          return 1.69 / 2 * 0.997 * mm;

     case LArG4::InnerElectrodWheel:
     case LArG4::OuterElectrodWheel:
     case LArG4::InnerElectrodModule:
     case LArG4::OuterElectrodModule:
          return 0.275/1.0036256 *mm * 0.5;  //new values, 02.11.06 J.T
  }
  throw std::runtime_error("LArWheelCalculator::GetFanHalfThickness: wrong wheel type");
}

void LArWheelCalculator::module_init(void)
{
  m_isModule = true;
  m_LastFan = m_NumberOfFans / 8;
  m_FirstFan = 0;
  m_ZeroFanPhi = - m_LastFan / 2 * m_FanStepOnPhi;
}

/*
  array of r is filled with:
  for inner wheel - 2 elements { r_front, r_back }
  for outer wheel - 3 elements { r_front, r_middle, r_back }
  return value - delta_z of middle point in case of outer wheel
*/
double LArWheelCalculator::GetWheelInnerRadius(double *r) const
{
  double zMid = 0.;
  if(m_isInner){
    double tanThetaInner = 2. * exp(-m_eta_hi ) / (1. - exp(-2.*m_eta_hi ));
    r[0] = m_zWheelFrontFace * tanThetaInner;
    r[1] = m_zWheelBackFace  * tanThetaInner;
  } else {
    double tanThetaMid   = 2. * exp(-m_eta_mid) / (1. - exp(-2.*m_eta_mid));
    double inv_tanThetaOuter = (1. - exp(-2.*m_eta_low)) / (2. * exp(-m_eta_low));
    // Note that there is a 3mm gap between the outer surface of the
    // inner wheel and the inner surface of the outer wheel.
    r[0] = m_zWheelFrontFace * tanThetaMid + m_HalfGapBetweenWheels;
    r[1] = m_rOuterCutoff * inv_tanThetaOuter * tanThetaMid + m_HalfGapBetweenWheels;
    r[2] = m_zWheelBackFace  * tanThetaMid + m_HalfGapBetweenWheels;
    zMid = m_rOuterCutoff * inv_tanThetaOuter - m_zWheelFrontFace;
  }
  return zMid;
}

/*
  array of r is filled with:
  for inner wheel - 2 elements { r_front, r_back }
  for outer wheel - 3 elements { r_front, r_middle, r_back }
*/
void LArWheelCalculator::GetWheelOuterRadius(double *r) const
{
  if(m_isInner){
    double tanThetaMid   = 2. * exp(-m_eta_mid) / (1. - exp(-2.*m_eta_mid));
    // Note that there is a 3mm gap between the outer surface of the
    // inner wheel and the inner surface of the outer wheel.
    r[0] = m_zWheelFrontFace * tanThetaMid - m_HalfGapBetweenWheels;
    r[1] = m_zWheelBackFace  * tanThetaMid - m_HalfGapBetweenWheels;
  } else {
    double tanThetaOuter = 2. * exp(-m_eta_low) / (1. - exp(-2.*m_eta_low));
    r[0] = m_zWheelFrontFace * tanThetaOuter;
    r[1] = m_rOuterCutoff;
    r[2] = m_rOuterCutoff;
  }
}

