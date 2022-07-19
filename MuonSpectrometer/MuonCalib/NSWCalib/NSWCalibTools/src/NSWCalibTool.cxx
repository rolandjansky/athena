/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWCalibTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

namespace {
  constexpr double toRad = M_PI/180;
  constexpr double pitchErr = 0.425 * 0.425 / 12;
  constexpr double reciprocalSpeedOfLight = 1. / Gaudi::Units::c_light; // mm/ns
  
  // since the final operation gas is not yet fixed different, different mixtures are added for studies 
  static const std::map<std::string, float> map_transDiff {{"ArCo2_937", 0.036},
                              {"ArCo2_8020", 0.019}, {"ArCo2iC4H10_9352", 0.035}};
  static const std::map<std::string, float> map_longDiff {{"ArCo2_937", 0.019},
                              {"ArCo2_8020", 0.022 }, {"ArCo2iC4H10_9352", 0.0195}};
  static const std::map<std::string, float> map_vDrift {{"ArCo2_937", 0.047},
                              {"ArCo2_8020", 0.040}, {"ArCo2iC4H10_9352", 0.045}};


  // unit correction factors
  constexpr double const MM_electronsPerfC = 6241.;
  constexpr double const sTGC_pCPerfC = 1000.;


  //Functional form fit to agree with Garfield simulations. Fit and parameters from G. Iakovidis
  // For now only the parametrisation for 93:7 is available
  static const std::map<std::string, std::vector<float>> map_lorentzAngleFunctionPars {
                              {"ArCo2_937", std::vector<float>{0, 58.87, -2.983, -10.62, 2.818}},
                              {"ArCo2_8020", std::vector<float>{0, 58.87, -2.983, -10.62, 2.818}},
                              {"ArCo2iC4H10_9352", std::vector<float>{0, 58.87, -2.983, -10.62, 2.818}}};

  // For now only the parametrisation for 93:7 is available
  static const std::map<std::string, float> map_interactionDensitySigma {{"ArCo2_937", 4.04 / 5.},
                                   {"ArCo2_8020", 4.04 / 5.}, {"ArCo2iC4H10_9352", 4.04 / 5.}};
  static  const std::map<std::string, float> map_interactionDensityMean {{"ArCo2_937", 16.15 / 5.},
                                   {"ArCo2_8020", 16.15 / 5.}, {"ArCo2iC4H10_9352", 16.15 / 5.}};

}

Muon::NSWCalibTool::NSWCalibTool(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p) {
  declareInterface<INSWCalibTool>(this);
}


StatusCode Muon::NSWCalibTool::initialize()
{
  ATH_MSG_DEBUG("In initialize()");
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_condTdoPdoKey.initialize());
  ATH_CHECK(m_fieldCondObjInputKey.initialize( m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc() ));
  ATH_CHECK(m_muDetMgrKey.initialize( m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc() ));

  if ( m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc() ) {
    ATH_CHECK(initializeGasProperties());
  } else {
    ATH_MSG_INFO("MM or STGC not part of initialized detector layout, skipping initialization");
  }
  ATH_CHECK(initializeGasProperties());
  return StatusCode::SUCCESS;
}

