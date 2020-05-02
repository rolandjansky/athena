/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
#ifndef SIMPLEAMBIGUITYPROCESSORTOOLDEBUG_H
#define SIMPLEAMBIGUITYPROCESSORTOOLDEBUG_H


// turn on debugging ? uncomment this
//#define SIMPLEAMBIGPROCDEBUGCODE

#include "SimpleAmbiguityProcessorTool.h"

#ifdef SIMPLEAMBIGPROCDEBUGCODE
// --------------- DEBUG CODE
#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Geometry/Point3D.h"
typedef HepGeom::Point3D<double> HepPoint3D;
#include "TrkTruthData/TrackTruthCollection.h"
#include "xAODEventInfo/EventInfo.h"
#include "AtlasHepMC/GenEvent.h"
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


namespace Trk {

  class SimpleAmbiguityProcessorToolDebug : public SimpleAmbiguityProcessorTool
  {
    public:

       SimpleAmbiguityProcessorToolDeug(const std::string&,const std::string&,const IInterface*);

       virtual StatusCode initialize() override;

    protected:
       // @TODO improve method naming:
       virtual void ntupleReset() override;
       virtual void fillEventData(std::vector<const Track*> &tracks) override;
       virtual void resetTrackOutliers() ovverride;
       virtual void setBarcodeStats(const Trk::Track *a_track, TrackScore score) override;
       virtual void fillBadTrack(const Trk::Track *a_track, const Trk::PRDtoTrackMap &prd_to_track_map) override;
       virtual void fillDuplicateTrack(const Trk::Track *a_track) override;
       virtual void associateToOrig(const Trk::Track*new_track, const Trk::Track* orig_track) override;
       virtual void keepTrackOfTracks(const Trk::Track* oldTrack, const Trk::Track* newTrack) override;
       virtual void countTrueTracksInMap(const TrackScoreMap &trackScoreTrackMap) override;
       virtual void rejectedTrack(const Trk::Track*track, const Trk::PRDtoTrackMap &prd_to_track_map) override;
       virtual void fitStatReset() override;
       virtual void keepFittedInputTrack(const Trk::Track *a_track, TrackScore ascore) override;
       virtual void acceptedTrack(const Trk::Track*track) override;
       virtual void memoriseOutliers(const Trk::Track*track) override;
       virtual void newCleanedTrack(const Trk::Track*new_track, const Trk::Track* orig_track) override;
       virtual void eventSummary(const TrackCollection *final_tracks) override;
       virtual void fillFailedFit(const Trk::Track *a_track, const Trk::PRDtoTrackMap &prd_to_track_map) override;

#ifdef SIMPLEAMBIGPROCDEBUGCODE
       virtual void findTrueTracks(const TrackCollection* recTracks) override;
#endif

#ifdef SIMPLEAMBIGPROCNTUPLECODE
      // private method to fill the validation tree 
       virtual void fillValidationTree(const Trk::Track* track) const override; // heather was const
#endif

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
      mutable  std::map<const Trk::Track* , const Trk::Track*> *m_trackTrackMap;
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

      void produceInputOutputConnection(const TrackCollection &final_tracks);
 
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
      void prdTruth(const Track* track, Trk::PRDtoTrackMap &prd_to_track_map);
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
#endif // TrackAmbiguityProcessorToolDebug_H
#endif

