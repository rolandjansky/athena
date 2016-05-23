/** @file Analysis_Tier0.h */

#ifndef TrigInDetAnalysisExample_Analysis_Tier0_H
#define TrigInDetAnalysisExample_Analysis_Tier0_H


#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

#include "TrigInDetAnalysisExample/VtxAnalysis.h"

class Analysis_Tier0 : public TrackAnalysis { 
  
 public:
  
  Analysis_Tier0(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut);
  
  virtual void initialise();
  
  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
		       const std::vector<TIDA::Track*>& testTracks,
		       TrackAssociator* associator);
  

  virtual void execute_vtx(const std::vector<TIDA::Vertex*>& vtx0,
			   const std::vector<TIDA::Vertex*>& vtx1 );
  
  virtual void finalise();

  void setvertices(int numvtx) {m_nVtx = numvtx;}
  
  std::map<std::string, TH1*>::const_iterator THbegin() const { return m_histos.begin(); }
  std::map<std::string, TH1*>::const_iterator THend()   const { return m_histos.end(); }

  std::map<std::string, TProfile*>::const_iterator TEffbegin() const { return m_effhistos.begin(); }
  std::map<std::string, TProfile*>::const_iterator TEffend()   const { return m_effhistos.end(); }

  bool debug() const { return m_debug; }
  
 private:

  void addHistogram( TH1* h ) {
    std::string name = h->GetName();
    m_histos.insert( std::map<std::string, TH1*>::value_type( name, h) );
  }

  void addHistogram( TProfile* h ) {
    std::string name = h->GetName();
    m_effhistos.insert( std::map<std::string, TProfile*>::value_type( name, h) );
  }

  double phi(double p);

 private:

  std::map<std::string, TH1*> m_histos;
  std::map<std::string, TProfile*> m_effhistos;
  
  TProfile* h_total_efficiency;
  TProfile* h_pTeff;
  TProfile* h_etaeff;
  TProfile* h_phieff;
  TProfile* h_d0eff;
  TProfile* h_z0eff;
  TProfile* h_nVtxeff;

  TProfile* h_pTres;
  TProfile* h_ipTres;
  TProfile* h_etares;
  TProfile* h_phires;
  TProfile* h_d0res;
  TProfile* h_z0res;


  TProfile* h_npixvseta;
  TProfile* h_npixvsphi;

  TProfile* h_nsctvseta;
  TProfile* h_nsctvsphi;

  TProfile* h_ntrtvseta;
  TProfile* h_ntrtvsphi;

  TProfile* h_npixvseta_rec;
  TProfile* h_npixvsphi_rec;

  TProfile* h_nsctvseta_rec;
  TProfile* h_nsctvsphi_rec;

  TProfile* h_ntrtvseta_rec;
  TProfile* h_ntrtvsphi_rec;


  TProfile* h_d0vsphi;
  TH2D*     h2d_d0vsphi;
  TH2D*     h2d_d0vsphi_rec;

  TH1D* h_chain;

  TH1D* h_trkpT;
  TH1D* h_trketa;
  TH1D* h_trkphi;
  TH1D* h_trkd0;
  TH1D* h_trkz0;

  TH1D* h_trkdd0;
  TH1D* h_trkdz0;

  TH1D* h_trkd0sig;

  TH1D* h_npix;
  TH1D* h_nsct;
  TH1D* h_nsihits;
  TH1D* h_ntrt;

  TH1D* h_trkpT_rec;
  TH1D* h_trketa_rec;
  TH1D* h_trkphi_rec;
  TH1D* h_trkd0_rec;
  TH1D* h_trkz0_rec;

  TH1D* h_trkdd0_rec;
  TH1D* h_trkdz0_rec;

  TH1D* h_trkd0sig_rec;

  TH1D* h_npix_rec;
  TH1D* h_nsct_rec;
  TH1D* h_nsihits_rec;
  TH1D* h_ntrt_rec;


  TH1D* h_trkpT_residual;
  TH1D* h_trkipT_residual;
  TH1D* h_trketa_residual;
  TH1D* h_trkphi_residual;
  TH1D* h_trkd0_residual;
  TH1D* h_trkz0_residual;

  TH1D* h_trkdd0_residual;
  TH1D* h_trkdz0_residual;


  // Efficiency plateau cuts
  double m_pTCut;
  double m_etaCut;
  double m_d0Cut;
  double m_z0Cut;

  int m_nVtx;

  bool m_debug;

  VtxAnalysis* m_vtxanal;

};


#endif // TrigInDetAnalysisExample_Analysis_Tier0_H
