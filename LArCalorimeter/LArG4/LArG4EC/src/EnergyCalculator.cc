/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "LArG4Code/ILArCalibCalculatorSvc.h"

#include "EnergyCalculator.h"
#include "AthenaKernel/Units.h"

#include "HVHelper.h"

#define MSG_VECTOR(v) "(" << v.x() << ", " << v.y() << ", " << v.z() << ")"

using namespace LArG4::EC;
namespace Units = Athena::Units;

// ****************************************************************************
EnergyCalculator::Wheel_Efield_Map EnergyCalculator::s_ChCollInner;
EnergyCalculator::Wheel_Efield_Map EnergyCalculator::s_ChCollOuter;

const G4double            EnergyCalculator::s_GridSize        =0.1; //[mm]
const G4double            EnergyCalculator::s_AverageGap      = 1.3*CLHEP::mm;
G4bool              EnergyCalculator::s_FieldMapsRead   =false;
G4String            EnergyCalculator::s_FieldMapVersion = "";
// ****************************************************************************
const G4double EnergyCalculator::s_LArTemperature_ECC0=88.15; //K
const G4double EnergyCalculator::s_LArTemperature_ECC1=88.37;
const G4double EnergyCalculator::s_LArTemperature_ECC5=87.97;
const G4double EnergyCalculator::s_LArTemperature_av  =88.16;
const G4double EnergyCalculator::s_AverageHV=1250.;//[V]
const G4double EnergyCalculator::s_AverageEfield=0.01*s_AverageHV/s_AverageGap;//[kv/cm]
const G4double EnergyCalculator::s_AverageCurrent=1./s_AverageGap*IonReco(s_AverageEfield)
                                                                        *DriftVelo(s_LArTemperature_av,s_AverageEfield);
// ****************************************************************************
//      G4double EnergyCalculator::s_CHC_Esr  =0.2*CLHEP::mm; //[mm]
#ifdef DEBUG_CHCL
G4int    EnergyCalculator::s_CHCIprint=0;
G4double EnergyCalculator::s_CHCEbad  =0.;
G4double EnergyCalculator::s_CHCEtotal=0.;
G4double EnergyCalculator::s_CHCStotal=0.;
#endif
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

// ****************************************************************************
G4double EnergyCalculator::DistanceToEtaLine(const G4ThreeVector &p, G4double eta) const {
  G4double sinTheta = 2. * exp(-eta) / (1. + exp(2. * -eta));
  G4double cosTheta = (1. - exp(2. * -eta)) / (1. + exp(2. * -eta));
  return p.perp() * cosTheta - p.z() * sinTheta;
}

// ****************************************************************************
G4bool EnergyCalculator::Process(const G4Step* step, std::vector<LArHitData>& hdata) const {
  // ****************************************************************************

  // make sure hdata is reset
  hdata.resize(1);
  return (this->*m_Process_type)(step, hdata);
}
// ****************************************************************************

StatusCode EnergyCalculator::finalize() {
  if(m_birksLaw) {
    delete m_birksLaw;
    m_birksLaw = nullptr;
  }
  delete m_lwc;
  m_lwc = nullptr;

    if(m_HVHelper){
        delete m_HVHelper;
        m_HVHelper = nullptr;
    }
  return StatusCode::SUCCESS;
}

// ****************************************************************************
G4bool EnergyCalculator::Process_Default(const G4Step* step, std::vector<LArHitData>& hdata) const
// ****************************************************************************
{
  G4ThreeVector startPointinLocal, endPointinLocal;
  if(!FindIdentifier_Default(step, hdata, startPointinLocal, endPointinLocal)){
    return false;
  }

  G4double E = step->GetTotalEnergyDeposit() * step->GetTrack()->GetWeight();
  if (m_birksLaw) {
    const G4ThreeVector midpoint = (startPointinLocal + endPointinLocal) * 0.5;
    const G4double wholeStepLengthCm = step->GetStepLength() / Units::cm;
    const G4double gap = GetGapSize(midpoint);
    const G4double efield = 0.01 * m_HVHelper->GetVoltage(midpoint) / gap; // estimate Efield[KV/cm]
    E = (*m_birksLaw)(E, wholeStepLengthCm, efield);
  }

  hdata[0].energy = (this->*m_ecorr_method)(E, startPointinLocal, endPointinLocal, 0.654321);
  // 0.654321 is arbitrary number. This value is not used for this type of process.
  return true;
}

