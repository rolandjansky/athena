/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TH1I_LW                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/TH1I_LW.h"
#include "LWHists/LWHistControls.h"
#include "Flex1DHisto.h"
#include "LWHistRootUtils.h"
#include "TH1I.h"
#include <iostream>

#define FLEXHIST() ((reinterpret_cast<Flex1DHisto<int>*>(m_flexHisto)))
#define CALL(x,y) m_rootHisto ? m_rootHisto-> x : reinterpret_cast<Flex1DHisto<int>*>(m_flexHisto)-> y
#ifdef LW_DEBUG_HEAVY_USERS
#define LWCOUNTCALL {if (m_flexHisto) FLEXHIST()->countCall(LWHistTraceUtils::getAdresses());};
#define LWREPORT {if (m_flexHisto) FLEXHIST()->produceReport(GetName());};
#else
#define LWCOUNTCALL {};
#define LWREPORT {};
#endif

//____________________________________________________________________
TH1I_LW * TH1I_LW::create( const char* name, const char* title,
			   unsigned nbinsx, const double& xlow, const double& xup )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH1I_LW * h;
  if (rootbackend) {
    h = new TH1I_LW (name,title,nbinsx,xlow,xup,rootbackend);
  } else {
    h = MP_NEW(TH1I_LW)(name,title,nbinsx,xlow,xup,rootbackend);
    h->m_nBytesFromPool = sizeof(TH1I_LW);
  }
  return h;
}

//____________________________________________________________________
TH1I_LW * TH1I_LW::create( const char* name, const char* title, unsigned nbinsx, const double* xbins )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH1I_LW * h;
  if (rootbackend) {
    h = new TH1I_LW(name,title,nbinsx,xbins,rootbackend);
  } else {
    h = MP_NEW(TH1I_LW)(name,title,nbinsx,xbins,rootbackend);
    h->m_nBytesFromPool = sizeof(TH1I_LW);
  }
  return h;
}

//____________________________________________________________________
TH1I_LW * TH1I_LW::create( const char* name, const char* title, unsigned nbinsx, const float* xbins )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TH1I_LW * h;
  if (rootbackend) {
    h = new TH1I_LW(name,title,nbinsx,xbins,rootbackend);
  } else {
    h = MP_NEW(TH1I_LW)(name,title,nbinsx,xbins,rootbackend);
    h->m_nBytesFromPool = sizeof(TH1I_LW);
  }
  return h;
}

//____________________________________________________________________
TH1I_LW::TH1I_LW( const char* name, const char* title,
		  unsigned nbins,
		  const double& xmin,
		  const double& xmax,
		  bool rootbackend )
  : LWHist1D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH1I(name,title,nbins,xmin,xmax);
  } else {
    m_flexHisto = Flex1DHisto<int>::create(nbins,xmin,xmax);
  }
}

//____________________________________________________________________
TH1I_LW::TH1I_LW( const char* name, const char* title,
		  unsigned nbins, const double* xbins, bool rootbackend )
  : LWHist1D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH1I(name,title,nbins,xbins);
  } else {
    m_flexHisto = Flex1DHisto<int>::create(nbins,xbins);
  }
}

//____________________________________________________________________
TH1I_LW::TH1I_LW( const char* name, const char* title,
		  unsigned nbins, const float* xbins, bool rootbackend )
  : LWHist1D(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0),
    m_rootbackend_fastloopbin(0),
    m_ownsRootSumw2(false)
{
  if (rootbackend) {
    m_rootHisto = new TH1I(name,title,nbins,xbins);
  } else {
    m_flexHisto = Flex1DHisto<int>::create(nbins,xbins);
  }
}

//____________________________________________________________________
TH1I_LW::~TH1I_LW()
{
  clearKeptROOTHist();
  clear();
  if (ownsROOTHisto())
    delete m_rootHisto;//for root backend
}

