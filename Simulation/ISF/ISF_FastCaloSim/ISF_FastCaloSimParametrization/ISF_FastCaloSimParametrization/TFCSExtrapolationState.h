/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSExtrapolationState_h
#define TFCSExtrapolationState_h

#include <TObject.h>
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"

class TFCSExtrapolationState:public TObject {
  public:
    TFCSExtrapolationState();

    void clear();
    
    enum SUBPOS { SUBPOS_MID = 0, SUBPOS_ENT = 1, SUBPOS_EXT = 2}; //MID=middle, ENT=entrance, EXT=exit of cal layer

    void set_OK (int layer,int subpos,bool   val) {m_CaloOK[layer][subpos]=val;};
    void set_eta(int layer,int subpos,double val) {m_etaCalo[layer][subpos]=val;};
    void set_phi(int layer,int subpos,double val) {m_phiCalo[layer][subpos]=val;};
    void set_r  (int layer,int subpos,double val) {m_rCalo[layer][subpos]=val;};
    void set_z  (int layer,int subpos,double val) {m_zCalo[layer][subpos]=val;};
    void set_d  (int layer,int subpos,double val) {m_dCalo[layer][subpos]=val;};
    void set_detaBorder(int layer,int subpos,double val) {m_distetaCaloBorder[layer][subpos]=val;};
    void set_IDCaloBoundary_eta(double val) {m_IDCaloBoundary_eta=val;};
    void set_IDCaloBoundary_phi(double val) {m_IDCaloBoundary_phi=val;};
    void set_IDCaloBoundary_r(double val) {m_IDCaloBoundary_r=val;};
    void set_IDCaloBoundary_z(double val) {m_IDCaloBoundary_z=val;};

    bool   OK(int layer,int subpos) const {return m_CaloOK[layer][subpos];};
    double eta(int layer,int subpos) const {return m_etaCalo[layer][subpos];};
    double phi(int layer,int subpos) const {return m_phiCalo[layer][subpos];};
    double r(int layer,int subpos) const {return m_rCalo[layer][subpos];};
    double z(int layer,int subpos) const {return m_zCalo[layer][subpos];};
    double d(int layer,int subpos) const {return m_dCalo[layer][subpos];};
    double detaBorder(int layer,int subpos) const {return m_distetaCaloBorder[layer][subpos];};

    double IDCaloBoundary_eta() const {return m_IDCaloBoundary_eta;};
    double IDCaloBoundary_phi() const {return m_IDCaloBoundary_phi;};
    double IDCaloBoundary_r() const {return m_IDCaloBoundary_r;};
    double IDCaloBoundary_z() const {return m_IDCaloBoundary_z;};

  private:
    bool   m_CaloOK[CaloCell_ID_FCS::MaxSample][3];
    double m_etaCalo[CaloCell_ID_FCS::MaxSample][3];
    double m_phiCalo[CaloCell_ID_FCS::MaxSample][3];
    double m_rCalo[CaloCell_ID_FCS::MaxSample][3];
    double m_zCalo[CaloCell_ID_FCS::MaxSample][3];
    double m_dCalo[CaloCell_ID_FCS::MaxSample][3];
    double m_distetaCaloBorder[CaloCell_ID_FCS::MaxSample][3];
    
    double m_IDCaloBoundary_eta;
    double m_IDCaloBoundary_phi;
    double m_IDCaloBoundary_r;
    double m_IDCaloBoundary_z;

  ClassDef(TFCSExtrapolationState,1)  //TFCSExtrapolationState
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSExtrapolationState;
#endif

#endif
