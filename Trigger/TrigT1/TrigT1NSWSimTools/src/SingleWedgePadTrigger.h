// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSW_SINGLEPADWEDGETRIGGER_H
#define NSW_SINGLEPADWEDGETRIGGER_H

#include "EtaPhiRectangle.h"

#include "TVector3.h"

#include <string>
#include <vector>

/*!

A trigger with coincident hits in at least 3 out of 4 layers within a wedge.

davide.gerbaudo@gmail.com, April 2013
*/

namespace nsw {
  class PadWithHits;
}

namespace nsw {
  class SingleWedgePadTrigger {
  public:
    typedef std::vector< nsw::PadWithHits >   vpads_t;
  public:
    struct EtaPhi{  // just to have better naming than std::pair
      int ieta, iphi;
      EtaPhi(int e, int p) : ieta(e), iphi(p) {}
      std::string str() const;
      std::string pickle() const { return str(); }
    private : EtaPhi(); // avoid gratuitous default constructor
    };
    struct EtaPhiHalf : EtaPhi { EtaPhiHalf(int e, int p) : EtaPhi(e,p){} }; // just to avoid mistakes
    //! Determine whether two single-wedge triggers are consistent with each other.
    /*! 'Consistent' here means that the difference in half-pad
      indices is what we expect for a track ~pointing to the center of
      the detector.

      \todo We need a way to check (enforce) that the single-wedge
      triggers are one inner and one outer.
     */
    static bool areInnerOuterConsistent(const EtaPhiHalf &inner,
                                        const EtaPhiHalf &outer,
                                        bool verbose=false);
    static EtaPhiHalf bandIndices(const EtaPhiHalf &inner, const EtaPhiHalf &outer);
    //! area that is overlapping between the pads that cause the trigger (pads are staggered)
    static nsw::EtaPhiRectangle padOverlap(const vpads_t &pads);
    //! direction of this trigger wrt. the origin
    /*!
      Direction defined by the origin of the detector and the center of
      the quarter-pad area (corresponding to the overlap with the
      other pads) on the first pad
    */
    static TVector3 direction(const nsw::PadWithHits &firstPad,
                              const nsw::EtaPhiRectangle &overlap);
  public:
    SingleWedgePadTrigger(const std::string &pattern,
                          const vpads_t &pads,
                          const std::vector<size_t> &padIndices);
    const std::string& pattern() const {return m_pattern;}
    const std::vector<size_t> padIndices() const {return m_padIndices;}
    EtaPhiHalf halfPadCoordinates() const;
    //! use the first pad to determine whether it's a small/large sector
    bool isSmallSector() const;
    bool isLargeSector() const { return !isSmallSector(); }
    float avgEtaFromFirstPad() const;
    //! determine whether we are in the transition region
    /*! The current implementation relies on the information from the
      first pad DG In the future, we might want to use some geometric
      pad info (center?)...good enough for now
     */
    bool isInTransitionRegion() const;
    bool is4outOf4Layers() const { return (m_pads.size()==4); }
    SingleWedgePadTrigger& setCombined() { m_alreadyCombined=true; return *this;}
    bool alreadyCombined() const { return m_alreadyCombined; }
    std::string pickle() const; //!< simple dict-like representation
    const vpads_t& pads() const {return m_pads;}
    TVector3 direction() const;
  private:
    std::string m_pattern;
    EtaPhiHalf m_halfPadIndices;
    std::vector<size_t> m_padIndices;
    vpads_t m_pads;
    bool m_alreadyCombined; //!< flag to keep track of whether this has already been combined w/ another wedge
  }; // end class SingleWedgePadTrigger

} // end namespace nsw

#endif