//____________________________________________________________________
TH1* TH1I_LW::getROOTHistBaseNoAlloc() const
{
  return m_rootHisto;
}
TH1* TH1I_LW::getROOTHistBase() { return getROOTHist(); }
TH1I* TH1I_LW::getROOTHist()
{
  if (!m_rootHisto) {
    TH1I* h = LWHistRootUtils::createRootHisto<int,TH1I_LW,TH1I,Flex1DHisto<int> >(this,FLEXHIST(),m_ownsRootSumw2);
    if (!apply(h))
      assert(false);
    if (LWHistControls::cleanupOnGetROOT())
      clear();
    m_rootHisto = h;
  }
  return m_rootHisto;
}

//____________________________________________________________________
void TH1I_LW::clear()
{
  LWREPORT
  LWHist::clear();
  Flex1DHisto<int>::destroy(FLEXHIST());
  m_flexHisto = 0;
}

//____________________________________________________________________
void TH1I_LW::clearKeptROOTHist()
{
  if (usingROOTBackend()||!ownsROOTHisto())
    return;

  if (m_rootHisto) {
    if (usingROOTBackend())
      delete m_rootHisto;
    else
      LWHistRootUtils::deleteRootHisto(m_rootHisto, m_ownsRootSumw2);
    m_rootHisto = 0;
  }
}

//____________________________________________________________________
void TH1I_LW::Reset()
{
  if (m_rootHisto) {
    m_rootHisto->Reset();
    return;
  }
  if (m_flexHisto) {
    LWREPORT
    const float * xbins = FLEXHIST()->getVarBins();
    double xmin(FLEXHIST()->getXMin()),xmax(FLEXHIST()->getXMax());
    unsigned nbins(FLEXHIST()->getNBins());
    Flex1DHisto<int>::destroy(FLEXHIST());
    m_flexHisto = xbins ? Flex1DHisto<int>::create(nbins,xbins) : Flex1DHisto<int>::create(nbins,xmin,xmax);

  }
}

//____________________________________________________________________
void TH1I_LW::SetBins(unsigned nbins,double xmin,double xmax)
{
  if (GetEntries())
    std::cout<<"TH1I_LW Warning: SetBins(..) called on non-empty histogram."
	     <<" Bin contents lost/reshuffled! (histogram name: \""<<GetName()<<"\")"<<std::endl;
  if (m_rootHisto) {
    m_rootHisto->SetBins(nbins,xmin,xmax);
  } else {
    Flex1DHisto<int>::destroy(FLEXHIST());
    m_flexHisto = Flex1DHisto<int>::create(nbins,xmin,xmax);
  }
}

const float * TH1I_LW::getVarBins() const
{
  if (m_rootHisto) {
    assert(false);
    return 0;
  }
  return FLEXHIST()->getVarBins();
}

double TH1I_LW::getXMin() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmin();
  return FLEXHIST()->getXMin();
}

double TH1I_LW::getXMax() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmax();
  return FLEXHIST()->getXMax();
}

void TH1I_LW::Fill(const double& x)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x);
  else
    FLEXHIST()->fill(x);
}

void TH1I_LW::Fill(const double& x, const double& w)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x,w);
  else
    FLEXHIST()->fill(x,w);
}

unsigned TH1I_LW::GetNbinsX() const { return CALL(GetNbinsX(),getNBins()); }
double TH1I_LW::GetBinContent(unsigned bin) const { LWCOUNTCALL;return CALL(GetBinContent(bin),getBinContent(bin)); }
double TH1I_LW::GetBinError(unsigned bin) const { LWCOUNTCALL;return CALL(GetBinError(bin),getBinError(bin)); }
void TH1I_LW::SetBinContent(unsigned bin, const double& c) { LWCOUNTCALL;CALL(SetBinContent(bin,c),setBinContent(bin,c)); }
void TH1I_LW::SetBinError(unsigned bin, const double& e ) { LWCOUNTCALL;CALL(SetBinError(bin,e),setBinError(bin,e)); }
unsigned TH1I_LW::GetEntries() const
{
  if (m_rootHisto)
    return static_cast<unsigned>(m_rootHisto->GetEntries());
  else
    return FLEXHIST()->getEntries();
}
void TH1I_LW::SetEntries(unsigned n) { CALL(SetEntries(n),setEntries(n)); }


