/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscSegmentUtilTool_H
#define CscSegmentUtilTool_H

// Utilities for building segments.
#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h" // separately...
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "CscSegmentMakers/ICscSegmentUtilTool.h"
#include "CscClusterization/ICscClusterUtilTool.h"
#include "MuonCondInterface/CscICoolStrSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
//#include "CscClusterization/CalibCscStripFitter.h"

namespace MuonGM {
  class MuonDetectorManager;
}
namespace Trk {
  class RIO_OnTrack;
  class PlaneSurface;
}
namespace Muon {
  class MuonIdHelperTool;
  class MuonSegment;
  class ICscClusterOnTrackCreator;
  class CscPrepData;
}
class ICscSegmentFinder;

class CscSegmentUtilTool : virtual public ICscSegmentUtilTool, public AthAlgTool {

public:

  // Constructor.
  CscSegmentUtilTool(const std::string& type, const std::string& name,
                 const IInterface* parent );

  // Destructor.
  virtual ~CscSegmentUtilTool();
  
  // AlgTool InterfaceID    
  //  static const InterfaceID& interfaceID( ) ;
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  

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
  

private:  // data
  const MuonGM::MuonDetectorManager* m_gm;
  const CscIdHelper* m_phelper;
  double m_max_chisquare_tight;
  double m_max_chisquare_loose;
  double m_max_chisquare;
  double m_max_slope_r;
  double m_max_slope_phi;
  double m_min_xylike; // Minimum value used for xy matching of 4D segments.
  float  m_cluster_error_scaler;
  bool   m_x5data;
  unsigned int m_max_seg_per_chamber;
  int m_max_3hitseg_sharehit;
  double m_fitsegment_tantheta_tolerance;
  bool m_zshift;
  bool m_IPconstraint; 
  double m_IPerror; 
  bool m_allEtaPhiMatches; 
  bool m_TightenChi2; 
  bool m_remove4Overlap; 
  bool m_remove3Overlap; 
  int  m_nunspoil;
 
  ToolHandle<Muon::ICscClusterOnTrackCreator> m_rotCreator;
  ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
  
  ServiceHandle<MuonCalib::CscICoolStrSvc> m_cscCoolStrSvc;

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};

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
  double pdf_sig(double x) const;

  // pdf distribution function for background events.
  double pdf_bkg(double x) const;

  // Likelihood function = psig/(psig +pbkg)
  double qratio_like(double pdf_sig, double pdf_bkg) const;

  bool isGood(uint32_t stripHashId) const;
  int stripStatusBit(uint32_t stripHashId) const;

};

#endif
