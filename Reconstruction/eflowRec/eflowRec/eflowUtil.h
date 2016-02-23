/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowUtil.h
 *
 *  Created on: 09.08.2013
 *      Author: tlodd
 */

#ifndef EFLOWUTIL_H_
#define EFLOWUTIL_H_

#include <sstream>
#include <math.h>

class eflowAzimuth {
public:
 eflowAzimuth(double phi): m_value(phi) { if (phi != -999.) adjustRange(); }
  eflowAzimuth(const eflowAzimuth& other): m_value(other.m_value) { }
  void operator=(const eflowAzimuth& other) { m_value = other.m_value; }
  ~eflowAzimuth() { }

  inline double operator ()() const { return m_value; }
  inline double operator =(double phi) {
    m_value = phi;
    adjustRange();
    return m_value;
  }
//  inline eflowAzimuth operator =(const eflowAzimuth& other) {
//    m_value = other.m_value;
//    return *this;
//  }
  inline eflowAzimuth operator +=(double deltaPhi) {
    m_value += deltaPhi;
    adjustRange();
    return *this;
  }
  inline eflowAzimuth operator -=(double deltaPhi) {
    m_value -= deltaPhi;
    adjustRange();
    return *this;
  }

//  inline double signedDifference(const eflowAzimuth& other) { return adjustRange(m_value - other.m_value); }
//  inline double signedDifference(double phi) { return adjustRange(m_value - phi); }
  inline double getAbsDifference(const eflowAzimuth& other) const {
    double plainAbsDifference = fabs(m_value - other.m_value);
    return plainAbsDifference <= M_PI ? plainAbsDifference : 2*M_PI - plainAbsDifference;
  }

  inline double cycle(const eflowAzimuth& other) { return cycle(other.m_value); }
  inline double cycle(double phi) {
    double plainDifference = phi-m_value;
    if (plainDifference > M_PI) {
      return m_value+2.0*M_PI;
    } else if (plainDifference < -M_PI) {
      return m_value-2.0*M_PI;
    } else {
      return m_value;
    }
  }

private:
  double m_value;

  inline double adjustRange(double a) {
    if (a <= -M_PI) {
      return a+(2*M_PI*floor(-(a-M_PI)/(2*M_PI)));
    } else if (a > M_PI) {
      return a-(2*M_PI*floor((a+M_PI)/(2*M_PI)));
    } else {
      return a;
    }
  }
  inline void adjustRange() {
    if (m_value <= -M_PI) {
      m_value+=(2*M_PI*floor(-(m_value-M_PI)/(2*M_PI)));
    } else if (m_value > M_PI) {
      m_value-=(2*M_PI*floor((m_value+M_PI)/(2*M_PI)));
    }
  }

};

class eflowEtaPhiPosition {
public:
  eflowEtaPhiPosition(): m_eta(NAN), m_phi(NAN) {}
  eflowEtaPhiPosition(double eta, double phi): m_eta(eta), m_phi(phi) {}
  eflowEtaPhiPosition(const eflowEtaPhiPosition& other) :
      m_eta(other.m_eta), m_phi(other.m_phi) { }
  void operator=(const eflowEtaPhiPosition& other) {  m_eta = other.m_eta; m_phi = other.m_phi; }
  ~eflowEtaPhiPosition() { }

  inline double getEta() const { return m_eta; }
  inline eflowAzimuth getPhi() const { return m_phi; }
  inline double getPhiD() const { return m_phi(); }

  inline double dRSq(const eflowEtaPhiPosition& other) const {
    double dEta(m_eta-other.m_eta);
    double dPhi(m_phi.getAbsDifference(other.m_phi));
    return dEta*dEta + dPhi*dPhi;
  }
  inline double dR(const eflowEtaPhiPosition& other) const { return sqrt(this->dRSq(other)); }

private:
  double m_eta;
  eflowAzimuth m_phi;
};

template <class T>
class eflowRangeBase{
public:
  eflowRangeBase(): m_min(NAN), m_max(NAN) { }
  eflowRangeBase(const T& min, const T& max): m_min(min), m_max(max) { }
  ~eflowRangeBase() { }

  inline void setCenterAndWidth(T center, double width) { m_min = center - width/2; m_max = m_min + width; }
  inline void shift(double shift) { m_min += shift; m_max += shift; }

  inline T getMax() const { return m_max; }
  inline T getMin() const { return m_min; }

  inline T getCenter() const { return (m_max + m_min)/2; }
  inline T getWidth() const { return (m_max - m_min); }

  bool contains(const T& x) { return ( (m_min < x) && (m_max > x) ); }

  std::string print() const {
    std::stringstream result;
    result << "[" << m_min << ", " << m_max << "]";
    return result.str();
  }

private:
  T m_min;
  T m_max;
};
typedef eflowRangeBase<double> eflowRange;

#endif /* EFLOWUTIL_H_ */
