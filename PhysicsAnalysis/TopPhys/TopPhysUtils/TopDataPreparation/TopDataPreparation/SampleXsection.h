/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef SAMPLEXSECTION_H
#define SAMPLEXSECTION_H

#include <map>
#include <string>
#include <unordered_map>

/** @class SampleXsectionSvc
**
** @author David Calvet <calvet@in2p3.fr>
**
** @brief A class to provide access to Monte Carlo sample luminosity information.
**
*/
class SampleXsection {
public:
  enum showering {
    pythia=0,
    herwig=1,
    sherpa=2,
    pythia8=4,
    herwigpp=5,
    sherpa21=6,
    amcatnlopythia8=7,
    herwigpp713=8,
    sherpa228=9,
    sherpa2210=10,
    herwigpp721=11,
    unknown=12
  };

  inline SampleXsection() : m_Xsects() {}
  inline SampleXsection(const SampleXsection& xs) : m_Xsects(xs.m_Xsects) {}
  inline ~SampleXsection() {}

  SampleXsection& operator = (const SampleXsection& xs);

  bool readFromFile(const char* fName);
  void setSample(const int dsid, const double xSect, const double kFact, const showering shower, const double xSectDw,
                 const double xSectUp);

  double getRawXsection(const int dsid) const;
  double getKfactor(const int dsid) const;
  double getXsection(const int dsid) const;
  double getXsectionUp(const int dsid) const;
  double getXsectionDown(const int dsid) const;
  std::pair<double, double> getXsectionDownUp(const int dsid) const;
  showering getShowering(const int dsid) const;
  std::string getShoweringString(const int dsid) const;
  int getShoweringIndex(const int dsid) const;
  void setTranslator(const std::unordered_map<std::string, std::string>& map) {m_translator = map;}

private:
  std::map<int, std::pair<double, double> > m_Xsects;
  std::map<int, std::pair<double, double> > m_Uncert;
  std::map<int, showering > m_Showering;
  std::unordered_map<std::string, std::string> m_translator;

  showering applyTranslation(const showering shower) const;
  std::string showerToString(const showering shower) const;
  showering stringToShower(const std::string& name) const;
};

#endif // SAMPLEXSECTION_H
