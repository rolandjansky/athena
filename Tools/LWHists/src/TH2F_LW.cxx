/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TH2F_LW                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/TH2F_LW.h"
#include "LWHists/LWHistControls.h"
#include "Flex2DHisto.h"
#include "LWHistRootUtils.h"
#include "TH2F.h"
#include <iostream>

#define FLEXHIST() ((reinterpret_cast<Flex2DHisto<float>*>(m_flexHisto)))
#define CALL(x,y) m_rootHisto ? m_rootHisto-> x : reinterpret_cast<Flex2DHisto<float>*>(m_flexHisto)-> y
#ifdef LW_DEBUG_HEAVY_USERS
#define LWCOUNTCALL {if (m_flexHisto) FLEXHIST()->countCall(LWHistTraceUtils::getAdresses());};
#define LWREPORT {if (m_flexHisto) FLEXHIST()->produceReport(GetName());};
#else
#define LWCOUNTCALL {};
#define LWREPORT {};
#endif

//____________________________________________________________________
TH2F_LW * TH2F_LW::create( const char* name, const char* title,
			   unsigned nbinsx, const double& xmin, const double& xmax,
			   unsigned nbinsy, const double& ymin, const double& ymax )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH2F_LW * h;
  if (rootbackend) {
    h = new TH2F_LW(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax,rootbackend);
  } else {
    h = MP_NEW(TH2F_LW)(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax,rootbackend);
    h->m_nBytesFromPool = sizeof(TH2F_LW);
  }
  return h;
}

//____________________________________________________________________
TH2F_LW * TH2F_LW::create( const char* name, const char* title,
			   unsigned nbinsx, const double* xbins,
			   unsigned nbinsy, const double& ymin, const double& ymax )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH2F_LW * h;
  if (rootbackend) {
    h = new TH2F_LW(name,title,nbinsx,xbins,nbinsy,ymin,ymax,rootbackend);
  } else {
    h = MP_NEW(TH2F_LW)(name,title,nbinsx,xbins,nbinsy,ymin,ymax,rootbackend);
    h->m_nBytesFromPool = sizeof(TH2F_LW);
  }
  return h;
}

//____________________________________________________________________
TH2F_LW * TH2F_LW::create( const char* name, const char* title,
			   unsigned nbinsx, const double& xmin, const double& xmax,
			   unsigned nbinsy, const double* ybins )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH2F_LW * h;
  if (rootbackend) {
    h = new TH2F_LW(name,title,nbinsx,xmin,xmax,nbinsy,ybins,rootbackend);
  } else {
    h = MP_NEW(TH2F_LW)(name,title,nbinsx,xmin,xmax,nbinsy,ybins,rootbackend);
    h->m_nBytesFromPool = sizeof(TH2F_LW);
  }
  return h;
}

//____________________________________________________________________
TH2F_LW * TH2F_LW::create( const char* name, const char* title,
			   unsigned nbinsx, const double* xbins,
			   unsigned nbinsy, const double* ybins )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH2F_LW * h;
  if (rootbackend) {
    h = new TH2F_LW(name,title,nbinsx,xbins,nbinsy,ybins,rootbackend);
  } else {
    h = MP_NEW(TH2F_LW)(name,title,nbinsx,xbins,nbinsy,ybins,rootbackend);
    h->m_nBytesFromPool = sizeof(TH2F_LW);
  }
  return h;
}

//____________________________________________________________________
TH2F_LW * TH2F_LW::create( const char* name, const char* title,
			   unsigned nbinsx, const float* xbins,
			   unsigned nbinsy, const float* ybins )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH2F_LW * h;
  if (rootbackend) {
    h = new TH2F_LW(name,title,nbinsx,xbins,nbinsy,ybins,rootbackend);
  } else {
    h = MP_NEW(TH2F_LW)(name,title,nbinsx,xbins,nbinsy,ybins,rootbackend);
    h->m_nBytesFromPool = sizeof(TH2F_LW);
  }
  return h;
}

//____________________________________________________________________
TH2F_LW::TH2F_LW( const char* name, const char* title,
		  unsigned nbinsx, const double& xmin, const double& xmax,
		  unsigned nbinsy, const double& ymin, const double& ymax, bool rootbackend )
  : LWHist2D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH2F(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  } else {
    m_flexHisto = Flex2DHisto<float>::create(nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  }
}

