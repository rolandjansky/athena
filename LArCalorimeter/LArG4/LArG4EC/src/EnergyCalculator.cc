/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EnergyCalculator
// Prepared 10-Apr-2002 by Bill Seligman
// from code written by Jozsef Toth.
// GapAdjustmena_StartPointt by S.Negroni
// 21-May-2003  J.T. Charge collection included
// 02-July-2003 J.T. Charge collection modified,code improved
// 20-July-2004 J.T. FieldMapVersion definition added
//                   FieldMapVersion="Old" or "New" should be defined
//                    in the code, parallel to the definition of the filename
//                    containing the fieldmap of version Old or New.
// 25-May-2005  J.T. - Gapadjustment changed: G4 steps subdivided, correction is
//                    done for each substep;
//                   - GapAdjusment_E: E_field estimated in gapadj method by U/gap;
//                     Ionrecomb effect and v_drift computed using this Efield;
//                   - treatment of HV from emec's power supplies introduced
//                      - HV supplies positioned in the coord system of Wheel
//                      - HV values are included in HVEMECMap.dat file;
//                        it contains HV - for + and -z Atlas side
//                                       - for 9x32 HV in eta x phi segmentation
//                                       - different for both sides of electrodes
// 01-July-2005 J.T. - fieldmap (version v02) extended for outer wheel;
//                   - 5 new layers 1200< r < 1500 mm are introduced
// 31-July-2005 J.T. - new energy correction options for chcoll
//                     syntax:  chcmap=v02   ==> define version number of fieldmap
//                              chcesr=0.2   ==> define distance [mm] from electrode,
//                                               where signals are cancelled
// 06-Nov-2005 J.T. - get options from StoreGate
// Nov 2005 AMS - alot of structural changes
// Sep-2006 J.T.  - collect signal from the Barrette volume
// Apr 2007 AMS - make use of m_AtlasZside which is set on initialization stage
// May-June 2009 AMS Move to namespace LArG4::EC
//                   General code cleanup

#include <cmath>
#include <cassert>
#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "PathResolver/PathResolver.h"
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "globals.hh"

#include "LArG4Code/LArG4BirksLaw.h"
#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4RunControl/LArG4EMECOptions.h"
#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "LArHV/LArHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVModuleConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVDescriptor.h"

#include "LArG4EC/EMECSupportCalibrationCalculator.h"
#include "LArG4EC/EnergyCalculator.h"

#define MSG_VECTOR(v) "(" << v.x() << ", " << v.y() << ", " << v.z() << ")"

using namespace LArG4::EC;

// ****************************************************************************
EnergyCalculator::Wheel_Efield_Map EnergyCalculator::ChCollInner;
EnergyCalculator::Wheel_Efield_Map EnergyCalculator::ChCollOuter;

const G4double            EnergyCalculator::GridSize        =0.1; //[mm]
const G4double            EnergyCalculator::AverageGap      = 1.3*CLHEP::mm;
      G4bool              EnergyCalculator::FieldMapsRead   =false;
      G4String            EnergyCalculator::FieldMapVersion = "";
// ****************************************************************************
const G4String EnergyCalculator::HVEMECMapFileName="/afs/cern.ch/atlas/offline/data/lar/emec/efield/HVEMECMap.dat";
      G4bool   EnergyCalculator::HVMapRead=false;
      G4String EnergyCalculator::HVMapVersion="unknown";
const G4double EnergyCalculator::HV_Etalim[NofEtaSection+1]=
                                    {1.375,1.5,1.6,1.8,2.,2.1,2.3,2.5,2.8,3.2};
const G4double EnergyCalculator::LArTemperature_ECC0=88.15; //K
const G4double EnergyCalculator::LArTemperature_ECC1=88.37;
const G4double EnergyCalculator::LArTemperature_ECC5=87.97;
const G4double EnergyCalculator::LArTemperature_av  =88.16;
      G4int    EnergyCalculator::HV_Start_phi[NofAtlasSide][NofEtaSection][NofElectrodeSide];
      G4double EnergyCalculator::HV_Values[NofAtlasSide][NofEtaSection][NofElectrodeSide][NofElectrodesOut];
const G4double EnergyCalculator::AverageHV=1250.;//[V]
const G4double EnergyCalculator::AverageEfield=0.01*AverageHV/AverageGap;//[kv/cm]
const G4double EnergyCalculator::AverageCurrent=1./AverageGap*IonReco(AverageEfield)
                                        *DriftVelo(LArTemperature_av,AverageEfield);
// ****************************************************************************
      G4double EnergyCalculator::CHC_Esr  =0.2*CLHEP::mm; //[mm]
      G4int    EnergyCalculator::CHCIprint=0;
      G4double EnergyCalculator::CHCEbad  =0.;
      G4double EnergyCalculator::CHCEtotal=0.;
      G4double EnergyCalculator::CHCStotal=0.;
// ****************************************************************************

const G4double EnergyCalculator::ColdCorrection         =  1.0036256;
const G4double EnergyCalculator::LongBarThickness       = 20.*CLHEP::mm;
const G4double EnergyCalculator::StripWidth             =  3.*CLHEP::mm/ColdCorrection;
const G4double EnergyCalculator::KapGap                 =  1.*CLHEP::mm/ColdCorrection;
const G4double EnergyCalculator::EdgeWidth              =  1.*CLHEP::mm;
const G4double EnergyCalculator::DistOfEndofCuFromBack  = 22.77*CLHEP::mm/ColdCorrection;
const G4double EnergyCalculator::DistOfStartofCuFromBack= 31.*CLHEP::mm; // frontface of the barrette
const G4double EnergyCalculator::ZmaxOfSignal           = DistOfStartofCuFromBack
                                                                - DistOfEndofCuFromBack + EdgeWidth;
G4double EnergyCalculator::RefzDist           = 0.;
G4bool EnergyCalculator::SetConstOuterBarrett = false;
G4bool EnergyCalculator::SetConstInnerBarrett = false;

const G4double EnergyCalculator::S3_Etalim[21]={
   1.50, 1.55, 1.60, 1.65, 1.70, 1.75, 1.80, 1.85, 1.90, 1.95,
   2.00, 2.05, 2.10, 2.15, 2.20, 2.25, 2.30, 2.35, 2.40, 2.45, 2.5
};
const G4double EnergyCalculator::Rmeas_outer[50]={
  11.59, 25.22, 57.28, 71.30, 85.90, 98.94, 103.09, 116.68, 130.42, 146.27,
 147.19, 11.59, 15.,   56.91, 44.37, 15.13,  14.93,  45.87,  35.03,  15.40,
  14.04, 39.67, 26.83, 15.64, 14.90, 30.26,  14.70,  29.09,  43.12,  34.51,
  25.08, 11.88, 14.39, 19.54, 17.80, 12.70,  15.31,  13.96,  11.79, -99.,
  23.57, 34.64, 55.32, 65.39, 76.34, 10.83,  94.84,  98.00, -99.,   -99.
};
const G4double EnergyCalculator::Zmeas_outer[2]={3.81, 7.81};

      G4double EnergyCalculator::S3_Rlim[21];
      G4double EnergyCalculator::rlim[50];
      G4double EnergyCalculator::zlim[4];
      G4int    EnergyCalculator::ModuleNumber;
      G4int    EnergyCalculator::PhiDivNumber;
      G4double EnergyCalculator::PhiStartOfPhiDiv;

LArG4::VCalibrationCalculator* EnergyCalculator::m_supportCalculator=0;

// ****************************************************************************

static const G4String
	ECorr_t_option = "type",
	CHC_Map_option = "chcmap",
	CHC_Esr_option = "chcesr",
	GApower_option = "gapower";

static G4double zsep12[44]; // used as const after initialization
static G4double ziw[7];     // used as const after initialization
static G4double zsep23[22]; // used as const after initialization
// ****************************************************************************

#if 0
s tatic G4String extract_option(G4String &options, G4String option) // Depracated? DM, 29 Jul 2015
// ****************************************************************************
{
	size_t l = options.length();
	size_t p1 = options.find(option);
	if(p1 < l){
		size_t p2 = options.find("=", p1);
		p2 ++;
		if(p2 < l){
			size_t p3 = options.find(" ", p2);
			if(p3 > l) p3 = l;
			return options.substr(p2, p3 - p2);
		}
	}
	return "";
}
#endif
// ****************************************************************************
static inline G4double DistanceToEtaLine(const G4ThreeVector &p, G4double eta) {
	G4double sinTheta = 2. * exp(-eta) / (1. + exp(2. * -eta));
	G4double cosTheta = (1. - exp(2. * -eta)) / (1. + exp(2. * -eta));
	return p.perp() * cosTheta - p.z() * sinTheta;
}

// ****************************************************************************
//void EnergyCalculator::vector_to_msg(G4ThreeVector &v) const
// ****************************************************************************
//{
//	(*m_msg) << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
//}


// ****************************************************************************
G4bool EnergyCalculator::Process(const G4Step* step, std::vector<LArHitData>& hdata){
// ****************************************************************************

  // make sure hdata is reset
  hdata.resize(1); 
  return (this->*Process_type)(step, hdata);
}
// ****************************************************************************

// ****************************************************************************
G4bool EnergyCalculator::FindIdentifier(
        const G4Step* step,
        std::vector<LArHitData>& hdata,
	G4ThreeVector &startPointLocal,
	G4ThreeVector &endPointLocal
	){
// ****************************************************************************

  return (this->*FindIdentifier_type) (step,hdata,startPointLocal,endPointLocal);
}
// ****************************************************************************

EnergyCalculator::~EnergyCalculator() {
  if(birksLaw) {
	delete birksLaw;
	birksLaw = 0;
  }
  delete m_lwc;
  m_lwc = 0;
}

// ****************************************************************************
G4bool EnergyCalculator::Process_Default(const G4Step* step, std::vector<LArHitData>& hdata)
// ****************************************************************************
{
	G4ThreeVector startPointinLocal, endPointinLocal;
	if(!FindIdentifier_Default(step, hdata, startPointinLocal, endPointinLocal)){
		return false;
	}

	G4double E = step->GetTotalEnergyDeposit();
	if (birksLaw) {
		const G4ThreeVector midpoint = (startPointinLocal + endPointinLocal) * 0.5;
		const G4double wholeStepLengthCm = step->GetStepLength() / CLHEP::cm;
	 	const G4double gap = (this->*GetGapSize_type)(midpoint);
		const G4double efield = 0.01 * (this->*GetHV_Value_type)(midpoint) / gap; // estimate Efield[KV/cm]
		E = (*birksLaw)(E, wholeStepLengthCm,efield);
	}

	hdata[0].energy = (this->*ecorr_method)(E, startPointinLocal, endPointinLocal);
	return true;
}

// ****************************************************************************
G4bool EnergyCalculator::Process_Barrett(const G4Step* step, std::vector<LArHitData>& hdata){
// ****************************************************************************
	G4ThreeVector startPointinLocal, endPointinLocal;
	if(!FindIdentifier_Barrett(step, hdata, startPointinLocal, endPointinLocal)){
	                                        //cell id is not found
	  if(!lwc()->GetisBarretteCalib()) return false; // ==> normal calculator return false
	  else{ // if Calibration Calculator for Barrett:
                // compute DM identifier;
     	        // In calibration calculator mode this process is used as "geometry calculator".
	        // We have to be prepared to return
	        // either the emec cell id or , if the hit does not belong to any cell,
                // the DeadMatter (DM) id.
	        // DM identifier for Barrett is defined
	        // by the EmecSupportCalibrationCalculator.( In general it is
	        // activated  by the atlas_calo.py)
	       const G4bool status=FindDMIdentifier_Barrett(step, hdata);
	       if(!status)
	         std::cout<<" WARNING!!EnergyCalculator::Process_Barrett:"
			  << " DM identifier not found????"
			  <<std::endl;
	       return status;
	  } // end of calibr. calculator
    }  // end if cell id not found

	// compute signal in 'normal' calculator mode, if cellid is found
	G4double E = step->GetTotalEnergyDeposit();
	if (birksLaw){
		const G4ThreeVector midpoint = (startPointinLocal + endPointinLocal) * 0.5;
		const G4double wholeStepLengthCm = step->GetStepLength() / CLHEP::cm;
	 	const G4double gap = (this->*GetGapSize_type)(midpoint);
		const G4double efield = 0.01 * (this->*GetHV_Value_type)(midpoint) / gap; // estimate Efield[KV/cm]
		E = (*birksLaw)(E, wholeStepLengthCm,efield);
	}

	hdata[0].energy = (this->*ecorr_method)(E, startPointinLocal, endPointinLocal);
	return true;
}
// ****************************************************************************
EnergyCalculator::EnergyCalculator(
	LArWheelCalculator::LArWheelCalculator_t solidtype,
	EnergyCorrection_t corr,
	G4int zside
	) : 
     m_correction_type(corr),
	    PhiGapNumber(0),
	    PhiHalfGapNumber(0),
	    HalfWaveNumber(0),
	    SignofZinHalfWave(0),
	    SignofSlopeofHalfWave(0),
	    SinPhiGap(0),
	    CosPhiGap(0),
	    ZinHalfWave(0),
	    HalfEleThickness(0),
	    ChCollWheelType(0),
	    ChCollFoldType(0),
	    PointFoldMapArea(0),
	    calculatorPhiGap(0),
	    chcollPhiGap(0),
    birksLaw(0),
    m_lwc(new LArWheelCalculator(solidtype, zside) ),
	m_DB_HV(false)
