#include "TrigT1NSWSimTools/MMT_Hit.h"
#include "AthenaKernel/getMessageSvc.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include <cmath>

MMT_Hit::MMT_Hit(char wedge, hitData_entry entry, const MuonGM::MuonDetectorManager* detManager) : AthMessaging(Athena::getMessageSvc(), "MMT_Hit") {
  m_sector = wedge;

  std::string module(1, wedge);
  module += (std::abs(entry.station_eta) == 1) ? "M1" : "M2";
  m_module = module;

  m_station_name = "MM";
  m_station_name += wedge;
  m_VMM_chip = entry.VMM_chip;
  m_MMFE_VMM = entry.MMFE_VMM;
  m_ART_ASIC = std::ceil(1.*entry.MMFE_VMM/2);
  m_station_eta = entry.station_eta;
  m_station_phi = entry.station_phi;
  m_multiplet = entry.multiplet;
  m_gasgap = entry.gasgap;
  m_plane = entry.plane;
  m_strip = entry.strip;
  m_localX = entry.localX;
  m_BC_time = entry.BC_time;
  m_age = entry.BC_time;
  m_Y = -1.;
  m_Z = -1.;
  m_R = -1.;
  m_Ri = -1.;
  m_isNoise = false;
  m_detManager = detManager;
}

MMT_Hit::MMT_Hit(const MMT_Hit &hit) : AthMessaging(Athena::getMessageSvc(), "MMT_Hit") {
  m_sector = hit.m_sector;
  m_module = hit.m_module;
  m_station_name = hit.m_station_name;
  m_VMM_chip = hit.m_VMM_chip;
  m_MMFE_VMM = hit.m_MMFE_VMM;
  m_ART_ASIC = hit.m_ART_ASIC;
  m_station_eta = hit.m_station_eta;
  m_station_phi = hit.m_station_phi;
  m_multiplet = hit.m_multiplet;
  m_gasgap = hit.m_gasgap;
  m_plane = hit.m_plane;
  m_strip = hit.m_strip;
  m_localX = hit.m_localX;
  m_BC_time = hit.m_BC_time;
  m_age = hit.m_age;
  m_Y = hit.m_Y;
  m_Z = hit.m_Z;
  m_R = hit.m_R;
  m_Ri = hit.m_Ri;
  m_RZslope = hit.m_RZslope;
  m_YZslope = hit.m_YZslope;
  m_isNoise = hit.m_isNoise;
  m_detManager = nullptr;
}

MMT_Hit& MMT_Hit::operator=(const MMT_Hit& hit) {
  m_sector = hit.m_sector;
  m_module = hit.m_module;
  m_station_name = hit.m_station_name;
  m_VMM_chip = hit.m_VMM_chip;
  m_MMFE_VMM = hit.m_MMFE_VMM;
  m_ART_ASIC = hit.m_ART_ASIC;
  m_station_eta = hit.m_station_eta;
  m_station_phi = hit.m_station_phi;
  m_multiplet = hit.m_multiplet;
  m_gasgap = hit.m_gasgap;
  m_plane = hit.m_plane;
  m_strip = hit.m_strip;
  m_localX = hit.m_localX;
  m_BC_time = hit.m_BC_time;
  m_age = hit.m_age;
  m_Y = hit.m_Y;
  m_Z = hit.m_Z;
  m_R = hit.m_R;
  m_Ri = hit.m_Ri;
  m_RZslope = hit.m_RZslope;
  m_YZslope = hit.m_YZslope;
  m_isNoise = hit.m_isNoise;
  m_detManager = nullptr;

  return *this;
}

bool MMT_Hit::isX() const {
  int id = this->getPlane();
  return (id == 0 || id == 1 || id == 6 || id == 7) ? true : false;
}

bool MMT_Hit::isU() const {
  int id = this->getPlane();
  return (id == 2 || id == 4) ? true : false;
}

bool MMT_Hit::isV() const {
  int id = this->getPlane();
  return (id == 3 || id == 5) ? true : false;
}

