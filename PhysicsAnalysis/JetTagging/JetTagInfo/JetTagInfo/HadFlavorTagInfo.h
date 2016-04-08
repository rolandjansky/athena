/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        HadFlavorTagInfo.h  -  Description
                             -------------------
    begin   : April 2011
    authors : Maria Laura Gonzalez Silva
    email   : laugs@cern.ch
    changes :

 ***************************************************************************/

#ifndef JETTAGINFO_HADFLAVORTAGINFO_H
#define JETTAGINFO_HADFLAVORTAGINFO_H

#include "JetTagInfo/BaseTagInfo.h"
#include <vector>
#include <iostream>



namespace Trk
{
  class RecVertex;
}

namespace Analysis
{

 
class HadFlavorTagInfo : public BaseTagInfo
{
public:
    /** default constructor */
    HadFlavorTagInfo();

    /** constructor with infotype */
    HadFlavorTagInfo(TagInfoType x);

    /** copy destructor */
    HadFlavorTagInfo(const HadFlavorTagInfo& rhs);

    /** assigenment operator */
    HadFlavorTagInfo &operator= (const HadFlavorTagInfo& rhs);

    /** default destructor */
    virtual ~HadFlavorTagInfo();
    virtual HadFlavorTagInfo* clone() const;

    // virtual HadFlavorTagInfo* clone() const;

    /** A variable to store number of tracks matching jet */
    int nMatchingTracks() const;

    /** A second variable to store TrackJet Width */
    double trkJetWidth() const;

    /** A second variable to store Max{DeltaR(trk,trk)} */
    double trkJetDRKtaxes() const;

  

    /** Set-methods for the private data members of the class. */
    void setnMatchingTracks(int nMatchingTracks);
    void settrkJetWidth(double trkJetWidth);
    void settrkJetDRKtaxes(double trkJetDRKtaxes);
    

private:

    int m_nMatchingTracks;
    double m_trkJetWidth;
    double m_trkJetDRKtaxes;
   

}; // End class

inline void HadFlavorTagInfo::setnMatchingTracks(int nMatchingTracks)
{
    m_nMatchingTracks=nMatchingTracks;
}

inline void HadFlavorTagInfo::settrkJetWidth(double trkJetWidth)
{
  m_trkJetWidth= trkJetWidth;
}

inline void HadFlavorTagInfo::settrkJetDRKtaxes(double trkJetDRKtaxes)
{
  m_trkJetDRKtaxes= trkJetDRKtaxes;
}
  
inline int HadFlavorTagInfo::nMatchingTracks() const
{
    return m_nMatchingTracks;
}

inline double HadFlavorTagInfo::trkJetWidth() const
{
  return m_trkJetWidth;
}

inline double HadFlavorTagInfo::trkJetDRKtaxes() const
{
  return m_trkJetDRKtaxes;
}
 
inline Analysis::HadFlavorTagInfo* Analysis::HadFlavorTagInfo::clone() const
{
  return new HadFlavorTagInfo(*this);
}
 


} // End namespace
#endif
