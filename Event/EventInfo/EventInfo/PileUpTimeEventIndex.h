/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* -C++- */
#ifndef EVENTINFO_PILEUPTIMEEVENTINDEX_H
#define EVENTINFO_PILEUPTIMEEVENTINDEX_H 1
/** @class PileUpTimeEventIndex
 *  @brief a struct encapsulating the identifier of a pile-up event
 **/
#include <string>
struct PileUpTimeEventIndex {
public:
  enum PileUpType { Unknown=-1, 
		    Signal=0, 
		    MinimumBias=1, 
		    Cavern=2, 
		    HaloGas=3, 
                    HighPtMinimumBias=4,
		    ZeroBias=5,
		    NTYPES };
  typedef long time_type;
  typedef unsigned long index_type;
  PileUpTimeEventIndex();
  PileUpTimeEventIndex(time_type time);
  PileUpTimeEventIndex(time_type time, index_type index);
  PileUpTimeEventIndex(time_type time, index_type index, PileUpType typ);

  ///bunch xing time in ns
  time_type  time() const;  

  ///the index of the component event in PileUpEventInfo
  index_type index() const;

  /// the pileup type - minbias, cavern, beam halo, signal?
  PileUpType type() const; 
  static const std::string& typeName(PileUpType typ);
  static PileUpType ushortToType(unsigned short);
private:
  short m_time;          // time of the pileup event with respect to BC=0
  short m_index;         // index of the pileup event
  PileUpType m_type;      // type of the pileup event
};

#include <iostream>
std::ostream& operator << (std::ostream&, const PileUpTimeEventIndex& index);

class MsgStream;
MsgStream& operator << (MsgStream&, const PileUpTimeEventIndex& index);
#endif
