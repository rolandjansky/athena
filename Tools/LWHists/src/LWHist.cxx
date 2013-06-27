/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWHist                            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/LWHist.h"
#include "LWHists/LWHist1D.h"
#include "LWHists/LWHist2D.h"
#include "LWHists/LWHistControls.h"
#include "LWHists/LWHistStats.h"
#include "LWPools.h"
#include "LWStrUtils.h"
#include "LWBinLabels.h"
#include "TH1.h"
#include <cassert>
#include <cstring>
#include <cmath>

//____________________________________________________________________
void LWHist::safeDelete(LWHist*h)
{
  if (!h) {
    //std::cout<<"LWHist::safeDelete ERROR: Called with null pointer!"<<std::endl;
    return;
  }
  if (h->m_nBytesFromPool) {
    h->~LWHist();
    LWPools::release(reinterpret_cast<char*>(h),h->m_nBytesFromPool);
    if (LWHistStats::s_nActiveHists==0) {
      std::cout<<"LWHists INFO: Last active histogram deleted. Triggering complete pool cleanup."<<std::endl;
      LWHistControls::releaseAllHeldMemory();
    }
  } else {
    delete h;
  }
}

#define DEFAULT_MARKERSIZE 1
#define DEFAULT_MARKERCOLOR 1
#define DEFAULT_MARKERSTYLE 1
#define DEFAULT_MINIMUM -1111
#define DEFAULT_MAXIMUM -1111
class LWHist::LWHistDecorations {
public:
  LWHistDecorations() :
    m_option(0),
    m_xAxis(0),
    m_yAxis(0),
    m_zAxis(0),
    m_minimum(DEFAULT_MINIMUM),
    m_maximum(DEFAULT_MAXIMUM),
    m_markerSize(DEFAULT_MARKERSIZE),
    m_markerColor(DEFAULT_MARKERCOLOR),
    m_markerStyle(DEFAULT_MARKERSTYLE) {}
  ~LWHistDecorations()
  {
    LWStrUtils::releaseString(m_option);
    MP_DELETE(m_xAxis);
    MP_DELETE(m_yAxis);
    MP_DELETE(m_zAxis);
  }

  char * m_option;
  LWHist::LWHistAxis * m_xAxis;
  LWHist::LWHistAxis * m_yAxis;
  LWHist::LWHistAxis * m_zAxis;
  float m_minimum;
  float m_maximum;
  float m_markerSize;
  short m_markerColor;
  short m_markerStyle;

private:
  LWHistDecorations( const LWHistDecorations & );
  LWHistDecorations & operator= ( const LWHistDecorations & );

};

//____________________________________________________________________
void LWHist::ensureInitDecorations()
{
  if (!m_decorations)
    m_decorations = usingROOTBackend() ? new LWHistDecorations : MP_NEW(LWHistDecorations);
}

short LWHist::GetMarkerColor() const
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot)
    return hroot->GetMarkerColor();

  return m_decorations ? m_decorations->m_markerColor : DEFAULT_MARKERCOLOR;
}

short LWHist::GetMarkerStyle() const
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot)
    return hroot->GetMarkerStyle();
  return m_decorations ? m_decorations->m_markerStyle : DEFAULT_MARKERSTYLE;
}

float LWHist::GetMarkerSize() const
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot)
    return hroot->GetMarkerSize();
  return m_decorations ? m_decorations->m_markerSize : DEFAULT_MARKERSIZE;
}

void LWHist::SetMarkerColor( short c )
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetMarkerColor(c);
    return;
  }
  if (!m_decorations&&c==DEFAULT_MARKERSTYLE)
    return;
  ensureInitDecorations();
  m_decorations->m_markerColor = c;
}
void LWHist::SetMarkerStyle( short s )
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetMarkerStyle(s);
    assert(GetMarkerStyle()==s);
    return;
  }
  if (!m_decorations&&s==DEFAULT_MARKERSTYLE) {
    assert(GetMarkerStyle()==s);
    return;
  }
  ensureInitDecorations();
  m_decorations->m_markerStyle = s;
  assert(GetMarkerStyle()==s);
}

void LWHist::SetMarkerSize( float sz ) {
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetMarkerSize(sz);
    return;
  }
  if (!m_decorations&&sz==DEFAULT_MARKERSIZE)
    return;
  ensureInitDecorations();
  m_decorations->m_markerSize = sz;
}

void LWHist::SetMinimum(const double& m )
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetMinimum(m);
    return;
  }
  if (!m_decorations&&fabs(m-DEFAULT_MINIMUM)<1.0e-5)
    return;
  ensureInitDecorations();
  m_decorations->m_minimum = m;
}

void LWHist::SetMaximum(const double& m )
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetMaximum(m);
    return;
  }
  if (!m_decorations&&fabs(m-DEFAULT_MAXIMUM)<1.0e-5)
    return;
  ensureInitDecorations();
  m_decorations->m_maximum = m;
}

