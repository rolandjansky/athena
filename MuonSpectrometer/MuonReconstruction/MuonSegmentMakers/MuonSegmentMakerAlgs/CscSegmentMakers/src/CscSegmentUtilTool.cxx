/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscSegmentUtilTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"

#include "CscSegmentMakers/ICscSegmentFinder.h"
#include "MuonRecToolInterfaces/ICscClusterOnTrackCreator.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscStripFitter.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "TMath.h"
#include <cmath>

#include <iostream>

using std::string;
using MuonGM::MuonDetectorManager;
using MuonGM::CscReadoutElement;
using Muon::MuonSegment;
using Muon::CscPrepData;
using Muon::CscClusterOnTrack;
using Muon::MuonSegmentCombination;
using Trk::RIO_OnTrack;
using Trk::PlaneSurface;

namespace {
  std::string station_name(int station) {
    if ( station == 1 ) return "CSS";
    if ( station == 2 ) return "CSL";
    return "UNKNOWN_STATION";
  }
  
  std::string measphi_name(bool measphi) {
    if ( measphi ) return "phi";
    return "eta";
  }

  double alignConst(bool measphi, int wlay) {
    if (measphi) return 0.;
    const double aConst[4] = { 0, -0.2289, -0.620181, -0.6534445 };
    return aConst[wlay-1];
  }

  bool enoughHitLayers (ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                        ICscSegmentFinder::ChamberTrkClusters& phi_clus) {

    int nHitLayer_eta =0;
    int nHitLayer_phi =0;
    for (int i=0; i<4; ++i) {
      //    ATH_MSG_DEBUG ( "No of clusters in layer " << i << " " << eta_clus[i].size() << " " << phi_clus[i].size() );
      if (eta_clus[i].size() >0) ++nHitLayer_eta;
      if (phi_clus[i].size() >0) ++nHitLayer_phi;
    }
    
    if (nHitLayer_eta <2 && nHitLayer_phi <2)
      return false;
    else
      return true;
  }

  bool IsUnspoiled ( Muon::CscClusterStatus status ) {
    if (status == Muon::CscStatusUnspoiled || status == Muon::CscStatusSplitUnspoiled )
      return true;
    
    return false;
  }
  
}

//******************************************************************************
// Constructor.
CscSegmentUtilTool::CscSegmentUtilTool
(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent), m_gm(0), m_phelper(0), 
    m_rotCreator("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator"),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_cscCoolStrSvc("MuonCalib::CscCoolStrSvc", name)
{
  declareInterface<ICscSegmentUtilTool>(this);
  declareProperty("max_chisquare_tight", m_max_chisquare_tight = 16.); // 16 for outlier removal...
  declareProperty("max_chisquare_loose", m_max_chisquare_loose = 2000.); // 2000 for outlier removal...
  declareProperty("max_chisquare",       m_max_chisquare = 25.);
  declareProperty("max_slope_r",         m_max_slope_r = 0.20);
  declareProperty("max_slope_phi",       m_max_slope_phi = 0.20);
  declareProperty("max_seg_per_chamber", m_max_seg_per_chamber = 50);
  declareProperty("min_xylike",          m_min_xylike = -1);
  declareProperty("X5data",              m_x5data = false);
  declareProperty("max_3hitseg_sharedhit", m_max_3hitseg_sharehit =0);

  declareProperty("tantheta_update_tolerance", m_fitsegment_tantheta_tolerance = 0.0001);
  declareProperty("cluster_error_scaler", m_cluster_error_scaler=1.0);

  declareProperty("zshift", m_zshift = true);
  declareProperty("IPconstraint", m_IPconstraint = true);
  declareProperty("IPerror", m_IPerror = 250.);
  declareProperty("allEtaPhiMatches", m_allEtaPhiMatches = true);  
  declareProperty("rot_creator", m_rotCreator);
  declareProperty("TightenChi2", m_TightenChi2 = true);
  declareProperty("Remove4Overlap", m_remove4Overlap = true);
  declareProperty("Remove3Overlap", m_remove3Overlap = true);
  declareProperty("UnspoiledHits", m_nunspoil = -1);

}

//******************************************************************************

/** destructor */
CscSegmentUtilTool::~CscSegmentUtilTool()
{}

/*********************************/
StatusCode CscSegmentUtilTool::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_MSG_DEBUG ( "  Max chi-square: " << m_max_chisquare );
  ATH_MSG_DEBUG ( "      chi-square tight: " << m_max_chisquare_tight );
  ATH_MSG_DEBUG ( "      chi-square loose: " << m_max_chisquare_loose );
  ATH_MSG_DEBUG ( "  Max r:phi slope: " << m_max_slope_r << " : "
                  << m_max_slope_phi );
  ATH_MSG_DEBUG ( "  Max segments/chamber: " << m_max_seg_per_chamber );
  ATH_MSG_DEBUG ( "  ROT tan(theta) tolerance: "
                  << m_fitsegment_tantheta_tolerance );
  ATH_MSG_DEBUG ( " cluster_error_scaler " << m_cluster_error_scaler);
  //ATH_MSG_DEBUG ( "  Precision cluster fitter is " << m_rotCreator->GetICscClusterFitter().typeAndName() );
  ATH_MSG_DEBUG ( "  ROT creator: " << m_rotCreator.typeAndName() );


  if(m_TightenChi2) m_IPerror = 2.;
  if(m_TightenChi2) ATH_MSG_DEBUG ( " Chi2 cuts are tightened and m_IPerror is: " <<   m_IPerror);

  if (m_x5data)
    ATH_MSG_DEBUG (" Things for X5Data analysis is applied such as alignment ");
  
  if (detStore()->retrieve(m_gm).isFailure() ) {
    ATH_MSG_WARNING ( " Cannot retrieve MuonReadoutGeometry " );
    return StatusCode::SUCCESS;
  }

  m_phelper = m_gm->cscIdHelper();

  if ( m_rotCreator.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Could not get " << m_rotCreator ); 
    return StatusCode::FAILURE;
  }else{
    ATH_MSG_DEBUG ( "Got " << m_rotCreator ); 
  }

  if ( m_idHelper.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Could not get " << m_idHelper ); 
    return StatusCode::FAILURE;
  }

  if ( m_cscCoolStrSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve pointer to the CSC COLL Conditions Service" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_eventInfo.initialize());

  return StatusCode::SUCCESS;
}


///////////////////
//Note: this code was required for old MuGirl but is not used by the current reconstruction
//Leaving it in place for backwards compatibility
std::unique_ptr<std::vector<std::unique_ptr<MuonSegment> > > CscSegmentUtilTool::
getMuonSegments(Identifier eta_id, Identifier phi_id,
                ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                const Amg::Vector3D& lpos000 ) const {

  if (! enoughHitLayers(eta_clus, phi_clus) ) {
    ATH_MSG_DEBUG (" Could not find at least two individual layer hits! ");
    std::unique_ptr<std::vector< std::unique_ptr<MuonSegment> > > segments;
    return segments;
  }
  
  for (int i=0; i<4; ++i) 
    ATH_MSG_DEBUG ( "getMuonSegments2: No of clusters in layer " << i << " " << eta_clus[i].size() << " " << phi_clus[i].size() );

  ATH_MSG_DEBUG ("getMuonSegments called get2dMuonSegmentCombination");
  std::unique_ptr<MuonSegmentCombination> Muon2dSegComb(get2dMuonSegmentCombination(eta_id, phi_id, eta_clus, phi_clus, lpos000));
  
  ATH_MSG_DEBUG ("getMuonSegments called get4dMuonSegmentCombination");
  std::unique_ptr<MuonSegmentCombination> Muon4dSegComb(get4dMuonSegmentCombination(Muon2dSegComb.get()));

  //  delete 4dMuonSegComb; WP careful...

  // Add the case for only 2d segments following the idea below ??
  /*
  if (!rsg && psg) {
    segments->push_back(psg);
    return segments;
  } else if (rsg && !psg) {
    segments->push_back(rsg);
    return segments;
  }
  */
  std::unique_ptr<std::vector<std::unique_ptr<MuonSegment> > > segments_clone (new std::vector<std::unique_ptr<MuonSegment> >);

  if (Muon4dSegComb) {
    std::vector<std::unique_ptr<MuonSegment> >* segments = Muon4dSegComb->stationSegments(0);
  
    if (!segments->empty()) {
      for (unsigned int i=0; i<segments->size(); ++i){
        segments_clone->push_back(std::move(segments->at(i)));
      }
    }

  }
  
  return segments_clone;
}


 
/////////////////////////////////////////////////////
MuonSegmentCombination* CscSegmentUtilTool::
get2dMuonSegmentCombination(  Identifier eta_id, Identifier phi_id,
                              ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                              ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                              const Amg::Vector3D& lpos000, int etaStat, int phiStat ) const {

  int nGoodEta=0,nGoodPhi=0;
  for (int i=0; i<4; ++i){
    ATH_MSG_DEBUG ( "get2dMuonSegmentCombination2: No of clusters in layer " << i << " " << eta_clus[i].size() << " " << phi_clus[i].size() );
    if((etaStat%(int)pow(10,i+1))/(int)pow(10,i)==0) nGoodEta++;
    if((phiStat%(int)pow(10,i+1))/(int)pow(10,i)==0) nGoodPhi++;
  }

  if(nGoodEta<2 && nGoodPhi<2){
    ATH_MSG_DEBUG (" Not enough hits in either eta or phi to form a segment");
    MuonSegmentCombination* pcol = 0;
    return pcol;
  }

  MuonSegmentCombination* pcol = new MuonSegmentCombination;
  pcol->setNGoodCscLayers(nGoodEta,nGoodPhi);
  // Find 2D segments.
  ICscSegmentFinder::Segments eta_segs;
  ICscSegmentFinder::Segments phi_segs;

  // get2dSegments does : find_2dsegments -> find_2dseg3hit -> add_2dsegments
  get2dSegments(eta_id, phi_id, eta_clus, phi_clus, eta_segs, phi_segs, lpos000, etaStat, phiStat);
  std::unique_ptr<MuonSegmentCombination::SegmentVec> psegs (new MuonSegmentCombination::SegmentVec);
  for ( ICscSegmentFinder::Segments::const_iterator iseg=eta_segs.begin(); iseg!=eta_segs.end(); ++iseg ) {
    std::unique_ptr<MuonSegment> pseg(build_segment(*iseg, false, eta_id, nGoodEta==2)); // build_segment does getRios
    if (pseg) {
      psegs->push_back(std::move(pseg));
      ATH_MSG_DEBUG( " =============================> get2dMuonSegmentCombination::  MuonSegment time (eta) from build_segment is " << pseg->time() );
      //      pseg->dump(cout);
    }
  }
  pcol->addSegments(std::move(psegs));
  ATH_MSG_DEBUG("added "<<psegs->size()<<" eta segments");
   
  // Insert phi-segments.
  std::unique_ptr<MuonSegmentCombination::SegmentVec> phisegs(new MuonSegmentCombination::SegmentVec);
  for ( ICscSegmentFinder::Segments::const_iterator iseg=phi_segs.begin(); iseg!=phi_segs.end(); ++iseg ) {
    std::unique_ptr<MuonSegment> pseg(build_segment(*iseg, true, phi_id, nGoodPhi==2));
    if (pseg) {
      phisegs->push_back(std::move(pseg));
      ATH_MSG_DEBUG( " get2dMuonSegmentCombination::  MuonSegment time (phi) from build_segment is " << pseg->time() );
    }
  }
  pcol->addSegments(std::move(phisegs));
  ATH_MSG_DEBUG("added "<<phisegs->size()<<" phi segments");

  // Add  to SG container.
  ATH_MSG_DEBUG ( "Added " << eta_segs.size() << " r-segments and "
                  << phi_segs.size() << " phi-segments." );

  return pcol;
}

/**********************************************/
// Fit a segment using a list of clusters.
// Filling is least squares with the usual 1/d**2 weighting.
// local z = -38.51  -12.82  12.87  38.56
void CscSegmentUtilTool::
//fit_segment(const ICscSegmentFinder::TrkClusters& clus, double& s0, double& s1,
//            double& d0, double& d1, double& d01, double& chsq) const {  old one....
fit_segment(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, double& s0, double& s1,
            double& d0, double& d1, double& d01, double& chsq, double& time, double& dtime,
            double& zshift, int outlierHitLayer) const {

  ATH_MSG_DEBUG ( " fit_segment called " );


  bool measphi = false;
  fit_detailCalcPart1(clus, lpos000, s0, s1, d0, d1, d01, chsq, measphi, time,  dtime, zshift,
                      false, outlierHitLayer); // IsSlopeGiven should be false

  if (measphi) return; //No need to do the second try
  fit_detailCalcPart1(clus, lpos000, s0, s1, d0, d1, d01, chsq, measphi, time, dtime, zshift,
                      true,  outlierHitLayer); // IsSlopeGiven should be true

  return;
}

/////
// if outlierid == iclu... error should be blown out in case of precision measurements..
// if IsSlopeGiven true, then.... error is recalculated.....

