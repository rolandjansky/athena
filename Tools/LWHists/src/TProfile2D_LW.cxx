/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TProfile2D_LW                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/TProfile2D_LW.h"
#include "LWHists/LWHistControls.h"
#include "Flex2DProfileHisto.h"
#include "LWHistRootUtils.h"
#include "TProfile2D.h"

#define CALL(x,y) m_rootHisto ? m_rootHisto-> x : m_flexHisto-> y
#ifdef LW_DEBUG_HEAVY_USERS
#define LWCOUNTCALL {if (m_flexHisto) m_flexHisto->countCall(LWHistTraceUtils::getAdresses());};
#define LWREPORT {if (m_flexHisto) m_flexHisto->produceReport(GetName());};
#else
#define LWCOUNTCALL {};
#define LWREPORT {};
#endif

//____________________________________________________________________
TProfile2D_LW * TProfile2D_LW::create( const char* name, const char* title,
				       unsigned nbinsx, const double& xlow, const double& xup,
				       unsigned nbinsy, const double& ylow, const double& yup, const char* option )
{
  return actualcreate(name,title,
		      nbinsx,xlow,xup,0,
		      nbinsy,ylow,yup,0,
		      0.0,0.0,option);
}

//____________________________________________________________________
TProfile2D_LW * TProfile2D_LW::create( const char* name, const char* title,
				       unsigned nbinsx, const double& xlow, const double& xup,
				       unsigned nbinsy, const double& ylow, const double& yup,
				       const double& zlow, const double& zup, const char* option )
{
  return actualcreate(name,title,
		      nbinsx,xlow,xup,0,
		      nbinsy,ylow,yup,0,
		      zlow,zup,option);
}

//____________________________________________________________________
TProfile2D_LW * TProfile2D_LW::create( const char* name, const char* title,
				       unsigned nbinsx, const double* xbins,
				       unsigned nbinsy, const double& ylow, const double& yup, const char* option )
{
  return actualcreate(name,title,
		      nbinsx,xbins[0],xbins[nbinsx],xbins,
		      nbinsy,ylow,yup,0,
		      0.0,0.0,option);
}

//____________________________________________________________________
TProfile2D_LW * TProfile2D_LW::create( const char* name, const char* title,
				       unsigned nbinsx, const double& xlow, const double& xup,
				       unsigned nbinsy, const double* ybins, const char* option )
{
  return actualcreate(name,title,
		      nbinsx,xlow,xup,0,
		      nbinsy,ybins[0],ybins[nbinsy],ybins,
		      0.0,0.0,option);

}

//____________________________________________________________________
TProfile2D_LW * TProfile2D_LW::create( const char* name, const char* title,
				       unsigned nbinsx, const double* xbins,
				       unsigned nbinsy, const double* ybins, const char* option )
{
  return actualcreate(name,title,
		      nbinsx,xbins[0],xbins[nbinsx],xbins,
		      nbinsy,ybins[0],ybins[nbinsy],ybins,
		      0.0,0.0,option);
}

//____________________________________________________________________
TProfile2D_LW * TProfile2D_LW::actualcreate( const char* name, const char* title,
					     unsigned nbinsx, const double& xlow, const double& xup, const double* xbins,
					     unsigned nbinsy, const double& ylow, const double& yup, const double* ybins,
					     const double& zlow, const double& zup, const char* option )
{
  bool rootbackend(LWHistControls::hasROOTBackend());
  TProfile2D_LW * h;
  if (rootbackend) {
    h = new TProfile2D_LW(name,title,nbinsx,xlow,xup,xbins,nbinsy,ylow,yup,ybins,zlow,zup,option,rootbackend);
  } else {
    h = MP_NEW(TProfile2D_LW)(name,title,nbinsx,xlow,xup,xbins,nbinsy,ylow,yup,ybins,zlow,zup,option,rootbackend);
    h->m_nBytesFromPool = sizeof(TProfile2D_LW);
  }
  return h;
}

