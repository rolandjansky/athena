// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_TOPOCLUSTERTRANSFORM_H_
#define BOOSTEDJETSTAGGERS_TOPOCLUSTERTRANSFORM_H_

#include <math.h>
#include <map>
#include <vector>
#include <string>

class TopoclusterTransform {
  public:

  static double pt_min_max_scale(double clust_pt);
  static double jet_pt_scale(double clust_pt, double jet_pt);
  static double eta_min_max_scale(double clust_eta);
  static double phi_min_max_scale(double clust_phi);
  static double eta_shift(double clust_eta, double jet_eta);
  static double phi_shift(double clust_phi, double jet_phi);
  static double eta_shift_and_scale(double clust_eta, double jet_eta);
  static double phi_shift_and_scale(double clust_phi, double jet_phi);
  static double calculate_theta_for_rotations(std::map<std::string,double> clusters);
  static void rotate_jet(std::map<std::string,double> &clusters, double angle);
  static void flip(std::map<std::string,double> &clusters);

};

#endif
