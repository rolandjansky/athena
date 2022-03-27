#include "TrigT1NSWSimTools/MMT_Diamond.h"
#include "AthenaKernel/getMessageSvc.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"
#include <cmath>

MMT_Diamond::MMT_Diamond(const MuonGM::MuonDetectorManager* detManager): AthMessaging(Athena::getMessageSvc(), "MMT_Diamond") {
  m_detManager = detManager;
}

void MMT_Diamond::clearEvent() {
  if (!m_diamonds.empty()) {
    for (auto &diam : m_diamonds) {
      if (!diam.ev_roads.empty()) diam.ev_roads.clear();
      if (!diam.ev_hits.empty()) diam.ev_hits.clear();
      diam.slopes.clear();
    }
    m_diamonds.clear();
  }
}

void MMT_Diamond::createRoads_fillHits(const unsigned int iterator, std::vector<hitData_entry> &hitDatas, const MuonGM::MuonDetectorManager* detManager, std::shared_ptr<MMT_Parameters> par, const int phi) {
  ATH_MSG_DEBUG("createRoads_fillHits: Feeding hitDatas Start");

  diamond_t entry;
  entry.wedgeCounter = iterator;
  entry.sector = par->getSector();
  entry.stationPhi = (par->getSector() == 'S') ? phi*2-1 : phi*2-2;

  micromegas_t micromegas;
  MMDetectorHelper aHelper;
  MMDetectorDescription* mm = aHelper.Get_MMDetector(par->getSector(), 1, 5, 1, 'A');
  MMReadoutParameters roP   = mm->GetReadoutParameters();
  MMDetectorDescription* mm2 = aHelper.Get_MMDetector(par->getSector(), 2, 5, 1, 'A');
  MMReadoutParameters roP_2   = mm2->GetReadoutParameters();

  micromegas.roadSize = this->getRoadSize();
  micromegas.nstrip_up_XX = this->getRoadSizeUpX();
  micromegas.nstrip_dn_XX = this->getRoadSizeDownX();
  micromegas.nstrip_up_UV = this->getRoadSizeUpUV();
  micromegas.nstrip_dn_UV = this->getRoadSizeDownUV();
  micromegas.strips = roP.tStrips;
  micromegas.pitch = roP.stripPitch;
  micromegas.nMissedTopEta = roP.nMissedTopEta;
  micromegas.nMissedBottomEta = roP.nMissedBottomEta;
  micromegas.nMissedTopStereo = roP.nMissedTopStereo;
  micromegas.nMissedBottomStereo = roP.nMissedBottomStereo;

  micromegas.dimensions_top = mm->lWidth();
  micromegas.dimensions_bottom = mm->sWidth();
  micromegas.dimensions_height = mm->Length();

  micromegas.activeArea_top = roP.activeTopLength;
  micromegas.activeArea_bottom = roP.activeBottomLength;
  micromegas.activeArea_height = roP.activeH;
  micromegas.innerRadiusEta1 = roP.distanceFromZAxis;
  micromegas.innerRadiusEta2 = roP_2.distanceFromZAxis;
  micromegas.stereoAngles = roP.stereoAngle;

  std::string sector = (par->getSector() == 'L') ? "MML" : "MMS";
  /*
   * The following for-loop merges all plane global coordinates in one single shot:
   * X & Y are constant in all layers, for a given phi (Not used at the moment)
   * Z (layer coordinates) changes only for Small and Large sectors --> USED and working!
   */
  for (unsigned int phi = 0; phi < 8; phi++) {
    Amg::Vector3D globalPos(0.0, 0.0, 0.0);
    int multilayer = (phi < 4) ? 1 : 2;
    int gasgap = ((phi+1)%4 == 0) ? 4 : (phi+1)%4;
    /*
     * Strip 100 (or 200) chosen as good compromise, considering offline strips.
     * channelMin() function (https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/MuonIdHelpers/MuonIdHelpers/MmIdHelper.h?v=21.3#0123)
     * could be used, instead of hardcoding the strip id, but it returns (always?) as initialized in level ranges
     */
    int strip = (phi > 1 && phi < 6) ? roP.nMissedBottomStereo+1 : roP.nMissedBottomEta+1;
    Identifier strip_id = detManager->mmIdHelper()->channelID(sector, 1, phi+1, multilayer, gasgap, strip);
    const MuonGM::MMReadoutElement* readout = detManager->getMMReadoutElement(strip_id);

    ROOT::Math::XYZVector coord(0.,0.,0.);
    if (readout->stripGlobalPosition(strip_id, globalPos)) coord.SetXYZ(globalPos.x(), globalPos.y(), globalPos.z());
    else ATH_MSG_WARNING("Wedge " << sector << " phi: " << phi << " mult. " << multilayer << " gas " << gasgap <<  " | Unable to retrieve global positions");
    micromegas.planeCoordinates.push_back(coord);
  }

  int nroad = 8192/this->getRoadSize();
  double B = (1./std::tan(1.5/180.*M_PI));
  int uvfactor = std::round( mm2->lWidth() / (B * 0.4 * 2.)/this->getRoadSize() ); // mm2 pointer is used because the full wedge has to be considered, i.e. S(L/M)2
  this->setUVfactor(uvfactor);

  for (int ihds = 0; ihds < (int)hitDatas.size(); ihds++) {
    auto myhit = std::make_shared<MMT_Hit>(par->getSector(), hitDatas[ihds], detManager, par);
    if (myhit->verifyHit()) {
      m_hitslopes.push_back(myhit->getRZSlope());
      entry.ev_hits.push_back(myhit);
    }
  }
  entry.side = (std::all_of(entry.ev_hits.begin(), entry.ev_hits.end(), [] (const auto hit) { return hit->getStationEta() < 0; })) ? 'C' : '-';
  entry.side = (std::all_of(entry.ev_hits.begin(), entry.ev_hits.end(), [] (const auto hit) { return hit->getStationEta() > 0; })) ? 'A' : '-';

  std::vector<std::shared_ptr<MMT_Road> > temp_roads;
  for (int i = 0; i < nroad; i++) {

    auto myroad = std::make_shared<MMT_Road>(par->getSector(), detManager, micromegas, this->getXthreshold(), this->getUVthreshold(), i);
    temp_roads.push_back(myroad);

    int nuv = (this->getUV()) ? this->getUVfactor() : 0;
    for (int uv = 1; uv <= nuv; uv++) {
      if (i-uv < 0) continue;

      auto myroad_0 = std::make_shared<MMT_Road>(par->getSector(), detManager, micromegas, this->getXthreshold(), this->getUVthreshold(), i, i+uv, i-uv);
      temp_roads.push_back(myroad_0);

      auto myroad_1 = std::make_shared<MMT_Road>(par->getSector(), detManager, micromegas, this->getXthreshold(), this->getUVthreshold(), i, i-uv, i+uv);
      temp_roads.push_back(myroad_1);

      auto myroad_2 = std::make_shared<MMT_Road>(par->getSector(), detManager, micromegas, this->getXthreshold(), this->getUVthreshold(), i, i+uv-1, i-uv);
      temp_roads.push_back(myroad_2);

      auto myroad_3 = std::make_shared<MMT_Road>(par->getSector(), detManager, micromegas, this->getXthreshold(), this->getUVthreshold(), i, i-uv, i+uv-1);
      temp_roads.push_back(myroad_3);

      auto myroad_4 = std::make_shared<MMT_Road>(par->getSector(), detManager, micromegas, this->getXthreshold(), this->getUVthreshold(), i, i-uv+1, i+uv);
      temp_roads.push_back(myroad_4);

      auto myroad_5 = std::make_shared<MMT_Road>(par->getSector(), detManager, micromegas, this->getXthreshold(), this->getUVthreshold(), i, i+uv, i-uv+1);
      temp_roads.push_back(myroad_5);
    }
  }

  for (const auto &road : temp_roads) {
    if (this->isTrapezoidalShape()) {
      if (road->iRoadu() < 0. && road->iRoadv() < 0.) continue;
      /* 
       * Here some condition(s) about the trapezoidal shape should be added
       */
      entry.ev_roads.push_back(road);
    } else entry.ev_roads.push_back(road);
  }

  temp_roads.clear();
  m_diamonds.push_back(entry);

  ATH_MSG_DEBUG("CreateRoadsAndFillHits: Feeding hitDatas Ended");
}