// ****************************************************************************
{
	ISvcLocator* svcLocator = Gaudi::svcLocator();
        IMessageSvc* msgSvc;
	StatusCode status = svcLocator->service("MessageSvc", msgSvc);
	if(!status.isFailure()){
		m_msg = new MsgStream(msgSvc, "LArG4::EC::EnergyCalculator");
	} else {
		throw std::runtime_error("EnergyCalculator constructor: cannot initialze message service");
	}
	(*m_msg) << MSG::DEBUG << "constructor started" << endreq;

	if(lwc()->GetisElectrode()){
		(*m_msg) << MSG::FATAL
		         << "energy caclculator must be of 'absorber' type,"
		         << " while 'electrode' type is requested."
		         << endreq;
		throw std::runtime_error("wrong type for EnergyCalculator");
	}

	RefzDist = lwc()->GetElecFocaltoWRP() +
	           lwc()->GetdWRPtoFrontFace() + lwc()->GetWheelThickness() +
	           lwc()->GetdWRPtoFrontFace() + LongBarThickness - DistOfEndofCuFromBack;

	StoreGateSvc* detStore;
	LArG4EMECOptions   *emecOptions;
	LArG4GlobalOptions *globalOptions;
	status = svcLocator->service("DetectorStore", detStore);

	if(status.isSuccess()){
	  status = detStore->retrieve(emecOptions, "LArG4EMECOptions");
	  if(status.isFailure()){
	    throw std::runtime_error("EnergyCalculator: cannot retrieve LArG4EMECOptions");
	  }
	  status = detStore->retrieve(globalOptions, "LArG4GlobalOptions");
	  if(status.isFailure()){
	    throw std::runtime_error("EnergyCalculator: cannot retrieve LArG4GlobalOptions");
	  }
	} else {
	  throw std::runtime_error("EnergyCalculator: cannot initialize StoreGate interface");
	}

	(*m_msg) << MSG::DEBUG << "StoreGate interface is ready" << endreq;

	//m_identifier = LArG4Identifier();
	m_compartment = 0;
	//m_time = 0.;
	//m_energy = 0.;
	m_isInTime = false;

	m_OOTcut = globalOptions->OutOfTimeCut();

	if(emecOptions->EMECBirksLaw()){
		const double Birks_LAr_density = 1.396;
		const double Birks_k = emecOptions->EMECBirksk();
		birksLaw = new LArG4BirksLaw(Birks_LAr_density,Birks_k);
	}

	m_DB_HV = emecOptions->EMECHVEnable();

	// Access the GeoModelSvc:
	IGeoModelSvc *geoModel;
	status = svcLocator->service ("GeoModelSvc",geoModel);
	if (status != StatusCode::SUCCESS) {
	  throw std::runtime_error ("Cannot locate GeoModelSvc!!");
	}

	// Access the geometry database:
	IRDBAccessSvc *pAccessSvc;
	status=svcLocator->service("RDBAccessSvc",pAccessSvc);
	if (status != StatusCode::SUCCESS) {
	  throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
	}

	// Obtain the geometry version information:

	DecodeVersionKey larVersionKey(geoModel, "LAr");

	pAccessSvc->connect();

	const IRDBRecordset *emecSamplingSep =
		pAccessSvc->getRecordset("EmecSamplingSep", larVersionKey.tag(), larVersionKey.node());
	if (emecSamplingSep->size()==0) {
	  throw std::runtime_error("Cannot find the EmecSamplingSep Table");
	}

	const IRDBRecordset *emecGeometry =
		pAccessSvc->getRecordset("EmecGeometry", larVersionKey.tag(), larVersionKey.node());
	if (emecGeometry->size()==0) {
	  throw std::runtime_error("Cannot find the EmecGeometry Table");
	}

	pAccessSvc->disconnect();

	for(int i = 0; i < 7; i ++){
	  std::ostringstream A0STR;
	  A0STR << "_" << i;
	  const std::string A0 = A0STR.str();
	  const std::string colName = "ZIW" + A0;
	  ziw[i] = (*emecSamplingSep)[0]->getDouble(colName)*CLHEP::cm;
	  //     cout<<"i,ziw= "<<i<<" "<<ziw[i]<<endl;
	}
	for(int i = 0; i < 44; i ++){
	  std::ostringstream A0STR;
	  A0STR << "_" << i;
	  const std::string A0 = A0STR.str();
	  const std::string colName = "ZSEP12" + A0;
	  zsep12[i] = (*emecSamplingSep)[0]->getDouble(colName)*CLHEP::cm;
	  //     cout<<"i,zsep12= "<<i<<" "<<zsep12[i]<<endl;
	}
	for(int i = 0; i < 22; i ++){
	  std::ostringstream A0STR;
	  A0STR << "_" << i;
	  const std::string A0 = A0STR.str();
	  const std::string colName = "ZSEP23" + A0;
	  zsep23[i] = (*emecSamplingSep)[0]->getDouble(colName)*CLHEP::cm;
	  //       cout<<"i,zsep23= "<<i<<" "<<zsep23[i]<<endl;
	}

	ElectrodeFanHalfThickness = LArWheelCalculator::GetFanHalfThickness(LArWheelCalculator::InnerElectrodWheel);
	FanEleThickness = ElectrodeFanHalfThickness * 2.;
	FanEleThicknessOld = 0.300*CLHEP::mm;
	FanEleFoldRadiusOld = 3.*CLHEP::mm;
	FanAbsThickness = lwc()->GetFanHalfThickness() * 2.;
	WaveLength = lwc()->GetActiveLength() / lwc()->GetNumberOfWaves();

	assert(fabs((*emecGeometry)[0]->getDouble("GAP0")*CLHEP::cm - AverageGap) < 0.000001);

//Barrette treatment
     if(lwc()->GetisBarrette()){
          if(lwc()->type() == LArWheelCalculator::OuterAbsorberWheel || lwc()->type() == LArWheelCalculator::OuterAbsorberModule) {
			SetConst_OuterBarrett();
		  } else{
           if(lwc()->type() == LArWheelCalculator::InnerAbsorberWheel || lwc()->type() == LArWheelCalculator::InnerAbsorberModule ){

	      (*m_msg) << MSG::FATAL
		           << "EnergyCalculator: "
				   << "Barrett section is not (yet) prepared for "
				   << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())
				   << endreq;
	      G4ExceptionDescription description;
	      description << G4String("Process_Barrett: ") + "Barrett section is not (yet) prepared for solidtype=" +
		LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type());
	      G4Exception("EnergyCalculator", "BarretteSectionNotPrepared", FatalException, description);
	   }
	   else{
	      (*m_msg) << MSG::FATAL
		        	   << "EnergyCalculator: "
				   << "Unknown solidtype:"
				   << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())
				   << endreq;
	      G4ExceptionDescription description;
	      description << G4String("Process_Barrett: ") + "Unknown  solidtype=" +
		          LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type());
	      G4Exception("EnergyCalculator", "UnknownSolidType", FatalException, description);
	   }
	  }

	  Process_type        = &EnergyCalculator::Process_Barrett;
      FindIdentifier_type = &EnergyCalculator::FindIdentifier_Barrett;
      GetHV_Value_type    = &EnergyCalculator::GetHV_Value_Barrett;
      GetGapSize_type     = &EnergyCalculator::GetGapSize_Barrett;
      distance_to_the_nearest_electrode_type =
              &EnergyCalculator::distance_to_the_nearest_electrode_Barrett;

	  if(lwc()->GetisBarretteCalib()) m_supportCalculator= new LArG4::EMECSupportCalibrationCalculator();

	} else {
	  Process_type        = &EnergyCalculator::Process_Default;
      FindIdentifier_type = &EnergyCalculator::FindIdentifier_Default;
      GetHV_Value_type    = &EnergyCalculator::GetHV_Value_Default;
      GetGapSize_type     = &EnergyCalculator::GetGapSize_Default;
      distance_to_the_nearest_electrode_type =
             &EnergyCalculator::distance_to_the_nearest_electrode_Default;
	}

// energy correction type
	if(m_correction_type == EMEC_ECOR_ROPT){
		LArG4EMECOptions::ECORRTYPE ecorr = emecOptions->EMECECorrType();
		switch(ecorr){
		case LArG4EMECOptions::GAPADJ:
			m_correction_type = EMEC_ECOR_GADJ; break;
		case LArG4EMECOptions::CHCOLL:
			m_correction_type = EMEC_ECOR_CHCL; break;
		case LArG4EMECOptions::GAPOLD:
			m_correction_type = EMEC_ECOR_GADJ_OLD; break;
		case LArG4EMECOptions::GAP_E:
			m_correction_type = EMEC_ECOR_GADJ_E; break;
		case LArG4EMECOptions::GAP_S:
			m_correction_type = EMEC_ECOR_GADJ_S; break;
		case LArG4EMECOptions::GAP_SE:
			m_correction_type = EMEC_ECOR_GADJ_SE; break;
		case LArG4EMECOptions::NONE:
			m_correction_type = EMEC_ECOR_OFF; break;
		case LArG4EMECOptions::CHCOLL1:
			m_correction_type = EMEC_ECOR_CHCL1; break;
		default:
			(*m_msg) << MSG::FATAL
			       << "got from StoreGate ECORRTYPE = " << ecorr
				   << ", don't know such correction" << endreq;
			throw std::runtime_error("EnergyCalculator: \
Bad ECORRTYPE");
		}
	}// end for defining m_correction_type

	switch(m_correction_type){
	case EMEC_ECOR_GADJ:
		ecorr_method = &EnergyCalculator::GapAdjustment;
		break;
	case EMEC_ECOR_CHCL:
		ecorr_method = &EnergyCalculator::CalculateChargeCollection;
		break;
	case EMEC_ECOR_GADJ_OLD:
		ecorr_method = &EnergyCalculator::GapAdjustment_old;
		break;
	case EMEC_ECOR_GADJ_E:
		ecorr_method = &EnergyCalculator::GapAdjustment_E;
		break;
	case EMEC_ECOR_GADJ_S:
		ecorr_method = &EnergyCalculator::GapAdjustment_s;
		break;
	case EMEC_ECOR_GADJ_SE:
		ecorr_method = &EnergyCalculator::GapAdjustment__sE;
		break;
	case EMEC_ECOR_OFF:
		ecorr_method = &EnergyCalculator::dummy_correction_method;
		break;
	case EMEC_ECOR_CHCL1:
		if(lwc()->GetisBarrette()){
			ecorr_method = &EnergyCalculator::CalculateChargeCollection;
			(*m_msg) << MSG::INFO << "CHCOLL energy correction is user for Barettes"
			         << " instead of CHCOLL1" << endreq;
		} else {
			ecorr_method = &EnergyCalculator::CalculateChargeCollection1;
		}
		break;
	default:
		(*m_msg) << MSG::FATAL
		       << "EnergyCalculator: unknown correction type "
			   << m_correction_type << endreq;
		G4Exception("EnergyCalculator", "UnknownCorrectionType", FatalException, 
			    "Process_Barrett: unknown correction type");
	}

// get power of gap in signal calculation
	m_GApower = emecOptions->EMECGapPower();

// pick up surface_suppression_range
	CHC_Esr = emecOptions->EMECEsr();

//read HV values of power supplies

        HVMapVersion = emecOptions->EMECHVMap();
        if(HVMapVersion == "v00" || HVMapVersion == "v01") NofPhiSections=32;
        else{
             NofPhiSections=lwc()->GetNumberOfFans();
             for(G4int i=0;i<NofAtlasSide;++i){
	       for(G4int j=0;j<NofEtaSection;++j){
		 for(G4int k=0;k<NofElectrodeSide;++k){
		   HV_Start_phi[i][j][k]=0;
		 }
               }
             }
	}
 	NumberOfElectrodesInPhiSection = lwc()->GetNumberOfFans() / NofPhiSections;

	Ylimits[0]=Ylimits[1]=Ylimits[2]=Ylimits[3]=0;

	if(HVMapRead == false){
	  (*m_msg) << MSG::DEBUG
		 << "EnergyCalculator: getting EMECHVMap version = "
		 << emecOptions->EMECHVMap() << " from StoreGate"
		 << endreq;
	  G4String HVMapPath  		= "LArG4EC";
	  G4String HVMapFileName  	= "HVEMECMap_"+ HVMapVersion + ".dat";
	  G4String HVpartialPath = HVMapPath + "/" + HVMapFileName;
	  G4String HVMapLocation = PathResolver::find_file(HVpartialPath, "ATLASCALDATA");
	  GetHVMap(HVMapLocation.c_str());
	}

	if(m_DB_HV) get_HV_map_from_DB();

