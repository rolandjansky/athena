/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWCalibTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

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


  // sTGC conversion from potential to PDO for VMM1 configuration, mV*1.0304 + 59.997; from Shandong cosmics tests
  // link to study outlining conversion https://doi.org/10.1016/j.nima.2019.02.061
  static constexpr double const& sTGC_chargeToPdoSlope = 1.0304;
  static constexpr double const& sTGC_chargeToPdoOffset = 59.997;
  // sTGC conversion from PDO to potential
  static constexpr double const& sTGC_pdoToChargeSlope = 0.97050;
  static constexpr double const& sTGC_pdoToChargeOffset = -54.345;

  // MM conversion from charge to PDO
  static constexpr double const& MM_chargeToPdoSlope = 9./6421;
  static constexpr double const& MM_chargeToPdoOffset = 0;
  // MM conversion from PDO to charge
  static constexpr double const& MM_pdoToChargeSlope = 713.4;
  static constexpr double const& MM_pdoToChargeOffset = 0;


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

Muon::NSWCalibTool::NSWCalibTool(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareInterface<INSWCalibTool>(this);

  declareProperty("DriftVelocity", m_vDrift = 0.047, "Drift Velocity");
  declareProperty("TimeResolution", m_timeRes = 25., "Time Resolution");
  declareProperty("longDiff",m_longDiff=0.019); //mm/mm
  declareProperty("transDiff",m_transDiff=0.036); //mm/mm
  declareProperty("ionUncertainty",m_ionUncertainty=4.0); //ns
  declareProperty("timeOffset", m_timeOffset = -100); //ns                       
  declareProperty("GasMixture", m_gasMixture = "ArCo2_937");
}


StatusCode Muon::NSWCalibTool::initialize()
{
  ATH_MSG_DEBUG("In initialize()");
  ATH_CHECK(m_idHelperSvc.retrieve());
  if ( !(m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc() ) ) {
    ATH_MSG_ERROR("MM or STGC not part of initialized detector layout");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  ATH_CHECK(m_muDetMgrKey.initialize());
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

  /// magnetic field
  MagField::AtlasFieldCache fieldCache;
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, Gaudi::Hive::currentContext()};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (!fieldCondObj) {
    ATH_MSG_ERROR("doDigitization: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return StatusCode::FAILURE;
  }
  fieldCondObj->getInitializedCache(fieldCache);
  Amg::Vector3D magneticField;
  fieldCache.getField(globalPos.data(), magneticField.data());

  /// get the component parallel to to the eta strips (same used in digitization)
  double phi    = globalPos.phi();
  double bfield = (magneticField.x()*std::sin(phi)-magneticField.y()*std::cos(phi))*1000.;

  /// swap sign depending on the readout side
  int gasGap = m_idHelperSvc->mmIdHelper().gasGap(prepData->identify());
  bool changeSign = ( globalPos.z() > 0. ? (gasGap==1 || gasGap==3) : (gasGap==2 || gasGap==4) );
  if (changeSign) bfield = -bfield;

  //// sign of the lorentz angle matches digitization - angle is in radians
  double lorentzAngle = (bfield>0. ? 1. : -1.)*m_lorentzAngleFunction->Eval(std::abs(bfield)) * toRad;

  /// loop over prepData strips
  for (unsigned int i = 0; i < prepData->stripNumbers().size(); i++){
    double time = prepData->stripTimes().at(i);
    double charge = prepData->stripCharges().at(i);
    NSWCalib::CalibratedStrip calibStrip;
    ATH_CHECK(calibrateStrip(time, charge, lorentzAngle, calibStrip));
    calibClus.push_back(calibStrip);
  }
  return StatusCode::SUCCESS;
}



StatusCode Muon::NSWCalibTool::calibrateStrip(const double time, const double charge, const double lorentzAngle, NSWCalib::CalibratedStrip& calibStrip) const {
  calibStrip.charge = charge;
  calibStrip.time = time;

  double vDriftCorrected = m_vDrift * std::cos(lorentzAngle);
  calibStrip.distDrift = vDriftCorrected * calibStrip.time;

  /// transversal and longitudinal components of the resolution
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2);
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * vDriftCorrected, 2)
    + std::pow(m_longDiff * calibStrip.distDrift, 2);
  calibStrip.dx = std::sin(lorentzAngle) * calibStrip.time * m_vDrift;
  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::calibrateStrip(const Muon::MM_RawData* mmRawData, NSWCalib::CalibratedStrip& calibStrip) const
{
  Identifier rdoId = mmRawData->identify();

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  //get globalPos
  Amg::Vector3D globalPos;
  const MuonGM::MMReadoutElement* detEl = muDetMgr->getMMReadoutElement(rdoId);
  detEl->stripGlobalPosition(rdoId,globalPos);

  calibStrip.charge = mmRawData->charge();
  calibStrip.time = mmRawData->time() - globalPos.norm() * reciprocalSpeedOfLight + m_timeOffset;
  calibStrip.identifier = mmRawData->identify();

  calibStrip.distDrift = m_vDrift * calibStrip.time;
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2);
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * m_vDrift, 2)
    + std::pow(m_longDiff * calibStrip.distDrift, 2);

  return StatusCode::SUCCESS;
}

StatusCode Muon::NSWCalibTool::calibrateStrip(const Muon::STGC_RawData* sTGCRawData, NSWCalib::CalibratedStrip& calibStrip) const
{
  Identifier rdoId = sTGCRawData->identify();

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  //get globalPos
  Amg::Vector3D globalPos;
  const MuonGM::sTgcReadoutElement* detEl = muDetMgr->getsTgcReadoutElement(rdoId);
  detEl->stripGlobalPosition(rdoId,globalPos);

  calibStrip.charge =sTGCRawData->charge();
  calibStrip.time = sTGCRawData->time() - globalPos.norm() * reciprocalSpeedOfLight + m_timeOffset;
  calibStrip.identifier = sTGCRawData->identify();

  return StatusCode::SUCCESS;
}



double Muon::NSWCalibTool::pdoToCharge(const int pdoCounts, const Identifier& stripID) const {
  double charge = 0;
  if (m_idHelperSvc->isMM(stripID)){
    charge = pdoCounts * MM_pdoToChargeSlope + MM_pdoToChargeOffset;
  }
  else if (m_idHelperSvc->issTgc(stripID)){
    charge = pdoCounts * sTGC_pdoToChargeSlope + sTGC_pdoToChargeOffset;
    charge = 0.001 * charge;
  }
  return charge;
}

int Muon::NSWCalibTool::chargeToPdo(const float charge, const Identifier& stripID) const {
  int pdoCounts = 0;
  if (m_idHelperSvc->isMM(stripID)){
    pdoCounts = charge * MM_chargeToPdoSlope + MM_chargeToPdoOffset;
  }
  else if (m_idHelperSvc->issTgc(stripID)){
    double c = charge * 1000;  // VMM gain setting for conversion from charge to potential, 1mV=1fC; from McGill cosmics tests
    pdoCounts = c * sTGC_chargeToPdoSlope + sTGC_chargeToPdoOffset;
  }
  return pdoCounts;
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
