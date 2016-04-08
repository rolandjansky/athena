/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************-*-c++-*-*************************************
                        JetFitterTagInfo.h  -  Description
                             -------------------
    begin   : March 2007
    authors : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
    email   : giacinto.piacquadio@physik.uni-freiburg.de
              christian.weiser@physik.uni-freiburg.de
    changes :

 ***************************************************************************/

#ifndef JETTAGINFO_JETFITTERTAGINFO_H
#define JETTAGINFO_JETFITTERTAGINFO_H

#include "JetTagInfo/IJetFitterTagInfo.h"
#include <vector>
#include <iostream>

namespace Trk
{
  class RecVertex;
}

namespace Analysis
{
  //typedef std::vector<double> FloatVec;
  //typedef std::vector<std::string> StringVector;
  //typedef std::vector<std::string>::iterator StringIterator;
  //typedef std::vector<const Rec::TrackParticle*> TrackVec;

/** The JetFitterTagInfo class:
This class contains information of the secondary vertex fit within the jet.
It is not supposed to be an overly intelligent class with much overhead.
The class inherits from IBInfo and can therefor be put into the BJet as
additional info to the B-Jet.

Currently available methods to retrieve information:
  - number of vertices with more than two tracks
  - number of additional single track vertices
  - number of tracks at the vertices with more than two tracks (sum of them)
  - mean 3d significance of the vertices (if there), otherwise of the single 
    track vertices
  - mass (only if at least one vertex with more than one track)
  - energyFraction (fraction of charged tracks at vertices divided by all  
    charged tracks)
  - deltaeta: difference between eta of sum of momenta of all particle at vertices 
    and the fitted B-meson flight direction
  - deltaphi: same but with phi
  - infoType(): Returns a enum with the type of information in the class.
    This is done so that the user directly can typecast from IBInfo to the
    correct concrete implementation (here: JetFitterTagInfo).

    Methods to fill the class with information:
    - set of all the variables above... */
  
class JetFitterTagInfo : public virtual IJetFitterTagInfo
{
public:
    /** default constructor */
    JetFitterTagInfo();

    /** constructor with infotype */
    JetFitterTagInfo(TagInfoType x);

    /** copy destructor */
    JetFitterTagInfo(const JetFitterTagInfo& rhs);

    /** assigenment operator */
    JetFitterTagInfo &operator= (const JetFitterTagInfo& rhs);

    /** default destructor */
    virtual ~JetFitterTagInfo();

    virtual JetFitterTagInfo* clone() const;

    /** number of vertices with more than one track. */
    int nVTX() const;

    /** number of single track vertices */
    int nSingleTracks() const;

    /** number of tracks at vertices (with more than one track - sum) */
    int nTracksAtVtx() const;

    /** The energy fraction: Evtx/Ejet of charges tracks */
    double energyFraction() const;

    /** The invariant mass of the tracks fitted to the vertices with at least 2 tracks. */
    double mass() const;

    /** significance3d of all vertices with more than one track or -if not there-  */
    /** of all single track vertices */

    double significance3d() const;


    /** get deltaphi between sum of all momenta at vertices 
        and the fitted B-meson flight direction */
    double deltaphi() const;

    /** get deltaeta between sum of all momenta at vertices 
        and the fitted B-meson flight direction */
    double deltaeta() const;


    /** Set-methods for the private data members of the class. */
    void setnVTX(int nVTX);
    void setnSingleTracks(int nSingleTracks);
    void setnTracksAtVtx(int nTracksAtVtx);
    void setMass(double mass);
    void setEnergyFraction(double energyFraction);
    void setSignificance3d(double significance3d);
    void setDeltaeta(double deltaeta);
    void setDeltaphi(double deltaphi);
    //    void setLikelihoods(double slh, double blh);
    
private:
    int m_nVTX;
    int m_nSingleTracks;
    int m_nTracksAtVtx;
    double m_mass;
    double m_energyFraction;
    double m_significance3d;
    double m_deltaeta;
    double m_deltaphi;
}; // End class


inline int JetFitterTagInfo::nVTX() const
{
    return m_nVTX;
}

inline int JetFitterTagInfo::nSingleTracks() const
{
  return m_nSingleTracks;
}

inline int JetFitterTagInfo::nTracksAtVtx() const
{
  return m_nTracksAtVtx;
}

inline double JetFitterTagInfo::mass() const
{
    return m_mass;
}
inline double JetFitterTagInfo::energyFraction() const
{
    return m_energyFraction;
}
inline double JetFitterTagInfo::significance3d() const 
{
    return m_significance3d;
}

inline double JetFitterTagInfo::deltaeta() const 
{
    return m_deltaeta;
}

inline double JetFitterTagInfo::deltaphi() const 
{
    return m_deltaphi;
}

inline void JetFitterTagInfo::setnVTX(int nVTX)
{
  m_nVTX=nVTX;
}
inline void JetFitterTagInfo::setnSingleTracks(int nSingleTracks)
{
  m_nSingleTracks=nSingleTracks;
}

inline void JetFitterTagInfo::setnTracksAtVtx(int nTracksAtVtx)
{
  m_nTracksAtVtx=nTracksAtVtx;
}

inline void JetFitterTagInfo::setMass(double mass)
{
    m_mass=mass;
}
inline void JetFitterTagInfo::setEnergyFraction(double energyFraction)
{
    m_energyFraction = energyFraction;
}

inline void JetFitterTagInfo::setSignificance3d(double significance3d) 
{
  m_significance3d=significance3d;
}

inline Analysis::JetFitterTagInfo* Analysis::JetFitterTagInfo::clone() const
{
    return new JetFitterTagInfo(*this);
}

inline void JetFitterTagInfo::setDeltaeta(double deltaeta) {
   m_deltaeta=deltaeta;
}

inline void JetFitterTagInfo::setDeltaphi(double deltaphi) {
   m_deltaphi=deltaphi;
}

} // End namespace
#endif
