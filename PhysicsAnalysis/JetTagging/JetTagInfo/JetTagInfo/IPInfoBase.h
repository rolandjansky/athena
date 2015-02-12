/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_IPINFOBASE_H
#define JETTAGINFO_IPINFOBASE_H
#include "JetTagInfo/BaseTagInfo.h"
#include <vector>
#include <iostream>

class MsgStream;
namespace Rec { class TrackParticle; }

namespace Analysis { 

  /**
  @class IPInfoBase
  Basic class to hold IP taggers results
  */

class IPInfoBase : public BaseTagInfo { 
public:
    /** default constructor */
    IPInfoBase();

    /** constructor with info type */
    IPInfoBase(TagInfoType x);
    
    /** copy destructor */
    IPInfoBase(const IPInfoBase& IPInfoBase);
    
    /** assigenment operator */
    IPInfoBase &operator= (const IPInfoBase& rhs);

    /** default destructor */
    virtual ~IPInfoBase();
    virtual IPInfoBase* clone() const;

    inline void nbTracks(int n) { m_ntrk = n; }
    inline int nbTracks() const { return m_ntrk; }
    
private:
    int m_ntrk; // number of tracks used

}
; // End class

/**output. This dumps the values of each of the possible summary enums*/
MsgStream& operator<<(MsgStream& out, const IPInfoBase&);
 
/**output. This dumps the values of each of the possible summary enums*/
std::ostream& operator<<(std::ostream& out, const IPInfoBase&);

inline Analysis::IPInfoBase* Analysis::IPInfoBase::clone() const { 
    return new IPInfoBase(*this);
}

} // End namespace
#endif