// if charge collection is required
   if(m_correction_type == EMEC_ECOR_CHCL ||  m_correction_type ==  EMEC_ECOR_CHCL1){
      if(!lwc()->GetisElectrode()){
// get fieldmap from file

	if(FieldMapsRead == false){
	  ChCollInner.FieldMapPrepared=false;
	  ChCollOuter.FieldMapPrepared=false;
// Determine which version of the file by examining the user option.
	  G4String suffix = emecOptions->EMECChMap();
	  if(suffix.empty()) FieldMapVersion = "v00";
	  else FieldMapVersion = suffix;

	  (*m_msg) << MSG::DEBUG
		 << "EnergyCalculator: field map version = "
		 << FieldMapVersion
                 << endreq;

	  IniGeomforFieldMaps();

// Now we have to get the full path to the file.  In
// stand-alone, we use AFS.  In Athena, we use
// PathResolver.
	   G4String FieldMapFileName      = "ec.fieldz" + suffix;
	   G4String FieldMapPath          = "LArG4EC";
	   G4String partialPath        = FieldMapPath + "/" + FieldMapFileName;
	   G4String FieldMapLocation   = PathResolver::find_file(partialPath, "ATLASCALDATA");

	   GetFieldMaps(FieldMapLocation.c_str());
           FieldMapsRead=true;

	} // finish to read the fieldmap

// prepare the fieldmaps for further calculation

	if(FieldMapVersion != "v00"){
	  FanEleThicknessOld  = FanEleThickness;
	  FanEleFoldRadiusOld = lwc()->GetFanFoldRadius();
	}

	if(lwc()->GetisInner()) ChCollWheelType=&ChCollInner;
	else        ChCollWheelType=&ChCollOuter;
	
	if(ChCollWheelType->FieldMapPrepared == false){
	   PrepareFieldMap();
	   ChCollWheelType->FieldMapPrepared = true;
	}
      }
      else{ //  electrode type
	(*m_msg) << MSG::FATAL
	       << "EnergyCalculator: "
	       << "Charge Collection cannot be prepared for "
	       << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())
	       << endreq;
	G4ExceptionDescription description;
	description << G4String("Process_Barrett: ") +
	  "Charge Collection cannot be prepared for solidtype=" +
	  LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type());
	G4Exception("EnergyCalculator", "IncorrectSolidType", FatalException, description);
      }
   }  // end if chcoll required
   (*m_msg) << MSG::DEBUG
	  << "EnergyCalculator parameters: " << std::endl
	  << "correction type " << m_correction_type << std::endl
	  << "GA power = " << m_GApower << std::endl
	  << "energy suppression range = " << CHC_Esr / CLHEP::mm << " [mm]"
	  << "\n"
      << "default method for computation of folding angle (param) - compiled in"
	  << endreq;
// Aug 2007 AMS, lost Aug 2008, restored May 2009
	m_electrode_calculator = 0;
	if(m_correction_type == EMEC_ECOR_CHCL1 && !lwc()->GetisBarrette()){
		LArWheelCalculator::LArWheelCalculator_t t = LArWheelCalculator::OuterElectrodWheel;
		if(lwc()->GetisModule()){
		  
			if(lwc()->GetisInner()) t = LArWheelCalculator::InnerElectrodModule;
			else t = LArWheelCalculator::OuterElectrodModule;
			
		} else if(lwc()->GetisInner()) t = LArWheelCalculator::InnerElectrodWheel;
		
		m_electrode_calculator = new LArWheelCalculator(t, lwc()->GetAtlasZside());
		if(m_electrode_calculator == 0){
			(*m_msg) << MSG::FATAL
			         << "cannot create helper electrode calculator"
				 << endreq;
			G4Exception("EnergyCalculator", "NoElectrodeCalculator", FatalException, 
				    "Process_Barrett: cannot create helper electrode calculator");
		}
		(*m_msg) << MSG::DEBUG
		       << "helper electrode calculator created ("
			   << LArWheelCalculator::LArWheelCalculatorTypeString(t) << ")"
			   << endreq;
	}
} // end of EnergyCalculator::EnergyCalculator

G4double EnergyCalculator::CalculateChargeCollection1(
	G4double a_energy, const G4ThreeVector &p1, const G4ThreeVector &p2)  // need to make const
{
	if(!lwc()->GetisBarrette()){   // should be wheel
		G4double current1 = GetCurrent1(p1, p2, a_energy);  // need to make const

		CHCEtotal += a_energy;
		CHCStotal += current1;

		if(m_msg->level()==MSG::DEBUG)
		  (*m_msg) << MSG::DEBUG
			   << " chcoll1: edep, current= " << a_energy << " " << current1
			   << " CHCEtotal= " << CHCEtotal << " CHCStotal= " << CHCStotal
			   << " CHCIprint= " << CHCIprint << " CHCEbad= " << CHCEbad
			   << " ebad/etotal= " << CHCEbad / CHCEtotal
			   << endreq;
		return current1;
	}
	return a_energy;
}



// ****************************************************************************
G4double EnergyCalculator::GapAdjustment_old(G4double a_energy,
						const G4ThreeVector& p1, const G4ThreeVector &p2) // need to make const
// ****************************************************************************
{
  //  std::cout<<"*** GapAdjustment_old is called, a_energy="<<a_energy
  //	   <<std::endl;

	const G4ThreeVector p = (p1 + p2) * .5;
	return(a_energy / pow( ((this->*GetGapSize_type)(p)) / AverageGap, 1.3));
}

// ****************************************************************************
G4double EnergyCalculator::GapAdjustment(G4double      a_energy,
                                         const G4ThreeVector& a_startPoint,
                                         const G4ThreeVector& a_endPoint) // need to make const
// ****************************************************************************
{
	static const G4double substpsize = 0.1*CLHEP::mm;
  //  std::cout<<"*** GapAdjustment is called, a_energy="<<a_energy
  //	   <<std::endl;

	const G4ThreeVector step( a_endPoint - a_startPoint );
	const G4int	        nofstep= int(step.mag()/substpsize)+1;
	const G4double		s_energy= a_energy/nofstep;
	G4double	  corr_energy= 0;
	G4ThreeVector vstep;

	for(G4int i = 0; i < nofstep; ++ i){ // loop for substeps
		const G4double ds = (i + 0.5) / nofstep;
	 	vstep = a_startPoint * (1. - ds) + a_endPoint * ds;
		const G4double gap = (this->*GetGapSize_type)(vstep);
		corr_energy += s_energy / pow((gap / AverageGap), m_GApower);
	}
	return corr_energy;
}

// ****************************************************************************
G4double EnergyCalculator::GapAdjustment_E(G4double a_energy,
                                                  const G4ThreeVector& a_startPoint,
                                                  const G4ThreeVector& a_endPoint) // need to make const
// ****************************************************************************
{
	static const G4double substpsize = 0.1*CLHEP::mm;
  //std::cout<<"*** GapAdjustment_E is called, a_energy="<<a_energy
  //	   <<std::endl;

	const G4ThreeVector step( a_endPoint - a_startPoint );
	const G4int nofstep = G4int(step.mag()/substpsize) + 1;
	const G4double s_energy = a_energy / nofstep / AverageCurrent;
	G4double corr_energy = 0;
	G4ThreeVector vstep;

	for(G4int i = 0; i < nofstep; i ++){
		const G4double ds = (i + 0.5) / nofstep;
		vstep = a_startPoint * (1. - ds) + a_endPoint * ds;
		const G4double gap = (this->*GetGapSize_type)(vstep);
		const G4double HV_value = (this->*GetHV_Value_type)(vstep);
		const G4double efield = (HV_value * CLHEP::volt) / (gap * CLHEP::mm) / (CLHEP::kilovolt / CLHEP::cm); // estimate Efield[KV/cm]
		corr_energy += s_energy / /* gap * gap / */ (gap - CHC_Esr)
		             * IonReco(efield) * DriftVelo(LArTemperature_av, efield);
	}
	return corr_energy;
}

// ****************************************************************************
G4double EnergyCalculator::CalculateChargeCollection(
	G4double a_energy,
	const G4ThreeVector &a_startPoint,
	const G4ThreeVector &a_endPoint) // need to make const
// ****************************************************************************
{
  // std::cout<<"*** CalculateChargeCollection is called, a_energy="<<a_energy
  //	   <<std::endl;

  if(m_msg->level()==MSG::DEBUG)
    (*m_msg) << MSG::DEBUG
	     << "starting CalculateChargeCollection:"
	     << "a_energy   = " << a_energy
	     << ", startPoint = " << MSG_VECTOR(a_startPoint)
	     << ", endPoint = " << MSG_VECTOR(a_endPoint)
	     << ", wheel type: "
	     << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())
	     << " isBarrette = " << lwc()->GetisBarrette()
	     << " isBarretteCalib = " << lwc()->GetisBarretteCalib()
	     << endreq;

//	calculatorPhiGap =
//		lwc()->PhiGapNumberForWheel(LArWheelCalculator::GetPhiGap(a_startPoint));

  G4double pStart[3],pEnd[3];
  for(G4int i = 0; i <= 2; ++ i){
    pStart[i]=a_startPoint[i];
    pEnd  [i]=a_endPoint  [i];
  }

  if(!lwc()->GetisBarrette()){   // should be wheel

    assert(pStart[2] >= -0.0001 &&
	     pEnd[2] >= -0.0001 &&
	   pStart[2] <= lwc()->GetWheelThickness()+0.0001   &&
	     pEnd[2] <= lwc()->GetWheelThickness()+0.0001);

    if(pStart[2]<0.) pStart[2]=0.0001;
    if(  pEnd[2]<0.)   pEnd[2]=0.0001;
    if(pStart[2]>lwc()->GetWheelThickness()) pStart[2]=lwc()->GetWheelThickness()-0.0001;
    if(  pEnd[2]>lwc()->GetWheelThickness())   pEnd[2]=lwc()->GetWheelThickness()-0.0001;
  }
  else{               // should be Barrett (only BackOuterBarrett at the moment)

    const double thick=lwc()->GetdWRPtoFrontFace() * 0.5;
    assert(fabs(pStart[2]) <= thick+0.0001 && fabs(pEnd[2]) <= thick+0.0001 );
    if(pStart[2]<-thick)  pStart[2]=-thick+0.0001;
    if(  pEnd[2]<-thick)    pEnd[2]=-thick+0.0001;
    if(pStart[2]> thick)  pStart[2]= thick-0.0001;
    if(  pEnd[2]> thick)    pEnd[2]= thick-0.0001;
  }

  const G4double current = GetCurrent(pStart, pEnd, a_energy);

  CHCEtotal=CHCEtotal+a_energy;
  CHCStotal=CHCStotal+current;

  if(m_msg->level()==MSG::DEBUG)
    (*m_msg) << MSG::DEBUG
//  std::cout
	     <<" isBarrett="<<lwc()->GetisBarrette()<<" isInner="<<lwc()->GetisInner()
	     <<" chcoll: edep, current= "<<a_energy<<" "<<current
	     <<" CHCEtotal= "<<CHCEtotal<<" CHCStotal= "<<CHCStotal
	     <<" CHCIprint= "<<CHCIprint<<" CHCEbad= "<<CHCEbad
	     <<" ebad/etotal= "<<CHCEbad/CHCEtotal
	     <<endreq;
//  <<std::endl;

  return current;
}

// ****************************************************************************
void EnergyCalculator::GetHVMap(const G4String fname){
// ****************************************************************************

	(*m_msg) << MSG::INFO
	       << "HVEMECMap is to be read from file: " << fname << endreq;

	FILE *lun = fopen(fname, "r");
	if(lun == 0){
		(*m_msg) << MSG::ERROR
		       << "GetHVMap - file '" << fname << "' not found!"
			   << std::endl
			   << " Trying to read HVEMECMap from local file:"
			   << "HVEMECMap.dat"
			   << endreq;
		lun = fopen("HVEMECMap.dat", "r");
	}
	if(lun == 0){
		(*m_msg) << MSG::FATAL
			 << "GetHVMap - file "<< "./HVEMECMap.dat not found!"
			 << "Cannot obtain HV map." << endreq;
		G4Exception("EnergyCalculator", "NoHVMap", FatalException, 
			    "GetHVMap: could not read map file");
	}

        char ch[80], ch1[80];

        for(G4int i=0;i<80;i++){ch1[i]=' ';}

	fgets(ch,80,lun);
	(*m_msg) << MSG::INFO << "actual HVMapVersion = " << ch
	       << endreq;
        ch1[0]=ch[9];
        ch1[1]=ch[10];
	if(HVMapVersion == "v02" || HVMapVersion == "v99"){
	  ch1[0]=ch[10];
          ch1[1]=ch[11];
        }
        G4int  iv=atoi(ch1);
        G4bool ok=false;
        if(HVMapVersion == "v00" && iv == 0) ok=true;
        if(HVMapVersion == "v01" && iv == 1) ok=true;
        if(HVMapVersion == "v02" && iv == 2) ok=true;
        if(HVMapVersion == "v99" && iv == 99) ok=true;  // this is a test file

        if(!ok){
          (*m_msg) << MSG::FATAL
		   << "GetHVMap - file does not contain the map requested"<< endreq;
	  G4Exception("EnergyCalculator", "IncorrectVMap", FatalException, 
		      "GetHVMap: incorrect map file");
	}

	if(HVMapVersion == "v00" || HVMapVersion == "v01" ){

	G4String AtlasSide, EtaSection, ElectrodeSide;
	for(G4int i = 0; i < NofAtlasSide; ++ i){
                if (fscanf(lun, "%79s", ch) < 1) {
                  (*m_msg) << MSG::ERROR << "GetHVMap: Error reading map file"
                           << endreq;
                }
		AtlasSide = ch;
		(*m_msg) << MSG::DEBUG << "AtlasSide = " << AtlasSide << endreq;
		for(G4int j = 0; j < NofEtaSection; ++ j){
			for(G4int k = 0; k < NofElectrodeSide; ++ k){
                                if (fscanf(lun, "%79s%79s", ch, ch1) < 2) {
                                  (*m_msg) << MSG::ERROR << "GetHVMap: Error reading map file"
                                           << endreq;
                                }
				EtaSection   =ch;
				ElectrodeSide=ch1;
				(*m_msg) << MSG::DEBUG << "EtaSection = " << EtaSection
				       << " ElectrodeSide = " << ElectrodeSide
				       << endreq;

				if (fscanf(lun, "%i", &HV_Start_phi[i][j][k]) < 1) {
                                  (*m_msg) << MSG::ERROR << "GetHVMap: Error reading map file"
                                           << endreq;
                                }
				(*m_msg) << MSG::DEBUG << "i, j, k = " << i << ", " << j << ", " << k
				       << " " <<" HV_Start_phi = " << HV_Start_phi[i][j][k]
				       << endreq;

				for(G4int l = 0; l < NofPhiSections; ++ l){
                                        if (fscanf(lun, "%lg", &HV_Values[i][j][k][l]) < 1) {
                                          (*m_msg) << MSG::ERROR << "GetHVMap: Error reading map file"
                                                   << endreq;
                                        }
					if(l == 0){
						(*m_msg) << MSG::DEBUG << " HV_Values = " << HV_Values[i][j][k][l]
						       << endreq;
					}
				}
			}
		}
	}
	(*m_msg) << MSG::DEBUG << endreq;
	}

       if(HVMapVersion == "v02" || HVMapVersion == "v99") {

       G4int iprmx,i,j,k,l,electrodenumber;
       char s[200];

       iprmx=3;
       //iprmx=3*256;
       for(i=0;i<NofAtlasSide;++i){       //loop for Atlas side
         for(j=0;j<3;++j){                // read header lines
           fgets(s,200,lun);
           printf("%s",s);
	 }
          for(j=0; j<NofElectrodesOut; ++j){        //loop for electrodes
             if (fscanf(lun,"%i",&electrodenumber) < 1) {
               (*m_msg) << MSG::ERROR << "GetHVMap: Error reading map file"
                        << endreq;
             }
             if(j<iprmx || j==NofElectrodesOut-1)  printf("%3i",electrodenumber);
             if(j==iprmx) printf("...\n");
             for(k=0;k<NofEtaSection;++k){          //loop for etasection
                for(l=0;l<NofElectrodeSide;++l){    //loop for side
                    if (fscanf(lun,"%lg", &HV_Values[i][k][l][j]) < 1) {
                      (*m_msg) << MSG::ERROR << "GetHVMap: Error reading map file"
                               << endreq;
                    }
                    if(j<iprmx || j==NofElectrodesOut-1) printf("%8.2f",HV_Values[i][k][l][j]);
		}
	     }
             if(j<iprmx || j==NofElectrodesOut-1) printf("\n");
	  }
          fgets(s,200,lun);
       }
       }
       fclose(lun);
       HVMapRead = true;
}

