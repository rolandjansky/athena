/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        GbbNNTagInfo.h  -  Description
                             -------------------
    begin   : April 2011
    authors : Maria Laura Gonzalez Silva
    email   : laugs@cern.ch
    changes : May 2012 - update to new Likelihood Gbb tagger

 ***************************************************************************/

#ifndef JETTAGINFO_GBBNNTAGINFO_H
#define JETTAGINFO_GBBNNTAGINFO_H

#include "JetTagInfo/BaseTagInfo.h"
#include <vector>
#include <iostream>



namespace Trk
{
  class RecVertex;
}

namespace Analysis
{

 
class GbbNNTagInfo : public BaseTagInfo
{
public:
    /** default constructor */
    GbbNNTagInfo();

    /** constructor with infotype */
    GbbNNTagInfo(TagInfoType x);

    /** copy destructor */
    GbbNNTagInfo(const GbbNNTagInfo& rhs);

    /** assigenment operator */
    GbbNNTagInfo &operator= (const GbbNNTagInfo& rhs);

    /** default destructor */
    virtual ~GbbNNTagInfo();
    virtual GbbNNTagInfo* clone() const;

    // virtual GbbNNTagInfo* clone() const;

    /** A variable to store number of tracks matching jet */
    int nMatchingTracks() const;

    /** A second variable to store TrackJet Width */
    double trkJetWidth() const;

    /** A second variable to store Max{DeltaR(trk,trk)} */
    double trkJetMaxDeltaR() const;

  

    /** Set-methods for the private data members of the class. */
    void setnMatchingTracks(int nMatchingTracks);
    void settrkJetWidth(double trkJetWidth);
    void settrkJetMaxDeltaR(double trkJetMaxDeltaR);
    

private:

    int m_nMatchingTracks;
    double m_trkJetWidth;
    double m_trkJetMaxDeltaR;
   

}; // End class

inline void GbbNNTagInfo::setnMatchingTracks(int nMatchingTracks)
{
    m_nMatchingTracks=nMatchingTracks;
}

inline void GbbNNTagInfo::settrkJetWidth(double trkJetWidth)
{
  m_trkJetWidth= trkJetWidth;
}

inline void GbbNNTagInfo::settrkJetMaxDeltaR(double trkJetMaxDeltaR)
{
  m_trkJetMaxDeltaR= trkJetMaxDeltaR;
}
  
inline int GbbNNTagInfo::nMatchingTracks() const
{
    return m_nMatchingTracks;
}

inline double GbbNNTagInfo::trkJetWidth() const
{
  return m_trkJetWidth;
}

inline double GbbNNTagInfo::trkJetMaxDeltaR() const
{
  return m_trkJetMaxDeltaR;
}
 
inline Analysis::GbbNNTagInfo* Analysis::GbbNNTagInfo::clone() const
{
  return new GbbNNTagInfo(*this);
}
 


} // End namespace
#endif