// ****************************************************************************
EnergyCalculator::EnergyCalculator(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalculatorSvcImp(name, pSvcLocator)
  , m_supportCalculator("EMECSupportCalibrationCalculator",name)
  , m_corrProp(8)
  , m_correction_type(EMEC_ECOR_CHCL1)
    // PhiGapNumber(0),
    // PhiHalfGapNumber(0),
    // HalfWaveNumber(0),
    // SignofZinHalfWave(0),
    // SignofSlopeofHalfWave(0),
    // SinPhiGap(0),
    // CosPhiGap(0),
    // ZinHalfWave(0),
    // HalfEleThickness(0),
    // ChCollWheelType(0),
    // ChCollFoldType(0),
    // PointFoldMapArea(0),
    // calculatorPhiGap(0),
    // chcollPhiGap(0),
  , m_solidtypeProp(0)
  , m_solidtype(LArG4::InnerAbsorberWheel)
  , m_zside(1)
  , m_birksLaw(nullptr)
  , m_lwc(nullptr)
  , m_HVHelper(nullptr)
    // ****************************************************************************
{
  ATH_MSG_DEBUG("constructor started");

  declareProperty("EMECHVEnable",m_DB_HV=false);
  // get power of gap in signal calculation
  declareProperty("EMECGapPower",m_GApower=1.4);
  // pick up surface_suppression_range
  declareProperty("EMECEsr",m_CHC_Esr=0.2*CLHEP::mm);
  declareProperty("EMECHVMap",m_HVMapVersion="v02");
  declareProperty("EMECChMap", m_suffix="v03");

  declareProperty("SupportCalculator",m_supportCalculator);
  declareProperty("WheelType",m_solidtypeProp);
  m_solidtypeProp.declareUpdateHandler(&EnergyCalculator::SolidTypeHandler, this);
  declareProperty("EnergyCorrection",m_corrProp);
  m_corrProp.declareUpdateHandler(&EnergyCalculator::CorrectionTypeHandler, this);
  declareProperty("zSide",m_zside);
}

void EnergyCalculator::CorrectionTypeHandler(Gaudi::Details::PropertyBase&)
{
  switch(m_corrProp.value())
    {
    case 0: m_correction_type=EMEC_ECOR_ROPT; break;
    case 1: m_correction_type=EMEC_ECOR_OFF; break;
    case 2: m_correction_type=EMEC_ECOR_GADJ; break;
    case 3: m_correction_type=EMEC_ECOR_CHCL; break;
    case 4: m_correction_type=EMEC_ECOR_GADJ_OLD; break;
    case 5: m_correction_type=EMEC_ECOR_GADJ_E; break;
    case 6: m_correction_type=EMEC_ECOR_GADJ_S; break;
    case 7: m_correction_type=EMEC_ECOR_GADJ_SE; break;
    case 8: m_correction_type=EMEC_ECOR_CHCL1; break;
    default:
      {
        std::ostringstream merr;
        merr <<
          "EnergyCalculator::CorrectionTypeHandler FATAL: invalid EnergyCorrection_t specified "
             << m_corrProp.value();
        std::cerr << merr.str() << std::endl;
        throw GaudiException(merr.str(), "EnergyCalculator::CorrectionTypeHandler", StatusCode::FAILURE);
      }
    }
}

void EnergyCalculator::SolidTypeHandler(Gaudi::Details::PropertyBase&)
{
  switch(m_solidtypeProp.value())
    {
    case  0: m_solidtype = LArG4::InnerAbsorberWheel; break;
    case  1: m_solidtype = LArG4::OuterAbsorberWheel; break;
    case  2: m_solidtype = LArG4::InnerElectrodWheel; break;
    case  3: m_solidtype = LArG4::OuterElectrodWheel; break;
    case  4: m_solidtype = LArG4::InnerAbsorberModule; break;
    case  5: m_solidtype = LArG4::OuterAbsorberModule; break;
    case  6: m_solidtype = LArG4::InnerElectrodModule; break;
    case  7: m_solidtype = LArG4::OuterElectrodModule; break;
    case  8: m_solidtype = LArG4::BackInnerBarretteWheel; break;
    case  9: m_solidtype = LArG4::BackOuterBarretteWheel; break;
    case 10: m_solidtype = LArG4::BackInnerBarretteWheelCalib; break;
    case 11: m_solidtype = LArG4::BackOuterBarretteWheelCalib; break;
    case 12: m_solidtype = LArG4::BackInnerBarretteModule; break;
    case 13: m_solidtype = LArG4::BackOuterBarretteModule; break;
    case 14: m_solidtype = LArG4::BackInnerBarretteModuleCalib; break;
    case 15: m_solidtype = LArG4::BackOuterBarretteModuleCalib; break;
    case 16: m_solidtype = LArG4::InnerGlueWheel; break;
    case 17: m_solidtype = LArG4::OuterGlueWheel; break;
    case 18: m_solidtype = LArG4::InnerLeadWheel; break;
    case 19: m_solidtype = LArG4::OuterLeadWheel; break;
    default:
      {
        std::ostringstream merr;
        merr <<
          "EnergyCalculator::SolidTypeHandler FATAL: invalid LArWheelCalculator_t specified "
             << m_solidtypeProp.value();
        std::cerr << merr.str() << std::endl;
        throw GaudiException(merr.str(), "EnergyCalculator::SolidTypeHandler", StatusCode::FAILURE);
      }
    }
}

StatusCode EnergyCalculator::initialize()
{
  m_lwc = new LArWheelCalculator(m_solidtype, m_zside);
  if(lwc()->GetisElectrode())
    {
      ATH_MSG_FATAL("energy caclculator must be of 'absorber' type,"
                    << " while 'electrode' type is requested.");
      return StatusCode::FAILURE;
    }
  s_RefzDist = lwc()->GetElecFocaltoWRP() +
    lwc()->GetdWRPtoFrontFace() + lwc()->GetWheelThickness() +
    lwc()->GetdWRPtoFrontFace() + s_LongBarThickness - s_DistOfEndofCuFromBack;

  ISvcLocator* svcLocator = Gaudi::svcLocator();

  if(m_BirksLaw)
    {
      const double Birks_LAr_density = 1.396;
      m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,m_Birksk);
    }

  // Access the GeoModelSvc:
  IGeoModelSvc *geoModel=0;
  ATH_CHECK(svcLocator->service ("GeoModelSvc",geoModel));

  // Access the geometry database:
  IRDBAccessSvc *pAccessSvc=0;
  ATH_CHECK(svcLocator->service("RDBAccessSvc",pAccessSvc));

  DecodeVersionKey larVersionKey(geoModel, "LAr");

  IRDBRecordset_ptr emecSamplingSep = pAccessSvc->getRecordsetPtr("EmecSamplingSep", larVersionKey.tag(), larVersionKey.node());
  if (emecSamplingSep->size()==0) {
    throw std::runtime_error("Cannot find the EmecSamplingSep Table");
  }

  IRDBRecordset_ptr emecGeometry = pAccessSvc->getRecordsetPtr("EmecGeometry", larVersionKey.tag(), larVersionKey.node());
  if (emecGeometry->size()==0) {
    throw std::runtime_error("Cannot find the EmecGeometry Table");
  }

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

  m_ElectrodeFanHalfThickness = LArWheelCalculator::GetFanHalfThickness(LArG4::InnerElectrodWheel);
  m_FanEleThickness = ElectrodeFanHalfThickness() * 2.;
  m_FanEleThicknessOld = 0.300*CLHEP::mm;
  m_FanEleFoldRadiusOld = 3.*CLHEP::mm;
  m_FanAbsThickness = lwc()->GetFanHalfThickness() * 2.;
  m_WaveLength = lwc()->GetActiveLength() / lwc()->GetNumberOfWaves();

  assert(fabs((*emecGeometry)[0]->getDouble("GAP0")*CLHEP::cm - s_AverageGap) < 0.000001);

  //Barrette treatment
  if(lwc()->GetisBarrette()){
    if(lwc()->type() == LArG4::OuterAbsorberWheel || lwc()->type() == LArG4::OuterAbsorberModule) {
      SetConst_OuterBarrett();
    } else{
      if(lwc()->type() == LArG4::InnerAbsorberWheel || lwc()->type() == LArG4::InnerAbsorberModule ){

        ATH_MSG_FATAL("EnergyCalculator: "
                      << "Barrett section is not (yet) prepared for "
                      << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type()));
        G4ExceptionDescription description;
        description << "EnergyCalculator: Barrett section is not (yet) prepared for solidtype="
                    << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type());
        G4Exception("EnergyCalculator", "BarretteSectionNotPrepared", FatalException, description);
      }
      else{
        ATH_MSG_FATAL("EnergyCalculator: "
                      << "Unknown solidtype:"
                      << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type()));
        G4ExceptionDescription description;
        description << "EnergyCalculator: Unknown solidtype="
                    << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type());
        G4Exception("EnergyCalculator", "UnknownSolidType", FatalException, description);
      }
    }
    m_Process_type        = &EnergyCalculator::Process_Barrett;
    m_GetGapSize_type     = &EnergyCalculator::GetGapSize_Barrett;
    m_distance_to_the_nearest_electrode_type =
      &EnergyCalculator::distance_to_the_nearest_electrode_Barrett;

    if(lwc()->GetisBarretteCalib()) {
      ATH_CHECK(m_supportCalculator.retrieve());
    }

  } else {
    m_Process_type        = &EnergyCalculator::Process_Default;
    m_GetGapSize_type     = &EnergyCalculator::GetGapSize_Default;
    m_distance_to_the_nearest_electrode_type =
      &EnergyCalculator::distance_to_the_nearest_electrode_Default;
  }

  // energy correction type from jO


  switch(m_correction_type){
  case EMEC_ECOR_GADJ:
    m_ecorr_method = &EnergyCalculator::GapAdjustment;
    break;
  case EMEC_ECOR_CHCL:
    m_ecorr_method = &EnergyCalculator::CalculateChargeCollection;
    break;
  case EMEC_ECOR_GADJ_OLD:
    m_ecorr_method = &EnergyCalculator::GapAdjustment_old;
    break;
  case EMEC_ECOR_GADJ_E:
    m_ecorr_method = &EnergyCalculator::GapAdjustment_E;
    break;
  case EMEC_ECOR_GADJ_S:
    m_ecorr_method = &EnergyCalculator::GapAdjustment_s;
    break;
  case EMEC_ECOR_GADJ_SE:
    m_ecorr_method = &EnergyCalculator::GapAdjustment__sE;
    break;
  case EMEC_ECOR_OFF:
    m_ecorr_method = &EnergyCalculator::dummy_correction_method;
    break;
  case EMEC_ECOR_CHCL1:
    if(lwc()->GetisBarrette()){
      m_ecorr_method = &EnergyCalculator::CalculateChargeCollection;
      ATH_MSG_INFO("CHCOLL energy correction is user for Barettes"
                   << " instead of CHCOLL1");
    } else {
      m_ecorr_method = &EnergyCalculator::CalculateChargeCollection1;
    }
    break;
  default:
    ATH_MSG_FATAL("EnergyCalculator: unknown correction type "
                  << m_correction_type);
    G4Exception("EnergyCalculator", "UnknownCorrectionType", FatalException,
                "EnergyCalculator: unknown correction type");
  }

  m_HVHelper = HVHelper::CreateHelper(lwc(), m_HVMapVersion, m_DB_HV);

  // if charge collection is required
  if(m_correction_type == EMEC_ECOR_CHCL ||  m_correction_type ==  EMEC_ECOR_CHCL1) {
    if(!lwc()->GetisElectrode()) {
      // get fieldmap from file
      if(s_FieldMapsRead == false) {
        s_ChCollInner.FieldMapPrepared=false;
        s_ChCollOuter.FieldMapPrepared=false;
        // Determine which version of the file by examining the user option.
        //if(m_suffix.empty()) FieldMapVersion = "v00";
        //else FieldMapVersion = m_suffix;
        s_FieldMapVersion = m_suffix.empty() ? "v00" : m_suffix;

        ATH_MSG_DEBUG("EnergyCalculator: field map version = " << s_FieldMapVersion);

        IniGeomforFieldMaps();

        // Now we have to get the full path to the file.  In
        // stand-alone, we use AFS.  In Athena, we use
        // PathResolver.
        const G4String FieldMapFileName   = "ec.fieldz" + m_suffix;
        const G4String FieldMapPath       = "LArG4EC";
        const G4String partialPath        = FieldMapPath + "/" + FieldMapFileName;
        const G4String FieldMapLocation   = PathResolver::find_file(partialPath, "ATLASCALDATA");

        LoadFieldMaps(FieldMapLocation.c_str());
        s_FieldMapsRead=true;
      } // finish to read the fieldmap

      // prepare the fieldmaps for further calculation

      if(s_FieldMapVersion != "v00"){
        m_FanEleThicknessOld  = FanEleThickness();
        m_FanEleFoldRadiusOld = lwc()->GetFanFoldRadius();
      }

      //	if(lwc()->GetisInner()) ChCollWheelType=&ChCollInner;
      //	else        ChCollWheelType=&ChCollOuter;
      m_ChCollWheelType = lwc()->GetisInner() ? &s_ChCollInner : &s_ChCollOuter;

      if(ChCollWheelType()->FieldMapPrepared == false){
        PrepareFieldMap(m_ChCollWheelType);
        m_ChCollWheelType->FieldMapPrepared = true;
      }
    } else {
      //  electrode type
      ATH_MSG_FATAL("EnergyCalculator: "
                    << "Charge Collection cannot be prepared for "
                    << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type()));
      G4ExceptionDescription description;
      description << G4String("EnergyCalculator: ") +
        "Charge Collection cannot be prepared for solidtype=" +
        LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type());
      G4Exception("EnergyCalculator", "IncorrectSolidType", FatalException, description);
    }
  }  // end if charge collection is required
  ATH_MSG_DEBUG("EnergyCalculator parameters: " << "correction type " << m_correction_type );
  ATH_MSG_DEBUG("GA power = " << m_GApower
                << "energy suppression range = " << m_CHC_Esr / CLHEP::mm << " [mm]"
                << "\n"
                << "default method for computation of folding angle (param) - compiled in");
  // Aug 2007 AMS, lost Aug 2008, restored May 2009
  m_electrode_calculator = 0;
  if(m_correction_type == EMEC_ECOR_CHCL1 && !lwc()->GetisBarrette()){
    LArG4::LArWheelCalculator_t t = LArG4::OuterElectrodWheel;
    if(lwc()->GetisModule()){

      if(lwc()->GetisInner()) t = LArG4::InnerElectrodModule;
      else t = LArG4::OuterElectrodModule;

    } else if(lwc()->GetisInner()) t = LArG4::InnerElectrodWheel;

    m_electrode_calculator = new LArWheelCalculator(t, lwc()->GetAtlasZside());
    if(m_electrode_calculator == 0){
      ATH_MSG_FATAL("cannot create helper electrode calculator");
      G4Exception("EnergyCalculator", "NoElectrodeCalculator", FatalException,
                  "cannot create helper electrode calculator");
    }
    ATH_MSG_DEBUG("helper electrode calculator created ("
                  << LArWheelCalculator::LArWheelCalculatorTypeString(t) << ")");
  }
  return StatusCode::SUCCESS;
} // end of EnergyCalculator::initialize

