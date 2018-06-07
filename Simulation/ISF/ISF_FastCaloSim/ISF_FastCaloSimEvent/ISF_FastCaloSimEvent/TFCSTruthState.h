/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSTruthState_h
#define ISF_FASTCALOSIMEVENT_TFCSTruthState_h

#include <TLorentzVector.h>

class TFCSTruthState:public TLorentzVector {
  public:
    TFCSTruthState();
    TFCSTruthState(Double_t x, Double_t y, Double_t z, Double_t t, int pdgid);

    void set_pdgid(int val) {m_pdgid=val;};
    void set_vertex(const TLorentzVector& val) {m_vertex=val;};
    void set_vertex(Double_t x, Double_t y, Double_t z, Double_t t=0) {m_vertex.SetXYZT(x,y,z,t);};

    int    pdgid() const {return m_pdgid;};
    double Ekin() const {return E()-M();};
    const TLorentzVector& vertex() const {return m_vertex;};
  private:
    int m_pdgid;
    TLorentzVector m_vertex;

  ClassDef(TFCSTruthState,1)  //TFCSTruthState
};

#endif
