/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSSimulationState_h
#define ISF_FASTCALOSIMEVENT_TFCSSimulationState_h

#include <TObject.h>
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

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
    virtual void deposit_HIT(int sample,double hit_eta,double hit_phi,double hit_weight);

    void clear();
  private:
    int    m_Ebin;
    double m_Etot;
    double m_E[CaloCell_ID_FCS::MaxSample];
    double m_Efrac[CaloCell_ID_FCS::MaxSample];

  ClassDef(TFCSSimulationState,1)  //TFCSSimulationState
};

#endif
