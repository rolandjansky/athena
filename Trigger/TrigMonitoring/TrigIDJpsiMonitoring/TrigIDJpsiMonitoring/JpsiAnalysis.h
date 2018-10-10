// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    JpsiAnalysis.h       
//
//                   
//
//
//   $Id: SigAnalysis.h, v0.0   Wed 28 Oct 2009 02:41:51 CET sutt $

//edits P. Wijeratne (paw@hep.ucl.ac.uk)	
//edits C. Gutschow (chris.g@cern.ch)	
	
#ifndef __JPSIANALYSIS_H
#define __JPSIANALYSIS_H

#include <iostream>
#include <vector>
#include <map>

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/Efficiency.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

#include "TrigIDJpsiMonitoring/JpsiTrack.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

///temp
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"

///temp
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

class JpsiAnalysis : public TrackAnalysis {
	 
public:

  JpsiAnalysis( const std::string& name ) : TrackAnalysis( name ), m_chainName(name),
                                            m_plusMass(0), m_minusMass(0),
                                            //Nreco(0),
                                            m_print(true), m_keepHistos(true), m_speaker(0) { 
  } 
	
  ~JpsiAnalysis() {
    
    if(!m_keepHistos){
      std::map<std::string, TH1F*>::iterator hitr=m_histos.begin();
      std::map<std::string, TH1F*>::iterator hend=m_histos.end();
      for ( ; hitr!=hend ; hitr++ ) delete hitr->second;     
      
      std::map<std::string, TProfile*>::iterator tphitr=m_tphistos.begin();
      std::map<std::string, TProfile*>::iterator tphend=m_tphistos.end();
      for ( ; tphitr!=tphend ; tphitr++ ) delete tphitr->second;
      
      //    delete h2; delete h2r; delete h2m;
    }

  } 
	 
  virtual void initialise();
	
  virtual void execute(const std::vector<TIDA::Track*>& reftracks,
                       const std::vector<TIDA::Track*>& testtracks,
                       TrackAssociator* matcher);

  virtual void finalise();

  //helper functions
  float deltaR(float eta1, float phi1, float eta2, float phi2);
  float deltaPhi(float phi1, float phi2);
  float fixPhi(float phi);
	 
  void setprint(bool p) { m_print=p; }
  
  std::map<std::string, TH1F*>::const_iterator THbegin() const { return m_histos.begin(); }
  std::map<std::string, TH1F*>::const_iterator THend()   const { return m_histos.end(); }

  std::map<std::string, TProfile*>::const_iterator TPbegin() const { return m_tphistos.begin(); }
  std::map<std::string, TProfile*>::const_iterator TPend()   const { return m_tphistos.end(); }

  std::map<std::string, TH2F*>::const_iterator TH2begin() const { return m_histos2d.begin(); }
  std::map<std::string, TH2F*>::const_iterator TH2end()   const { return m_histos2d.end(); }

  const std::map<std::string, TH1F*>& getHistograms() const { return m_histos; }	

  void setChainName( const std::string& c ){ m_chainName = c; }  

  void setPlusMass( const double& pm ){ m_plusMass = pm; }  
  void setMinusMass( const double& mm ){ m_minusMass = mm; }  

  void setTracksA( const std::vector<TIDA::Track*>& tracksA ){ m_idTracksA = tracksA; }
  void setTracksB( const std::vector<TIDA::Track*>& tracksB ){ m_idTracksB = tracksB; }

  void setSuperTracks( const std::vector< std::vector<TIDA::Track*> >& superTracks ){ m_superTracks = superTracks; }
  void clearSuper(){ 
    std::cout << "m_superTracks " << m_superTracks.size() << std::endl;
    for(unsigned i=m_superTracks.size(); i--; ){
      const std::vector<TIDA::Track*> temp = m_superTracks[i];
      for(unsigned j=temp.size(); j--; ){
	std::cout << "about to crash... " << temp.size() << " temp[0] eta " << temp[j]->eta() << " j " << j << std::endl;
	delete temp[j];
	std::cout << " ... " << std::endl;
      }
      m_superTracks[i].clear();
    } 
    m_superTracks.clear(); 
    std::cout << "OUT" << std::endl;
  }

  void setTIDARois(std::map<int, int>    &m) { m_trackRois = &m; }
  //void setEtaAtCalo(std::map<int, double> &m) { m_etaAtCalo = &m; }
  //void setPhiAtCalo(std::map<int, double> &m) { m_phiAtCalo = &m; }

