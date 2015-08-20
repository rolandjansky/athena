/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONHITTIMINGTOOL_H
#define MUON_IMUONHITTIMINGTOOL_H

/** 
    @class IMuonHitTimingTool

    Interface class for tools calculating the offset wrt the current bunch of a vector of MuonClusterOnTrack.
    These tools are used to identify out of time segments during the reconstruction.

    @author Niels.Van.Eldik@cern.ch
*/

#include "GaudiKernel/IAlgTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include <vector>
#include <limits>

namespace Muon {

  /** Interface ID for IMuonHitTimingTool*/  
  static const InterfaceID IID_IMuonHitTimingTool("Muon::IMuonHitTimingTool", 1, 0);

  class MuonClusterOnTrack;

  /** Tool to calculate the timing wrt to the current BC 
      Depending to the implementation the tool can handle different types of technologies BUT hits 
      for different technolgies should NEVER be passes into a single call to the tool. 
      If there are multiple technologies, uses will have to call the tool multiple times and combine the information
   */
  class IMuonHitTimingTool : virtual public IAlgTool {
  public:

    /** simple struct holding the result of the tool */
    struct TimingResult {
      /** default constructor, constructs an invalid TimingResult */
      TimingResult() : valid(false),time(std::numeric_limits<float>::lowest()),error(std::numeric_limits<float>::lowest()) {}

      /** full constructor */
      TimingResult(bool valid_,double time_, double error_) : valid(valid_),time(time_),error(error_) {}

      /** flag indicating whether the time was correctly calculated */
      bool valid;

      /** the actual time offset (ns) wrt the current bunch assuming the particle travelled in a straight line at the speed of light */
      float time;

      /** uncertainty on the time (ns) */
      float error;
    };
    
     /**Virtual destructor*/
    virtual ~IMuonHitTimingTool(){};
  
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMuonHitTimingTool; }
    
    /** Calculate the time offset of a given set of hits wrt to the current bunch 
        The hits SHOULD be of the same technology
     */
    virtual TimingResult calculateTimingResult( const std::vector<const MuonClusterOnTrack*>& hits ) const = 0;

    /** return a set of technologies accepted by the tool */
    virtual std::set<MuonStationIndex::TechnologyIndex> acceptedTechnologies() const = 0;
    
  };

}

#endif