void CscSegmentUtilTool::
fit_detailCalcPart1(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, double& s0, double& s1,
                    double& d0, double& d1, double& d01, double& chsq, bool& measphi,
                    double& time, double& dtime, double& zshift,
                    bool IsSlopeGiven, int outlierHitLayer) const {

//  if (IsSlopeGiven)
// measure zshift 

  double q0 = 0.0;   double q1 = 0.0;  
  for (ICscSegmentFinder::TrkClusters::const_iterator iclu=clus.begin(); iclu!=clus.end(); ++iclu ) {
    const ICscSegmentFinder::Cluster& cl = *iclu;
    const CscClusterOnTrack* clu = cl.cl;
    const CscPrepData* prd = clu->prepRawData();
    Identifier id = clu->identify();

    measphi = m_phelper->measuresPhi(id);
    // Cluster position.
    double y = cl.locY();
    if (m_x5data) {
      y = y - alignConst(measphi, m_phelper->wireLayer(id));
    }
    double x = cl.locX();

    bool isunspoiled = IsUnspoiled (prd->status());

    // Error in cluster position.
    // Slope is given and Unspoiled Status..... then error should be recalculated depending on slope...
    // Outlier treatment is only working in case of unspoiled...
    double d = Amg::error(clu->localCovariance(),Trk::locX);
    if (isunspoiled) {
      if ( IsSlopeGiven && outlierHitLayer != ( iclu - clus.begin() ) )
        d = m_rotCreator->GetICscClusterFitter()->getCorrectedError(prd, s1);
//      if ( outlierHitLayer == ( iclu - clus.begin() ) )
//        d = getDefaultError(id, measphi, prd);
    }
       

    d *= m_cluster_error_scaler; // This is for error scaler for cosmic!!!
    double w = 1.0/(d*d);
    q0 += w;
    q1 += w*x;
  }  
  zshift = 0.0 ;
  if(q0>0.) zshift = q1/q0; 
  if(!m_zshift) zshift = 0.0;
 
  ATH_MSG_VERBOSE ( " fit_detailCalcPart1 zshift " << zshift);
  
  q0 = 0.0;   
  q1 = 0.0;   
  double q2 = 0.0;
  double q01 = 0.0;  double q11 = 0.0;  double q02 = 0.0;
 
  
 
  if(m_IPconstraint) {
    double x = lpos000.z();
    double y = measphi ? lpos000.x() : lpos000.y();

    ATH_MSG_DEBUG ( " constraint x " <<  x << "  y " << y << " error " << m_IPerror );

    double d = m_IPerror; 
    double w = 1.0/(d*d);
    q0 += w;
    q1 += w*(x-zshift);
    q2 += w*(x-zshift)*(x-zshift);
    q01 += w*y;
    q11 += w*(x-zshift)*y;
    q02 += w*y*y;
  }

  int cntSuccessHit =0;
  double sumHitTimes =0.;
  double sumHitTimeSquares =0.;

  int cntLateHit =0;
  int cntEarlyHit =0;
  float latestEarlyTime = -9999;
  float earliestLateTime = 9999;
  for (ICscSegmentFinder::TrkClusters::const_iterator iclu=clus.begin(); iclu!=clus.end(); ++iclu ) {
    const ICscSegmentFinder::Cluster& cl = *iclu;
    const CscClusterOnTrack* clu = cl.cl;
    const CscPrepData* prd = clu->prepRawData();
    Identifier id = clu->identify();

    measphi = m_phelper->measuresPhi(id);
    // Cluster position.
    double y = cl.locY();
    if (m_x5data) {
      y = y - alignConst(measphi, m_phelper->wireLayer(id));
    }
    double x = cl.locX();

    bool isunspoiled = IsUnspoiled (prd->status());
    
    // Error in cluster position.
    // Slope is given and Unspoiled Status..... then error should be recalculated depending on slope...
    // Outlier treatment is only working in case of unspoiled...
    double d = Amg::error(clu->localCovariance(),Trk::locX);
    if (isunspoiled) {
      if ( IsSlopeGiven )
        d = m_rotCreator->GetICscClusterFitter()->getCorrectedError(prd, s1);
      if ( outlierHitLayer == ( iclu - clus.begin() ) ) 
        d = getDefaultError(id, measphi, prd);
    }
    d *= m_cluster_error_scaler; // This is for error scaler for cosmic!!!
    ATH_MSG_VERBOSE ( " +++fit_segment() x/y/d = " << x << " " << y << " " << d );
    
    double w = 1.0/(d*d);
    q0 += w;
    q1 += w*(x-zshift);
    q2 += w*(x-zshift)*(x-zshift);
    q01 += w*y;
    q11 += w*(x-zshift)*y;
    q02 += w*y*y;

    if (prd->timeStatus() == Muon::CscTimeSuccess) {
      ++cntSuccessHit;
      sumHitTimes += prd->time(); 
      sumHitTimeSquares += std::pow(prd->time(),2);
    }

    if (prd->timeStatus() == Muon::CscTimeEarly) {
      ++cntEarlyHit;
      if (latestEarlyTime < prd->time())
        latestEarlyTime = prd->time();
    }

    if (prd->timeStatus() == Muon::CscTimeLate) {
      ++cntLateHit;
      if (earliestLateTime > prd->time())
        earliestLateTime = prd->time();
    }
  }

  ATH_MSG_DEBUG (" Time Calculation!!! " << cntSuccessHit << " " << cntEarlyHit << " " << cntLateHit << " "
                 << sumHitTimes << " " << latestEarlyTime << " " << earliestLateTime );
  
  if (cntSuccessHit>0) {
    time       = sumHitTimes/float(cntSuccessHit);
    float timeSquare = sumHitTimeSquares/float(cntSuccessHit);
    dtime = timeSquare - time*time;
    if (dtime<0.0) dtime = 0.0;
    else dtime = sqrt(dtime);
  } else {
    if (cntEarlyHit >0 && cntLateHit >0) {
      time = 99999;
      dtime = 99999;
      ATH_MSG_DEBUG ("Segment has nonzero earlyHits and nonzero lateHits. This should be backgrounds!!");
    } else if (cntEarlyHit >0) {
      time  = latestEarlyTime;
      dtime = fabs(latestEarlyTime);  
    } else if (cntLateHit >0) {
      time  = earliestLateTime;
      dtime = earliestLateTime;  
    } else {
      time = 99999;
      dtime = 99999;
      ATH_MSG_DEBUG ("Every Hit is CscTimeUnavailable or CscTimeStatusUndefined!!! Should be investigated");
    }
  }

  ATH_MSG_DEBUG(" calc result " << time << " " << dtime );
  
  
  // IsSlopeGiven (true) means 2nd try and somehow denominator is zero
  // then return only available calc..
  if ( IsSlopeGiven && q2*q0-q1*q1 ==0.0 ) return;

  fit_detailCalcPart2(q0, q1, q2, q01, q11, q02,   s0, s1, d0, d1, d01, chsq);
  return;
}
 
//******************************************************************************
// 
int CscSegmentUtilTool::
find_outlier_cluster(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, double& returned_chsq) const {
  int nunspoil = 0;
 
  double chsq_reference =10000;
  int remclu_ind =-1;
   ATH_MSG_VERBOSE ( " find_outlier_cluster  clus size " <<  clus.size() );

  // check all four cases of outlier ... found the best chsq!!!
  for (unsigned int ire=0; ire<clus.size(); ire++) {
    ICscSegmentFinder::TrkClusters fitclus;
    bool isunspoiled = IsUnspoiled (clus[ire].cl->status() );
    if (isunspoiled) ++nunspoil;

    for (unsigned int itr=0; itr<clus.size(); itr++) 
      if (ire !=itr) fitclus.push_back(clus[itr]);

    ATH_MSG_VERBOSE ( " find_outlier_cluster drop ire " << ire << "  fitclus size " <<  fitclus.size() );
    
    double s0, s1, d0, d1, d01, chsq, time, dtime, zshift;
    fit_segment(fitclus, lpos000, s0, s1, d0, d1, d01, chsq, time, dtime, zshift);
    if (chsq < chsq_reference && isunspoiled) { // require outlier should be precision measurement
      chsq_reference =chsq;
      remclu_ind =ire;
    }

    if (remclu_ind>=0)
      ATH_MSG_VERBOSE ( "  new chsq  " <<  ire << "  " << chsq << "  @" << remclu_ind );
  }

  // If nunspoil clusters are only 2 or less, we don't want to treat this segment outlier removal process!!
  // So, set remclu_ind = -1 meaning no outlier is found...
  if (nunspoil < m_nunspoil ) {
    ATH_MSG_VERBOSE (" Number of unspoiled cluster is " << nunspoil << " which is not enough!! At least 3 unspoiled hits required!! ");
    remclu_ind =-1;
  }


  returned_chsq = 99999;
  if (remclu_ind>=0) {
    ICscSegmentFinder::TrkClusters fitclus; // Muon::CscClusterOnTrack!!
    // loading clusters into fitclus with outlier error blown up ....
    for (unsigned int ire=0; ire<clus.size(); ire++) 
      fitclus.push_back(clus[ire]);

    ATH_MSG_VERBOSE ( " final fit outlier_cluster drop cluster index " << remclu_ind << " fitclus.size " <<  fitclus.size() );
    
    double s0, s1, d0, d1, d01, chsq, time, dtime, zshift;
    fit_segment(fitclus, lpos000, s0, s1, d0, d1, d01, chsq, time, dtime, zshift, remclu_ind);
    returned_chsq = chsq;
  }
  
  return remclu_ind;
}


//******************************************************************************

// Fit a segment residual with a list of clusters.
// Segment is fit excluding cluster irclu and then the residual is calculated
// as the difference between the cluster position and that predicted by
// the segment. The error in the residual includes those of the segment and the
// excluded cluster.

void CscSegmentUtilTool::
fit_residual(const ICscSegmentFinder::TrkClusters& clus, const Amg::Vector3D& lpos000, unsigned int irclu,
             double& res, double& dres) const {
  ATH_MSG_DEBUG ( "CscSegmentUtilTool::fit_residual called " );
  //  const CscIdHelper* phelper = m_gm->cscIdHelper();

  ICscSegmentFinder::TrkClusters fitclus;
  for ( unsigned int iclu=0; iclu<clus.size(); ++iclu ) {
    if ( iclu != irclu ) fitclus.push_back(clus[iclu]);
  }
  // Fit cluster.
  double s0, s1, d0, d1, d01, chsq, time, dtime, zshift;
  fit_segment(fitclus, lpos000, s0, s1, d0, d1, d01, chsq, time, dtime, zshift);

  // Extract excluded cluster paramters.
  const CscClusterOnTrack *cot = clus[irclu].cl;

  Trk::ParamDefs ierr = Trk::loc1;
  double y = clus[irclu].locY();
  if (m_x5data) {
    const Identifier& id = cot->identify();
    y = y - alignConst(m_phelper->measuresPhi(id), m_phelper->wireLayer(id));
  }
  double x = clus[irclu].locX();
  // Error in cluster position.
  double d = Amg::error(cot->localCovariance(),ierr)*m_cluster_error_scaler;

  //  This is for CscClusterCollection
  //    const Amg::Vector3D lpos = pro->localPos(clu.position());
  //  double y = lpos.y();
  //  double x = lpos.x();
  //  double d = clu.sigma();

  if (d0 <0 || d1 <0) {
    res =-99.;
    dres=-9.;
  } else {
    // Calculate predicted position and error.
    double seg_y = s0 + s1*x;
    double seg_dsquare = d0*d0 + 2.0*x*d01 + d1*d1*x*x;
    // Calculate residual and error.
    res = y - seg_y;
    dres = d*d + seg_dsquare;

    ATH_MSG_VERBOSE ( " fit_residual d0:d01:d1 :: seg_dsquare:dres "
                      << d0 << "  " << d01 << "  " << d1 << " :: "
                      << seg_dsquare << " " << dres );
    
    if (dres < 0)
      dres = -1.0*sqrt(-1.0*dres);
    else if (dres >= 0) 
      dres = sqrt(dres);
    else // in case of nan
      dres = -9.;
  }
}

//******************************************************************************

// Fit a segment using a list of RIO's.
// Filling is least squares with the usual 1/d**2 weighting.

void CscSegmentUtilTool::
fit_rio_segment(const Trk::PlaneSurface& ssrf, bool /*dump*/,
                const ICscSegmentFinder::RioList& rios, double& s0, double& s1,
                double& d0, double& d1, double& d01, double& chsq, double& zshift) const {

  ATH_MSG_DEBUG ( "CscSegmentUtilTool::fit_rio_segment called: " );
  //  const CscIdHelper* phelper = m_gm->cscIdHelper();

// measure zshift 

  double q0 = 0.0;
  double q1 = 0.0;
  for ( ICscSegmentFinder::RioList::const_iterator irio=rios.begin();
        irio!=rios.end(); ++irio ) {
    const RIO_OnTrack& rio = **irio;
    // Fetch the measurement.
    Trk::ParamDefs iloc = Trk::loc1;
    const Trk::LocalParameters& msmt = rio.localParameters();
    double y = msmt[iloc];
    if (m_x5data) {
      y = y - alignConst(m_phelper->measuresPhi(rio.identify()), m_phelper->wireLayer(rio.identify()));
    }
    // Fetch the measurement error.
    const Amg::MatrixX& cov = rio.localCovariance();
    int dim = cov.rows();
    Trk::ParamDefs ierr = dim==1 ? Trk::loc1 : iloc;
    double d = Amg::error(cov,ierr);
    // Fetch the position of the measurement coordinate.
    const Amg::Vector3D& gpos = rio.globalPosition();
    Amg::Vector3D lpos = ssrf.transform().inverse()*gpos;
    double x = lpos.z();
    ATH_MSG_VERBOSE ( "   RIO global pos: "
                      << gpos.x() << " " << gpos.y() << " " << gpos.z() );
    ATH_MSG_VERBOSE ( "    RIO local pos: "
                      << lpos.x() << " " << lpos.y() << " " << lpos.z() );
    ATH_MSG_DEBUG ( "  RIO: " << x << " " << y << " " << d );

    // Update least-square sums.
    double w = 1.0/(d*d);
    q0 += w;
    q1 += w*x;
  }

  zshift = 0.0;
  if(q0>0) zshift = q1/q0; 
  if(!m_zshift) zshift = 0.0;
  ATH_MSG_VERBOSE ( " fit_rio_segment: zshift " << zshift);
  // Initialize least-square sums.

  q0 = 0.;
  q1 = 0.;
  double q2 = 0.0;
  double q01 = 0.0;
  double q11 = 0.0;
  double q02 = 0.0;
  for ( ICscSegmentFinder::RioList::const_iterator irio=rios.begin();
        irio!=rios.end(); ++irio ) {
    const RIO_OnTrack& rio = **irio;
    // Fetch the measurement.
    Trk::ParamDefs iloc = Trk::loc1;
    const Trk::LocalParameters& msmt = rio.localParameters();
    double y = msmt[iloc];
    if (m_x5data) {
      y = y - alignConst(m_phelper->measuresPhi(rio.identify()), m_phelper->wireLayer(rio.identify()));
    }
    // Fetch the measurement error.
    // Fetch the measurement error.
    const Amg::MatrixX& cov = rio.localCovariance();
    int dim = cov.rows();
    Trk::ParamDefs ierr = dim==1 ? Trk::loc1 : iloc;
    double d = Amg::error(cov,ierr);
    // Fetch the position of the measurement coordinate.
    const Amg::Vector3D& gpos = rio.globalPosition();
    Amg::Vector3D lpos = ssrf.transform().inverse()*gpos;
    double x = lpos.z();
    ATH_MSG_VERBOSE ( "   RIO global pos: "
                      << gpos.x() << " " << gpos.y() << " " << gpos.z() );
    ATH_MSG_VERBOSE ( "    RIO local pos: "
                      << lpos.x() << " " << lpos.y() << " " << lpos.z() );
    ATH_MSG_DEBUG ( "  RIO: " << x << " " << y << " " << d );

    // Update least-square sums.
    double w = 1.0/(d*d);
    q0 += w;
    q1 += w*(x-zshift);
    q2 += w*(x-zshift)*(x-zshift);
    q01 += w*y;
    q11 += w*(x-zshift)*y;
    q02 += w*y*y;
  }
  
  // detailed calc part2
  fit_detailCalcPart2(q0, q1, q2, q01, q11, q02,   s0, s1, d0, d1, d01, chsq);
  return;
}