void MMT_Hit::printHit() {
  ATH_MSG_DEBUG("****************** HIT PROPERTIES ******************\n"<<
                "\t\t\t\t      *** wedge:     " << this->getStationName()      << "\n"<<
                "\t\t\t\t      *** VMM:       " << this->getVMM()              << "\n"<<
                "\t\t\t\t      *** MMFE:      " << this->getMMFE8()            << "\n"<<
                "\t\t\t\t      *** ART_ASIC:  " << this->getART()              << "\n"<<
                "\t\t\t\t      *** plane:     " << this->getPlane()            << "\n"<<
                "\t\t\t\t      *** st. Eta:   " << this->getStationEta()       << "\n"<<
                "\t\t\t\t      *** st. Phi:   " << this->getStationPhi()       << "\n"<<
                "\t\t\t\t      *** Multip.:   " << this->getMultiplet()        << "\n"<<
                "\t\t\t\t      *** Gas Gap:   " << this->getGasGap()           << "\n"<<
                "\t\t\t\t      *** strip:     " << this->getChannel()          << "\n"<<
                "\t\t\t\t      *** slopeRZ:   " << this->getRZSlope()          << "\n"<<
                "\t\t\t\t      *** BC:        " << this->getBC()               << "\n"<<
                "\t\t\t\t      *** X:         " << this->getX()                << "\n"<<
                "\t\t\t\t      *** Y:         " << this->getY()                << "\n"<<
                "\t\t\t\t      *** Z:         " << this->getZ()                << "\n"<<
                "\t\t\t\t      *** R:         " << this->getR()                << "\n"<<
                "\t\t\t\t      ****************************************************");
}

void MMT_Hit::setHitProperties(const Hit &hit) {
  m_Y = hit.info.y;
  m_Z = hit.info.z;
}

void MMT_Hit::updateHitProperties(std::shared_ptr<MMT_Parameters> par) {
  Identifier strip_id = this->getDetManager()->mmIdHelper()->channelID(this->getStationName(), this->getStationEta(), this->getStationPhi(), this->getMultiplet(), this->getGasGap(), this->getChannel());
  const MuonGM::MMReadoutElement* readout = this->getDetManager()->getMMReadoutElement(strip_id);
  Amg::Vector3D globalPos(0.0, 0.0, 0.0);
  readout->stripGlobalPosition(strip_id, globalPos);

  MMDetectorHelper aHelper;
  char side = (globalPos.z() > 0.) ? 'A' : 'C';
  MMDetectorDescription* mm = aHelper.Get_MMDetector(this->getSector(), std::abs(this->getStationEta()), this->getStationPhi(), this->getMultiplet(), side);
  MMReadoutParameters roP   = mm->GetReadoutParameters();

  double R = roP.distanceFromZAxis + this->getChannel()*roP.stripPitch - roP.stripPitch/2.;
  m_R = R;
  m_Z = globalPos.z();
  m_Ri = this->getChannel()*roP.stripPitch;
  m_RZslope = R / this->getZ();
  m_Z = this->getZ();
  ATH_MSG_DEBUG("Module: " << this->getModule() <<
                " HIT --> R_0: " << roP.distanceFromZAxis <<
                " + Ri " << this->getChannel()*roP.stripPitch << " corresponding to channel " << this->getChannel() <<
                " ----- Z: " << this->getZ() << ", Plane: " << this->getPlane() << ", eta " << this->getStationEta() << " -- BC: " << this->getBC() <<
                " RZslope: " << this->getRZSlope());

  int eta = std::abs(this->getStationEta())-1;
  double base = par->ybases[this->getPlane()][eta];
  double Y = base + this->getChannel()*roP.stripPitch - roP.stripPitch/2.;
  m_Y = Y;
  m_YZslope = Y / this->getZ();
}

bool MMT_Hit::verifyHit() {
  /*
   * Put here all Hit checks, probably redundant if digitization is ok
   */
  if (this->getBC() < 0.) return false;
  else if (isinf(this->getRZSlope())) return false;
  else if (this->getChannel() < 1 || this->getChannel() > 8192) return false;
  else return true;
}

