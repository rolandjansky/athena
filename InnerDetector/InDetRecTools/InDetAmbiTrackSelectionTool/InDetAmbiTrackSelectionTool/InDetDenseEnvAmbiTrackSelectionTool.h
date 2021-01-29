// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetDenseEnvAmbiTrackSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETInDetDenseEnvAmbiTrackSelectionTool_H
#define INDETInDetDenseEnvAmbiTrackSelectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "PixelGeoModel/IBLParameterSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h" //for use in the struct lessTrkTrack implementation in this header
#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include "GaudiKernel/ToolHandle.h"

#include <cmath> //for std::fabs in implementation of structs in this header
#include <iostream> //for cout in dumpInfo 
#include <map>
#include <mutex>
#include <vector>

class Identifier;
class SiliconID;
namespace Trk {
  class TrackStateOnSurface;
  class PrepRawData;
  class RIO_OnTrack;
}

namespace InDet 
{

  //In here we refer to ROIs as region of interests corresponding to high pt B tracks.
  /** @class InDetDenseEnvAmbiTrackSelectionTool 
      This tool cross checks the hits on a track with the hits already stored in 
      a PRD_AssociationTool. Shared hits are allowed to some extent und under certain conditions
      (e.g. the track score is high enough), additional shared hits are removed if necessary.
      This tool does not delete any tracks. If the client is going to discard tracks entered into
      this tool, the tracks have to be deleted by this client.
      
  */  
  class InDetDenseEnvAmbiTrackSelectionTool : public extends<AthAlgTool, Trk::IAmbiTrackSelectionTool>
  {
  public:
    InDetDenseEnvAmbiTrackSelectionTool(const std::string&,const std::string&,const IInterface*);
      
    /** default destructor */
    virtual ~InDetDenseEnvAmbiTrackSelectionTool() = default;
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    /** 
     * Decide what to do with a candidate track
     *  - count and characterize hits on track
     *  - not those which are used too often (decideWhichHitsToKeep)
     *  - accept the track as is, remove hits and make a subtrack, or
     *    reject track
     *  - track candidate are also checked against accepted tracks to
     *    ensure accepted tracks are not pushed over threshold
     * */
    virtual std::tuple<Trk::Track*,bool> getCleanedOutTrack(const Trk::Track *track,
                                                            const Trk::TrackScore score,
                                                            Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                            Trk::PRDtoTrackMap &prd_to_track_map) const override;

  private:
    
    // possible classification of TSOS
    enum TsosTypes {
      // A measurement not yet used in any other track
      UnusedHit   = 1,
      // A measurement shared with another track that can be split
      SplitSharedHit = 2, 
      // A measurement shared with another track
      SharedHit   = 3, 
      // A hit that needs to be removed from the track 
      // because it is used on too many tracks already
      RejectedHitOverUse = 4,
      // A hit that needs to be removed from the track
      // because sharing it would invalidate an accepted track
      RejectedHitInvalid = 5,
      // A hit that needs to be removed from the track
      // reason other than the above two
      RejectedHit = 6,
      // an outlier, to be copied in case
      Outlier     = 7,
      // other TSOS types to be copied in case
      OtherTsos   = 8
    };
    
     
    struct TrackHitDetails
    {  
      bool  m_isPatternTrack        ;
      bool  m_thisHasIBLHit         ; //Track has a hit in the IBL
      bool  m_hasSharedIBLHit       ; //Track has a shared hit in the IBL
      bool  m_hasSharedPixel        ; //Track has a shared pixel hit (does not include IBL)
      bool  m_firstPixIsShared      ; //The first pixel hit on track is shared (includes IBL)
      bool  m_passHadronicROI       ; //Track is within hadronic ROI
      bool  m_passConversionSel     ; //Track is compatible with a conversion
      bool  m_trkCouldBeAccepted    ; //No rejected hits on track
   