//******************************************************************************

// Fit a RIO segment residual with a list of clusters.
// Segment is fit excluding RIO irio and then the residual is calculated
// as the difference between the cluster position and that predicted by
// the segment. The error in the residual includes those of the segment and the
// excluded cluster.

void CscSegmentUtilTool::
fit_rio_residual(const Trk::PlaneSurface& ssrf, bool dump,
                 const ICscSegmentFinder::RioList& rios, unsigned int irrio,
                 double& res, double& dres,
                 double& rs, double& drs
                 ) const {

  ATH_MSG_DEBUG ( "CscSegmentUtilTool::fit_rio_segment called " );
  //  const CscIdHelper* phelper = m_gm->cscIdHelper();
  
  ICscSegmentFinder::RioList fitrios;
  ICscSegmentFinder::RioList fitrios3pt;
  for ( unsigned int irio=0; irio<rios.size(); ++irio ) {
    if ( irio != irrio ) fitrios.push_back(rios[irio]);
    // For three point method....  0 x 2; 1 x 3;
    if ( (irrio==1) &&
         ( irio == 0 || irio ==2 ) ) fitrios3pt.push_back(rios[irio]);
    if ( (irrio==2) &&
         ( irio == 1 || irio ==3 ) ) fitrios3pt.push_back(rios[irio]);
  }
  // Fit cluster.
  double s0, s1, d0, d1, d01, chsq, zshift;
  fit_rio_segment(ssrf, dump, fitrios, s0, s1, d0, d1, d01, chsq, zshift);

  // Extract excluded cluster paramters (see fit_rio_segment).
  const RIO_OnTrack& rio = *rios[irrio];
  Trk::ParamDefs iloc = Trk::loc1;
  const Trk::LocalParameters& msmt = rio.localParameters();
  double y = msmt[iloc];
  if (m_x5data) {
    y = y - alignConst(m_phelper->measuresPhi(rio.identify()), m_phelper->wireLayer(rio.identify()));
  }
  const Amg::MatrixX& cov = rio.localCovariance();
  int dim = cov.rows();
  Trk::ParamDefs ierr = dim==1 ? Trk::loc1 : iloc;
  double d = Amg::error(cov,ierr);
  const Amg::Vector3D& gpos = rio.globalPosition();
  Amg::Vector3D lpos = ssrf.transform().inverse()*gpos;
  double x = lpos.z();
  ATH_MSG_VERBOSE ( "  excluded RIO: " << x << " " << y << " " << d );
  
  // Calculate predicted position and error.
  double seg_y = s0 + s1*x;
  double seg_dsquare = d0*d0 + 2.0*x*d01 + d1*d1*x*x;
  // Calculate residual and error.
  res = y - seg_y;
  dres = d*d + seg_dsquare;

  ATH_MSG_VERBOSE ( " fit_residual d0:d01:d1 :: seg_dsquare:dres "
                    << d0 << "  " << d01 << "  " << d1 << " :: "
                    << seg_dsquare << " " << dres );

  if (dres < 0)
    dres = -1.0*sqrt(-1.0*dres);
  else if ( dres>=0 )
    dres = sqrt(dres);
  else // in case of nan
    dres = -9.;

  // 3pt method
  // Calculate predicted position and error.
  if (irrio==1 || irrio==2) {
    // For 3 point method
    double s0_3pt, s1_3pt, d0_3pt, d1_3pt, d01_3pt, chsq_3pt, zshift_3pt;
    fit_rio_segment(ssrf, dump, fitrios3pt, s0_3pt, s1_3pt, d0_3pt, d1_3pt, d01_3pt, chsq_3pt, zshift_3pt);

    if (d0_3pt <0 || d1_3pt <0) {
      rs =-99.;
      drs=-9.;
    } else {
      seg_y = s0_3pt + s1_3pt*x;
      seg_dsquare = d0_3pt*d0_3pt + 2.0*x*d01_3pt + d1_3pt*d1_3pt*x*x;
      // Calculate residual and error.
      rs = y - seg_y;
      drs = d*d + seg_dsquare;

      ATH_MSG_VERBOSE ( " fit_rio_residual d0:d01:d1 :: seg_dsquare:dres "
                        << d0_3pt << "  " << d01_3pt << "  " << d1_3pt << " :: "
                        << seg_dsquare << " " << drs );
      
      if (drs < 0)
        drs = -1.0*sqrt(-1.0*drs);
      else if (drs >= 0)
        drs = sqrt(drs);
      else // in case of nan
        drs = -999.; 
    }
  } else {
    rs =-99.;
    drs =-9.;
  }

  ATH_MSG_DEBUG ( "Residual " << irrio << ": " << res << " " << dres );
}


//******************************************************************************

// Extract then numbers of spoiled and unspoiled measurements from a list
// of RIO's.

void CscSegmentUtilTool::
spoiled_count(const ICscSegmentFinder::RioList& rios, double threshold,
              int& nspoil, int& nunspoil) {
  nspoil = 0;
  nunspoil = 0;
  for ( ICscSegmentFinder::RioList::const_iterator irio=rios.begin();
        irio!=rios.end(); ++irio ) {
    const RIO_OnTrack& rio = **irio;
    // Fetch the measurement.
    Trk::ParamDefs iloc = Trk::loc1;
    const Amg::MatrixX& cov = rio.localCovariance();
    int dim = cov.rows();
    Trk::ParamDefs ierr = dim==1 ? Trk::loc1 : iloc;
    double d = Amg::error(cov,ierr);
    if ( d < threshold ) ++nunspoil;
    else ++nspoil;
  }
}


void CscSegmentUtilTool::
spoiled_count(const ICscSegmentFinder::RioList& rios, int& nspoil, int& nunspoil) {
  nspoil = 0;
  nunspoil = 0;
  for ( ICscSegmentFinder::RioList::const_iterator irio=rios.begin();
        irio!=rios.end(); ++irio ) {
    const RIO_OnTrack& rio = **irio;
    const CscPrepData* pclu = dynamic_cast<const CscPrepData*>(rio.prepRawData());
    
    if (pclu) {
      if ( IsUnspoiled (pclu->status()) ) ++nunspoil;
      else  ++nspoil;
    } else {
      ATH_MSG_WARNING (" Could not find CscPrepData from RIO_OnTrack " );
    }
  }
  
}


void CscSegmentUtilTool::
spoiled_count(const ICscSegmentFinder::RioList& rios, int& nspoil, int& nunspoil, int& spoilmap) {
  nspoil = 0;
  nunspoil = 0;
  spoilmap =0;
  //  const CscIdHelper* phelper = m_gm->cscIdHelper();
  
  for ( ICscSegmentFinder::RioList::const_iterator irio=rios.begin();
        irio!=rios.end(); ++irio ) {
    const RIO_OnTrack& rio = **irio;
    const CscPrepData* pclu = dynamic_cast<const CscPrepData*>(rio.prepRawData());
    if (pclu) {
      if ( IsUnspoiled ( pclu->status()) ) ++nunspoil;
      else  {
        int wlay = m_phelper->wireLayer(pclu->identify());
        ++nspoil;
        spoilmap += int(std::pow(2,wlay-1));
      }
    } else {
      ATH_MSG_WARNING (" Could not find CscPrepData from RIO_OnTrack " );
    }
  }
  
}

//******************************************************************************
StatusCode CscSegmentUtilTool::finalize() {
  ATH_MSG_DEBUG ( "Goodbye" );
  return StatusCode::SUCCESS;
}


//******************************************************************************
// Build an ATLAS segment.
// Using the muon convention that x is normal to the plane of measurement:
// My r segment: (z, dz/dx)
// My phi segment: (y, dy/dx)
// ATLAS segment: (u, v, atan(dx/du), atan(dx/dv)) = (y, z, axu, axv)
//   d(axy)/d(dx/dy) = -1/(1+(dx/dy)**2)
// where u is the measured coordinate.
// Use 0+/-1000 for the missing position and pi/2+/-1 for the missing direction.

