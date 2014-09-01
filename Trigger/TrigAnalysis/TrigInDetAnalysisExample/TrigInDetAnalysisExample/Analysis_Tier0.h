/** @file Analysis_Tier0.h */

#ifndef TrigInDetAnalysisExample_Analysis_Tier0_H
#define TrigInDetAnalysisExample_Analysis_Tier0_H


#include <iostream>

#include "TH1.h"
#include "TProfile.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

class Analysis_Tier0 : public TrackAnalysis { 
  
 public:
  
  Analysis_Tier0(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut);
  
  virtual void initialise();
  
  virtual void execute(const std::vector<TrigInDetAnalysis::Track*>& referenceTracks,
		       const std::vector<TrigInDetAnalysis::Track*>& testTracks,
		       TrackAssociator* associator);
  
  virtual void finalise();

  void setvertices(int numvtx) {m_nVtx = numvtx;}
  
  std::map<std::string, TH1*>::const_iterator THbegin() const { return m_histos.begin(); }
  std::map<std::string, TH1*>::const_iterator THend()   const { return m_histos.end(); }

  std::map<std::string, TProfile*>::const_iterator TEffbegin() const { return m_effhistos.begin(); }
  std::map<std::string, TProfile*>::const_iterator TEffend()   const { return m_effhistos.end(); }
  
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
  TProfile* h_etares;
  TProfile* h_phires;
  TProfile* h_d0res;
  TProfile* h_z0res;

  TH1D* h_trkpT;
  TH1D* h_trketa;
  TH1D* h_trkphi;
  TH1D* h_trkd0;
  TH1D* h_trkz0;

  // Efficiency plateau cuts
  double m_pTCut;
  double m_etaCut;
  double m_d0Cut;
  double m_z0Cut;

  int m_nVtx;

};


#endif // TrigInDetAnalysisExample_Analysis_Tier0_H