//____________________________________________________________________
TH2F_LW::TH2F_LW( const char* name, const char* title,
		  unsigned nbinsx, const double* xbins,
		  unsigned nbinsy, const double& ymin, const double& ymax, bool rootbackend )
  : LWHist2D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH2F(name,title,nbinsx,xbins,nbinsy,ymin,ymax);
  } else {
    m_flexHisto = Flex2DHisto<float>::create(nbinsx,xbins,nbinsy,ymin,ymax);
  }
}

//____________________________________________________________________
TH2F_LW::TH2F_LW( const char* name, const char* title,
		  unsigned nbinsx, const double& xmin, const double& xmax,
		  unsigned nbinsy, const double* ybins, bool rootbackend )
  : LWHist2D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH2F(name,title,nbinsx,xmin,xmax,nbinsy,ybins);
  } else {
    m_flexHisto = Flex2DHisto<float>::create(nbinsx,xmin,xmax,nbinsy,ybins);
  }
}

//____________________________________________________________________
TH2F_LW::TH2F_LW( const char* name, const char* title,
		  unsigned nbinsx, const double* xbins,
		  unsigned nbinsy, const double* ybins, bool rootbackend )
  : LWHist2D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH2F(name,title,nbinsx,xbins,nbinsy,ybins);
  } else {
    m_flexHisto = Flex2DHisto<float>::create(nbinsx,xbins,nbinsy,ybins);
  }
}

//____________________________________________________________________
TH2F_LW::TH2F_LW( const char* name, const char* title,
		  unsigned nbinsx, const float* xbins,
		  unsigned nbinsy, const float* ybins, bool rootbackend )
  : LWHist2D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH2F(name,title,nbinsx,xbins,nbinsy,ybins);
  } else {
    m_flexHisto = Flex2DHisto<float>::create(nbinsx,xbins,nbinsy,ybins);
  }
}

//____________________________________________________________________
TH2F_LW::~TH2F_LW()
{
  clearKeptROOTHist();
  clear();
  if (ownsROOTHisto())
    delete m_rootHisto;//for root backend
}

//____________________________________________________________________
TH1* TH2F_LW::getROOTHistBaseNoAlloc() const
{
  return m_rootHisto;
}
TH1* TH2F_LW::getROOTHistBase() { return getROOTHist(); }
TH2F* TH2F_LW::getROOTHist()
{
  if (!m_rootHisto) {
    TH2F * h = LWHistRootUtils::createRootHisto<float,TH2F_LW,TH2F,Flex2DHisto<float> >(this,FLEXHIST(),m_ownsRootSumw2);
    if (!apply(h))
      assert(false);
    if (LWHistControls::cleanupOnGetROOT())
      clear();
    m_rootHisto = h;
  }
  return m_rootHisto;
}

//____________________________________________________________________
void TH2F_LW::clear()
{
  LWREPORT
  LWHist::clear();
  Flex2DHisto<float>::destroy(FLEXHIST());
  m_flexHisto = 0;
}

//____________________________________________________________________
void TH2F_LW::clearKeptROOTHist()
{
  if (usingROOTBackend()||!ownsROOTHisto())
    return;
  if (m_rootHisto) {
    LWHistRootUtils::deleteRootHisto(m_rootHisto, m_ownsRootSumw2);
    m_rootHisto = 0;
  }
}