//____________________________________________________________________
LWHist::LWHist( const char* n,
		const char* t,
		bool rootbackend )
  : m_nBytesFromPool(0),
    m_usingROOTBackend(rootbackend),
    m_ownsRootHisto(true),
    m_name(0),
    m_title(0),
    m_decorations(0),
    m_customData(0)
{
  //Fixme: Sanity check of input parameters
  assert(typeid(short)==typeid(Color_t));
  assert(typeid(short)==typeid(Style_t));
  assert(typeid(float)==typeid(Size_t));
  if (!m_usingROOTBackend) {
    LWStrUtils::setStringFromInput(n,m_name);
    LWStrUtils::setStringFromInput(t,m_title);
  }
  ++LWHistStats::s_nActiveHists;
  static bool first = true;
  if (first) {
    first = false;
    std::cout<<"LWHists INFO: Using light weight histograms version "<<PACKAGE_VERSION<<std::endl;
  }
}

//____________________________________________________________________
LWHist::~LWHist()
{
  clear();
  --LWHistStats::s_nActiveHists;
}

//____________________________________________________________________
void LWHist::SetName(const char*n)
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetName(n);
    return;
  }
  LWStrUtils::setStringFromInput(n,m_name);
}

//____________________________________________________________________
void LWHist::SetTitle(const char*t)
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetTitle(t);
    return;
  }
  LWStrUtils::setStringFromInput(t,m_title);
}

//____________________________________________________________________
void LWHist::SetNameTitle(const char*n,const char*t)
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetNameTitle(n,t);
    return;
  }
  LWStrUtils::setStringFromInput(n,m_name);
  LWStrUtils::setStringFromInput(t,m_title);
}

//____________________________________________________________________
bool LWHist::apply(TH1*h) const
{
  if (!h)
    return false;
  assert(!m_usingROOTBackend);

  if (!LWStrUtils::stringsEqual(h->GetTitle(),GetTitle()))
    h->SetTitle(GetTitle());

  if (m_decorations&&m_decorations->m_xAxis) {
    TAxis * a = h->GetXaxis();
    assert(a);
    if (m_decorations->m_xAxis->m_title)
      a->SetTitle(m_decorations->m_xAxis->m_title);
    if (m_decorations->m_xAxis->m_labelSize!=a->GetLabelSize())
      a->SetLabelSize(m_decorations->m_xAxis->m_labelSize);
    if (m_decorations->m_xAxis->m_binLabels)
      m_decorations->m_xAxis->m_binLabels->apply(a);
  }
  if (m_decorations&&m_decorations->m_yAxis) {
    TAxis * a = h->GetYaxis();
    assert(a);
    if (m_decorations->m_yAxis->m_title)
      a->SetTitle(m_decorations->m_yAxis->m_title);
    if (m_decorations->m_yAxis->m_labelSize!=a->GetLabelSize())
      a->SetLabelSize(m_decorations->m_yAxis->m_labelSize);
    if (m_decorations->m_yAxis->m_binLabels)
      m_decorations->m_yAxis->m_binLabels->apply(a);
  }
  if (m_decorations&&m_decorations->m_zAxis) {
    TAxis * a = h->GetZaxis();
    assert(a);
    if (m_decorations->m_zAxis->m_title)
      a->SetTitle(m_decorations->m_zAxis->m_title);
    if (m_decorations->m_zAxis->m_labelSize!=a->GetLabelSize())
      a->SetLabelSize(m_decorations->m_zAxis->m_labelSize);
    if (m_decorations->m_zAxis->m_binLabels)
      m_decorations->m_zAxis->m_binLabels->apply(a);
  }
  h->SetMarkerColor( GetMarkerColor() );
  h->SetMarkerStyle( GetMarkerStyle() );
  h->SetMarkerSize( GetMarkerSize() );
  if (m_decorations&&fabs(m_decorations->m_minimum-DEFAULT_MINIMUM)>1.0e-5)
    h->SetMinimum(m_decorations->m_minimum);
  if (m_decorations&&fabs(m_decorations->m_maximum-DEFAULT_MAXIMUM)>1.0e-5)
    h->SetMaximum(m_decorations->m_maximum);
  if (m_decorations&&m_decorations->m_option)
    h->SetOption(m_decorations->m_option);

  return true;
}

//____________________________________________________________________
LWHist::LWHistAxis * LWHist::GetXaxis() const
{
  const_cast<LWHist *>(this)->ensureInitDecorations();
  if (!m_decorations->m_xAxis)
    const_cast<LWHist *>(this)->m_decorations->m_xAxis = usingROOTBackend() ? new LWHistAxis(this) : MP_NEW(LWHistAxis)(this);

  return m_decorations->m_xAxis;
}

//____________________________________________________________________
LWHist::LWHistAxis * LWHist::GetYaxis() const
{
  const_cast<LWHist *>(this)->ensureInitDecorations();
  if (!m_decorations->m_yAxis)
    const_cast<LWHist *>(this)->m_decorations->m_yAxis = usingROOTBackend() ? new LWHistAxis(this) : MP_NEW(LWHistAxis)(this);
  return m_decorations->m_yAxis;
}

