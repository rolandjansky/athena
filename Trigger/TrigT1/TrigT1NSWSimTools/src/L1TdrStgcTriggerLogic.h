// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSW_L1TDRSTGCTRIGGERLOGIC_H
#define NSW_L1TDRSTGCTRIGGERLOGIC_H

#include "tdr_typedefs.h"

#include "TRandom.h"

#include <string>
#include <vector>

namespace nsw{
  class Pad;
  class PadWithHits;
  class SingleWedgePadTrigger;
  class SectorTriggerCandidate;
}

namespace nsw {
/**
  @brief Initial version of the stgc pad trigger logic

  This class contains the trigger logic for the stgc pads as implemented
  for the NSW TDR (CERN-LHCC-2013-006, https://cds.cern.ch/record/1552862).
  In particular, the naming convention and the trigger logic are
  described in ATL-MUON-INT-2014-003 https://cds.cern.ch/record/1694727.

  This class requires the input data to be formatted as a vector<PadWithHits>,
  and it provides as output a vector<SectorTriggerCandidate>.

  Imported from
  svn+ssh://svn.cern.ch/reps/atlasusr/ataffard/UpgradeNSW/NSWAna/NSWNtuple/trunk ,
  this class is in TrigT1NSWSimTools mainly meant to provide a reference.
  Note that all the 'legacy' code from the TDR study is in the 'nsw' namespace.
  (newer 'official' code is in the 'NSWL1' namespace).

  @todo add unit test
 
  Based on the original implementation by Shikma, sTGCTriggerLogic.
 
  davide.gerbaudo@gmail.com
  April 2013
*/
class L1TdrStgcTriggerLogic {

public:
	L1TdrStgcTriggerLogic();
	virtual ~L1TdrStgcTriggerLogic();
    /**
       @brief main function to compute trigger candidates

       @arg pads all active pads in this event
       @arg padIndicesThisSector indices of pads that should be
       considered to compute the triggers for this sector (i.e. after
       filtering by side+sector and eventually applying the
       efficiency)

     */
    bool buildSectorTriggers(const nsw::vpads_t &pads, const vsize_t  &padIndicesThisSector);
    /// access cached output of buildSectorTriggers()
    const nsw::vstcand_t& candidates() const {return m_secTrigCand;}
    /// simulate efficiency by dropping random pads (their indices)
    nsw::vsize_t removeRandomPadIndices(const nsw::vsize_t &padIndices);
    /**
       @brief trigger patterns that will be stored in the lookup table

       These are the original patterns we got in 2013 from D.Lellouch.
       They are probably obsolete and they should be updated
       \todo update trigger patterns
    */
    static std::vector<std::string> sTGC_triggerPatterns();
public:
    static bool hitPattern(const nsw::Pad &firstPad, const nsw::Pad &otherPad,
                           std::string &pattern, bool verbose);
	static bool hitPattern(const int &iEta0, const int &iPhi0,
                           const int &iEta1, const int &iPhi1,
                           std::string &pattern);
    static nsw::vswptrig_t buildSingleWedgeTriggers(const nsw::vpads_t &pads,
                                                    const nsw::vsize_t &padIndicesLayer0,
                                                    const nsw::vsize_t &padIndicesLayer1,
                                                    const nsw::vsize_t &padIndicesLayer2,
                                                    const nsw::vsize_t &padIndicesLayer3,
                                                    bool isLayer1, bool isLayer2,
                                                    bool isLayer3, bool isLayer4,
                                                    bool verbose=false);
    static nsw::vswptrig_t build3of4SingleWedgeTriggers(const nsw::vpads_t &pads,
                                                        const nsw::vsize_t &padIndicesLayer0,
                                                        const nsw::vsize_t &padIndicesLayer1,
                                                        const nsw::vsize_t &padIndicesLayer2,
                                                        const nsw::vsize_t &padIndicesLayer3,
                                                        bool verbose=false);
    static nsw::vswptrig_t build4of4SingleWedgeTriggers(const nsw::vpads_t &pads,
                                                        const nsw::vsize_t &padIndicesLayer0,
                                                        const nsw::vsize_t &padIndicesLayer1,
                                                        const nsw::vsize_t &padIndicesLayer2,
                                                        const nsw::vsize_t &padIndicesLayer3,
                                                        bool verbose=false);
public:
    bool m_verbose;
    bool m_writePickle; /// after computing the triggers, write the canditates to 'pickle' files (for event display)
    std::string m_picklePrefix; /// path where the pickle files will be written
protected:
 	TRandom rand;
    nsw::vstcand_t m_secTrigCand;
};
} // end namespace nsw
#endif // NSW_L1TDRSTGCTRIGGERLOGIC_H