//____________________________________________________________________
void TH2F_LW::Reset()
{
  if (m_rootHisto) {
    m_rootHisto->Reset();
    return;
  }
  if (m_flexHisto) {
    LWREPORT
    const float * xbins = FLEXHIST()->getVarBinsX();
    const float * ybins = FLEXHIST()->getVarBinsY();
    double xmin(FLEXHIST()->getXMin()),xmax(FLEXHIST()->getXMax());
    double ymin(FLEXHIST()->getYMin()),ymax(FLEXHIST()->getYMax());
    unsigned nbinsx(FLEXHIST()->getNBinsX()), nbinsy(FLEXHIST()->getNBinsY());
    Flex2DHisto<float>::destroy(FLEXHIST());
    if (xbins)
      m_flexHisto = ybins ? Flex2DHisto<float>::create(nbinsx,xbins,nbinsy,ybins)
                          : Flex2DHisto<float>::create(nbinsx,xbins,nbinsy,ymin,ymax);
    else
      m_flexHisto = ybins ? Flex2DHisto<float>::create(nbinsx,xmin,xmax,nbinsy,ybins)
                          : Flex2DHisto<float>::create(nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  }
}

//____________________________________________________________________
void TH2F_LW::SetBins(unsigned nbinsx,double xmin,double xmax,
		      unsigned nbinsy,double ymin,double ymax)
{
  if (GetEntries())
    std::cout<<"TH2F_LW Warning: SetBins(..) called on non-empty histogram."
	     <<" Bin contents lost/reshuffled! (histogram name: \""<<GetName()<<"\")"<<std::endl;
  if (m_rootHisto) {
    m_rootHisto->SetBins(nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  } else {
    Flex2DHisto<float>::destroy(FLEXHIST());
    m_flexHisto = Flex2DHisto<float>::create(nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  }
}

const float * TH2F_LW::getVarBinsX() const
{
  assert (!m_rootHisto);
  return FLEXHIST()->getVarBinsX();
}

const float * TH2F_LW::getVarBinsY() const
{
  assert (!m_rootHisto);
  return FLEXHIST()->getVarBinsY();
}

double TH2F_LW::getXMin() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmin();
  return FLEXHIST()->getXMin();
}

double TH2F_LW::getXMax() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmax();
  return FLEXHIST()->getXMax();
}

double TH2F_LW::getYMin() const
{
  if (m_rootHisto)
    return m_rootHisto->GetYaxis()->GetXmin();
  return FLEXHIST()->getYMin();
}

double TH2F_LW::getYMax() const
{
  if (m_rootHisto)
    return m_rootHisto->GetYaxis()->GetXmax();
  return FLEXHIST()->getYMax();
}


void TH2F_LW::Fill(const double& x,const double& y)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x,y);
  else
    FLEXHIST()->fill(x,y);
}

void TH2F_LW::Fill(const double& x,const double& y, const double& w)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x,y,w);
  else
    FLEXHIST()->fill(x,y,w);
}

unsigned TH2F_LW::GetNbinsX() const
{
  return CALL(GetNbinsX(),getNBinsX());
}

unsigned TH2F_LW::GetNbinsY() const
{
  return CALL(GetNbinsY(),getNBinsY());
}

unsigned TH2F_LW::actualFindBinX(const double&x) const
{
  assert(!m_rootHisto);//Should have called the TAxis::FindBin() directly
  return FLEXHIST()->valueToXBin(x);
}

unsigned TH2F_LW::actualFindBinY(const double&y) const
{
  assert(!m_rootHisto);//Should have called the TAxis::FindBin() directly
  return FLEXHIST()->valueToYBin(y);
}

double TH2F_LW::GetBinContent(unsigned binx, unsigned biny) const
{
  LWCOUNTCALL;return CALL(GetBinContent(binx,biny),getBinContent(binx,biny));
}

double TH2F_LW::GetBinError(unsigned binx, unsigned biny) const
{
  LWCOUNTCALL;return CALL(GetBinError(binx,biny),getBinError(binx,biny));
}

void TH2F_LW::SetBinContent(unsigned binx, unsigned biny, const double& c)
{
  LWCOUNTCALL;return CALL(SetBinContent(binx,biny,c),setBinContent(binx,biny,c));
}

void TH2F_LW::SetBinError(unsigned binx, unsigned biny, const double& e )
{
  LWCOUNTCALL;return CALL(SetBinError(binx,biny,e),setBinError(binx,biny,e));
}

unsigned TH2F_LW::GetEntries() const
{
  if (m_rootHisto)
    return static_cast<unsigned>(m_rootHisto->GetEntries());
  else
    return FLEXHIST()->getEntries();
}

void TH2F_LW::SetEntries(unsigned n) { CALL(SetEntries(n),setEntries(n)); }

