/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigL2SiTrackFinder.h
// 
// Description: a part of L2STAR project
// 
// date: 20/07/2010
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#ifndef __TRIG_L2_SITRACK_FINDER_H__
#define __TRIG_L2_SITRACK_FINDER_H__

#include<string>
#include<vector>
#include<algorithm>

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"

#include "IRegionSelector/RoiUtil.h"


class ISpacePointProvider;
class ITrigInDetTrackFitter;
class ITrigTRT_TrackExtensionTool;
class ITrigL2ResidualCalculator;
class IRegSelSvc;
class IBeamCondSvc;
class EventID;
class ITrigL2PattRecoStrategy;
class SCT_ID;
class PixelID;
class IRoiDescriptor;

namespace Trk {
  class ITrackSummaryTool;
}


/// just until we can change the TrigRoiDescriptor header
// bool roiContainsZed( const IRoiDescriptor& roi, double _z, double _r ); 
// bool roiContains( const IRoiDescriptor& roi, double z, double r, double phi);


class TrigL2SiTrackFinder : public HLT::FexAlgo
{

  class BaseSpacePointFilter {
  public:
    BaseSpacePointFilter(std::vector<const TrigSiSpacePoint*>& vec) : m_vec(vec){};
    virtual ~BaseSpacePointFilter(){};
    virtual void operator()(const TrigSiSpacePoint*) = 0;
    int vectorSize() {
      return m_vec.size();
    }
  protected:
    std::vector<const TrigSiSpacePoint*>& m_vec;
  };

  class FullScanFilter : public BaseSpacePointFilter {
  public: 
    FullScanFilter(std::vector<const TrigSiSpacePoint*>& vec) : BaseSpacePointFilter(vec) {};
    virtual void operator()(const TrigSiSpacePoint* p) {
      m_vec.push_back(p);
    }
  };

  class StandardRoI_Filter : public BaseSpacePointFilter {
  public:
    StandardRoI_Filter(std::vector<const TrigSiSpacePoint*>& vec, double min, double max) : BaseSpacePointFilter(vec), 
      m_roiPhiMin(min), m_roiPhiMax(max) {};
    virtual void operator()(const TrigSiSpacePoint* p) {
      // std::cout<<"r="<<p->r()<<" orig r="<<p->original_r()<<" phi="<<p->phi()<<" orig phi="<<p->original_phi()<<std::endl;
      if( p->original_phi() > m_roiPhiMin && p->original_phi() < m_roiPhiMax) m_vec.push_back(p);
    }
  private:
    double m_roiPhiMin, m_roiPhiMax;
  };

  class BoundaryRoI_Filter : public BaseSpacePointFilter {
  public:
    BoundaryRoI_Filter(std::vector<const TrigSiSpacePoint*>& vec, double min, double max) : BaseSpacePointFilter(vec), 
      m_roiPhiMin(min), m_roiPhiMax(max) {};
    virtual void operator()(const TrigSiSpacePoint* p) {
      //std::cout<<"*r="<<p->r()<<" orig r="<<p->original_r()<<" phi="<<p->phi()<<" orig phi="<<p->original_phi()<<std::endl;
      if( p->original_phi() > m_roiPhiMin || p->original_phi() < m_roiPhiMax) m_vec.push_back(p);
    }
  private:
    double m_roiPhiMin, m_roiPhiMax;
  };



  
  class RoI_Filter : public BaseSpacePointFilter {
  public:
  RoI_Filter(std::vector<const TrigSiSpacePoint*>& vec, const IRoiDescriptor* roi )
    : BaseSpacePointFilter(vec),
      m_roi(roi) { }
    virtual void operator()(const TrigSiSpacePoint* p) {
      /// this is a hack for filtering in phi *and* r-z, but only until  
      /// we are allowed to modify the *RoiDescriptor headers    
      if( RoiUtil::contains( *m_roi, p->z(), p->original_r(), p->original_phi() ) ) m_vec.push_back(p);
      /// filter on phi only for the time being
      // if( m_roi->containsPhi( p->original_phi() ) ) m_vec.push_back(p);
    }
  private:
    const IRoiDescriptor* m_roi;
  };
  



  template < typename T> class SpacePointSelector {
  public:
    SpacePointSelector(T& t) : m_filter(t), m_pIDC(nullptr) {};
    SpacePointSelector() = delete;
    ~SpacePointSelector(){};
    void operator() (int id) {
      TrigSiSpacePointContainer::const_iterator collIt = m_pIDC->indexFind(id);
      if(collIt!=m_pIDC->end()) {
	std::for_each((*collIt)->begin(),(*collIt)->end(),m_filter);
      }
    }
    int select(const TrigSiSpacePointContainer* p, const std::vector<int>& idVec) {
      m_pIDC=p;
      int ret = m_filter.vectorSize();
      std::for_each(idVec.begin(),idVec.end(),*this);
      return m_filter.vectorSize()-ret;
    }

  private:
    T& m_filter;
    const TrigSiSpacePointContainer* m_pIDC;
  };



 public:
  
  TrigL2SiTrackFinder(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2SiTrackFinder();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltStart();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE);

 private:

  // AlgTools and Services

  ToolHandle<ISpacePointProvider> m_spacePointProvider;
  ToolHandle<ITrigInDetTrackFitter> m_trigInDetTrackFitter;
  ToolHandle<ITrigTRT_TrackExtensionTool> m_trigTRT_TrackExtensionTool;
  ToolHandle<ITrigL2ResidualCalculator> m_trigL2ResidualCalculator;
  

