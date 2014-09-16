/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LWHISTFACTORY_H
#define LWHISTFACTORY_H

#include<string>
#include "LWHists/LWHist.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"


class LWHistFactoryBase {
public:
  //FIXME: Take std::strings as name/title if that is how they are provided anyway...
  LWHistFactoryBase(const char*title):m_title(title), m_laronlineidstrhelper(0) {}
  virtual ~LWHistFactoryBase(){};
  void SetXTitle(const char* t) { m_xtitle=t; }
  void SetYTitle(const char* t) { m_ytitle=t; }
  void SetZTitle(const char* t) { m_ztitle=t; }
  void setXAxisBinLabelsAllFebs(LArOnlineIDStrHelper*l,const std::string& detector_str)
  {
    m_laronlineidstrhelper=l;
    m_xaxislabels_detstr=detector_str;
  }
  virtual LWHist * create(const char*name) const = 0;
  LWHist * create(const std::string&name) const { return create(name.c_str()); }
  LWHist * create(const char*name,const char*title) const {
    LWHist * h = create(name);
    h->SetTitle((std::string(h->GetTitle())+title).c_str());
    return h;
  }
  LWHist * create(const std::string&name,const std::string&title) const {
    LWHist * h = create(name.c_str());
    h->SetTitle((std::string(h->GetTitle())+title).c_str());
    return h;
  }
protected:
  void apply(LWHist*h) const {
    h->SetTitle(m_title.c_str());
    if (!m_xtitle.empty()) h->SetXTitle(m_xtitle.c_str());
    if (!m_ytitle.empty()) h->SetYTitle(m_ytitle.c_str());
    if (!m_ztitle.empty()) h->SetZTitle(m_ztitle.c_str());
    if (!m_xaxislabels_detstr.empty()&&m_laronlineidstrhelper) {
      m_laronlineidstrhelper->setAxisBinLabelsAllFebs( h->GetXaxis(), m_xaxislabels_detstr );
    }
  }
private:
  LWHistFactoryBase( const LWHistFactoryBase & );
  LWHistFactoryBase & operator= ( const LWHistFactoryBase & );
  std::string m_title;
  std::string m_xtitle;
  std::string m_ytitle;
  std::string m_ztitle;
  std::string m_xaxislabels_detstr;
  LArOnlineIDStrHelper* m_laronlineidstrhelper;
};

template<class TH1x_LW>
class LWHist1DFactory : public LWHistFactoryBase {
public:

  LWHist1DFactory(const char*title, unsigned nbins, double xmin, double xmax)
    : LWHistFactoryBase(title), m_nbins(nbins), m_xmin(xmin), m_xmax(xmax) {}
  ~LWHist1DFactory(){}
  LWHist * create(const char*name) const
  {
    TH1x_LW * h=TH1x_LW::create(name,"",m_nbins,m_xmin,m_xmax);
    apply(h);
    return h;
  }
private:
  unsigned m_nbins;
  double m_xmin;
  double m_xmax;
};

template<class TH2x_LW>
class LWHist2DFactory : public LWHistFactoryBase {
public:

