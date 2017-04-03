/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_CSCSEGMENTMAKER_H
#define TRIGL2MUONSA_CSCSEGMENTMAKER_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrigL2MuonSA/TrackData.h"
#include "TrigL2MuonSA/TgcFitResult.h"
#include "TrigL2MuonSA/MuonRoad.h"
#include "TrigL2MuonSA/CscData.h"
#include "TrigL2MuonSA/CscRegUtils.h"

#include <vector>


namespace MuonGM{
  class MuonDetectorManager;
}


namespace TrigL2MuonSA{
  
class CscSegment;
 
 class CscSegmentMaker: public AthAlgTool{
  
  
 public:
  
  
  typedef struct{
    double loc_x;
    double loc_y;
    double loc_z;
    double error;
    double residual;
    int measphi;
    //int index4; not used
    bool enabled;
    int stationname;
    bool isIP;
  } localCscHit;
  
  typedef struct{
    double a;
    double b;
    double zshift;
    double chi2;
    int nhit;
    int stationname;
    double residual;
    //int outlier; obsolete
    std::vector<localCscHit> localHits;
  }local2dSegment;
  
 public:
  static const InterfaceID& interfaceID();
  CscSegmentMaker(const std::string& type, const std::string &name, const IInterface* parent);
  ~CscSegmentMaker();
  
  
  StatusCode initialize();
  StatusCode finalize();
  
  
  //    ReturnCode initializeRegDict(ToolHandle<CscRegDict> cscregdict);
  
  ReturnCode	FindSuperPointCsc( const TrigL2MuonSA::CscHits &cscHits, std::vector<TrigL2MuonSA::TrackPattern> &v_trackPatterns, const TrigL2MuonSA::TgcFitResult &tgcFitResult, const TrigL2MuonSA::MuonRoad &muroad);
  
  
  ReturnCode make_segment(int mod_hash, TrigL2MuonSA::CscHits clusters[8], CscSegment &cscsegment, CscSegment &cscsegment_noip );
  
  ReturnCode make_2dsegment(int measphi, const localCscHit &ip_loc,const std::vector<localCscHit> hits_loc[4], local2dSegment &seg2d_eta,local2dSegment &local2d_noip, int &nhite);
  
  ReturnCode make_2dseg4hit(int measphi, const localCscHit &ip_loc,std::vector<localCscHit> hits_loc[4], std::vector<local2dSegment> &seg2d_4hitCollection, int &nhite);
  
  ReturnCode make_2dseg3hit(int measphi, const localCscHit &ip_loc, const std::vector<localCscHit> hits_loc[4], std::vector<local2dSegment> &seg2d_4hitCollection, int &nhit);
  
  ReturnCode fit_clusters(int measphi, const std::vector<localCscHit> &hits_fit, local2dSegment &seg2d);
  
  ReturnCode make_4dsegment(const local2dSegment &seg2d_eta, const local2dSegment &seg2d_phi,
			    Amg::Vector3D &seg_pos, Amg::Vector3D &seg_dir);
  
  ReturnCode getModuleSP(int mod_hash[2], const TrigL2MuonSA::TgcFitResult &tgcFitResult, int phibin, const TrigL2MuonSA::MuonRoad &muroad, const int exist_clusters[32]);
  
  ReturnCode display_hits(const std::vector<localCscHit> localHits[4]);
  
  CscSegment segmentAtFirstLayer(int mod_hash, TrigL2MuonSA::CscSegment *mu_seg);
  
 private:
  IMessageSvc* m_msgSvc;
  IntegerProperty m_msglevel;
  UtilTools *m_util;
  ToolHandle<CscRegDict> m_cscregdict;
#ifndef XAOD_ANALYSIS
  const MuonGM::MuonDetectorManager *m_muonMgr;
#endif
  
  
  //properties                                                                                                                                                             
  BooleanProperty m_use_geometry;
  DoubleProperty m_max_chisquare;
  DoubleProperty m_max_residual_eta;
  DoubleProperty m_max_residual_phi;
  DoubleProperty m_err_eta;
  DoubleProperty m_err_phi;
  DoubleProperty m_err_ip;
  
  
};
 
class CscSegment{
  
 public:
  
  CscSegment();
  ~CscSegment();
  
  double x(){ return m_x; }
  double y(){ return m_y; }
  double z(){ return m_z; }
  double px(){ return m_px; }
  double py(){ return m_py; }
  double pz(){ return m_pz; }
  double slopeRZ(){ return m_slopeRZ; }
  double interceptRZ(){ return m_interceptRZ; }
  double chiSquare(){ return m_chisquare; }
  double chiSquarePhi(){ return m_chisquare_phi; }
  
  //unsigned int l1id(){ return m_l1id; }
  //void setL1id(unsigned int l1id){ m_l1id = l1id; }
  int nHitEta(){ return m_nhit_eta; }
  void setNHitEta( int nhite){ m_nhit_eta = nhite; }
  int nHitPhi(){ return m_nhit_phi; }
  void setNHitPhi( int nhitp){ m_nhit_phi = nhitp; }
  bool isClean();
  
  ReturnCode set(double x, double y, double z, double px, double py, double pz, double chisquare, double chisquare_phi);
  ReturnCode set(Amg::Vector3D &seg_pos, Amg::Vector3D &seg_dir, double chisquare, double chisquare_phi);
  
 private:
  //unsigned int m_l1id; //not used
  double m_x, m_y, m_z, m_px, m_py, m_pz;
  double m_slopeRZ, m_interceptRZ;
  int m_nhit_eta, m_nhit_phi;
  //bool m_clean; //not used
  double m_chisquare;    
  double m_chisquare_phi;
  
};
 
 
}//namespace TrigL2MuonSA


#endif /* TRIGL2MUONSA_CSCSEGMENTMAKER_H */