MuonSegment* CscSegmentUtilTool::
build_segment(const ICscSegmentFinder::Segment& seg, bool measphi, Identifier chid, bool use2Lay) const {
  // chid from any last cluster in given chamber

  ATH_MSG_DEBUG ( "Building csc segment." );

  //if(use2Lay) std::cout<<"using 2-layer segments"<<std::endl;
  //std::cout<<"CscSegmentUtilTool::build_segment in chamber "<<m_idHelper->toString(chid)<<std::endl;

  const double pi = acos(-1.0);
  const double pi2 = 0.5*pi;
  const CscReadoutElement* pro = m_gm->getCscReadoutElement(chid);
  Amg::Transform3D gToLocal = pro->GlobalToAmdbLRSTransform();
  Amg::Vector3D lpos000 = gToLocal*Amg::Vector3D(0.0, 0.0, 0.0);
  Amg::Transform3D lToGlobal = gToLocal.inverse();

  // Surface.
  // Position is the local origin transformed to the global coordinate system.
  // We cannot use chamber position because it is at the first plane, not the
  // origin of the local coordinate system!
  ATH_MSG_VERBOSE ( " build_segment: zshift " << seg.zshift);
  Amg::Vector3D locp(0.0, 0.0, seg.zshift);
  Amg::Vector3D glop  = lToGlobal*locp;
  // Use chamber rotation.
  Amg::Transform3D* pxf = new Amg::Transform3D();
  *pxf = pro->transform(chid).rotation();
  pxf->pretranslate(glop);
  //Amg::Transform3D* pxf = new Amg::Transform3D(pro->transform(chid));
  // Use chamber bounds.
  Trk::SurfaceBounds* pbnd = pro->bounds(chid).clone();
  Trk::TrapezoidBounds* pbnd_trap =
    dynamic_cast<Trk::TrapezoidBounds*>(pbnd);
  Trk::RotatedTrapezoidBounds* pbnd_rtrap =
    dynamic_cast<Trk::RotatedTrapezoidBounds*>(pbnd);
  Trk::PlaneSurface* psrf = 0;

  ATH_MSG_VERBOSE ( "                Surface: " );
  if ( pbnd_trap ) {
    psrf = new Trk::PlaneSurface(pxf, pbnd_trap);
    ATH_MSG_VERBOSE ( "trapezoid" );
  } else if ( pbnd_rtrap ) {
    psrf = new Trk::PlaneSurface(pxf, pbnd_rtrap);
    ATH_MSG_VERBOSE ( "rotated trapezoid" );
  } else {
    ATH_MSG_FATAL ( "  Invalid boundary: " << *pbnd );
    delete pxf;
    abort();
  }
  ATH_MSG_VERBOSE ( "  Segment surface: " << *psrf );

  Amg::Transform3D gToSurf = psrf->transform().inverse();
  double lx = measphi ? seg.s0 : 0.;
  double ly = measphi ? 0. : seg.s0;
  double lz = seg.zshift;
  double dlx = measphi ? seg.s1 : 0.;
  double dly = measphi ? 0. : seg.s1;
  double dlz = 1.;
  Amg::Vector3D lposAMDB(lx,ly,lz);
  Amg::Vector3D gpos = lToGlobal*lposAMDB;
  Amg::Vector3D lposRef = gToSurf*gpos;
  Amg::Vector3D ldirAMDB(dlx,dly,dlz);
  Amg::Vector3D gdir = lToGlobal.linear()*ldirAMDB;
  Amg::Vector3D ldirRef = gToSurf.linear()*gdir;
  Amg::Vector3D lposRefShift = lposRef - ldirRef*lposRef.z();

  ATH_MSG_VERBOSE ( " extrapolation to lposRef.z() " << lposRef.z() );
  //std::cout << " local pos " << lposAMDB << " locref " << lposRef << " ldir ref " << ldirRef << " lpos shift " << lposRefShift
  //<< " segpos " << seg.s0 << " angle " << seg.s1 << std::endl;
  double s0 = lposRefShift.x();
//   for ( int iclu=0; iclu<seg.nclus; ++iclu ) {
//     double seg_y = seg.s0 + seg.s1*seg.clus[iclu].locX();
//     Amg::Vector3D lposHit = gToSurf*seg.clus[iclu].cl->globalPosition();
//     Amg::Vector3D lposSeg = lposRefShift + ldirRef*(lposHit.z()-lposRefShift.z());
//     std::cout << " cl pos (" << seg.clus[iclu].locY() << "," << seg.clus[iclu].locX() << ") residual " << seg.clus[iclu].locY() - seg_y 
// 	      << " lpos ref " << lposHit << " lposSeg " << lposSeg << " res " << lposHit.x() - lposSeg.x() << std::endl;
//   }

  ATH_MSG_VERBOSE ( "  Input position, slope: " << s0 << " " << seg.s1 );
  ATH_MSG_VERBOSE ( "                  Error: " << seg.d0 << "  " << seg.d1 << " " << seg.d01 );
  //std::cout<<"  Input position, slope: " << s0 << " " << seg.s1<<std::endl;
  //std::cout<<"                  Error: " << seg.d0 << "  " << seg.d1 << " " << seg.d01<<std::endl;
  //  ATH_MSG_VERBOSE ( "            Orientation: " << measphi_name(measphi) );


  //  const DataHandle<CscStripPrepDataContainer> colldig;
  //  if (evtStore()->retrieve(colldig, m_cscdig_sg_inkey).isFailure()) {
  //    ATH_MSG_FATAL << " Cannot retrieve CscStripPrepData Collection " );
  //    return StatusCode::FAILURE;
  //  }


  // Build list of RIO on track objects.
  ICscSegmentFinder::MbaseList* prios = new ICscSegmentFinder::MbaseList;
  getRios(seg, prios, measphi);  // if hit is in outlier, error is estimated in width/sqrt(12) 

  // Fit quality.
  int ndof = int(prios->size()) - 2;
  if(m_IPconstraint) ndof = ndof + 1;
  if(use2Lay) ndof=1;
  Trk::FitQuality* pfq = new Trk::FitQuality(seg.chsq, ndof);
  // Build segment.
  // Build position vector.
  Amg::Vector2D pos(s0, 0.0);
  // Build direction vector.
  double ameas = atan2(1.0, seg.s1);
  Trk::LocalDirection pdir(ameas, pi2);
  // Error matrix.
  double dfac = -1.0/(1 + seg.s1*seg.s1);
  double e_pos_pos = seg.d0*seg.d0;
  double e_pos_dir = seg.d01*dfac;
  const double e_dir_dir = seg.d1*seg.d1*dfac*dfac;
  Amg::MatrixX cov(4,4);
  cov.setIdentity();
  cov(0,0) = e_pos_pos;
  cov(0,2) = e_pos_dir;
  cov(2,0) = cov(0,2);
  cov(2,2) = e_dir_dir;
  cov(1,1) = 1000000.0;
  cov(1,3) = 0.0;
  cov(3,1) = cov(1,3);
  cov(3,3) = 1.0;

  MuonSegment* pseg_ref = new MuonSegment(pos, pdir, cov, psrf, prios, pfq, Trk::Segment::Csc2dSegmentMaker);
  pseg_ref->setT0Error(float(seg.time), float(seg.dtime));

  ATH_MSG_DEBUG ( "  build_segment::  right after ctor*    " << pseg_ref->time() );
  
  Amg::Transform3D globalToLocal = pro->transform(chid).inverse();
  Amg::Vector3D d(globalToLocal.linear()*pseg_ref->globalDirection());
  //std::cout<<"d.x()="<<d.x()<<"; d.z()="<<d.z()<<std::endl;
  //std::cout<<"   Position: " << pos[Trk::loc1] << " " << pos[Trk::loc2]<<std::endl;
  //std::cout<<"seg.s1 : ameas " << seg.s1 << " " << ameas<<std::endl;
  //std::cout<<"  Direction: " << pdir.angleXZ() << " " << pdir.angleYZ()<<std::endl;
  double tantheta = d.x()/d.z(); // is equal to seg.s1

  ATH_MSG_VERBOSE ( "   Position: " << pos[Trk::loc1] << " " << pos[Trk::loc2] );
  ATH_MSG_VERBOSE ("seg.s1 : ameas " << seg.s1 << " " << ameas );
  ATH_MSG_VERBOSE ( "  Direction: " << pdir.angleXZ() << " " << pdir.angleYZ() );
  ATH_MSG_VERBOSE ( "  d.x : d.z : tantheta " << d.x() << " " << d.z() << " " << tantheta );
  ATH_MSG_VERBOSE ( resetiosflags(std::ios_base::floatfield) );

  
  MuonSegment* pseg =pseg_ref->clone();
  ATH_MSG_DEBUG ( "  build_segment::  right after copying *  " << pseg->time() );

  double diff_tantheta = 99;
  int n_update =0;
  while (diff_tantheta > m_fitsegment_tantheta_tolerance && n_update<10) {
    // Loop over collections in the container.
    ICscSegmentFinder::MbaseList* prios_new = new ICscSegmentFinder::MbaseList;
    ICscSegmentFinder::TrkClusters fitclus;
    const ICscSegmentFinder::RioList& oldrios = pseg_ref->containedROTs();

    int cnt =0;
    for ( ICscSegmentFinder::RioList::size_type irio=0; irio<prios->size(); ++irio ) {
      //      const Trk::RIO_OnTrack* pold = oldrios[irio];
      const Trk::RIO_OnTrack* pold = oldrios[irio];
      const Trk::RIO_OnTrack* cot = (seg.outlierid == cnt) ? pold->clone() // No update for outlier owing to error blown up
        :  m_rotCreator->createRIO_OnTrack( *pold->prepRawData(),
                                            pold->globalPosition(),
                                            pseg->globalDirection() );
      if( !cot ) {
	ATH_MSG_WARNING("Failed to create CscClusterOnTrack");
	continue;
      }
      const CscClusterOnTrack* pcl = dynamic_cast<const CscClusterOnTrack*>(cot);
      if( !pcl ){
	ATH_MSG_WARNING("Failed to cast to CscClusterOnTrack");
	delete cot;
	continue;
      }
      prios_new->push_back(cot);
      
      // Create new calibrated hit to put into fitclus      
      const CscPrepData* prd = pcl->prepRawData();
      Amg::Vector3D lpos = gToLocal*pcl->globalPosition();
      // std::cout << " " << m_idHelper->toString(pcl->identify()) << " lpos " << lpos << "  locPos " << prd->localPosition() << std::endl;


      ATH_MSG_DEBUG ( "    ---+++----> build_segment each rios time " << pcl->time() << " " << prd->time() );

      ATH_MSG_VERBOSE ( cnt << " " << n_update << "   error " );
      if (seg.outlierid == cnt) 
        ATH_MSG_VERBOSE (  " !! outlier !! " );
      else 
        ATH_MSG_VERBOSE (  " !!         !! " );
      ATH_MSG_VERBOSE ( Amg::error(pcl->localCovariance(),Trk::loc1) );

      fitclus.push_back(ICscSegmentFinder::Cluster(lpos,pcl,measphi));
      cnt++;
    }

    ICscSegmentFinder::Segment seg_new; // Reconstruct segment in 2nd try with re-calibrated cluster position error
    seg_new.outlierid = seg.outlierid;

    fit_segment(fitclus, lpos000, seg_new.s0, seg_new.s1, seg_new.d0, seg_new.d1, seg_new.d01, seg_new.chsq, seg_new.time, seg_new.dtime, seg_new.zshift, seg.outlierid);
    ATH_MSG_DEBUG("build_segments:: " << seg_new.time << " " << seg_new.dtime << " fitclus size " << fitclus.size() );
    
    if (seg.outlierid >= 0) 
      ATH_MSG_VERBOSE ( n_update << " outlierid =" << seg.outlierid
                        << "   new chsq  ==> " << seg_new.chsq );

    Trk::FitQuality* pfq_new = new Trk::FitQuality(seg_new.chsq, ndof);

    double lx = measphi ? seg_new.s0 : 0.;
    double ly = measphi ? 0. : seg_new.s0;
    double lz = seg_new.zshift;
    double dlx = measphi ? seg_new.s1 : 0.;
    double dly = measphi ? 0. : seg_new.s1;
    double dlz = 1.;
    Amg::Vector3D lposAMDB(lx,ly,lz);
    Amg::Vector3D gpos = lToGlobal*lposAMDB;
    Amg::Vector3D lposRef = gToSurf*gpos;
    Amg::Vector3D ldirAMDB(dlx,dly,dlz);
    Amg::Vector3D gdir = lToGlobal.linear()*ldirAMDB;
    Amg::Vector3D ldirRef = gToSurf.linear()*gdir;
    Amg::Vector3D lposRefShift = lposRef - ldirRef*lposRef.z();
//     std::cout << " local pos " << lposAMDB << " locref " << lposRef << " ldir ref " << ldirRef << " lpos shift " << lposRefShift
// 	      << " segpos " << seg_new.s0 << " angle " << seg_new.s1 << std::endl;
    double s0 = lposRefShift.x();

    Amg::Vector2D pos_new(s0, 0.0);
    // Build direction vector.
    double ameas_new = atan2(1.0, seg_new.s1);
    Trk::LocalDirection pdir_new(ameas_new, pi2);
    // Error matrix.
    double dfac_new = -1.0/(1 + seg_new.s1*seg_new.s1);
    double e_pos_pos_new = seg_new.d0*seg_new.d0;
    double e_pos_dir_new = seg_new.d01*dfac_new;
    const double e_dir_dir_new = seg_new.d1*seg_new.d1*dfac_new*dfac_new;
    Amg::MatrixX cov(4,4);
    cov.setIdentity();
    cov(0,0) = e_pos_pos_new;
    cov(0,2) = e_pos_dir_new;
    cov(2,2) = e_dir_dir_new;
    cov(1,1) = 1000000.0;
    cov(1,3) = 0.0;
    cov(3,3) = 1.0;

    MuonSegment* pseg_new = new MuonSegment(pos_new, pdir_new, cov, pseg->associatedSurface().clone(),
                                            prios_new, pfq_new, Trk::Segment::Csc2dSegmentMaker);
    pseg_new->setT0Error(float(seg_new.time), float(seg_new.dtime));
    ATH_MSG_DEBUG ( "  build_segment::  right after recreating *  " << pseg_new->time() );

    delete pseg;
    pseg = pseg_new;
    //    ATH_MSG_DEBUG ( "  build_segment::  right after new assigning *  " << pseg->time() );
    //    pseg->setT0Error(float(seg_new.time), float(seg_new.dtime));
    ATH_MSG_DEBUG ( "  build_segment::  right after new assigning and resetting *  " << pseg->time() );

    Amg::Vector3D dnew(globalToLocal.linear()*pseg->globalDirection());
    double tanthetanew = dnew.x()/dnew.z();
    diff_tantheta = fabs(tanthetanew-tantheta)/tantheta;

    ATH_MSG_VERBOSE ( "    tantheta change in segment: "
                      << tantheta << " ==> " << tanthetanew << " -ratio- " << diff_tantheta );
    
    tantheta = tanthetanew;
    n_update++;
  }
  ATH_MSG_VERBOSE ( " chisq " << (pseg->fitQuality())->chiSquared() );

  if ( (pseg->fitQuality())->chiSquared() > m_max_chisquare ) {
    ATH_MSG_DEBUG (" Segment dropped chisq " << (pseg->fitQuality())->chiSquared() << " cut value " <<  m_max_chisquare);
    delete pseg;
    delete pseg_ref;
    return 0;
  }

  const Trk::LocalParameters& l = pseg->localParameters();
  ATH_MSG_VERBOSE ( "    Seg local key: " << l.parameterKey() );
  ATH_MSG_VERBOSE ( " Seg local params: " << l[Trk::loc1] << " " << l[Trk::loc2] << " "
                    << l[Trk::phi] << " " << l[Trk::theta] );
  const Trk::LocalDirection& dr = pseg->localDirection();
  ATH_MSG_VERBOSE ( "    Seg local dir: " << dr.angleXZ() << " " << dr.angleYZ() );
  const Amg::Vector3D& g = pseg->globalPosition();
  ATH_MSG_VERBOSE ( "    Seg global pos: " << g.x() << " " << g.y() << " " << g.z() );

  delete pseg_ref;

  //  pseg->dump(cout);
  
  ATH_MSG_DEBUG ( "  build_segment::  right just before returning *  " << pseg->time() );
  return pseg;
}

/**************************************************************/
void CscSegmentUtilTool::
find_2dsegments(bool measphi, int station,  int eta, int phi, 
                const ICscSegmentFinder::ChamberTrkClusters& chclus, const Amg::Vector3D& lpos000, 
                ICscSegmentFinder::Segments& segs,
                double lpos, double lslope) const {

  ATH_MSG_DEBUG ( "find_2dsegments called!!  ID: " << measphi_name(measphi) << " " << std::showpos << eta
                    << " " << station_name(station) << " " << phi << "   " );
  ATH_MSG_DEBUG ( "  Counts: "  << chclus[0].size() << " " << chclus[1].size() << " "
                    << chclus[2].size() << " " << chclus[3].size() );

  //  for (ICscSegmentFinder::TrkClusters::const_iterator iclu=chclus.begin(); iclu!=chclus.end(); ++iclu ) {
  //    Trk::ParamDefs ierr = Trk::loc1;
  //    double d = (*iclu)->localErrorMatrix().error(ierr);


  
  const ICscSegmentFinder::TrkClusters& clus1 = chclus[0];
  const ICscSegmentFinder::TrkClusters& clus2 = chclus[1];
  const ICscSegmentFinder::TrkClusters& clus3 = chclus[2];
  const ICscSegmentFinder::TrkClusters& clus4 = chclus[3];
  ICscSegmentFinder::TrkClusters fitclus;
  fitclus.reserve(4);
  ICscSegmentFinder::Segment seg;

  // Loop over clusters in each layer and find all combinatoric segments.
  for (ICscSegmentFinder::TrkClusters::const_iterator icl1=clus1.begin(); icl1!=clus1.end(); ++icl1 ) {
    for (ICscSegmentFinder::TrkClusters::const_iterator icl2=clus2.begin(); icl2!=clus2.end(); ++icl2 ) {
      for (ICscSegmentFinder::TrkClusters::const_iterator icl3=clus3.begin(); icl3!=clus3.end(); ++icl3 ) {
        for (ICscSegmentFinder::TrkClusters::const_iterator icl4=clus4.begin(); icl4!=clus4.end(); ++icl4 ) {
          fitclus.clear();
          fitclus.push_back(*icl1);
          fitclus.push_back(*icl2);
          fitclus.push_back(*icl3);
          fitclus.push_back(*icl4);

          Trk::ParamDefs ierr = Trk::loc1;
          ATH_MSG_VERBOSE ( " +++++++ find_2dsegments ++ Errors "
                            << Amg::error( (*icl1).cl->localCovariance(),ierr) << " "
                            << Amg::error( (*icl2).cl->localCovariance(),ierr) << " "
                            << Amg::error( (*icl3).cl->localCovariance(),ierr) << " "
                            << Amg::error( (*icl4).cl->localCovariance(),ierr) << " " );

          seg.s0 = lpos;
          seg.s1 = lslope;
          fit_segment(fitclus, lpos000, seg.s0, seg.s1, seg.d0, seg.d1, seg.d01, seg.chsq, seg.time, seg.dtime, seg.zshift);
          seg.clus[0] = *icl1;
          seg.clus[1] = *icl2;
          seg.clus[2] = *icl3;
          seg.clus[3] = *icl4;


          int nunspoil=0;
          for (int i=0; i<4; ++i) {
            if ( IsUnspoiled ( seg.clus[i].cl->status() ) )++nunspoil;
            ATH_MSG_DEBUG("Status of HIT #" << i << " " << seg.clus[i].cl->status() );
          }
          
          seg.nunspoil =nunspoil;
          seg.outlierid =-1;
          seg.nclus = 4;

          double local_max_chi =0.;
          if (nunspoil > 2)      local_max_chi =m_max_chisquare_loose;
          else local_max_chi =m_max_chisquare;

          bool keep = true;
          if (seg.chsq > m_max_chisquare_tight) keep = false;

          ATH_MSG_VERBOSE (" find_2dsegments:: nunspoil and local_max_chi " << nunspoil << " " << local_max_chi << " "
                           << keep << "  " << m_max_chisquare_tight << "  " << seg.chsq );
          
          if(!keep) {
          // chi2 after outlier removal on 4 hit segments.
            double outlierRemoved_chsq;
            int ioutlierid = find_outlier_cluster(seg.clus, lpos000, outlierRemoved_chsq);
            if(ioutlierid>0&&outlierRemoved_chsq<m_max_chisquare_tight) {
              keep = true;
              seg.chsq = outlierRemoved_chsq; 
              seg.outlierid = ioutlierid; 
              ATH_MSG_DEBUG("find_2dsegments: keep outlier ");
            } else {
              ATH_MSG_DEBUG("find_2dsegments: reject segment bad chi2 ");
            }
          } 
          ATH_MSG_DEBUG("find_2dsegments:: " << seg.time << " " << seg.dtime);

	  //reject segments with slope of 0 (should only apply to NCB segments)
	  if(seg.s1==0 && !m_IPconstraint){ATH_MSG_DEBUG("slope too small, rejecting"); keep=false;}

          if ( keep )  segs.push_back(seg);

          ATH_MSG_VERBOSE (" nunspoil and local_max_chi " << nunspoil << " " << local_max_chi << " " << keep << " "
                           << seg.s1 << " >< " << m_max_slope_r << " +  " << seg.d1); 

          ATH_MSG_VERBOSE ( "  Segment  measphi? " << measphi
                            << " chsq:" << seg.chsq << "  abs(seg.s1) "<< std::abs(seg.s1)
                            << " req eta:phi " <<  m_max_slope_r + seg.d1
                            << " : " <<m_max_slope_phi + seg.d1
                            << " keep? " << keep );
        }
      }
    }
  }
  
  // Sort segments by nunspoil then chi-square.
  std::sort(segs.begin(), segs.end(), ICscSegmentFinder::sortByNunspoilAndChsq());
}

