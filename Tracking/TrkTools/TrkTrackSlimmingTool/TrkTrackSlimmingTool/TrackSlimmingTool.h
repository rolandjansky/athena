/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKTRACKSLIMMINGTOOL_H
#define TRKTRACKSLIMMINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkTrack/Track.h"
#include <memory>
#include "CxxUtils/checker_macros.h"
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
      virtual StatusCode initialize() override;
      
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  () override;
      
      /**This method 'skims' interesting information from the passed track, and creates a 
       * new one with cloned copies of this information
       * When m_setPersistificationHints = False
	     @param track A const reference to the track to be skimmed. It will not be modified in any way.
	     @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is configured
       * When m_setPersistificationHints = True
       @param track A reference to the track to be skimmed.It gets modified by setting persistification hints
       @return nullptr
       The later behaviour can be not thread-safe , look method slimCopy below 
      */
      Trk::Track* slim  ATLAS_NOT_THREAD_SAFE (const Trk::Track& track) const override final;
      /**This method always creates a std::unique_ptr<Trk::Track*> with information removed
       * based on the tool configuration (m_setPersistificationHints is not used)
       @param track A const reference to the track to be skimmed. It will not be modified in any way.
       @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is configured
       */
      std::unique_ptr<Trk::Track> slimCopy(const Trk::Track& track) const override final;
      
       /**
        * Slim/skim a non const Track. (m_setPersistificationHints is not used)
        * @param track A reference to the track to be skimmed. It will be modified.
        */     
      void slimTrack(Trk::Track& track) const override final;
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
      
      void checkForValidMeas(const Trk::TrackStateOnSurface* tsos, bool& isIDmeas, bool& isMSmeas) const;
      

      void findLastValidTSoS(const DataVector<const Trk::TrackStateOnSurface>* oldTrackStates,
                             const Trk::TrackStateOnSurface*& lastValidIDTSOS,
                             const TrackStateOnSurface*& lastValidMSTSOS) const;
    
      bool keepParameters(const Trk::TrackStateOnSurface* TSoS,
                          const TrackStateOnSurface*& firstValidIDTSOS, 
                          const TrackStateOnSurface*& lastValidIDTSOS,
                          const TrackStateOnSurface*& firstValidMSTSOS,
                          const TrackStateOnSurface*& lastValidMSTSOS) const;


    }; 
} // end of namespace

#endif 
