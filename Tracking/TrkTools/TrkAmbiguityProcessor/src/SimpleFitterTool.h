/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimpleFitterTool_H
#define SimpleFitterTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"
#include "TrkParameters/TrackParameters.h"

#include <map>
#include <set>
#include <vector>
#include <functional>


class AtlasDetectorID;
class PixelID;

namespace InDet{
  class PixelCluster;
  class SCT_Cluster;
}

namespace Trk {

  class IPRD_AssociationTool;

  class SimpleFitterTool : public AthAlgTool, 
                           virtual public ITrackAmbiguityProcessorTool {
    public:

      // default methods
      SimpleFitterTool(const std::string&,const std::string&,const IInterface*);
      virtual ~SimpleFitterTool ();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      /**Returns a processed TrackCollection from the passed 'tracks'
       *     @param tracks collection of tracks which will have ambiguities resolved. Will not be 
       *     modified.
       *     The tracks will be refitted if no fitQuality is given at input.
       *     @return new collections of tracks, with ambiguities resolved. Ownership is passed on 
       *     (i.e. client handles deletion)
       */      
      virtual TrackCollection*  process(const TrackCollection* tracks);
    
      /** statistics output */
      virtual void statistics();

    private:
      
      void reset();
    
      /** refit PRDs */
      const Track* refitPrds( const Track* track);

      Trk::Track *fit(std::vector<const Trk::PrepRawData*> &raw,
                      const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const;
      
      template<typename... Args> 
      Trk::Track *fit(const Track &track, Args... args) const;
      bool _checkTrack(const Trk::Track *) const;
    
      // private data members

      /** brem recovery mode with brem fit ? */
      bool  m_tryBremFit;
      bool  m_caloSeededBrem;
      float m_pTminBrem;

      /** control material effects (0=non-interacting, 1=pion, 2=electron, 3=muon, 4=pion) read in as an integer 
      read in as an integer and convert to particle hypothesis */
      int m_matEffects;
      Trk::ParticleHypothesis m_particleHypothesis;
   
      /** refitting tool - used to refit tracks once shared hits are removed. 
          Refitting tool used is configured via jobOptions.*/
      ToolHandleArray<ITrackFitter> m_fitterTool;
      
      /** Track summary tool to fill the summary associated to the track*/
      ToolHandle<Trk::ITrackSummaryTool> m_trkSummaryTool;

      /**Association tool - used to work out which (if any) PRDs are shared between 
       tracks*/
      ToolHandle<IPRD_AssociationTool> m_assoTool;

      /**Tracks that will be passed out of AmbiProcessor. 
         Recreated anew each time process() is called*/ 
      TrackCollection* m_finalTracks;
      
      /** monitoring statistics */
      int m_Nevents;
      std::vector<int> m_Ncandidates, m_NcandScoreZero, m_NcandDouble,
      m_NscoreOk,m_NscoreZeroBremRefit,m_NscoreZeroBremRefitFailed,m_NscoreZeroBremRefitScoreZero,m_NscoreZero,
      m_Naccepted,m_NsubTrack,m_NnoSubTrack,m_NacceptedBrem,
      m_NbremFits,m_Nfits,m_NrecoveryBremFits,m_NgoodFits,m_NfailedFits;
      /** internal monitoring: categories for counting different types of extension results*/
      enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3, iForwrd = 4};
      std::vector<float>  m_etabounds;           //!< eta intervals for internal monitoring

      /** helper for monitoring and validation: does success/failure counting */
      void increment_by_eta(std::vector<int>&,const Track*,bool=true);

      bool m_rejectInvalidTracks;

  };

      inline
      Trk::Track *SimpleFitterTool::fit(std::vector<const Trk::PrepRawData*> &raw,
                                                           const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const {
         Trk::Track *new_track=nullptr;
         for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
              delete new_track;
              new_track=nullptr;
              new_track =  a_fitter->fit(raw, param, flag,hypo);
              if (Trk::SimpleFitterTool::_checkTrack(new_track)) {
                          return new_track;
              }
              ATH_MSG_WARNING( "The track fitter, " <<  a_fitter->name() << ", produced a track with an invalid covariance matrix." );
         }
         ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
         if (m_rejectInvalidTracks) {
             delete new_track;
             new_track=nullptr;
         }
         return new_track;
      }

      template<typename... Args>
      inline
      Trk::Track *SimpleFitterTool::fit(const Track &track, Args... args) const
      {
         Trk::Track *new_track=nullptr;
         for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
            delete new_track;
            new_track=nullptr;
            new_track =  a_fitter->fit(track,args...);
            if (Trk::SimpleFitterTool::_checkTrack(new_track)) {
               return new_track;
            }
            ATH_MSG_WARNING( "The track fitter, " <<  a_fitter->name() << ", produced a track with an invalid covariance matrix." );
         }
        ATH_MSG_WARNING( "None of the " <<  m_fitterTool.size() << " track fitter(s) produced a track with a valid covariance matrix." );
        if (m_rejectInvalidTracks) {
          delete new_track;
          new_track=nullptr;
        }
      return new_track;
      }

} //end ns


#endif // SimpleFitterTool_H

