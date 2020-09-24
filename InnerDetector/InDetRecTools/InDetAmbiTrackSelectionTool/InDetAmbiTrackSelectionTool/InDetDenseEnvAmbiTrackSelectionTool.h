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

#include "CxxUtils/checker_macros.h"

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
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;

    virtual std::tuple<Trk::Track*,bool> getCleanedOutTrack ATLAS_NOT_THREAD_SAFE
      (const Trk::Track*,
       const Trk::TrackScore score,
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
      RejectedHit = 4,
      // an outlier, to be copied in case
      Outlier     = 5,
      // other TSOS types to be copied in case
      OtherTsos   = 6
    };
    
     
    struct TrackHitDetails
    {  
      bool  isPatternTrack        ;
      bool  inROI                 ;
      bool  thishasblayer         ; //Track has a hit in the b-layer
      bool  hassharedblayer       ; //Track has a shared it in the b-layer
      bool  hassharedpixel        ; //Track has a shared pixel hit
      bool  firstisshared         ; //The first hit on track is shared
   
      int   numPixelDeadSensor    ;// I don't think i need to explain these
      int   numSCTDeadSensor      ;
      int   numPixelHits          ;
      int   numPixelHoles         ;
      int   numSCTHoles           ;
      int   numSCTHits            ;
        
      int   numUnused             ; //Number of unique hits
      int   numTRT_Unused         ; //Number of unique TRT hits
      int   numSCT_Unused         ; //Number of unique SCT hits 
      int   numPseudo             ; //Number of pseudo measurements on track
  
      float averageSplit1         ; //Average split prob1
      float averageSplit2         ; //Averege split prob2
  
      int   numSplitSharedPix     ; //Number of Pixel clusters comptaible with being split that are also shared
      int   numSplitSharedSCT     ; //Number of SCT clusters comptaible with being split that are also shared
      int   numSharedOrSplit      ; //Number of split + shared clusters
      int   numSharedOrSplitPixels; //Number of pixel clusters that are either split or shared
      int   numShared             ; //Number of shared hits on track
      int   numWeightedShared     ; //Weighted number of shared hits on trak (pixels count for 2)
                 
      TrackHitDetails()
      {          
        isPatternTrack    = true;
        inROI             = false;
        thishasblayer     = false;   
        hassharedblayer   = false;
        hassharedpixel    = false;
        firstisshared     = true;
          
        numPixelDeadSensor = -1;
        numSCTDeadSensor   = -1;
        numPixelHits       = 0;
        numPixelHoles      = 0;
        numSCTHoles        = 0;
        numSCTHits         = 0;
 
        numUnused         = 0;
        numSCT_Unused     = 0;
        numTRT_Unused     = 0;
        numPseudo         = 0;
          
        averageSplit1 = 0;
        averageSplit2 = 0;
          
        numSplitSharedPix = 0;
        numSplitSharedSCT = 0;
        numSharedOrSplit = 0;
        numSharedOrSplitPixels = 0;
        numShared = 0;
        numWeightedShared = 0;
      }
        
      int totalSiHits()
      { 
        return numUnused + numPixelDeadSensor + numSCTDeadSensor + numSplitSharedPix + numSplitSharedSCT + (inROI ? numShared/2 : 0) ;
      };
        
      void dumpInfo()
      {
        std::cout << "isPatternTrack: " << isPatternTrack    << std::endl;
        std::cout << "thishasblayer : " << thishasblayer     << std::endl;   
        std::cout << "hassharedblayer:" << hassharedblayer   << std::endl;
        std::cout << "hassharedpixel: " << hassharedpixel    << std::endl;
        std::cout << "firstisshared:  " << firstisshared     << std::endl;
          
        std::cout << "Dead Pixels:    " << numPixelDeadSensor<< std::endl;
        std::cout << "Dead SCT:       " << numSCTDeadSensor  << std::endl;
        std::cout << "Pixel hit       " << numPixelHits      << std::endl;
        std::cout << "Pixel holes     " << numPixelHoles     << std::endl;
        std::cout << "SCT hits        " << numSCTHits        << std::endl;
        std::cout << "SCT holes       " << numSCTHoles       << std::endl;
 
        std::cout << "Unique hits     " << numUnused         << std::endl;
        std::cout << "Unique SCT      " << numSCT_Unused     << std::endl;
        std::cout << "Unique TRT      " << numTRT_Unused     << std::endl;
        std::cout << "Pseudo          " << numPseudo         << std::endl;
          
        std::cout << "Average Split1  " << averageSplit1     << std::endl;
        std::cout << "Average Split2  " << averageSplit2     << std::endl;
          
        std::cout << "SplitSharedPix  " << numSplitSharedPix << std::endl;
        std::cout << "SplitSharedSCT  " << numSplitSharedSCT << std::endl;
        std::cout << "Shared/split    " << numSharedOrSplit  << std::endl;
        std::cout << "Shared/split px " << numSharedOrSplitPixels << std::endl;
        std::cout << "Shared          " << numShared         << std::endl;
        std::cout << "Weighted shared " << numWeightedShared << std::endl;
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
      unsigned int        nTSoS;
      std::vector<int>    type;                                   // The type of TSOS 
      std::vector<int>    detType;                                // The Detector type 1== Pixel, 11 = b-layer, 2 SCT, 3 TRT
      std::vector<int>    hitIsShared;                            // Number of tracks the hit is shared with
      std::vector<float>  splitProb1;                             // The Probablilty that the cluster on that surface is from 2 tracks
      std::vector<float>  splitProb2;                             // The Probablilty that the cluster on that surface is from 3 or more tracks
      std::vector<const Trk::RIO_OnTrack*> RIO;                   // The cluster on track
      std::multimap<const Trk::Track*, int, lessTrkTrack >  overlappingTracks;   // The tracks that overlap with the current track
      std::multimap< int, const Trk::Track*> tracksSharingHit;    // The tracks that overlap with the current track

      TSoS_Details()
      {
        std::cout << "WARNING DON'T USE THE DEFAULT CONSTRUCTOR OF tsosDetails" << std::endl;
      };
        
      TSoS_Details(unsigned int temp_nTSoS)
      { 
        nTSoS = temp_nTSoS;
        type.resize(nTSoS,OtherTsos);    
        detType.resize(nTSoS,-1.);
        hitIsShared.resize(nTSoS, 0 );
        splitProb1.resize(nTSoS,-1.) ;
        splitProb2.resize(nTSoS,-1.) ;
        RIO.resize(nTSoS,0);
      };
        
      int findIndexOfPreviousMeasurement( int currentIndex ) const
      {
        int indexPreviousMeasurement = currentIndex-1;
        while(indexPreviousMeasurement >= 0){
          if ( type[indexPreviousMeasurement] != OtherTsos ){
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

      int m_maxShared;    // Max shared hits -- calulated from  m_maxSharedModules
      int m_minNotShared; // Min number of hits that are not shared -- can change if we are in ROI
      int m_minSiHits;    // Min number of hits before we allow split sharing of hits -- can change if we are in ROI

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
                          const Trk::PRDtoTrackMap &prd_to_track_map,
                          TrackHitDetails& trackHitDetails,
                          TSoS_Details& tsosDetails ) const;

    /** Determine which hits to keep on this track*/
    bool decideWhichHitsToKeep(const Trk::Track*,
                               const Trk::TrackScore score,
                               Trk::PRDtoTrackMap &prd_to_track_map,
                               TrackHitDetails& trackHitDetails,
                               TSoS_Details& tsosDetails,
                               int nCutTRT,
                               CacheEntry* ent) const;

    /** Update the pixel clusters split information*/
    void updatePixelClusterInformation ATLAS_NOT_THREAD_SAFE (TSoS_Details& tsosDetails) const;
      
    /** Check if the cluster is compatible with a hadronic cluster*/
    bool isHadCaloCompatible(const Trk::TrackParameters& Tp, CacheEntry* ent) const;

    /** Check if the cluster is compatible with a EM cluster*/
    bool isEmCaloCompatible(const Trk::TrackParameters& Tp, CacheEntry* ent) const;
      
    /** Fill hadronic & EM cluster map*/
    void newEvent(CacheEntry* ent) const;
      
    /** Returns the number of track that use that hit already
        Need to let it know if that cluster is splittable
        maxiShared  = max number of shared hits on a  shared track
        which has maxothernpixel pixe hits and blayer hit if maxotherhasblayer is true
    */
    int checkOtherTracksValidity(const Trk::RIO_OnTrack*,
                                 bool isSplitable,
                                 Trk::PRDtoTrackMap &prd_to_track_map,
                                 int& maxiShared,
                                 int& maxothernpixel,
                                 bool& maxotherhasblayer,
                                 bool& failMinHits,
                                 CacheEntry* ent) const;


    /** Returns the Trackparameters of the two tracks on the n'th TrackStateOnSurface of the first track*/
    std::pair< const Trk::TrackParameters* , const Trk::TrackParameters* > 
    getOverlapTrackParameters(int n, const Trk::Track* track1,
                              const Trk::Track* track2,
                              const Trk::PRDtoTrackMap &prd_to_track_map,
                              int numSplitSharedPix ) const;

    /** Check if two sets of track paremeters are compatible with being from a the same low mass particle decay. 
        It is assumed that the track parmeters are on the same surface.*/
    bool isNearbyTrackCandidate(const Trk::TrackParameters* paraA, const Trk::TrackParameters* paraB) const;


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
    IntegerProperty m_maxSharedModulesInROI{this, "maxSharedModulesInROI", 2, "Max number of shared modules in ROI. Test value for recovering B jet efficiency at high pt"};
    IntegerProperty m_maxTracksPerPRD{this, "maxTracksPerSharedPRD", 2, "Max number of tracks per hit if it is nor split"};
    IntegerProperty m_minNotSharedModules{this, "minNotShared", 6, "Min number of non shared modules"};
    IntegerProperty m_minNotSharedModulesInROI{this, "minNotSharedInROI", 4, "Min number of non shared modules  in ROI. Test value for recovering B jet efficiency at high pt"};
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
    IntegerProperty m_maxPixMultiCluster{this, "maxPixMultiCluster", 4, "Max number of tracks that can be associated to a split cluster"};

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
    FloatProperty m_pairDeltaX{this, "pairDeltaX", 0.5, "Seperation distance in mm"};
    FloatProperty m_pairDeltaY{this, "pairDeltaY", 2.0};
    FloatProperty m_pairDeltaPhi{this, "pairDeltaPhi", 5e-2, "Seperation distance in rad"};
    FloatProperty m_pairDeltaEta{this, "pairDeltaEta", 5e-2};

    BooleanProperty m_monitorTracks{this, "MonitorAmbiguitySolving", false, "to track observeration/monitoring (default is false)"};
    BooleanProperty m_doSCTSplitting{this, "doSCTSplitting", false}; //WPM

  }; 
} // end of namespace


#endif 
