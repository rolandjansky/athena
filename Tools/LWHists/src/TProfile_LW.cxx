/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TProfile_LW                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "LWHists/TProfile_LW.h"
#include "LWHists/LWHistControls.h"
#include "Flex1DProfileHisto.h"
#include "LWHistRootUtils.h"
#include "TProfile.h"

#define CALL(x,y) m_rootHisto ? m_rootHisto-> x : m_flexHisto-> y
#ifdef LW_DEBUG_HEAVY_USERS
#define LWCOUNTCALL {if (m_flexHisto) m_flexHisto->countCall(LWHistTraceUtils::getAdresses());};
#define LWREPORT {if (m_flexHisto) m_flexHisto->produceReport(GetName());};
#else
#define LWCOUNTCALL {};
#define LWREPORT {};
#endif

//____________________________________________________________________
TProfile_LW * TProfile_LW::create( const char* name, const char* title,
				   unsigned nbinsx, const double& xlow, const double& xup, const char* option )
{
  return actualcreate(name,title,nbinsx,xlow,xup,0.0,0.0,0,0,option);
}

//____________________________________________________________________
TProfile_LW * TProfile_LW::create( const char* name, const char* title,
				   unsigned nbinsx, const double& xlow, const double& xup,
				   const double& ylow, const double& yup, const char* option )
{
  return actualcreate(name,title,nbinsx,xlow,xup,ylow,yup,0,0,option);
}

//____________________________________________________________________
TProfile_LW * TProfile_LW::create( const char* name, const char* title,unsigned nbinsx, const float* xbins, const char* option)
{
  return actualcreate(name,title,nbinsx,xbins[0],xbins[nbinsx],0.0,0.0,xbins,0,option);
}

//____________________________________________________________________
TProfile_LW * TProfile_LW::create( const char* name, const char* title,unsigned nbinsx, const double* xbins, const char* option)
{
  return actualcreate(name,title,nbinsx,xbins[0],xbins[nbinsx],0.0,0.0,0,xbins,option);
}

//____________________________________________________________________
TProfile_LW * TProfile_LW::create( const char* name, const char* title,unsigned nbinsx, const double* xbins,
				   const double& ylow, const double& yup, const char* option)
{
  return actualcreate(name,title,nbinsx,xbins[0],xbins[nbinsx],ylow,yup,0,xbins,option);
}


//____________________________________________________________________
TProfile_LW * TProfile_LW::actualcreate( const char* name, const char* title,
					 unsigned nbinsx, const double& xlow, const double& xup,
					 const double& ylow, const double& yup,
					 const float* xbins_flt, const double* xbins_dbl, const char* option )
{
   bool rootbackend(LWHistControls::hasROOTBackend());
   TProfile_LW * h;
   if (rootbackend) {
     h = new TProfile_LW(name,title,nbinsx,xlow,xup,ylow,yup,xbins_flt,xbins_dbl,option,rootbackend);
   } else {
     h = MP_NEW(TProfile_LW)(name,title,nbinsx,xlow,xup,ylow,yup,xbins_flt,xbins_dbl,option,rootbackend);
     h->m_nBytesFromPool = sizeof(TProfile_LW);
   }
   return h;
}

