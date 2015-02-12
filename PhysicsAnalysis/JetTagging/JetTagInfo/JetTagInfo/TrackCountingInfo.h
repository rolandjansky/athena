/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_TRACKCOUNTINGINFO_H
#define JETTAGINFO_TRACKCOUNTINGINFO_H

#include "JetTagInfo/BaseTagInfo.h"

#include <iostream>

class MsgStream;
namespace Rec { class TrackParticle; }

namespace Analysis { 

  /**
  @class TrackCountingInfo

  Class to hold TrackCounting tagger results

  @author Thomas Goepfert <thomas.goepfert@cern.ch>
  */

class TrackCountingInfo : public BaseTagInfo { 

  public:
    /** default constructor */
    TrackCountingInfo();

    /** constructor with info type */
    TrackCountingInfo(TagInfoType x);
    
    /** copy constructor 
	not really needed now, but the object might get more complicated ...
    */
    TrackCountingInfo(const TrackCountingInfo& TrackCountingInfo);
    
    /** assigenment operator */
    TrackCountingInfo &operator= (const TrackCountingInfo& rhs);

    /** default destructor */
    virtual ~TrackCountingInfo();
    virtual TrackCountingInfo* clone() const;

    inline void setnTracks(int n)                     { m_ntrk = n; }
    inline void setd0sig_2nd(float d0sig_2nd)         { m_d0sig_2nd     = d0sig_2nd; }
    inline void setd0sig_abs_2nd(float d0sig_abs_2nd) { m_d0sig_abs_2nd = d0sig_abs_2nd; }
    inline void setd0sig_3rd(float d0sig_3rd)         { m_d0sig_3rd     = d0sig_3rd; }
    inline void setd0sig_abs_3rd(float d0sig_abs_3rd) { m_d0sig_abs_3rd = d0sig_abs_3rd; }

    inline int   nTracks()       const { return m_ntrk; }
    inline float d0sig_2nd()     const { return m_d0sig_2nd; }
    inline float d0sig_abs_2nd() const { return m_d0sig_abs_2nd; }
    inline float d0sig_3rd()     const { return m_d0sig_3rd; }
    inline float d0sig_abs_3rd() const { return m_d0sig_abs_3rd; }

  private:
    int m_ntrk;              //!< number of tracks used for tagging
    float m_d0sig_2nd;       //!< 2nd highest IP2D significance
    float m_d0sig_abs_2nd;   //!< 2nd highest absolute IP2D significance
    float m_d0sig_3rd;       //!< 3rd highest IP2D significance
    float m_d0sig_abs_3rd;   //!< 3rd highest absolute IP2D significance

  }; // End class

  /**output. This dumps the values of each of the possible summary enums*/
  MsgStream& operator<<(MsgStream& out, const TrackCountingInfo&);
  
  /**output. This dumps the values of each of the possible summary enums*/
  std::ostream& operator<<(std::ostream& out, const TrackCountingInfo&);
  
  inline Analysis::TrackCountingInfo* Analysis::TrackCountingInfo::clone() const { 
    return new TrackCountingInfo(*this);
  }
  
} // End namespace
#endif
