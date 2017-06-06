/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLEXSECTION_H
#define SAMPLEXSECTION_H

#include <map>

/** @class SampleXsectionSvc
 **    
 ** @author David Calvet <calvet@in2p3.fr>
 **
 ** @brief A class to provide access to Monte Carlo sample luminosity information.
 **
*/
class SampleXsection {

public:
  
  enum showering{ pythia=0, herwig=1, sherpa=2, pythia8=4, herwigpp=5, sherpa21=6, unknown=10};

  inline SampleXsection() : m_Xsects() {}
  inline SampleXsection(const SampleXsection &xs) : m_Xsects(xs.m_Xsects) {}
  inline ~SampleXsection() {}

  SampleXsection &operator=(const SampleXsection &xs);

  bool readFromFile(const char *fName);
  void setSample(const int dsid,const double xSect,const double kFact,const showering shower, const double xSectDw, const double xSectUp);

  double getRawXsection(const int dsid) const;
  double getKfactor(const int dsid) const;
  double getXsection(const int dsid) const;
  double getXsectionUp(const int dsid) const;
  double getXsectionDown(const int dsid) const;
  std::pair<double,double> getXsectionDownUp(const int dsid) const;
  showering getShowering(const int dsid) const;
  int getShoweringIndex(const int dsid) const;

private:

  std::map<int,std::pair<double,double> > m_Xsects;
  std::map<int,std::pair<double,double> > m_Uncert;
  std::map<int,showering > m_Showering;

};

#endif // SAMPLEXSECTION_H