void EnergyCalculator::get_HV_map_from_DB(void)
{
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	StoreGateSvc* pDetStore;

	StatusCode status = svcLocator->service("DetectorStore", pDetStore);
	if(status.isFailure()){
		(*m_msg) << MSG::WARNING
		         << "unable to get Detector Store! Use default HV values"
			     << endreq;
		return;
	}

  // get EMECHV Manager
	const LArHVManager *manager = 0;
	if(pDetStore->retrieve(manager) == StatusCode::SUCCESS){
		const EMECHVManager* hvManager =
			manager->getEMECHVManager(lwc()->GetisInner()? EMECHVModule::INNER: EMECHVModule::OUTER);
		(*m_msg) << MSG::INFO << "got HV Manager for "
		         << (lwc()->GetisInner()? "inner": "outer") << " wheel" << endreq;
		const EMECHVDescriptor* dsc = hvManager->getDescriptor();
		unsigned int counter = 0;
      // loop over HV modules
		for(unsigned int iSide = hvManager->beginSideIndex();
			iSide < hvManager->endSideIndex(); ++ iSide
		){
			unsigned short jSide = 1 - iSide; // local numbering is inverse
			for(unsigned int iEta = hvManager->beginEtaIndex();
				iEta < hvManager->endEtaIndex(); ++ iEta
			){
				unsigned int jEta = iEta;
				if(lwc()->GetisInner()) jEta += 7;
				for(unsigned int iPhi = hvManager->beginPhiIndex();
					iPhi < hvManager->endPhiIndex(); ++ iPhi
				){
					for(unsigned int iSector = hvManager->beginSectorIndex();
						iSector < hvManager->endSectorIndex(); ++ iSector
					){
						EMECHVModuleConstLink hvMod =
							hvManager->getHVModule(iSide, iEta, iPhi, iSector);
						unsigned int nElec = hvMod->getNumElectrodes();
						for(unsigned int iElec = 0; iElec < nElec; ++ iElec){
							EMECHVElectrodeConstLink electrode =
								hvMod->getElectrode(iElec);
							unsigned int jElec = iElec;
							jElec += iSector*nElec;
							jElec += iPhi*nElec*dsc->getSectorBinning().getNumDivisions();
							if(jSide == 1){
								jElec = lwc()->GetNumberOfFans() + lwc()->GetNumberOfFans() / 2 - jElec;
								if(jElec >= (unsigned int)lwc()->GetNumberOfFans()) jElec -= lwc()->GetNumberOfFans();
							}
							for(unsigned int iGap = 0; iGap < 2; ++ iGap){
								double hv = electrode->voltage(iGap);
								if(m_msg->level()==MSG::DEBUG)
								  (*m_msg) << MSG::DEBUG
									   << "Side, Eta, Elec, Gap, hv "
									   << jSide << " " << jEta << " "
									   << jElec << " " << iGap << " "
									   << HV_Values[jSide][jEta][iGap][jElec]
									   << " -> " << hv << endreq;
								if(fabs((HV_Values[jSide][jEta][iGap][jElec] - hv)/HV_Values[jSide][jEta][iGap][jElec]) > 0.05){
									(*m_msg) << MSG::INFO
											<< "eta: " << dsc->getEtaBinning().binCenter(iEta) * (jSide == 0? 1: -1) << " "
											<< "phi: " << dsc->getPhiBinning().binCenter(iPhi) << " "
											<< "ele phi: " << electrode->getPhi()
											<< " side " << iGap
											<< " change HV from "
											<< HV_Values[jSide][jEta][iGap][jElec]
											<< " to " << hv << endreq;
								}
								if(hv > -999.){
									HV_Values[jSide][jEta][iGap][jElec] = hv;
									++ counter;
								}
							}
						}
					}
				}
			}
		}
		(*m_msg) << MSG::INFO << counter
		         << " HV values updated from DB" << endreq;
	} else {
		(*m_msg) << MSG::WARNING << "Unable to find HV Manager" << endreq;
	}
}
// ****************************************************************************
G4double EnergyCalculator::GetHV_Value(const G4ThreeVector& p) const
// ****************************************************************************
{
// pickup HV value from the data of power supplies;
// everything positioned in the Wheel's coord.system;
// if it is not the same as the Atlas's one, adjustment is needed
// either in this code or in the data file

	const G4int atlasside = (lwc()->GetAtlasZside() > 0) ? 0 : 1;

	const G4ThreeVector pforeta(p.x(), p.y(), p.z() + lwc()->GetElecFocaltoWRP() + lwc()->GetdWRPtoFrontFace());
	const G4double mideta = pforeta.pseudoRapidity();
	G4int etasection = -1;
	for(G4int i = 1; i <= NofEtaSection; ++ i){
	  if(mideta <= HV_Etalim[i]){
	    etasection = i - 1;
	    break;
	  }
	}
	if(!(etasection>=0 && etasection <=NofEtaSection-1)) throw std::runtime_error("Index out of range");	

	//assert(etasection >= 0 && etasection <= NofEtaSection - 1);

	const std::pair<G4int, G4int> gap = lwc()->GetPhiGapAndSide(p);
	G4int electrodeside = 0;  //left side of electrode(small phi)
	if(gap.second > 0) electrodeside = 1;

	const G4int electrodenumber = lwc()->PhiGapNumberForWheel(gap.first);
	const G4int firstelectrode  = HV_Start_phi[atlasside][etasection][electrodeside];
	G4int electrodeindex  = electrodenumber-firstelectrode;
	if(electrodeindex < 0) electrodeindex += lwc()->GetNumberOfFans();

	const G4int phisection      = electrodeindex / NumberOfElectrodesInPhiSection;
	assert(phisection>=0 && phisection<=NofPhiSections-1);

	G4double HV_value= HV_Values[atlasside][etasection][electrodeside][phisection];

	if(m_msg->level()==MSG::DEBUG)
//	std::cout
	  (*m_msg) << MSG::DEBUG
		   <<"***GetHV::="<<HV_value<<" Asde="<<atlasside<<" eta="<<etasection
		   <<" Esde="<<electrodeside
		   <<" fi="<<phisection<<" xyz="<<p.x()<<" "<<p.y()<<" "<<p.z()
		   <<" igap.first="<<lwc()->PhiGapNumberForWheel(gap.first)
		   <<" gap.second="<<gap.second
		   <<endreq;

	return HV_value;
}

/*
In this type of correction energy deposited close than CHC_Esr to an electrod
is suppressed.
*/
// ****************************************************************************
G4double EnergyCalculator::GapAdjustment_s(G4double      a_energy,
                                         const G4ThreeVector& a_startPoint,
                                         const G4ThreeVector& a_endPoint) // need to make const
// ****************************************************************************

{
	static const G4double substpsize = 0.1*CLHEP::mm;
  //  std::cout<<"*** GapAdjustment_s is called, a_energy="<<a_energy
  //	   <<std::endl;

	const G4ThreeVector step( a_endPoint - a_startPoint );
	const G4int   nofstep= int(step.mag()/substpsize)+1;
	const G4double	  s_energy= a_energy/nofstep;
	G4double	  corr_energy= 0.;
	G4ThreeVector vstep, tmp;

	for(G4int i = 0; i < nofstep; ++ i){ // loop for substeps
		const G4double ds = (i + 0.5) / nofstep;
		vstep = a_startPoint * (1. - ds) + a_endPoint * ds;
		tmp = vstep;
		const G4double dte = (this->*distance_to_the_nearest_electrode_type)(tmp);
		if(fabs(dte) < CHC_Esr) continue;
		const G4double gap = (this->*GetGapSize_type)(vstep);
		corr_energy += s_energy / pow((gap / AverageGap), m_GApower)
		               * gap / (gap - CHC_Esr);
	}
	return corr_energy;
}
// ****************************************************************************
G4double EnergyCalculator::GapAdjustment__sE(G4double a_energy,
                                                  const G4ThreeVector& a_startPoint,
                                                  const G4ThreeVector& a_endPoint) // need to make const
// ****************************************************************************
{
	static const G4double substpsize = 0.1*CLHEP::mm;
  //  std::cout<<"*** GapAdjustment__sE is called, a_energy="<<a_energy
  //	   <<std::endl;

	const G4ThreeVector step( a_endPoint - a_startPoint );
	const G4int nofstep = G4int(step.mag()/substpsize) + 1;
	const G4double s_energy = a_energy / nofstep;
	G4double corr_energy = 0;
	G4ThreeVector vstep, tmp;
	
	for(G4int i = 0; i < nofstep; i ++){
		const G4double ds = (i + 0.5) / nofstep;
		vstep = a_startPoint * (1. - ds) + a_endPoint * ds;
		tmp = vstep;
		const G4double dte = (this->*distance_to_the_nearest_electrode_type)(tmp);
		if(fabs(dte) < CHC_Esr) continue;
		const G4double gap = (this->*GetGapSize_type)(vstep);
		const G4double efield = 0.01 * (this->*GetHV_Value_type)(vstep) / gap; // estimate Efield[KV/cm]
		corr_energy += s_energy / AverageCurrent / gap *
		               IonReco(efield) * DriftVelo(LArTemperature_av, efield)
		               * gap / (gap - CHC_Esr);
	}
	return corr_energy;
}
// ****************************************************************************
// The static arrays that describe the endcap geometry.  Mostly these
// were copied from the ATLAS G3 code.

// The values of eta, phi, and z determine which "compartment" (a
// sampling and region) the point is in.  The following structure and
// table define constants for each compartment.

// Someday, the following table should be determined from the detector
// database and not hard-coded.

typedef struct {
  G4int    zSide;      // +- 3 for inner wheel, +- 2 for outer wheel, z determines sign
  G4int    sampling;
  G4int    region;
  G4double etaScale;   // 1/deta
  G4double etaOffset;  // set so that the range of etaBin starts at zero for each compartment
  G4int    maxEta;     // the maximum value of etaBin in this compartment
  G4int    gapsPerBin; // number of phi gaps (in LArWheelSolid) for each cell bin.
  G4int    maxPhi;     // the maximum value of phiBin in this compartment
} geometry_t;

static const geometry_t geometry[] =
// zSide sampling region etaScale etaOffset maxEta gapsPerBin maxPhi
{ {   3,      1,      0,      10,     25  ,     6,       4,     63 },
  {   3,      2,      0,      10,     25  ,     6,       4,     63 },
  {   2,      1,      5,      40,     96  ,     3,      12,     63 },
  {   2,      1,      4,     160,    320  ,    63,      12,     63 },
  {   2,      1,      3,     240,    432  ,    47,      12,     63 },
  {   2,      1,      2,     320,    480  ,    95,      12,     63 },
  {   2,      1,      1,      40,     57  ,     2,      12,     63 },
  {   2,      2,      1,      40,     57  ,    42,       3,    255 },
  {   2,      3,      0,      20,     30  ,    19,       3,    255 },
  // These last two compartments did not exist separately
  // in the G3 study.  Yes, they have only one bin in eta.
  {   2,      1,      0,      20,     27.5,     0,      12,     63 },
  {   2,      2,      0,      20,     27.5,     0,       3,    255 } };

