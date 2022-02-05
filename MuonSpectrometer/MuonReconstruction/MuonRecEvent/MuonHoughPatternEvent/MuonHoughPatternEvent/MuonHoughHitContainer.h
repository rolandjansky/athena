/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHHITCONTAINER_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHHITCONTAINER_H

#include "MuonHoughPatternEvent/MuonHoughHit.h"

namespace Trk {
    class PrepRawData;
}

class MuonHoughHitContainer {
    /** MuonHoughHitContainer does own its hits all added hits should be 'newed',
        except when m_ownhits==false */

public:
    /** constructor, flag for deleting hits at destructor */
    MuonHoughHitContainer(bool ownhits = true);

    /** destructor */
    ~MuonHoughHitContainer();

    /** returns Hit at position hitno */
    MuonHoughHit* getHit(int hitno) const;
    /** returns hit vector */
    std::vector<MuonHoughHit*> getHits() const;

    /** add hit to container */
    void addHit(MuonHoughHit* hit);

    /** remove hit from container */
    void removeHit(int hitno);

    /** returns hitid of hit hitno */
    int getHitId(int hitno) const;
    /** returns x position of hit hitno*/
    double getHitx(int hitno) const;
    /** returns y position of hit hitno*/
    double getHity(int hitno) const;
    /** returns z position of hit hitno*/
    double getHitz(int hitno) const;

    /** returns radius of hit hitno*/
    double getRadius(int hitno) const;
    /** returns theta of hit hitno*/
    double getTheta(int hitno) const;
    /** returns phi of hit hitno*/
    double getPhi(int hitno) const;
    /** returns weight of hit hitno*/
    double getWeight(int hitno) const;
    /** returns the orignal weight of hit hitno*/
    double getOrigWeight(int hitno) const;

    /** returns if hit hitno measures phi*/
    bool getMeasuresPhi(int hitno) const;
    /** returns preprawdata pointer of hit hitno*/
    const Trk::PrepRawData* getPrd(int hitno) const;

    /** returns detectortechnology in string of hit hitno*/
    std::string getWhichDetector(int hitno) const;
    /** returns detectortechnology of hit hitno*/
    MuonHough::DetectorTechnology getDetectorId(int hitno) const;
    /** returns size of hitcontainer */
    unsigned int size() const;
    /** returns if hitcontainer is empty */
    bool empty() const;
    /** allocates memory for hitvector*/
    void reserve(int size);

    /** returns number of rpc hits in container */
    int getRPChitno() const;
    /** returns number of rpc eta hits in container */
    int getRPCetahitno() const;
    /** returns number of mdt hits in container */
    int getMDThitno() const;
    /** returns number of csc hits in container */
    int getCSChitno() const;
    /** returns number of tgc hits in container */
    int getTGChitno() const;

protected:
    /** vector of hits in container */
    std::vector<MuonHoughHit*> m_hit;

    /** delete hits in destructor and create hits when added */
    const bool m_ownhits;
};

inline MuonHoughHit* MuonHoughHitContainer::getHit(int hitno) const { return m_hit[hitno]; }
inline std::vector<MuonHoughHit*> MuonHoughHitContainer::getHits() const { return m_hit; }
inline int MuonHoughHitContainer::getHitId(int hitno) const { return m_hit[hitno]->getId(); }
inline double MuonHoughHitContainer::getHitx(int hitno) const { return m_hit[hitno]->getHitx(); }
inline double MuonHoughHitContainer::getHity(int hitno) const { return m_hit[hitno]->getHity(); }
inline double MuonHoughHitContainer::getHitz(int hitno) const { return m_hit[hitno]->getHitz(); }
inline double MuonHoughHitContainer::getRadius(int hitno) const { return m_hit[hitno]->getRadius(); }

inline double MuonHoughHitContainer::getTheta(int hitno) const { return m_hit[hitno]->getTheta(); }
inline double MuonHoughHitContainer::getPhi(int hitno) const { return m_hit[hitno]->getPhi(); }
inline double MuonHoughHitContainer::getWeight(int hitno) const { return m_hit[hitno]->getWeight(); }
inline double MuonHoughHitContainer::getOrigWeight(int hitno) const { return m_hit[hitno]->getOrigWeight(); }

inline unsigned int MuonHoughHitContainer::size() const { return m_hit.size(); }
inline bool MuonHoughHitContainer::empty() const { return m_hit.empty(); }
inline void MuonHoughHitContainer::reserve(int size) { m_hit.reserve(size); }
inline MuonHough::DetectorTechnology MuonHoughHitContainer::getDetectorId(int hitno) const { return m_hit[hitno]->getDetectorId(); }
inline std::string MuonHoughHitContainer::getWhichDetector(int hitno) const { return m_hit[hitno]->getWhichDetector(); }
inline bool MuonHoughHitContainer::getMeasuresPhi(int hitno) const { return m_hit[hitno]->getMeasuresPhi(); }
inline const Trk::PrepRawData* MuonHoughHitContainer::getPrd(int hitno) const { return m_hit[hitno]->getPrd(); }

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHHITCONTAINER_H
