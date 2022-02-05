#include "TrigT1NSWSimTools/MMT_Road.h"
#include "AthenaKernel/getMessageSvc.h"
#include <cmath>

MMT_Road::MMT_Road(const char sector, const MuonGM::MuonDetectorManager* detManager, const micromegas_t mm, int xthr, int uvthr, int iroadx, int iroadu, int iroadv): AthMessaging(Athena::getMessageSvc(), "MMT_Road") {
  m_sector = sector;
  m_iroad  = iroadx;
  m_iroadx = iroadx;
  m_iroadu = (iroadu != -1) ? iroadu : iroadx;
  m_iroadv = (iroadv != -1) ? iroadv : iroadx;
  m_trig = false;
  m_xthr = xthr;
  m_uvthr = uvthr;

  if (iroadu == -1 && iroadv != -1) ATH_MSG_WARNING("iroadu = -1 but iroadv ain't");
  if (iroadu != -1 && iroadv == -1) ATH_MSG_WARNING("iroadv = -1 but iroadu ain't");
  m_detManager = detManager;
  m_micromegas = mm;
  m_roadSize = mm.roadSize;
  m_roadSizeUpX = mm.nstrip_up_XX;
  m_roadSizeDownX = mm.nstrip_dn_XX;
  m_roadSizeUpUV = mm.nstrip_up_UV;
  m_roadSizeDownUV = mm.nstrip_dn_UV;
  m_B = (1./std::tan(1.5/180.*M_PI));
}

MMT_Road::~MMT_Road() {
  this->reset();
}

void MMT_Road::addHits(std::vector<std::shared_ptr<MMT_Hit> > &hits) {
  for (auto hit_i : hits) {
    int bo = hit_i->getPlane();
    bool has_hit = false;
    if( this->containsNeighbors(*hit_i) ) {
      for (const auto &hit_j : m_road_hits) {
        if (hit_j.getPlane() == bo) {
          has_hit = true;
          break;
        }
      }
      if (hit_i->isNoise() == false) {
        int erase_me = -1;
        for (unsigned int j = 0; j < m_road_hits.size(); j++) {
          if (m_road_hits[j].getPlane() == bo && m_road_hits[j].isNoise()) {
            erase_me = j;
            has_hit = false;
            break;
          }
        }
        if (erase_me > -1) m_road_hits.erase(m_road_hits.begin() + erase_me);
      }

      if (has_hit) continue;
      m_road_hits.push_back(*hit_i);
      m_road_hits.back().setAge(0);
    }
  }
}

bool MMT_Road::containsNeighbors(const MMT_Hit &hit) {
  int iroad = 0;
  if (hit.isX()) iroad = this->iRoadx();
  else if (hit.isU()) iroad = this->iRoadu();
  else if (hit.isV()) iroad = this->iRoadv();
  else {
    std::cerr << "ERROR! Unknown hit type: " << hit.getPlane() << std::endl;
    return false;
  }

  double pitch = this->getMM().pitch;
  double R = (std::abs(hit.getStationEta()) == 1) ? this->getMM().innerRadiusEta1 : this->getMM().innerRadiusEta2;
  double Z = hit.getZ();

  double index = std::round((std::abs(hit.getRZSlope())-0.1)/5e-04); // 0.0005 is approx. the step in slope achievable with a road size of 8 strips
  double roundedSlope = 0.1 + index*((0.6 - 0.1)/1000.);
  double shift = roundedSlope*(this->getMM().planeCoordinates[hit.getPlane()].Z() - this->getMM().planeCoordinates[0].Z());

  double olow = 0., ohigh = 0.;
  if (hit.isX()) {
    olow  = this->getRoadSizeDownX()*pitch;
    ohigh = this->getRoadSizeUpX()*pitch;
  } else {
    olow  = this->getRoadSizeDownUV()*pitch;
    ohigh = this->getRoadSizeUpUV()*pitch;
  }

  double slow  = (R + (this->getRoadSize()*iroad  )*pitch + shift + pitch/2. - olow)/Z;
  double shigh = (R + (this->getRoadSize()*(iroad+1))*pitch + shift + pitch/2. + ohigh)/Z;

  double slope = hit.getRZSlope();
  if (this->getSector() != hit.getSector()) {
    ATH_MSG_DEBUG("Mismatch between road (" << this->getSector() << ") and hit (" << hit.getSector() << ") sectors");
    return false;
  }
  if (slope > 0.) return (slope >= slow && slope < shigh);
  else return (slope >= shigh && slope < slow);
}

double MMT_Road::avgSofX() {
  std::vector<double> sl;
  for (const auto &hit : m_road_hits) {
    int bo = hit.getPlane();
    if (bo < 2 || bo > 5) sl.push_back(hit.getRZSlope());
  }
  double avg_x = std::accumulate(sl.begin(), sl.end(), 0.0)/(double)sl.size();
  return avg_x;
}

