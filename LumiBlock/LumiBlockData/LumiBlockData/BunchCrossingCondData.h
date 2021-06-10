// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/BunchCrossingCondData.h
 * @author Walter Lampl <walter.lampl@cern.ch>
 * @date Oct 2019
 * @brief Replaces the BunchCrossing AlgTool used in run1/2
 */


#ifndef LUMIBLOCKDATA_BUNCHCROSSINGCONDDATA_H
#define LUMIBLOCKDATA_BUNCHCROSSINGCONDDATA_H


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <bitset>


class BunchCrossingCondData {

public:

  typedef unsigned int bcid_type;
  static constexpr int m_MAX_BCID=3564;
  static constexpr int m_BUNCH_SPACING = 25;


  /// The simplest query: Is the bunch crossing filled or not?
  /**
   * This is the simplest question that one can ask the configuration:
   * Was the current BCID coming from a collision or not? If people
   * properly use the triggers to select their events, then this function
   * is not too useful, but for some special analyses it might be a good
   * idea to have it.
   *
   * @param bcid The bcid that should be checked
   * @returns <code>true</code> if the bcid was a collision bunch crossing,
   *          <code>false</code> otherwise
   */
  bool isFilled(const bcid_type bcid ) const;
    

  /// Function deciding if a given bunch crossing is in a filled train
  /**
   * Most of the functionality of the interface only makes sense for bunches
   * which were in trains. So this function can be checked at the beginning
   * of the analysis code, to make sure that the BC in question is interesting.
   *
   * @param bcid The bcid that should be checked
   * @returns <code>true</code> if the bcid is part of a bunch train,
   *          <code>false</code> otherwise
   */
  bool isInTrain(const bcid_type bcid ) const;

  /// Function deciding if a given bunch crossing has an unpaired bunch
  /**
   * This function can be used to quickly decide whether the current event
   * came from an unpaired bunch. Note that the function doesn't make a difference
   * between "isolated" and "non isolated" unpaired bunches. You have to use
   * the <code>gapBeforeBunch</code> function to decide how isolated an
   * unpaired bunch is.
   *
   * @param bcid The bcid that should be checked
   * @returns <code>true</code> if the bcid belongs to an unpaired bunch,
   *          <code>false</code> otherwise
   */
  bool isUnpaired(const bcid_type bcid ) const;

  /// Function deciding if there was a bunch from "beam 1" in this bunch crossing
  /**
   * This function is useful for differentiating the unpaired bunch
   * crossings based on which beam provided the unpaired bunch.
   *
   * @param bcid The bcid that should be checked
   * @returns <code>true</code> if the bcid contained a bunch from beam 1,
   *          <code>false</code> otherwise
   */
  bool isBeam1(const bcid_type bcid ) const;

  /// Function deciding if there was a bunch from "beam 2" in this bunch crossing
  /**
   * This function is useful for differentiating the unpaired bunch
   * crossings based on which beam provided the unpaired bunch.
   *
   * @param bcid The bcid that should be checked
   * @returns <code>true</code> if the bcid contained a bunch from beam 2,
   *          <code>false</code> otherwise
   */
  bool isBeam2(const bcid_type bcid ) const;

  /// Enumeration type for a given bunch crossing
  /**
  * This enumeration can specify what kind of bunch crossing one BCID
  * belongs to. The types could easily be extended later on.
  */
  enum BunchCrossingType
  {
    Empty = 0,       ///< An empty bunch far away from filled bunches
    FirstEmpty = 1,  ///< The first empty bunch after a train
    MiddleEmpty = 2, ///< An empty BCID in the middle of a train
    Single = 100,    ///< This is a filled, single bunch (not in a train)
    Front = 200,     ///< The BCID belongs to the first few bunches in a train
    Middle = 201,    ///< The BCID belongs to the middle bunches in a train
    Tail = 202,      ///< The BCID belongs to the last few bunces in a train
    Unpaired = 300   ///< This is an unpaired bunch (either beam1 or beam2)
  };

  /// Convenience function for the type of the specific bunch crossing
  /**
   * This function could be used as a single entry point to this data in
   * principle. It gives a summary about a specific BCID. Remember however
   * that the exact meaning of many of the return values of this function
   * are configuration dependent.
   *
   * @param bcid The bcid that should be checked
   * @returns The type of the bcid in question
   */
  BunchCrossingType bcType(const bcid_type bcid ) const;