G4double EnergyCalculator::CalculateChargeCollection1(
                                                      G4double a_energy,
                                                      const G4ThreeVector &p1,
                                                      const G4ThreeVector &p2,
                                                      G4double /*Barret_PhiStart*/) const
{
  if(!lwc()->GetisBarrette()){   // should be wheel
    G4double current1 = GetCurrent1(p1, p2, a_energy);
#ifdef DEBUG_CHCL
    s_CHCEtotal += a_energy;
    s_CHCStotal += current1;


    ATH_MSG_DEBUG(" chcoll1: edep, current= " << a_energy << " " << current1
                  << " CHCEtotal= " << CHCEtotal << " CHCStotal= " << CHCStotal
                  << " CHCIprint= " << CHCIprint << " CHCEbad= " << CHCEbad
                  << " ebad/etotal= " << CHCEbad / CHCEtotal);
#endif
    return current1;
  }
  return a_energy;
}

// ****************************************************************************
G4double EnergyCalculator::CalculateChargeCollection(
                                                     G4double a_energy,
                                                     const G4ThreeVector &a_startPoint,
                                                     const G4ThreeVector &a_endPoint,
                                                     G4double Barret_PhiStart) const
// ****************************************************************************
{
  // std::cout<<"*** CalculateChargeCollection is called, a_energy="<<a_energy
  //       <<std::endl;

  ATH_MSG_DEBUG("starting CalculateChargeCollection:"
                << "a_energy   = " << a_energy
                << ", startPoint = " << MSG_VECTOR(a_startPoint)
                << ", endPoint = " << MSG_VECTOR(a_endPoint)
                << ", wheel type: "
                << LArWheelCalculator::LArWheelCalculatorTypeString(lwc()->type())
                << " isBarrette = " << lwc()->GetisBarrette()
                << " isBarretteCalib = " << lwc()->GetisBarretteCalib());

  //	m_calculatorPhiGap =
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

  const G4double current = GetCurrent(pStart, pEnd, a_energy, Barret_PhiStart);

#ifdef DEBUG_CHCL
  s_CHCEtotal=s_CHCEtotal+a_energy;
  s_CHCStotal=s_CHCStotal+current;

  ATH_MSG_DEBUG(" isBarrett="<<lwc()->GetisBarrette()<<" isInner="<<lwc()->GetisInner()
                <<" chcoll: edep, current= "<<a_energy<<" "<<current
                <<" CHCEtotal= "<<s_CHCEtotal<<" CHCStotal= "<<s_CHCStotal
                <<" CHCIprint= "<<s_CHCIprint<<" CHCEbad= "<<s_CHCEbad
                <<" ebad/etotal= "<<s_CHCEbad/s_CHCEtotal);
#endif

  return current;
}

