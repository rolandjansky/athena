/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UNBIASEDMUONRESIDUALS_H
#define UNBIASEDMUONRESIDUALS_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrkAlignEvent/AlignModule.h"

#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"
//#include "TrkEventPrimitives/GlobalDirection.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkParameters/MeasuredAtaPlane.h"

#include "TrkAlignEvent/AlignModule.h"

#include <vector>
#include <list>
#include <string>
#include <map>

class StoreGateSvc;
class Identifier;

class TFile;
class TTree;

namespace Trig{
  class ITrigDecisionTool;
}

namespace Trk{
  class Track;
  class Segment;
  class IExtrapolator;
  class PlaneSurface;
  class ITrackFitter;
  class IResidualPullCalculator;
}

namespace Analysis {
  class Muon;
  class MuonContainer;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Rec {
  class ICombinedMuonTrackBuilder;
}

namespace MuonCombined {
  class IMuonTrackTagTool;
}

namespace Muon {

  class MuonSegment;
  class MuonSegmentCombination;
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  class IMuonSegmentTrackBuilder;
  class MuonAlignHelperTool;
  class MuonAlignRefitTool;
  class IMuonSegmentTrackBuilder;
  //class MuonRefitTool;
  class IMdtDriftCircleOnTrackCreator;
  class IMuonClusterOnTrackCreator;
  class MuonEDMPrinterTool;

  class UnbiasedMuonResiduals: public AthAlgorithm {
    
  public:
    enum TrackFittingType {

      /** uses iPat, combines separately-fit ID and MS standalone tracks into a combined track */
      CombinedMuonTrackBuilder   = 0,

      /** uses track fitter to fit standalone muon, using ID momentum for multiple scattering*/
      MSStandaloneWithIDMomentum = 1,

      /** uses track fitter to refit combined muon track */
      NormalTrackFitter          = 2,
      
      /** uses iPat, does combined fit, and then selects standalone track */
      MSStandaloneFromCombinedMuon = 3,

      /** uses muon standalone, phi/eta matched to ID track */
      MSStandaloneWithIDTrackMatch = 4,

      /** maximum number of enums */
      NumberOfTrackFittingTypes  = 5
    };

    UnbiasedMuonResiduals( const std::string& name,
			   ISvcLocator* pSvcLocator);
    ~UnbiasedMuonResiduals();
    
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();
    
    typedef std::pair<Muon::MuonSegment*, const Analysis::Muon*> SegMatch;
    typedef std::pair<Muon::MuonSegment*, const Trk::Track*> SegMatchTrack;

  private:

    /** name of the segment collection to retrieve from StoreGate */
    std::vector<int>                             m_segmCollectionFlag;
    std::vector<std::string>                     m_segmCollectionName; 
    std::vector<std::string>                     m_trackCollectionName;
    std::vector<std::string>                     m_inDetTrackCollectionName;
    
    /** pointer to detector manager*/
    const MuonGM::MuonDetectorManager*           m_detMgr;  
    
    /** a handle on Store Gate for access to the Event Store */
    StoreGateSvc*                                m_storeGate;
    
    /** class member version of retrieving MsgStream */
    ToolHandle<MuonCombined::IMuonTrackTagTool>  m_tagTool;
    ToolHandle<Muon::MuonEDMHelperTool>          p_muonEDMHelperTool;
    ToolHandle<Muon::MuonIdHelperTool>           p_idHelperTool;
    ToolHandle<Muon::IMuonSegmentTrackBuilder>   p_trackBuilder;
    //ToolHandle<Muon::MuonRefitTool>              p_trackRefitTool;
    ToolHandle<Trk::IExtrapolator>               p_IExtrapolator;
    ToolHandle<Rec::ICombinedMuonTrackBuilder>   m_combinedTrackBuilder; //!< combined track builder
    ToolHandle<Trk::ITrackFitter>                m_trackFitter;          //!< fitter
    ToolHandle<Trig::ITrigDecisionTool>          m_trigDec;              //!< trigger decision tool
    ToolHandle<Trk::IResidualPullCalculator>     m_pullCalculator;       //!< residual pull calculator
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtRotCreator;     //!< mdt ROT creator
    ToolHandle<Muon::IMuonClusterOnTrackCreator>    m_cscRotCreator;     //!< csc ROT creator