      int   m_numPixelDeadSensor    ; //Taken from track summary. Stored value not changed in this tool
      int   m_numSCTDeadSensor      ; //Taken from track summary. Stored value not changed in this tool
      int   m_numPixelHits          ; //Taken from track summary. Stored value not changed in this tool  
      int   m_numPixelHoles         ; //Taken from track summary. Stored value not changed in this tool 
      int   m_numSCTHoles           ; //Taken from track summary. Stored value not changed in this tool 
      int   m_numSCTHits            ; //Taken from track summary. Stored value not changed in this tool 
        
      // values counted in this tool
      int   m_numUnused             ; //Number of unique hits
      int   m_numTRT_Unused         ; //Number of unique TRT hits (NOT included within m_numUnused)
      int   m_numSCT_Unused         ; //Number of unique SCT hits (    included within m_numUnused)
      int   m_numPseudo             ; //Number of pseudo measurements on track
  
      int   m_numSplitSharedPix     ; //Number of Pixel clusters comptaible with being split that are also shared
      int   m_numSplitSharedSCT     ; //Number of SCT clusters comptaible with being split that are also shared
      int   m_numShared             ; //Number of shared hits on track (does not include SplitShared)
      int   m_numSCT_Shared         ; //Number of shared sct hits on track (included within m_numShared)
      int   m_numWeightedShared     ; //Weighted number of shared hits on track used to test shared module cuts (pixels count for 2)
      
      TrackHitDetails()
      {          
        m_isPatternTrack    = true;
        m_thisHasIBLHit     = false;   
        m_hasSharedIBLHit   = false;
        m_hasSharedPixel    = false;
        m_firstPixIsShared  = false;
        m_passHadronicROI   = false;
        m_passConversionSel = false;
        m_trkCouldBeAccepted = true;
          
        m_numPixelDeadSensor = -1;
        m_numSCTDeadSensor   = -1;
        m_numPixelHits       = 0;
        m_numPixelHoles      = 0;
        m_numSCTHoles        = 0;
        m_numSCTHits         = 0;
 
        m_numUnused         = 0;
        m_numSCT_Unused     = 0;
        m_numTRT_Unused     = 0;
        m_numPseudo         = 0;
          
        m_numSplitSharedPix = 0;
        m_numSplitSharedSCT = 0;
        m_numShared = 0;
        m_numSCT_Shared = 0; 
        m_numWeightedShared = 0;
      }
       
      // All SiHits including shared
      int totalSiHits() const
      { 
        return (m_numUnused + m_numPixelDeadSensor + m_numSCTDeadSensor + m_numSplitSharedPix + m_numSplitSharedSCT + m_numShared);
      };

      // SplitShared are not counted as shared (no penality) 
      // since are compatible with originating from multiple particles
      int totalUniqueSiHits() const
      { 
        return (totalSiHits() - m_numShared);
      };

      // No counters, for pixel hits. Solve for them to avoid too many counters
      int totalUniquePixelHits() const
      { 
        // m_numTRT_Unused not included in m_numUnused
        return (m_numUnused - m_numSCT_Unused);
      };

      // No counters, for pixel hits. Solve for them to avoid too many counters
      int totalSharedPixelHits() const
      { 
        return (m_numShared - m_numSCT_Shared);
      };

      int totalPixelHits() const
      { 
        return (totalUniquePixelHits() + totalSharedPixelHits() + m_numSplitSharedPix + m_numPixelDeadSensor);
      };
        
