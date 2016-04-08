/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     SoftElectronInfo.cxx
PACKAGE:  offline/PhysicsAnalysis/BTaggingID/BTaggingUtils

AUTHORS:  F. Derue, A. Kaczmarska, M. Wolter
CREATED:  Nov 2005 

PURPOSE:  EDM for b-tagging based on soft lepton identification
       
COMMENTS: evolved from BJetLifetimeInfo

UPDATE:

********************************************************************/

#ifndef JETTAGINFO_SOFTELECTRONINFO_H
#define JETTAGINFO_SOFTELECTRONINFO_H
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/SETrackInfo.h"
#include <vector>
#include <iostream>

namespace Analysis
{
typedef std::vector<double> FloatVec;

class SoftElectronInfo : public BaseTagInfo
{
public:
    /** default constructor */
    SoftElectronInfo();

    /** constructor with info type */
    SoftElectronInfo(TagInfoType x);
    
    /** copy destructor */
    SoftElectronInfo(const SoftElectronInfo& SoftElectronInfo);
    
    /** assigenment operator */
    SoftElectronInfo &operator= (const SoftElectronInfo& rhs);

    /** default destructor */
    virtual ~SoftElectronInfo();

    /** Return and set methods for the likelihood. */
    virtual SoftElectronInfo* clone() const;

    /** Return and set methods for the signal and background likelihood. Both
    numbers are needed in order to make normalisation possible when calculating 
    the combined likelihood in the algorithm. */
    void setTrackProb(const FloatVec& vec);
    void setTrackProb(FloatVec&& vec);
    void setNTrackProb(double nTrackProb);
    const FloatVec& vectorTrackProb(void) const;
    double nTrackProb(void) const;

    /** Number of muon track info objects stored */
    inline int numTrackInfo() const { return m_trackinfo.size();}
    /** Insert a new track info object */
    inline void addTrackInfo(const SETrackInfo& i) { m_trackinfo.push_back(i); }
    /** Get a track info object */
    inline const SETrackInfo& getTrackInfo(uint i) const { 
      return m_trackinfo.at(i);
    }

private:
    FloatVec m_vectorOfTrackProb;
    double m_nTrackProb;
    std::vector<SETrackInfo> m_trackinfo;

}
; // End class


inline void SoftElectronInfo::setTrackProb(const FloatVec& vec)
{
    m_vectorOfTrackProb=vec;
}
inline void SoftElectronInfo::setTrackProb(FloatVec&& vec)
{
    m_vectorOfTrackProb=std::move(vec);
}
inline void SoftElectronInfo::setNTrackProb(double nTrackProb)
{
    m_nTrackProb=nTrackProb;
}
inline const FloatVec& SoftElectronInfo::vectorTrackProb(void) const
{
    return m_vectorOfTrackProb;
}
inline double SoftElectronInfo::nTrackProb(void) const
{
    return m_nTrackProb;
}
inline Analysis::SoftElectronInfo* Analysis::SoftElectronInfo::clone() const
{
    return new SoftElectronInfo(*this);
}

} // End namespace
#endif
