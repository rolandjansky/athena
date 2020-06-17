#include "NSWCalibTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"


namespace {
  static constexpr double const& toRad = M_PI/180;
  static constexpr double const& pitchErr = 0.425 * 0.425 / 12;
  static constexpr double const& reciprocalSpeedOfLight = 1. / Gaudi::Units::c_light; // mm/ns
  
  // since the final operation gas is not yet fixed different, different mixtures are added for studies 
  static const std::map<std::string, float> map_transDiff {{"ArCo2_937", 0.036},
                              {"ArCo2_8020", 0.019}, {"ArCo2iC4H10_9352", 0.035}};
  static const std::map<std::string, float> map_longDiff {{"ArCo2_937", 0.019},
                              {"ArCo2_8020", 0.022 }, {"ArCo2iC4H10_9352", 0.0195}};
  static const std::map<std::string, float> map_vDrift {{"ArCo2_937", 0.047},
                              {"ArCo2_8020", 0.040}, {"ArCo2iC4H10_9352", 0.045}};

  //Functional form fit to agree with Garfield simulations. Fit and parameters from G. Iakovidis
  // For now only the parametrisation for 93:7 is available
  static const std::map<std::string, std::vector<float>> map_lorentzAngleFunctionPars {
                              {"ArCo2_937", std::vector<float>{0, 58.87, -2.983, -10.62, 2.818}},
                              {"ArCo2_8020", std::vector<float>{0, 58.87, -2.983, -10.62, 2.818}},
                              {"ArCo2iC4H10_9352", std::vector<float>{0, 58.87, -2.983, -10.62, 2.818}}};

  // For now only the parametrisation for 93:7 is available
  static const std::map<std::string, float> map_interactionDensitySigma {{"ArCo2_937", 4.04 / 5.},
                                   {"ArCo2_8020", 4.04 / 5.}, {"ArCo2iC4H10_9352", 4.04 / 5.}};
  static const std::map<std::string, float> map_interactionDensityMean {{"ArCo2_937", 16.15 / 5.},
                                   {"ArCo2_8020", 16.15 / 5.}, {"ArCo2iC4H10_9352", 16.15 / 5.}};

}

