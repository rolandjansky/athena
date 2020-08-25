/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscSegmentUtilTool_H
#define CscSegmentUtilTool_H

// Utilities for building segments.
#include "CscSegmentMakers/ICscSegmentUtilTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "CscClusterization/ICscClusterUtilTool.h"
#include "MuonRecToolInterfaces/ICscClusterOnTrackCreator.h"
#include "MuonCondData/CscCondDbData.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <string>
#include <vector>

namespace Trk {
  class PlaneSurface;
}
namespace Muon {
  class MuonSegment;
  class CscPrepData;
}
class ICscSegmentFinder;

class CscSegmentUtilTool : virtual public ICscSegmentUtilTool, public AthAlgTool {

public:

  // Constructor.
  CscSegmentUtilTool(const std::string& type, const std::string& name,
                 const IInterface* parent );

  // Destructor.
  virtual ~CscSegmentUtilTool()=default;
  
  virtual StatusCode initialize();

  // calls get2dMuonSegmentCombination and get4dMuonSegmentCombination with 2d segments!!
  std::unique_ptr<std::vector<std::unique_ptr<Muon::MuonSegment> > >
  getMuonSegments(Identifier eta_id, Identifier phi_id,
                  ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                  ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                  const Amg::Vector3D& lpos000 ) const;

  ////////////////////////////////////////    
  // calls get2dMuonSegmentCombination and get4dMuonSegmentCombination with 2d segments!!
  Muon::MuonSegmentCombination*
  get4dMuonSegmentCombination(Identifier eta_id, Identifier phi_id,
                              ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                              ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                              const Amg::Vector3D& lpos000) const;

  Muon::MuonSegmentCombination*
  get4dMuonSegmentCombination(const Muon::MuonSegmentCombination* Muon2dSegComb) const;  

  //get2dMuonSegmentCombination : get2dSegments does
  //  -> (1) find_2dsegments (2) find_2dseg3hit (3) add_2dsegments
  Muon::MuonSegmentCombination*
  get2dMuonSegmentCombination(Identifier eta_id, Identifier phi_id,
                              ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                              ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                              const Amg::Vector3D& lpos000, int etaStat=0, int phiStat=0 ) const;

  
  // Return the counts of spoiled and unspoiled measurements from a list
  // of RIOS's.
  void spoiled_count(const ICscSegmentFinder::RioList& rios, double threshold,
                     int& nspoil, int& nunspoil);
  void spoiled_count(const ICscSegmentFinder::RioList& rios, int& nspoil, int& nunspoil); 
  void spoiled_count(const ICscSegmentFinder::RioList& rios, int& nspoil, int& nunspoil, int& spoilmap); 

  // Fit a RIO residual.
  void fit_rio_residual(const Trk::PlaneSurface& ssrf, bool dump,
                        const ICscSegmentFinder::RioList& clus, unsigned int irclu,
                        double& res, double& dres, double& rs, double& drs // for 3pt-method
                        ) const;
  

private:
  Gaudi::Property<double> m_max_chisquare_tight {this, "max_chisquare_tight", 16, "for outlier removal"};
  Gaudi::Property<double> m_max_chisquare_loose {this, "max_chisquare_loose", 2000, "for outlier removal"};
  Gaudi::Property<double> m_max_chisquare {this, "max_chisquare", 25};
  Gaudi::Property<double> m_max_slope_r {this, "max_slope_r", 0.2};
  Gaudi::Property<double> m_max_slope_phi {this, "max_slope_phi", 0.2};
  Gaudi::Property<double> m_min_xylike {this, "min_xylike", -1, "Minimum value used for xy matching of 4D segments"};
  Gaudi::Property<double> m_fitsegment_tantheta_tolerance {this, "tantheta_update_tolerance", 0.0001};
  Gaudi::Property<double> m_IPerror {this, "IPerror", 250};

  Gaudi::Property<float> m_cluster_error_scaler {this, "cluster_error_scaler", 1};

  Gaudi::Property<int> m_nunspoil {this, "UnspoiledHits", -1};
  Gaudi::Property<int> m_max_3hitseg_sharehit {this, "max_3hitseg_sharedhit", 0};

  Gaudi::Property<unsigned int> m_max_seg_per_chamber {this, "max_seg_per_chamber", 50};

  Gaudi::Property<bool> m_x5data {this, "X5data", false};
  Gaudi::Property<bool> m_zshift {this, "zshift", true};
  Gaudi::Property<bool> m_IPconstraint {this, "IPconstraint", true};
  Gaudi::Property<bool> m_allEtaPhiMatches {this, "allEtaPhiMatches", true};
  Gaudi::Property<bool> m_TightenChi2 {this, "TightenChi2", true};
  Gaudi::Property<bool> m_remove4Overlap {this, "Remove4Overlap", true};
  Gaudi::Property<bool> m_remove3Overlap {this, "Remove3Overlap", true};

  ToolHandle<Muon::ICscClusterOnTrackCreator> m_rotCreator{this, "rot_creator", "Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  
  SG::ReadCondHandleKey<CscCondDbData> m_readKey{this, "ReadKey", "CscCondDbData", "Key of CscCondDbData"};   
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};

  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  // Convert a local 2D segment to MuonSegment
  Muon::MuonSegment*  build_segment(const ICscSegmentFinder::Segment& seg, bool measphi, Identifier chid, bool use2Lay) const;

