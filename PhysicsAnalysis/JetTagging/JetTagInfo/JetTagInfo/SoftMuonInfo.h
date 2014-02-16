/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     SoftMuonInfo.cxx
PACKAGE:  offline/PhysicsAnalysis/BTaggingID/BTaggingUtils
PURPOSE:  EDM for b-tagging based on soft muon identification
********************************************************************/

#ifndef JETTAGINFO_SOFTMUONINFO_H
#define JETTAGINFO_SOFTMUONINFO_H
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/SMTrackInfo.h"
#include <vector>
#include <iostream>

namespace Analysis { 

  /**
  @class SoftMuonInfo
  Class for soft muon information
  @ author henri bachacou
  */

class SoftMuonInfo : public BaseTagInfo
{
public:
    /** default constructor */
    SoftMuonInfo();

    /** constructor with info type */
    SoftMuonInfo(TagInfoType x);
    
    /** copy destructor */
    SoftMuonInfo(const SoftMuonInfo& SoftMuonInfo);
    
    /** assigenment operator */
    SoftMuonInfo &operator= (const SoftMuonInfo& rhs);

    /** default destructor */
    virtual ~SoftMuonInfo();

    /** Return and set methods for the likelihood. */
    virtual SoftMuonInfo* clone() const;
   
    /** Number of muon track info objects stored */
    inline int numTrackInfo() const { return m_trackinfo.size();}
    /** Insert a new track info object */
    inline void addTrackInfo(const SMTrackInfo& i) { m_trackinfo.push_back(i); }
    /** Get a track info object */
    inline const SMTrackInfo& getTrackInfo(uint i) const { 
      return m_trackinfo.at(i);
    }

private:
    std::vector<SMTrackInfo> m_trackinfo;
}
; // End class

MsgStream& operator<<(MsgStream& out, const SoftMuonInfo&);
 
std::ostream& operator<<(std::ostream& out, const SoftMuonInfo&);

inline Analysis::SoftMuonInfo* Analysis::SoftMuonInfo::clone() const
{
    return new SoftMuonInfo(*this);
}
} // End namespace
#endif
