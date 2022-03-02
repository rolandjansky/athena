/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackHoleSearchTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRACKHOLESEARCHTOOL_H
#define ITRACKHOLESEARCHTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "AthContainers/DataVector.h"

namespace Trk
{
  class Track;
  class TrackStateOnSurface;

  static const InterfaceID IID_ITrackHoleSearchTool("Trk::ITrackHoleSearchTool", 1, 0);

  /** @brief interface for searching, counting and adding holes on
             tracks anywhere in ATLAS.

      A hole is defined as a sensitive detector element passed by
      the track without a hit assigned from it.

      @author Common Tracking Software Group
   */

  class ITrackHoleSearchTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    /** Input : track, partHyp
	Output: Changes in information
	This method fills the fields relevant to the hole counts in the vector information. These fields should be initialised to 0. 
	The relevant indices are specified by the enumeration in Tracking/TrkEvent/TrkTrackSummary.
	If problems occur, the information counters are reset to -1 flagging them as not set.
	The parthyp argument is relevant for the extrapolation steps in the hole search.
    */
    virtual void countHoles( 
			    const Trk::Track& track, 
			    std::vector<int>& information ,
			    const Trk::ParticleHypothesis partHyp = Trk::pion) const = 0;

    /** Input : track, parthyp
	Return: A DataVector containing pointers to TrackStateOnSurfaces which each represent an identified hole on the track.
	The parthyp argument is relevant for the extrapolation steps in the hole search.
	Attention: This is a factory, ownership of the return vector is passed to the calling method.
    */
    virtual const DataVector<const Trk::TrackStateOnSurface>* getHolesOnTrack(
									      const Trk::Track& track, 
									      const Trk::ParticleHypothesis partHyp = Trk::pion) const = 0;

    /** Input : track, parthyp
	Return: A pointer to a new Trk::Track which containes the information of the input track plus the tsos of the identified holes
	The parthyp argument is relevant for the extrapolation steps in the hole search.
	Attention: This is a factory, ownership of the return track is passed to the calling method.
    */
    virtual const Trk::Track*  getTrackWithHoles(
						 const Trk::Track& track, 
						 const Trk::ParticleHypothesis partHyp = Trk::pion) const = 0;
    

    /** Input : track, parthyp
	Return: A pointer to a new Trk::Track which containes the information of the input track plus the tsos of the identified holes or outliers
	The parthyp argument is relevant for the extrapolation steps in the hole search.
	Attention: This is a factory, ownership of the return track is passed to the calling method.
    */
    virtual const Trk::Track* getTrackWithHolesAndOutliers(
							   const Trk::Track& track, 
							   const Trk::ParticleHypothesis partHyp = Trk::pion) const = 0;

      
  };

  inline const InterfaceID& Trk::ITrackHoleSearchTool::interfaceID()
    { 
      return IID_ITrackHoleSearchTool; 
    }

} // end of namespace

#endif 