  // Fit a list of RIO's to form a 2D segment.
  // The fit is relative to the passsed surface. called by fit_rio_residual
  void fit_rio_segment(const Trk::PlaneSurface& ssrf, bool dump,
                       const ICscSegmentFinder::RioList& clus, double& s0, double& s1,
                       double& d0, double& d1, double& d01, double& chsq, double& zshift) const;
  
  // Fit a residual = difference between cluster position and prediction from
  // the remaining clusters in a list.
  // clus = list of clusters
  // rslu is cluster for which residual is calculated
  // res, dres = residual and error in residual  // looks obsolete
  void fit_residual(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, unsigned int irclu,
                    double& res, double& dres) const;
  

  // Find segments in a chamber.
  //  calls find_outlier_cluster(Muon::CscClusterOnTrack* const clus[4]) const;
  void find_2dsegments(bool measphi, int station,  int eta, int phi,
                       const ICscSegmentFinder::ChamberTrkClusters& clus, const Amg::Vector3D& lpos000, 
                       ICscSegmentFinder::Segments& segs,
                       double localPos, double localSlope) const;
  
  // Find 3 hit segments in a chamber.
  void find_2dseg3hit(bool measphi, int station,  int eta, int phi,
                      const ICscSegmentFinder::ChamberTrkClusters& clus, const Amg::Vector3D& lpos000, 
                      ICscSegmentFinder::Segments& segs, ICscSegmentFinder::Segments& segs4hit,
                      double localPos, double localSlope) const;

    // Find 2 hit segments in a chamber.
  void find_2dseg2hit(bool measphi, int station,  int eta, int phi, int layStat,
                      const ICscSegmentFinder::ChamberTrkClusters& clus, const Amg::Vector3D& lpos000,
                      ICscSegmentFinder::Segments& segs, 
                      double localPos, double localSlope) const;

  /** Adds 3-hit segments to 4-hit segments **/
  void add_2dsegments(ICscSegmentFinder::Segments &segs4, ICscSegmentFinder::Segments &segs3) const;

  //Stores 2-hit segments (does overlap removal and then saves the remaining segments)
  void add_2dseg2hits(ICscSegmentFinder::Segments &segs, ICscSegmentFinder::Segments &segs2, int layStat) const;
  
  // Check to see if 3-hit segment is unique.
  bool unique_hits(ICscSegmentFinder::TrkClusters& fitclus, ICscSegmentFinder::Segments& segs) const;

  void get2dSegments( Identifier eta_id, Identifier phi_id,
                      ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                      ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                      ICscSegmentFinder::Segments& etasegs, ICscSegmentFinder::Segments& phisegs,
                      const Amg::Vector3D& lpos000, int etaStat=0, int phiStat=0 ) const;

  Muon::MuonSegment* make_4dMuonSegment(const Muon::MuonSegment& rsg, const Muon::MuonSegment& psg, bool use2LaySegsEta, bool use2LaySegsPhi) const;

  /***** Find outlier cluster *****/
  /* It finds the biggest chisquare contributing cluster:
     returns 'n' th cluster ( n = 0-3 )
     Finding is proceeded only for N_unspoiled_clusters >=3
     otherwise it returns -1 meaning N_unspoiled_clusters <= 2 */
  int find_outlier_cluster(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, double& returned_chsq) const;

  /***** Fit a 2D segment. *****/
  /* clus = input list of clusters
     s0 = position
     s1 = slope
     d0, d1 and d01 = errors and correlation
     chsq = chi-square  **/
  //  void fit_segment(const ICscSegmentFinder::TrkClusters& clus, double& s0, double& s1,
  //                   double& d0, double& d1, double& d01, double& chsq) const; old by Feb 15, 2010
  // time/dtime is added in Feb 17, 2011
  void fit_segment(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, double& s0, double& s1,
                   double& d0, double& d1, double& d01, double& chsq,
                   double& time, double& dtime, double& zshift,
                   int outlierLayer=-1) const;


  // fit_segment is split into several parts of detailed calculation...
  // time/dtime is added in Feb 17, 2011
  void fit_detailCalcPart1(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, double& s0, double& s1,
                           double& d0, double& d1, double& d01, double& chsq, bool& measphi,
                           double& time, double& dtime, double& zshift,
                           bool IsSlopeGive, int outlierHitLayer) const;
  void fit_detailCalcPart2(double q0, double q1, double q2, double q01, double q11, double q02,
                           double& s0, double& s1, double& d0, double& d1, double& d01, double& chsq) const;
  
  double getDefaultError ( Identifier id, bool measphi, const Muon::CscPrepData *prd ) const;


  void getRios(const ICscSegmentFinder::Segment& seg, ICscSegmentFinder::MbaseList* prios, bool measphi) const;
  // if hit is in outlier, error is estimated in width/sqrt(12)
  // For future, error estimate should be from CscClusterUtilTool.

  
  // ECC - Returns likelihood for matching xy segments 
  //     - currently uses charge ratio, but should be expanded for timing.
  double matchLikelihood(const Muon::MuonSegment& rsg, const Muon::MuonSegment& psg) const;

  // pdf distribution function for signal events.
  double pdf_sig(const double x) const;

  // pdf distribution function for background events.
  double pdf_bkg(const double x) const;

  // Likelihood function = psig/(psig +pbkg)
  double qratio_like(const double pdf_sig, const double pdf_bkg) const;

  bool isGood(const uint32_t stripHashId) const;
  int stripStatusBit(const uint32_t stripHashId) const;

};

#endif
