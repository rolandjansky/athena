/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_MUONOVERLAPDESCRIPTOR_H
#define REC_MUONOVERLAPDESCRIPTOR_H

#include <iostream>
#include <set>
#include "Identifier/Identifier.h"

class MsgStream;

namespace Rec {

  /** @class MuonOverlapDescriptor
      
      @brief A class describing ID, MS and hit overlaps,
             mind this class is TRANSIENT ONLY.

      @author Wolfgang Liebig

  */

  class MuonOverlapDescriptor {

  public:

    /** full constructor with some overlap information */
    MuonOverlapDescriptor ( bool SharesIDTrack, bool SharesSpectroTrack,
			    unsigned int SharedPrecisionHits,
			    unsigned int TotalPrecisionHits, // Really? The larger of the two # ?
			    unsigned int SharedSpectroPhiHits,
			    unsigned int TotalSpectroPhiHits,
                            std::set <Identifier> ms_intersection);

    /** default constructor */
    MuonOverlapDescriptor();

    /** destructor */
    ~MuonOverlapDescriptor();

    /** copy constructor */
    MuonOverlapDescriptor (const MuonOverlapDescriptor&);

    /** assignment operator */
    MuonOverlapDescriptor &operator= (const MuonOverlapDescriptor&);

    /** Access if the overlap consists of sharing the ID track */
    bool sharesIndetTrack()   const {return m_sharesIndetTrack;}

    /** Access if the overlap consists of sharing the MS track */
    bool sharesSpectroTrack() const {return m_sharesSpectroTrack;}

    /** Tell if there are shared MS precision shared hits */
    unsigned int nSharedPrecisionHits()   const {return m_sharedPrecisionHits; }
    /** Total MS precision hits to compare to */
    unsigned int nTotalPrecisionHits()    const {return m_totalPrecisionHits; }
    /** Tell if there are shared MS phi shared hits */
    unsigned int nSharedSpectroPhiHits()  const {return m_sharedSpectroPhiHits; }
    /** Total MS phi hits to compare to */
    unsigned int nTotalSpectroPhiHits()   const {return m_totalSpectroPhiHits; }
    /** set of shared identifier, currently for the muon spectrometer */
    std::set <Identifier> intersectionInSpectro() const {return m_intersection;}
   
    /** summary method */
    bool hasOverlap(unsigned int tolaratedSharedHits = 0) const;

  private:

    bool m_sharesIndetTrack;
    bool m_sharesSpectroTrack;
    unsigned int m_sharedPrecisionHits;
    unsigned int m_totalPrecisionHits;
    unsigned int m_sharedSpectroPhiHits;
    unsigned int m_totalSpectroPhiHits;
    std::set <Identifier> m_intersection;
  };

  /** Overload of << operator for MsgStream for debug output*/ 
  MsgStream& operator << ( MsgStream& sl, const MuonOverlapDescriptor& des);

  /** Overload of << operator for std::ostream for debug output*/ 
  std::ostream& operator << ( std::ostream& sl, const MuonOverlapDescriptor& des);
 
}
#endif // REC_MUONOVERLAPDESCRIPTOR_H


