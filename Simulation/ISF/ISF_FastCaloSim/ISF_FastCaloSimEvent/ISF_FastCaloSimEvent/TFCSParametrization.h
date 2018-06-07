/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSParametrization_h
#define ISF_FASTCALOSIMEVENT_TFCSParametrization_h

#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"

class TFCSParametrization:public ::TFCSParametrizationBase {
public:
  TFCSParametrization(const char* name=0, const char* title=0);

  virtual bool is_match_pdgid(int id) const {return m_pdgid.find(id)!=m_pdgid.end();};
  virtual bool is_match_Ekin(float Ekin) const {return (Ekin>=m_Ekin_min) && (Ekin<m_Ekin_max);};
  virtual bool is_match_eta(float eta) const {return (eta>=m_eta_min) && (eta<m_eta_max);};

  const std::set< int > &pdgid() const {return m_pdgid;};
  double Ekin_nominal() const {return m_Ekin_nominal;};
  double Ekin_min() const {return m_Ekin_min;};
  double Ekin_max() const {return m_Ekin_max;};
  double eta_nominal() const {return m_eta_nominal;};
  double eta_min() const {return m_eta_min;};
  double eta_max() const {return m_eta_max;};

  void set_pdgid(int id);
  void add_pdgid(int id);
  void clear_pdgid();

  void set_Ekin_nominal(double min);
  void set_Ekin_min(double min);
  void set_Ekin_max(double max);
  void set_eta_nominal(double min);
  void set_eta_min(double min);
  void set_eta_max(double max);

private:
  std::set< int > m_pdgid;
  double m_Ekin_nominal,m_Ekin_min,m_Ekin_max;
  double m_eta_nominal,m_eta_min,m_eta_max;

  ClassDef(TFCSParametrization,1)  //TFCSParametrization
};

#endif
