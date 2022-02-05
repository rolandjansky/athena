/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMT_FITTER_H
#define MMT_FITTER_H

#include "AthenaBaseComps/AthMessaging.h"

#include "MMT_struct.h"


class MMT_Fitter : AthMessaging {
 public:
  MMT_Fitter(int nlg=256, double LG_min=0., double LG_max=0.5);
  void Get_Fit(std::vector<Hit>& track, std::map<int,evFit_entry>& Event_Fit, std::map<hitData_key,hitData_entry>& Hits_Data_Set_Time);
  evFit_entry fit_event(int event, std::vector<Hit>& track, std::vector<hitData_entry>& hitDatas, int& nfit, std::vector<std::pair<double,double> >&mxmy, double& mxl, double& mv, double& mu, std::shared_ptr<MMT_Parameters> par) const;
  int get_last() const {return m_last;}
  int SC_ROI_n_x(std::shared_ptr<MMT_Parameters> par) const {return par->n_x;}
  int SC_ROI_n_y(std::shared_ptr<MMT_Parameters> par) const {return par->n_y;}
  int find_hitData(const std::vector<hitData_entry>& hitDatas, const hitData_key& key) const;

 protected:
  int m_last;

  //functions translated
  int Filter_UV(std::vector<Hit>& track, std::shared_ptr<MMT_Parameters> par) const;
  double Get_Global_Slope(const std::vector<Hit>& track, const std::string& type, std::shared_ptr<MMT_Parameters> par) const;
  double Get_Local_Slope(const std::vector<Hit>& Track,double theta, double phi, std::shared_ptr<MMT_Parameters> par) const;
  ROI Get_ROI(double M_x,double M_u,double M_v,const std::vector<Hit>&track, std::shared_ptr<MMT_Parameters> par) const;
  double phi_correct_factor(const std::vector<Hit>&track, std::shared_ptr<MMT_Parameters> par)const;
  double Get_Delta_Theta(double M_local,double M_global, std::shared_ptr<MMT_Parameters> par) const;
  double Get_Delta_Theta_division(double M_local,double M_global, double a) const;
  int Rough_ROI_temp(double theta, double phi, std::shared_ptr<MMT_Parameters> par) const;

  //sim hit code stuff
  int track_to_index(const std::vector<Hit>&track, std::shared_ptr<MMT_Parameters> par)const;

  //ideal local slope for debugging purposes
  double ideal_local_slope(const std::vector<Hit>& Track, std::shared_ptr<MMT_Parameters> par)const;
  double ideal_z(const Hit& hit, std::shared_ptr<MMT_Parameters> par)const;
  double ideal_ak(const std::vector<Hit>& Track, std::shared_ptr<MMT_Parameters> par)const;
  double ideal_zbar(const std::vector<Hit>& Track, std::shared_ptr<MMT_Parameters> par)const;

  //translated from Table_Generators.m
  double LG_lgr(int ilgr, double a, int number_LG_regions, double min, double max) const;
  double mult_factor_lgr(int ilgr, double a, int number_LG_regions, double min, double max) const;
  double Slope_Components_ROI_val(int jy, int ix, int thetaphi, std::shared_ptr<MMT_Parameters> par) const;
  double Slope_Components_ROI_theta(int jy, int ix, std::shared_ptr<MMT_Parameters> par) const;
  double Slope_Components_ROI_phi(int jy, int ixi, std::shared_ptr<MMT_Parameters> par) const;
  double DT_Factors_val(int i, int j, std::shared_ptr<MMT_Parameters> par) const;


 private:
  //some functions
  std::vector<Hit> q_hits(const std::string& type,const std::vector<Hit>& hits,std::shared_ptr<MMT_Parameters> par) const;

  //Fitter components
  int m_number_LG_regions,m_n_fit;
  double m_LG_min,m_LG_max;
  std::vector<int> q_planes(char type) const;//return position of what planes are where

};
#endif