      void dumpInfo() const
      {
        std::cout << "isPatternTrack    : " << m_isPatternTrack    << std::endl;
        std::cout << "thisHasIBLHit     : " << m_thisHasIBLHit     << std::endl;   
        std::cout << "hasSharedIBLHit   : " << m_hasSharedIBLHit   << std::endl;
        std::cout << "hasSharedPixel    : " << m_hasSharedPixel    << std::endl;
        std::cout << "firstPixIsShared  : " << m_firstPixIsShared  << std::endl;
        std::cout << "passHadronicROI   : " << m_passHadronicROI   << std::endl;
        std::cout << "passConversionSel : " << m_passConversionSel << std::endl; 
        std::cout << "trkCouldBeAccepted: " << m_trkCouldBeAccepted<< std::endl; 
          
        std::cout << "Dead Pixels:    " << m_numPixelDeadSensor<< std::endl;
        std::cout << "Dead SCT:       " << m_numSCTDeadSensor  << std::endl;
        std::cout << "Pixel hit       " << m_numPixelHits      << std::endl;
        std::cout << "Pixel holes     " << m_numPixelHoles     << std::endl;
        std::cout << "SCT hits        " << m_numSCTHits        << std::endl;
        std::cout << "SCT holes       " << m_numSCTHoles       << std::endl;
 
        std::cout << "Unique hits     " << m_numUnused         << std::endl;
        std::cout << "Unique SCT      " << m_numSCT_Unused     << std::endl;
        std::cout << "Unique TRT      " << m_numTRT_Unused     << std::endl;
        std::cout << "Unique Pix      " << totalUniquePixelHits() << std::endl;
        std::cout << "Pseudo          " << m_numPseudo         << std::endl;
          
        std::cout << "SplitSharedPix  " << m_numSplitSharedPix << std::endl;
        std::cout << "SplitSharedSCT  " << m_numSplitSharedSCT << std::endl;
        std::cout << "Shared          " << m_numShared         << std::endl;
        std::cout << "Shared SCT      " << m_numSCT_Shared     << std::endl;
        std::cout << "Shared Pix      " << totalSharedPixelHits() << std::endl;
        std::cout << "Weighted shared " << m_numWeightedShared << std::endl;
      };
    };

    struct lessTrkTrack {
      bool operator() (const Trk::Track* x, const Trk::Track* y) const
      {
        if (ATH_UNLIKELY(!x and !y)) return false;
        if (ATH_UNLIKELY(!x)) return true;
        if (ATH_UNLIKELY(!y)) return false;
        if (ATH_UNLIKELY(!x->trackParameters()  and !y->trackParameters())) return false;
        if (ATH_UNLIKELY(!x->trackParameters() && x->trackParameters()->size() <= 0) ) return true;
        if (ATH_UNLIKELY(!y->trackParameters() && y->trackParameters()->size() <= 0) ) return false;
        return std::fabs( (*x->trackParameters())[0]->parameters()[Trk::qOverP]) < std::fabs( (*y->trackParameters())[0]->parameters()[Trk::qOverP]) ;
      }
    };
 
    struct TSoS_Details
    {
      unsigned int        m_nTSoS;
      std::vector<int>    m_type;                                   // The type of TSOS 
      std::vector<int>    m_detType;                                // The Detector type 1== Pixel, 11 = b-layer, 2 SCT, 3 TRT
      std::vector<int>    m_hitIsShared;                            // Number of tracks the hit is shared with
      std::vector<float>  m_splitProb1;                             // The Probablilty that the cluster on that surface is from 2 tracks
      std::vector<float>  m_splitProb2;                             // The Probablilty that the cluster on that surface is from 3 or more tracks
      std::vector<const Trk::RIO_OnTrack*> m_RIO;                   // The cluster on track
      std::multimap<const Trk::Track*, int, lessTrkTrack >  m_overlappingTracks;   // The tracks that overlap with the current track
      std::multimap< int, const Trk::Track*> m_tracksSharingHit;    // The tracks that overlap with the current track

      TSoS_Details()
      {
        std::cout << "WARNING DON'T USE THE DEFAULT CONSTRUCTOR OF tsosDetails" << std::endl;
      };
        
      TSoS_Details(unsigned int temp_nTSoS)
      { 
        m_nTSoS = temp_nTSoS;
        m_type.resize(m_nTSoS,OtherTsos);    
        m_detType.resize(m_nTSoS,-1.);
        m_hitIsShared.resize(m_nTSoS, 0 );
        m_splitProb1.resize(m_nTSoS,-1.) ;
        m_splitProb2.resize(m_nTSoS,-1.) ;
        m_RIO.resize(m_nTSoS,0);
      };
        
      int findIndexOfPreviousMeasurement( int currentIndex ) const
      {
        int indexPreviousMeasurement = currentIndex-1;
        while(indexPreviousMeasurement >= 0){
          if ( m_type[indexPreviousMeasurement] != OtherTsos ){
            break;
          } else {
            --indexPreviousMeasurement;
          }
        } // end while
        return indexPreviousMeasurement;
      };
    };   
     
