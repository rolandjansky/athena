/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TLateralShapeCorrection_h
#define TLateralShapeCorrection_h

#include <TNamed.h>
#include <string>

class TCanvas;

class TLateralShapeCorrectionBase:public TNamed {
protected:
  virtual std::string prefix() const {return "ShapeCorrection";};
  virtual void SetStandardName();
  
public:
  enum EShapeCorrectionBits {
    kHasShapeFactor = BIT(14), // set if the shapefactor function is sensible
    kHasCellFactor  = BIT(15)  // set if the cellfactor function is sensible
  };

  TLateralShapeCorrectionBase();
  
  virtual bool is_match(int /*pdgid*/,int /*calosample*/,float /*p*/,float /*eta*/,float /*depth*/) const {return false;};

  // gives an energy reweighting factor to the shape as function of the distance in eta and phi 
  // from the center of the shape function. deta and dphi can be measured in mm and in cell units.
  virtual float shapefactor(float /*deta_mm*/,float /*dphi_mm*/,float /*deta_cellunits*/,float /*dphi_cellunits*/) const {return 1.0;};

  // gives an energy reweighting factor to the cells as function of the distance in eta and phi 
  // from the hit cell. deta==0 and dphi==0 corresponds to the hit cell
  virtual float cellfactor(int /*deta*/,int /*dphi*/) const {return 1.0;};
  
  virtual std::string str() const {return prefix();};
  virtual std::string gen_name() const {return prefix();};

  virtual TCanvas* DrawCorr() const {return 0;};
  
  Bool_t HasShapeFactor() const { return TestBit(kHasShapeFactor); };
  Bool_t HasCellFactor()  const { return TestBit(kHasCellFactor); };

  ClassDef(TLateralShapeCorrectionBase,1)  //TLateralShapeCorrectionBase
};

class TLateralShapeCorrection:public TLateralShapeCorrectionBase {
protected:
  int m_pdgid;
  int m_calosample;
  float m_min_eta,m_max_eta;
  float m_min_p,m_max_p;
  float m_min_pT,m_max_pT;
public:
  TLateralShapeCorrection();

  virtual bool is_match(int pdgid,int calosample,float p,float eta,float depth) const;
  virtual std::string str() const;
  virtual std::string gen_name() const;
  
  void set_pdgid(int pdgid) {m_pdgid=pdgid;SetStandardName();};
  void set_calosample(int calosample) {m_calosample=calosample;SetStandardName();};
  void set_eta_range(float min_eta,float max_eta) {m_min_eta=min_eta,m_max_eta=max_eta;SetStandardName();};
  void set_p_range(float min_p,float max_p) {m_min_p=min_p,m_max_p=max_p;SetStandardName();};
  void set_pT_range(float min_pT,float max_pT) {m_min_pT=min_pT,m_max_pT=max_pT;SetStandardName();};

  ClassDef(TLateralShapeCorrection,1)  //TLateralShapeCorrection
};

#endif
