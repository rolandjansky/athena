/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>

#include "DataQualityTools/ZdcSignalSinc.h"


ZdcSignalSinc::ZdcSignalSinc(int nSlc)
  : n_Slices(nSlc)
    , m_AmpThresh(5.)
    , tClock      (25.)
    , Pi(4*atan(double(1.)))
{
  m_buf = new double[n_Slices];
  m_Time   =  0.;
  m_Amp    =  0.;
  m_Err    =  e_noData;
}

ZdcSignalSinc::~ZdcSignalSinc() { delete[] m_buf;}

int ZdcSignalSinc::process(double *buf, double gain, double ped,
			   double frac, bool corr) {

  m_Time = 0.;
  m_Amp  = 0.;
  m_Err  = e_OK;
  m_Warn = 0;

  if (ped==0) ped=buf[0];
  for (double *p=m_buf;p<m_buf+n_Slices;p++) {
    double a = *(buf++);
    //std::cout << "ZDC+++ A" << a << std::endl;
    if (a>1015) {
      m_Err = e_Overflow;
      return  m_Err;
    }
    *p = a-ped;
  }
  if (frac<=0.) {
    m_Err = e_wrongFrac;
    return m_Err;
  }

  int imax = -1;
  if (m_buf[0         ]-m_buf[1         ]>m_AmpThresh) {
    imax = 0;
    m_Warn += 1;
  }

  if (m_buf[n_Slices-1]-m_buf[n_Slices-2]>m_AmpThresh) {
    if (imax<0) {
      imax = n_Slices-1;
      m_Warn += 2;
    } else {
      m_Err = e_wrongSignal;
      return m_Err;
    }
  }

  for (int i=1;i<n_Slices-1;i++) {
    double da1 = m_buf[i]-m_buf[i-1];
    double da2 = m_buf[i]-m_buf[i+1];
    if (da1>0. && da2>=0.) {
      if (da1>m_AmpThresh || da2>m_AmpThresh) {
	if (imax<0) imax = i;
	else {
	  m_Err = e_wrongSignal;
	  return m_Err;
	}
      }
    }
  }

  if (imax<0) {
    m_Err = e_noSignal;
    return m_Err;
  }

  if (imax==1) m_Warn += 4;

  if (imax>=2) {
    //       printf("%d %f %f\n",imax,m_buf[imax-2],m_AmpThresh);
    if (m_buf[imax-2]>m_AmpThresh)  m_p = imax-2;
    else                            m_p = imax-1;
  } else m_p = imax;

  m_np = n_Slices-m_p;

  double t_peak = findpeak(imax-m_p);
  if (m_Err) return m_Err;

  if (frac>=1.) m_Time = t_peak;
  else          m_Time = fraction(frac,t_peak);
  m_Amp = waveform(t_peak);

  //std::cout << "ZDC+++ m_AMP " << m_Amp << std::endl;

  /*
  printf("aap: %d %d %d  ",imax, m_p, m_np);
  for (int i=0;i<n_Slices;i++) printf("%6.1f ",m_buf[i]);
  printf("  %6.1f %6.1f %6.1f\n",t_peak*tClock,m_Time*tClock,m_Amp);
  */

  if (gain==1.) {
    if (m_Amp<200.) m_Warn +=  8;
    if (m_Amp>900.) m_Warn += 16;
  } else {
    if (m_Amp< 50.) m_Warn +=  8;
    if (m_Amp>250.) m_Warn += 16;
  }
  m_Amp *= gain;
  m_Time = (m_Time+m_p)*tClock;
  if (corr) {
    int off = int(10.+m_Time/tClock)-10;
    double t = m_Time - off*tClock;

    if      (t< 5.) t =   0.056836274872111      + 1.532763686481279  *t
		      +   0.229808343735056 *t*t - 0.0365636647119192 *t*t*t;
    else if (t<16.) t =  -1.28974955223497       + 2.653578699334604  *t
		      -   0.1371240146140209*t*t + 0.00281211806384422*t*t*t;
    else            t = -42.18688740322650       + 8.61752055701946   *t
		      -   0.4259239806065329*t*t + 0.00753849507486617*t*t*t;

    m_Time = off*tClock + t;
  }

  return m_Err;
}

int    ZdcSignalSinc::getError()   {return m_Err;}
int    ZdcSignalSinc::getWarning() {return m_Warn;}
double ZdcSignalSinc::getTime()    {return m_Time;}
double ZdcSignalSinc::getAmp()     {return m_Amp;}

double ZdcSignalSinc::waveform(double t) {
  double f = 0.;
  for (int i=0; i<m_np; i++) {
    double x = Pi*(t-i);
    if (x) f += m_buf[m_p+i]*sin(x)/x;
    else   f += m_buf[m_p+i];
  }
  return f;
}

double ZdcSignalSinc::fraction(double frac, double tpeak) {
  tim[0] = 0.;    wfm[0] = waveform(tim[0]);
  tim[1] = tpeak; wfm[1] = waveform(tim[1]);
  dt     = tpeak/2.;
  double thr = frac*wfm[1];
  while(wfm[0]>thr) {
    tim[0] -= 0.1;
    wfm[0] = waveform(tim[0]);
    if (tim[0]<-2.) {
      tim[0] = 0.;
      wfm[0] = waveform(tim[0]);
      m_Err = e_wrongSignal;
      break;
    }
  }

  while (dt>0.001) {
    double t = tim[0] + dt;
    double f = waveform(t);
    if (f>thr) {tim[1] = t; wfm[1] = f;}
    else       {tim[0] = t; wfm[0] = f;}
    dt /= 2.;
  }
  return (tim[0]+tim[1])/2.;
}

double ZdcSignalSinc::findpeak(int im) {

  tim[0] = im; tim[1]=im+0.5; tim[2]=im+1.;
  for (int i=0;i<3;i++) wfm[i]=waveform(tim[i]);
  dt = 0.5;
  double t = findpeak();
  return t;
}

double ZdcSignalSinc::findpeak() {
  if (dt<0.001) return tim[1];

  if (wfm[0]<wfm[1]) {
    if (wfm[2]<=wfm[1]) {
      dt /=2.;
      double t1 = tim[1]-dt;
      double f1 = waveform(t1);
      if (f1>wfm[1]) {
	tim[2] = tim[1]; wfm[2] = wfm[1];
	tim[1] = t1;      wfm[1] = f1;
      } else {
	double t2 = tim[1]+dt;
	double f2 = waveform(t2);
	if (f2>wfm[1]) {
	  tim[0] = tim[1]; wfm[0] = wfm[1];
	  tim[1] = t2;      wfm[1] = f2;
	} else {
	  tim[0] = t1;      wfm[0] = f1;
	  tim[2] = t2;      wfm[2] = f2;
	}
      }
    } else {
      tim[0] = tim[1]; wfm[0] = wfm[1];
      tim[1] = tim[2]; wfm[1] = wfm[2];
      tim[2] += dt;     wfm[2] = waveform(tim[2]);
    }
  } else {
    if (wfm[2]<=wfm[1]) {
      tim[2] = tim[1]; wfm[2] = wfm[1];
      tim[1] = tim[0]; wfm[1] = wfm[0];
      tim[0] -= dt;    wfm[0] = waveform(tim[0]);
      return findpeak();
    } else {
      m_Err = e_localMinimum;
      return 0.;
    }
  }
  return findpeak();
}



