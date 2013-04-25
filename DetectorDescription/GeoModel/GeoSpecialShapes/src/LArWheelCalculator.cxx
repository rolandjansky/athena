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
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;

#include "GeoSpecialShapes/LArWheelCalculator.h"

// uncomment this to activate test code for
// styding EMEC geometry deformations
#define LARWHEELCALCULATOR_STUDY_DEFORMATIONS

#ifdef LARWHEELCALCULATOR_STUDY_DEFORMATIONS
#include <sys/time.h>
#include <stdio.h>
#endif

#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
//define table for folding angle computation, requested in LArEMECRunControl;
std::string  LArWheelCalculator::OptFoldA="iter";
bool    LArWheelCalculator::FoldAInnerPrepared=false;
bool    LArWheelCalculator::FoldAOuterPrepared=false;
double* LArWheelCalculator::FoldAInner=0; // table for folding angle computation
double* LArWheelCalculator::FoldAOuter=0;
double  LArWheelCalculator::RminAInner=300.*mm;  //limits for the elecric Field map
double  LArWheelCalculator::RmaxAInner=800.*mm;
double  LArWheelCalculator::RminAOuter=600.*mm;
double  LArWheelCalculator::RmaxAOuter=2100.*mm;
double  LArWheelCalculator::RstepA    =0.01*mm;
#endif // LARWHEELCALCULATOR_PSA_DEVELOPMENT

// The radial boundaries of the inner and outer wheels are defined
// by values of eta, the distance from z=0 to the front face of the
// wheel, and the thickness of the wheel.