// ****************************************************************************
G4bool EnergyCalculator::FindIdentifier_Default(
	const G4Step* step,
        std::vector<LArHitData>& hdata,
	G4ThreeVector &startPointLocal,
	G4ThreeVector &endPointLocal
)  // need to make const. return m_identifier, not modify
// ****************************************************************************
{
	G4bool validhit = true;

	const G4StepPoint* pre_step_point = step->GetPreStepPoint();
	const G4StepPoint* post_step_point = step->GetPostStepPoint();

	const G4ThreeVector startPoint = pre_step_point->GetPosition();
	const G4ThreeVector endPoint   = post_step_point->GetPosition();
//	p = (startPoint + endPoint) * 0.5;
	const G4ThreeVector p = startPoint;  // middle point may be out of volume

	const G4AffineTransform transformation =
		pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();

	startPointLocal = transformation.TransformPoint(startPoint);
	endPointLocal = transformation.TransformPoint(endPoint);
//	pinLocal = (startPointLocal + endPointLocal) * 0.5;
	const G4ThreeVector pinLocal = startPointLocal;  // middle point may be out of volume

	const G4ThreeVector pforcell(pinLocal.x(), pinLocal.y() ,
	                       pinLocal.z() + lwc()->GetElecFocaltoWRP() + lwc()->GetdWRPtoFrontFace());

	const G4double eta = pforcell.pseudoRapidity();
	const G4double   z = pforcell.z();
	G4double phi = pforcell.phi();
	if(phi < 0.) phi += CLHEP::twopi;

	m_compartment = 0;

	if(lwc()->GetisInner())
	{
		G4int ipad = G4int((eta - 2.5) / 0.1);
		if(ipad > 6 || ipad < 0){
			(*m_msg) << MSG::WARNING
			       << "FindIdentifier: invalid hit, eta = "
			       << eta << ", ipad=" << ipad << endreq;
                        validhit=false;
                        if (ipad<0) ipad=0;
                        if (ipad>6) ipad=6;
		}
		if(z < ziw[ipad]) m_compartment = 1;
		else              m_compartment = 2;
	} else { // !isInner
		G4int ipad = G4int((eta - 1.4) / 0.025);
		if(ipad < 0) ipad = 0;   // first electrode starts below eta=1.4
		if(ipad > 43){
			(*m_msg) << MSG::WARNING
			       << "FindIdentifier: invalid hit, eta = "
			       << eta << ", ipad = " << ipad << endreq;
                        validhit=false;
                        ipad=43;
		}
		if(z < zsep12[ipad]){
			if     (eta > 2.4)   m_compartment = 3;
			else if(eta > 2.0)   m_compartment = 4;
			else if(eta > 1.8)   m_compartment = 5;
			else if(eta > 1.5)   m_compartment = 6;
			else if(eta > 1.425) m_compartment = 7;
			else                 m_compartment = 10;
		} else {
			if(z < zsep23[ipad/2]){
				if(eta > 1.425) m_compartment = 8;
				else            m_compartment = 11;
			} else                  m_compartment = 9;
		}
	}
// if m_compartment==0, then none of the above "if's" were resolved.

	assert(m_compartment > 0);

	G4int c = m_compartment - 1;

  // There were some differences between the definition of a
  // "compartment" in the G3 study and the assignment of
  // sampling/region in "atlnot/Communication/com-soft-2001-002".  The
  // following "if"s took compartments that did not exist separately
  // in the G3 study and combined them.  The following statements
  // are now obsolete and are commented out, but they're left in place
  // in case we have to do a study with the old G3 numbers.

  // if ( m_compartment == 10 ) m_compartment = 7;
  // if ( m_compartment == 11 ) m_compartment = 8;


// 19-04-2007 AMS    use constant m_AtlasZside obtained in constructor
  // zSide is negative if z<0.
//	m_AtlasZside = geometry[c].zSide;
//	if(p.z() < 0.) m_AtlasZside = -m_AtlasZside;
//	G4int atlasside = m_AtlasZside;
	const G4int atlasside = lwc()->GetAtlasZside() * geometry[c].zSide;

	G4int sampling = geometry[c].sampling;
	G4int region   = geometry[c].region;

	G4int etaBin = G4int(eta * geometry[c].etaScale - geometry[c].etaOffset);

	if(etaBin < 0 || etaBin > geometry[c].maxEta) {
         validhit=false;
         if (etaBin<0) etaBin=0;
         if (etaBin>geometry[c].maxEta) etaBin=geometry[c].maxEta;
    }

// ===============
// Treatment of the constraints on the electrodes' geometry,
// and HV buses (J.T. 10.2004)
// Does not work for old G3 numbering scheme!!

  // overall geom param.:

//   G4double WheelThickness    =  514.*CLHEP::mm;
   static const G4double rOuterCutoff      = 2034.*CLHEP::mm;
   static const G4double gapBetweenWheels  =    3.*CLHEP::mm;
   static const G4double eta_hi = 3.2;
   static const G4double eta_mid= 2.5;
   static const G4double eta_low= 1.375;

  // specific param of electrode design;

   static const G4double WidthofHVBus           = 2.5*CLHEP::mm;
   static const G4double WidthofKaptonGap       = 1.0*CLHEP::mm;
   static const G4double WidthofKaptonGapinS1   = 0.5*CLHEP::mm;
   static const G4double WidthofKaptonGapatEdge = 2.*CLHEP::mm;
   static const G4double zStartofIW   =   4.*CLHEP::mm;   // what follows measured by hand
   static const G4double zStartofC7   =   8.4*CLHEP::mm;
   static const G4double zStartofC3   =   7.8*CLHEP::mm;
   static const G4double  zEndofC8e2  =   4.5*CLHEP::mm;
   static const G4double  zEndofC9e19 =  30.*CLHEP::mm;
   static const G4double   r0aofC9e18 = 743.1*CLHEP::mm;
   static const G4double   r0cofC9e18 = 751.1*CLHEP::mm;
   static const G4double  zSepofC9e18 =  14.*CLHEP::mm;
   static const G4double    txofC9e18 =   0.48;

   static const G4double DistMax              = WidthofHVBus+WidthofKaptonGap;
   static const G4double DistMaxS1            = WidthofHVBus+WidthofKaptonGapinS1;
   static const G4double DistMaxatEdgeinCrack = gapBetweenWheels/2.+WidthofKaptonGapatEdge+
                                   WidthofHVBus+WidthofKaptonGap/2.;
   static const G4double DistMinatEdgeinCrack =(gapBetweenWheels+WidthofKaptonGapatEdge)/2.;

   G4double dist,dist_min,dist_max,eta_min,eta_max;

  // check against the edges of the electrodes: hits are deleted if the
  // distance of the hit to the Cu signal layer on the (r,z) plane
  // is bigger than 1mm.

   switch(m_compartment){

//compartment=1:inner wheel S1

   case 1:
     switch(etaBin){
     case 0:
        if(pinLocal.z() < zStartofIW ) {
           validhit=false;
         }
        if (-DistanceToEtaLine( pforcell, eta_mid) < DistMinatEdgeinCrack) {
           validhit=false;
         }
        break;
     case 4:
     case 5:
        if(pinLocal.z() < zStartofIW ) {
          validhit=false;
        }
	break;
     case 6:
        if(pinLocal.z() < zStartofIW )  {
           validhit=false;
        }
        if (DistanceToEtaLine( pforcell, eta_hi) < WidthofKaptonGapatEdge * 0.5) {
           validhit=false;
         }
       break;
     }
   break;

//compartment=2:inner wheel S2

   case 2:
     switch(etaBin){
     case 0:
        if(-DistanceToEtaLine( pforcell, eta_mid) < DistMinatEdgeinCrack) {
           validhit=false;
         }
        break;
     case 6:
        if (DistanceToEtaLine( pforcell, eta_hi) < WidthofKaptonGapatEdge * 0.5) {
           validhit=false;
         }
       break;
     }
   break;

//compartment=3:outer wheel S1 eta 2.4

   case 3:
     switch(etaBin){
     case 0:
     case 1:
     case 2:
        if(pinLocal.z() < zStartofC3 ) {
           validhit=false;
         }
        break;
     case 3:
        if(pinLocal.z() < zStartofC3 ) {
           validhit=false;
         }

        if(DistanceToEtaLine( pforcell, eta_mid) < DistMinatEdgeinCrack) {
           validhit=false;
         }
        break;
     }
     break;

//compartment=7:outer wheel S1 eta 1.425

   case 7:
     if(pinLocal.z()<zStartofC7) {
           validhit=false;
         }

     break;

//compartment=8:outer wheel S2 eta 1.425

   case 8:
     switch(etaBin){
     case 0:
     case 1:
       if(rOuterCutoff-pforcell.perp() < WidthofKaptonGapatEdge * 0.5) {
           validhit=false;
         }

       break;
     case 2:
       if(pinLocal.z() > lwc()->GetWheelThickness()-zEndofC8e2) {
           validhit=false;
         }
       break;
     case 42:
       if(DistanceToEtaLine( pforcell, eta_mid) < DistMinatEdgeinCrack) {
           validhit=false;
         }

       break;
     }
     break;

//compartment=9:outer wheel S3 eta 1.5

   case 9:
     switch(etaBin){
     case 18:
       if(pinLocal.z() >  lwc()->GetWheelThickness()-zEndofC9e19 &&
          pinLocal.z() <= lwc()->GetWheelThickness()-zSepofC9e18 &&
          pinLocal.perp() < r0aofC9e18 ) {
//          G4cout<<" Skip of Hit in aC9e18"<<" r="<<pinLocal.perp()<<
//                  " zinWheel="<<pinLocal.z()<<G4endl;
           validhit=false;

       }
       if(pinLocal.z() > lwc()->GetWheelThickness()-zSepofC9e18 &&
          pinLocal.perp() <
         r0cofC9e18+txofC9e18*(pinLocal.z()-(lwc()->GetWheelThickness()-zSepofC9e18))){
//          G4cout<<" Skip of Hit in cC9e18"<<" r="<<pinLocal.perp()<<
//                  " zinWheel="<<pinLocal.z()<<G4endl;
           validhit=false;

       }
       break;
     case 19:
	if(pinLocal.z() > lwc()->GetWheelThickness()-zEndofC9e19) {
//           G4cout<<" Skip of Hit in z crack C9e19"<<" r="<<pinLocal.perp()<<
//                  " zinWheel="<<pinLocal.z()<<G4endl;
           validhit=false;

        }
        if(DistanceToEtaLine( pforcell, eta_mid) < DistMinatEdgeinCrack) {
//           G4cout<<" Skip of Hit in eta crack of C9e19"<<
//                   " r="<<pinLocal.perp()<< " zinWheel="<<pinLocal.z()<<G4endl;
           validhit=false;
        }
        break;
     }
     break;

//compartment=10:outer wheel S1 eta 1.375

   case 10:
     if(-DistanceToEtaLine( pforcell, eta_low) < WidthofKaptonGapatEdge * 0.5) {
           validhit=false;
         }
     if(rOuterCutoff-pforcell.perp() < WidthofKaptonGapatEdge * 0.5)
       validhit=false;
     break;

//compartment=11:outer wheel S2 eta 1.375

   case 11:
     if(rOuterCutoff-pforcell.perp() < WidthofKaptonGapatEdge/2.) {
           validhit=false;
         }
     break;
   }

// treatment of HV bus;the signal collected in the region of the bus
// should be attached to different cell what found above

  // outer wheel, S1:

   G4int cnew=-1;

   if(m_compartment == 10 || m_compartment == 7 ){

     eta_max  = (geometry[c].etaOffset+etaBin+1.)/ geometry[c].etaScale;
     dist=DistanceToEtaLine(pforcell,eta_max);
//      assert(dist >= 0.);
     if (dist>=0. && dist <= DistMaxS1) cnew = c+1;
   }

// outer wheel, S3:

   else if(m_compartment == 9 ) {

     eta_min=(geometry[c].etaOffset+etaBin)   / geometry[c].etaScale;
     eta_max=(geometry[c].etaOffset+etaBin+1.)/ geometry[c].etaScale;
     dist_min =DistanceToEtaLine(pforcell,eta_min);
     dist_max =DistanceToEtaLine(pforcell,eta_max);

//      assert(dist_min <=0.);
//      assert(dist_max >=0.);

     if(dist_min<=0. && -dist_min <= DistMax) cnew = c-1;
     else {
       if(etaBin != geometry[c].maxEta) {
	 if(dist_max>=0. && dist_max <= DistMax) cnew = c-1;
       }
       else{ // close to the crack region between wheels, etaBin=maxEta;
	 if(dist_max >= DistMinatEdgeinCrack &&
	    dist_max <= DistMaxatEdgeinCrack)   cnew=c-1;
       }
     }
   }

   // inner wheel S2:

   else if(m_compartment == 2) {

     eta_min=(geometry[c].etaOffset+etaBin)/geometry[c].etaScale;
     dist   = DistanceToEtaLine(pforcell,eta_min);

//        assert(dist<=0.);

     if(etaBin == 0) {
       if(dist <=0. && -dist >= DistMinatEdgeinCrack &&
	  -dist <= DistMaxatEdgeinCrack)   cnew=c-1;
     }
     else if(dist<=0. && -dist <= DistMax) cnew=c-1;
   }

   if(cnew >= 0 && cnew <= 10 ){

/* G4cout<<" edep in HV bus: old:comp="<<c+1<<" sampl="<<sampling<<
              " eta="<<etaBin<<" reg="<<region
         <<G4endl;*/

     c=cnew;
     m_compartment = c + 1;
     sampling = geometry[c].sampling;
     region   = geometry[c].region;
     etaBin = G4int(eta * geometry[c].etaScale - geometry[c].etaOffset);
/* G4cout<<" edep in HV bus: new:comp="<<c+1<<" sampl="<<sampling<<
              " eta="<<etaBin<<" reg="<<region
	      <<G4endl;*/
   }
//=== end of edge and HV bus treatment================


	G4int phiGap = lwc()->GetPhiGap(pinLocal);
	if(lwc()->GetisModule()){
		if(phiGap < lwc()->GetFirstFan() || phiGap >= lwc()->GetLastFan()){
        // this hit is beyond of edge absorbers of the module and should be ignored
			if (phiGap < lwc()->GetFirstFan()) phiGap = lwc()->GetFirstFan();
			if (phiGap >= lwc()->GetLastFan()) phiGap = lwc()->GetLastFan() - 1;
			validhit = false;
		}
	}

   G4int phiBin = phiGap / geometry[c].gapsPerBin;

   if(atlasside < 0){
// The following formula assumes that the z<0 endcap was derived
// from the positive endcap by rotateY(180.*deg)
// 29-March-2004 ML
     phiBin = (geometry[c].maxPhi - 1)/2 - phiBin;
     if(phiBin < 0) phiBin += geometry[c].maxPhi + 1;
   }

   assert(phiBin >= 0);
   assert(phiBin <= geometry[c].maxPhi);
   if(phiBin<0) {phiBin=0;validhit=false;}
   if(phiBin>geometry[c].maxPhi) {
     phiBin=geometry[c].maxPhi;
     validhit=false;
   }
   if(etaBin > geometry[c].maxEta){
     (*m_msg) << MSG::WARNING
	    << "FindIdentifier: invalid hit, etaBin = "
	    << etaBin << " > geometry[" << c << "].maxEta="
	    << geometry[c].maxEta << endreq;
     etaBin=geometry[c].maxEta;
     validhit=false;
   }
   if(etaBin < 0){
     (*m_msg) << MSG::WARNING
	    << "FindIdentifier: invalid hit, etaBin < 0"
	    << endreq;
     etaBin=0;
     validhit=false;
   }

   hdata[0].id.clear();
   hdata[0].id  << 4 // LArCalorimeter
		<< 1 // LArEM
		<< atlasside
		<< sampling
		<< region
		<< etaBin
		<< phiBin;

   G4double timeOfFlight = (pre_step_point->GetGlobalTime() +
			    post_step_point->GetGlobalTime()) * 0.5;
   hdata[0].time = timeOfFlight/CLHEP::ns - p.mag()/CLHEP::c_light/CLHEP::ns;
   if (hdata[0].time > m_OOTcut) m_isInTime = false;
   else m_isInTime = true;

   return validhit;
}

