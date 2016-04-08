/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          LifetimeInfo.h  -  Description
                             -------------------
    begin   : 04-06-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

 ***************************************************************************/

#ifndef JETTAGINFO_LIFETIMEINFO_H
#define JETTAGINFO_LIFETIMEINFO_H
#include "JetTagInfo/BaseTagInfo.h"
#include <vector>
#include <iostream>

namespace Analysis
{
typedef std::vector<double> FloatVec;
typedef std::vector<std::string> StringVector;
typedef std::vector<std::string>::iterator StringIterator;

  /** Class LifetimeInfo:
  Class to store information produced by the LifetimeTag.

  @author Andreas.Wildauer@cern.ch
  */
class LifetimeInfo : public BaseTagInfo
{
public:

    /** default constructor */
    LifetimeInfo();

    /** constructor with info type */
    LifetimeInfo(TagInfoType x);

    /** copy destructor */
    LifetimeInfo(const LifetimeInfo& LifetimeInfo);

    /** assigenment operator */
    LifetimeInfo &operator= (const LifetimeInfo& rhs);

    /** default destructor */
    virtual ~LifetimeInfo();

    /** clone method */
    virtual LifetimeInfo* clone() const;

    /** The signed 2D impact parameters. TODO: Add 3D impact parameters. */
    FloatVec signedIP() const;      //!< return signed Impact Parameters
    FloatVec significance() const;  //!< return significance (= signedIP/sigD0)
    void setIP(FloatVec ipVec);     //!< set signed impact parameter
    void setSignificance(FloatVec ipVec); //!< set significance

   void setTrackProb(const FloatVec& vec); //!< set Track probability (not ACTIVE)
   void setNTrackProb(double nTrackProb);  //!< set nTrack probability (nor ACTIVE)

   const FloatVec& vectorTrackProb(void) const; //!< return vector of Track probabilities (not ACTIVE)
   double nTrackProb(void) const;  //!< return nTrack probabilities (not ACTIVE)

private:
    FloatVec m_trackSIP;            //!< signed impact parameters
    FloatVec m_trackSignificance;   //!< significance (= signedIP/sigD0)

    FloatVec m_vectorOfTrackProb;   //!< vector of track probabilities (calculated but not in use)
    double m_nTrackProb;            //!< nTrack probability (calculated but not in use)
}; // End class


inline FloatVec LifetimeInfo::signedIP() const
{
    return m_trackSIP;
}
inline FloatVec LifetimeInfo::significance() const
{
    return m_trackSignificance;
}
inline void LifetimeInfo::setIP(FloatVec ipVec)
{
    m_trackSIP=ipVec;
    return;
}
inline void LifetimeInfo::setSignificance(FloatVec ipVec)
{
    m_trackSignificance=ipVec;
    return;
}
inline void LifetimeInfo::setTrackProb(const FloatVec& vec)
{
    m_vectorOfTrackProb=vec;
}
inline void LifetimeInfo::setNTrackProb(double nTrackProb)
{
    m_nTrackProb=nTrackProb;
}
inline const FloatVec& LifetimeInfo::vectorTrackProb(void) const
{
    return m_vectorOfTrackProb;
}
inline double LifetimeInfo::nTrackProb(void) const
{
    return m_nTrackProb;
}
inline Analysis::LifetimeInfo* Analysis::LifetimeInfo::clone() const
{
    return new LifetimeInfo(*this);
}
} // End namespace
#endif
