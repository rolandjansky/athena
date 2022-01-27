/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHHIT_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHHIT_H

#include <GeoPrimitives/GeoPrimitives.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace MuonHough {
    /** enum to identify the muondetectortechnology */
    enum DetectorTechnology { MDT, CSC, RPC, TGC };
    /** number of phi sectors */
    constexpr int phisectors = 16;
    /** angle of half a sector in rad */
    const double half_phisector = M_PI / phisectors;
}  // namespace MuonHough

namespace Trk {
    class PrepRawData;
}

class MuonHoughHit {
public:
    /** default constructor (should not be used) */
    //  MuonHoughHit();
    /** constructor per preprawdata (not used) */
    MuonHoughHit(const Trk::PrepRawData* prd);
    /** constructor */
    MuonHoughHit(const Amg::Vector3D& pos_vec, bool measures_phi, MuonHough::DetectorTechnology detector_id, double prob = 1,
                 double weight = 1., const Trk::PrepRawData* prd = nullptr, int id = -1);
    /** destructor */
    virtual ~MuonHoughHit() = default;

    /** return DetectorTechnology */
    MuonHough::DetectorTechnology getDetectorId() const;
    /** return DetectorTechnology in string */
    std::string getWhichDetector() const;

    /** return (x,y,z) vector */
    const Amg::Vector3D& getPosition() const;
    /** returns id */
    int getId() const;
    /** returns x position */
    double getHitx() const;
    /** returns y position */
    double getHity() const;
    /** returns z position */
    double getHitz() const;
    /** returns weight in histogram after rescaling */
    double getWeight() const;
    /** returns original weight */
    double getOrigWeight() const;
    /** returns probability that hit is part of pattern (true muon) */
    double getProbability() const;

    /** returns radius */
    double getRadius() const;
    /** returns radius */
    double getAbs() const;
    /** returns theta */
    double getTheta() const;
    /** returns phi */
    double getPhi() const;

    /** hit is barrel or endcap (for curved track model) */
    bool isBarrel() const;
    /** phi sector of hit */
    int phiSector() const;
    /** ratio of the tracklength of the particle to which hit might belong would have traversed in magnetic field (for curved track model)
     */
    double getMagneticTrackRatio() const;

    /** hit measures phi? */
    bool getMeasuresPhi() const;

    /** return if hit already associated to pattern */
    bool getAssociated() const;

    /** set id */
    void setId(int id);

    /** set weight */
    void setWeight(double weight);

    /** set probability */
    void setProbability(double prob);

    /** set associated */
    void setAssociated(bool associated);

    /** return preprawdata */
    const Trk::PrepRawData* getPrd() const;

protected:
    /** Pointer to preprawdata */
    const Trk::PrepRawData* m_prd{nullptr};

    Amg::Vector3D m_pos{};
    /** unique id */
    int m_id = 0;

    /** radius of hit*/
    double m_radius{0.};
    /** absolute value of hit */
    double m_abs{0.};
    /** phi of hit */
    double m_phi{0.};
    /** theta of hit */
    double m_theta{0.};
    /** hit is barrel / endcap */
    bool m_barrel{true};
    /** phi sector (0,15), Atlas Convention */
    int m_phi_sector{0};
    /** ratio of the tracklength of the particle to which hit might belong would have traversed in magnetic field (for curved track model)
     */
    double m_magnetic_trackratio{0.};
    /** weight of hit in histogram, after rescaling*/
    double m_weight{1.};
    /** original weight */
    const double m_orig_weight{1.};
    /** probability that hit belongs to true muon (based on trigger confirmation and crude segment finding (used for mdts)) */
    double m_probability{1.};

    /** detector technology of hit */
    MuonHough::DetectorTechnology m_detector_id{MuonHough::MDT};

    /** hit measures phi? */
    bool m_measures_phi{false};

    /** hit associated to pattern */
    bool m_associated = false;

private:
    /** method that calculates phi sector of hit */
    int calcPhiSector() const;
    /** method that calculates 'magnetic track ratio' (for curved track model) */
    double calcMagneticTrackRatio() const;
};

inline bool operator==(const MuonHoughHit& hit1, const MuonHoughHit& hit2) {
    bool equal_to = 0;

    if (hit1.getHitx() == hit2.getHitx() && hit1.getHity() == hit2.getHity() && hit1.getHitz() == hit2.getHitz()) { equal_to = 1; }

    return equal_to;
}

inline MuonHough::DetectorTechnology MuonHoughHit::getDetectorId() const { return m_detector_id; }

inline int MuonHoughHit::getId() const { return m_id; }
inline const Amg::Vector3D& MuonHoughHit::getPosition() const { return m_pos; }

inline double MuonHoughHit::getHitx() const { return m_pos[Amg::x]; }
inline double MuonHoughHit::getHity() const { return m_pos[Amg::y]; }
inline double MuonHoughHit::getHitz() const { return m_pos[Amg::z]; }
inline double MuonHoughHit::getWeight() const { return m_weight; }
inline double MuonHoughHit::getOrigWeight() const { return m_orig_weight; }
inline double MuonHoughHit::getProbability() const { return m_probability; }

inline double MuonHoughHit::getRadius() const { return m_radius; }
inline double MuonHoughHit::getAbs() const { return m_abs; }
inline double MuonHoughHit::getTheta() const { return m_theta; }
inline double MuonHoughHit::getPhi() const { return m_phi; }

inline bool MuonHoughHit::isBarrel() const { return m_barrel; }
inline int MuonHoughHit::phiSector() const { return m_phi_sector; }
inline double MuonHoughHit::getMagneticTrackRatio() const { return m_magnetic_trackratio; }

inline bool MuonHoughHit::getMeasuresPhi() const { return m_measures_phi; }
inline bool MuonHoughHit::getAssociated() const { return m_associated; }

inline void MuonHoughHit::setId(int id) { m_id = id; }
inline void MuonHoughHit::setWeight(double weight) { m_weight = weight; }
inline void MuonHoughHit::setProbability(double prob) { m_probability = prob; }
inline void MuonHoughHit::setAssociated(bool associated) { m_associated = associated; }

inline const Trk::PrepRawData* MuonHoughHit::getPrd() const { return m_prd; }

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHHIT_H