//____________________________________________________________________
TProfile_LW::TProfile_LW( const char* name, const char* title,
			  unsigned nbinsx, const double& xlow, const double& xup,
			  const double& ylow, const double& yup,
			  const float* xbins_flt, const double* xbins_dbl,
			  const char* option, bool rootbackend )
  : LWHist(name,title,rootbackend),
    m_flexHisto(0),
    m_rootHisto(0)
{
  //Fixme: these asserts could be in common for all binnings... [+nan
  //checking) (check input and revert to dummy range in case of
  //problems +emit warning).
  assert(!(xbins_flt&&xbins_dbl));//max one var binning defined
  assert(!xbins_flt||(xbins_flt[0]==xlow&&xbins_flt[nbinsx]==xup));
  assert(!xbins_dbl||(xbins_dbl[0]==xlow&&xbins_dbl[nbinsx]==xup));
  assert(!xbins_flt||(ylow==0.0&&yup==0.0));//because otherwise we would have to use
                                            //a non-existing constructor from root.
  assert(xup>xlow);
  assert(nbinsx>0);
  if (rootbackend) {
    if (xbins_dbl)
      m_rootHisto = new TProfile(name,title,nbinsx,xbins_dbl,ylow,yup,option);
    else if (xbins_flt)
      m_rootHisto = new TProfile(name,title,nbinsx,xbins_flt,option);
    else
      m_rootHisto = new TProfile(name,title,nbinsx,xlow,xup,ylow,yup,option);
  } else {
    if (xbins_dbl)
      m_flexHisto = Flex1DProfileHisto::create(nbinsx,xbins_dbl,ylow,yup);
    else if (xbins_flt)
      m_flexHisto = Flex1DProfileHisto::create(nbinsx,xbins_flt,ylow,yup);
    else
      m_flexHisto = Flex1DProfileHisto::create(nbinsx,xlow,xup,ylow,yup);
    m_flexHisto->setErrorOptionFromString(option);
  }
}

//____________________________________________________________________
TProfile_LW::~TProfile_LW()
{
  clearKeptROOTHist();
  clear();
  if (ownsROOTHisto())
    delete m_rootHisto;//for root backend
}

//____________________________________________________________________
TH1* TProfile_LW::getROOTHistBaseNoAlloc() const
{
  return m_rootHisto;
}
TH1* TProfile_LW::getROOTHistBase() { return getROOTHist(); }
TProfile* TProfile_LW::getROOTHist()
{
  if (!m_rootHisto) {
    TProfile * h = LWHistRootUtils::createRootProfileHisto(this, m_flexHisto);
    if (!apply(h))
      assert(false);
    double stats[6];
    getSums(stats[0],stats[1],stats[2],stats[3],stats[4],stats[5]);
    h->PutStats(stats);
    if (LWHistControls::cleanupOnGetROOT())
      clear();
    m_rootHisto = h;
  }
  return m_rootHisto;
}

//____________________________________________________________________
void TProfile_LW::clear()
{
  LWREPORT
  LWHist::clear();
  Flex1DProfileHisto::destroy(m_flexHisto);
  m_flexHisto = 0;
}

//____________________________________________________________________
void TProfile_LW::clearKeptROOTHist()
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
void TProfile_LW::Reset()
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

void TProfile_LW::SetErrorOption(Option_t *option) { CALL(SetErrorOption(option),setErrorOptionFromString(option)); }
const char* TProfile_LW::GetErrorOption() const { return CALL(GetErrorOption(),getErrorOptionAsString()); }

const float * TProfile_LW::getVarBins() const
{
  assert(!m_rootHisto);
  return m_flexHisto->getVarBins();
}

double TProfile_LW::getXMin() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmin();
  return m_flexHisto->getXMin();
}

double TProfile_LW::getXMax() const
{
  if (m_rootHisto)
    return m_rootHisto->GetXaxis()->GetXmax();
  return m_flexHisto->getXMax();
}

void TProfile_LW::Fill(const double& x, const double& y)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x,y);
  else
    m_flexHisto->fill(x,y);
}

void TProfile_LW::Fill(const double& x, const double& y, const double& w)
{
  LWCOUNTCALL
  if (m_rootHisto)
    m_rootHisto->Fill(x,y,w);
  else
    m_flexHisto->fill(x,y,w);
}

