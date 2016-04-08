/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_JETPROBINFOBASE_H
#define JETTAGINFO_JETPROBINFOBASE_H
#include "JetTagInfo/BaseTagInfo.h"
#include <vector>
#include <iostream>

class MsgStream;
namespace Rec { class TrackParticle; }

namespace Analysis { 

  /**
  @class JetProbInfoBase
  Basic class to hold JetProb taggers results
  */

class JetProbInfoBase : public BaseTagInfo { 
public:
    /** default constructor */
    JetProbInfoBase();

    /** constructor with info type */
    JetProbInfoBase(TagInfoType x);
    
    /** copy destructor */
    JetProbInfoBase(const JetProbInfoBase& JetProbInfoBase);
    
    /** assigenment operator */
    JetProbInfoBase &operator= (const JetProbInfoBase& rhs);

    /** default destructor */
    virtual ~JetProbInfoBase();
    virtual JetProbInfoBase* clone() const;

    inline void nbTracks(int n) { m_ntrk = n; }
    inline int nbTracks() const { return m_ntrk; }
    
private:
    int m_ntrk; // number of tracks used

}
; // End class

/**output. This dumps the values of each of the possible summary enums*/
MsgStream& operator<<(MsgStream& out, const JetProbInfoBase&);
 
/**output. This dumps the values of each of the possible summary enums*/
std::ostream& operator<<(std::ostream& out, const JetProbInfoBase&);

inline Analysis::JetProbInfoBase* Analysis::JetProbInfoBase::clone() const { 
    return new JetProbInfoBase(*this);
}

} // End namespace
#endif
