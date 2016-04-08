// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTIMERTREEENTRY_H
#define TRIGTIMERTREEENTRY_H

#include "TrigTimeAlgs/TrigTimer.h"
#include "GaudiKernel/StatusCode.h"

class TTree;
class TBranch;

/**
 * \brief Helper class to allow storage of the timers in ROOT Trees.
 * Implementation is ~ to the TrigNtAlg but uses different backend (ROOT).
 * 
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>
 **/

class TrigTimeTreeEntry {
public:
  /**
   * @brief creates one entry
   * @param timer is timer to be placed in the tree
   * @param  doStamps saying that the absolute time values should also be recorded
   */
  TrigTimeTreeEntry ( TrigTimer* timer, bool doStamps );
  ~TrigTimeTreeEntry(){}
  TrigTimer * timer() const {return m_timer;}
  
  /**
   * @brief adds the timer the tree
   * @param tree is a tree in which timers caount should be stored
   * @param b branch which was asigned to that timer
   */
  StatusCode addToTree(TTree* tree, std::vector<TBranch*>& b );  
  void fill();    // fills the internal fields
  float getValue() const {return m_bData.timerValue;} 
  
private:
  TrigTimer* m_timer;  //!< pointer to the timers for which it was created
  bool m_doStamps;     //!< flag if absolute stamps to be done 

  /**
   * @brief TTree Branch definition for basic timer data
   */
  struct  BranchBaseData {
    float timerValue;
    long propValue;
  };

  /**
   * @brief TTree Branch definition for the absolute times staps data
   */
  struct BranchAbsData {
    struct timeval startTime;
    struct timeval stopTime;
  };
  struct BranchBaseData m_bData;   //!< starage for branches
  struct BranchAbsData m_absData;

};

#endif //TRIGTIMERTREEENTRY_H