StatusCode Muon::NSWCalibTool::initializeGasProperties() {
  if (!map_vDrift.count(m_gasMixture)) {
    ATH_MSG_FATAL("Configured Micromegas with unkown gas mixture: " << m_gasMixture);
    return StatusCode::FAILURE;
  }

  m_vDrift = map_vDrift.find(m_gasMixture)->second;
  m_transDiff = map_transDiff.find(m_gasMixture)->second;
  m_longDiff = map_longDiff.find(m_gasMixture)->second;
  m_interactionDensitySigma = map_interactionDensitySigma.find(m_gasMixture)->second;
  m_interactionDensityMean =  map_interactionDensityMean.find(m_gasMixture)->second;

  m_lorentzAngleFunction = std::make_unique<TF1>("lorentzAngleFunction", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x", 0, 2);
  for (uint i_par = 0; i_par < map_lorentzAngleFunctionPars.find(m_gasMixture)->second.size(); i_par++) {
    m_lorentzAngleFunction -> SetParameter(i_par, map_lorentzAngleFunctionPars.find(m_gasMixture)->second[i_par]);
  }

  return StatusCode::SUCCESS;
}

const NswCalibDbTimeChargeData* Muon::NSWCalibTool::getCalibData(const EventContext& ctx) const {
  // set up pointer to conditions object
  SG::ReadCondHandle<NswCalibDbTimeChargeData> readTdoPdo{m_condTdoPdoKey, ctx};
  if(!readTdoPdo.isValid()){
    ATH_MSG_ERROR("Cannot find conditions data container for TDOs and PDOs!");
    return nullptr;
  } 
  return readTdoPdo.cptr();
}

StatusCode Muon::NSWCalibTool::calibrateClus(const EventContext& ctx, const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos, std::vector<NSWCalib::CalibratedStrip>& calibClus) const {

  /// magnetic field
  MagField::AtlasFieldCache fieldCache;
  if (!loadMagneticField(ctx, fieldCache)) return StatusCode::FAILURE;
  Amg::Vector3D magneticField{0.,0.,0.};
  fieldCache.getField(globalPos.data(), magneticField.data());

  /// get the component parallel to to the eta strips (same used in digitization)
  double phi    = globalPos.phi();
  double bfield = (magneticField.x()*std::sin(phi)-magneticField.y()*std::cos(phi))*1000.;

  /// swap sign depending on the readout side
  int gasGap = m_idHelperSvc->mmIdHelper().gasGap(prepData->identify());
  bool changeSign = ( globalPos.z() < 0. ? (gasGap==1 || gasGap==3) : (gasGap==2 || gasGap==4) );
  if (changeSign) bfield = -bfield;

  //// sign of the lorentz angle matches digitization - angle is in radians
  double lorentzAngle = (bfield>0. ? 1. : -1.)*m_lorentzAngleFunction->Eval(std::abs(bfield)) * toRad;

  /// loop over prepData strips
  for (unsigned int i = 0; i < prepData->stripNumbers().size(); ++i){
    Identifier id = prepData->rdoList().at(i);
    double time = prepData->stripTimes().at(i);
    double charge = prepData->stripCharges().at(i);
    NSWCalib::CalibratedStrip calibStrip;
    ATH_CHECK(calibrateStrip(id,time, charge, lorentzAngle, calibStrip));
    calibClus.push_back(std::move(calibStrip));
  }
  return StatusCode::SUCCESS;
}

StatusCode Muon::NSWCalibTool::calibrateStrip(const Identifier& id, const double time, const double charge, const double lorentzAngle, NSWCalib::CalibratedStrip& calibStrip) const {

  //get local positon
  Amg::Vector2D locPos;
  if(!localStripPosition(id,locPos)) {
    ATH_MSG_WARNING("Failed to retrieve local strip position");
    return StatusCode::FAILURE;
  }
  
  calibStrip.identifier = id;
  calibStrip.charge = charge;
  calibStrip.time = time;

  double vDriftCorrected = m_vDrift * std::cos(lorentzAngle);
  calibStrip.distDrift = vDriftCorrected * calibStrip.time;

  /// transversal and longitudinal components of the resolution
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2);
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * vDriftCorrected, 2)
    + std::pow(m_longDiff * calibStrip.distDrift, 2);
  calibStrip.dx = std::sin(lorentzAngle) * calibStrip.time * m_vDrift;  
  calibStrip.locPos = Amg::Vector2D(locPos.x() + calibStrip.dx, locPos.y()); 
  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::calibrateStrip(const EventContext& ctx, const Muon::MM_RawData* mmRawData, NSWCalib::CalibratedStrip& calibStrip) const {  
 
  const Identifier rdoId = mmRawData->identify();
  //get local postion
  Amg::Vector2D locPos{0,0};
  if(!localStripPosition(rdoId,locPos)) {
    ATH_MSG_WARNING("Failed to retrieve local strip position");
    return StatusCode::FAILURE;
  }

  // MuonDetectorManager from the conditions store
  /// DO WE REALLY NEED THE DETECTOR MANAGER HERE? 
  /// WHY DOES PREPDATA->surface().associatedDetEle not work?
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey, ctx};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  //get globalPos
  Amg::Vector3D globalPos{0.,0.,0.};
  const MuonGM::MMReadoutElement* detEl = muDetMgr->getMMReadoutElement(rdoId);
  detEl->stripGlobalPosition(rdoId,globalPos);

  // RDO has values in counts for both simulation and data
  double time{-DBL_MAX}, charge{-DBL_MAX};
  tdoToTime  (ctx, mmRawData->timeAndChargeInCounts(), mmRawData->time  (), rdoId, time  , mmRawData->relBcid()); 
  pdoToCharge(ctx, mmRawData->timeAndChargeInCounts(), mmRawData->charge(), rdoId, charge                      );

  calibStrip.charge     = charge;
  // historically the peak time is included in the time determined by the MM digitization and therefore added back in the tdoToTime function
  // in order to not break the RDO to digit conversion needed for the trigger and the overlay
  calibStrip.time       = time - globalPos.norm() * reciprocalSpeedOfLight - mmPeakTime();
  calibStrip.identifier = rdoId;

  ATH_MSG_DEBUG("Calibrating RDO " << m_idHelperSvc->toString(rdoId) << "with pdo: " << mmRawData->charge() << " tdo: "<< mmRawData->time() << " relBCID "<< mmRawData->relBcid() << " charge and time in counts  " <<
                         mmRawData->timeAndChargeInCounts() << " isData "<< m_isData  << " to charge: " << calibStrip.charge << " electrons  time after corrections " << calibStrip.time << " ns  time before corrections "<< time << "ns");


  //get stripWidth
  detEl->getDesign(rdoId)->channelWidth(locPos); // positon is not used for strip width 

  calibStrip.distDrift = m_vDrift * calibStrip.time;
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2);
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * m_vDrift, 2)
                              + std::pow(m_longDiff * calibStrip.distDrift, 2);

  calibStrip.locPos = locPos;

  return StatusCode::SUCCESS;
}

