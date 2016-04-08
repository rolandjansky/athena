/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          SecVtxInfo.h  -  Description
                             -------------------
    begin   : 04-06-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

 ***************************************************************************/

#ifndef JETTAGINFO_SECVTXINFO_H
#define JETTAGINFO_SECVTXINFO_H

#include "JetTagInfo/BaseTagInfo.h"
#include "Particle/TrackParticle.h"
#include <vector>
#include <iostream>

namespace Trk
{
  class RecVertex;
}

namespace Analysis
{
typedef std::vector<double> FloatVec;
typedef std::vector<std::string> StringVector;
typedef std::vector<std::string>::iterator StringIterator;
typedef std::vector<const Rec::TrackParticle*> TrackVec;

/** The SecVtxInfo class:
This class contains information of the secondary vertex fit within the jet.
It is not supposed to be an overly intelligent class with much overhead.
The class inherits from IBInfo and can therefor be put into the BJet as
additional info to the B-Jet.

Currently available methods to retrieve information:
  - distance(): Distance between the reconstructed primary vertex and the
    fitted secondary vertex. The distance is calculated in three dimensions.
  - probability(): The fit probability of the secondary vertex.
  - mass(): The reconstructed mass of the secondary vertex.
  - mult(): Number of tracks in the secondary vertex.
  - secVertexPos(): The fitted vertex and its errors on the position. The
    return type is Trk::RecVertex.
  - infoType(): Returns a enum with the type of information in the class.
    This is done so that the user directly can typecast from IBInfo to the
    correct concrete implementation (here: SecVtxInfo).

Methods to fill the class with information:
  - setSecVtx: Sets the secondary vertex position, the fit probability and
    the vector of fitted tracks.
  - setMult: Sets the multiplicity of the secondary vertex.
  - setMass: Sets the mass of the secondary vertex. */

class SecVtxInfo : public BaseTagInfo
{
public:
    enum FitType { TearDown=0, BuildUp=1 , NoFit=2 };
    /** default constructor */
    SecVtxInfo();

    /** constructor with infotype */
    SecVtxInfo(TagInfoType x);

    /** copy destructor */
    SecVtxInfo(const SecVtxInfo& rhs);

    /** assigenment operator */
    SecVtxInfo &operator= (const SecVtxInfo& rhs);

    /** default destructor */
    virtual ~SecVtxInfo();

    virtual SecVtxInfo* clone() const;

    /** The number of tracks available for the sec.vtx. fit after track sel. cuts.*/
    int numSelTracksForFit() const;

    /** The distance between the primary and the secondary vertex. Not implemented
    yet. Maybe needs two- and three dim distances. */
    double distance() const;
    double rphidistance() const;

    /** The fit probability of the fit. It is set to -1 it the fit did not work.*/
    double probability() const;

    /** The invariant mass of the tracks fitted to the vertex. */
    double mass() const;

    /** The energy fraction: Evtx/Ejet of charges tracks */
    double energyFraction() const;

    /** The number of tracks fitted in the vertex. */
    int mult()const;

    /** Returns an enum to indicate the type of fit. */
    FitType fitType() const; // Type of fit

    /** Returns the reconstructed secondary vertex. */
    const Trk::RecVertex& secVertexPos() const;
    
    /** Returns number of 2 track vertices */
    int NumberOfG2TrackVertices() const;

    /** Set-methods for the private data members of the class. */
    void setFitType(FitType type);
    void setSecVtx(const Trk::RecVertex& secVtx,
                   double fitProb,
                   const TrackVec& fittedTrks);
    void setNumSelTracksForFit(int numSelTracksForFit);
    void setMult(int mult);
    void setMass(double mass);
    void setEnergyFraction(double energyFraction);
    ////DRQ: Comments out since not implemented
    ////void setLikelihoods(double slh, double blh);
    void setDist(double dist);
    void setRPhiDist(double dist);
    void setNumberOfG2TrackVertices(int NSVPair);
    
private:
    int m_numSelTracksForFit; //!< The number of tracks available for the sec.vtx. fit after track sel. cuts
    double m_dist;
    double m_rphidist;
    double m_prob; // CumulativeChiSquare of vtx chi2
    double m_mass;
    double m_energyFraction;
    TrackVec m_fittedTrks;
    int m_mult;
    FitType m_fitType;
    Trk::RecVertex m_secVtxPos;
    int m_NGood2TrackVertices;
}; // End class

inline int SecVtxInfo::numSelTracksForFit() const
{
    return m_numSelTracksForFit;
}
inline double SecVtxInfo::distance() const
{
    return m_dist;
}
inline double SecVtxInfo::rphidistance() const
{
    return m_rphidist;
}
inline double SecVtxInfo::probability() const
{
    return m_prob;
}
inline double SecVtxInfo::mass() const
{
    return m_mass;
}
inline double SecVtxInfo::energyFraction() const
{
    return m_energyFraction;
}
inline int SecVtxInfo::mult() const
{
    return m_mult;
}
inline SecVtxInfo::FitType SecVtxInfo::fitType() const
{
    return m_fitType;
}
inline const Trk::RecVertex& SecVtxInfo::secVertexPos() const
{
    return m_secVtxPos;
}
inline void SecVtxInfo::setSecVtx(const Trk::RecVertex& secVtx,
                                  double fitProb, 
                                  const TrackVec& fittedTrks)
{
    m_secVtxPos=secVtx;
    m_prob=fitProb;
    m_fittedTrks=fittedTrks;
}
inline void SecVtxInfo::setNumSelTracksForFit(int numSelTracksForFit)
{
    m_numSelTracksForFit=numSelTracksForFit;
}
inline void SecVtxInfo::setMult(int mult)
{
    m_mult=mult;
}
inline void SecVtxInfo::setMass(double mass)
{
    m_mass=mass;
}
inline void SecVtxInfo::setEnergyFraction(double energyFraction)
{
    m_energyFraction = energyFraction;
}
inline void SecVtxInfo::setFitType(FitType type)
{
    m_fitType=type;
}
inline void SecVtxInfo::setDist(double dist)
{
    m_dist=dist;
}
inline void SecVtxInfo::setRPhiDist(double dist)
{
    m_rphidist=dist;
}
inline void SecVtxInfo::setNumberOfG2TrackVertices(int NGood2TrackVertices)
{
    m_NGood2TrackVertices=NGood2TrackVertices;
}
inline int SecVtxInfo::NumberOfG2TrackVertices() const
{
    return m_NGood2TrackVertices;
}
inline Analysis::SecVtxInfo* Analysis::SecVtxInfo::clone() const
{
    return new SecVtxInfo(*this);
}

} // End namespace
#endif