    mutable std::mutex m_mutex;
    struct CacheEntry {
      EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};

      // Max shared modules -- calulated from  m_maxSharedModules
      //  1 pixel hit is 1 module
      //  a double sided SCT hit (2 SCT hits) is 1 module
      //  so count by 2s for shared pixel hits and 1 per SCT (single sided hit) hit
      int m_maxSharedModules; 
      // Min number of unique hits that are not already used on any other track
      // but split hits can be used on multiple tracks and be considered unique
      //  - can change in ROI
      int m_minNotShared;
      // Min number of hits before we allow split sharing of hits -- can change if we are in ROI
      int m_minSiHits;    

      std::vector<double> m_hadF;
      std::vector<double> m_hadE;
      std::vector<double> m_hadR;
      std::vector<double> m_hadZ;

      std::vector<double> m_emF;
      std::vector<double> m_emE;
      std::vector<double> m_emR;
      std::vector<double> m_emZ;
    };
    mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
      
    /** method to create a new track from a vector of TSOS's */
    Trk::Track* createSubTrack( const std::vector<const Trk::TrackStateOnSurface*>& tsos, const Trk::Track* track ) const ;
      
    /** Fill the two structs TrackHitDetails & TSoS_Details full of information*/
    void fillTrackDetails(const Trk::Track* ptrTrack,
                          Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                          const Trk::PRDtoTrackMap &prd_to_track_map,
                          TrackHitDetails& trackHitDetails,
                          TSoS_Details& tsosDetails ) const;

    /** Determine which hits to keep on this track
     * Look at the hits on track and decided if they should be kept on the track or rejected.
     * The decision focuses on the track in question and how often clusters are used 
     * Within getCleanedOutTrack (from which this is called), accepted tracks are checked 
     * to see if they are pushed over limits
     * */
    void decideWhichHitsToKeep(const Trk::Track*,
                               const Trk::TrackScore score,
                               Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                               Trk::PRDtoTrackMap &prd_to_track_map,
                               TrackHitDetails& trackHitDetails,
                               TSoS_Details& tsosDetails,
                               CacheEntry* ent) const;

    /** Specific logic for identifing conversions with the goal 
     * of passing those tracks through to the final collection 
     * with as little loss as possible
     * */
    bool performConversionCheck(const Trk::Track* ptrTrack,
        Trk::PRDtoTrackMap &prd_to_track_map,
        TrackHitDetails& trackHitDetails,
        TSoS_Details& tsosDetails,
        CacheEntry* ent) const;

    /** Specific logic for identifing boosted light particle
     * decays in jet topologies (tau and b), with the goal 
     * of passing those tracks through to the final collection 
     * with as little loss as possible
     * */
    bool performHadDecayCheck(const Trk::Track* ptrTrack,
        Trk::PRDtoTrackMap &prd_to_track_map,
        TrackHitDetails& trackHitDetails,
        TSoS_Details& tsosDetails,
        CacheEntry* ent) const;

    /** Handle update of the shared hit counts if 
     * either a conversion or a dense hadronic decay
     * was identified using one of the methods above.
     **/ 
    void updateSharedForCollimated(TrackHitDetails& trackHitDetails,
          TSoS_Details& tsosDetails) const; 


    /** Update the pixel clusters split information*/
    void setPixelClusterSplitInformation(TSoS_Details& tsosDetails,
                                         Trk::ClusterSplitProbabilityContainer &clusterSplitProbMap) const;
      
    /** Does track pass criteria for hadronic ROI? */
    bool inHadronicROI(const Trk::Track* ptrTrack, CacheEntry* ent) const;

    /** Check if the cluster is compatible with a hadronic cluster*/
    bool isHadCaloCompatible(const Trk::TrackParameters& Tp, CacheEntry* ent) const;

    /** Check if the cluster is compatible with a EM cluster*/
    bool isEmCaloCompatible(const Trk::TrackParameters& Tp, CacheEntry* ent) const;
      