///////////////////////////////////////
/** Adds 3-hit segments to 4-hit segments **/
void CscSegmentUtilTool::add_2dsegments(ICscSegmentFinder::Segments &segs4, ICscSegmentFinder::Segments &segs3) const {
  // Limit number of segments

  if(segs4.size()==0&&segs3.size()==0) return; 
  
  ATH_MSG_DEBUG(" Total Input seg4 segment size " << segs4.size()); 
  ATH_MSG_DEBUG(" Total Input seg3 segment size " << segs3.size()); 

  std::vector <int> isegs4OK(segs4.size(),1);
  std::vector <int> isegs3OK(segs3.size(),1);
  ICscSegmentFinder::Segments segs4All;

  ICscSegmentFinder::Segments::const_iterator iseg;
  ICscSegmentFinder::Segments::const_iterator iseg2;

  for (iseg=segs4.begin(); iseg!=segs4.end(); iseg++) {
     segs4All.push_back(*iseg);
  }

  segs4.clear();
  
  int iiseg = -1;
  for (iseg=segs4All.begin(); iseg!=segs4All.end(); iseg++) {
    iiseg++;
    int iiseg2 = iiseg;
    for (iseg2=iseg+1; iseg2!=segs4All.end(); iseg2++) {
      int nhits_common = 0;
      iiseg2++;
      for (int iclus=0; iclus<iseg->nclus; iclus++) {
	const Muon::CscClusterOnTrack* cot = iseg->clus[iclus].cl;
        for (int iclus2=0; iclus2<iseg2->nclus; iclus2++) {
	  const Muon::CscClusterOnTrack* cot2 = iseg2->clus[iclus2].cl;
          if (cot->identify() == cot2->identify()) nhits_common++;
        }
      }
      if(nhits_common > 0 && m_remove4Overlap) {
        isegs4OK[iiseg2] = 0;
        ATH_MSG_DEBUG(" seg4 segment nr " << iiseg2 << " dropped with nhits_common " << nhits_common );
      }
    }
  }

  iiseg = -1;
  for (iseg=segs4All.begin(); iseg!=segs4All.end(); iseg++) {
    iiseg++;
    const Muon::CscClusterOnTrack* cot = iseg->clus[0].cl;
    Identifier id = cot->identify();
    if(!m_phelper->measuresPhi(id) && iseg->nunspoil < m_nunspoil) {
       ATH_MSG_DEBUG(" seg4 eta segment rejected with nclusters too few unspoiled hits " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil);
       isegs4OK[iiseg] = 0;
    }
    if( isegs4OK[iiseg] == 1 && segs4.size()< m_max_seg_per_chamber ) {
      segs4.push_back(*iseg);
      ATH_MSG_DEBUG(" seg4 segment accepted with nclusters " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil <<  " measuresPhi " << m_phelper->measuresPhi(id) );
    } else {
      ATH_MSG_DEBUG(" seg4 segment rejected with nclusters " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil <<  " measuresPhi " << m_phelper->measuresPhi(id) ) ;
    }
  }
  int segs4Size = segs4.size();  
  ATH_MSG_DEBUG(" Total seg4 segment accepted size " << segs4Size ); 

  if ( segs4.size() == m_max_seg_per_chamber ) {
    return;
  } 

  for (iseg=segs4.begin(); iseg!=segs4.end(); iseg++) {
    int iiseg2 = -1;
    for (iseg2=segs3.begin(); iseg2!=segs3.end(); iseg2++) {
      iiseg2++;
      int nhits_common = 0;
      for (int iclus=0; iclus<iseg->nclus; iclus++) {
	const Muon::CscClusterOnTrack* cot = iseg->clus[iclus].cl;
        for (int iclus2=0; iclus2<iseg2->nclus; iclus2++) {
	  const Muon::CscClusterOnTrack* cot2 = iseg2->clus[iclus2].cl;
          if (cot->identify() == cot2->identify()) nhits_common++;
        }
      }
      if(nhits_common > 0 && m_remove3Overlap) {
        isegs3OK[iiseg2] = 0;
        ATH_MSG_DEBUG(" seg3 segment nr " << iiseg2 << " dropped with nhits_common " << nhits_common );
      }
    }
  }

  iiseg = -1;
  for (iseg=segs3.begin(); iseg!=segs3.end(); iseg++) {
    iiseg++;
    int iiseg2 = iiseg;
    for (iseg2=iseg+1; iseg2!=segs3.end(); iseg2++) {
      iiseg2++;
      int nhits_common = 0;
      for (int iclus=0; iclus<iseg->nclus; iclus++) {
	const Muon::CscClusterOnTrack* cot = iseg->clus[iclus].cl;
        for (int iclus2=0; iclus2<iseg2->nclus; iclus2++) {
	  const Muon::CscClusterOnTrack* cot2 = iseg2->clus[iclus2].cl;
          if (cot->identify() == cot2->identify()) nhits_common++;
        }
      }
      if(nhits_common > 0 && m_remove3Overlap) {
        isegs3OK[iiseg2] = 0;
        ATH_MSG_DEBUG(" seg3 segment nr " << iiseg2 << " dropped with nhits_common " << nhits_common );
      } 
    }
  }

    // As long as we don't exceed max size, add segments from segs3 to end of segs4
  iiseg = -1;
  for (iseg=segs3.begin(); iseg!=segs3.end(); iseg++) {
    iiseg++;
    const Muon::CscClusterOnTrack* cot = iseg->clus[0].cl;
    Identifier id = cot->identify();
    if(!m_phelper->measuresPhi(id) && iseg->nunspoil < m_nunspoil) {
       ATH_MSG_DEBUG(" seg3 eta segment rejected with nclusters too few unspoiled hits " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil);
       isegs3OK[iiseg] = 0;
    }
    if( isegs3OK[iiseg] == 1 && segs4.size()< m_max_seg_per_chamber ) {
      segs4.push_back(*iseg);
      ATH_MSG_DEBUG(" seg3 segment accepted with nclusters " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil << " measuresPhi " << m_phelper->measuresPhi(id) );
    } else {
      ATH_MSG_DEBUG(" seg3 segment rejected with nclusters " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil << " measuresPhi " << m_phelper->measuresPhi(id) ) ;
    }
  }
  ATH_MSG_DEBUG(" Total seg3 segment size " << segs4.size() - segs4Size ); 
}

/////////////////////////////////////////
//stores 2-hit segments
void CscSegmentUtilTool::add_2dseg2hits(ICscSegmentFinder::Segments &segs, ICscSegmentFinder::Segments &segs2, int layStat) const {
  if(segs2.size()==0) return;
  ATH_MSG_DEBUG(" Total Input 2-layer segment size " << segs2.size());

  int lay0=-1, lay1=-1;
  for(int i=0;i<4;i++){
    if((layStat%(int)pow(10,i+1))/(int)pow(10,i)==0){
      if(lay0==-1) lay0=i;
      else if(lay1==-1) lay1=i;
    }
  }
  bool checkCrossTalk=false;
  if(fabs(lay0-lay1)==1 && lay0+lay1!=3) checkCrossTalk=true; //if we have layers 0 and 1 or 2 and 3 there could be cross-talk creating fake 2-layer segments

  std::vector <int> isegs2OK(segs2.size(),1);
  ICscSegmentFinder::Segments::const_iterator iseg;
  ICscSegmentFinder::Segments::const_iterator iseg2;
  ICscSegmentFinder::Segments segsAll;
  for (iseg=segs.begin(); iseg!=segs.end(); iseg++) {
    segsAll.push_back(*iseg);
  }
  segs.clear();

  int iiseg = -1;
  for (iseg=segs2.begin(); iseg!=segs2.end(); iseg++) {
    iiseg++;
    if(!isegs2OK[iiseg]) continue;
    int iiseg2 = iiseg;
    for (iseg2=iseg+1; iseg2!=segs2.end(); iseg2++) {
      int nhits_common = 0;
      iiseg2++;
      if(!isegs2OK[iiseg2]) continue;
      double charges[2]={0,0};
      for (int iclus=0; iclus<iseg->nclus; iclus++) {
        const Muon::CscClusterOnTrack* cot = iseg->clus[iclus].cl;
	if(checkCrossTalk){
	  const Muon::CscPrepData* prep = cot->prepRawData();
	  std::vector<const Muon::CscStripPrepData*> strips = m_rotCreator->GetICscClusterUtilTool()->getStrips(prep);
	  std::vector<double> stripCharges;
	  for (unsigned int s=0; s<strips.size(); ++s) {
	    ICscClusterFitter::StripFit sfit;
	    sfit = m_rotCreator->GetICscStripFitter()->fit(*strips[s]);
	    stripCharges.push_back(sfit.charge);
	  }
	  double maxCharge=0,centCharge=0;
	  for(unsigned int s=0;s<stripCharges.size();s++){
	    if(stripCharges[s]>maxCharge){
	      maxCharge=stripCharges[s];
	      centCharge=stripCharges[s];
	      if(s>0) centCharge+=stripCharges[s-1];
	      if(s<stripCharges.size()-1) centCharge+=stripCharges[s+1];
	    }
	  }
	  charges[iclus]=centCharge;
	  if(iclus==1){
	    float chargeRatio=charges[0]/charges[1];
	    if(charges[0]>charges[1]) chargeRatio=charges[1]/charges[0];
	    if(chargeRatio<.01){ nhits_common=-1; break; } //ratio this small means crosstalk, kill this segment
	  }
	}
        for (int iclus2=0; iclus2<iseg2->nclus; iclus2++) {
          const Muon::CscClusterOnTrack* cot2 = iseg2->clus[iclus2].cl;
          if (cot->identify() == cot2->identify()) nhits_common++;
        }
      }
      if(nhits_common != 0) { //>0, overlap; <0, cross-talk
        isegs2OK[iiseg2] = 0;
        ATH_MSG_DEBUG(" seg2 segment nr " << iiseg2 << " dropped with nhits_common " << nhits_common );
      }
    }
  }
  iiseg = -1;
  for (iseg=segsAll.begin(); iseg!=segsAll.end(); iseg++) {
    iiseg++;
    int iiseg2 = -1;
    for (iseg2=segs2.begin(); iseg2!=segs2.end(); iseg2++) {
      int nhits_common = 0;
      iiseg2++;
      if(isegs2OK[iiseg2]==0) continue; //already rejected this segment
      for (int iclus=0; iclus<iseg->nclus; iclus++) {
        const Muon::CscClusterOnTrack* cot = iseg->clus[iclus].cl;
	int wlay=m_phelper->wireLayer(cot->identify());
	if((layStat%(int)pow(10,wlay+1))/(int)pow(10,wlay)==1){ //this 3-layer segment has a hit in a bad layer: dump it
	  nhits_common=-1;
	  break;
	}
        for (int iclus2=0; iclus2<iseg2->nclus; iclus2++) {
          const Muon::CscClusterOnTrack* cot2 = iseg2->clus[iclus2].cl;
          if (cot->identify() == cot2->identify()) nhits_common++;
        }
      }
      if(nhits_common > 0) {
        isegs2OK[iiseg2] = 0;
	ATH_MSG_DEBUG(" seg2 segment nr " << iiseg2 << " dropped with nhits_common " << nhits_common );
	if(iseg2==segs2.begin()) segs.push_back(*iseg); //no hits in bad layers, add this segment (but only once)
      }
      else if(nhits_common==0){
	if(iseg2==segs2.begin()) segs.push_back(*iseg); //no hits in bad layers, add this segment (but only once)
      }
      else if(nhits_common==-1) break; //this segment has a hit in a bad layer, skip to the next one
    }
  }
  iiseg = -1;
  for (iseg=segs2.begin(); iseg!=segs2.end(); iseg++) {
    iiseg++;
    const Muon::CscClusterOnTrack* cot = iseg->clus[0].cl;
    Identifier id = cot->identify();
    if( isegs2OK[iiseg] == 1 && segs.size()< m_max_seg_per_chamber ) {
      segs.push_back(*iseg);
      ATH_MSG_DEBUG(" seg2 accepted, nclusters " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil <<  " mPhi " << m_phelper->measuresPhi(id) );
    } else {
      ATH_MSG_DEBUG(" seg2 rejected, nclusters " << iseg->nclus << " chi2 " << iseg->chsq << " unspoiled " << iseg->nunspoil <<  " mPhi " << m_phelper->measuresPhi(id) ) ;
    }
  }
  ATH_MSG_DEBUG(" Total seg2 accepted: " << segs.size() );
}

