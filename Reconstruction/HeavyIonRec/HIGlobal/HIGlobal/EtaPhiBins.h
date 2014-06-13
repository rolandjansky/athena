/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef HEAVYIONREC_ETAPHIBINS_H
#define HEAVYIONREC_ETAPHIBINS_H 1

#include "CLIDSvc/CLASS_DEF.h"
#include "TH2D.h"
#include "Particle/TrackParticleContainer.h"
#include "EventKernel/INavigable4Momentum.h"

class EtaPhiBins
{
 public:
  EtaPhiBins(const char* name, Float_t eta_min,Float_t eta_max,Int_t netabins,Int_t nphibins);
  EtaPhiBins(void) {};
  ~EtaPhiBins(void) {} ;

  Float_t GetDeta();
  Float_t GetDphi();

  TH2F& GetH2() {return m_H2;}
  Float_t Integral(){return m_H2.Integral();}
  TH1D* ProjectionEta(){return m_H2.ProjectionX();}
  TH1D* ProjectionPhi(){return m_H2.ProjectionY();}
  void  Fill(Rec::TrackParticleContainer*);
  void  Fill(DataVector<INavigable4Momentum>*);
  void  Fill(float eta, float phi);
  float GetBinContent(int ieta, int iphi) const;

 private:
  Float_t m_EtaMin;
  Float_t m_EtaMax;
  Int_t m_NEtaBins;
  Int_t m_NPhiBins;

  TH2F m_H2;

};

CLASS_DEF( EtaPhiBins , 260373593 , 1 )

#endif
