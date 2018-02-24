/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSParametrization_h
#define ISF_FASTCALOSIMEVENT_TFCSParametrization_h

#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"

class TFCSParametrization:public ::TFCSParametrizationBase {
public:
  TFCSParametrization(const char* name=nullptr, const char* title=nullptr);
  void clear();

  virtual bool is_match_pdgid(int id) const {return TestBit(kMatchAllPDGID) || m_pdgid.find(id)!=m_pdgid.end();};
  virtual bool is_match_Ekin(float Ekin) const {return (Ekin>=m_Ekin_min) && (Ekin<m_Ekin_max);};
  virtual bool is_match_eta(float eta) const {return (eta>=m_eta_min) && (eta<m_eta_max);};

  virtual bool is_match_all_Ekin() const {return Ekin_min()==init_Ekin_min && Ekin_max()==init_Ekin_max;};
  virtual bool is_match_all_eta() const {return eta_min()==init_eta_min && eta_max()==init_eta_max;};
  virtual bool is_match_all_Ekin_bin() const {return true;};
  virtual bool is_match_all_calosample() const {return true;};

  const std::set< int > &pdgid() const {return m_pdgid;};
  double Ekin_nominal() const {return m_Ekin_nominal;};
  double Ekin_min() const {return m_Ekin_min;};
  double Ekin_max() const {return m_Ekin_max;};
  double eta_nominal() const {return m_eta_nominal;};
  double eta_min() const {return m_eta_min;};
  double eta_max() const {return m_eta_max;};

  virtual void set_pdgid(int id);
  virtual void set_pdgid(const std::set< int > &ids);
  virtual void add_pdgid(int id);
  virtual void clear_pdgid();

  virtual void set_Ekin_nominal(double min);
  virtual void set_Ekin_min(double min);
  virtual void set_Ekin_max(double max);
  virtual void set_eta_nominal(double min);
  virtual void set_eta_min(double min);
  virtual void set_eta_max(double max);
  
  virtual void set_Ekin(const TFCSParametrizationBase& ref);
  virtual void set_eta(const TFCSParametrizationBase& ref);
  virtual void set_Ekin_eta(const TFCSParametrizationBase& ref);
  virtual void set_pdgid_Ekin_eta(const TFCSParametrizationBase& ref);

protected:
  const double init_Ekin_nominal=0;
  const double init_Ekin_min=0;
  const double init_Ekin_max=14000000;
  const double init_eta_nominal=0;
  const double init_eta_min=-100;
  const double init_eta_max=100;

private:
  std::set< int > m_pdgid;
  double m_Ekin_nominal,m_Ekin_min,m_Ekin_max;
  double m_eta_nominal,m_eta_min,m_eta_max;

  ClassDef(TFCSParametrization,1)  //TFCSParametrization
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSParametrization+;
#endif

#endif