//____________________________________________________________________
TProfile2D_LW::TProfile2D_LW( const char* name, const char* title,
			      unsigned nbinsx, const double& xmin,const double& xmax, const double* xbins,
			      unsigned nbinsy, const double& ymin,const double& ymax, const double* ybins,
			      const double& zlow, const double& zup,const char* option,bool rootbackend )
  : LWHist(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0)
{
  //Fixme: these asserts could be in common for all binnings... [+nan
  //checking) (check input and revert to dummy range in case of
  //problems +emit warning).
  assert(!xbins||(xbins[0]==xmin&&xbins[nbinsx]==xmax));
  assert(!ybins||(ybins[0]==ymin&&ybins[nbinsy]==ymax));
  assert(xmax>xmin);
  assert(ymax>ymin);
  assert(nbinsx>0);
  assert(nbinsy>0);

  //fixme
  if (rootbackend) {
    if (xbins) {
      if (ybins)
	m_rootHisto = new TProfile2D(name,title,nbinsx,xbins,nbinsy,ybins,option);
      else
	m_rootHisto = new TProfile2D(name,title,nbinsx,xbins,nbinsy,ymin,ymax,option);
    } else {
      if (ybins)
	m_rootHisto = new TProfile2D(name,title,nbinsx,xmin,xmax,nbinsy,ybins,option);
      else
	m_rootHisto = new TProfile2D(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax,zlow,zup,option);
    }
  } else {
    m_flexHisto = Flex2DProfileHisto::create(nbinsx,xmin,xmax,xbins,nbinsy,ymin,ymax,ybins,zlow,zup);
    m_flexHisto->setErrorOptionFromString(option);
  }
}

//____________________________________________________________________
TProfile2D_LW::~TProfile2D_LW()
{
  clearKeptROOTHist();
  clear();
  if (ownsROOTHisto())
    delete m_rootHisto;//for root backend
}

//____________________________________________________________________
TH1* TProfile2D_LW::getROOTHistBaseNoAlloc() const
{
  return m_rootHisto;
}
TH1* TProfile2D_LW::getROOTHistBase() { return getROOTHist(); }
TProfile2D* TProfile2D_LW::getROOTHist()
{
  if (!m_rootHisto) {
    TProfile2D * h = LWHistRootUtils::createRoot2DProfileHisto(this, m_flexHisto);
    if (!apply(h))
      assert(false);
    double stats[9];
    getSums(stats[0],stats[1],stats[2],stats[3],stats[4],stats[5],stats[6],stats[7],stats[8]);
    h->PutStats(stats);
    if (LWHistControls::cleanupOnGetROOT())
      clear();
    m_rootHisto = h;
  }
  return m_rootHisto;
}

//____________________________________________________________________
void TProfile2D_LW::clear()
{
  LWREPORT
  LWHist::clear();
  Flex2DProfileHisto::destroy(m_flexHisto);
  m_flexHisto = 0;
}

//____________________________________________________________________
void TProfile2D_LW::clearKeptROOTHist()
{
  if (usingROOTBackend()||!ownsROOTHisto())
    return;

  if (m_rootHisto) {
    if (usingROOTBackend())
      delete m_rootHisto;
    else
      LWHistRootUtils::deleteProfileHisto(m_rootHisto);

    m_rootHisto = 0;
  }
}

//____________________________________________________________________
void TProfile2D_LW::Reset()
{
  if (m_rootHisto) {
    m_rootHisto->Reset();
    return;
  }
  if (m_flexHisto) {
    LWREPORT
    m_flexHisto->reset();
  }
}

void TProfile2D_LW::SetErrorOption(Option_t *option) { CALL(SetErrorOption(option),setErrorOptionFromString(option)); }
const char* TProfile2D_LW::GetErrorOption() const { return CALL(GetErrorOption(),getErrorOptionAsString()); }