/* gives the distance to the nearest electrode's surface */
// ****************************************************************************
G4double EnergyCalculator::distance_to_the_nearest_electrode(const G4ThreeVector &p) const
// ****************************************************************************
{
	G4ThreeVector p1( p );
	int fan_number = G4int((p.phi() - M_PI_2 - lwc()->GetZeroFanPhi()) / lwc()->GetFanStepOnPhi());
	G4double angle = lwc()->GetFanStepOnPhi() * fan_number + lwc()->GetZeroFanPhi();
	p1.rotateZ(-angle);
	G4double d0 = lwc()->DistanceToTheNeutralFibre(p1, lwc()->adjust_fan_number(fan_number));
	G4double d1;
	d1 = d0;
	G4int delta = 1;
	if(d0 < 0.) delta = -1;
	angle = - lwc()->GetFanStepOnPhi() * delta;
	do{
		p1.rotateZ(angle);
		fan_number += delta;
		d1 = lwc()->DistanceToTheNeutralFibre(p1, lwc()->adjust_fan_number(fan_number));
	} while(d0 * d1 > 0.);
	p1.rotateZ(-angle / 2);
	if(delta > 0) fan_number --;
	G4double d3 = lwc()->DistanceToTheNeutralFibre(p1, lwc()->adjust_fan_number(fan_number));
	return(fabs(d3) - ElectrodeFanHalfThickness);
}

// No account for edge effects in this function in case of module,
// because Process() will return "false" in this case.
// Affects m_fan_number.
// returns size of gap between absorber's and electrode's surfaces

// ****************************************************************************
double EnergyCalculator::GetGapSize(const G4ThreeVector& p) const
// ****************************************************************************
{
	G4ThreeVector p1 ( p );
	int fan_number = G4int((p.phi() - M_PI_2 - lwc()->GetZeroFanPhi()) / lwc()->GetFanStepOnPhi());
	G4double angle = lwc()->GetFanStepOnPhi() * fan_number + lwc()->GetZeroFanPhi();
	p1.rotateZ(-angle);
	const G4double d0 = lwc()->DistanceToTheNeutralFibre(p1, lwc()->adjust_fan_number(fan_number));

	const G4int delta = (d0 < 0.)? -1 : 1;
	G4double d1(d0), d2;

	angle = - lwc()->GetFanStepOnPhi() * delta;
	do {
		d2 = d1;
		p1.rotateZ(angle);
		fan_number += delta;
		d1 = lwc()->DistanceToTheNeutralFibre(p1, lwc()->adjust_fan_number(fan_number));
	} while(d0 * d1 > 0.);
	p1.rotateZ(-angle / 2);
	if(delta > 0) fan_number --;
	const G4double d3 = lwc()->DistanceToTheNeutralFibre(p1, lwc()->adjust_fan_number(fan_number));
	if(d3 * d2 < 0.){
		return(fabs(d2)
		       - lwc()->GetFanHalfThickness() + fabs(d3) - ElectrodeFanHalfThickness);
	} else {
		return(fabs(d1)
		       - lwc()->GetFanHalfThickness() + fabs(d3) - ElectrodeFanHalfThickness);
	}
}

// ****************************************************************************
void EnergyCalculator::SetConst_InnerBarrett(void){
// ****************************************************************************
  if(SetConstInnerBarrett) return;
     SetConstInnerBarrett=true;

  std::cout <<" ===>>> ERROR!!  SetConst_InnerBarrett is called!!!" <<std::endl;
  exit(99);
}

