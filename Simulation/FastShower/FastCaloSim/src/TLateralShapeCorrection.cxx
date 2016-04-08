/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/TLateralShapeCorrection.h"
#include "TMath.h"
#include "TCanvas.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

//=============================================
//======= TLateralShapeCorrectionBase =========
//=============================================

TLateralShapeCorrectionBase::TLateralShapeCorrectionBase():TNamed("ShapeCorrection","ShapeCorrection")
{
}

void TLateralShapeCorrectionBase::SetStandardName()
{
  SetName(gen_name().c_str());
  SetTitle(str().c_str());
}

//=============================================
//========= TLateralShapeCorrection ===========
//=============================================

TLateralShapeCorrection::TLateralShapeCorrection():TLateralShapeCorrectionBase(),m_pdgid(0),m_calosample(0),m_min_eta(0),m_max_eta(0),m_min_p(0),m_max_p(0),m_min_pT(0),m_max_pT(0)
{
}

bool TLateralShapeCorrection::is_match(int pdgid,int calosample,float p,float eta,float /*depth*/) const
{
  if(m_pdgid!=0 && m_pdgid!=pdgid) return false;
  if(m_calosample!=calosample) return false;
  
  if(eta< m_min_eta) return false;
  if(eta>=m_max_eta) return false;
  
  if(  p< m_min_p)   return false;
  if(  p>=m_max_p)   return false;
  
  float pT=p/TMath::CosH(eta);
  if( pT< m_min_pT)  return false;
  if( pT>=m_max_pT)  return false;
  
  return true;
}

std::string TLateralShapeCorrection::str() const
{
  stringstream out;
  out<<prefix();
  out<<" ID="<<m_pdgid<<" calosample="<<m_calosample;
  out<<"; "<<m_min_eta<<"<eta<"<<m_max_eta;
  out<<"; "<<m_min_p<<"< p <"<<m_max_p;
  out<<"; "<<m_min_pT<<"< pT<"<<m_max_pT;
  return out.str();
}

std::string to_nice_str(double x)
{
  stringstream out;
  if(x<0) out<<"m"<<fabs(x);
   else out<<"p"<<fabs(x);
  return out.str();
}

std::string to_nice_str(int x)
{
  stringstream out;
  if(x<0) out<<"m"<<-x;
   else out<<"p"<<x;
  return out.str();
}

std::string TLateralShapeCorrection::gen_name() const
{
  stringstream out;
  out<<prefix();
  out<<"_ID"<<to_nice_str(m_pdgid)<<"_cs"<<m_calosample;
  out<<"_mineta_"<<to_nice_str((int)(m_min_eta*100))<<"_maxeta_"<<to_nice_str((int)(m_max_eta*100));
  out<<"_minp_"  <<to_nice_str((int) m_min_p  )     <<"_maxp_"  <<to_nice_str((int) m_max_p  );
  out<<"_minpT_" <<to_nice_str((int) m_min_pT )     <<"_maxpT_" <<to_nice_str((int) m_max_pT );
  return out.str();
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TLateralShapeCorrectionBase)

ClassImp(TLateralShapeCorrection)