  /// Enumeration specifying the units in which to expect the bunch distance type
  /**
   * To make it clear for the following functions what units to interpret their
   * return values in, it is possible to request their return values in different
   * units.
   */
  enum BunchDistanceType {
    NanoSec, ///< Distance in nanoseconds
    BunchCrossings, ///< Distance in units of 25 nanoseconds
    /// Distance in units of filled bunches (depends on filling scheme)
    FilledBunches
  };

  /// Gap before the train this BCID is in
  /**
   * Get the gap that's between the train that the specified BCID is in, and
   * the previous train. This is a useful number for some jet/MET studies.
   *
   * Note that the function doesn't work with the FilledBunches type, as the
   * size of the gaps doesn't have to be a multiple of the bunch distance
   * within the trains.
   *
   * Returns "-1" when there's no right answer to the question. (BCID not
   * part of a train.)
   *
   * @param bcid The bcid whose train should be investigated
   * @param type The type of the requested return value
   * @returns The gap before the train of the specified bcid
   */
  int gapBeforeTrain( bcid_type bcid = 0,
		      BunchDistanceType type = NanoSec ) const;

  /**
   * Get the distance of the specified bunch crossing to the preceeding 
   * filled bunch. 
   *
   * The distance can be returned in multiple units. By default it is
   * returned in nanoseconds. If one specifies BunchCrossings, then the
   * distance is returned in nanosec_distance / 25. Finally, if one
   * specifies FilledBunches, then the distance is returned in the units of
   * the bunch spacing within the train.
   *
   * @param bcid The bcid that should be checked
   * @param type The type of the requested return value
   * @returns Distance of the bcid to the preceeding filled bunch
   *          
   */
  int gapBeforeBunch( bcid_type bcid = 0,
		      BunchDistanceType type = NanoSec ) const;
  

  /// Gap after the train this BCID is in
  /**
   * Get the gap that's between the train that the specified BCID is in, and
   * the next train. This is a useful number for some jet/MET studies.
   *
   * Note that the function doesn't work with the FilledBunches type, as the
   * size of the gaps doesn't have to be a multiple of the bunch distance
   * within the trains.
   *
   * Returns "-1" when there's no right answer to the question. (BCID not
   * part of a train.)
   *
   * @param bcid The bcid whose train should be investigated
   * @param type The type of the requested return value
   * @returns The gap after the train of the specified bcid
   */
  int gapAfterTrain( bcid_type bcid = 0,
		     BunchDistanceType type = NanoSec ) const;

  /**
   * Get the distance of the specified bunch crossing to the next 
   * filled bunch. 
   *
   * The distance can be returned in multiple units. By default it is
   * returned in nanoseconds. If one specifies BunchCrossings, then the
   * distance is returned in nanosec_distance / 25. Finally, if one
   * specifies FilledBunches, then the distance is returned in the units of
   * the bunch spacing within the train.
   *
   * @param bcid The bcid that should be checked
   * @param type The type of the requested return value
   * @returns The gap after the train of the specified bcid
   *         
   */

  int gapAfterBunch( bcid_type bcid = 0,
		     BunchDistanceType type = NanoSec ) const;
 

  /// The distance of the specific bunch crossing from the front of the train
  /**
   * Get the distance of the specified bunch crossing from the front of the
   * bunch train. If the specified bunch crossing is not part of a bunch
   * train, then the function returns -1.
   *
   * The distance can be returned in multiple units. By default it is
   * returned in nanoseconds. If one specifies BunchCrossings, then the
   * distance is returned in nanosec_distance / 25. Finally, if one
   * specifies FilledBunches, then the distance is returned in the units of
   * the bunch spacing within the train.
   *
   * @param bcid The bcid that should be checked
   * @param type The type of the requested return value
   * @returns The distance of the bcid in question from the front of its
   *          bunch train
   */
  int distanceFromFront( const bcid_type bcid,
			 const BunchDistanceType type = NanoSec ) const;

  /// The distance of the specific bunch crossing from the tail of the train
  /**
   * Get the distance of the specified bunch crossing from the tail of the
   * bunch train. If the specified bunch crossing is not part of a bunch
   * train, then the function returns -1.
   *
   * The distance can be returned in multiple units. By default it is
   * returned in nanoseconds. If one specifies BunchCrossings, then the
   * distance is returned in nanosec_distance / 25. Finally, if one
   * specifies FilledBunches, then the distance is returned in the units of
   * the bunch spacing within the train.
   *
   * @param bcid The bcid that should be checked
   * @param type The type of the requested return value
   * @returns The distance of the bcid in question from the tail of its
   *          bunch train
   */
  int distanceFromTail( const bcid_type bcid,
			const BunchDistanceType type = NanoSec ) const;