  ToolHandle<ITrigL2PattRecoStrategy> m_recoStrategy;
  ToolHandle< Trk::ITrackSummaryTool > m_trackSummaryTool;
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  

  // Control flags

  bool m_useBeamSpot; 
  bool m_fullScanMode; 
  bool m_vertexSeededMode; 
  bool m_doTrackFit;
  bool m_doShift;//must be m_doCosmic ?
  bool m_doTRTpropagation;
  bool m_printDiagnosticMessages;
  bool m_adjustLayerThreshold;
  bool m_doResMonitoring;
  bool m_attachVertices;

  // Cuts

  int  m_minHits;

  // RoI parameters 

  //bool m_generateRoI;
  bool m_useRoiDescriptorWidths;
  double m_etaHalfWidth;
  double m_phiHalfWidth;

  // RoI features

  // Names of IDCs with input data 

  std::string m_pixelSpContName, m_sctSpContName;

  // IDCs with input data 

  const TrigSiSpacePointContainer* m_pixelSpacePointsContainer;
  const TrigSiSpacePointContainer* m_sctSpacePointsContainer;

  // Internal representation of input data, for example vector of SPs

  std::vector< const TrigSiSpacePoint* > m_filteredSpacePoints;

  // Reconstructed tracks 

  TrigInDetTrackCollection* m_recoTracks;
  TrackCollection* m_recoTrkTracks;

  std::string m_regionSelectorName;
  IRegSelSvc*       m_regionSelector;

  IBeamCondSvc* m_iBeamCondSvc;
  
  // Data members for monitoring

  float nStraw_TRT; 
  float nTR_TRT;
  int m_ntracks;
  int m_nPixSPsInRoI;  // Total number of (filtered) pixel SPs in the RoI
  int m_nSCTSPsInRoI;  // Total number of (filtered) SCT SPs in the RoI
  int m_currentStage;  // The last stage reached during the processing of a given RoI

  int m_roi_nSPs;

  double m_roiPhi, m_roiEta;  
  double m_roiPhiWidth, m_roiEtaWidth;  

  int m_nZvertices; //Total number of z-vertices used by patt.reco strategy
  std::vector<float> m_zVertices; //vector with reconstructed z-vertex positions

  std::vector<double> m_a0beam;
  std::vector<double> m_trkdPhi0, m_trkdEta;

  std::vector<double> m_sp_x, m_sp_y, m_sp_z, m_sp_r;

  std::vector<double> m_pixResPhiBarrel;
  std::vector<double> m_pixResEtaBarrel;
  std::vector<double> m_pixPullPhiBarrel;
  std::vector<double> m_pixPullEtaBarrel;
  std::vector<double> m_sctResBarrel;
  std::vector<double> m_sctPullBarrel;
  std::vector<double> m_pixResPhiEC;
  std::vector<double> m_pixResEtaEC;
  std::vector<double> m_pixPullPhiEC;
  std::vector<double> m_pixPullEtaEC;
  std::vector<double> m_sctResEC;
  std::vector<double> m_sctPullEC;
  
  std::vector<int> m_dataErrors;
  std::vector<int> m_pixelDataErrors;
  std::vector<int> m_sctDataErrors;
  std::vector<int> m_trtDataErrors;

  // Monitoring member functions 

  static inline double monPt(const TrigInDetTrack *t){return t->param()->pT(); }
  static inline double monA0(const TrigInDetTrack *t){return t->param()->a0(); }
  static inline double monZ0(const TrigInDetTrack *t){return t->param()->z0(); }
  static inline double monPhi0(const TrigInDetTrack *t){return t->param()->phi0();}
  static inline double monEta(const TrigInDetTrack *t){return t->param()->eta(); }
  static inline double monNHit_Si(const TrigInDetTrack *t){return t->siSpacePoints()->size();}
  static inline double monChi2(const TrigInDetTrack *t){return (t->chi2()>9e7)?-9.9:t->chi2();}

  // Timers 

  TrigTimer* m_SpacePointReaderTimer;
  TrigTimer* m_PatternRecoTimer; 
  TrigTimer* m_TrackFitterTimer; 
  TrigTimer* m_TrtExtensionTimer;

  // Other member functions

  //  void storeInfo(double, HLT::TriggerElement*);

  HLT::ErrorCode retrieveSpacePointsContainers();

  void setup_detector_mask(const EventID*);
  void convertToTrkTrack(const TrigInDetTrackCollection* oldTracks,
      TrackCollection* fittedTracks) const;
  int  findMissingLayers(const double& , const double&, const double& , const double&); 
  int  findMissingLayers(const TrigRoiDescriptor* roi);
  void runResidualMonitoring(const TrigInDetTrack*);

  // Internal bookkeeping

  std::string m_instanceName, m_attachedFeatureName,m_attachedFeatureName2,
    m_outputCollectionSuffix;

  unsigned int m_l1Id;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countRoIwithEnoughHits;
  unsigned int m_countRoIwithTracks;

  // Masking stuff - there might be a better place for these ...

  BooleanProperty m_check_detector_mask;
  BooleanProperty m_detector_mask_on_event;

  bool m_detector_mask_not_checked;
  bool m_hasPixelBarrel, m_hasPixelDisk, m_hasPixelBLayer;
  bool m_hasSCTBarrelA, m_hasSCTEndcapA;
  bool m_hasSCTBarrelC, m_hasSCTEndcapC;

	//Only print roi warning message once per event 
	bool m_roiWidthWarning=false;
	bool m_roiForIDWarning=false;

};

#endif