void MMT_Diamond::printHits(const unsigned int iterator) {
  if (iterator < m_diamonds.size() && !m_diamonds[iterator].ev_hits.empty()) {
    for (const auto &hit : m_diamonds[iterator].ev_hits) hit->printHit();
  } else {
    ATH_MSG_DEBUG("Hit vector is empty!");
  }
}

void MMT_Diamond::findDiamonds(const unsigned int iterator, const double &sm_bc, const int &event) {
  auto t0 = std::chrono::high_resolution_clock::now();
  int ntrig = 0;
  int bc_start = 999999;
  int bc_end = -1;
  int bc_wind = 4; // fixed time window (in bunch crossings) during which the algorithm collects ART hits
  unsigned int ibc = 0;

  m_diamonds[iterator].slopes.clear();

  // Comparison with lambda function (easier to implement)
  std::sort(m_diamonds[iterator].ev_hits.begin(), m_diamonds[iterator].ev_hits.end(), [](auto h1, auto h2){ return h1->getAge() < h2->getAge(); });
  bc_start = m_diamonds[iterator].ev_hits.front()->getAge() - bc_wind*2;
  bc_end = m_diamonds[iterator].ev_hits.back()->getAge() + bc_wind*2;
  ATH_MSG_DEBUG("Window Start: " << bc_start << " - Window End: " << bc_end);

  for (const auto &road : m_diamonds[iterator].ev_roads) road->reset();

  std::vector<std::shared_ptr<MMT_Hit> > hits_now = {};
  std::vector<int> vmm_same  = {};
  std::vector< std::pair<int, int> > addc_same = {};
  std::vector<int> to_erase = {};
  int n_vmm  = std::ceil(8192/64.);
  int n_addc = std::ceil(8192/2048.);

  // each road makes independent triggers, evaluated on each BC
  for (int bc = m_diamonds[iterator].ev_hits.front()->getBC(); bc < bc_end; bc++) {
    // Cleaning stuff
    hits_now.clear();

    for (unsigned int j = ibc; j < m_diamonds[iterator].ev_hits.size(); j++) {
      if (m_diamonds[iterator].ev_hits[j]->getAge() == bc) hits_now.push_back(m_diamonds[iterator].ev_hits[j]);
      else if (m_diamonds[iterator].ev_hits[j]->getAge() > bc) {
        ibc = j;
        break;
      }
    }

    // Implement VMM Art signal
    for (unsigned int ib = 1; ib < 9; ib++) {
      for (int j = 1; j < n_vmm; j++) {
        vmm_same.clear();
        for (unsigned int k = 0; k < hits_now.size(); k++) {
          if ((unsigned int)(hits_now[k]->getPlane()) != ib) continue;
          if (hits_now[k]->getVMM() == j) vmm_same.push_back(k);
        }
        /*
         * Is this really necessary with signal only? It kills most of the hits
         *
         * if (vmm_same.size() > 1) {
         *   int the_chosen_one = ran->Integer((int)(vmm_same.size()));
         *   for (int k = vmm_same.size()-1; k > -1; k--) if (k != the_chosen_one) hits_now.erase(hits_now.begin()+vmm_same[k]);
         * }
         */
      }

      // Implement ADDC-like handling
      for (int ia = 1; ia < n_addc; ia++) { // From 1 to 4
        addc_same.clear();
        for (unsigned int k = 0; k < hits_now.size(); k++) {
          if ((unsigned int)(hits_now[k]->getPlane()) == ib && hits_now[k]->getART() == ia) addc_same.push_back( std::make_pair(k, hits_now[k]->getChannel()) );
        }

        if (addc_same.size() <= 8) continue;

        // priority encode the hits by channel number; remember hits 8+
        to_erase.clear();

        std::sort(addc_same.begin(), addc_same.end(), [](std::pair<int, int> p1, std::pair<int, int> p2) { return p1.second < p2.second; });
        for (unsigned int it = 8; it < addc_same.size(); it++) to_erase.push_back(addc_same[it].first);

        // reverse and erase
        std::sort(to_erase.rbegin(), to_erase.rend());
        for (auto k : to_erase) {
          if (hits_now[k]->isNoise() == false) continue;
          else hits_now.erase(hits_now.begin() + k);
        }
      }
    }

    for (auto &road : m_diamonds[iterator].ev_roads) {
      road->incrementAge(bc_wind);
      if (!hits_now.empty()) road->addHits(hits_now);

      if (road->checkCoincidences(bc_wind) && bc >= (sm_bc - 1)) {
        ntrig++;

        ATH_MSG_DEBUG("------------------------------------------------------------------");
        ATH_MSG_DEBUG("Coincidence FOUND @BC: " << bc);
        ATH_MSG_DEBUG("Road (i, u, v, count): (" << road->iRoad() << ", " << road->iRoadu() << ", " << road->iRoadv() << ", " << road->countHits() << ")");
        ATH_MSG_DEBUG("------------------------------------------------------------------");
        for (const auto &hit: road->getHitVector()) {
          ATH_MSG_DEBUG("Hit (board, BC, strip, eta): (" << hit.getPlane() << ", " << hit.getBC() << ", " << hit.getChannel() << ", " << hit.getStationEta() << ")");
        }

        slope_t slope;
        slope.event = event;
        slope.BC = bc;
        slope.totalCount = road->countHits();
        slope.realCount = road->countRealHits();
        slope.iRoad = road->iRoad();
        slope.iRoadu = road->iRoadu();
        slope.iRoadv = road->iRoadv();
        slope.uvbkg = road->countUVHits(true); // the bool in the following 4 functions refers to background/noise hits
        slope.xbkg = road->countXHits(true);
        slope.uvmuon = road->countUVHits(false);
        slope.xmuon = road->countXHits(false);
        slope.age = bc - sm_bc;
        slope.mxl = road->mxl();
        slope.my = road->avgSofX(); // defined as my in ATL-COM-UPGRADE-2015-033
        slope.uavg = road->avgSofUV(2,4);
        slope.vavg = road->avgSofUV(3,5);
        slope.mx = (slope.uavg-slope.vavg)/(2.*std::tan(0.02618)); // The stereo angle is fixed and can be hardcoded
        double theta = std::atan(std::sqrt(std::pow(slope.mx,2) + std::pow(slope.my,2)));
        slope.theta = (slope.my > 0.) ? theta : M_PI - theta;
        slope.eta = -1.*std::log(std::tan(slope.theta/2.));
        slope.dtheta = (slope.mxl - slope.my)/(1. + slope.mxl*slope.my);
        slope.side = (slope.my > 0.) ? 'A' : 'C';
        double phi = std::atan(slope.mx/slope.my), phiShifted = this->phiShift(this->getDiamond(iterator).stationPhi, phi, slope.side);
        slope.phi = phi;
        slope.phiShf = phiShifted;
        slope.lowRes = road->evaluateLowRes();

        m_diamonds[iterator].slopes.push_back(slope);
      }
    }
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  ATH_MSG_DEBUG("Processing roads took " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms");
}

double MMT_Diamond::phiShift(const int n, const double &phi, const char &side) {
  double Phi = (side == 'A') ? phi : -phi;
  float shift = (n > 8) ? (16-n)*M_PI/8. : n*M_PI/8.;
  if (n < 8)       return (Phi + shift);
  else if (n == 8) return (Phi + ((Phi > 0.) ? -1. : 1.)*shift);
  else             return (Phi - shift);
}

void MMT_Diamond::resetSlopes() {
  if (!m_hitslopes.empty()) m_hitslopes.clear();
}

slope_t::slope_t(int ev, int bc, unsigned int tC, unsigned int rC, int iX, int iU, int iV, unsigned int uvb, unsigned int xb, unsigned int uvm, unsigned int xm,
                 int age, double mxl, double my, double uavg, double vavg, double mx, double th, double eta, double dth, char side, double phi, double phiS,
                 bool lowRes) :
  event(ev), BC(bc), totalCount(tC), realCount(rC), iRoad(iX), iRoadu(iU), iRoadv(iV), uvbkg(uvb), xbkg(xb), uvmuon(uvm), xmuon(xm),
  age(age), mxl(mxl), my(my), uavg(uavg), vavg(vavg), mx(mx), theta(th), eta(eta), dtheta(dth), side(side), phi(phi), phiShf(phiS), lowRes(lowRes) {}