  /// Gap before the train this BCID is in
  /**
   * Get the gap that's between the train that the specified BCID is in, and
   * the previous train. This is a useful number for some jet/MET studies.
   *
   * Note that the function doesn't work with the FilledBunches type, as the
   * size of the gaps doesn't have to be a multiple of the bunch distance
   * within the trains.
   *
   * Returns "-1" when there's no right answer to the question. (BCID not
   * part of a train.)
   *
   * @param bcid The bcid whose train should be investigated
   * @param type The type of the requested return value
   * @returns The gap before the train of the specified bcid
   */

  unsigned int numberOfFilledBunches() const;

  /// Get the number of unpaired bunches in the current configuration
  /**
   * This function gives the total number of unpaired bunches in the current
   * configuration. This could come in handy in some circumstances.
   *
   * @returns The total number of unpaired bunches in the machine
   */
  unsigned int numberOfUnpairedBunches() const;
  
  /// Get the number of the bunch trains in the current configuration
  /**
   * This function gives the total number of bunch trains that were
   * identified in the current configuration. This could come in handy in
   * some circumstances.
   *
   * @returns The total number of bunch trains in the machine
   */
  unsigned int numberOfBunchTrains() const;


  /// Get colliding and total bcids in a train
  /**
   * This function gives the number of colliding bunches and the total 
   * number of bcids in a given train
   *
   * @param bcid The bcid whose train should be investigated
   * @returns a pair<colliding,total> bcids in this train
   */
  std::pair<unsigned,unsigned> bunchTrainPopulation(const bcid_type bcid) const;


private:

  friend class BunchCrossingCondAlg;// The cond-alg filling this class


  // Data
  std::bitset<m_MAX_BCID> m_beam1;
  std::bitset<m_MAX_BCID> m_beam2;
  std::bitset<m_MAX_BCID> m_luminous;
  const static int m_headTailLength = 300; // magic number 300 ns from Run 2 tool

  struct bunchTrain_t {
    bunchTrain_t(bcid_type first, bcid_type last, unsigned ncoll) : 
      m_first(first),m_last(last),m_nColl(ncoll) {};
    int m_first;      ///First BCID of this train
    int m_last;       ///Last BCID of this train
    unsigned m_nColl; ///Number of colliding bunches in this train
  };

  std::vector<bunchTrain_t> m_trains;

  unsigned m_nTrains; //might be one less than the size of m_trains

  ///Helper method to find the train of a bcid (nullptr if bcd is not in a train)
  const bunchTrain_t* findTrain(const bcid_type bcid) const;

  ///Helper method to count colliding bunches in the range from-to
  unsigned countColliding(int from, int to) const;

};  

CLASS_DEF (BunchCrossingCondData, 158463439, 0)
CONDCONT_DEF (BunchCrossingCondData, 157905099 );

inline 
bool BunchCrossingCondData::isFilled(const bcid_type bcid) const {
  if (ATH_UNLIKELY(bcid>=m_MAX_BCID)) return false;
  return m_luminous.test(bcid);
}

inline
bool  BunchCrossingCondData::isBeam1(const bcid_type bcid) const {
  if (ATH_UNLIKELY(bcid>=m_MAX_BCID)) return false;
  return m_beam1.test(bcid);
}

inline
bool BunchCrossingCondData::isBeam2(const bcid_type bcid) const {
  if (ATH_UNLIKELY(bcid>=m_MAX_BCID)) return false;
  return  m_beam2.test(bcid);
}


inline 
bool  BunchCrossingCondData::isUnpaired(const bcid_type bcid) const {
  if (ATH_UNLIKELY(bcid>=m_MAX_BCID)) return false;
  return ((m_beam1.test(bcid) || m_beam2.test(bcid)) && !m_luminous.test(bcid)); 

}

inline 
unsigned BunchCrossingCondData::numberOfFilledBunches() const {
  return m_luminous.count();
}


inline 
unsigned int BunchCrossingCondData::numberOfBunchTrains() const {
  return m_trains.size();
}

inline 
bool BunchCrossingCondData::isInTrain(const bcid_type bcid ) const {
  return (findTrain(bcid) != nullptr);
}

#endif // not COOLLUMIUTILITIES_FILLPARAMSCONDDATA_H
