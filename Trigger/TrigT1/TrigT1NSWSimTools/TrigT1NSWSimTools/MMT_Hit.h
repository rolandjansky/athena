/*   
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */ 

#ifndef MMT_HIT_H 
#define MMT_HIT_H

#include "AthenaBaseComps/AthMessaging.h"
#include "MMT_struct.h"

namespace MuonGM {
  class MuonDetectorManager;
}

class MMT_Hit : public AthMessaging {
  public:
    MMT_Hit(char wedge, hitData_entry entry, const MuonGM::MuonDetectorManager* detManager, const std::shared_ptr<MMT_Parameters> par);
    MMT_Hit(const MMT_Hit &hit);
    MMT_Hit& operator=(const MMT_Hit&);

    int getART() const { return m_ART_ASIC; }
    int getAge() const { return m_age; }
    int getBC() const { return m_BC_time; }
    int getChannel() const { return m_strip; }
    int getGasGap() const { return m_gasgap; }
    std::string getModule() const { return m_module; }
    int getMultiplet() const { return m_multiplet; }
    int getPlane() const { return m_plane; }
    char getSector() const { return m_sector; }
    double getRZSlope() const { return m_RZslope; }
    double getYZSlope() const { return m_YZslope; }
    int getVMM() const { return m_VMM_chip; }
    int getMMFE8() const { return m_MMFE_VMM; }
    std::string getStationName() const { return m_station_name; }
    int getStationEta() const { return m_station_eta; }
    int getStationPhi() const { return m_station_phi; }
    double getR() const { return m_R; }
    double getRi() const { return m_Ri; }
    double getX() const { return m_localX; }
    double getY() const { return m_Y; }
    double getZ() const { return m_Z; }
    bool isNoise() const { return m_isNoise; }
    bool isX() const;
    bool isU() const;
    bool isV() const;
    void printHit() const;
    void setAge(int age) { m_age = age; }
    void setAsNoise() { m_isNoise = true; }
    void setBC(int bc) { m_BC_time = bc; }
    void setHitProperties(const Hit &hit);
    void setRZSlope(double slope) { m_RZslope = slope; }
    void setYZSlope(double slope) { m_YZslope = slope; }
    void setY(double y) { m_Y = y; }
    void setZ(double z) { m_Z = z; }
    bool verifyHit() const;

  private:
    char m_sector;
    std::string m_module, m_station_name;
    int m_VMM_chip;
    int m_MMFE_VMM;
    int m_ART_ASIC;
    int m_plane;
    int m_station_eta;
    int m_station_phi;
    int m_multiplet;
    int m_gasgap;
    int m_strip;
    double m_localX;
    double m_RZslope, m_YZslope;
    int m_BC_time, m_age;
    double m_Y, m_Z, m_R, m_Ri;
    bool m_isNoise;

    const MuonGM::MuonDetectorManager* m_detManager{};        //!< MuonDetectorManager
    const MuonGM::MuonDetectorManager* getDetManager() { return m_detManager; }
};
#endif