    /** Fill hadronic & EM cluster map*/
    void newEvent(CacheEntry* ent) const;
      
    /** Returns true if accepted tracks remain about thresholds, false otherwise
        maxiShared  = max number of shared modules on an accepted shared track
        which has maxOtherNPixel pixel hits and blayer hit if maxOtherHasIBL is true
    */
    bool checkOtherTracksValidity(TSoS_Details& tsosDetails,
                                 int index,
                                 Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                 Trk::PRDtoTrackMap &prd_to_track_map,
                                 int& maxiShared,
                                 int& maxOtherNPixel,
                                 bool& maxOtherHasIBL,
                                 CacheEntry* ent) const;


    /** Returns the Trackparameters of the two tracks on the n'th TrackStateOnSurface of the first track*/
    std::pair< const Trk::TrackParameters* , const Trk::TrackParameters* > 
    getOverlapTrackParameters(int n, const Trk::Track* track1,
                              const Trk::Track* track2,
                              const Trk::PRDtoTrackMap &prd_to_track_map,
                              int splitSharedPix ) const;

    /** Check if two sets of track paremeters are compatible with being from a the same low mass particle decay. 
        It is assumed that the track parmeters are on the same surface.*/
    bool isNearbyTrackCandidate(const Trk::TrackParameters* paraA, const Trk::TrackParameters* paraB) const;

    /** Simple helper functions to tell is cluster is split*/
    bool clusCanBeSplit(float splitProb1, float splitProb2) const;
    bool isTwoPartClus(float splitProb1, float splitProb2) const;
    bool isMultiPartClus(float splitProb2) const;

    void rejectHitOverUse(TrackHitDetails& trackHitDetails, TSoS_Details& tsosDetails, int index) const;
    void rejectHit       (TrackHitDetails& trackHitDetails, TSoS_Details& tsosDetails, int index) const;
    void rejectSharedHit (TrackHitDetails& trackHitDetails, TSoS_Details& tsosDetails, int index) const;
    void rejectSharedHitInvalid (TrackHitDetails& trackHitDetails, TSoS_Details& tsosDetails, int index) const;
    void sharedToSplitPix(TrackHitDetails& trackHitDetails, TSoS_Details& tsosDetails, int index) const;
    void addSharedHit    (TrackHitDetails& trackHitDetails, TSoS_Details& tsosDetails, int index) const;
    void increaseSharedHitCounters(TrackHitDetails& trackHitDetails, bool isPix, bool isSCT) const;
    void decreaseSharedHitCounters(TrackHitDetails& trackHitDetails, bool isPix, bool isSCT) const;

    /** TRT minimum number of drift circles tool- returns allowed minimum number of TRT drift circles */
    PublicToolHandle<ITrtDriftCircleCutTool>  m_selectortool{this, "DriftCircleCutTool", "InDet::InDetTrtDriftCircleCutTool"};
    ServiceHandle<IBLParameterSvc> m_IBLParameterSvc{this, "IBLParameterSvc", "IBLParameterSvc"};
      
    /**atlas id helper*/
    const SiliconID* m_detID{nullptr};
      
    ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
         {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };

