/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_IPINFOPLUS_H
#define JETTAGINFO_IPINFOPLUS_H
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/IPTrackInfo.h"
#include <vector>
#include <iostream>

class MsgStream;
namespace Rec { class TrackParticle; }

namespace Analysis {

   /**
   @class IPInfoPlus
   This class contains additional information for tags based on impact parameter, 
   like pointers to the TrackParticles used in the tag, impact parameter at the primary vertex, ...
  
   @author laurent vacavant 
   */

class IPInfoPlus : public BaseTagInfo { 
public:
    /** Default constructor */
    IPInfoPlus();
    /** Constructors from tag type */
    IPInfoPlus(TagInfoType x);
    IPInfoPlus(const IPInfoPlus& IPInfoPlus);
    IPInfoPlus &operator= (const IPInfoPlus& rhs);
    virtual ~IPInfoPlus();
    virtual IPInfoPlus* clone() const;
  
    /** Number of track info objects stored */
    inline int numTrackInfo() const { return m_trackinfo.size();}
    /** Insert a new track info object */
    inline void addTrackInfo(const IPTrackInfo& i) { m_trackinfo.push_back(i); }
    /** Get a track info object */
    inline const IPTrackInfo& getTrackInfo(uint i) const { 
      return m_trackinfo.at(i);
    }
    /** Update the track weights for a trackparticle */
    void updateTrackWeight(const Rec::TrackParticle* trk, std::string view, double w);

private:
    std::vector<IPTrackInfo> m_trackinfo;

};

MsgStream& operator<<(MsgStream& out, const IPInfoPlus&);
 
std::ostream& operator<<(std::ostream& out, const IPInfoPlus&);

inline IPInfoPlus* IPInfoPlus::clone() const { 
    return new IPInfoPlus(*this);
}

}
#endif // JETTAGINFO_IPINFOPLUS
