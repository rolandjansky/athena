/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/TSplineReweight.h"
#include "TSpline.h"
#include "TMath.h"
#include "TCanvas.h"
#include <iostream>
#include <sstream>

using namespace std;

//=============================================
//========= TSplineReweight ===========
//=============================================

TSplineReweight::TSplineReweight():TLateralShapeCorrection(),m_spline(0)
{
  SetBit(kHasShapeFactor);
}

TSplineReweight::~TSplineReweight()
{
  SetSpline(0);
}

void TSplineReweight::SetSpline(TSpline3* spline) 
{
  if(m_spline) delete m_spline;
  if(spline) m_spline=(TSpline3*)spline->Clone(gen_name().c_str());
   else m_spline=0;
  SetStandardName(); 
}

std::string TSplineReweight::str() const
{
  stringstream out;
  out<<TLateralShapeCorrection::str();
  out<<"; #=";
  if(m_spline) out<<m_spline->GetNp();
   else out<<0;
  
  return out.str();
}

TCanvas* TSplineReweight::DrawCorr() const
{
  TCanvas* c=new TCanvas(gen_name().c_str(),str().c_str());
  m_spline->Draw();
  
  return c;
}

float TSplineReweight::Eval(float x) const
{
  if(!GetSpline()) return 1.0;
  if(x<GetSpline()->GetXmin()) {
    return GetSpline()->Eval(GetSpline()->GetXmin());
  }
  if(x>GetSpline()->GetXmax()) {
    return GetSpline()->Eval(GetSpline()->GetXmax());
  }
  return GetSpline()->Eval(x);
}

//=============================================
//==== TLateralShapeCorrectionEtaSplineReweight ====
//=============================================

float TEtaSplineReweight::shapefactor(float /*deta_mm*/,float /*dphi_mm*/,float deta_cellunits,float /*dphi_cellunits*/) const
{
  return Eval(deta_cellunits);
}

//=============================================
//==== TLateralShapeCorrectionPhiSplineReweight ====
//=============================================

float TPhiSplineReweight::shapefactor(float /*deta_mm*/,float /*dphi_mm*/,float /*deta_cellunits*/,float dphi_cellunits) const
{
  return Eval(dphi_cellunits);
}

//=============================================
//==== TLateralShapeCorrectionRSplineReweight ====
//=============================================

float TRSplineReweight::shapefactor(float /*deta_mm*/,float /*dphi_mm*/,float deta_cellunits,float dphi_cellunits) const
{
  float dR_cellunits=TMath::Sqrt(deta_cellunits*deta_cellunits + dphi_cellunits*dphi_cellunits);
  return Eval(dR_cellunits);
}

//=============================================
//==== TLateralShapeCorrectionMetricEtaSplineReweight ====
//=============================================

float TMetricEtaSplineReweight::shapefactor(float deta_mm,float /*dphi_mm*/,float /*deta_cellunits*/,float /*dphi_cellunits*/) const
{
  return Eval(deta_mm);
}

//=============================================
//==== TLateralShapeCorrectionMetricPhiSplineReweight ====
//=============================================

float TMetricPhiSplineReweight::shapefactor(float /*deta_mm*/,float dphi_mm,float /*deta_cellunits*/,float /*dphi_cellunits*/) const
{
  return Eval(dphi_mm);
}

//=============================================
//==== TLateralShapeCorrectionMetricRSplineReweight ====
//=============================================

float TMetricRSplineReweight::shapefactor(float deta_mm,float dphi_mm,float /*deta_cellunits*/,float /*dphi_cellunits*/) const
{
  float dR_mm=TMath::Sqrt(deta_mm*deta_mm + dphi_mm*dphi_mm);
  return Eval(dR_mm);
}


//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TSplineReweight)

ClassImp(TEtaSplineReweight)

ClassImp(TPhiSplineReweight)

ClassImp(TRSplineReweight)

ClassImp(TMetricEtaSplineReweight)

ClassImp(TMetricPhiSplineReweight)

ClassImp(TMetricRSplineReweight)
