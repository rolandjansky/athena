/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONTRACKPREPROCESSOR_H
#define MUONALIGNGENTOOLS_MUONTRACKPREPROCESSOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "DataModel/DataVector.h"
#include "TrkAlignInterfaces/IAlignTrackPreProcessor.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

 
/**
   @file MuonTrackPreProcessor.h
   @class MuonTrackPreProcessor

   @brief Tool for processing collections of tracks.  Removes unwanted hits from tracks.

   @author Robert Harrington <roberth@bu.edu>
   
   @date 10/25/2009
*/

class AtlasDetectorID;
class ITrackingVolumesSvc;
class TTree;

namespace Trk {
  class Track;
  class AlignTrack;
  class ITrackFitter;
  //class ITrackSelectorTool;
  class IResidualPullCalculator;
  class MeasurementTypeID;
  class Volume; 
}

class TFile;
class TTree;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  
  class MuonIdHelperTool;
  class IMuonSegmentTrackBuilder;
  class MuonAlignRefitTool;
  class MuonEDMPrinterTool;
  class MuonAlignHelperTool;

  class MuonTrackPreProcessor : virtual public Trk::IAlignTrackPreProcessor, public AthAlgTool {

  public:
    MuonTrackPreProcessor(const std::string& type, const std::string& name,
			  const IInterface* parent);
    virtual ~MuonTrackPreProcessor();
    
    StatusCode initialize();
    StatusCode finalize();
    
    DataVector<Trk::Track>* processTrackCollection(const DataVector<Trk::Track>* trks);
    
    void setNtuple(TFile* file) { m_ntuple=file; }
    StatusCode fillNtuple();

  private:

    void dumpChamberNames(const Trk::Track& track) const;
    void printCompetingROTs(const Trk::Track& track) const;
    void calculateTrackChi2(const Trk::Track* track);

    // private methods
    Trk::AlignTrack* refitTrack(const Trk::Track& origTrack) const;
    const Trk::Track* removeHits(const Trk::Track& origTrack) const;
    const Trk::Track* declusterTGCHits(const Trk::Track& origTrack) const;
    
    bool passesStandardTrackSelection(const Trk::Track& origTrack);
    bool passesInitTrackSelection(const Trk::Track& origTrack);
    bool passesFinalTrackSelection(const Trk::Track& origTrack);
    
    int getMdtStatLayer(Identifier id) const;

    void identifyScatterers(const Trk::Track& origTrack) const;
    
    void createTree();

    bool isEndcap(Identifier id) const; // checks whether isEndcap or BEE

    bool isBIS8(Identifier id) const; // checks whether BIS8 chamber

    double trackSagitta(const Trk::Track& track) const;

    const MuonGM::MuonDetectorManager* p_muonMgr;

    StoreGateSvc* m_storeGate;

    TFile* m_ntuple;  //!> output ntuple

    // private variables
    //ToolHandleArray<Trk::ITrackSelectorTool>  m_trackSelectorTools; 
    ToolHandle<Trk::ITrackFitter>  m_trackFitterTool;
    ToolHandle<Trk::ITrackFitter>  m_SLTrackFitterTool;  
    ToolHandle<MuonIdHelperTool>   m_muonIdHelperTool; 

    ToolHandle<Muon::IMuonSegmentTrackBuilder> m_segmentTrackBuilder;

    ToolHandle<Trk::IResidualPullCalculator>   m_pullCalculator;
    
    ToolHandle<Muon::MuonAlignRefitTool> m_muonAlignRefitTool;

    ToolHandle<Muon::MuonAlignHelperTool> m_muonAlignHelperTool;

    ToolHandle<Muon::MuonEDMPrinterTool>  m_printer;

    //ServiceHandle<ITrackingVolumesSvc> m_trackingVolumesSvc;
    //const Trk::Volume*                 m_msVolume;
    //const Trk::Volume*                 m_calVolume;
    //const Trk::Volume*                 m_indetVolume;

    const AtlasDetectorID*   m_idHelper;
    Trk::MeasurementTypeID* m_measTypeIdHelper;

    bool m_refitTracks;                      //!> refit tracks with new fitter
    //bool m_runOutlierRemoval;                //!> run outlier removal in track refit
    int  m_particleNumber;
    Trk::ParticleHypothesis m_particleHypothesis; //!> particle hypothesis in track refit

    bool m_resetScatteringAngles;   //!> reset scattering angles before refit
    bool m_removeScattererTSOS;     //!> remove TSOS of type Scatterer

    //bool m_redoErrorScaling;        //!> uses recalibrateHitsOnTrack method of MuonSegmentTrackBuilder, which redoes ROT creation with error scaling (if ROT creator configured with error scaling)

    //bool m_removePerigeeBeforeRefit; //!> remove perigee from track before refit
    bool m_pTCorrectTrack;           //!> correct pT for first TSOS on track

    //bool m_declusterTGCHits;        //!> break up TGC Competing RIOsOnTrack into RIO_OnTrack
    //bool m_cutOnBarrel;             //!> remove tracks that have barrel hits

    bool m_applyStandardSelectionCuts;  //!> apply standard selection cuts
    bool m_applySelectionCuts;          //!> apply selection cuts
    bool m_requireOneHitPerTubeLayerEC; //!> require one tube hit per multilayer (for MDT hits in endcap only)
    bool m_requireOuterLayerEndcapMdt;  //!> remove tracks that don't have a hit in the outer layer MDT endcap
    bool m_requireInnerLayerEndcapMdt;  //!> remove tracks that don't have a hit in the inner layer MDT endcap
    bool m_requireSmallLargeOverlap;    //!> select only small/large overlap tracks
    bool m_requireSectorOverlap;        //!> select only tracks hitting m_sector1 as well as m_sector2
    int  m_sector1;                     //!> selected sector for overlap requirement
    int  m_sector2;                     //!> selected sector for overlap requirement

    bool m_requireBarrelECOverlap;      //!> remove tracks that don't overlap barrel and EC
    bool m_requireRPCPhiHit;            //!> use only tracks which have a RPC phi hit

    unsigned int m_minMdtChambers;      //!> min number of MDT chambers required on track
    unsigned int m_minSmallMdtChambers; //!> min number of small MDT chambers required on track
    unsigned int m_minLargeMdtChambers; //!> min number of large MDT chambers required on track

    bool   m_selectCombinedMuons;  //!> select only muons with combined tracks
    double m_MSID_ptDiffCut;       //!> cut on relative difference of MS and ID pT
    double m_maxDriftSignSumInner; //!> cut on sum of drift radius signs for inner layer multilayers
    double m_maxDriftSignSum;      //!> cut on sum of drift radius signs for each multilayer
    double m_chiSqCut;             //!> cut on normalized chi2
    int    m_nAllowedOutliers;     //!> number of outliers allowed on track
    int    m_nAllowedHoles;        //!> number of holes allowed on track
    std::string m_muonContainer;   //!> name of muon container


    // tree variables
    TTree*  m_tree;                           //!> output tree
    int     m_runNumber;
    int     m_evtNumber;
    int     m_nresiduals;
    double* m_residuals;
    double* m_respulls;
    int     m_nInnerLayersHit;
    int     m_nMiddleLayersHit;      
    int     m_nOuterLayersHit;       
    int     m_nOtherLayersHit;       
    int     m_nTubeLayersHitInner;
    int     m_nTubeLayersHitMiddle;
    int     m_nTubeLayersHitOuter;
    int     m_nTubeLayersHitOther;
    int     m_nChambersHit;
    int     m_nLargeChambersHit;
    int     m_nSmallChambersHit; 
    double  m_driftSignsInnerLayer;
    double  m_driftSignsMiddleLayer;
    double  m_driftSignsOuterLayer;
    double  m_driftSignsOtherLayer;
    int     m_nInnerLayerOutliers;   
    int     m_nMiddleLayerOutliers;  
    int     m_nOuterLayerOutliers;   
    int     m_nOtherLayerOutliers;   
    int     m_nInnerLayerHoles;      
    int     m_nMiddleLayerHoles;     
    int     m_nOuterLayerHoles;      
    int     m_nOtherLayerHoles;      
    int     m_trackNDoF;             
    double  m_trackChi2;             
    double  m_trackPt;               
    double  m_trackEta;
    double  m_qOverP;
    //double  m_trackSagitta;
    // residuals

    int     m_nIDres;
    double* m_IDres;
    double* m_IDerr;

    int     m_nIDscatRes;
    double* m_IDscatPhiRes;
    double* m_IDscatPhiErr;
    double* m_IDscatThetaRes;
    double* m_IDscatThetaErr;

    int     m_nMSres;
    double* m_MSres;
    double* m_MSerr;
    int*    m_MSstatLayer;
    int*    m_MSisEndcap;

    int     m_nMStrigRes;
    double* m_MStrigRes;
    double* m_MStrigErr;
    int   * m_MStrigStatLayer;
    int   * m_MStrigIsEndcap;

    int     m_nMSscatRes;
    double* m_MSscatPhiRes;
    double* m_MSscatPhiErr;
    double* m_MSscatThetaRes;
    double* m_MSscatThetaErr;

    int     m_nCalRes;
    double* m_CalRes;
    double* m_CalErr;

    int     m_nChambers;
    int*    m_chamberId;
    int*    m_resIndex;
    
    double  m_beePosX;
    double  m_beePosY;

    // cut flow variables
    mutable int m_nProcessed;
    mutable int m_nPassHitRemoval;
    mutable int m_nPassTGCHitRemoval;
    mutable int m_nPassBarrelVeto;
    mutable int m_nPassInnerLayer;
    mutable int m_nPassOuterLayer;
    mutable int m_nPassSmallLargeOverlap;
    mutable int m_nPassSectorOverlap;
    mutable int m_nPassMinChambers;
    mutable int m_nPassMinSmallChambers;
    mutable int m_nPassMinLargeChambers;
    mutable int m_nPassRPCPhiHit;
    mutable int m_nPassStandardTrackSel;
    mutable int m_nPassInitTrackSel;
    mutable int m_nPassTubeLayerEC;
    mutable int m_nPassTubeLayerBarrel;
    mutable int m_nPassDriftSignSumEC;
    mutable int m_nPassDriftSignSumBarrel;
    mutable int m_nPassTrackRefit;
    mutable int m_nPassTrackChi2Cut;
    mutable int m_nPassOutlierCut;
    mutable int m_nPassHoleCut;
    mutable int m_nPassFinalTrackSel;

    mutable int m_nProcessTrackSelection;
    mutable int m_nPassBarrelECOverlap;
    mutable int m_nPassNMdtHits;
    mutable int m_nPassNLayers;
    mutable int m_nPassTrackChi2;
    mutable int m_nPassMomentum;
    mutable int m_nPassD0;
    mutable int m_nPassZ0;
  };
}
#endif // MUONALIGNGENTOOLS_MUONTRACKPREPROCESSOR_H