    ToolHandle<Muon::MuonAlignHelperTool>        m_muonAlignHelperTool;
    ToolHandle<Muon::MuonAlignRefitTool>         m_muonAlignRefitTool;
    ToolHandle<Muon::MuonEDMPrinterTool>         m_printer;

    void checkCovVar(const Trk::Track& track) const;
 
    bool passesChamberSelection(Trk::AlignModule::DetectorType detType,
				Identifier id, int flag=0);
    
    /** methods called by fillHistograms */
    StatusCode getSegments( Trk::AlignModule::DetectorType detType,
			    std::vector<SegMatch>& muonInn, int iflag);

    StatusCode getSegments( Trk::AlignModule::DetectorType detType,
			    std::vector<SegMatchTrack>& muonInn, int iflag);

    //StatusCode getAllSegments( const std::string& segmCollectionName, 
    //		       const std::string& trackCollectionName, 
    //		       std::vector<SegMatch>& muonInn);
    
    const Trk::PlaneSurface* getDetectorSurface(Trk::AlignModule::DetectorType detType,
						const Muon::MuonSegment* mSeg); 

    const Trk::Track* getTrackFromTrack(const Trk::Track* track);
    const Trk::Track* getTrackFromMuon(const Analysis::Muon* muon);

    const Trk::Track* getTrack(const Trk::Track* idTrack,
			       const Trk::Track* muTrack,
			       const Trk::Track* combTrack);
    
    Trk::Track* getUnbiasedTrack(Identifier id1,
				 const Trk::Track* initTrack,
				 const Trk::PlaneSurface* detSurface,
				 Trk::AlignModule::DetectorType detType,
				 int flag=0);

    const Trk::TrackParameters* getIDTrackExtrapolated(const Trk::Track* track,
						       const Trk::PlaneSurface* detSurface);
    
    const Trk::TrackParameters* getSegmentExtrapolated(const Muon::MuonSegment* mSeg,
						       const Trk::PlaneSurface* detSurface);
        
    const Trk::TrackParameters* getTrackExtrapolated(Identifier id,
						     const Trk::Track* unbiasedTrack,
						     const Trk::PlaneSurface* detSurface);
    
    const Analysis::Muon* findMuon(const Analysis::MuonContainer* muonTES, 
				   Muon::MuonSegment* mSeg) const;

    const Trk::Track* findBestTrack(Trk::AlignModule::DetectorType detType,
				    const TrackCollection* muonTracks,
				    const TrackCollection* idTracks,
				    Muon::MuonSegment* mSeg);
    
    const Trk::Track* prepareTrack(const Trk::Track& track, double qOverP);

    void compareSurfaces(const std::string& trackType,
			 const Trk::Surface& segSurface, 
			 const Trk::Surface& detSurf);
    
    //const Trk::MeasuredAtaPlane* trackParameters(const Trk::TrackStateOnSurface* tsit, 
    //						 const Trk::PlaneSurface& surface);  
    
    const Trk::TrackStateOnSurface* cloneTSOS(const Trk::TrackStateOnSurface* tsos,
					      bool makeOutlier=false);
    
    Trk::Track* cleanTrack(Identifier id, const Trk::Track& initTrack,
			   bool removePerigee);

    bool checkMuon(const Analysis::Muon& muon);
    bool checkTrack(const Trk::Track& track);
    
    void getBiasedResiduals(const Trk::Track& track);

    int  getMdtStatLayer(Identifier id) const;

    void setIdentificationVariables(Trk::AlignModule::DetectorType detType, 
				    const MuonSegment& mSeg);

    void calcResForExtrapolatedIDToDetSurface(const Analysis::Muon& muon,
					      const Trk::TrackParameters& segPar,
					      const Trk::PlaneSurface* detSurface,
					      Trk::AlignModule::DetectorType detType);
    