void TH1I_LW::getSums( double& sumW, double& sumW2,
		       double& sumWX,double& sumWX2 ) const
{
  if (m_rootHisto) {
    sumW = LWHistRootUtils::getSumW(m_rootHisto);
    sumW2 = LWHistRootUtils::getSumW2(m_rootHisto);
    sumWX = LWHistRootUtils::getSumWX(m_rootHisto);
    sumWX2 = LWHistRootUtils::getSumWX2(m_rootHisto);
  } else {
    sumW = FLEXHIST()->getSumW();     sumW2 = FLEXHIST()->getSumW2();
    sumWX = FLEXHIST()->getSumWX();   sumWX2 = FLEXHIST()->getSumWX2();
  }
}

void TH1I_LW::setSums( const double& sumW,const double&sumW2,const double& sumWX,const double& sumWX2 )
{
  if (m_rootHisto) {
    LWHistRootUtils::setSumW(m_rootHisto,sumW);
    LWHistRootUtils::setSumW2(m_rootHisto,sumW2);
    LWHistRootUtils::setSumWX(m_rootHisto,sumWX);
    LWHistRootUtils::setSumWX2(m_rootHisto,sumWX2);
  } else {
    FLEXHIST()->setSums(sumW,sumW2,sumWX,sumWX2);
  }
}

double TH1I_LW::actualGetBinCenterX(int bin) const
{
  assert(!m_rootHisto);//Should have called the TAxis method
  if (m_rootHisto) {
    assert(false);
    return 0;
  }
  return FLEXHIST()->getBinCenter(bin);
}

unsigned TH1I_LW::actualFindBinX(const double&x) const
{
  assert(!m_rootHisto);//Should have called the TAxis method
  return FLEXHIST()->valueToBin(x);
}

double TH1I_LW::Integral() const { return CALL(Integral(),Integral()); }
void TH1I_LW::resetActiveBinLoop() {
  if (m_rootHisto) {
    m_rootbackend_fastloopbin = 0;
    return;
  }
  m_rootbackend_fastloopbin=UINT_MAX;
  FLEXHIST()->resetActiveBinLoop();
}
bool TH1I_LW::getNextActiveBin(unsigned& bin, double& content, double& error) {
  if (m_rootHisto) {
    unsigned n(GetNbinsX()+2);
    assert(m_rootbackend_fastloopbin<n+1);
    while(m_rootbackend_fastloopbin<n) {
      content = m_rootHisto->GetBinContent(m_rootbackend_fastloopbin);
      error = m_rootHisto->GetBinError(m_rootbackend_fastloopbin);
      ++m_rootbackend_fastloopbin;
      if (content!=0.0||error!=0.0) {
	bin = m_rootbackend_fastloopbin-1;
	return true;
      }
    }
    return false;
  }
  assert(m_rootbackend_fastloopbin==UINT_MAX);
  return FLEXHIST()->getNextActiveBin(bin, content, error);
}
void TH1I_LW::GetBinContentAndError(unsigned bin,double& content, double&error) const
{
  LWCOUNTCALL
  if (m_rootHisto) {
    content = m_rootHisto->GetBinContent(bin);
    error = m_rootHisto->GetBinError(bin);
  } else {
    FLEXHIST()->getBinContentAndError(bin,content,error);
  }
}

void TH1I_LW::SetBinContentAndError(unsigned bin,const double& content, const double& error)
{
  LWCOUNTCALL
  if (m_rootHisto) {
    m_rootHisto->SetBinContent(bin,content);
    m_rootHisto->SetBinError(bin,error);
  } else {
    FLEXHIST()->setBinContentAndError(bin,content,error);
  }
}

void TH1I_LW::scaleContentsAndErrors( const double& fact )
{
  //Treated like changing unit on the weights.
  if (m_rootHisto) {
    LWHistRootUtils::scaleContentsAndErrors(m_rootHisto,fact);
    double sumW, sumW2, sumWX, sumWX2;
    getSums(sumW, sumW2, sumWX, sumWX2);
    setSums(fact*sumW, sumW2*fact*fact, sumWX*fact, sumWX2*fact);
    return;
  }
  FLEXHIST()->scaleContentsAndErrors(fact);
}