// ****************************************************************************
void EnergyCalculator::SetConst_OuterBarrett(void){
// ****************************************************************************

  if(SetConstOuterBarrett) return;
     SetConstOuterBarrett=true;

  for(G4int i=0;i<=20;++i){
	  const G4double teta = 2.*atan( exp(-S3_Etalim[i]));
	  S3_Rlim[i] = RefzDist*tan(teta);
  }

  rlim[0] = S3_Rlim[3] + KapGap    +  Rmeas_outer[0] /*11.59 */  / ColdCorrection;
  rlim[1] = S3_Rlim[3] + KapGap    +  Rmeas_outer[1] /*25.22 */  / ColdCorrection;
  rlim[2] = S3_Rlim[3] + KapGap    +  Rmeas_outer[2] /*57.28 */  / ColdCorrection;
  rlim[3] = S3_Rlim[3] + KapGap    +  Rmeas_outer[3] /*71.30 */  / ColdCorrection;
  rlim[4] = S3_Rlim[3] + KapGap    +  Rmeas_outer[4] /*85.90 */  / ColdCorrection;
  rlim[5] = S3_Rlim[3] + KapGap    +  Rmeas_outer[5] /*98.94 */  / ColdCorrection;
  rlim[6] = S3_Rlim[3] + KapGap    +  Rmeas_outer[6] /*103.09 */ / ColdCorrection;
  rlim[7] = S3_Rlim[3] + KapGap    +  Rmeas_outer[7] /*116.68 */ / ColdCorrection;
  rlim[8] = S3_Rlim[3] + KapGap    +  Rmeas_outer[8] /*130.42 */ / ColdCorrection;
  rlim[9] = S3_Rlim[3] + KapGap/2. +  Rmeas_outer[9] /*146.27 */ / ColdCorrection + EdgeWidth;
  rlim[10]= rlim[8]                +  Rmeas_outer[10]/*147.19 */ / ColdCorrection;

  rlim[11]= S3_Rlim[3] + KapGap    +  Rmeas_outer[11]/*11.59 */ / ColdCorrection; //eta=1.65
  rlim[12]= S3_Rlim[3] - KapGap    -  Rmeas_outer[12]/*15.   */ / ColdCorrection;

  rlim[13]= S3_Rlim[4] + KapGap    +  Rmeas_outer[13]/*56.91 */ / ColdCorrection; //eta=1.7
  rlim[14]= S3_Rlim[4] - KapGap    -  Rmeas_outer[14]/*44.37 */ / ColdCorrection;

  rlim[15]= S3_Rlim[5] + KapGap    +  Rmeas_outer[15]/*15.13 */ / ColdCorrection; //eta=1.75
  rlim[16]= S3_Rlim[5] - KapGap    -  Rmeas_outer[16]/*14.93 */ / ColdCorrection;

  rlim[17]= S3_Rlim[6] + KapGap    +  Rmeas_outer[17]/*45.87 */ / ColdCorrection; //eta=1.80
  rlim[18]= S3_Rlim[6] - KapGap    -  Rmeas_outer[18]/*35.03 */ / ColdCorrection;

  rlim[19]= S3_Rlim[7] + KapGap    +  Rmeas_outer[19]/*15.40 */ / ColdCorrection; //eta=1.85
  rlim[20]= S3_Rlim[7] - KapGap    -  Rmeas_outer[20]/*14.04 */ / ColdCorrection;

  rlim[21]= S3_Rlim[8] + KapGap    +  Rmeas_outer[21]/*39.67 */ / ColdCorrection; //eta=1.90
  rlim[22]= S3_Rlim[8] - KapGap    -  Rmeas_outer[22]/*26.83 */ / ColdCorrection;

  rlim[23]= S3_Rlim[9] + KapGap    +  Rmeas_outer[23]/*15.64 */ / ColdCorrection; //eta=1.95
  rlim[24]= S3_Rlim[9] - KapGap    -  Rmeas_outer[24]/*14.90 */ / ColdCorrection;

  rlim[25]= S3_Rlim[10] + KapGap   +  Rmeas_outer[25]/*30.26 */ / ColdCorrection; //eta=2.00
  rlim[26]= S3_Rlim[10] - KapGap   -  Rmeas_outer[26]/*14.70 */ / ColdCorrection;

  rlim[27]= S3_Rlim[10] - KapGap   -  Rmeas_outer[27]/*29.09 */ / ColdCorrection; //eta=2.05
  rlim[28]= S3_Rlim[10] - KapGap   -  Rmeas_outer[28]/*43.12 */ / ColdCorrection; //SHAPE CHANGE!!ZZ

  rlim[29]= S3_Rlim[12] + KapGap   +  Rmeas_outer[29]/*34.51 */ / ColdCorrection; //eta=2.10
  rlim[30]= S3_Rlim[12] - KapGap   -  Rmeas_outer[30]/*25.08 */ / ColdCorrection;

  rlim[31]= S3_Rlim[13] + KapGap   +  Rmeas_outer[31]/*11.88 */ / ColdCorrection; //eta=2.15
  rlim[32]= S3_Rlim[13] - KapGap   -  Rmeas_outer[32]/*14.39 */ / ColdCorrection;

  rlim[33]= S3_Rlim[14] + KapGap   +  Rmeas_outer[33]/*19.54 */ / ColdCorrection; //eta=2.20
  rlim[34]= S3_Rlim[14] - KapGap   -  Rmeas_outer[34]/*17.80 */ / ColdCorrection; // !!ZZ

  rlim[35]= S3_Rlim[15] + KapGap   +  Rmeas_outer[35]/*12.70 */ / ColdCorrection; //eta=2.25
  rlim[36]= S3_Rlim[15] - KapGap   -  Rmeas_outer[36]/*15.31 */ / ColdCorrection;

  rlim[37]= S3_Rlim[16] + KapGap   +  Rmeas_outer[37]/*13.96 */ / ColdCorrection; //eta=2.30
  rlim[38]= S3_Rlim[16] - KapGap   -  Rmeas_outer[38]/*11.79 */ / ColdCorrection; // !!ZZ!!

  rlim[40]= S3_Rlim[16] - KapGap   -  Rmeas_outer[40]/*23.57 */ / ColdCorrection;
  rlim[41]= S3_Rlim[16] - KapGap   -  Rmeas_outer[41]/*34.64 */ / ColdCorrection;
  rlim[42]= S3_Rlim[16] - KapGap   -  Rmeas_outer[42]/*55.32 */ / ColdCorrection;
  rlim[43]= S3_Rlim[16] - KapGap   -  Rmeas_outer[43]/*65.39 */ / ColdCorrection;
  rlim[44]= S3_Rlim[16] - KapGap   -  Rmeas_outer[44]/*76.34 */ / ColdCorrection;
  rlim[45]= rlim[44]               -  Rmeas_outer[45]/*10.83 */ / ColdCorrection;
  rlim[46]= S3_Rlim[16] - KapGap/2.-  Rmeas_outer[46]/*94.84 */ / ColdCorrection - EdgeWidth;
  rlim[47]= S3_Rlim[16] - KapGap/2.-  Rmeas_outer[47]/*98.00 */ / ColdCorrection - EdgeWidth;


  zlim[0] = - EdgeWidth    +  Zmeas_outer[0]/*3.81*/ / ColdCorrection; //rel. to the end_of_HV_Cu
  zlim[1] = - KapGap/2.    +  Zmeas_outer[1]/*7.81*/ / ColdCorrection;
  zlim[2] =  StripWidth + 1./2. * KapGap;
  zlim[3] =2*StripWidth + 3./2. * KapGap;

  for (G4int i=0; i<=3; ++i){
    zlim[i]= (ZmaxOfSignal-EdgeWidth) - zlim[i]; //rel to start of the Barrette
    if(zlim[i] < 0.) zlim[i]=0.;
  }

  return;
}
// ****************************************************************************
G4bool EnergyCalculator::GetCompartment_Barrett(
       G4ThreeVector pforcell, G4double r_inb, G4double z_inb, G4double eta_inb,
       G4int &b_compartment, G4int &etabin) const{
// ****************************************************************************

   G4double d,d1,d2,rlim1,rlim2,rlim3,zlim1,zlim2,eta1,eta2;
   G4int i;
   G4int i0 = 3;

   G4bool validhit=true;
   b_compartment=-99;
   etabin=-99;

   if(r_inb > rlim[10] || r_inb < rlim[47] )
                            {validhit=false; goto label99;}
   if(z_inb > ZmaxOfSignal ){validhit=false; goto label99;}

   if(r_inb > rlim[0]) {  // Upper corner

   if(r_inb > rlim[9])      {
      if(z_inb > zlim[0])   {validhit=false; goto label99;}
label1:
      if(z_inb > zlim[1])   {
                               b_compartment = 8;
                               etabin        = 4;
                               goto label99;
      }
label2:
                               b_compartment = 9;
                               etabin        = 0;
                               goto label99;
   }
   if(r_inb > rlim[8])  goto label1;
   if(r_inb > rlim[7])  goto label2;
   if(r_inb > rlim[6])      {
                               b_compartment = 8;
                               etabin        = 5;
                               goto label99;
   }
   if(r_inb > rlim[5])      {
label3:
                               b_compartment = 8;
                               etabin        = 6;
                               goto label99;
   }
   if(r_inb > rlim[4])      {
     if(z_inb >  zlim[1])   goto label3;

label4:
                               b_compartment = 9;
                               etabin        = 1;
                               goto label99;
   }

   if(r_inb > rlim[3])  goto label4;

   if(r_inb > rlim[2])      {
                               b_compartment = 8;
                               etabin        = 7;
                               goto label99;
   }

   if(r_inb > rlim[1])      {
                               b_compartment = 8;
                               etabin        = 8;
                               goto label99;
   }
   if(r_inb > rlim[0])      {
                               b_compartment = 9;
                               etabin        = 2;
                               goto label99;
   }
   }

   if(r_inb < rlim[38]){    // lower corner

     if( r_inb > rlim[40] ) {
                               b_compartment = 9;
                               etabin        = 16;
                               goto label99;
     }

     if( r_inb > rlim[41] ) {
label5:
                               b_compartment = 8;
                               etabin        = 37;
                               goto label99;
     }

     if( r_inb > rlim[42] ) {

       d=DistanceToEtaLine( pforcell,2.35);
       if(fabs(d) < StripWidth+KapGap) {
	 if( d < 0.) {

label6:
                               b_compartment = 8;
	                       etabin        = 38;
	                       goto label99;
	 }
label7:
         if( z_inb < zlim[3] ) goto label5;
         goto label6;
       }

       if( d > 0. )  goto label7;

                               b_compartment = 9;
                               etabin        = 17;
                               goto label99;
     }

     if( r_inb > rlim[43] ) {
                               b_compartment = 8;
                               etabin        = 39;
                               goto label99;
     }

     if( r_inb > rlim[44] ) {
label8:
                               b_compartment = 8;
                               etabin        = 40;
                               goto label99;
     }

     if( r_inb > rlim[45] ) {
       if( z_inb < zlim[3] ) goto label8;

label9:
                               b_compartment = 9;
                               etabin        = 18;
                               goto label99;
     }

     if( r_inb > rlim[46] ) goto label9;

     if( z_inb < ZmaxOfSignal/(rlim[46]-rlim[47])*(r_inb-rlim[47])) goto label9;

     validhit=false;
     goto label99;
   }

// medium r region:   rlim[0] > r > rlim[38];
//   from middle of cellno 2 to middle of cellno. 16
//

   for( i=3; i<=17; ++i){         // eta= 1.65 - 2.35
     if( eta_inb < S3_Etalim[i] ) {
       i0=i;
       break;
     }
   }

     i=i0;

     eta1    = S3_Etalim[i-1];
     eta2    = S3_Etalim[i];
     rlim1   = rlim[2*i+5 - 1];
     rlim2   = rlim[2*i+5];
     zlim1   = zlim[2];
     zlim2   = zlim[3];

     if( i == 15 || i == 17) {
        zlim1   = zlim[3];
        zlim2   = zlim[2];
     }

     switch(i){

     case 3:

       if( fabs( DistanceToEtaLine(pforcell, eta2) ) <  StripWidth+KapGap
          || z_inb > zlim1 ){

                                       b_compartment = 8;
                                       etabin        = 2*i+3;;
                                       break;
       }

                                       b_compartment = 9;
                                       etabin        = i-1;
                                       break;

     case 4:
     case 5:
     case 6:
     case 7:
     case 8:
     case 9:
     case 10:
     case 13:
     case 16:

       d1=fabs( DistanceToEtaLine( pforcell, eta1) );
       d2=fabs( DistanceToEtaLine( pforcell, eta2) );

       if( d1 < StripWidth+KapGap ){
label11:
                                       b_compartment = 8;
                                       etabin        = 2*i+2;
                                       break;
       }
       if( d2 < StripWidth+KapGap ){
label12:
                                       b_compartment = 8;
                                       etabin        = 2*i+3;
                                       break;
       }

       if( z_inb < zlim1 || (r_inb > rlim2 && r_inb < rlim1) ) {

                                       b_compartment = 9;
                                       etabin        = i-1;
                                       break;
       }
       if( r_inb > rlim1 ) goto label11;
       if( r_inb < rlim2 ) goto label12;
       validhit=false;
       break;
//========================================================
     case 11:

       rlim3 = rlim[28];

       d1=fabs( DistanceToEtaLine( pforcell, eta1) );
       d2=fabs( DistanceToEtaLine( pforcell, eta2) );

       if( d1 < StripWidth+KapGap ){
label13:
                                       b_compartment = 8;
                                       etabin        = 2*i+2;
                                       break;
       }

       if( r_inb > rlim1 && z_inb > zlim1 ) goto label13;
       if( r_inb > rlim2 ){
label14:
                                       b_compartment = 9;
                                       etabin        = i-1;
                                       break;
       }

       if( d2 < StripWidth+KapGap ){
	 if( z_inb > zlim1 ) {
label15:
                                       b_compartment = 8;
                                       etabin        = 2*i+4;
                                       break;
	 }

label16:
                                       b_compartment = 8;
                                       etabin        = 2*i+3;
                                       break;
       }

       if( z_inb < zlim2) goto label14;
       if( r_inb > rlim3) goto label16;
       if( z_inb > zlim1) goto label15;
       goto label16;
//======================================================
     case 12:

       d1=fabs( DistanceToEtaLine( pforcell, eta1) );
       d2=fabs( DistanceToEtaLine( pforcell, eta2) );

       if( d1 < StripWidth+KapGap ){
                                       b_compartment = 8;
                                       etabin        = 2*i+2;
                                       break;
       }
       if( d2 < StripWidth+KapGap ){
label17:
                                       b_compartment = 8;
                                       etabin        = 2*i+3;
                                       break;
       }

       if( r_inb > rlim2 || z_inb < zlim1 ){
                                       b_compartment = 9;
                                       etabin        = i-1;
                                       break;
       }
       goto label17;

//========================================================

     case 14:
     case 15:

       d1=fabs( DistanceToEtaLine( pforcell, eta1) );
       d2=fabs( DistanceToEtaLine( pforcell, eta2) );

       if( d1 < StripWidth+KapGap ){
label18:
                                       b_compartment = 8;
                                       etabin        = 2*i+2;
                                       break;
       }
       if( d2 < StripWidth+KapGap ){
label19:
                                       b_compartment = 8;
                                       etabin        = 2*i+3;
                                       break;
       }
       if( r_inb > rlim1 && z_inb > zlim1 ) goto label18;
       if( r_inb < rlim2 && z_inb > zlim2 ) goto label19;

                                       b_compartment = 9;
                                       etabin        = i-1;
                                       break;
//======================================================

     case 17:

       d1=fabs( DistanceToEtaLine( pforcell, eta1) );

       if( d1 < StripWidth+KapGap || z_inb > zlim1  ){
                                       b_compartment = 8;
                                       etabin        = 2*i+2;
                                       break;
       }
                                       b_compartment = 9;
                                       etabin        = i-1;
                                       break;
     }

//======================================================


// end of search for compartment and etabin

label99:
        return validhit;
}
// ****************************************************************************
G4bool EnergyCalculator::GetVolumeIndex(const G4Step *step) const{
// ****************************************************************************
  ModuleNumber = -999;
  PhiDivNumber = -999;

  const G4StepPoint*  pre_step_point = step->GetPreStepPoint();
  const G4int         ndepth = pre_step_point->GetTouchable()->GetHistoryDepth();

  for(G4int i=0;i<=ndepth;++i){
   G4String ivolname=pre_step_point->GetTouchable()->GetVolume(i)->GetName();
    if( ivolname.find("BackOuterBarrette::Module") != std::string::npos ){
      ModuleNumber = pre_step_point->GetTouchable()->GetVolume(i)->GetCopyNo();
    }
    if( ivolname.find("BackOuterBarrette::Module::Phidiv") != std::string::npos ){
      PhiDivNumber = pre_step_point->GetTouchable()->GetVolume(i)->GetCopyNo();
    }
  }

  if(!lwc()->GetisModule()){
    if(ModuleNumber < 0 || PhiDivNumber < 0) {return false;}
  }
  else if(PhiDivNumber < 0 ) {return false;}

  return true;
}
// ****************************************************************************
G4bool EnergyCalculator::FindIdentifier_Barrett(
	const G4Step* step,
        std::vector<LArHitData>& hdata,
	G4ThreeVector &startPointLocal,
	G4ThreeVector &endPointLocal
	){
// ****************************************************************************
// works only for outer part of the full wheel or of the module in the Barrett
//     at the back side of EMEC

// check whether we are in the outer wheel
	if(lwc()->type() != LArWheelCalculator::OuterAbsorberWheel && lwc()->type() != LArWheelCalculator::OuterAbsorberModule) {
		(*m_msg) << MSG::FATAL
		<< " ERROR ::FindIdentifier_Barrett, not yet prepared for solidtype="
		<< LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())
		<< endreq;
	}

    G4bool validhit=true;

// Get point coordinates in the Atlas coord. system

	const G4StepPoint*  pre_step_point = step->GetPreStepPoint();
	const G4StepPoint* post_step_point = step->GetPostStepPoint();

	const G4ThreeVector startPoint =  pre_step_point->GetPosition();
	const G4ThreeVector   endPoint = post_step_point->GetPosition();
	//	G4ThreeVector          p = 0.5 *(startPoint+endPoint);
	const G4ThreeVector     p = startPoint; // bec. middle point maybe out of volume

    // transform point to the coord system of Barrett::Module::Phidiv (alias local)

	const G4AffineTransform transformation =
			pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();
	startPointLocal = transformation.TransformPoint(startPoint);
	endPointLocal = transformation.TransformPoint(endPoint);
    // G4ThreeVector  pinLocal = 0.5 * (startPointLocal + endPointLocal);
	const G4ThreeVector  pinLocal = startPointLocal;

// get Module and Phidiv number (result is put into static VolumeNumber and PhiDivNumber)

	validhit=GetVolumeIndex(step);
	if(!validhit){
          //std::cout
	  (*m_msg) << MSG::FATAL
		 <<" ERROR ::FindIdentifier_Barrett:Module, Phidiv is not found"
		 <<" ModuleNumber= "<<ModuleNumber<<" PhiDivNumber= "<<PhiDivNumber
		 <<endreq;
	  //	 <<std::endl;
	}