void TH2F_LW::getSums( double& sumW, double& sumW2,
		       double& sumWX,double& sumWX2,
		       double& sumWY, double& sumWY2,
		       double& sumWXY) const
{
  if (m_rootHisto) {
    sumW = LWHistRootUtils::getSumW(m_rootHisto);
    sumW2 = LWHistRootUtils::getSumW2(m_rootHisto);
    sumWX = LWHistRootUtils::getSumWX(m_rootHisto);
    sumWX2 = LWHistRootUtils::getSumWX2(m_rootHisto);
    sumWY = LWHistRootUtils::getSumWY(m_rootHisto);
    sumWY2 = LWHistRootUtils::getSumWY2(m_rootHisto);
    sumWXY = LWHistRootUtils::getSumWXY(m_rootHisto);
  } else {
    sumW = FLEXHIST()->getSumW();     sumW2 = FLEXHIST()->getSumW2();
    sumWX = FLEXHIST()->getSumWX();   sumWX2 = FLEXHIST()->getSumWX2();
    sumWY = FLEXHIST()->getSumWY();   sumWY2 = FLEXHIST()->getSumWY2();
    sumWXY = FLEXHIST()->getSumWXY();
  }
}
void TH2F_LW::setSums( const double& sumW,const double&sumW2,const double& sumWX,const double& sumWX2,
		       const double& sumWY, const double& sumWY2,const double& sumWXY)
{
  if (m_rootHisto) {
    LWHistRootUtils::setSumW(m_rootHisto,sumW);
    LWHistRootUtils::setSumW2(m_rootHisto,sumW2);
    LWHistRootUtils::setSumWX(m_rootHisto,sumWX);
    LWHistRootUtils::setSumWX2(m_rootHisto,sumWX2);
    LWHistRootUtils::setSumWY(m_rootHisto,sumWY);
    LWHistRootUtils::setSumWY2(m_rootHisto,sumWY2);
    LWHistRootUtils::setSumWXY(m_rootHisto,sumWXY);
  } else {
    FLEXHIST()->setSums(sumW,sumW2,sumWX,sumWX2,sumWY,sumWY2,sumWXY);
  }
}

double TH2F_LW::actualGetBinCenterX(int bin) const
{
  if (m_rootHisto) {
    assert(false);
    return 0;
  }
  return FLEXHIST()->getBinCenterX(bin);
}

double TH2F_LW::actualGetBinCenterY(int bin) const
{
  if (m_rootHisto) {
    assert(false);
    return 0;
  }
  return FLEXHIST()->getBinCenterY(bin);
}

double TH2F_LW::Integral() const
{
  return CALL(Integral(),Integral());
}

void TH2F_LW::resetActiveBinLoop()
{
  if (m_rootHisto) {
    m_rootbackend_fastloopbin = 0;
    return;
  }
  m_rootbackend_fastloopbin=UINT_MAX;
  FLEXHIST()->resetActiveBinLoop();
}

bool TH2F_LW::getNextActiveBin(unsigned& binx, unsigned& biny, double& content, double& error)
{
  if (m_rootHisto) {
    unsigned nxp2(GetNbinsX()+2);
    unsigned n(nxp2*(GetNbinsY()+2));
    assert(m_rootbackend_fastloopbin<n+1);
    while(m_rootbackend_fastloopbin<n) {
      content = m_rootHisto->GetBinContent(m_rootbackend_fastloopbin);
      error = m_rootHisto->GetBinError(m_rootbackend_fastloopbin);
      ++m_rootbackend_fastloopbin;
      if (content!=0.0||error!=0.0) {
	binx = (m_rootbackend_fastloopbin-1) % nxp2;
	biny = (m_rootbackend_fastloopbin-1-binx) / nxp2;
	return true;
      }
    }
    return false;
  }
  assert(m_rootbackend_fastloopbin==UINT_MAX);
  return FLEXHIST()->getNextActiveBin(binx,biny,content,error);
}

void TH2F_LW::GetBinContentAndError(unsigned binx, unsigned biny,double& content, double&error) const
{
  LWCOUNTCALL
  if (m_rootHisto) {
    content = m_rootHisto->GetBinContent(binx,biny);
    error = m_rootHisto->GetBinError(binx,biny);
  } else {
    FLEXHIST()->getBinContentAndError(binx,biny,content,error);
  }
}

void TH2F_LW::SetBinContentAndError(unsigned binx, unsigned biny,const double& content, const double& error)
{
  LWCOUNTCALL
  if (m_rootHisto) {
    m_rootHisto->SetBinContent(binx,biny,content);
    m_rootHisto->SetBinError(binx,biny,error);
  } else {
    FLEXHIST()->setBinContentAndError(binx,biny,content,error);
  }
}

void TH2F_LW::scaleContentsAndErrors( const double& fact )
{
  //Treated like changing unit on the weights.
  if (m_rootHisto) {
    LWHistRootUtils::scaleContentsAndErrors(m_rootHisto,fact);
    double sumW, sumW2, sumWX,sumWX2,sumWY, sumWY2,sumWXY;
    getSums(sumW, sumW2, sumWX,sumWX2,sumWY, sumWY2,sumWXY);
    setSums(sumW*fact, sumW2*fact*fact, sumWX*fact,sumWX2*fact,
	    sumWY*fact, sumWY2*fact,sumWXY*fact);
    return;
  }
  FLEXHIST()->scaleContentsAndErrors(fact);
}
