/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DenseEnvironmentsAmbiguityProcessorTool_H
#define DenseEnvironmentsAmbiguityProcessorTool_H

// turn on debugging ? uncomment this
// #define SIMPLEAMBIGPROCDEBUGCODE

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "TrkValInterfaces/ITrkObserverTool.h"
#include "TrkAmbiguityProcessor/dRMap.h"


//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"
#include "TrkParameters/TrackParameters.h"

#include <map>
#include <set>
#include <vector>
#include <functional>


#ifdef SIMPLEAMBIGPROCDEBUGCODE
  // --------------- DEBUG CODE
  #include "HepMC/GenEvent.h"
  #include "TrkTruthData/PRD_MultiTruthCollection.h"
  #include "GeneratorObjects/McEventCollection.h"
  #include "TrkTruthData/TrackTruthCollection.h"
  typedef std::map<const Trk::Track*, const Trk::Track*> TrackCollectionConnection;

#ifndef SIMPLEAMBIGPROCDEBUGCODE_CLASS_DEF
#define SIMPLEAMBIGPROCDEBUGCODE_CLASS_DEF
  CLASS_DEF( TrackCollectionConnection , 148639440 , 1 )
#endif

#endif



class AtlasDetectorID;
class PixelID;

namespace InDet{
  class IPixelClusterSplitProbTool;
  class PixelCluster;
  class SCT_Cluster;
}

namespace Trk {

  class ITrackScoringTool;
  class IPRD_AssociationTool;
  class ITruthToTrack;
  class IExtrapolator;

