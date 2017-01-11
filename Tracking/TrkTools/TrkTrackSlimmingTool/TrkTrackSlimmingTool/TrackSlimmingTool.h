/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKTRACKSLIMMINGTOOL_H
#define TRKTRACKSLIMMINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"

class AtlasDetectorID;
class Identifier;

namespace Trk 
{
  class TrackStateOnSurface;

  /** @class TrackSlimmingTool 

    A tool to produce 'slimmed' Tracks from a reference track.

    By default, this slimmed track will include all the measurements, and the Perigee parameters (currently these
    are assumed to exist, but eventually an extrapolator should be used to provide it if the perigee is missing)

    @author  Edward Moyse <Edward.Moysecern.ch>
  */
  class TrackSlimmingTool : virtual public ITrackSlimmingTool, public AthAlgTool
    {
    public:
      TrackSlimmingTool(const std::string&,const std::string&,const IInterface*);
      
      /** default destructor */
      virtual ~TrackSlimmingTool ();
      
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /**This method 'skims' interesting information from the passed track, and creates a new one with cloned copies of this information
	 @param track A reference to the track to be skimmed. It will not be modified in any way.
	 @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is configured
      */
      Trk::Track* slim(const Trk::Track& track);
      
    private:
      /** any CaloDeposit with its adjacent MEOT's will be kept on the slimmed track (combined muon property) */
      bool m_keepCaloDeposit;
	
      /** If true, Outliers will be kept on the slimmed track*/
      bool m_keepOutliers;
      
      /** If true, the first and last parameters of ID & MS subsystems will be kept on the slimmed track*/
      bool m_keepParameters;

      /** Do not create slimmed tracks but only set hints for the persistifier to drop information while writing.
       */
      bool m_setPersistificationHints;
      
      /**atlas id helper*/
      const AtlasDetectorID* m_detID;
      
      void checkForValidMeas(const Trk::TrackStateOnSurface* tsos, bool& isIDmeas, bool& isMSmeas);
      
      void checkIfInDet(const Trk::TrackStateOnSurface* tsos, bool& isIDmeas);

    }; 
} // end of namespace

#endif 
