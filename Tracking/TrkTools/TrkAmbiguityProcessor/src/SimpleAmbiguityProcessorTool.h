/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLEAMBIGUITYPROCESSORTOOL_H
#define SIMPLEAMBIGUITYPROCESSORTOOL_H

// turn on debugging ? uncomment this
//#define SIMPLEAMBIGPROCDEBUGCODE

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"

#include <map>
#include <set>
#include <vector>
#include <functional>

#ifdef SIMPLEAMBIGPROCDEBUGCODE
// --------------- DEBUG CODE
#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Geometry/Point3D.h"
typedef HepGeom::Point3D<double> HepPoint3D;
#include "TrkTruthData/TrackTruthCollection.h"
#include "xAODEventInfo/EventInfo.h"
#include "HepMC/GenEvent.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
     typedef std::map<const Trk::Track*, const Trk::Track*> TrackCollectionConnection;
#ifndef SIMPLEAMBIGPROCDEBUGCODE_CLASS_DEF
#define SIMPLEAMBIGPROCDEBUGCODE_CLASS_DEF
     CLASS_DEF( TrackCollectionConnection , 148639440 , 1 )
#endif

#endif

#ifdef SIMPLEAMBIGPROCNTUPLECODE
// --------------- DEBUG CODE
#include "xAODEventInfo/EventInfo.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IPartPropSvc.h"
     class TTree;
     
     struct TrackBarcodeStats { 
       int numhits;
       int numbarcodes;
       int numtruthlost;
       int leadingbarcode;
       int leadingnumhits;  
     };

        #ifndef MAXSEEDSPERTRACK
	#define MAXSEEDSPERTRACK 10
	#endif    
#endif

class AtlasDetectorID;
class PixelID;

namespace Trk {

  class ITrackScoringTool;
  class IPRD_AssociationTool;
  class ITruthToTrack;

  class SimpleAmbiguityProcessorTool : public AthAlgTool, virtual public ITrackAmbiguityProcessorTool 
    {
    public:

      // public types
      typedef std::pair< const Track*, const bool > TrackFittedPair;
      typedef std::multimap< TrackScore, TrackFittedPair > TrackScoreMap;
    
      typedef std::set<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;

      // default methods
      SimpleAmbiguityProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~SimpleAmbiguityProcessorTool ();
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


      /** refitting tool - used to refit tracks once shared hits are removed. 
	  Refitting tool used is configured via jobOptions.*/
      ToolHandle<ITrackFitter> m_fitterTool;
	
      /** selection tool - here the decision which hits remain on a track and
	  which are removed are made
      */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

      /** unsorted container of track and track scores.*/
      TrackScoreMap m_trackScoreTrackMap;
	
      /** signature map to drop double track. */
      PrdSignatureSet m_prdSigSet;

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
      enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3, iDBM = 4};
      std::vector<float>  m_etabounds;           //!< eta intervals for internal monitoring

      /** helper for monitoring and validation: does success/failure counting */
      void increment_by_eta(std::vector<int>&,const Track*,bool=true);

//==================================================================================================
//
//
//   FROM HERE EVERYTHING IS DEBUGGING CODE !!!
//
//
//  PART 1 : NTUPLE writing
//==================================================================================================

#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
      SG::ReadHandleKey<PRD_MultiTruthCollection> m_truth_locationPixel;
      SG::ReadHandle<PRD_MultiTruthCollection> m_truthPIX;
      SG::ReadHandleKey<PRD_MultiTruthCollection> m_truth_locationSCT;
      SG::ReadHandle<PRD_MultiTruthCollection> m_truthSCT;
      SG::ReadHandleKey<PRD_MultiTruthCollection> m_truth_locationTRT;
      SG::ReadHandle<PRD_MultiTruthCollection> m_truthTRT;

      bool m_isBackTracking;
#endif

#ifdef SIMPLEAMBIGPROCNTUPLECODE
      
      /** determine if the ambiguity processor is being used for back tracking */
      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo_key;
    
      std::map<const Trk::Track*, TrackBarcodeStats>   m_trackBarcodeMap;
      std::multimap<int,const Trk::Track*>             m_barcodeTrackMap;

      /** Pointer to the particle properties svc */
      ServiceHandle<IPartPropSvc>      m_particlePropSvc;

      /** ParticleDataTable needed to get connection pdg_code <-> charge */
      const HepPDT::ParticleDataTable* m_particleDataTable;      
      
      // private method to fill the validation tree 
      void fillValidationTree(const Trk::Track* track) const ; // heather was const
      
      // put some validation code is
      std::string                   m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                   m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                   m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out
      
      TTree*                        m_validationTree;            //!< Root Validation Tree
      
      /** Ntuple variables */      
      mutable int                           m_event;
      mutable int                           m_score;
      mutable int                           m_duplicate;
      mutable int                           m_accepted;
      mutable int                           m_perigeeInfo;
      mutable int                           m_track;
      mutable float                         m_eta;
      mutable float                         m_pt;
      mutable float                         m_phi;
      
      // truth related quantities   
      mutable float                         m_pT_mc;
      mutable float                         m_charge_mc;	  
	  
      mutable int                           m_author;
      mutable int                           m_numhits;
      mutable int                           m_numbarcodes;
      mutable int                           m_numtruthlost;
      mutable int                           m_leadingbarcode;
      mutable int                           m_leadingnumhits;
      mutable int                           m_barcodeDuplicates;    
      
      SG::ReadHandle<Trk::TrackSeedMap> m_trackSeedMap;
      SG::ReadHandleKey<Trk::TrackSeedMap> m_trackSeedMapLocation;
      bool m_has_trackSeedMap;
      
      mutable int			    m_nseeds;
      mutable int                           m_seeds[MAXSEEDSPERTRACK];
      mutable  std::map<const Trk::Track* , Trk::Track*> *m_trackTrackMap;
#endif

#ifdef SIMPLEAMBIGPROCDEBUGCODE
//==================================================================================================
// PART 2 : Output statistics
//==================================================================================================

    private:

      SG::ReadHandleKey<McEventCollection> m_generatedEventCollectionName;
      SG::ReadHandleKey<TrackTruthCollection> m_truthCollection;
      SG::ReadHandleKey<TrackCollectionConnection> m_resolvedTrackConnection;
      bool m_has_resolvedTrackConnection;
      SG::WriteHandleKey<TrackCollectionConnection> m_write_resolvedTrackConnection;

      std::set<const Trk::Track*> m_trueTracks;
      std::map<const Trk::Track*, const Trk::Track*> m_trackHistory;

      void findTrueTracks(const TrackCollection* recTracks);
      void keepTrackOfTracks(const Trk::Track* oldTrack, const Trk::Track* newTrack);
      void produceInputOutputConnection();
 
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
      const std::vector<HepPoint3D> positionsOfBremVertices( const HepMC::GenEvent* genEvent ) const;
      bool vertexAssociatedWithOriginalTrack( HepMC::GenVertex* genVertex) const;

      Trk::ITruthToTrack*                m_truthToTrack         ;
#endif // DebugCode
	
    };

} //end ns

#endif // TrackAmbiguityProcessorTool_H