  LWHist2DFactory(const char*title, unsigned nbinsx, double xmin, double xmax,unsigned nbinsy, double ymin, double ymax)
    : LWHistFactoryBase(title), m_nbinsx(nbinsx), m_xmin(xmin), m_xmax(xmax),
      m_nbinsy(nbinsy), m_ymin(ymin), m_ymax(ymax),m_varbinsx(0) {}
  LWHist2DFactory(const char*title, unsigned nbinsx, double* xvarbins,unsigned nbinsy, double ymin, double ymax)
    : LWHistFactoryBase(title), m_nbinsx(nbinsx), m_xmin(xvarbins[0]), m_xmax(xvarbins[nbinsx]),
      m_nbinsy(nbinsy), m_ymin(ymin), m_ymax(ymax),m_varbinsx(new double[nbinsx+1])
  {
    for (unsigned i=0;i<=nbinsx;++i)
      m_varbinsx[i]=xvarbins[i];
  }
  LWHist2DFactory(const LWHist2DFactory& other) : LWHistFactoryBase(other.m_title)
  {
    m_nbinsx = other.m_nbinsx;
    m_xmin = other.m_xmin;
    m_xmax = other.m_xmax;
    m_nbinsy = other.m_nbinsy;
    m_ymin = other.m_ymin;
    m_ymax = other.m_ymax;
    if( other.m_varbinsx != 0x0 )
    {
      m_varbinsx = new double[m_nbinsx+1];
      for( unsigned int i=0; i < m_nbinsx+1; ++i )
      {
	m_varbinsx[i] = other.m_varbinsx[i];
      }
    }
  }
  ~LWHist2DFactory(){if( m_varbinsx != 0x0 ) delete[] m_varbinsx;}
  LWHist * create(const char*name) const
  {
    TH2x_LW * h(0);
    if (m_varbinsx)
      h=TH2x_LW::create(name,"",m_nbinsx,m_varbinsx,m_nbinsy,m_ymin,m_ymax);
    else
      h=TH2x_LW::create(name,"",m_nbinsx,m_xmin,m_xmax,m_nbinsy,m_ymin,m_ymax);
    apply(h);
    return h;
  }
private:
  unsigned m_nbinsx;
  double m_xmin;
  double m_xmax;
  unsigned m_nbinsy;
  double m_ymin;
  double m_ymax;
  double * m_varbinsx;

  LWHist2DFactory& operator= (const LWHist2DFactory&);
};

class LWHistProfileFactory : public LWHistFactoryBase {
public:

  LWHistProfileFactory(const char*title, unsigned nbinsx, double xmin, double xmax, double ymin=0.0, double ymax=0.0)
    : LWHistFactoryBase(title), m_nbinsx(nbinsx), m_xmin(xmin), m_xmax(xmax), m_ymin(ymin), m_ymax(ymax) {}
  ~LWHistProfileFactory(){}
  LWHist * create(const char*name) const
  {
    TProfile_LW * h=TProfile_LW::create(name,"",m_nbinsx,m_xmin,m_xmax,m_ymin,m_ymax);
    apply((LWHist*)h);
    return h;
  }
private:
  unsigned m_nbinsx;
  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;
};

class LWHistProfile2DFactory : public LWHistFactoryBase {
public:

  LWHistProfile2DFactory(const char*title, unsigned nbinsx, double xmin, double xmax,
			 unsigned nbinsy, double ymin, double ymax,double zmin=0.0,double zmax=0.0)
    : LWHistFactoryBase(title), m_nbinsx(nbinsx), m_xmin(xmin), m_xmax(xmax),
      m_nbinsy(nbinsy), m_ymin(ymin), m_ymax(ymax), m_zmin(zmin), m_zmax(zmax),m_varbinsx(0) {}
  LWHistProfile2DFactory(const char*title, unsigned nbinsx, double* xvarbins,
			 unsigned nbinsy, double ymin, double ymax)
    : LWHistFactoryBase(title), m_nbinsx(nbinsx), m_xmin(xvarbins[0]), m_xmax(xvarbins[nbinsx]),
      m_nbinsy(nbinsy), m_ymin(ymin), m_ymax(ymax), m_zmin(0.0), m_zmax(0.0),m_varbinsx(new double[nbinsx+1])
  {
    for (unsigned i=0;i<=nbinsx;++i)
      m_varbinsx[i]=xvarbins[i];
  }
  ~LWHistProfile2DFactory(){ delete [] m_varbinsx; }
  LWHist * create(const char*name) const
  {
    TProfile2D_LW * h(0);
    if (m_varbinsx)
      h=TProfile2D_LW::create(name,"",m_nbinsx,m_varbinsx,m_nbinsy,m_ymin,m_ymax/*,m_zmin,m_zmax*/);
    else
      h=TProfile2D_LW::create(name,"",m_nbinsx,m_xmin,m_xmax,m_nbinsy,m_ymin,m_ymax,m_zmin,m_zmax);
    apply((LWHist*)h);
    return h;
  }
private:
  unsigned m_nbinsx;
  double m_xmin;
  double m_xmax;
  unsigned m_nbinsy;
  double m_ymin;
  double m_ymax;
  double m_zmin;
  double m_zmax;
  double * m_varbinsx;

  LWHistProfile2DFactory (const LWHistProfile2DFactory&);
  LWHistProfile2DFactory& operator= (const LWHistProfile2DFactory&);
};

#endif