/////////////////////////////////////////
void CscSegmentUtilTool::
find_2dseg3hit(bool measphi, int station,  int eta, int phi, 
               const ICscSegmentFinder::ChamberTrkClusters& chclus, const Amg::Vector3D& lpos000, 
               ICscSegmentFinder::Segments& segs,
               ICscSegmentFinder::Segments& segs4hit,
               double lpos, double lslope) const {

  ATH_MSG_DEBUG("find_2dseg3hit called");

  // List of possible combinations for three hits.
  const int maxcomb = 4;
  int layAcomb[maxcomb]     = {1, 2, 3, 0};
  int layBcomb[maxcomb]     = {2, 3, 0, 1};
  int layCcomb[maxcomb]     = {3, 0, 1, 2};

  ATH_MSG_VERBOSE ( "station " << station << " eta " << eta
                    << " phi " << phi );
      
  
  // Maximum number of hits per segment
  const int maxhits = 3;
  ICscSegmentFinder::TrkClusters fitclus;
  fitclus.reserve(3);
  ICscSegmentFinder::Segment seg;

  // Loop over four possible combinations of clusters for three segments.
  for (int icomb=0; icomb<maxcomb; icomb++) {

    // iterator over clusters
    ICscSegmentFinder::TrkClusters::const_iterator icl[maxhits];

    // Select appropriate layer for first layer
    const ICscSegmentFinder::TrkClusters& clus1 = chclus[layAcomb[icomb]];
    for (icl[0]=clus1.begin(); icl[0]!=clus1.end(); ++icl[0] ) {

      // Select appropriate layer for second layer
      const ICscSegmentFinder::TrkClusters& clus2 = chclus[layBcomb[icomb]];
      for (icl[1]=clus2.begin(); icl[1]!=clus2.end(); ++icl[1]) {

        // Select appropriate layer for third layer
        const ICscSegmentFinder::TrkClusters& clus3 = chclus[layCcomb[icomb]];
        for (icl[2]=clus3.begin(); icl[2]!=clus3.end(); ++icl[2] ) {
          
          fitclus.clear();
	  // Use these three clusters as a segment.
          for (int i=0; i<maxhits; i++) {
            fitclus.push_back(*icl[i]);
	  }

	  // Check if these hits are used by any other segments
          if ( !unique_hits( fitclus, segs4hit ) ) {
            ATH_MSG_VERBOSE ( " hits already used by other segments ");
            continue;
          } 
	  // Calculate chi2 for this segment.
          seg.s0 = lpos;
          seg.s1 = lslope;
          fit_segment(fitclus, lpos000, seg.s0, seg.s1, seg.d0, seg.d1, seg.d01, seg.chsq, seg.time, seg.dtime, seg.zshift);

	  // Count number of unspoiled clusters
          int nunspoil=0;
          for (int i=0; i<maxhits; i++) {
            seg.clus[i] = *icl[i];
            if ( IsUnspoiled ( seg.clus[i].cl->status() ) )++nunspoil;
	  }

          seg.nunspoil =nunspoil;
          seg.outlierid =-1;
          seg.nclus = 3;

	  // Cut on chi2
          double local_max_chi =0.;
          if (nunspoil > 2)      local_max_chi =m_max_chisquare_loose;
          else local_max_chi =m_max_chisquare;

//  tighten chi2 cut
          if(m_TightenChi2) local_max_chi = 2.*m_max_chisquare/3.;

          bool keep = true;
          if(seg.chsq > local_max_chi) keep = false;

          ATH_MSG_VERBOSE (" nunspoil and local_max_chi " << nunspoil << " " << local_max_chi << " " << keep); 
          ATH_MSG_DEBUG("find_2dseg3hit:: " << seg.time << " " << seg.dtime);

          if(!keep) {
          // chi2 after outlier removal on 3 hit segments.
            double outlierRemoved_chsq;
            int ioutlierid = find_outlier_cluster(fitclus, lpos000, outlierRemoved_chsq);
            if(ioutlierid>-1&&outlierRemoved_chsq<m_max_chisquare_tight) {
              keep = true;
              seg.chsq = outlierRemoved_chsq; 
              seg.outlierid = ioutlierid; 
              ATH_MSG_DEBUG("find_2dseg3hit: keep outlier ");
            } else {
              ATH_MSG_DEBUG("find_2dseg3hit: reject segment bad chi2");
            }
          } 
 	  // Add to segment list.
          if ( keep )  segs.push_back(seg);

          ATH_MSG_VERBOSE ( "  Segment  measphi? " << measphi << " chsq:" << seg.chsq
                            << "  abs(seg.s1) "<< std::abs(seg.s1)
                            << " keep? " << keep );
        }
      }
    }
  }

  // Sort segments by nunspoil then chi-square.
  std::sort(segs.begin(), segs.end(), ICscSegmentFinder::sortByNunspoilAndChsq());

}
void CscSegmentUtilTool::
find_2dseg2hit(bool measphi, int station,  int eta, int phi, int layStat,
               const ICscSegmentFinder::ChamberTrkClusters& chclus, const Amg::Vector3D& lpos000, 
               ICscSegmentFinder::Segments& segs,
               double lpos, double lslope) const {

  ATH_MSG_DEBUG("find_2dseg2hit called");
  // List of possible combinations for three hits.

  ATH_MSG_VERBOSE ( "station " << station << " eta " << eta
                    << " phi " << phi );
      
  int lay0=-1, lay1=-1;
  for(int i=0;i<4;i++){
    if((layStat%(int)pow(10,i+1))/(int)pow(10,i)==0){
      if(lay0==-1) lay0=i;
      else if(lay1==-1) lay1=i;
      else{
	ATH_MSG_WARNING("can't do 2-layer segment finding, more than 2 layers marked as good");
	return;
      }
    }
  }
  if(lay0==-1 || lay1==-1){
    ATH_MSG_WARNING("can't do 2-layer segment finding, fewer than 2 layers marked as good");
    return;
  }
  // Maximum number of hits per segment
  const int maxhits = 2;

    // iterator over clusters
    ICscSegmentFinder::TrkClusters::const_iterator icl[maxhits];

    // Select appropriate layer for first layer
    const ICscSegmentFinder::TrkClusters& clus1 = chclus[lay0];
    for (icl[0]=clus1.begin(); icl[0]!=clus1.end(); ++icl[0] ) {

      // Select appropriate layer for second layer
      const ICscSegmentFinder::TrkClusters& clus2 = chclus[lay1];
      for (icl[1]=clus2.begin(); icl[1]!=clus2.end(); ++icl[1]) {

	// Use these two clusters as a segment.
	ATH_MSG_DEBUG("got 2 clusters for segment");

          ICscSegmentFinder::TrkClusters fitclus;
          for (int i=0; i<maxhits; i++) {
            fitclus.push_back(*icl[i]);
	  }

	  // Calculate chi2 for this segment.
          ICscSegmentFinder::Segment seg;
          seg.s0 = lpos;
          seg.s1 = lslope;
          fit_segment(fitclus, lpos000, seg.s0, seg.s1, seg.d0, seg.d1, seg.d01, seg.chsq, seg.time, seg.dtime, seg.zshift);
       
	  // Count number of unspoiled clusters
          int nunspoil=0;
          for (int i=0; i<maxhits; i++) {
            seg.clus[i] = *icl[i];
            if ( IsUnspoiled ( seg.clus[i].cl->status() ) )++nunspoil;
	  }
          seg.nunspoil =nunspoil;
          seg.outlierid =-1;
          seg.nclus = 2;

	  // Cut on chi2
          double local_max_chi =0.;
          if (nunspoil > 2)      local_max_chi =m_max_chisquare_loose;
          else local_max_chi =m_max_chisquare;
	  //  tighten chi2 cut
          if(m_TightenChi2) local_max_chi = 1.*m_max_chisquare/3.;

          bool keep = true;
          if(seg.chsq > local_max_chi) keep = false; 
          ATH_MSG_VERBOSE (" nunspoil, chi2, and local_max_chi " << nunspoil << " " << seg.chsq<<" "<<local_max_chi << " " << keep); 
          ATH_MSG_DEBUG("find_2dseg2hit:: " << seg.time << " " << seg.dtime);
          // No outlier done on 3 hit segments.


          ATH_MSG_VERBOSE (seg.s1 << " >< " << m_max_slope_r << " +  " << seg.d1); 

	  // Add to segment list.
          if ( keep ){ATH_MSG_DEBUG("good segment found");  segs.push_back(seg);}
	  else ATH_MSG_DEBUG("bad segment, not keeping");

          ATH_MSG_VERBOSE ( "  Segment  measphi? " << measphi << " chsq:" << seg.chsq
                            << "  abs(seg.s1) "<< std::abs(seg.s1)
                            << " req eta:phi " <<  m_max_slope_r + seg.d1
                            << " : " <<m_max_slope_phi + seg.d1
                            << " keep? " << keep );
      }
    }

  // Sort segments by nunspoil then chi-square.
  std::sort(segs.begin(), segs.end(), ICscSegmentFinder::sortByNunspoilAndChsq());

}


void CscSegmentUtilTool::
fit_detailCalcPart2(double q0, double q1, double q2, double q01, double q11, double q02,
               double& s0, double& s1, double& d0, double& d1, double& d01, double& chsq) const {

  ATH_MSG_VERBOSE ( " q1 " << q1 );

  if ( q2*q0-q1*q1 ==0.0 ) {
    s0 = -999;
    s1 = -999;
    d0 = -99;
    d1 = -99;
    d01 = -99;
    chsq = 999;
    return;
  }
  
  double r00 = q2/(q2*q0-q1*q1);
  double r01 = q1/(q1*q1-q2*q0);
  double r10 = q1/(q1*q1-q0*q2);
  double r11 = q0/(q0*q2-q1*q1);
  s0 = r01*q11 + r00*q01;
  s1 = r11*q11 + r10*q01;
  d0 = r01*r01*q2 + 2.0*r00*r01*q1 + r00*r00*q0;
  if (d0<0)
    d0 = -1.0*sqrt(-1.0*d0);
  else
    d0 = sqrt(d0);
  
  d1 = r11*r11*q2 + 2.0*r10*r11*q1 + r10*r10*q0;
  if (d1<0)
    d1 = -1.0*sqrt(-1.0*d1);
  else
    d1 = sqrt(d1);
  d01 = r01*r11*q2 + (r01*r10+r00*r11)*q1 + r00*r10*q0;
  chsq = q02 + s1*s1*q2 + 2*s0*s1*q1 + s0*s0*q0 - 2*s0*q01 - 2*s1*q11;

  ATH_MSG_VERBOSE ( "  details s0 = " << s0 << " " << d0 << " => " << r00);
  ATH_MSG_VERBOSE ( "  details s1 = " << s1 << " " << d1 << " " << d01 << " => "
                    << r11 << " " << r10 << "chsq = " << chsq );


  return;
}


MuonSegmentCombination* CscSegmentUtilTool::
get4dMuonSegmentCombination( const MuonSegmentCombination* insegs ) const {

  ATH_MSG_DEBUG("get4dMuonSegmentCombination called");
  const ICscSegmentFinder::SegmentVec& rsegs = *insegs->stationSegments(0);
  const ICscSegmentFinder::SegmentVec& phisegs = *insegs->stationSegments(1);

  MuonSegmentCombination* pcol = 0;

  ATH_MSG_DEBUG ( "  Combination has r/phi segments " << rsegs.size() << " / " << phisegs.size() );
  ATH_MSG_DEBUG("chamber has "<<insegs->getNGoodCscLayers(1)<<" good eta layers and "<<insegs->getNGoodCscLayers(0)<<" good phi layers");
  
  if( (rsegs.empty() && insegs->useStripsInSegment(1)) || (phisegs.empty() && insegs->useStripsInSegment(0)) ){
    ATH_MSG_DEBUG ( "  Station does not have a r/phi segment, cannot make 4d segment " ); 
    return pcol;
  }

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);

  std::unique_ptr<ICscSegmentFinder::SegmentVec> pnewsegs(new ICscSegmentFinder::SegmentVec);
  if(insegs->useStripsInSegment(1) && insegs->useStripsInSegment(0)){
    for ( ICscSegmentFinder::SegmentVec::const_iterator irsg=rsegs.begin();
	  irsg!=rsegs.end(); ++irsg ) {
      
      for ( ICscSegmentFinder::SegmentVec::const_iterator ipsg=phisegs.begin();
	    ipsg!=phisegs.end(); ++ipsg ) {
	// If there are too many segments, we break from the inner loop.
	// There will be one additional message for each remaining entry in
	// the outer loop.
	if ( pnewsegs->size() >= m_max_seg_per_chamber ) {
	  ATH_MSG_DEBUG ( "Too many segments: Discarding segments m_max_seg_per_chamber="
			  << m_max_seg_per_chamber );
	  break;
	}
	const MuonSegment& rsg = **irsg;
	const MuonSegment& psg = **ipsg;
	const Trk::FitQuality& rfq   = *rsg.fitQuality();
	ATH_MSG_DEBUG("got fit quality for r segment");
	const Trk::FitQuality& phifq = *psg.fitQuality();
	ATH_MSG_DEBUG("and phi segment");
	// Fit quality.
	double chsq = rfq.chiSquared() + phifq.chiSquared();
	ATH_MSG_DEBUG("total chi2="<<chsq);
	if ( chsq > m_max_chisquare ) {
	  ATH_MSG_DEBUG ( "Segment rejected too large chsq: " << chsq);
	  continue;
	}
	// ECC - require good x-y matching.
	//     - a better way would be to calculate this value for all segments,
	//       sort the segments (best to worst) and keep only the top N best ones.
	double xylike = matchLikelihood(rsg, psg);
	ATH_MSG_DEBUG ( "xy likelihood: " << xylike << " min: " << m_min_xylike);
	if (xylike < m_min_xylike) {
	  ATH_MSG_DEBUG ( "Segment rejected too low likelihood: " << xylike);
	  continue;
	}
	//if don't use phi, make segments from eta only; if don't use eta, make segments from phi only; else make segments from both
	std::unique_ptr<MuonSegment> pseg(make_4dMuonSegment(rsg, psg, insegs->use2LayerSegments(1), insegs->use2LayerSegments(0)));
	if( pseg ){
	  ATH_MSG_DEBUG("created new 4d segment");
	  pnewsegs->push_back(std::move(pseg));
	}
      } // for phisegs
    } // for rsegs
  }
  else if(!insegs->useStripsInSegment(0)){
    for ( ICscSegmentFinder::SegmentVec::const_iterator irsg=rsegs.begin();irsg!=rsegs.end(); ++irsg ) {
      const MuonSegment& rsg = **irsg;
      unsigned int nMinRIOs=3;
      if(insegs->use2LayerSegments(1)) nMinRIOs=2;
      if ( rsg.containedROTs().size()<nMinRIOs){
	ATH_MSG_DEBUG("only "<<rsg.containedROTs().size()<<", RIO's, insufficient to build the 4d segment from a single eta segment");
	continue;
      }
      std::unique_ptr<MuonSegment> pseg(new MuonSegment(rsg));
      ATH_MSG_DEBUG("created new 4d segment from eta hits only");
      pnewsegs->push_back(std::move(pseg));
    }
  }
  else if(!insegs->useStripsInSegment(1)){
    for ( ICscSegmentFinder::SegmentVec::const_iterator ipsg=phisegs.begin();ipsg!=phisegs.end(); ++ipsg ) {
      const MuonSegment& psg = **ipsg;
      unsigned int nMinRIOs=3;
      if(insegs->use2LayerSegments(0)) nMinRIOs=2;
      if ( psg.containedROTs().size()<nMinRIOs){
	ATH_MSG_DEBUG("only "<<psg.containedROTs().size()<<", RIO's, insufficient to build the 4d segment from a single phi segment");
        continue;
      }
      std::unique_ptr<MuonSegment> pseg(new MuonSegment(psg));
      ATH_MSG_DEBUG("created new 4d segment from phi hits only");
      pnewsegs->push_back(std::move(pseg));
    }
  }

  if (pnewsegs->empty()) {
    return pcol;
  }
  else ATH_MSG_DEBUG("created "<<pnewsegs->size()<<" new 4d segments");

  pcol =new MuonSegmentCombination;
  pcol->addSegments(std::move(pnewsegs));

  return pcol;
}


