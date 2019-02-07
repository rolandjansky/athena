/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ParticleEnergyParametrization_h
#define ParticleEnergyParametrization_h

#include <iostream>

#include <TNamed.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TRandom.h>
#include <TObjArray.h>

#include "FastCaloSim/FastCaloSim_CaloCell_ID.h"
#include "FastCaloSim/ParticleEnergyParametrizationInDistbin.h"
#include "FastCaloSim/ParticleEnergyShape.h"

class FastShowerInfo;

class ParticleEnergyParametrization:public TNamed {
public:
//  typedef std::vector<TH1D*>        pTH1D_vec;
//  typedef std::vector<TVectorD>     TVectorD_vec;
//  typedef std::vector<TMatrixD>     TMatrixD_vec;

  ParticleEnergyParametrization(int id=22,double E=10000,double eta=20);
  ~ParticleEnergyParametrization();
  
  double& weight(int calosample)                             {return m_weights[calosample];};
  double  weight(int calosample) const                       {return m_weights[calosample];};
  double& weight_err(int calosample)                         {return m_weights_err[calosample];};
  double  weight_err(int calosample) const                   {return m_weights_err[calosample];};
  void    set_Ecal_vs_dist(TH2* h);
  void    set_dist_fine(TH1* h)                               {m_h_layer_d_fine=h;};
  void    set_Elayer_vs_dist(int calosample,TH2* h)         {MakeCumulativeX(calosample,h);};
  void    set_mean_vs_dist(int distbin,const TVectorD& mean) {SetTVectorD(DistPara(distbin)->m_mean,mean);};
  void    set_RMS_vs_dist (int distbin,const TVectorD& RMS)  {SetTVectorD(DistPara(distbin)->m_RMS ,RMS );};
  void    set_corr_vs_dist(int distbin,const TMatrixD& cova) {CorelatedGausRandom_corset(cova,DistPara(distbin)->m_corr);};
  
  void    DiceParticle(ParticleEnergyShape& p,TRandom& rand) const;
  void    RepeatDiceParticles(ParticleEnergyShape* p,int n);
  double  GetRandomInBinRange(TRandom& rand, double xmin_in1,double xmax_in2,TH1* in2) const;

  void MakeCumulativeX(int calosample,TH2* h);
  static void CorelatedGausRandom_corset(const TMatrixD& v,TMatrixD& c);
  static void CorelatedGausRandom_corset(const TMatrixD& v,TMatrixD* c) {CorelatedGausRandom_corset(v,*c);};
  static void CorelatedGausRandom_corgen(const TMatrixD& c,TVectorD& x,TRandom& rand,int startidx=-100000);
  
  int    id() const {return m_id;};
  double E() const {return m_E;};
  double eta() const {return m_eta;};
  const TH2*  Ecal_vs_dist() const {return m_Ecal_vs_dist;};
  TH1*  dist_fine() {return m_h_layer_d_fine; };
  
  double  GetDistMin() const {return Ecal_vs_dist()->GetXaxis()->GetXmin();};
  double  GetDistMax() const {return Ecal_vs_dist()->GetXaxis()->GetXmax();};
  int     GetNDistBins() const {return Ecal_vs_dist()->GetNbinsX();};
  
  ParticleEnergyParametrizationInDistbin* DistPara(int distbin) {return (ParticleEnergyParametrizationInDistbin*)m_DistPara.At(distbin);};
  const ParticleEnergyParametrizationInDistbin* DistPara(int distbin) const {return (ParticleEnergyParametrizationInDistbin*)m_DistPara.At(distbin);};
  
  void SetNoDirectoryHisto();
  
  void CopyDebugInfo( FastShowerInfo* fsi ) const;
  
protected:
  static void SetTVectorD(TVectorD& tgt,const TVectorD& src) {tgt.ResizeTo(src.GetLwb(),src.GetUpb());tgt=src;}; 
  static void SetTMatrixD(TMatrixD& tgt,const TMatrixD& src) {tgt.ResizeTo(src.GetRowLwb(),src.GetRowUpb(),src.GetColLwb(),src.GetColUpb());tgt=src;}; 

  int m_id;
  double m_E,m_eta;
  double m_weights[CaloCell_ID_FCS::MaxSample];
  double m_weights_err[CaloCell_ID_FCS::MaxSample];
  TH2*  m_Ecal_vs_dist;
  TH1*  m_h_layer_d_fine;

  TObjArray m_DistPara;
  TObjArray m_Elayer_vs_dist[CaloCell_ID_FCS::MaxSample];

//  TVectorD_vec m_mean;
//  TVectorD_vec m_RMS;
//  TMatrixD_vec m_corr;
  
  ClassDef(ParticleEnergyParametrization,3)  //ParticleEnergyParametrization
};

#endif
