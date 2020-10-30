/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// MuonPatternCombination.h
//   Header file for class MuonPatternCombination
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUON_MUONPATTERNCOMBINATION_H
#define MUON_MUONPATTERNCOMBINATION_H

#include "MuonPattern/MuonPatternChamberIntersect.h"


#include "TrkParameters/TrackParameters.h"

#include <vector>

namespace Muon {

  /**
     @brief The MuonPatternCombination class provides the means to store the output of the initial 
            global pattern recognition in the muon spectrometer. 
     The parameters of the road (position, direction, momentum) are stored in form of a TrackParameter. 
     The hits are stored sorted per chambers in form a MuonPatternChamberIntersect objects.
  */
  class MuonPatternCombination {
  public:
    /** constructor taking as argmuents:
	@param parameter the road prediction (Trk::TrackParameter), ownership is transfered to the MuonPatternCombination, the user may pass a zero pointer 
	@param pat a list of associated MuonPatternChamberIntersect objects */
    MuonPatternCombination( const Trk::TrackParameters* parameter, 
			    const std::vector< MuonPatternChamberIntersect >& chamberData );
    
    MuonPatternCombination( const MuonPatternCombination& pat );

    MuonPatternCombination&  operator=( const MuonPatternCombination& pat );
    
    ~MuonPatternCombination();
  

   /** access to the global position, direction and if available momentum of the road, the pointer might be zero! */
    const Trk::TrackParameters* trackParameter() const;

    /** access to the MuonPatternChamberIntersect associated with the MuonPatternCombination */
    const std::vector< MuonPatternChamberIntersect >& chamberData() const;

  private:
    /** Position and direction of the road
	In addition the track parameter can provides a first estimate of the momentum */
    const Trk::TrackParameters* m_parameter;

    /** vector of hits per chamber */
    std::vector< MuonPatternChamberIntersect > m_chamberData;

    /** type of road **/
    int m_roadType;

  public:
    int trackRoadType() const;

  };

  inline const Trk::TrackParameters* MuonPatternCombination::trackParameter() const{
    return m_parameter;
  }

  inline const std::vector< MuonPatternChamberIntersect >& MuonPatternCombination::chamberData() const {
    return m_chamberData;
  }

  inline int MuonPatternCombination::trackRoadType() const {
    return m_roadType;
  }

}
 

#endif