  class DenseEnvironmentsAmbiguityProcessorTool : public AthAlgTool, 
                                                  virtual public ITrackAmbiguityProcessorTool
    {
    public:

      // public types
      typedef std::pair< const Track*, const bool > TrackFittedPair;
      typedef std::multimap< TrackScore, TrackFittedPair > TrackScoreMap;
    
      typedef std::set<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;

      // default methods
      DenseEnvironmentsAmbiguityProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~DenseEnvironmentsAmbiguityProcessorTool ();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      /**Returns a processed TrackCollection from the passed 'tracks'
     @param tracks collection of tracks which will have ambiguities resolved. Will not be 
     modified.
     The tracks will be refitted if no fitQuality is given at input.
     @return new collections of tracks, with ambiguities resolved. Ownership is passed on 
     (i.e. client handles deletion)*/
      virtual TrackCollection*  process(const TrackCollection* tracks);
    
      /** statistics output */
      virtual void statistics();


    private:
      
      void reset();
    
      /**Add passed TrackCollection, and Trk::PrepRawData from tracks to caches
         @param tracks the TrackCollection is looped over, 
         and each Trk::Track is added to the various caches. 
         The Trk::PrepRawData from each Trk::Track are added to the IPRD_AssociationTool*/
      void addNewTracks(const TrackCollection* tracks);

      void addTrack(const Track* track, const bool fitted);

      void solveTracks();
    
      /** add subtrack to map */
      void addSubTrack( const std::vector<const TrackStateOnSurface*>& tsos);

      /** refit track */
      void refitTrack( const Trk::Track* track);

      /** refit PRDs */
      const Track* refitPrds( const Track* track);

      /** refit ROTs corresponding to PRDs */
      const Track* refitRots( const Track* track);

      /** print out tracks and their scores for debugging*/
      void dumpTracks(const TrackCollection& tracks);

      /** stores the minimal dist(trk,trk) for covariance correction*/
      void storeTrkDistanceMapdR(const TrackCollection& tracks,
                                 std::vector<const Trk::Track*> &refit_tracks_out );
      
      /**  Find SiS Tracks that share hits in the track score map*/
      void overlapppingTracks();
     
      /**  Update pixel split information based using the fitted track*/    
      void updatePixelSplitInformation(std::map< const InDet::PixelCluster*, const Trk::TrackParameters* >& setOfClustersOnTrack);
      void updatePixelSplitInformationForCluster(const std::pair<const InDet::PixelCluster* const,
                                                                 const Trk::TrackParameters*> & clusterTrkPara );
      
 
      void updateSCT_SplitInformation(std::map< const InDet::SCT_Cluster*, const Trk::TrackParameters* >& setOfClustersOnTrack);

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

      /** by default drop double tracks before refit*/
      bool m_dropDouble;

      /** by default tracks at input get refitted */
      bool m_forceRefit;

      /** by default refit tracks from PRD */
      bool m_refitPrds;
      
      /** rescale pixel PRD covariances */
      bool m_applydRcorrection;

      /** suppress Hole Search */ 
      bool m_suppressHoleSearch;

      /** suppress Track Fit */ 
      bool m_suppressTrackFit;

      /** control material effects (0=non-interacting, 1=pion, 2=electron, 3=muon, 4=pion) read in as an integer 
      read in as an integer and convert to particle hypothesis */
      int m_matEffects;
      Trk::ParticleHypothesis m_particleHypothesis;
   
      
      /**Scoring tool
         This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
         @todo The actual tool that is used should be configured through job options*/
      ToolHandle<ITrackScoringTool> m_scoringTool;

	  /**Observer tool
         This tool is used to observe the tracks and their 'score' */
      ToolHandle<ITrkObserverTool> m_observerTool;
      
      /** refitting tool - used to refit tracks once shared hits are removed. 
          Refitting tool used is configured via jobOptions.*/
      ToolHandleArray<ITrackFitter> m_fitterTool;
      /** extrapolator tool - used to refit tracks once shared hits are removed. 
          Extrapolator tool used is configured via jobOptions.*/
      ToolHandle<IExtrapolator> m_extrapolatorTool;

      /** selection tool - here the decision which hits remain on a track and
          which are removed are made */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

      /** recalculate split prob tool **/
      ToolHandle<InDet::IPixelClusterSplitProbTool>       m_splitProbTool; 

      /**Association tool - used to work out which (if any) PRDs are shared between 
       tracks */ 
      ToolHandle<Trk::IPRD_AssociationTool> m_assoTool;
      
      /** unsorted container of track and track scores.*/
      TrackScoreMap m_trackScoreTrackMap;
    
      /** signature map to drop double track. */
      PrdSignatureSet m_prdSigSet;

      /**Tracks that will be passed out of AmbiProcessor. 
         Recreated anew each time process() is called*/ 
      TrackCollection* m_finalTracks;
      
      /**NN split sprob cut for 2 particle clusters */      
      float m_sharedProbCut;

      /**NN split sprob cut for 3 particle clusters */      
      float m_sharedProbCut2;
      
      

      /** monitoring statistics */
      int m_Nevents;
      std::vector<int> m_Ncandidates, m_NcandScoreZero, m_NcandDouble,
      m_NscoreOk,m_NscoreZeroBremRefit,m_NscoreZeroBremRefitFailed,m_NscoreZeroBremRefitScoreZero,m_NscoreZero,
      m_Naccepted,m_NsubTrack,m_NnoSubTrack,m_NacceptedBrem,
      m_NbremFits,m_Nfits,m_NrecoveryBremFits,m_NgoodFits,m_NfailedFits;
      /** internal monitoring: categories for counting different types of extension results*/
      enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3};
      std::vector<float>  m_etabounds;           //!< eta intervals for internal monitoring

      /** helper for monitoring and validation: does success/failure counting */
      void increment_by_eta(std::vector<int>&,const Track*,bool=true);

      bool m_monitorTracks; // to track observeration/monitoring (default is false)
      
      std::string                                         m_splitClusterMapName; //!< split cluster ambiguity map
      SG::WriteHandleKey<InDet::DRMap>                    m_dRMap;      //!< the actual dR map         
//      std::string                                         m_dRMapName;  //!< dR map


//==================================================================================================
//
//   FROM HERE EVERYTHING IS DEBUGGING CODE !!!
//
//==================================================================================================

#ifdef SIMPLEAMBIGPROCDEBUGCODE
    SG::ReadHandleKey<PRD_MultiTruthCollection> m_truth_locationPixel;
    SG::ReadHandle<PRD_MultiTruthCollection> m_truthPIX;