Muon::NSWCalibTool::NSWCalibTool(const std::string& t,
				  const std::string& n, 
				  const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_magFieldSvc("AtlasFieldSvc",n)
{
  declareInterface<INSWCalibTool>(this);

  declareProperty("MagFieldSvc",   m_magFieldSvc, "Magnetic Field Service");
  declareProperty("DriftVelocity", m_vDrift = 0.047, "Drift Velocity");
  declareProperty("TimeResolution", m_timeRes = 25., "Time Resolution");
  declareProperty("longDiff",m_longDiff=0.019); //mm/mm
  declareProperty("transDiff",m_transDiff=0.036); //mm/mm
  declareProperty("ionUncertainty",m_ionUncertainty=4.0); //ns
  declareProperty("timeOffset", m_timeOffset = -100); //ns                       
  declareProperty("MuonIdHelperTool", m_idHelperTool);
  declareProperty("GasMixture", m_gasMixture = "ArCo2_937");
}


StatusCode Muon::NSWCalibTool::initialize()
{

  ATH_MSG_DEBUG("In initialize()");
  ATH_CHECK(m_magFieldSvc.retrieve());
  // initialize the MuonIdHelperTool and check the configuration
  ATH_CHECK(m_idHelperTool.retrieve());

  if ( !(m_idHelperTool->HasMM() && m_idHelperTool->HasSTgc() ) ) {
    ATH_MSG_ERROR("MuonIdHelperTool not properly configured, missing MM or STGC");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(initializeGasProperties());
  return StatusCode::SUCCESS;
}

StatusCode Muon::NSWCalibTool::initializeGasProperties() {
  if (map_vDrift.count(m_gasMixture) == 0) {
    ATH_MSG_FATAL("Configured Micromegas with unkown gas mixture: " << m_gasMixture);
    return StatusCode::FAILURE;
  }

  m_vDrift = map_vDrift.find(m_gasMixture)->second;
  m_transDiff = map_transDiff.find(m_gasMixture)->second;
  m_longDiff = map_longDiff.find(m_gasMixture)->second;
  m_interactionDensitySigma = map_interactionDensitySigma.find(m_gasMixture)->second;
  m_interactionDensityMean =  map_interactionDensityMean.find(m_gasMixture)->second;

  m_lorentzAngleFunction = new TF1("lorentzAngleFunction", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x", 0, 2);
  for (uint i_par = 0; i_par < map_lorentzAngleFunctionPars.find(m_gasMixture)->second.size(); i_par++) {
    m_lorentzAngleFunction -> SetParameter(i_par, map_lorentzAngleFunctionPars.find(m_gasMixture)->second[i_par]);
  }

  return StatusCode::SUCCESS;
}

StatusCode Muon::NSWCalibTool::calibrateClus(const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos, std::vector<NSWCalib::CalibratedStrip>& calibClus) const {
  Amg::Vector3D magneticField;
  m_magFieldSvc->getField(&globalPos,&magneticField);

  /// get the component parallel to to the eta strips (same used in digitization)
  double phi    = globalPos.phi();
  double bfield = (magneticField.x()*std::sin(phi)-magneticField.y()*std::cos(phi))*1000.;

  //// swap sign depending on the readout side
  int gasGap = m_idHelperTool->mmIdHelper().gasGap(prepData->identify());
  bool changeSign = ( globalPos.z() > 0. ? (gasGap==1 || gasGap==3) : (gasGap==2 || gasGap==4) );
  if (changeSign) bfield = -bfield;

  //// sign of the lorentz angle matches digitization - angle is in radians
  double lorentzAngle = (bfield>0. ? 1. : -1.)*m_lorentzAngleFunction->Eval(std::abs(bfield)) * toRad;

  /// loop over prepData strips
  Muon::MMPrepData calibPrepData;
  for (unsigned int i = 0; i < prepData->stripNumbers().size(); i++){
    double time = prepData->stripTimes().at(i);
    double charge = prepData->stripCharges().at(i);
    double localPos = prepData->stripNumbers().at(i);
    NSWCalib::CalibratedStrip calibStrip;
    calibrateStrip(time, charge, globalPos, lorentzAngle, calibStrip);
    calibClus.push_back(calibStrip);
  }
  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::calibrateStrip(const double time, const double charge, const Amg::Vector3D& globalPos, const double lorentzAngle, NSWCalib::CalibratedStrip& calibStrip) const {
  calibStrip.charge = charge;
  calibStrip.time = time - globalPos.norm() * reciprocalSpeedOfLight + m_timeOffset;

  double vDriftCorrected = m_vDrift * std::cos(lorentzAngle);
  calibStrip.distDrift = vDriftCorrected * calibStrip.time;

  /// transversal and longitudinal components of the resolution
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2);
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * vDriftCorrected, 2)
    + std::pow(m_longDiff * calibStrip.distDrift, 2);
  calibStrip.dx = std::sin(lorentzAngle) * calibStrip.time * m_vDrift;
  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::calibrate( const Muon::MM_RawData* mmRawData, const Amg::Vector3D& globalPos, NSWCalib::CalibratedStrip& calibStrip) const
{

  double  vDriftCorrected;
  calibStrip.charge = mmRawData->charge();
  calibStrip.time = mmRawData->time() - globalPos.norm() * reciprocalSpeedOfLight + m_timeOffset;

  calibStrip.identifier = mmRawData->identify();

  /// magnetic field
  Amg::Vector3D magneticField;
  m_magFieldSvc->getField(&globalPos,&magneticField);

  /// get the component parallel to to the eta strips (same used in digitization)
  double phi    = globalPos.phi();
  double bfield = (magneticField.x()*std::sin(phi)-magneticField.y()*std::cos(phi))*1000.;

  /// swap sign depending on the readout side
  int gasGap = m_idHelperTool->mmIdHelper().gasGap(mmRawData->identify());
  bool changeSign = ( globalPos.z() > 0. ? (gasGap==1 || gasGap==3) : (gasGap==2 || gasGap==4) );
  if (changeSign) bfield = -bfield;

  /// sign of the lorentz angle matches digitization - angle is in radians
  double lorentzAngle = (bfield>0. ? 1. : -1.)*m_lorentzAngleFunction->Eval(std::abs(bfield)) * toRad;

  vDriftCorrected = m_vDrift * std::cos(lorentzAngle);
  calibStrip.distDrift = vDriftCorrected * calibStrip.time;

  /// transversal and longitudinal components of the resolution
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2); 
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * vDriftCorrected, 2) 
    + std::pow(m_longDiff * calibStrip.distDrift, 2);
  
  calibStrip.dx = std::sin(lorentzAngle) * calibStrip.time * m_vDrift;

  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::finalize()
{
  ATH_MSG_DEBUG("In finalize()");
  m_lorentzAngleFunction->Delete();
  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::mmGasProperties(float &vDrift, float &longDiff, float &transDiff, float &interactionDensityMean, float &interactionDensitySigma, TF1* &lorentzAngleFunction) const {
  vDrift = m_vDrift;
  longDiff = m_longDiff;
  transDiff = m_transDiff;
  interactionDensityMean = m_interactionDensityMean;
  interactionDensitySigma = m_interactionDensitySigma;
  lorentzAngleFunction = m_lorentzAngleFunction;
  return StatusCode::SUCCESS;
}