unsigned TProfile_LW::GetNbinsX() const { return CALL(GetNbinsX(),getNBins()); }
double TProfile_LW::GetBinEntries(unsigned bin) const { LWCOUNTCALL;return CALL(GetBinEntries(bin),getBinEntries(bin)); }
double TProfile_LW::GetBinContent(unsigned bin) const { LWCOUNTCALL;return CALL(GetBinContent(bin),getBinContent(bin)); }
double TProfile_LW::GetBinError(unsigned bin) const { LWCOUNTCALL;return CALL(GetBinError(bin),getBinError(bin)); }
void TProfile_LW::SetBinEntries(unsigned bin, const double& c) { LWCOUNTCALL;CALL(SetBinEntries(bin,c),setBinEntries(bin,c)); }
void TProfile_LW::SetBinContent(unsigned bin, const double& c) { LWCOUNTCALL;CALL(SetBinContent(bin,c),setBinContent(bin,c)); }
void TProfile_LW::SetBinError(unsigned bin, const double& e ) { LWCOUNTCALL;CALL(SetBinError(bin,e),setBinError(bin,e)); }
unsigned TProfile_LW::GetEntries() const
{
  if (m_rootHisto)
    return static_cast<unsigned>(m_rootHisto->GetEntries());
  else
    return m_flexHisto->getEntries();
}
void TProfile_LW::SetEntries(unsigned n) { CALL(SetEntries(n),setEntries(n)); }

void TProfile_LW::getSums( double& sumW, double& sumW2,
			   double& sumWX,double& sumWX2,
			   double& sumWY,double& sumWY2 ) const
{
  if (m_rootHisto) {
    sumW = LWHistRootUtils::getSumW(m_rootHisto);
    sumW2 = LWHistRootUtils::getSumW2(m_rootHisto);
    sumWX = LWHistRootUtils::getSumWX(m_rootHisto);
    sumWX2 = LWHistRootUtils::getSumWX2(m_rootHisto);
    sumWY = LWHistRootUtils::getSumWY(m_rootHisto);
    sumWY2 = LWHistRootUtils::getSumWY2(m_rootHisto);
  } else {
    sumW = m_flexHisto->getSumW();     sumW2 = m_flexHisto->getSumW2();
    sumWX = m_flexHisto->getSumWX();   sumWX2 = m_flexHisto->getSumWX2();
    sumWY = m_flexHisto->getSumWProfPar();   sumWY2 = m_flexHisto->getSumWProfPar2();
  }
}

void TProfile_LW::setSums( const double& sumW, const double&sumW2,
			   const double& sumWX, const double& sumWX2,
			   const double& sumWY, const double& sumWY2 )
{
  if (m_rootHisto) {
    LWHistRootUtils::setSumW(m_rootHisto,sumW);
    LWHistRootUtils::setSumW2(m_rootHisto,sumW2);
    LWHistRootUtils::setSumWX(m_rootHisto,sumWX);
    LWHistRootUtils::setSumWX2(m_rootHisto,sumWX2);
    LWHistRootUtils::setSumWY(m_rootHisto,sumWY);
    LWHistRootUtils::setSumWY2(m_rootHisto,sumWY2);
  } else {
    m_flexHisto->setSums(sumW,sumW2,sumWX,sumWX2,sumWY,sumWY2);
  }
}
double TProfile_LW::Integral() const { return CALL(Integral(),integral()); }
void TProfile_LW::GetBinInfo(unsigned bin,double& entries, double& content, double&error) const
{
  LWCOUNTCALL;
  if (m_rootHisto) {
    entries = m_rootHisto->GetBinEntries(bin);
    content = m_rootHisto->GetBinContent(bin);
    error = m_rootHisto->GetBinError(bin);
  } else {
    m_flexHisto->getBinInfo(bin,entries,content,error);
  }
}

void TProfile_LW::SetBinInfo(unsigned bin, const double& entries, const double& content, const double& error)
{
  LWCOUNTCALL;
  if (m_rootHisto) {
    m_rootHisto->SetBinEntries(bin,entries);
    m_rootHisto->SetBinContent(bin,content);
    m_rootHisto->SetBinError(bin,error);
  } else {
    m_flexHisto->setBinInfo(bin,entries,content,error);
  }
}

double TProfile_LW::actualGetBinCenterX(int bin) const
{
  if (m_rootHisto) {
    assert(false);
    return 0;
  }
  return m_flexHisto->getBinCenter(bin);
}

unsigned TProfile_LW::actualFindBinX(const double&x) const
{
  assert(!m_rootHisto);//Should have called the TAxis method
  return m_flexHisto->valueToBin(x);
}