    /** some cut values */
    IntegerProperty m_minHits{this, "minHits", 5, "Min Number of hits on track"};
    IntegerProperty m_minTRT_Hits{this, "minTRTHits", 0, "Min Number of TRT hits on track"};
    IntegerProperty m_maxSharedModules{this, "maxShared", 1, "Max number of shared modules"};
    IntegerProperty m_maxSharedModulesInROI{this, "maxSharedModulesInROI", 2, "Max number of shared modules in a hadronic ROI. Test value for recovering B jet efficiency at high pt"};
    IntegerProperty m_maxTracksPerPRD{this, "maxTracksPerSharedPRD", 2, "Max number of tracks per hit. When NN is used, other flags set the limits."};
    IntegerProperty m_minNotSharedHits{this, "minNotShared", 6, "Min number of non shared hits"};
    IntegerProperty m_minNotSharedHitsInROI{this, "minNotSharedInROI", 4, "Min number of non shared hits in ROI. Test value for recovering B jet efficiency at high pt"};
    FloatProperty m_minScoreShareTracks{this, "minScoreShareTracks", 0.0, "Min track score to alow it to share hits"};
    BooleanProperty m_cosmics{this, "Cosmics", false, "Trying to reco cosmics?"};
    BooleanProperty m_parameterization{this, "UseParameterization", true, "Use table of min number DCs"};
    BooleanProperty m_doPixelClusterSplitting{this, "doPixelSplitting", false, "Split pixel clusters"};
    FloatProperty m_sharedProbCut{this, "sharedProbCut", 0.3, "Min split prob to break a cluster into two parts"};
    FloatProperty m_sharedProbCut2{this, "sharedProbCut2", 0.3, "Min split prob to break a clsuter into three parts"};
    FloatProperty m_minsharedProbCut{this, "minSharedProbCut", 0.05, "Min split prob cut to all a cluster to be shared"};
      
    FloatProperty m_minTrackChi2ForSharedHits{this, "minTrackChi2ForSharedHits", 3, "Min track chi2 to split share hits"};
    IntegerProperty m_minUniqueSCTHits{this, "minUniqueSCTHits", 2, "Min number of hits in the SCT that we need before we allow hit sharing in the SCT"};
    IntegerProperty m_minSiHitsToAllowSplitting{this, "minSiHitsToAllowSplitting", 9, "Min number of hits before we allow split sharing of hits"};
    IntegerProperty m_minSiHitsToAllowSplittingInROI{this, "minSiHitsToAllowSplittingInROI", 7, "Min number of hits before we allow split sharing of hits In ROI. Test value for recovering B jet efficiency"}; 
    IntegerProperty m_maxPixOnePartCluster{this, "maxPixOnePartCluster", 2, "Max number of tracks that can be associated to a 1 particle cluster"};
    IntegerProperty m_maxPixTwoPartCluster{this, "maxPixTwoPartCluster", 2, "Max number of tracks that can be associated to a 2 particle cluster"};
    IntegerProperty m_maxPixMultiCluster{this, "maxPixMultiCluster", 4, "Max number of tracks that can be associated to a >= 3 particle cluster"};
    BooleanProperty m_shareSplitHits{this, "shareSplitHits", false, "Allow shared hits to be shared on 1 more track"};
    IntegerProperty m_minPixHitAccepted{this, "minPixHitAccepted", 2, "Min number of pixel hits needed to be allowed to push accepted tracks over shared module limits"};

    // ROI stuff
    BooleanProperty m_useHClusSeed{this, "doHadCaloSeed", false};
    FloatProperty m_minPtSplit{this, "minPtSplit", 0.};
    FloatProperty m_minPtBjetROI{this, "minPtBjetROI", 15000., "in MeV"};
    FloatProperty m_phiWidth{this, "phiWidth", 0.2};
    FloatProperty m_etaWidth{this, "etaWidth", 0.2};
    SG::ReadHandleKey<CaloClusterROI_Collection> m_inputHadClusterContainerName{this, "InputHadClusterContainerName", "InDetHadCaloClusterROIs"};
      
    BooleanProperty m_useEmClusSeed{this, "doEmCaloSeed", false};
    FloatProperty m_minPtEm{this, "minPtConv", 10000., "in MeV"};
    FloatProperty m_phiWidthEm{this, "phiWidthEM", 0.05};
    FloatProperty m_etaWidthEm{this, "etaWidthEM", 0.05};

    SG::ReadHandleKey<CaloClusterROI_Collection> m_inputEmClusterContainerName{this, "InputEmClusterContainerName", "InDetCaloClusterROIs"};

    //Track Pair Selection
    BooleanProperty m_doPairSelection{this, "doPairSelection", true};
    FloatProperty m_minPairTrackPt{this, "minPairTrackPt", 1000., "In MeV"};

    BooleanProperty m_monitorTracks{this, "MonitorAmbiguitySolving", false, "to track observeration/monitoring (default is false)"};

  }; 
} // end of namespace


#endif 