    SG::ReadHandleKey<PRD_MultiTruthCollection> m_truth_locationSCT;
    SG::ReadHandle<PRD_MultiTruthCollection> m_truthSCT;

    SG::ReadHandleKey<PRD_MultiTruthCollection>  m_truth_locationTRT;
    SG::ReadHandle<PRD_MultiTruthCollection>  m_truthTRT;

//==================================================================================================
// PART 2 : Output statistics
//==================================================================================================

    private:

      std::set<const Trk::Track*> m_trueTracks;
      std::map<const Trk::Track*, const Trk::Track*> m_trackHistory;

      void findTrueTracks(const TrackCollection* recTracks);
      void keepTrackOfTracks(const Trk::Track* oldTrack, const Trk::Track* newTrack);
      void produceInputOutputConnection();

      SG::ReadHandleKey<McEventCollection> m_generatedEventCollectionName;
      SG::ReadHandleKey<TrackTruthCollection> m_truthCollection;
      SG::ReadHandleKey<TrackCollectionConnection> m_resolvedTrackConnection;
      bool m_has_resolvedTrackConnection;
      SG::WriteHandleKey<TrackCollectionConnection> m_write_resolvedTrackConnection;

      int n_trueFitFails;
      int n_fitFails;
      int numOutliersDiff;
      int numOutliersBefore;
      int numOutliersAfter;
      int numFirstFitLost;
      int numSecondFitLost;
      int numSharedTruth;
      int truthBefore;
      int truthAfter;

      bool isSharedTrack( const Track* track);
      bool isTrueTrack( const Track* track);

      const PixelID* m_pixelId;

      void addTrackToMap(Trk::Track* Tr);
      void findSharedTrueTracks(const TrackCollection* recTracks);    
      void prdTruth(const Track* track);
      void tsosTruth(const Track* track);
      const Track* origTrack( const Track* track);

      std::map<const Trk::PrepRawData*, const Trk::Track*> m_tracksShared;

      StatusCode getBremTruth();
      double originalMomentum( const HepMC::GenEvent* genEvent );
      double momentumLostByBrem( const HepMC::GenEvent* genEvent ) const;
      const std::vector<double> fractionOfIncidentMomentumLostPerVertex( const HepMC::GenEvent* genEvent ) const;
      const std::vector<Amg::Vector3D> positionsOfBremVertices( const HepMC::GenEvent* genEvent ) const;
      bool vertexAssociatedWithOriginalTrack( HepMC::GenVertex* genVertex) const;

      Trk::ITruthToTrack*                m_truthToTrack         ;

#endif // DebugCode
      bool m_rejectInvalidTracks;
 
  };
      inline
      Trk::Track *DenseEnvironmentsAmbiguityProcessorTool::fit(std::vector<const Trk::PrepRawData*> &raw,
                                                           const TrackParameters &param, bool flag, Trk::ParticleHypothesis hypo) const {
         Trk::Track *new_track=nullptr;
         for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
              delete new_track;
              new_track=nullptr;
              new_track =  a_fitter->fit(raw, param, flag,hypo);
              if (Trk::DenseEnvironmentsAmbiguityProcessorTool::_checkTrack(new_track)) {
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
      Trk::Track *DenseEnvironmentsAmbiguityProcessorTool::fit(const Track &track, Args... args) const
      {
         Trk::Track *new_track=nullptr;
         for ( const ToolHandle<ITrackFitter> &a_fitter : m_fitterTool) {
            delete new_track;
            new_track=nullptr;
            new_track =  a_fitter->fit(track,args...);
            if (Trk::DenseEnvironmentsAmbiguityProcessorTool::_checkTrack(new_track)) {
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


#endif // TrackAmbiguityProcessorTool_H

