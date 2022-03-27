/* emacs: this is -*- c++ -*- */
/**
 **     @file    AnalysisR3_Tier0.h
 **
 **     @author  mark sutton
 **     @date    $Id: AnalysisR3_Tier0.h   Thu 18 May 2017 15:29:53 CEST 
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_AnalysisR3_Tier0_H
#define TrigInDetAnalysisExample_AnalysisR3_Tier0_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include <iostream>

#include "TH1.h"
#include "TProfile.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

#include "TrigInDetAnalysisExample/VtxR3Analysis.h"

#include "TrigInDetAnalysisExample/TIDAHistogram.h"

class AnalysisR3_Tier0 : public TrackAnalysis { 

public:

  using TrackAnalysis::execute;
  
public:
  
  AnalysisR3_Tier0(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut);
  
  virtual void initialise();

  virtual void initialise_R2();
  virtual void initialise_R3();
  
  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
  		       const std::vector<TIDA::Track*>& testTracks,
  		       TrackAssociator* associator );

  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
  		       const std::vector<TIDA::Track*>& testTracks,
  		       TrackAssociator* associator,
		       const TIDA::Event* tevt );
  
  virtual void execute_vtx(const std::vector<TIDA::Vertex*>& vtx0,
			   const std::vector<TIDA::Vertex*>& vtx1, 
			   const TIDA::Event* tevt=0 );
  
  virtual void finalise();

  void setvertices(int numvtx) {m_nVtx = numvtx;}
  
  std::map<std::string, TH1*>::const_iterator THbegin() const { return m_histos.begin(); }
  std::map<std::string, TH1*>::const_iterator THend()   const { return m_histos.end(); }

  std::map<std::string, TProfile*>::const_iterator TEffbegin() const { return m_effhistos.begin(); }
  std::map<std::string, TProfile*>::const_iterator TEffend()   const { return m_effhistos.end(); }

  void set_monTool( ToolHandle<GenericMonitoringTool>* m ) { m_monTool=m; }

  ToolHandle<GenericMonitoringTool>* monTool() { return m_monTool; }

  bool debug() const { return m_debug; }
  
private:

  void addHistogram( TH1* ) { }

  void addHistogram( TProfile* ) { }

  double phi(double p);

private:

  /// these aren't used for this class but are still needed 
  /// so that the older class can work - it iuses the same interface
  std::map<std::string, TH1*>      m_histos;
  std::map<std::string, TProfile*> m_effhistos;
  
  /// Monitorwd::AScalar Histogram wrapper class

  TIDA::Histogram<float>* m_htotal_efficiency;
  TIDA::Histogram<float>* m_hpTeff;
  TIDA::Histogram<float>* m_hetaeff;
  TIDA::Histogram<float>* m_hphieff;
  TIDA::Histogram<float>* m_hd0eff;
  TIDA::Histogram<float>* m_hz0eff;
  TIDA::Histogram<float>* m_hnVtxeff;
  TIDA::Histogram<float>* m_hlbeff;

  TIDA::Histogram<float>* m_hpTres;
  TIDA::Histogram<float>* m_hipTres;
  TIDA::Histogram<float>* m_hetares;
  TIDA::Histogram<float>* m_hphires;
  TIDA::Histogram<float>* m_hd0res;
  TIDA::Histogram<float>* m_hz0res;

  TIDA::Histogram<float>* m_htrkvtx_x_lb;
  TIDA::Histogram<float>* m_htrkvtx_y_lb;
  TIDA::Histogram<float>* m_htrkvtx_z_lb;


  TIDA::Histogram<float>* m_hnpixvseta;
  TIDA::Histogram<float>* m_hnpixvsphi;
  TIDA::Histogram<float>* m_hnpixvsd0;
  TIDA::Histogram<float>* m_hnpixvspT;

  TIDA::Histogram<float>* m_hnsctvseta;
  TIDA::Histogram<float>* m_hnsctvsphi;
  TIDA::Histogram<float>* m_hnsctvsd0;
  TIDA::Histogram<float>* m_hnsctvspT;

  TIDA::Histogram<float>* m_hntrtvseta;
  TIDA::Histogram<float>* m_hntrtvsphi;

  TIDA::Histogram<float>* m_hnsihits_lb;

  TIDA::Histogram<float>* m_hnpixvseta_rec;
  TIDA::Histogram<float>* m_hnpixvsphi_rec;
  TIDA::Histogram<float>* m_hnpixvsd0_rec;
  TIDA::Histogram<float>* m_hnpixvspT_rec;

  TIDA::Histogram<float>* m_hnsctvseta_rec;
  TIDA::Histogram<float>* m_hnsctvsphi_rec;
  TIDA::Histogram<float>* m_hnsctvsd0_rec;
  TIDA::Histogram<float>* m_hnsctvspT_rec;

  TIDA::Histogram<float>* m_hntrtvseta_rec;
  TIDA::Histogram<float>* m_hntrtvsphi_rec;

  TIDA::Histogram<float>* m_hnsihits_lb_rec;

  TIDA::Histogram<float>* m_hd0vsphi;
  TIDA::Histogram<float>* m_hd0vsphi_rec;


  TIDA::Histogram<float>* m_hchain;
  TIDA::Histogram<float>* m_hntrk;
  TIDA::Histogram<float>* m_htrkpT;

  TIDA::Histogram<float>* m_htrketa;
  TIDA::Histogram<float>* m_htrkphi;
  TIDA::Histogram<float>* m_htrkd0;
  TIDA::Histogram<float>* m_htrkz0;


  TIDA::Histogram<float>* m_htrkdd0;
  TIDA::Histogram<float>* m_htrkdz0;

  TIDA::Histogram<float>* m_htrkd0sig;

  TIDA::Histogram<float>* m_hnpix;
  TIDA::Histogram<float>* m_hnsct;
  TIDA::Histogram<float>* m_hnsihits;
  TIDA::Histogram<float>* m_hntrt;

  TIDA::Histogram<float>* m_hntrk_rec;

  TIDA::Histogram<float>* m_hlayer;


  TIDA::Histogram<float>* m_htrkpT_rec;
  TIDA::Histogram<float>* m_htrketa_rec;
  TIDA::Histogram<float>* m_htrkphi_rec;
  TIDA::Histogram<float>* m_htrkd0_rec;
  TIDA::Histogram<float>* m_htrkz0_rec;

  TIDA::Histogram<float>* m_htrkdd0_rec;
  TIDA::Histogram<float>* m_htrkdz0_rec;

  TIDA::Histogram<float>* m_htrkd0sig_rec;

  TIDA::Histogram<float>* m_hnpix_rec;
  TIDA::Histogram<float>* m_hnsct_rec;
  TIDA::Histogram<float>* m_hnsihits_rec;
  TIDA::Histogram<float>* m_hntrt_rec;


  TIDA::Histogram<float>* m_hlayer_rec;


  TIDA::Histogram<float>* m_htrkpT_residual;
  TIDA::Histogram<float>* m_htrkipT_residual;
  TIDA::Histogram<float>* m_htrketa_residual;
  TIDA::Histogram<float>* m_htrkphi_residual;
  TIDA::Histogram<float>* m_htrkd0_residual;
  TIDA::Histogram<float>* m_htrkz0_residual;

  TIDA::Histogram<float>* m_htrkdd0_residual;
  TIDA::Histogram<float>* m_htrkdz0_residual;



  /// sundry other items

  int      m_nVtx;

  bool     m_debug;

  unsigned long long m_eventid;

  VtxR3Analysis* m_vtxanal;

  ToolHandle<GenericMonitoringTool>* m_monTool;

};


#endif // TrigInDetAnalysisExample_AnalysisR3_Tier0_H