    void calcResForSegmentAtDetSurface(const Trk::TrackParameters& trackPar,
				       const Trk::TrackParameters& segPar,
				       const Trk::ErrorMatrix& trkErrMat,
				       const Trk::ErrorMatrix& segErrMat);

    void removeExtraSegments(Trk::AlignModule::DetectorType detType,
			     std::list<Muon::MuonSegment*>& muonSegments) const;

    void printPerigee(const Trk::Track& track);

    Trk::Track* correctErrors(const Trk::Track& track);

    std::set<Identifier> chamberIds(Trk::AlignModule::DetectorType detType, 
				    const MuonSegment& seg) const;

    Identifier chamberId(Trk::AlignModule::DetectorType detType, const MuonSegment& seg) const;
    Identifier triggerChamberId(const MuonSegment& seg) const;

    void getTrackIds(const Trk::Track& track, std::list<Identifier>& trackIds) const;

    std::vector<std::string> m_triggerList;

    bool m_doMdt;
    bool m_doCsc;
    bool m_doRpc;
    bool m_doTgc;

    bool m_makeTGCOutliers;
    bool m_redoErrorScaling;

    /** tree variables */
    TFile* m_file;
    TTree* m_tree;
    
    int m_run;
    int m_evt;
    int m_nPixHits;
    int m_nSCTHits;
    int m_nTRTHits;
    int m_nCSCHits;
    int m_nMDTHits;
    int m_nRPCHits;
    int m_nTGCHits;

    int m_nInnerMDTHits;
    int m_nMiddleMDTHits;
    int m_nOuterMDTHits;
    int m_nTriggerPhiHits;

    const int m_MAXNMDTHITS;
    const int m_MAXNTGCHITS;
    const int m_MAXNCSCHITS;
    const int m_MAXNRPCHITS;

    double* m_cscHitLocX;
    double* m_cscHitLocY;
    double* m_cscHitErrors;
   
    double* m_mdtRes;
    double* m_tgcRes;
    double* m_cscRes;
    double* m_rpcRes;

    double* m_mdtPull;
    double* m_tgcPull;
    double* m_cscPull;
    double* m_rpcPull;   

    int m_author;
    
    int m_hIndex;
    int m_phiSector;
    int m_isEndcap;
    int m_identifier;
    int m_isCSide;
    
    int m_detType;
    
    int m_layer;
    int m_stName;
    int m_stPhi;
    int m_stEta;
    
    double m_momentum;
    double m_charge;
    double m_d0;
    double m_z0;
    double m_phi0;
    double m_theta0;
    double m_eta0;
    double m_trackChi2;
    double m_calEnergyLoss;
    double m_calEnergyLossErr;
    double m_materialOnTrack;
    
    double m_deltaPhi;
    double m_deltaTheta;

    double m_dR;
    double m_dZlocal;
    double m_dXlocal;
    double m_dYlocal;
    double m_dXlocalAngle;
    double m_dYlocalAngle;

    double m_pullXlocal;
    double m_pullYlocal;
    double m_pullXlocalAngle;
    double m_pullYlocalAngle;

    double m_dXglobal;
    double m_dYglobal;
    double m_dZglobal;
    double m_dXglobalAngle;
    double m_dYglobalAngle;
    double m_dZglobalAngle;
    
    double m_detTheta;
    double m_detThetaErr;

    /** Quality/multiplicity cuts */
    float m_MinMuonMom;
    int   m_minSCTHits, m_minPIXHits, m_minTRTHits, m_minMDTHits, m_minCSCHits;
    float m_MaxIDd0, m_MaxIDz0, m_MaxIDChiDof;
    
    double m_trackMatchDeltaR;
    double m_matchChi2Cut;

    int m_trackFittingType;

    bool m_extrapolateIDTrack;
    bool m_useTrackContainer;

    std::string m_muonTgcPrdKey; //TGC Container;
    std::string m_muonTgcPrdKey_prev; //TGC Container;
    std::string m_muonTgcPrdKey_next; //TGC Container;
    
    std::vector<std::string> m_listOfTriggers;
  };

} // end namespace

#endif