StatusCode Muon::NSWCalibTool::calibrateStrip(const EventContext& ctx, const Muon::STGC_RawData* sTGCRawData, NSWCalib::CalibratedStrip& calibStrip) const {

  Identifier rdoId = sTGCRawData->identify();

  // MuonDetectorManager from the conditions store
  /// AGAIN WHY DO WE NEED THE DETMGR?
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey, ctx};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  //get globalPos
  Amg::Vector3D globalPos{0.,0.,0.};
  const MuonGM::sTgcReadoutElement* detEl = muDetMgr->getsTgcReadoutElement(rdoId);
  detEl->stripGlobalPosition(rdoId,globalPos);
  
  //get local postion
  Amg::Vector2D locPos{0.,0.};
  if(!localStripPosition(rdoId,locPos)) {
    ATH_MSG_WARNING("Failed to retrieve local strip position");
    return StatusCode::FAILURE;
  }

  // RDO has values in counts for both simulation and data
  double time{-DBL_MAX}, charge{-DBL_MAX};
  tdoToTime  (ctx, sTGCRawData->timeAndChargeInCounts(), sTGCRawData->time  (), rdoId, time  , sTGCRawData->bcTag()); 
  pdoToCharge(ctx, sTGCRawData->timeAndChargeInCounts(), sTGCRawData->charge(), rdoId, charge                      );
  if(sTGCRawData->timeAndChargeInCounts()){
    calibStrip.charge     = charge  * sTGC_pCPerfC;
  } else {
    calibStrip.charge     = charge;
  }
  calibStrip.time       = time - stgcPeakTime();
  calibStrip.identifier = rdoId;
  calibStrip.locPos = locPos;
  return StatusCode::SUCCESS;
  
}
bool Muon::NSWCalibTool::loadMagneticField(const EventContext& ctx, MagField::AtlasFieldCache& fieldCache) const {
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  if (!readHandle.isValid()) {
      ATH_MSG_ERROR("doDigitization: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return false;
  }
  readHandle.cptr()->getInitializedCache(fieldCache);
  return true;
}
StatusCode Muon::NSWCalibTool::distToTime(const EventContext& ctx, const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos, const std::vector<double>& driftDistances, std::vector<double>& driftTimes) const {
  /// retrieve the magnetic field
  MagField::AtlasFieldCache fieldCache;
  if (!loadMagneticField(ctx, fieldCache)) return StatusCode::FAILURE;  
  Amg::Vector3D magneticField{0.,0.,0.};
  fieldCache.getField(globalPos.data(), magneticField.data());

  /// get the component parallel to to the eta strips (same used in digitization)
  const double phi    = globalPos.phi();
  double bfield = (magneticField.x()*std::sin(phi)-magneticField.y()*std::cos(phi))*1000.;

  /// swap sign depending on the readout side  
  int gasGap = m_idHelperSvc->mmIdHelper().gasGap(prepData->identify());
  bool changeSign = ( globalPos.z() < 0. ? (gasGap==1 || gasGap==3) : (gasGap==2 || gasGap==4) );
  if (changeSign) bfield = -bfield;
  double cos2_lorentzAngle = std::pow(std::cos ( (bfield>0. ? 1. : -1.)*m_lorentzAngleFunction->Eval(std::abs(bfield)) * toRad), 2);
  /// loop over drift distances                                                                                             
  for (const double dist : driftDistances){
    double time = dist / (m_vDrift*cos2_lorentzAngle);
    driftTimes.push_back(time);
  }
  return StatusCode::SUCCESS;

}

bool
Muon::NSWCalibTool::chargeToPdo(const EventContext& ctx, const double charge, const Identifier& chnlId, int& pdo) const {
  const NswCalibDbTimeChargeData* tdoPdoData = getCalibData(ctx);
  if (!tdoPdoData) {
    pdo = 0;
    return false;  
  }
  const TimeCalibConst& calib = tdoPdoData->getCalibForChannel(TimeCalibType::PDO, chnlId);
  if (!calib.is_valid) {
    pdo = 0;
    return false; 
  }
  double c = charge;
  if     (m_idHelperSvc->isMM  (chnlId)) c /= MM_electronsPerfC;
  else if(m_idHelperSvc->issTgc(chnlId)) c *= sTGC_pCPerfC;
  else {
    pdo = 0;
    return false;
  }
  pdo = c * calib.slope + calib.intercept;
  return true;
}

bool
Muon::NSWCalibTool::pdoToCharge(const EventContext& ctx, const bool inCounts, const int pdo, const Identifier& chnlId, double& charge) const {  
  if(!inCounts){
    charge = pdo;
    return true;
  }
  const NswCalibDbTimeChargeData* tdoPdoData = getCalibData(ctx);
  if (!tdoPdoData) {
    charge =0.;
    return false;  
  }
  const TimeCalibConst& calib = tdoPdoData->getCalibForChannel(TimeCalibType::PDO, chnlId);
  if (!calib.is_valid) {
    charge = 0.;
    return false; 
  }
  charge = (pdo-calib.intercept)/calib.slope;
  if     (m_idHelperSvc->isMM  (chnlId)) charge *= MM_electronsPerfC;
  else if(m_idHelperSvc->issTgc(chnlId)) charge /= sTGC_pCPerfC;
  else return false;
  return true;
}

bool 
Muon::NSWCalibTool::timeToTdo(const EventContext& ctx, const double time, const Identifier& chnlId, int& tdo, int& relBCID) const {
  const NswCalibDbTimeChargeData* tdoPdoData = getCalibData(ctx);
  if (!tdoPdoData) return false;
  if     (m_idHelperSvc->isMM  (chnlId)) return timeToTdoMM  (tdoPdoData, time, chnlId, tdo, relBCID);
  else if(m_idHelperSvc->issTgc(chnlId)) return timeToTdoSTGC(tdoPdoData, time, chnlId, tdo, relBCID);
  return false;
}

bool 
Muon::NSWCalibTool::timeToTdoMM(const NswCalibDbTimeChargeData* tdoPdoData, const double time, const Identifier& chnlId, int& tdo, int& relBCID) const {
  const double t   = time - m_mmPeakTime - m_mmLatencyMC; // subtract peaking time first! This is not supossed to run on data ever only needed for the RDO->Digit step
  const TimeCalibConst& calib = tdoPdoData->getCalibForChannel(TimeCalibType::TDO, chnlId);
  if (!calib.is_valid) return false;
  double tdoTime = -999.9;
  constexpr double lowerBound = Muon::MM_RawData::s_lowerTimeBound;
  for(int i_relBCID=0; i_relBCID<Muon::MM_RawData::s_BCWindow; i_relBCID++){
    if(t >= lowerBound+i_relBCID*25 && t < (lowerBound+25)+i_relBCID*25){
      tdoTime = i_relBCID*25 - t;
      relBCID = i_relBCID;
      break;
    }
  }
  if(tdoTime < lowerBound) {
    tdo = relBCID = 0;
    return false;
  }
  tdo = tdoTime*calib.slope + calib.intercept;
  return true;
}

bool 
Muon::NSWCalibTool::timeToTdoSTGC(const NswCalibDbTimeChargeData* tdoPdoData, const double time, const Identifier& chnlId, int& tdo, int& relBCID) const {
  const double t   = time - m_stgcPeakTime - m_stgcLatencyMC; // subtract peaking time and latency first! This is not supossed to run on data ever only needed for the RDO->Digit step
  const TimeCalibConst& calib = tdoPdoData->getCalibForChannel(TimeCalibType::TDO, chnlId);
  double tdoTime = -999.9;
  const double lowerBound = Muon::STGC_RawData::s_lowerTimeBound - m_stgcLatencyMC; // this is not supossed to run on data ever, only needed for the RDO->Digit step
  for(int i_relBCID=0; i_relBCID<Muon::STGC_RawData::s_BCWindow; ++i_relBCID){
    if(t >= lowerBound+i_relBCID*25 && t < (lowerBound+25)+i_relBCID*25){
      tdoTime = i_relBCID*25 - t;
      relBCID = i_relBCID;
      break;
    }
  }
  if(tdoTime < lowerBound) {
    tdo = relBCID = 0;
    return false;
  }
  tdo = tdoTime*calib.slope + calib.intercept;
  return true;
}

bool 
Muon::NSWCalibTool::tdoToTime(const EventContext& ctx, const bool inCounts, const int tdo, const Identifier& chnlId, double& time, const int relBCID) const {
  if(!inCounts){
    time = tdo;
    return true;
  }
  const NswCalibDbTimeChargeData* tdoPdoData = getCalibData(ctx);
  if (!tdoPdoData) {
    time = 0.;
    return false;  
  }
  const TimeCalibConst& calib = tdoPdoData->getCalibForChannel(TimeCalibType::TDO, chnlId);
  if (!calib.is_valid){
    time = 0.;
    return false;
  } 
  //this shift of 25ns is necessary to align the time of the signal with the way the VMM determines the time
  //(relBCID 0 corresponds to -37.5 ns to - 12.5 ns)
  //Eventually it should go into the conditions db since it is probably not the same for MC and Data
  //but for now it is kept like it is. pscholer 8th of June 2022
  double mmLatency = (m_isData? m_mmLatencyData : m_mmLatencyMC);
  double stgcLatency = (m_isData? m_stgcLatencyData : m_stgcLatencyMC);

  const double peakTime  = m_idHelperSvc->isMM(chnlId) ? mmPeakTime() + mmLatency : stgcPeakTime() + stgcLatency; 
  time = relBCID*25. - (tdo-calib.intercept)/calib.slope + peakTime;
  return true;
}

NSWCalib::MicroMegaGas  Muon::NSWCalibTool::mmGasProperties() const {
  NSWCalib::MicroMegaGas properties{};
  properties.vDrift = m_vDrift;
  properties.longDiff = m_longDiff;
  properties.transDiff = m_transDiff;
  properties.interactionDensityMean = m_interactionDensityMean;
  properties.interactionDensitySigma = m_interactionDensitySigma;
  properties.lorentzAngleFunction = m_lorentzAngleFunction.get();
  return properties;
}


bool Muon::NSWCalibTool::localStripPosition(const Identifier& id, Amg::Vector2D &locPos) const {
  // MuonDetectorManager from the conditions store
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  if(m_idHelperSvc->isMM(id)){
    const MuonGM::MMReadoutElement* detEl = muDetMgr->getMMReadoutElement(id);
    return detEl->stripPosition(id,locPos);

  } else if(m_idHelperSvc->issTgc(id)){
    const MuonGM::sTgcReadoutElement* detEl = muDetMgr->getsTgcReadoutElement(id);
    return detEl->stripPosition(id,locPos);

  } else {
    return false;
  }
}