//____________________________________________________________________
LWHist::LWHistAxis * LWHist::GetZaxis() const
{
  const_cast<LWHist *>(this)->ensureInitDecorations();
  if (!m_decorations->m_zAxis)
    const_cast<LWHist *>(this)->m_decorations->m_zAxis = usingROOTBackend() ? new LWHistAxis(this) : MP_NEW(LWHistAxis)(this);
  return m_decorations->m_zAxis;
}

//Axis implementation:
//Constructor and destructor should do nothing due to our use of the mem-pool.
LWHist::LWHistAxis::LWHistAxis(const LWHist *hist) : m_hist(hist), m_labelSize(0.035),
						     m_title(0),m_binLabels(0) {}
TAxis * LWHist::LWHistAxis::rootAxis() const
{
  TH1*hroot=m_hist->getROOTHistBaseNoAlloc();
  return hroot ? (isXAxis() ? hroot->GetXaxis() : (isYAxis() ? hroot->GetYaxis():hroot->GetZaxis())) : 0;
}

void LWHist::LWHistAxis::SetLabelSize(float s)
{
  TAxis*a=rootAxis();
  if (a) a->SetLabelSize(s);
  else m_labelSize = s;
}

void LWHist::LWHistAxis::SetTitle(const char*t)
{
  TAxis*a=rootAxis();
  if (a) a->SetTitle(t);
  else LWStrUtils::setStringFromInput(t,m_title);
}

const char* LWHist::GetName() const
{
  TH1 * hroot = getROOTHistBaseNoAlloc();
  return hroot ? hroot->GetName() : m_name;
}

const char* LWHist::GetTitle() const
{
  TH1 * hroot = getROOTHistBaseNoAlloc();
  return hroot ? hroot->GetTitle() : m_title;
}


LWHist::LWHistAxis::~LWHistAxis() { LWStrUtils::releaseString(m_title); MP_DELETE(m_binLabels); }
bool LWHist::LWHistAxis::isXAxis() const { return this==m_hist->m_decorations->m_xAxis; }
bool LWHist::LWHistAxis::isYAxis() const { return this==m_hist->m_decorations->m_yAxis; }

unsigned LWHist::LWHistAxis::GetNbins() const
{
  TAxis*a=rootAxis();
  if (a)
    return a->GetNbins();
  return isXAxis() ? m_hist->actualGetNBinsX() : (isYAxis()?m_hist->actualGetNBinsY():0);
}

double LWHist::LWHistAxis::GetBinCenter(int bin) const
{
  TAxis*a=rootAxis();
  if (a)
    return a->GetBinCenter(bin);
  return isXAxis()
    ? m_hist->actualGetBinCenterX(bin)
    : (isYAxis()?m_hist->actualGetBinCenterY(bin):0.0);
}

const char * LWHist::LWHistAxis::GetBinLabel(unsigned bin) const
{
  TAxis*a=rootAxis();
  if (a)
    return a->GetBinLabel(bin);
  if (!m_binLabels)
    return "";
  return m_binLabels->getBinLabel(bin);
}

void LWHist::LWHistAxis::SetBinLabel(unsigned bin, const char* label)
{
  TAxis*a=rootAxis();
  if (a) {
    a->SetBinLabel(bin,label);
    return;
  }
  if (!m_binLabels)
    m_binLabels = MP_NEW(LWBinLabels);
  m_binLabels->setBinLabel(bin,label);
}

unsigned LWHist::LWHistAxis::FindBin(const double& x)
{
  TAxis*a=rootAxis();
  if (a)
    return a->FindBin(x);
  return isXAxis() ? m_hist->actualFindBinX(x) : (isYAxis()?m_hist->actualFindBinY(x):0);
}

void LWHist::SetXTitle(const char *t)
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetXTitle(t);
    return;
  }
  GetXaxis()->SetTitle(t);
}
void LWHist::SetYTitle(const char *t)
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetYTitle(t);
    return;
  }
  GetYaxis()->SetTitle(t);
}
void LWHist::SetZTitle(const char *t)
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetZTitle(t);
    return;
  }
  GetZaxis()->SetTitle(t);
}

//____________________________________________________________________
void LWHist::SetOption(const char* option)
{
  TH1 * hroot= getROOTHistBaseNoAlloc();
  if (hroot) {
    hroot->SetOption(option);
    return;
  }
  ensureInitDecorations();
  LWStrUtils::setStringFromInput(option,m_decorations->m_option);
}



//____________________________________________________________________
void LWHist::clear()
{
  if (m_decorations) {
    if (usingROOTBackend()) {
      delete m_decorations->m_xAxis;
      m_decorations->m_xAxis=0;
      delete m_decorations->m_yAxis;
      m_decorations->m_yAxis=0;
      delete m_decorations->m_zAxis;
      m_decorations->m_zAxis=0;
      delete m_decorations;
    } else {
      MP_DELETE(m_decorations);
    }
    m_decorations = 0;
  }
  LWStrUtils::releaseString(m_name);
  LWStrUtils::releaseString(m_title);
}
