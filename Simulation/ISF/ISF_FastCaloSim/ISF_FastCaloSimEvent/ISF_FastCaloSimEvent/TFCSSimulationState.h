/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSSimulationState_h
#define ISF_FASTCALOSIMEVENT_TFCSSimulationState_h

#include <TObject.h>
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include <vector>

class TFCSSimulationState:public TObject
{
  public:
    TFCSSimulationState();

    bool   is_valid() const {return m_Ebin>=0;};
    double E() const {return m_Etot;};
    double E(int sample)     const {return m_E[sample];};
    double Efrac(int sample) const {return m_Efrac[sample];};
    int    Ebin() const {return m_Ebin;};

    void set_Ebin(int bin) {m_Ebin=bin;};
    void set_E(int sample,double Esample)     { m_E[sample]=Esample; } ;
    void set_Efrac(int sample,double Efracsample) { m_Efrac[sample]=Efracsample; } ;
    void set_E(double E) { m_Etot=E; } ;
    void add_E(int sample,double Esample) { m_E[sample]+=Esample;m_Etot+=Esample; };

    //empty function so far
    //not sure if we should keep the function here or rather write a deposit_cell function or similar
    class t_hit
    {
      public:
      t_hit():m_eta(0),m_phi(0),m_weight(0) {};
      t_hit(float eta, float phi, float weight):m_eta(eta),m_phi(phi),m_weight(weight) {};

      float& eta() {return m_eta;};
      float& phi() {return m_phi;};
      float& weight() {return m_weight;};

      private:
      float m_eta,m_phi,m_weight;
    };
    virtual void deposit_HIT(int sample,const t_hit& hit) {m_hits[sample].push_back(hit);};
    virtual void deposit_HIT(int sample,float hit_eta,float hit_phi,float hit_weight) {m_hits[sample].emplace_back(hit_eta,hit_phi,hit_weight);};

    std::vector< t_hit >& get_hits(int sample) {return m_hits[sample];};
    
    void Print(Option_t *option="") const;

    void clear();
  private:
    int    m_Ebin;
    double m_Etot;
    double m_E[CaloCell_ID_FCS::MaxSample];
    double m_Efrac[CaloCell_ID_FCS::MaxSample];
    
    std::vector< std::vector< t_hit > > m_hits;

  ClassDef(TFCSSimulationState,1)  //TFCSSimulationState
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSSimulationState+;
#endif

#endif