double MMT_Road::avgSofUV(const int uv1, const int uv2) {
  std::vector<double> sl;
  for (const auto &hit : m_road_hits) {
    int bo = hit.getPlane();
    if (bo == uv1 || bo == uv2) sl.push_back(hit.getRZSlope());
  }
  double avg_uv = std::accumulate(sl.begin(), sl.end(), 0.0)/(double)sl.size();
  return avg_uv;
}

double MMT_Road::avgZofUV(const int uv1, const int uv2) {
  std::vector<double> zs;
  for (const auto &hit : m_road_hits) {
    int bo = hit.getPlane();
    if (bo == uv1 || bo == uv2) zs.push_back(hit.getZ());
  }
  double avg_z = std::accumulate(zs.begin(), zs.end(), 0.0)/(double)zs.size();
  return avg_z;
}

bool MMT_Road::checkCoincidences(const int &bcwind) {
  bool passHorizontalCheck = this->horizontalCheck();
  bool passStereoCheck = this->stereoCheck();
  bool passMatureCheck = this->matureCheck(bcwind);
  return (passHorizontalCheck && passStereoCheck && passMatureCheck) ? true : false;
}

unsigned int MMT_Road::countRealHits() {
  int nreal = 0;
  for (const auto &hit : m_road_hits) {
    if (hit.isNoise() == false) nreal++;
  }
  return nreal;
}

unsigned int MMT_Road::countUVHits(bool flag) {
  unsigned int nuv = 0;
  for (const auto &hit : m_road_hits) {
    if (hit.getPlane() == 2 || hit.getPlane() == 4) {
      if (hit.isNoise() == flag) nuv++;
    }
    if (hit.getPlane() == 3 || hit.getPlane() == 5) {
      if (hit.isNoise() == flag) nuv++;
    }
  }
  return nuv;
}

unsigned int MMT_Road::countXHits(bool flag) {
  unsigned int nx = 0;
  for (const auto &hit : m_road_hits) {
    if (hit.getPlane() < 2 || hit.getPlane() > 5) {
      if (hit.isNoise() == flag) nx++;
    }
  }
  return nx;
}

bool MMT_Road::evaluateLowRes() {
  unsigned int nhits1 = 0, nhits2 = 0;
  for (const auto &hit : m_road_hits) {
    if (hit.getPlane() < 4 && !hit.isNoise()) nhits1++;
    else if (hit.getPlane() > 3 && !hit.isNoise()) nhits2++;
  }
  return (nhits1 < 4 || nhits2 < 4) ? true : false;
}

bool MMT_Road::horizontalCheck() {
  int nx1 = 0, nx2 = 0;
  for (const auto &hit : m_road_hits) {
    if (hit.getPlane() >-1 && hit.getPlane() < 2) nx1++;
    if (hit.getPlane() > 5 && hit.getPlane() < 8) nx2++;
  }
  if (nx1 > 0 && nx2 > 0 && (nx1+nx2) >= this->getXthreshold()) return true;
  return false;
}

void MMT_Road::incrementAge(const int &bcwind) {
  std::vector<unsigned int> old_ihits;
  for (unsigned int j = 0; j < m_road_hits.size(); j++) {
    m_road_hits[j].setAge(m_road_hits[j].getAge() +1);
    if (m_road_hits[j].getAge() > (bcwind-1)) old_ihits.push_back(j);
  }
  for (int j = old_ihits.size()-1; j > -1; j--) m_road_hits.erase(m_road_hits.begin()+j);
}

bool MMT_Road::matureCheck(const int &bcwind) {
  for (const auto &hit : m_road_hits) {
    if (hit.getAge() == (bcwind - 1)) return true;
  }
  return false;
}

double MMT_Road::mxl() {
  std::vector<double> ys, zs;
  for (const auto &hit : m_road_hits) {
    int bo = hit.getPlane();
    if (bo < 2 || bo > 5) {
      ys.push_back(hit.getR());
      zs.push_back(hit.getZ());
    }
  }
  double mxl = 0;
  double avg_z = std::accumulate(zs.begin(), zs.end(), 0.0)/(double)zs.size();
  double sum_sq_z = std::inner_product(zs.begin(), zs.end(), zs.begin(), 0.0);
  for (unsigned int i = 0; i < ys.size(); i++) mxl += ys[i]*( (zs[i]-avg_z) / (sum_sq_z - zs.size()*std::pow(avg_z,2)) );

  return mxl;
}

void MMT_Road::reset() {
  if (!m_road_hits.empty()) m_road_hits.clear();
}

bool MMT_Road::stereoCheck() {
  int nu = 0, nv = 0;
  for (const auto &hit : m_road_hits) {
    if (hit.getPlane() == 2 || hit.getPlane() == 4) nu++;
    if (hit.getPlane() == 3 || hit.getPlane() == 5) nv++;
  }
  if (this->getUVthreshold() == 0) return true;
  if (nu > 0 && nv > 0 && (nu+nv) >= this->getUVthreshold()) return true;
  return false;
}