// these numbers are taken from DB in constructor,
// hardcoded values here are just for reference
/*
double LArWheelCalculator::zWheelRefPoint		= 3689.5*mm;  //=endg_z0
double LArWheelCalculator::dMechFocaltoWRP  	= 3691. *mm;  //=endg_z1
double LArWheelCalculator::dElecFocaltoWRP  	= 3689. *mm;  //=endg_dcf
double LArWheelCalculator::rOuterCutoff 		= 2034. *mm;  //=endg_rlimit
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

const char *LArWheelCalculator::LArWheelCalculatorTypeString(LArWheelCalculator_t type)
{
	switch(type){
	case InnerAbsorberWheel:           return("InnerAbsorberWheel");
	case OuterAbsorberWheel:           return("OuterAbsorberWheel");
	case InnerElectrodWheel:           return("InnerElectrodWheel");
	case OuterElectrodWheel:           return("OuterElectrodWheel");
	case InnerAbsorberModule:          return("InnerAbsorberModule");
	case OuterAbsorberModule:          return("OuterAbsorberModule");
	case InnerElectrodModule:          return("InnerElectrodModule");
	case OuterElectrodModule:          return("OuterElectrodModule");
	case BackOuterBarretteWheel:       return("BackOuterBarretteWheel");
	case BackInnerBarretteWheel:       return("BackInnerBarretteWheel");
	case BackOuterBarretteModule:      return("BackOuterBarretteModule");
	case BackInnerBarretteModule:      return("BackInnerBarretteModule");
	case BackOuterBarretteWheelCalib:  return("BackOuterBarretteWheelCalib");
	case BackInnerBarretteWheelCalib:  return("BackInnerBarretteWheelCalib");
	case BackOuterBarretteModuleCalib: return("BackOuterBarretteModuleCalib");
	case BackInnerBarretteModuleCalib: return("BackInnerBarretteModuleCalib");
	case InnerGlueWheel:               return("InnerGlueWheel");
	case InnerLeadWheel:               return("InnerLeadWheel");
	case OuterGlueWheel:               return("OuterGlueWheel");
	case OuterLeadWheel:               return("OuterLeadWheel");
	}
	return("unknown");
}

LArWheelCalculator::~LArWheelCalculator() {}

LArWheelCalculator::LArWheelCalculator(LArWheelCalculator_t a_wheelType, int zside) : m_type(a_wheelType), m_AtlasZside(zside)
{
  // Get pointer to the message service
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	IMessageSvc* msgSvc;
	StatusCode status = svcLocator->service("MessageSvc", msgSvc);
	if(status.isFailure()){
		throw std::runtime_error("LArWheelCalculator constructor: \
cannot initialze message service");
	}
	MsgStream msg(msgSvc, "LArWheelCalculator");
	msg << MSG::DEBUG << "LArWheelCalculator constructor (type "
	         << LArWheelCalculatorTypeString(m_type)
	         << "):" << endreq;
	// Access source of detector parameters.
	msg << MSG::DEBUG
	         << "initializing data members from DB..." << endreq;

	IGeoModelSvc *geoModel;
	IRDBAccessSvc* rdbAccess;

	if(svcLocator->service("GeoModelSvc", geoModel) == StatusCode::FAILURE)
		throw std::runtime_error("Error cannot access GeoModelSvc");
	if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
		throw std::runtime_error("Error cannot access RDBAccessSvc");

	DecodeVersionKey larVersionKey(geoModel, "LAr");

	rdbAccess->connect();

	IRDBRecordset_ptr DB_EmecGeometry =
		rdbAccess->getRecordsetPtr("EmecGeometry", larVersionKey.tag(), larVersionKey.node());
	if(DB_EmecGeometry->size() == 0){
		DB_EmecGeometry = rdbAccess->getRecordsetPtr("EmecGeometry", "EmecGeometry-00");
	}

	m_zWheelRefPoint = (*DB_EmecGeometry)[0]->getDouble("Z0") *cm;
	m_dMechFocaltoWRP = (*DB_EmecGeometry)[0]->getDouble("Z1") *cm;
	m_dElecFocaltoWRP = (*DB_EmecGeometry)[0]->getDouble("DCF") *cm;
	m_HalfGapBetweenWheels = (*DB_EmecGeometry)[0]->getDouble("DCRACK") *cm;
	m_rOuterCutoff = (*DB_EmecGeometry)[0]->getDouble("RLIMIT") *cm;
	m_zShift = (*DB_EmecGeometry)[0]->getDouble("ZSHIFT") *cm;

	IRDBRecordset_ptr DB_EmecWheelParameters =
		rdbAccess->getRecordsetPtr("EmecWheelParameters", larVersionKey.tag(), larVersionKey.node());
	if(DB_EmecWheelParameters->size() == 0){
		DB_EmecWheelParameters = rdbAccess->getRecordsetPtr("EmecWheelParameters", "EmecWheelParameters-00");
	}

	m_eta_hi  = (*DB_EmecWheelParameters)[0]->getDouble("ETAINT");
	m_eta_mid = (*DB_EmecWheelParameters)[0]->getDouble("ETAEXT");
	m_eta_low = (*DB_EmecWheelParameters)[1]->getDouble("ETAEXT");

	IRDBRecordset_ptr DB_EMECmn =
		rdbAccess->getRecordsetPtr("EmecMagicNumbers", larVersionKey.tag(), larVersionKey.node());
	if(DB_EMECmn->size() == 0)
		DB_EMECmn = rdbAccess->getRecordsetPtr("EmecMagicNumbers","EMECMagigNumbers-00");
	m_ActiveLength = (*DB_EMECmn)[0]->getDouble("ACTIVELENGTH")*mm;
	try {
		m_StraightStartSection = (*DB_EMECmn)[0]->getDouble("STRAIGHTSTARTSECTION")*mm;
	}
	catch(...){
		m_StraightStartSection = 2.*mm;
		msg << MSG::WARNING
		    << "cannot get STRAIGHTSTARTSECTION from DB"
			<< endreq;
	}
	m_dWRPtoFrontFace = (*DB_EMECmn)[0]->getDouble("REFTOACTIVE")*mm;
	m_WheelThickness = m_ActiveLength + 2.*m_StraightStartSection;
	m_HalfWheelThickness = m_WheelThickness * 0.5;

	IRDBRecordset_ptr DB_EMECParams =
		rdbAccess->getRecordsetPtr("EMECParams", larVersionKey.tag(), larVersionKey.node());
	if(DB_EMECParams->size() == 0)
		DB_EMECParams = rdbAccess->getRecordsetPtr("EMECParams","EMECParams-00");

	std::string pr_opt_value = (*DB_EMECParams)[0]->getString("PHIROTATION");
	m_phiRotation =  pr_opt_value == "g3"? true: false;

	m_zWheelFrontFace = m_dMechFocaltoWRP + m_dWRPtoFrontFace;
	m_zWheelBackFace = m_zWheelFrontFace + m_WheelThickness;

	rdbAccess->disconnect();

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
		<< endreq;
	msg << MSG::DEBUG << "hardcoded constants: " << std::endl
	    << "m_WheelThickness       : " << m_WheelThickness / cm << " [cm]" << std::endl
		<< "m_dWRPtoFrontFace      : " << m_dWRPtoFrontFace / cm << " [cm]"
		<< endreq;
#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
	msg << MSG::DEBUG << "LARWHEELCALCULATOR_PSA_DEVELOPMENT defined, runtime selection allowed" << endreq;
#else
	msg << MSG::DEBUG << "LARWHEELCALCULATOR_PSA_DEVELOPMENT is not defined, default method 'param' is used \
in parameterized_slant_angle()" << endreq;
#endif // LARWHEELCALCULATOR_PSA_DEVELOPMENT

	// Constructor initializes the geometry.

	m_isBarrette      = false;
	m_isBarretteCalib = false;
	m_isModule        = false;
	m_isElectrode     = false;
	m_isInner         = false;
	m_FirstFan        = 0;
	m_LastFan         = 0;

	switch(m_type){
	case BackInnerBarretteWheelCalib:
		m_isBarretteCalib = true;
	case BackInnerBarretteWheel:
		m_isBarrette = true;
		m_type = InnerAbsorberWheel;
	case InnerAbsorberWheel:
	case InnerGlueWheel:
	case InnerLeadWheel:
		inner_wheel_init(DB_EmecWheelParameters.get());
		m_ZeroFanPhi = m_FanStepOnPhi * 0.5;
		if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
		break;
	case BackOuterBarretteWheelCalib:
	    m_isBarretteCalib = true;
	case BackOuterBarretteWheel:
		m_isBarrette = true;
		m_type = OuterAbsorberWheel;
	case OuterAbsorberWheel:
	case OuterGlueWheel:
	case OuterLeadWheel:
		outer_wheel_init(DB_EmecWheelParameters.get());
		m_ZeroFanPhi = m_FanStepOnPhi * 0.5;
		if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
		break;
	case InnerElectrodWheel:
		inner_wheel_init(DB_EmecWheelParameters.get());
		m_ZeroFanPhi = 0;
		if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
		m_isElectrode = true;
		break;
	case OuterElectrodWheel:
		outer_wheel_init(DB_EmecWheelParameters.get());
		m_ZeroFanPhi = 0;
		if(m_phiRotation) m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
		m_isElectrode = true;
		break;
	case BackInnerBarretteModuleCalib:
		m_isBarretteCalib = true;
	case BackInnerBarretteModule:
		m_isBarrette = true;
		m_type = InnerAbsorberModule;
	case InnerAbsorberModule:
		inner_wheel_init(DB_EmecWheelParameters.get());
		module_init();
		m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
// later for all?		m_ZeroFanPhi_ForDetNeaFan = m_ZeroFanPhi - m_FanStepOnPhi * 0.5;
		break;
	case BackOuterBarretteModuleCalib:
	    m_isBarretteCalib = true;
	case BackOuterBarretteModule:
		m_isBarrette = true;
		m_type = OuterAbsorberModule;
	case OuterAbsorberModule:
		outer_wheel_init(DB_EmecWheelParameters.get());
		module_init();
		m_ZeroFanPhi += m_FanStepOnPhi * 0.5;
// later for all?		m_ZeroFanPhi_ForDetNeaFan = m_ZeroFanPhi - m_FanStepOnPhi * 0.5;
		break;
	case InnerElectrodModule:
		inner_wheel_init(DB_EmecWheelParameters.get());
		module_init();
		m_FirstFan ++;
		m_isElectrode = true;
		break;
	case OuterElectrodModule:
		outer_wheel_init(DB_EmecWheelParameters.get());
		module_init();
		m_FirstFan ++;
		m_isElectrode = true;
		break;
	default:
		throw std::runtime_error("LArWheelCalculator constructor:\
unknown LArWheelCalculator_t");
	}
	m_ZeroFanPhi_ForDetNeaFan = m_ZeroFanPhi - m_FanStepOnPhi * 0.5;
	m_NumberOfHalfWaves = m_NumberOfWaves * 2;
	m_HalfWaveLength = m_ActiveLength / m_NumberOfHalfWaves;
	m_QuarterWaveLength = m_HalfWaveLength * 0.5;
	m_HalfNumberOfFans = m_NumberOfFans / 2;
	m_FanHalfThickness = GetFanHalfThickness(m_type);

// Init sagging
	m_sagging_parameter.resize (m_NumberOfFans, std::vector<double> (5, 0.));
	std::string sagging_opt_value = (*DB_EMECParams)[0]->getString("SAGGING");

#ifdef LARWHEELCALCULATOR_STUDY_DEFORMATIONS
#define EMECPARAMFILE "EMECPARAM"
#define READEMECPARAM(a, key) \
{ \
	FILE *E = fopen(EMECPARAMFILE, "r"); \
	if(E != 0){ \
		char buf[1024]; \
		while(!feof(E)){ \
			fgets(buf, 1023, E); \
			char *b = strrchr(buf, '\n'); \
			if(b) *b = 0; \
			if(strncmp(key, buf, strlen(key)) == 0){ \
				a = buf + strlen(key); \
				msg << MSG::INFO << "data from " \
				    << EMECPARAMFILE << " is used for " << key << "!!!" \
		    	    << endreq; \
				break; \
			} \
		} \
		fclose(E); \
	} \
}
	READEMECPARAM(sagging_opt_value, "sagging ")
#endif // LARWHEELCALCULATOR_STUDY_DEFORMATIONS

	m_SaggingOn = (sagging_opt_value != "" && sagging_opt_value != "off")? true: false;
	if(m_SaggingOn){
		if(sagging_opt_value.substr(0, 4) == "file"){
			std::string sag_file = sagging_opt_value.substr(5);
			msg << MSG::DEBUG
			    << "geting sagging parameters from file "
				<< sag_file << " ..." << endreq;
			FILE *F = fopen(sag_file.c_str(), "r");
			if(F == 0){
		   		msg << MSG::FATAL
				    << "cannot open EMEC sagging parameters file "
					<< sag_file
					<< endreq;
				throw std::runtime_error("LArWheelCalculator: read sagging parameters from file");
			}
			int s, w, t, n;
			double p0, p1, p2, p3, p4;
			while(!feof(F)
			&& fscanf(F, "%d %d %d %d %le %le %le %le %le",
			          &s, &w, &t, &n, &p0, &p1, &p2, &p3, &p4) == 9)
			{
				if(s == m_AtlasZside
				&& ((w == 0 && m_isInner) || (w == 1 && !m_isInner))
				&& ((t == 0 && !m_isElectrode) || (t == 1 && m_isElectrode))
				&& (n >= 0 && n < m_NumberOfFans))
				{
					m_sagging_parameter[n][0] = p0;
					m_sagging_parameter[n][1] = p1;
					m_sagging_parameter[n][2] = p2;
					m_sagging_parameter[n][3] = p3;
					m_sagging_parameter[n][4] = p4;
					msg << MSG::VERBOSE
					    << "sagging for " << s << " " << w << " " << t
						<< " " << n << ": " << p0 << " " << p1 << " "
						<< p2 << " " << p3 << endreq;
				}
			}
			fclose(F);
		} else {
			double a, b, c, d;
			if(sscanf(sagging_opt_value.c_str(), "%le %le %le %le", &a, &b, &c, &d) != 4){
	    		msg << MSG::ERROR
				    << "wrong value(s) "
					<< " for EMEC sagging parameters: "
					<< sagging_opt_value << ", defaults are used" << endreq;
			} else {
				for(int j = 0; j < m_NumberOfFans; j ++){
					if(m_isInner){
						m_sagging_parameter[j][1] = a;
						m_sagging_parameter[j][0] = b * mm;
					} else {
						m_sagging_parameter[j][1] = c;
						m_sagging_parameter[j][0] = d * mm;
					}
				}
			}
		}
	}

  // Get option: Slant params.
	std::string slant_params;
	if(m_isInner) slant_params = (*DB_EMECParams)[0]->getString("INNERSLANTPARAM");
	else slant_params = (*DB_EMECParams)[0]->getString("OUTERSLANTPARAM");

#ifdef LARWHEELCALCULATOR_STUDY_DEFORMATIONS
	if(m_isInner) READEMECPARAM(slant_params, "innerslant ")
	else READEMECPARAM(slant_params, "outerslant ")
#endif // LARWHEELCALCULATOR_STUDY_DEFORMATIONS

	if(slant_params != "" && slant_params != "default"){
		double a, b, c, d, e;
		if(sscanf(slant_params.c_str(), "%le %le %le %le %le", &a, &b, &c, &d, &e) != 5){
			msg << MSG::ERROR
			    << "LArWheelCalculator: ERROR: wrong value(s) "
				<< "for EMEC slant angle parameters: "
				<< slant_params << ", "
				<< "defaults are used" << endreq;
		} else {
			m_slant_parametrization[0] = a;
			m_slant_parametrization[1] = b;
			m_slant_parametrization[2] = c;
			m_slant_parametrization[3] = d;
			m_slant_parametrization[4] = e;
			m_slant_use_default = false;
		}
	} // else already initialized in inner/outer_wheel_init()

	msg << MSG::DEBUG << "Data members:" << std::endl
	    << "m_AtlasZside              = " << m_AtlasZside << std::endl
	    << "m_NumberOfFans            = " << m_NumberOfFans << std::endl
	    << "m_ZeroFanPhi              = " << m_ZeroFanPhi << std::endl
	    << "m_ZeroFanPhi_ForDetNeaFan = " << m_ZeroFanPhi_ForDetNeaFan << std::endl
	    << "m_FanStepOnPhi            = " << m_FanStepOnPhi << std::endl
	    << "m_FanHalfThickness        = " << m_FanHalfThickness << std::endl
	    << "Sagging parameters        : " << m_sagging_parameter[0][0] << " "
	    << m_sagging_parameter[0][1] << std::endl
	    << "Sagging parameters        : " << m_sagging_parameter[1][0] << " "
	    << m_sagging_parameter[1][1] << std::endl
	    << "slant_params              = " << slant_params << std::endl
	    << "Sagging option            = " << sagging_opt_value << std::endl
	    << "SaggingOn                 = " << (m_SaggingOn? "true": "false") << std::endl
	    << "Slant parameters          : ";
	for(int i = 0; i < 5; i ++) msg << " " << m_slant_parametrization[i];
	msg << endreq;

	if(m_isModule){
		msg << MSG::DEBUG
		    << "module_init: FirstFan = " << m_FirstFan
			<< ", LastFan = " << m_LastFan
			<< ", ZeroFanPhi = " << m_ZeroFanPhi
			<< endreq;
	}

	m_fan_number = -1000;
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
	if(m_isModule){
		i += m_ZeroGapNumber;
		i -= m_LastFan / 2;
		if(i < 0) i += m_NumberOfFans;
		if(i >= m_NumberOfFans) i -= m_NumberOfFans;
//		i ++;
	}
	return i;
}


void LArWheelCalculator::inner_wheel_init(const IRDBRecordset *DB_EmecWheelParameters)
{
	for(int i = 0; i < 5; ++ i) m_slant_parametrization[i] = default_slant_parametrization[0][i];
	m_slant_use_default = true;
	m_NumberOfFans = (*DB_EmecWheelParameters)[0]->getInt("NABS");
	m_NumberOfWaves = (*DB_EmecWheelParameters)[0]->getInt("NACC");
	m_FanFoldRadius = 3.25*mm;
	m_ZeroGapNumber = 64; // internal constant, should not be taken from DB
	m_FanStepOnPhi = twopi / m_NumberOfFans;
	m_isInner = true;

#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
// optimized parameters for iterativ calculation of slant angle (see LAr report No:..)
//(JT)

	AlfInt=107. *deg; //fold angle at inner radius at the middle of the wheel= ROPT[0]
	AlfExt= 64.5*deg; //------        outer   -----------                    = ROPT[1]

	double zmidle=m_dMechFocaltoWRP+m_dWRPtoFrontFace+m_HalfWheelThickness;
	double tetint=2.*atan(exp(-m_eta_hi));
	double tetext=2.*atan(exp(-m_eta_mid));
	double hwl=m_ActiveLength/m_NumberOfWaves * 0.5;

	ROpt[0]=zmidle*tan(tetint);
	ROpt[1]=zmidle*tan(tetext);
	// the length of the arc of the waves
	ZlOpt[0]=hwl/sin(AlfInt/2.)-2.*m_FanFoldRadius/tan(AlfInt/2.)+(pi-AlfInt)*m_FanFoldRadius;
	ZlOpt[1]=hwl/sin(AlfExt/2.)-2.*m_FanFoldRadius/tan(AlfExt/2.)+(pi-AlfExt)*m_FanFoldRadius;

	FoldA = FoldAInner;
	RminA = RminAInner;
	RmaxA = RmaxAInner;

	if(!FoldAInnerPrepared){

	  int length = int( (RmaxAInner-RminAInner)/RstepA ) + 2;
	  FoldAInner = new double [length];
	  FoldA = FoldAInner;

	  std::cout
		 <<" LArWheelCalculator::inner_wheel_ini: AlfInt,ext[deg]="
		 <<AlfInt/deg<<" "<<AlfExt/deg<<" ROpt[mm]="<<ROpt[0]<<" "<<ROpt[1]
		 <<" ZlOpt[mm]="<<ZlOpt[0]<<" "<<ZlOpt[1]
		 <<" RminA= "<<RminA<<" RmaxA= "<<RmaxA<<" RstepA= "<<RstepA
		 <<std::endl;

	  for(int i=0;i<length;++i){

     	    FoldAInner[i]=( pi-AlphIter(hwl,RminA+i*RstepA) )/2.;
	    if( (100+i)%100 == 0 ) {
	      std::cout
		     <<" r="<<RminA+i*RstepA<<" FoldAInner[deg]="<< FoldAInner[i]/pi*180.
		     <<" alf[deg]="<< (pi-2*FoldAInner[i])/pi*180.
		     <<std::endl;
	    }
	  } // end of iteration

	  FoldAInnerPrepared=true;
	} //end of defining the table
#endif // LARWHEELCALCULATOR_PSA_DEVELOPMENT
}

void LArWheelCalculator::outer_wheel_init(const IRDBRecordset *DB_EmecWheelParameters)
{
	for(int i = 0; i < 5; ++ i) m_slant_parametrization[i] = default_slant_parametrization[1][i];
	m_slant_use_default = true;
	m_NumberOfFans = (*DB_EmecWheelParameters)[1]->getInt("NABS");
	m_NumberOfWaves = (*DB_EmecWheelParameters)[1]->getInt("NACC");
	m_FanFoldRadius = 3.0*mm;
	m_ZeroGapNumber = 192; // internal constant, should not be taken from DB
	m_FanStepOnPhi = twopi / m_NumberOfFans;
	m_isInner = false;

#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
// optimized parameters for iterativ calculation of slant angle (see LAr report No:..)
//(JT)

	double zmidle=m_dMechFocaltoWRP+m_dWRPtoFrontFace+m_HalfWheelThickness;
	double tetint=2.*atan(exp(-m_eta_mid));
	double hwl=m_ActiveLength/m_NumberOfWaves * 0.5;

	AlfInt=122.5*deg;
	AlfExt= 59. *deg;
	ROpt[0]=zmidle*tan(tetint);
	ROpt[1]=m_rOuterCutoff;
	ZlOpt[0]=hwl/sin(AlfInt/2.)-2.*m_FanFoldRadius/tan(AlfInt/2.)+(pi-AlfInt)*m_FanFoldRadius;
	ZlOpt[1]=hwl/sin(AlfExt/2.)-2.*m_FanFoldRadius/tan(AlfExt/2.)+(pi-AlfExt)*m_FanFoldRadius;

	FoldA = FoldAOuter;
	RminA = RminAOuter;
	RmaxA = RmaxAOuter;

	if(!FoldAOuterPrepared){

	  int length = int( (RmaxAOuter-RminAOuter)/RstepA ) + 2;
	  FoldAOuter = new double [length];
	  FoldA = FoldAOuter;

	std::cout
		 <<" LArWheelCalculator::outer_wheel_ini: AlfInt,ext[deg]="
		 <<AlfInt/deg<<" "<<AlfExt/deg<<" ROpt[mm]="<<ROpt[0]<<" "<<ROpt[1]
		 <<" ZlOpt[mm]="<<ZlOpt[0]<<" "<<ZlOpt[1]
		 <<" RminA= "<<RminA<<" RmaxA= "<<RmaxA<<" RstepA= "<<RstepA
	         <<std::endl;


	  for(int i=0;i<length;++i){

     	    FoldAOuter[i]=( pi-AlphIter(hwl,RminA+i*RstepA) )/2.;

	    if( (100+i)%100 == 0 ) {
	      std::cout
		     <<" r="<<RminA+i*RstepA<<" FoldAOuter[deg]="<< FoldAOuter[i]/pi*180.
		     <<" alf[deg]="<< (pi-2*FoldAOuter[i])/pi*180.
		     <<std::endl;
	    }
	  }  // end of iteration
	  FoldAOuterPrepared=true;
	} //end of defining the table
#endif // LARWHEELCALCULATOR_PSA_DEVELOPMENT
}

double LArWheelCalculator::GetFanHalfThickness(LArWheelCalculator_t t)
{
	switch(t){
	case BackInnerBarretteWheelCalib:
	case BackInnerBarretteModuleCalib:
	case BackInnerBarretteWheel:
	case BackInnerBarretteModule:
	case InnerAbsorberWheel:
	case InnerAbsorberModule:
//              return (2.2 / 2 + 0.2 + 0.15) * mm;
 		return (2.2 / 2 + 0.2 + 0.1)*0.997 * mm; // new values, 02.11.06 J.T. with contraction in cold
		    // lead / 2 + steel + glue
	case InnerGlueWheel:
		return (2.2 / 2 + 0.1)*0.997 * mm;
	case InnerLeadWheel:
		return 2.2 / 2 * 0.997 * mm;

	case BackOuterBarretteWheelCalib:
	case BackOuterBarretteModuleCalib:
	case BackOuterBarretteWheel:
	case BackOuterBarretteModule:
	case OuterAbsorberWheel:
	case OuterAbsorberModule:
//		return (1.7 / 2 + 0.2 + 0.15) * mm;
		return (1.69 / 2 + 0.2 + 0.1)*0.997 * mm;  // new values, 02.11.06 J.T.
	case OuterGlueWheel:
		return (1.69 / 2 + 0.1)*0.997 * mm;
	case OuterLeadWheel:
		return 1.69 / 2 * 0.997 * mm;

	case InnerElectrodWheel:
	case OuterElectrodWheel:
	case InnerElectrodModule:
	case OuterElectrodModule:
		return 0.275/1.0036256 *mm * 0.5;  //new values, 02.11.06 J.T
	}
	throw std::runtime_error("LArWheelCalculator::GetFanHalfThickness:\
wrong wheel type");
	return 0.;
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
		double tanThetaOuter = 2. * exp(-m_eta_low) / (1. - exp(-2.*m_eta_low));
  // Note that there is a 3mm gap between the outer surface of the
  // inner wheel and the inner surface of the outer wheel.
		r[0] = m_zWheelFrontFace * tanThetaMid + m_HalfGapBetweenWheels;
		r[1] = m_rOuterCutoff / tanThetaOuter * tanThetaMid + m_HalfGapBetweenWheels;
		r[2] = m_zWheelBackFace  * tanThetaMid + m_HalfGapBetweenWheels;
		zMid = m_rOuterCutoff / tanThetaOuter - m_zWheelFrontFace;
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