/////////////////// This code is perhaps not used any more?
MuonSegmentCombination* CscSegmentUtilTool::
get4dMuonSegmentCombination(  Identifier eta_id, Identifier phi_id,
                              ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                              ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                              const Amg::Vector3D& lpos000 ) const {
  
  ATH_MSG_DEBUG (" get4dMuonSegmentCombination called ");
  
  MuonSegmentCombination* Muon2dSegComb
    = get2dMuonSegmentCombination(eta_id, phi_id, eta_clus, phi_clus,lpos000 );
  
  MuonSegmentCombination* Muon4dSegComb
    = get4dMuonSegmentCombination(Muon2dSegComb);

  delete Muon2dSegComb;

  return Muon4dSegComb;
}


/*************** PRIVATE FUNCTION *******************/
MuonSegment* CscSegmentUtilTool::
make_4dMuonSegment(const MuonSegment& rsg, const MuonSegment& psg, bool use2LaySegsEta, bool use2LaySegsPhi) const {

  ATH_MSG_DEBUG("make_4dMuonSegment called");
  // if(use2LaySegs) std::cout<<"make 4d segment"<<std::endl;
  //  const CscIdHelper* phelper = m_gm->cscIdHelper();

  double rpos = rsg.localParameters()[Trk::locX];
  double rdir = rsg.localDirection().angleXZ();
  double rtime= rsg.time();
  double rerrorTime = rsg.errorTime();

  
  const Amg::MatrixX& rcov = rsg.localCovariance();
  const Trk::PlaneSurface& etasrf = rsg.associatedSurface();
  const ICscSegmentFinder::RioList& etarios = rsg.containedROTs();
  const Trk::FitQuality& rfq = *rsg.fitQuality();
  
  const Trk::FitQuality& phifq = *psg.fitQuality();
  double phipos = psg.localParameters()[Trk::locX];
  double phidir = psg.localDirection().angleXZ();
  const Amg::MatrixX& phicov = psg.localCovariance();
  const Trk::PlaneSurface& phisrf = psg.associatedSurface();
  const ICscSegmentFinder::RioList& phirios = psg.containedROTs();

  // Fit quality.
  double chsq = rfq.chiSquared() + phifq.chiSquared();

  Amg::MatrixX cov(4,4);
  cov.setIdentity();
  // Local position and direction.
  Trk::LocalDirection pdir(phidir, rdir);
  // Error matrix.
  cov(0,0) = phicov(0,0);
  cov(2,2) = phicov(2,2);
  cov(0,2) = phicov(0,2);
  cov(2,0) = cov(0,2);
  cov(1,1) = rcov(0,0);
  cov(3,3) = rcov(2,2);
  cov(1,3) = rcov(0,2);
  cov(3,1) = rcov(1,3);
  ATH_MSG_VERBOSE ( "Error matrix is " << cov.cols() << "D" );
  ATH_MSG_VERBOSE ( "( " << cov(0,0) << " " << cov(0,1) << " " << cov(0,2) << " " << cov(0,3) );
  ATH_MSG_VERBOSE ( "( " << cov(1,0) << " " << cov(1,1) << " " << cov(1,2) << " " << cov(1,3) );
  ATH_MSG_VERBOSE ( "( " << cov(2,0) << " " << cov(2,1) << " " << cov(2,2) << " " << cov(2,3) );
  ATH_MSG_VERBOSE ( "( " << cov(3,0) << " " << cov(3,1) << " " << cov(3,2) << " " << cov(3,3) );
  
// Surface -- for rotations use phi surface for positions use eta surface 
  
  Trk::SurfaceBounds* bnd = phisrf.bounds().clone();
  Trk::TrapezoidBounds* bnd_trap = dynamic_cast<Trk::TrapezoidBounds*>(bnd);
  Trk::RotatedTrapezoidBounds* bnd_rtrap = dynamic_cast<Trk::RotatedTrapezoidBounds*>(bnd);
  Amg::Vector3D glop = etasrf.transform().translation();
  Amg::Transform3D* pxf = new Amg::Transform3D();
  *pxf = phisrf.transform().rotation();
  pxf->pretranslate(glop);
  Trk::PlaneSurface* psrf = 0;
  if( bnd_trap ) {
     psrf = new Trk::PlaneSurface(pxf,bnd_trap);
  } else if( bnd_rtrap ) {
     psrf = new Trk::PlaneSurface(pxf,bnd_rtrap);
  } else {
     ATH_MSG_WARNING ( " no SurfaceBounds bounds for 4D segment found keep old bound ");
     delete pxf;
     psrf = new Trk::PlaneSurface(phisrf);
  }

  Amg::Vector3D philpos(phipos,0.,0.);
  Amg::Vector3D phigpos = phisrf.transform()*philpos;

  Amg::Vector3D phietalpos = psrf->transform().inverse()*phigpos;
  
  ATH_MSG_VERBOSE ( " positions in NEW Eta frame for phi measurement x "  << phietalpos.x() << " y "  <<  phietalpos.y() << " z shift " << phietalpos.z() << " angleXZ " << phidir );
  // if(use2LaySegs)
  //   std::cout<<" positions in NEW Eta frame for phi measurement x "<<phietalpos.x()<<" y " <<phietalpos.y()<<" z shift "<<phietalpos.z()<<" angleXZ "<<phidir<<std::endl;

  double phiposNew = phietalpos.x() - phidir*phietalpos.z();

  ATH_MSG_VERBOSE ( " positions old z " << phipos << " New frame " <<  phietalpos.x() << " corrected " << phiposNew );

  Amg::Vector2D pos(phiposNew, rpos);

  // List of RIO's.
  ICscSegmentFinder::MbaseList* rios = new ICscSegmentFinder::MbaseList;
  // If each orientation has four RIOS's (as expected) then order the
  // RIO's eta1 phi1 eta2 .... to make fitting easier.
  
  // ECC - allow for 3-hit segments.
  unsigned int nMinRIOsEta=3,nMinRIOsPhi=3;
  if(use2LaySegsEta) nMinRIOsEta=2;
  if(use2LaySegsPhi) nMinRIOsPhi=2;
  if ( etarios.size()>=nMinRIOsEta && phirios.size()>=nMinRIOsPhi ) {
    ATH_MSG_DEBUG ( "Using new RIO order." );
    ATH_MSG_DEBUG ( " eta/phi segment sizes: " << etarios.size() << " " << phirios.size() );
    // if(use2LaySegs) std::cout<<" eta/phi segment sizes: " << etarios.size() << " " << phirios.size()<<std::endl;
    // ECC - try to match eta and phi layers
    //          for ( RioList::size_type irio=0; irio<4; ++irio ) {
    int maxeta = etarios.size();
    int maxphi = phirios.size();
    
    // This is used to determine whether any hits are left unmatched.         
    int eta_matchcode = 6; int phi_matchcode = 6;
    if (maxeta == 3) eta_matchcode = 3;
    if (maxphi == 3) phi_matchcode = 3;
    int eta_match=0, phi_match=0;
    
    ATH_MSG_DEBUG("make_4dMuonSegment:: r/phi seg time " << rsg.time() << " " << psg.time() );
    // Loop over all hits in eta and then in phi
    for (int ieta=0; ieta<maxeta; ieta++) {
      for (int iphi=0; iphi<maxphi; iphi++) {
        // Get RIO_OnTrack
        const Trk::RIO_OnTrack* etapold = etarios[ieta];
        const Trk::RIO_OnTrack* phipold = phirios[iphi];
        // ECC figure out wire layer.
        Identifier id_eta = etapold->identify();
        Identifier id_phi = phipold->identify();
        int iw_eta = m_phelper->wireLayer(id_eta);
        int iw_phi = m_phelper->wireLayer(id_phi);
        
        // Check to see if these are the same layers.
    // if(use2LaySegs) std::cout<<" id_eta: " << m_idHelper->toString(id_eta) << " " <<" id_phi: " << m_idHelper->toString(id_phi)<<std::endl;
        if (iw_eta != iw_phi){
            // if(use2LaySegs) std::cout<<"hits in different layers, skip"<<std::endl;
            continue;
        }
        ATH_MSG_DEBUG ( " id_eta: " << m_idHelper->toString(id_eta) << " " <<
                        " id_phi: " << m_idHelper->toString(id_phi) );

	/* commenting out because : 1/ coverity defect 13763+4 "Unchecked dynamic_cast"
	   2/ segment finding must be fast, dynamic cast is time consuming, here only used for dbg cout ... 

	   const CscClusterOnTrack* csceta = dynamic_cast<const Muon::CscClusterOnTrack*>(etapold);
	   const CscClusterOnTrack* cscphi = dynamic_cast<const Muon::CscClusterOnTrack*>(phipold);
	   ATH_MSG_DEBUG ( "make_4dMuonSegment:: ieta/iphi: " << ieta << "/" << iphi
	   << " iw_eta, iw_phi: " << iw_eta << " " << iw_phi
	   << " rio times r/phi: "
	   << csceta->time() << " " << cscphi->time() 
	   );
	*/
        
        // get the reference surface of the eta hit
        const Trk::Surface& surf = etapold->associatedSurface();
        
        // transform the global position of the phi hit into the reference frame of the eta hit
        const Amg::Vector2D* lpos = surf.globalToLocal(phipold->globalPosition());
        
        // calculate the 2D space point
        if(!lpos) {
          Amg::Vector3D locPos = surf.transform().inverse()*phipold->globalPosition();
          Amg::Vector3D locPosS = surf.transform().inverse()*phipold->associatedSurface().center();
          Amg::Vector3D locNorm = surf.transform().inverse().linear()*phipold->associatedSurface().normal();
          Amg::Vector2D lpn(locPos.x(),locPos.y());
          ATH_MSG_WARNING("phipold->globalPosition() not on surface!" << std::endl << std::setprecision(9) <<
                          " etapos: r " << etapold->globalPosition().perp() << " z " << etapold->globalPosition().z() << " surfz " << surf.center().z() << std::endl <<
                          surf << std::endl << 
                          " phipos: r "<<phipold->globalPosition().perp()<<" z "<<phipold->globalPosition().z()<<" surfz "<< phipold->associatedSurface().center().z() << 
			  std::endl << phipold->associatedSurface() << std::endl << 
                          " locpos " << locPos.x() << " " << locPos.y() << " " << locPos.z() << " locposS " << locPosS.x() << " " << locPosS.y() << " " << locPosS.z() << 
                          " inbounds " << surf.insideBounds(lpn) << " normals " << std::setprecision(9) << surf.normal().dot(phipold->associatedSurface().normal()) << 
                          " locN " << locNorm.x() << " " << locNorm.y() << " " << locNorm.z()                          
                          );
	  // if(use2LaySegs) std::cout<<"failed to get local position, skip"<<std::endl;
          continue;
        }

        // Keep track of matched hits.
        phi_match++; eta_match++;
        eta_matchcode -= ieta;
        phi_matchcode -= iphi;
        
        Amg::Vector2D lposnew(etapold->localParameters()[Trk::locX], (*lpos)[Trk::locY]);
        
        // calculate the corresponding global position
        const Amg::Vector3D* gposnew = surf.localToGlobal(lposnew);
        const Amg::Vector3D& gdirnew = rsg.globalDirection();
        
        // create the new rots using the ROT creator
        const Trk::RIO_OnTrack* etaRot
          = m_rotCreator->createRIO_OnTrack(*etapold->prepRawData(), *gposnew, gdirnew); 
        rios->push_back(etaRot);

        const Trk::RIO_OnTrack* phiRot
          = m_rotCreator->createRIO_OnTrack(*phipold->prepRawData(), *gposnew);
        rios->push_back(phiRot);
        
        // debug output, to be removed
        const Trk::Surface& surfPhi = phipold->associatedSurface();
        const Amg::Vector2D* lposEta = surf.globalToLocal( etaRot->globalPosition() );
        const Amg::Vector2D* lposPhi = surfPhi.globalToLocal( phiRot->globalPosition() );
        ATH_MSG_VERBOSE ( " eta gp " << etapold->globalPosition()
                          << " new " << etaRot->globalPosition() << " loc " << *lposEta );
        ATH_MSG_VERBOSE ( " phi gp " << phipold->globalPosition()
                          << " new " << phiRot->globalPosition() << " loc " << *lposPhi );
        delete lposEta;
        delete lposPhi;
        
        // clean up pointers
        delete lpos;
        delete gposnew;
      } // end loop over phi
    } // end loop over eta

    // if(use2LaySegs) std::cout<<"matched "<<eta_match<<" eta hits and "<<phi_match<<" phi hits"<<std::endl;

    // Handle unmatched hits here.
    int eta_single = maxeta - eta_match;
    int phi_single = maxphi - phi_match;
    
    ATH_MSG_DEBUG ( " eta_single, phi_single: " << eta_single << " " << phi_single
                    << " eta_matchcode, phi_matchcode: " << eta_matchcode << " " << phi_matchcode );
    
    // Add unmatched eta hit to the segment.
    if(maxeta>2){ //only if there are at least 3 hits, if it's a 2-layer segment require both hits to be matched
      if (eta_single == 1) {
	if (eta_matchcode>=0 && eta_matchcode<4) {
	  const Trk::RIO_OnTrack* pold = etarios[eta_matchcode];
	  Trk::RIO_OnTrack* pnew = pold->clone();
	  rios->push_back(pnew);
	}
      }
      // This should never happen
      else if (eta_single > 1) {
	ATH_MSG_WARNING ( "More than one unmatched eta hit: " << eta_single );
      }
    }
    else{
      if(eta_single!=0){
	ATH_MSG_DEBUG("eta hit in a 2-layer segment not matched, bailing");
	delete rios;
	delete psrf;
	return 0;
      }
    }
    
    // Add unmatched phi hit to the segment.
    if(maxphi>2){
      if (phi_single == 1) {
	if (phi_matchcode >=0 && phi_matchcode <4) {
	  const Trk::RIO_OnTrack* pold = phirios[phi_matchcode];
	  Trk::RIO_OnTrack* pnew = pold->clone();
	  rios->push_back(pnew);
	}
      }
      // This should never happen
      else if (phi_single > 1) {
	ATH_MSG_WARNING ( "More than one unmatched phi hit: " << phi_single );
      }
    }
    else{
      if(phi_single!=0){
        ATH_MSG_DEBUG("phi hit in a 2-layer segment not matched, bailing");
        delete rios;
	delete psrf;
        return 0;
      } 
    }
  } //if ( etarios.size()>2 && phirios.size()>2 ) {
  else {  // We should never get here!
    ATH_MSG_WARNING ( "Unexpected input RIO counts: " << etarios.size()
                      << " " << phirios.size() );
    for ( ICscSegmentFinder::RioList::const_iterator irio=etarios.begin();
          irio!=etarios.end(); ++irio ) {
      const Trk::RIO_OnTrack* pold = *irio;
      Trk::RIO_OnTrack* pnew = pold->clone();
      rios->push_back(pnew);
    }
    for ( ICscSegmentFinder::RioList::const_iterator irio=phirios.begin();
          irio!=phirios.end(); ++irio ) {
      const Trk::RIO_OnTrack* pold = *irio;
      Trk::RIO_OnTrack* pnew = pold->clone();
      rios->push_back(pnew);
    }
  } //if ( etarios.size()>2 && phirios.size()>2 ) 
  
  unsigned int nMinRIOsTot=5;
  if(use2LaySegsEta || use2LaySegsPhi) nMinRIOsTot=4;
  if( rios->size() < nMinRIOsTot ){
    ATH_MSG_WARNING( "too few CSC hits collected, not making segment: rios " << rios->size() );
    delete rios;
    delete psrf;
    return 0;
  }
  int ndof = rfq.numberDoF() + phifq.numberDoF();
  Trk::FitQuality* pfq = new Trk::FitQuality(chsq, ndof);
  // Build 4D segment.

  MuonSegment* pseg = new MuonSegment(pos, pdir, cov, psrf, rios, pfq, Trk::Segment::Csc4dSegmentMaker);
  pseg->setT0Error(rtime, rerrorTime);
  ATH_MSG_DEBUG ( "Segment " << rios->size() << " : ");
  // if(use2LaySegs) std::cout<<"segment from "<< rios->size()<<" rios"<<std::endl;
  
  
  return pseg;
}

