/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloAna_tools_h
#define CaloAna_tools_h

#include "FastCaloSim/FastCaloSim_CaloCell_ID.h"
#include "FastCaloSim/TLateralShapeCorrection.h"

#include <TNamed.h>
#include <TArrayD.h>
#include <TMath.h>
#include <vector>

class TSpline;
class TSpline3;

//using namespace std;

class CaloDetDescrElement;
class FastShowerInfo;

class TShape_Result:public TNamed {
public:
  enum EShapeCorrectionBits {
    kHasShapeFactor = TLateralShapeCorrectionBase::kHasShapeFactor, // set if the shapefactor function is sensible
    kHasCellFactor  = TLateralShapeCorrectionBase::kHasCellFactor   // set if the cellfactor function is sensible
  };
  virtual ~TShape_Result();
protected:
  inline double Sqr(double a) const {return a*a;};
  double   m_fitsplines_EnergyDistribution_maxx;//!
  double   m_fitsplines_EtaPhiAspectRatio_minx;//!
  double   m_fitsplines_EtaPhiAspectRatio_maxx;//!
public:
  double   m_deta;
  double   m_reldeta;
  double   m_reldeta_dist;
  double   m_dphi;
  TSpline* m_fitsplines_EnergyDistribution;
  TSpline* m_fitsplines_EtaPhiAspectRatio;
  int      m_pdgid;
  double   m_energy;
  double   m_eta;
  int      m_calosample;
  double   m_distmin;
  double   m_distmax;
  TArrayD  m_params;
  
  double   m_integral_eta_scale;
  double   m_integral_phi_scale;
  double   m_cutoff_eta;
  double   m_cutoff_phi;
  
  double   m_cellbordersmearing;
  
  double   m_reletascale;
  double   m_cell_width_eta;
  double   m_cell_width_eta1;
  double   m_cell_width_eta2;
  double   m_cell_width_eta3;

  double   m_chi2;
  double   m_errdef;
  int      m_flags;
  
  int      m_nevents;
  double   m_cputime;
  
  std::vector<TLateralShapeCorrectionBase*> m_correction;//! add correction functions to shape
  
  TShape_Result(const char* name="ShapeResult", const char* title="ShapeResult");
  
  double   deta()       const {return m_deta;};
  double   reldeta()    const {return m_reldeta;};
  double   reldeta_dist() const {return m_reldeta_dist;};
  double   dphi()       const {return m_dphi;};
  int      pdgid()      const {return m_pdgid;};
  int      id()         const {return m_pdgid;};
  double   energy()     const {return m_energy;};
  double   E()          const {return m_energy;};
  double   eta()        const {return m_eta;};
  int      calosample() const {return m_calosample;};
  double   distmin()    const {return m_distmin;};
  double   distmax()    const {return m_distmax;};
  double   meandist()   const {return 0.5*(m_distmin+m_distmax);};
  const TArrayD& params() const {return m_params;};
  
  double   integral_eta_scale() const {return m_integral_eta_scale;};
  double   integral_phi_scale() const {return m_integral_phi_scale;};
  double   cutoff_eta() const {return m_cutoff_eta;};
  double   cutoff_phi() const {return m_cutoff_phi;};

  double   cellbordersmearing() const {return m_cellbordersmearing;};

  double   reletascale() const {return m_reletascale;};
  double   cell_width_eta() const {return m_cell_width_eta;};
  double   cell_width_eta1() const {return m_cell_width_eta1;};
  double   cell_width_eta2() const {return m_cell_width_eta2;};
  double   cell_width_eta3() const {return m_cell_width_eta3;};

  double   chi2()        const {return m_chi2;};
  double   errdef()      const {return m_errdef;};
  int      flags()       const {return m_flags;};

  int      nevents()     const {return m_nevents;};
  double   cputime()     const {return m_cputime;};

   
  TSpline* fitsplines_EnergyDistribution() const {return m_fitsplines_EnergyDistribution;};
  TSpline* fitsplines_EtaPhiAspectRatio()  const {return m_fitsplines_EtaPhiAspectRatio;};

  Double_t f_2DSpline_getd(double dxfcx_mm,double dyfcy_mm,int maxiter,double max_aspect);
  static Double_t SplineEval(TSpline3* sp,Double_t x,Double_t xmin,Double_t xmax);
  Double_t f_2DSpline(double dxfcx_mm,double dyfcy_mm) const;
  Double_t CellIntegralEtaPhi(const CaloDetDescrElement& cell,const double eta, const double phi,double& ein_wide,double fcx, double fcy,double direction_factor) const;
  inline double eta_center(double eta,double direction_factor) const {if(eta>0) return eta+m_deta+direction_factor*m_reldeta;else return eta-m_deta+direction_factor*m_reldeta;};
  inline double phi_center(double phi) const {return phi+m_dphi;};
  static inline double eta_jakobi_factor(double fcx) {return TMath::Abs( 2.0*TMath::Exp(-fcx)/(1.0+TMath::Exp(-2*fcx)) );};

  void AddShapeCorrection(TLateralShapeCorrectionBase* shape);
  Bool_t HasShapeFactor() const { return TestBit(kHasShapeFactor); };
  Bool_t HasCellFactor()  const { return TestBit(kHasCellFactor); };

  void SetDebugInfo( const int& sample, FastShowerInfo* fsi ) const;

  ClassDef(TShape_Result,10)  //TShape_Result
};

#endif