  void clearOffline(){ m_offlineProbes->clear(); m_offlineTags->clear(); }
  void setOfflineProbes( std::map<unsigned int, const Rec::TrackParticle*>& offs ){ 
    m_offlineProbes = &offs;
  }
  void setOfflineTags(   std::map<unsigned int, const Rec::TrackParticle*>& offs ){ 
    m_offlineTags = &offs;
  }
  void setSpeaker(IHLTMonTool* p){ m_speaker = p; }
  void setVertices(std::vector<TIDA::Vertex*> &v){ m_vertices = v; }
  void setTool(ToolHandle<Trig::TrigDecisionTool> *t){ m_tool = t; }
  void setProbeOfflineDr( std::vector<float> &pOffDr ){ m_pOffDr = &pOffDr; }
  void setProbeOfflineEt( std::vector<float> &pOffEt ){ m_pOffEt = &pOffEt; }
  void setProbeOfflineDEta( std::vector<float> &pOffDeta ){ m_pOffDeta = &pOffDeta; }
  void setProbeOfflineDPhi( std::vector<float> &pOffDphi ){ m_pOffDphi = &pOffDphi; }
  void setProbeOfflineCharge( std::vector<float> &pOffCharge ){ m_pOffCharge = &pOffCharge; }
  void setTagOfflineDr( std::vector<float> &tOffDr ){ m_tOffDr = &tOffDr; }
  void setTagOfflineCharge( std::vector<float> &tOffCharge ){ m_tOffCharge = &tOffCharge; }
  void setCase( bool isMuon, bool isJpsi ){ m_isMuon = isMuon; m_isJpsi = isJpsi; }

  private:
	 
  void addHistogram( TH1F* h ) {
      std::string name = h->GetName();
      m_histos.insert( std::map<std::string, TH1F*>::value_type( name, h) );
   }

  void addHistogram( TProfile* h ) {
    std::string name = h->GetName();
    m_tphistos.insert( std::map<std::string, TProfile*>::value_type( name, h) );
  }

  void addHistogram( TH2F* h ) {
    std::string name = h->GetName();
    m_histos2d.insert( std::map<std::string, TH2F*>::value_type( name, h) );
  }

  TH1F* find( const std::string& n ) {
    m_speaker->msg(MSG::DEBUG) << "name: " << n << endmsg;
    std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(n);
    if ( hmitr!=m_histos.end() ) { 
      m_speaker->msg(MSG::DEBUG) << "hmitr for " << n << ": " << hmitr->second << endmsg;
      return hmitr->second;
    }
    return 0;
  }

  private:

  std::string m_chainName;
  double m_plusMass;
  double m_minusMass;

  std::map<std::string, TH1F*> m_histos;
  std::map<std::string, TProfile*> m_tphistos;
  std::map<std::string, TH2F*> m_histos2d;

  std::vector<TIDA::Track*> m_idTracksA;
  std::vector<TIDA::Track*> m_idTracksB;

  std::vector< std::vector<TIDA::Track*> > m_superTracks;
  
  TProfile* m_pT_eff;
  TProfile* m_eta_eff;	
  TProfile* m_z0_eff;
  TProfile* m_d0_eff;
  TProfile* m_phi_eff;
  TProfile* m_vert_eff;
  TProfile* m_track_eff;
  TProfile* m_invmass_eff;

  ///temp
  TProfile* m_pT_eff_d0cut;
  TProfile* m_eta_eff_d0cut;	
  TProfile* m_z0_eff_d0cut;
  TProfile* m_d0_eff_d0cut;
  TProfile* m_phi_eff_d0cut;
  TProfile* m_vert_eff_d0cut;
  TProfile* m_track_eff_d0cut;
  TProfile* m_invmass_eff_d0cut;

  TProfile* m_pT_eff_offline;
  TProfile* m_eta_eff_offline;	
  TProfile* m_z0_eff_offline;
  TProfile* m_d0_eff_offline;
  TProfile* m_phi_eff_offline;
  TProfile* m_vert_eff_offline;
  TProfile* m_track_eff_offline;
  TProfile* m_invmass_eff_offline;

  ///temp
  TProfile* m_pT_eff_d0cut_offline;
  TProfile* m_eta_eff_d0cut_offline;	
  TProfile* m_z0_eff_d0cut_offline;
  TProfile* m_d0_eff_d0cut_offline;
  TProfile* m_phi_eff_d0cut_offline;
  TProfile* m_vert_eff_d0cut_offline;
  TProfile* m_track_eff_d0cut_offline;
  TProfile* m_invmass_eff_d0cut_offline;

  //TProfile* m_pT_pur;
  //TProfile* m_eta_pur;
  //TProfile* m_phi_pur;
  //TProfile* m_z0_pur;
  //TProfile* m_d0_pur;
  //TProfile* m_dRvspT;
	
  //  TH2F* h2;
  //  TH2F* h2m;
  //  TH2F* h2r;

  /// number of reconstructed tracks
  //int Nreco;

  /// flag to print out the matched tracks etc
  bool m_print, m_isMuon, m_isJpsi, m_keepHistos;

  ///temp
  std::map<unsigned int, const Rec::TrackParticle*> *m_offlineProbes;
  std::map<unsigned int, const Rec::TrackParticle*> *m_offlineTags;
  //std::map<int, double> *m_etaAtCalo, *m_phiAtCalo;
  std::map<int, int> *m_trackRois;
  IHLTMonTool* m_speaker;
  std::vector<TIDA::Vertex*> m_vertices;
  ToolHandle<Trig::TrigDecisionTool>* m_tool;
  const DataHandle<EventInfo> m_eventInfo;
  std::vector<float> *m_pOffDr,     *m_tOffDr,     *m_pOffDeta, *m_pOffDphi,
                     *m_pOffEt,     *m_pOffCharge, *m_tOffCharge;
  ///temp

};
	
#endif  // __JPSIANALYSIS_H 