// ****************************************************************************
// The static arrays that describe the endcap geometry.  Mostly these
// were copied from the ATLAS G3 code.

// The values of eta, phi, and z determine which "compartment" (a
// sampling and region) the point is in.  The following structure and
// table define constants for each compartment.

// Someday, the following table should be determined from the detector
// database and not hard-coded.

const EnergyCalculator::geometry_t EnergyCalculator::s_geometry[] =
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
                                                ) const
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

  G4int compartment = 0;

  if(lwc()->GetisInner())
    {
      G4int ipad = G4int((eta - 2.5) / 0.1);
      if(ipad > 6 || ipad < 0){
        ATH_MSG_WARNING("FindIdentifier: invalid hit, eta = "
                        << eta << ", ipad=" << ipad);
        validhit=false;
        if (ipad<0) ipad=0;
        if (ipad>6) ipad=6;
      }
      if(z < ziw[ipad]) compartment = 1;
      else              compartment = 2;
    } else { // !isInner
    G4int ipad = G4int((eta - 1.4) / 0.025);
    if(ipad < 0) ipad = 0;   // first electrode starts below eta=1.4
    if(ipad > 43){
      ATH_MSG_WARNING("FindIdentifier: invalid hit, eta = "
                      << eta << ", ipad = " << ipad);
      validhit=false;
      ipad=43;
    }
    if(z < zsep12[ipad]){
      if     (eta > 2.4)   compartment = 3;
      else if(eta > 2.0)   compartment = 4;
      else if(eta > 1.8)   compartment = 5;
      else if(eta > 1.5)   compartment = 6;
      else if(eta > 1.425) compartment = 7;
      else                 compartment = 10;
    } else {
      if(z < zsep23[ipad/2]){
        if(eta > 1.425) compartment = 8;
        else            compartment = 11;
      } else                  compartment = 9;
    }
  }
  // if compartment==0, then none of the above "if's" were resolved.
  assert(compartment > 0);

  G4int c = compartment - 1;

  // There were some differences between the definition of a
  // "compartment" in the G3 study and the assignment of
  // sampling/region in "atlnot/Communication/com-soft-2001-002".  The
  // following "if"s took compartments that did not exist separately
  // in the G3 study and combined them.  The following statements
  // are now obsolete and are commented out, but they're left in place
  // in case we have to do a study with the old G3 numbers.

  // if ( compartment == 10 ) compartment = 7;
  // if ( compartment == 11 ) compartment = 8;


  // 19-04-2007 AMS    use constant m_AtlasZside obtained in constructor
  // zSide is negative if z<0.
  //	m_AtlasZside = s_geometry[c].zSide;
  //	if(p.z() < 0.) m_AtlasZside = -m_AtlasZside;
  //	G4int atlasside = m_AtlasZside;
  const G4int atlasside = lwc()->GetAtlasZside() * s_geometry[c].zSide;

  G4int sampling = s_geometry[c].sampling;
  G4int region   = s_geometry[c].region;

  G4int etaBin = G4int(eta * s_geometry[c].etaScale - s_geometry[c].etaOffset);

  if(etaBin < 0 || etaBin > s_geometry[c].maxEta) {
    validhit=false;
    if (etaBin<0) etaBin=0;
    if (etaBin>s_geometry[c].maxEta) etaBin=s_geometry[c].maxEta;
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

  switch(compartment){

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

  if(compartment == 10 || compartment == 7 ){

    eta_max  = (s_geometry[c].etaOffset+etaBin+1.)/ s_geometry[c].etaScale;
    dist=DistanceToEtaLine(pforcell,eta_max);
    //      assert(dist >= 0.);
    if (dist>=0. && dist <= DistMaxS1) cnew = c+1;
  }

  // outer wheel, S3:

  else if(compartment == 9 ) {

    eta_min=(s_geometry[c].etaOffset+etaBin)   / s_geometry[c].etaScale;
    eta_max=(s_geometry[c].etaOffset+etaBin+1.)/ s_geometry[c].etaScale;
    dist_min =DistanceToEtaLine(pforcell,eta_min);
    dist_max =DistanceToEtaLine(pforcell,eta_max);

    //      assert(dist_min <=0.);
    //      assert(dist_max >=0.);

    if(dist_min<=0. && -dist_min <= DistMax) cnew = c-1;
    else {
      if(etaBin != s_geometry[c].maxEta) {
        if(dist_max>=0. && dist_max <= DistMax) cnew = c-1;
      }
      else{ // close to the crack region between wheels, etaBin=maxEta;
        if(dist_max >= DistMinatEdgeinCrack &&
           dist_max <= DistMaxatEdgeinCrack)   cnew=c-1;
      }
    }
  }

  // inner wheel S2:

  else if(compartment == 2) {

    eta_min=(s_geometry[c].etaOffset+etaBin)/s_geometry[c].etaScale;
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
    compartment = c + 1;
    sampling = s_geometry[c].sampling;
    region   = s_geometry[c].region;
    etaBin = G4int(eta * s_geometry[c].etaScale - s_geometry[c].etaOffset);
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

  G4int phiBin = phiGap / s_geometry[c].gapsPerBin;

  if(atlasside < 0){
    // The following formula assumes that the z<0 endcap was derived
    // from the positive endcap by rotateY(180.*deg)
    // 29-March-2004 ML
    phiBin = (s_geometry[c].maxPhi - 1)/2 - phiBin;
    if(phiBin < 0) phiBin += s_geometry[c].maxPhi + 1;
  }

  assert(phiBin >= 0);
  assert(phiBin <= s_geometry[c].maxPhi);
  if(phiBin<0) {phiBin=0;validhit=false;}
  if(phiBin>s_geometry[c].maxPhi) {
    phiBin=s_geometry[c].maxPhi;
    validhit=false;
  }
  if(etaBin > s_geometry[c].maxEta){
    ATH_MSG_WARNING("FindIdentifier: invalid hit, etaBin = "
                    << etaBin << " > geometry[" << c << "].maxEta="
                    << s_geometry[c].maxEta);
    etaBin=s_geometry[c].maxEta;
    validhit=false;
  }
  if(etaBin < 0){
    ATH_MSG_WARNING("FindIdentifier: invalid hit, etaBin < 0");
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
  hdata[0].time = timeOfFlight/Units::ns - p.mag()/CLHEP::c_light/Units::ns;

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
  return(fabs(d3) - ElectrodeFanHalfThickness());
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
           - lwc()->GetFanHalfThickness() + fabs(d3) - ElectrodeFanHalfThickness());
  } else {
    return(fabs(d1)
           - lwc()->GetFanHalfThickness() + fabs(d3) - ElectrodeFanHalfThickness());
  }
}
