/*   
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */ 

#ifndef MMT_ROAD_H 
#define MMT_ROAD_H

#include "AthenaBaseComps/AthMessaging.h"
#include "MMT_Hit.h"
#include <numeric>
 
namespace MuonGM {
  class MuonDetectorManager;
}

struct micromegas_t {
  int roadSize{-1};
  int nstrip_up_XX{-1};
  int nstrip_dn_XX{-1};
  int nstrip_up_UV{-1};
  int nstrip_dn_UV{-1};
  unsigned int strips{0};
  unsigned int nMissedTopEta{0};
  unsigned int nMissedBottomEta{0};
  unsigned int nMissedTopStereo{0};
  unsigned int nMissedBottomStereo{0};
  double pitch{0.};
  double dimensions_top{0.};
  double dimensions_bottom{0.};
  double dimensions_height{0.};
  double activeArea_top{0.};
  double activeArea_bottom{0.};
  double activeArea_height{0.};
  double innerRadiusEta1{0.};
  double innerRadiusEta2{0.};
  std::vector<double> stereoAngles{};
  std::vector<ROOT::Math::XYZVector> planeCoordinates{};
};

class MMT_Road : public AthMessaging {
  public:
    MMT_Road(const char sector, const MuonGM::MuonDetectorManager* detManager, const micromegas_t mm, int xthr, int uvthr, int iroadx, int iroadu = -1, int iroadv = -1);
    ~MMT_Road();

    void addHits(std::vector<std::shared_ptr<MMT_Hit> > &hits);
    double avgSofX();
    double avgSofUV(const int uv1, const int uv2);
    double avgZofUV(const int uv1, const int uv2);
    bool checkCoincidences(const int &bcwind);
    bool containsNeighbors(const MMT_Hit &hit);
    unsigned int countHits() const { return m_road_hits.size(); }
    unsigned int countRealHits();
    unsigned int countUVHits(bool flag);
    unsigned int countXHits(bool flag);
    bool evaluateLowRes();
    bool horizontalCheck();
    void incrementAge(const int &bcwind);
    double getB() const { return m_B; }
    int getRoadSize() const { return m_roadSize; }
    int getRoadSizeUpX() const { return m_roadSizeUpX; }
    int getRoadSizeDownX() const { return m_roadSizeDownX; }
    int getRoadSizeUpUV() const { return m_roadSizeUpUV; }
    int getRoadSizeDownUV() const { return m_roadSizeDownUV; }
    std::vector<MMT_Hit> getHitVector() const { return m_road_hits; }
    char getSector() const { return m_sector; }
    int getXthreshold() const { return m_xthr; }
    int getUVthreshold() const { return m_uvthr; }
    int iRoad() const { return m_iroad; }
    int iRoadx() const { return m_iroadx; }
    int iRoadu() const { return m_iroadu; }
    int iRoadv() const { return m_iroadv; }
    bool matureCheck(const int &bcwind);
    double mxl();
    void reset();
    bool stereoCheck();
    micromegas_t getMM() const { return m_micromegas; }

  private:
    const MuonGM::MuonDetectorManager* m_detManager{};        //!< MuonDetectorManager
    const MuonGM::MuonDetectorManager* GetDetManager() { return m_detManager; }
    double m_B;
    int m_iroad;
    int m_iroadx;
    int m_iroadu;
    int m_iroadv;
    micromegas_t m_micromega;
    char m_sector;
    int m_xthr, m_uvthr;
    int m_roadSize, m_roadSizeUpX, m_roadSizeDownX, m_roadSizeUpUV, m_roadSizeDownUV;
    bool m_trig;

    micromegas_t m_micromegas;
    std::vector<MMT_Hit> m_road_hits;
};
#endif