double TProfile2D_LW::getXMin() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmin();
  return m_flexHisto->getXMin();
}

double TProfile2D_LW::getXMax() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmax();
  return m_flexHisto->getXMax();
}
double TProfile2D_LW::getYMin() const
{
  if (m_rootHisto)
    return m_rootHisto->GetYaxis()->GetXmin();
  return m_flexHisto->getYMin();
}

double TProfile2D_LW::getYMax() const
{
  if (m_rootHisto)
    return m_rootHisto->GetYaxis()->GetXmax();
  return m_flexHisto->getYMax();
}

void TProfile2D_LW::Fill(const double& x, const double& y, const double& z)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x,y,z);
  else
    m_flexHisto->fill(x,y,z);
}

void TProfile2D_LW::Fill(const double& x, const double& y,const double& z,const double& w)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x,y,z,w);
  else
    m_flexHisto->fill(x,y,z,w);
}

unsigned TProfile2D_LW::GetNbinsX() const { return CALL(GetNbinsX(),getNBinsX()); }
unsigned TProfile2D_LW::GetNbinsY() const { return CALL(GetNbinsY(),getNBinsY()); }
//double TProfile2D_LW::GetBinEntries(unsigned binx, unsigned biny) const { LWCOUNTCALL;return CALL(GetBinEntries(binx,biny),getBinEntries(binx,biny)); }
double TProfile2D_LW::GetBinContent(unsigned binx, unsigned biny) const {   LWCOUNTCALL;return CALL(GetBinContent(binx,biny),getBinContent(binx,biny)); }
double TProfile2D_LW::GetBinError(unsigned binx, unsigned biny) const { LWCOUNTCALL;return CALL(GetBinError(binx,biny),getBinError(binx,biny)); }
//void TProfile2D_LW::SetBinEntries(unsigned binx, unsigned biny, const double& c) { LWCOUNTCALL;CALL(SetBinEntries(binx,biny,c),setBinEntries(binx,biny,c)); }
void TProfile2D_LW::SetBinContent(unsigned binx, unsigned biny, const double& c) { LWCOUNTCALL;CALL(SetBinContent(binx,biny,c),setBinContent(binx,biny,c)); }
void TProfile2D_LW::SetBinError(unsigned binx, unsigned biny, const double& e ) { LWCOUNTCALL;CALL(SetBinError(binx,biny,e),setBinError(binx,biny,e)); }
unsigned TProfile2D_LW::GetEntries() const
{
  if (m_rootHisto)
    return static_cast<unsigned>(m_rootHisto->GetEntries());
  else
    return m_flexHisto->getEntries();
}
void TProfile2D_LW::SetEntries(unsigned n) { CALL(SetEntries(n),setEntries(n)); }

void TProfile2D_LW::getSums( double& sumW, double& sumW2,
			     double& sumWX,double& sumWX2,
			     double& sumWY, double& sumWY2,
			     double& sumWXY,
			     double& sumWZ, double&sumWZ2 ) const
{
  if (m_rootHisto) {
    sumW = LWHistRootUtils::getSumW(m_rootHisto);
    sumW2 = LWHistRootUtils::getSumW2(m_rootHisto);
    sumWX = LWHistRootUtils::getSumWX(m_rootHisto);
    sumWX2 = LWHistRootUtils::getSumWX2(m_rootHisto);
    sumWY = LWHistRootUtils::getSumWY(m_rootHisto);
    sumWY2 = LWHistRootUtils::getSumWY2(m_rootHisto);
    sumWXY = LWHistRootUtils::getSumWXY(m_rootHisto);
    sumWZ = LWHistRootUtils::getSumWZ(m_rootHisto);
    sumWZ2 = LWHistRootUtils::getSumWZ2(m_rootHisto);
  } else {
    sumW = m_flexHisto->getSumW();     sumW2 = m_flexHisto->getSumW2();
    sumWX = m_flexHisto->getSumWX();   sumWX2 = m_flexHisto->getSumWX2();
    sumWY = m_flexHisto->getSumWY();   sumWY2 = m_flexHisto->getSumWY2();
    sumWXY = m_flexHisto->getSumWXY();
    sumWZ = m_flexHisto->getSumWProfPar();   sumWZ2 = m_flexHisto->getSumWProfPar2();
  }
}