///////////////////////////////////////////////////////
void CscSegmentUtilTool::
get2dSegments(  Identifier eta_id, Identifier phi_id,
                ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                ICscSegmentFinder::Segments& eta_segs, ICscSegmentFinder::Segments& phi_segs,
                const Amg::Vector3D& lpos000, int etaStat, int phiStat ) const {
  
  if( !eta_id.is_valid() && !phi_id.is_valid() ){
    ATH_MSG_WARNING("in get2dSegments: got two invalid identifiers" );
    return;
  }
  //  const CscIdHelper* phelper = m_gm->cscIdHelper();
  // check whether Id is valid
  Identifier chId = eta_id.is_valid() ? eta_id : phi_id;
  int col_station = m_phelper->stationName(chId) - 49;
  int col_eta     = m_phelper->stationEta(chId);
  int col_phisec  = m_phelper->stationPhi(chId);

  ATH_MSG_DEBUG ( "get2dSegments called  " << eta_id << "  " << phi_id << "  "
                  << col_station << "  " << col_eta <<  " " << col_phisec << "  "
		  << etaStat << " " << phiStat<< " "
                  << "  Counts: "  << eta_clus[0].size() << " " << eta_clus[1].size() << " "
                  << eta_clus[2].size() << " " << eta_clus[3].size() );


  
  ICscSegmentFinder::Segments eta_segs3hit, phi_segs3hit;

  double pos_eta = -999;
  double slope_eta = -999;
  double pos_phi = -999;
  double slope_phi = -999;
  
  // Find 2D segments.
  find_2dsegments(false, col_station, col_eta, col_phisec, eta_clus, lpos000, eta_segs, pos_eta, slope_eta);
  find_2dsegments(true,  col_station, col_eta, col_phisec, phi_clus, lpos000, phi_segs, pos_phi, slope_phi);
    
  // Find 3-hit 2D segments.
  find_2dseg3hit(false, col_station, col_eta, col_phisec, eta_clus, lpos000, eta_segs3hit, eta_segs, pos_eta, slope_eta);
  find_2dseg3hit(true,  col_station, col_eta, col_phisec, phi_clus, lpos000, phi_segs3hit, phi_segs, pos_phi, slope_phi);

  // Add 3-hit segments to 4-hit segments.
  add_2dsegments(eta_segs, eta_segs3hit);
  add_2dsegments(phi_segs, phi_segs3hit);

  int nGoodEta=0,nGoodPhi=0;
  for (int i=0; i<4; ++i){
    if((etaStat%(int)pow(10,i+1))/(int)pow(10,i)==0) nGoodEta++;
    if((phiStat%(int)pow(10,i+1))/(int)pow(10,i)==0) nGoodPhi++;
  }
  if(nGoodEta==2){
    // Find 2-hit 2D segments for eta.
    ICscSegmentFinder::Segments eta_segs2hit;

    ATH_MSG_VERBOSE ( " start find_2dseg2hit eta ");
    find_2dseg2hit(false, col_station, col_eta, col_phisec, etaStat, eta_clus, lpos000, eta_segs2hit, pos_eta, slope_eta);
	  
    // store 2-hit segments
    ATH_MSG_VERBOSE ( " store 2hit eta segments");
    add_2dseg2hits(eta_segs, eta_segs2hit, etaStat);
  }
  if(nGoodPhi==2){
    // Find 2-hit 2D segments for eta.
    ICscSegmentFinder::Segments phi_segs2hit;
    ATH_MSG_VERBOSE ( " start find_2dseg2hit phi ");
    find_2dseg2hit(true,  col_station, col_eta, col_phisec, phiStat, phi_clus, lpos000, phi_segs2hit, pos_phi, slope_phi);
    ATH_MSG_VERBOSE ( " store 2hit phi segments");
    add_2dseg2hits(phi_segs, phi_segs2hit, phiStat);
  }

}


//***********************************************************************
// By Elliott
/** Method for checking whether three hit segments are already part of 4 hit segments **/
bool CscSegmentUtilTool::
unique_hits(ICscSegmentFinder::TrkClusters& fitclus, ICscSegmentFinder::Segments& segs) const {  // segs => 4hitsegs...

  int nclus = fitclus.size();
  ATH_MSG_VERBOSE ( " unique_hits nclus " << nclus << " segs size " << segs.size());
  // Loop over segments
  ICscSegmentFinder::Segments::const_iterator iseg;
  for (iseg=segs.begin(); iseg!=segs.end(); iseg++) {

    int nhits_common = 0;
    // Loop over hits in the 4-hit segment
    for (int iclus=0; iclus<iseg->nclus; iclus++) {

      // Loop over hits in fitclus
      ICscSegmentFinder::TrkClusters::const_iterator ihit;
      for (ihit=fitclus.begin(); ihit!=fitclus.end(); ihit++) {

	// Increment number of common hits if the ids are the same.
	const Muon::CscClusterOnTrack* cot = iseg->clus[iclus].cl;
	const Muon::CscClusterOnTrack* hit = ihit->cl;
        if (cot->identify() == hit->identify()) nhits_common++;
      }
      //      if (nhits_common == nclus) return false; 
      if (nhits_common == nclus || (nclus>=2 && nhits_common > m_max_3hitseg_sharehit ) ) return false; // WP 4/5/10
    }
  }
  // If we get here, then the hits from the 3-hit segment are unique.
  return true;
}

/////////////////////////////////////
// if hit is in outlier, error is estimated in width/sqrt(12)
void CscSegmentUtilTool::getRios(const ICscSegmentFinder::Segment& seg,
                                 ICscSegmentFinder::MbaseList* prios, bool measphi) const { 
  // Getting CscPrepData
  for ( int iclu=0; iclu<seg.nclus; ++iclu ) {
    const CscClusterOnTrack* pclu = seg.clus[iclu].cl;
    
    if ( pclu != 0 ) {
      // if this cluster is picked up as outlier cluster
      if (seg.outlierid == iclu) {// && pclu->status() == Muon::CscStatusUnspoiled) {
        Identifier id = pclu->identify();

	// get prep raw data.
        const CscPrepData *prd = pclu->prepRawData();
        double err = getDefaultError(id, measphi, prd);

	Amg::MatrixX cov(1,1);
        cov(0,0) = err*err;
        
	// Create new calibrated hit with new error matrix.
        Amg::Vector2D lpos = prd->localPosition();
	Trk::DefinedParameter  locPar(lpos.x(),Trk::locX);
	Trk::LocalParameters   ppars(locPar); 

        //        Trk::ParamDefs icor = Trk::loc1;
        //        double positionAlongStrip = lpos.get(icor); // should be carefully implemented

        Trk::RIO_OnTrack* pclu2 = new CscClusterOnTrack(prd, ppars, cov, 0.0, prd->status(), prd->timeStatus(), prd->time()); 
        prios->push_back(pclu2);
      }
      // Just put into list of RIO_OnTrack
      else {
	Trk::RIO_OnTrack* pclu2 = pclu->clone();
        prios->push_back(pclu2);
      }
    }
  }
}

///////////////////////////////
double CscSegmentUtilTool::
getDefaultError (Identifier id, bool measphi, const CscPrepData *prd ) const {
  const std::vector<Identifier>& strip_ids = prd->rdoList();
  unsigned int nstrip = strip_ids.size();
  const CscReadoutElement* pro = m_gm->getCscReadoutElement(id);
  double pitch = pro->cathodeReadoutPitch(0, measphi);
  // Assign position error.
  double wmeas = pitch*nstrip;
  double weff = wmeas - 20; // wmeas - m_intrinsic_cluster_width;
  double weffmin = 0.5*wmeas;
  if ( weff < weffmin ) weff = weffmin;
  double err = weff/sqrt(12.0);
  return err;
}

// Likelihood calculation for xy matching.
double CscSegmentUtilTool::
matchLikelihood(const MuonSegment& rsg, const MuonSegment& psg) const {

  //  const CscIdHelper* phelper = m_gm->cscIdHelper();

  // Loop over eta and phi segments.
  const ICscSegmentFinder::RioList& etarios = rsg.containedROTs();
  const ICscSegmentFinder::RioList& phirios = psg.containedROTs();
  int maxeta = etarios.size();
  int maxphi = phirios.size();

  double prob_real=1, prob_ghost=1;
  bool match = false;
  for (int ieta=0; ieta<maxeta; ieta++) {
    for (int iphi=0; iphi<maxphi; iphi++) {
      const Trk::RIO_OnTrack* etapold = etarios[ieta];
      const Trk::RIO_OnTrack* phipold = phirios[iphi];
      double qratio;

      const Muon::CscClusterOnTrack *eta_clu = 
	dynamic_cast<const Muon::CscClusterOnTrack*>(etapold);
      if(!eta_clu){
        ATH_MSG_DEBUG ( " continuing because not a CSC eta cluster" );
        continue;
      }

      const Muon::CscClusterOnTrack *phi_clu = 
	dynamic_cast<const Muon::CscClusterOnTrack*>(phipold);
      if(!phi_clu){
        ATH_MSG_DEBUG ( " continuing because not a CSC phi cluster" );
        continue;
      }

      // Get prep raw data
      const Muon::CscPrepData *eta_prd = eta_clu->prepRawData();
      const Muon::CscPrepData *phi_prd = phi_clu->prepRawData();

      // Figure out the layer for each view
      Identifier eta_sid = eta_prd->identify();
      Identifier phi_sid = phi_prd->identify();
      int eta_wlay = m_phelper->wireLayer(eta_sid);
      int phi_wlay = m_phelper->wireLayer(phi_sid);

      // Calculate the charge ratio for same eta & phi layers
      if (eta_wlay == phi_wlay) {

	// Get charge.
	double eta_chg = eta_prd->charge();
	double phi_chg = phi_prd->charge();

	if (eta_chg > 0 && phi_chg > 0) {
	  qratio = eta_chg/phi_chg;
	  prob_real  *= pdf_sig(qratio);
	  prob_ghost *= pdf_bkg(qratio);
          match = true;  
	}
      } // if (eta_wlay)
    } // Loop over phi hits
  } // Loop over eta hits

  // Determine likelihood
  double like = qratio_like(prob_real, prob_ghost);
  if(m_allEtaPhiMatches) {
    if(match)  ATH_MSG_VERBOSE ( " MATCH allEtaPhiMatches " <<  like );
    if(!match)  ATH_MSG_VERBOSE ( " NO MATCH allEtaPhiMatches " <<  like );
    if(match) return 0.5;
    if(!match) return 0.;
  }
  return like;
}

// Function to return pdf value for signal distribution in xy matching.
double CscSegmentUtilTool::pdf_sig(double x) const {

  double f1, f2;
  double par[6] = {1.25049, 1.02934, 0.0517436, 0.0229711, 0.900799, 0.374422};

  // Overflow bin.
  if (x > 10) return 0.015619;

  // Landau
  f1 = par[0]*TMath::Landau(x,par[1], par[2]);

  // gaussian
  f2 = par[3]*TMath::Gaus(x, par[4], par[5]);

  // function is the sum of a landau plus gaussian distribution.
  return f1+f2;
}

// function to return pdf value for background distribution.
double CscSegmentUtilTool::pdf_bkg(double x) const {
  double e1, e2;
  double par[8] = {0.0394188, 0.0486057, 0.0869231, 1.16153, -0.109998, 0.009729, 0.36183,0.228344};

  // Overflow bin
  if (x > 10) return 0.0895146;

  // Treat values below 0.2 as flat bins.
  if (x<0.1) e1 = par[0];
  else if (x<0.2) e1 = par[1];
  else
    e1 = par[2]*exp(-1*par[3]*(x-par[4]));

  // 2nd exponential
  e2 = par[5]*exp(-1*par[6]*(x-par[7]));

  // value is the sum of two functions.
  return e1+e2;
}

// likelihood function for charge ratio
double CscSegmentUtilTool::qratio_like(double pdf_sig, double pdf_bkg) const {
  double like = 0;

  // return zero if both probability distribution functions are zero.
  if (pdf_sig != 0 && pdf_bkg !=0) 
    like = pdf_sig /(pdf_sig + pdf_bkg);

  return like;
}

bool CscSegmentUtilTool::isGood(uint32_t stripHashId) const {
  //ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );                                                                                                             

  unsigned int status = stripStatusBit(stripHashId);
  bool is_good = !( (status & 0x1) || ((status >> 1) & 0x1) ); // test for hot/dead channel                                                                                  
  return is_good;
}

int CscSegmentUtilTool::stripStatusBit ( uint32_t stripHashId ) const {
  uint32_t status = 0x0;
  if ( !m_cscCoolStrSvc->getStatus(status,stripHashId) ) {
    ATH_MSG_WARNING ( " failed to access CSC conditions database - status - "
                      << "strip hash id = " << stripHashId );

    uint8_t status2 = 0x0;
    if ( (m_cscCoolStrSvc->getStatus(status2,stripHashId)).isFailure() ) {
      ATH_MSG_WARNING ( " failed to access CSC conditions database old way - status - "
                        << "strip hash id = " << stripHashId );
    }else{
      ATH_MSG_INFO ( " Accessed CSC conditions database old way - status - "
                     << "strip hash id = " << stripHashId );
    }
  } else {
    ATH_MSG_VERBOSE ( "The status word is " << std::hex << status
      << " for strip hash = " << std::dec << stripHashId );
  }
  return status;
}

