/*   
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */ 

#ifndef MMT_DIAMOND_H 
#define MMT_DIAMOND_H

#include "AthenaBaseComps/AthMessaging.h"
#include "MMT_Road.h"
#include <vector>

struct slope_t {
  slope_t(int ev=-1, int bc=-1, unsigned int tC=999, unsigned int rC=999, int iX=-1, int iU=-1, int iV=-1, unsigned int uvb=999, unsigned int xb=999, 
          unsigned int uvm=999, unsigned int xm=999, int age=-1, double mxl=999., double xavg=999., double uavg=999., double vavg=999., double mx=999., 
          double th=999., double eta=999., double dth=999., char side='-', double phi=999., double phiS=999., bool lowRes=false);
  unsigned int event;
  int BC;
  unsigned int totalCount;
  unsigned int realCount;
  int iRoad;
  int iRoadu;
  int iRoadv;
  unsigned int uvbkg;
  unsigned int xbkg;
  unsigned int uvmuon;
  unsigned int xmuon;
  int age;
  double mxl;
  double xavg;
  double uavg;
  double vavg;
  double mx;
  double theta;
  double eta;
  double dtheta;
  char side;
  double phi;
  double phiShf;
  bool lowRes;
};

struct diamond_t {
  unsigned int wedgeCounter;
  char sector;
  char side;
  int stationPhi;
  std::vector<std::shared_ptr<MMT_Road> > ev_roads;
  std::vector<slope_t> slopes;
  std::vector<std::shared_ptr<MMT_Hit> > ev_hits;
};

class MMT_Diamond : public AthMessaging {
  public:
    MMT_Diamond(const MuonGM::MuonDetectorManager* detManager);

    void clearEvent();
    void createRoads_fillHits(const unsigned int iterator, std::vector<hitData_entry> &hitDatas, const MuonGM::MuonDetectorManager* detManager, std::shared_ptr<MMT_Parameters> par, const int phi);
    void findDiamonds(const unsigned int iterator, const double &sm_bc, const int &event);
    double phiShift(const int n, const double &phi, const char &side);
    std::vector<diamond_t> getDiamondVector() const { return m_diamonds; }
    diamond_t getDiamond(const unsigned int iterator) const { return m_diamonds.at(iterator); }
    std::vector<double> getHitSlopes() const { return m_hitslopes; }
    std::vector<std::shared_ptr<MMT_Hit> > getHitVector(const unsigned int iterator) const { return m_diamonds.at(iterator).ev_hits; }
    std::vector<slope_t> getSlopeVector(const unsigned int iterator) const { return m_diamonds.at(iterator).slopes; }
    unsigned int getDiamondSize() const { return m_diamonds.size(); }
    int getUVfactor() const { return m_uvfactor; }
    void printHits(const unsigned int iterator);
    void resetSlopes();
    void setTrapezoidalShape(bool flag) { m_trapflag = flag; }
    void setUVfactor(int factor) { m_uvfactor = factor; }
    bool isTrapezoidalShape() const { return m_trapflag; }

    int getStationPhi() const { return m_phi; }
    int getRoadSize() const { return m_roadSize; }
    void setRoadSize(int size) { m_roadSize = size; }
    int getRoadSizeUpX() const { return m_roadSizeUpX; }
    void setRoadSizeUpX(int sizeUp) { m_roadSizeUpX = sizeUp; }
    int getRoadSizeDownX() const { return m_roadSizeDownX; }
    void setRoadSizeDownX(int sizeDown) { m_roadSizeDownX = sizeDown; }
    int getRoadSizeUpUV() const { return m_roadSizeUpUV; }
    void setRoadSizeUpUV(int sizeUpUV) { m_roadSizeUpUV = sizeUpUV; }
    int getRoadSizeDownUV() const { return m_roadSizeDownUV; }
    void setRoadSizeDownUV(int sizeDownUV) { m_roadSizeDownUV = sizeDownUV; }
    char getSector() const { return m_sector; }
    unsigned int getXthreshold() const { return m_xthr; }
    void setXthreshold(int threshold) { m_xthr = threshold; }
    bool getUV() const { return m_uvflag; }
    void setUV(bool flag) { m_uvflag = flag; }
    unsigned int getUVthreshold() const { return m_uvthr; }
    void setUVthreshold(int threshold) { m_uvthr = threshold; }

  private:
    const MuonGM::MuonDetectorManager* m_detManager{};        //!< MuonDetectorManager
    bool m_trapflag;
    int m_uvfactor;
    bool m_uvflag;
    int m_roadSize, m_roadSizeUpX, m_roadSizeDownX, m_roadSizeUpUV, m_roadSizeDownUV;
    int m_xthr, m_uvthr;
    int m_phi;
    char m_sector;

    std::vector<diamond_t> m_diamonds;
    std::vector<double> m_hitslopes;
};
#endif