void TProfile2D_LW::setSums( const double& sumW, const double&sumW2,
			     const double& sumWX, const double& sumWX2,
			     const double& sumWY, const double& sumWY2,
			     const double& sumWXY,
			     const double& sumWZ, const double&sumWZ2 )
{
  assert(false);
  if (m_rootHisto) {
    LWHistRootUtils::setSumW(m_rootHisto,sumW);
    LWHistRootUtils::setSumW2(m_rootHisto,sumW2);
    LWHistRootUtils::setSumWX(m_rootHisto,sumWX);
    LWHistRootUtils::setSumWX2(m_rootHisto,sumWX2);
    LWHistRootUtils::setSumWY(m_rootHisto,sumWY);
    LWHistRootUtils::setSumWY2(m_rootHisto,sumWY2);
    LWHistRootUtils::setSumWXY(m_rootHisto,sumWXY);
    assert(false);
    LWHistRootUtils::setSumWZ(m_rootHisto,sumWZ);
    LWHistRootUtils::setSumWZ2(m_rootHisto,sumWZ2);
  } else {
    m_flexHisto->setSums(sumW,sumW2,sumWX,sumWX2,sumWY,sumWY2,sumWXY,sumWZ,sumWZ2);
  }
}
double TProfile2D_LW::Integral() const { return CALL(Integral(),integral()); }
void TProfile2D_LW::GetBinInfo(unsigned binx, unsigned biny,double& entries, double& content, double&error) const
{
  LWCOUNTCALL;
  if (m_rootHisto) {
    entries = m_rootHisto->GetBinEntries( biny * (m_rootHisto->GetNbinsX()+2) + binx);
    content = m_rootHisto->GetBinContent(binx,biny);
    error = m_rootHisto->GetBinError(binx,biny);
  } else {
    m_flexHisto->getBinInfo(binx,biny,entries,content,error);
  }
}

void TProfile2D_LW::SetBinInfo(unsigned binx, unsigned biny,const double& entries, const double& content, const double& error)
{
  LWCOUNTCALL;
  if (m_rootHisto) {
    m_rootHisto->SetBinEntries(biny * (m_rootHisto->GetNbinsX()+2) + binx,entries);
    m_rootHisto->SetBinContent(binx,biny,content);
    m_rootHisto->SetBinError(binx,biny,error);
  } else {
    m_flexHisto->setBinInfo(binx,biny,entries,content,error);
  }
}

unsigned TProfile2D_LW::actualFindBinX(const double&x) const
{
  assert(!m_rootHisto);//Should have called the TAxis method
  return m_flexHisto->valueToXBin(x);
}

unsigned TProfile2D_LW::actualFindBinY(const double&y) const
{
  assert(!m_rootHisto);//Should have called the TAxis method
  return m_flexHisto->valueToYBin(y);
}

double TProfile2D_LW::actualGetBinCenterX(int bin) const
{
  assert(!m_rootHisto);//Should have called the TAxis method
  return m_flexHisto->getBinCenterX(bin);
}

double TProfile2D_LW::actualGetBinCenterY(int bin) const
{
  assert(!m_rootHisto);//Should have called the TAxis method
  return m_flexHisto->getBinCenterY(bin);
}

const float * TProfile2D_LW::getVarBinsX() const
{
  assert(!m_rootHisto);
  return m_flexHisto->getVarBinsX();
}

const float * TProfile2D_LW::getVarBinsY() const
{
  assert(!m_rootHisto);
  return m_flexHisto->getVarBinsY();
}