//z,r,eta,phi_inb : Specific for Barrette at the Back side of EMEC!!

	G4double phi_inb=0.;

	const G4double z_inb= lwc()->GetdWRPtoFrontFace()/2.-pinLocal.z(); //dist. from front end of the Back Barrettes
	const G4double r_inb= pinLocal.perp();                 //dist from the z axis
    const G4ThreeVector pforcell=G4ThreeVector( pinLocal.x(), pinLocal.y(),
                 lwc()->GetElecFocaltoWRP()+lwc()->GetdWRPtoFrontFace()+lwc()->GetWheelThickness()+z_inb );
    const G4double eta_inb=pforcell.pseudoRapidity();    //eta in the system where electrodes were designed

	if(lwc()->type() ==  LArWheelCalculator::OuterAbsorberWheel){ // for wheel calculation
        PhiStartOfPhiDiv = lwc()->GetFanStepOnPhi()/2. + ModuleNumber * CLHEP::twopi/8.
	                                   + PhiDivNumber * lwc()->GetFanStepOnPhi();
        phi_inb = PhiStartOfPhiDiv + pinLocal.phi();  //in ::BackOuterBarrettes
	    if(phi_inb < 0.)      phi_inb = phi_inb + CLHEP::twopi;
	    if(phi_inb > CLHEP::twopi) phi_inb = phi_inb - CLHEP::twopi;
	    phi_inb =  CLHEP::twopi - phi_inb;        // phi in ::EmecMother system;
	}
	else if(lwc()->type() ==  LArWheelCalculator::OuterAbsorberModule){ // for TB modul calculation

  	    G4double PhiStart = M_PI_2 - M_PI/8.; //this is from EMECSupportConstruction
	    PhiStartOfPhiDiv = PhiStart + lwc()->GetFanStepOnPhi()/2 + PhiDivNumber * lwc()->GetFanStepOnPhi();

	    phi_inb = PhiStartOfPhiDiv + pinLocal.phi(); //in BackOuterBarrettes;
	    phi_inb =  M_PI - phi_inb;        // phi in ::EmecMother system;
    }

	  G4int compartment,etabin,phigap,etaBin,phiBin;

// get m_compartment and etaBin

	  validhit=GetCompartment_Barrett(pforcell, r_inb, z_inb, eta_inb, compartment, etabin);

      m_compartment = compartment;
	  etaBin        = etabin;
      if (!validhit) {
		m_compartment = 9;  // to have some 'reasonable' number
		etaBin        = 0;
      }

	  const G4int c = m_compartment-1;

	  G4int sampling = geometry[c].sampling;
	  G4int region   = geometry[c].region;

// get m_AtlasZside, it is negative if z<0.

// DM 2015-07-30
// copy of 
// 19-04-2007 AMS    use constant m_AtlasZside obtained in constructor
  // zSide is negative if z<0.
//	m_AtlasZside = geometry[c].zSide;
//	if(p.z() < 0.) m_AtlasZside = -m_AtlasZside;
//	G4int atlasside = m_AtlasZside;
	const G4int atlasside = lwc()->GetAtlasZside() * geometry[c].zSide;

	  if(lwc()->GetisModule() && atlasside < 0 ) {
		(*m_msg) << MSG::FATAL
		  <<"EnergyCalculator: TB modul should be at pos z"
		  <<endreq;
	  }

// In future version we may use constant m_AtlasZside set by constructor,
// don't know if it is necessary for barettes
//      G4int atlasside = m_AtlasZside * geometry[c].zSide;

// get phiBin

	  phigap = GetPhiGap_Barrett(pinLocal); // in wheel numbering scheme
	  //int phigapwheel=phigap; //for check

	  if(lwc()->GetisModule()) {
		phigap = phigap - lwc()->GetStartGapNumber() + lwc()->GetLastFan()/2;  // in module numbering scheme

/*
//CHECK!!in Module
	  int phigapmodul=phigap;
	  G4ThreeVector tmp=G4ThreeVector( r_inb*cos(phi_inb),r_inb*sin(phi_inb),0.01 );
	  int phigapmodulx=LArWheelCalculator::GetPhiGap(tmp); // module numbering scheme
	  int phigapwheelx=lwc()->PhiGapNumberForWheel(phigapmodulx); // transformed to wheelnumbering

      	  if(phigapwheel != phigapwheelx)
	    std::cout<<"*** FindIdentifier_BArrett:ERROR:phigapwheel="
		     <<phigapwheel<<" phigapwheelx="<<phigapwheelx
		     <<std::endl;
          else std::cout<<"*** checked--wheel--Ok"<<std::endl;
      	  if(phigapmodul != phigapmodulx)
	    std::cout<<"*** FindIdentifier_BArrett:ERROR:phigapmodul="
		     <<phigapmodul<<" phigapmodulx="<<phigapmodulx
		     <<" FirstFan="<<FirstFan<<" LastFan="<<LastFan
		     <<std::endl;
          else std::cout<<"*** checked--modul--Ok"<<std::endl;
// --- end CHECK
*/

	  if(phigap < lwc()->GetFirstFan() || phigap >= lwc()->GetLastFan()){
	    if (phigap<lwc()->GetFirstFan()) phigap=lwc()->GetFirstFan();
	    if (phigap>=lwc()->GetLastFan()) phigap=lwc()->GetLastFan()-1;
	    validhit=false;
	  }
	}

	phiBin = phigap / geometry[c].gapsPerBin;

	if(atlasside < 0){

    // The following formula assumes that the z<0 endcap was derived
    // from the positive endcap by rotateY(180.*deg)
    // 29-March-2004 ML

	  phiBin = (geometry[c].maxPhi - 1)/2 - phiBin;
	  if(phiBin < 0) phiBin += geometry[c].maxPhi + 1;
        }

// checks for phiBin and etaBin

        if(phiBin<0) {
	  (*m_msg) << MSG::WARNING
		 << "FindIdentifier_Barrett: invalid hit, phiBin < 0"
		 << endreq;
	  phiBin=0;
	  validhit=false;
        }
        if(phiBin>geometry[c].maxPhi) {
	  (*m_msg) << MSG::WARNING
		 << "FindIdentifier_Barrett: invalid hit, phiBin = " << phiBin
		 << " > geometry[" << c << "].maxPhi="<< geometry[c].maxPhi
		 << endreq;
	  phiBin=geometry[c].maxPhi;
	  validhit=false;
        }
        if(etaBin < 0){
	  (*m_msg) << MSG::WARNING
		 << "FindIdentifier_Barrett: invalid hit, etaBin < 0"
		 << endreq;
	  etaBin=0;
	  validhit=false;
        }
        if(etaBin > geometry[c].maxEta){
	  (*m_msg) << MSG::WARNING
		 << "FindIdentifier_Barrett: invalid hit, etaBin = "
		 << etaBin << " > geometry[" << c << "].maxEta="
		 << geometry[c].maxEta << endreq;
	  etaBin=geometry[c].maxEta;
	  validhit=false;
        }

        hdata[0].id.clear();
        hdata[0].id  << 4 // LArCalorimeter
                     << 1 // LArEM
                     << atlasside
                     << sampling
                     << region
                     << etaBin
                     << phiBin;

        G4double timeOfFlight = 0.5 *
	  ( pre_step_point->GetGlobalTime() +
	    post_step_point->GetGlobalTime()    );
        hdata[0].time = timeOfFlight/CLHEP::ns - p.mag()/CLHEP::c_light/CLHEP::ns;
        if (hdata[0].time > m_OOTcut) m_isInTime = false;
        else m_isInTime = true;

        return validhit;
}
// ****************************************************************************
G4bool EnergyCalculator::FindDMIdentifier_Barrett(const G4Step* step, std::vector<LArHitData>& hdata){
// ****************************************************************************

  G4bool validid = false;
  hdata[0].id = LArG4Identifier();

  validid      = m_supportCalculator->Process(step,LArG4::VCalibrationCalculator::kOnlyID);
  hdata[0].id = m_supportCalculator->identifier();

  return validid;
}

//****************************************************************************
G4int EnergyCalculator::GetPhiGap_Barrett(const G4ThreeVector& p) const {
// ****************************************************************************
  G4double phi=p.phi();           // in Module::Phidiv
  phi = PhiStartOfPhiDiv + phi;   // in Barrette
  if(phi < 0. )       phi=phi+CLHEP::twopi;
  if(phi >= CLHEP::twopi ) phi=phi-CLHEP::twopi;

  if(lwc()->GetisModule()) phi =    M_PI - phi;     // in EMECMother; TB modul
  else          phi = CLHEP::twopi - phi;     // in EMECMother; full wheel

  if(phi > CLHEP::twopi-lwc()->GetFanStepOnPhi()*0.5) {return 0;}
  return (G4int( (phi+lwc()->GetFanStepOnPhi()*0.5)/lwc()->GetFanStepOnPhi()) );
}
// ****************************************************************************
G4double EnergyCalculator::GetGapSize_Barrett(const G4ThreeVector& p) const {
// ****************************************************************************
  const G4double r=p.perp();
  const G4double ta1=1./ sqrt(4.*r/FanAbsThickness*r/FanAbsThickness - 1.);
  const G4double ta2=1./ sqrt(4.*r/FanEleThickness*r/FanEleThickness - 1.);
  const G4double phieff= lwc()->GetFanStepOnPhi()*0.5-atan(ta1)-atan(ta2);
  return (r*phieff);
}
// ****************************************************************************
G4double EnergyCalculator::distance_to_the_nearest_electrode_Barrett(
                                                             const G4ThreeVector &p) const {
// ****************************************************************************
  G4double phi=p.phi();           // in Module::Phidiv
  phi = PhiStartOfPhiDiv + phi;   // in Barrette
  if(phi < 0. ) phi=phi+CLHEP::twopi;
  if(phi >= CLHEP::twopi ) phi=phi-CLHEP::twopi;

  if(lwc()->GetisModule()) phi =    M_PI - phi;     // in EMECMother; TB modul
  else          phi = CLHEP::twopi - phi;     // in EMECMother; full wheel

  G4double r=p.perp();
  G4int igap;
  G4double elephi,dphi;
  if (phi > CLHEP::twopi-lwc()->GetFanStepOnPhi()*0.5) {
	dphi=phi-CLHEP::twopi;
  } else {
	igap=G4int( (phi+lwc()->GetFanStepOnPhi()*0.5)/lwc()->GetFanStepOnPhi() );
    elephi=igap*lwc()->GetFanStepOnPhi();
	dphi=phi-elephi;
  }
  G4double dist=r*sin(fabs(dphi))-FanEleThickness*0.5;

  return fabs(dist);
}

// ****************************************************************************
G4double EnergyCalculator::GetHV_Value_Barrett(const G4ThreeVector& p) const {
// ****************************************************************************

// get atlasside

	const G4int atlasside = (lwc()->GetAtlasZside() > 0) ? 0 : 1;
	
// get etasection

	const G4double z_inb = lwc()->GetdWRPtoFrontFace() * 0.5-p.z(); //dist. from front end of the Back Barrettes
	const G4double r_inb= p.perp();                 //dist from the z axis
    const G4ThreeVector pforcell=G4ThreeVector( p.x(), p.y(),
                 lwc()->GetElecFocaltoWRP()+lwc()->GetdWRPtoFrontFace()+lwc()->GetWheelThickness()+z_inb );
    const G4double eta_inb=pforcell.pseudoRapidity();    //eta in the system where electrodes were designed

	G4int compartment=-99; G4int etabin=-99; G4int etasection=-99;

	G4bool validhit=GetCompartment_Barrett(pforcell, r_inb, z_inb, eta_inb, compartment, etabin);

	if(!validhit) { return 0.;}  // p is not in the sens. region

	switch (compartment) {

	case 8:
	  if( etabin <   0) { validhit=false; break;}
	  if( etabin <=  3) { etasection = 0; break;}
	  if( etabin <=  7) { etasection = 1; break;}
	  if( etabin <= 15) { etasection = 2; break;}
	  if( etabin <= 23) { etasection = 3; break;}
	  if( etabin <= 27) { etasection = 4; break;}
	  if( etabin <= 35) { etasection = 5; break;}
	  if( etabin <= 43) { etasection = 6; break;}
	  validhit=false;
	  break;

	case 9:
	  if( etabin <   0) { validhit=false; break;}
	  if( etabin <=  1) { etasection = 1; break;}
	  if( etabin <=  5) { etasection = 2; break;}
	  if( etabin <=  9) { etasection = 3; break;}
	  if( etabin <= 11) { etasection = 4; break;}
	  if( etabin <= 15) { etasection = 5; break;}
	  if( etabin <= 19) { etasection = 6; break;}
	  validhit=false;
	  break;

	default:
	  validhit=false;
	  break;
	}

	if(!validhit) { return 0.;}  // p is not in the sens. region

// get electrode number and side

  G4double phi=p.phi();           // in Module::Phidiv
  phi = PhiStartOfPhiDiv + phi;   // in Barrette
  if(phi < 0. )       phi=phi+CLHEP::twopi;
  if(phi >= CLHEP::twopi ) phi=phi-CLHEP::twopi;

  if(lwc()->GetisModule()) phi =    M_PI - phi;     // in EMECMother; TB modul
  else          phi = CLHEP::twopi - phi;     // in EMECMother; full wheel

  G4int igap,iside;
  if     (phi > CLHEP::twopi-lwc()->GetFanStepOnPhi()*0.5){igap=0;iside=0;}
  else{   igap=G4int( (phi+lwc()->GetFanStepOnPhi()*0.5)/lwc()->GetFanStepOnPhi() );
          G4double elephi=igap*lwc()->GetFanStepOnPhi();
	  G4double dphi=phi-elephi;
	  if(dphi <=0. ) iside=0;
	  else iside=1;
  }

  const G4int electrodenumber = igap;
  const G4int electrodeside   = iside;

// prepare indices for different versions of HV Map

  G4int firstelectrode  = HV_Start_phi[atlasside][etasection][electrodeside];
  G4int electrodeindex  = electrodenumber-firstelectrode;
  if(electrodeindex < 0) electrodeindex += lwc()->GetNumberOfFans();
  G4int phisection      = electrodeindex / NumberOfElectrodesInPhiSection;

  assert(phisection>=0 && phisection<=NofPhiSections-1);

// pick up HV value from the array

  G4double HV_value= HV_Values[atlasside][etasection][electrodeside][phisection];

  return HV_value;
}



