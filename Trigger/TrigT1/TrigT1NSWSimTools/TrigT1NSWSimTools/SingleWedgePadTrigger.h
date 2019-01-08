// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSW_SINGLEPADWEDGETRIGGER_H
#define NSW_SINGLEPADWEDGETRIGGER_H

#include "TrigT1NSWSimTools/PadWithHits.h"
#include "TrigT1NSWSimTools/GeoUtils.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "TVector3.h"

#include <string>
#include <vector>

//forward declarations
class MsgStream;

/*!

A trigger with coincident hits in at least 3 out of 4 layers within a wedge.

davide.gerbaudo@gmail.com, April 2013
*/

namespace NSWL1 {
  class SingleWedgePadTrigger {

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
    static Polygon padOverlap3(const std::vector<PadWithHits> & pads);
    
  public:
    SingleWedgePadTrigger(const std::string &pattern,
                          const std::vector<PadWithHits> &pads,
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
    const std::vector<PadWithHits>& pads() const {return m_pads;}
    TVector3 direction() const;

  protected:
    /// Log a message using the Athena controlled logging system
    MsgStream& msg(MSG::Level lvl) const { return m_msg.get() << lvl; }

    /// Check whether the logging system is active at the provided verbosity level
    bool msgLvl(MSG::Level lvl) { return m_msg.get().level() <= lvl; }

    /// Private message stream member
    mutable Athena::MsgStreamMember m_msg;

  private:
    std::string m_pattern;
    EtaPhiHalf m_halfPadIndices;
    std::vector<size_t> m_padIndices;
    std::vector<PadWithHits> m_pads;
    bool m_alreadyCombined; //!< flag to keep track of whether this has already been combined w/ another wedge
  }; // end class SingleWedgePadTrigger

} // end namespace nsw

#endif
