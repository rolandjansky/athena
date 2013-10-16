/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRAJECTORY_H
#define TRAJECTORY_H
#include <string> 
#include <utility>

namespace TrigFake{

class Trajectory {
 public:
  Trajectory(double px, double py, double pz, double vx, double vy, double vz, double q);
  ~Trajectory(){};

 public:
  
  double d0() const { return m_d0; };
  double phi0() const { return m_phi0; };
  double z0() const { return m_z0; };
  double Rcurv() const { return m_Rcurv; };
  double eta() const { return m_eta; };
  double theta() const { return m_theta; };
  double pz() const { return m_pz; };
  double pT() const { return m_pT; };

  std::pair<double,double> etaPhiAtCylinder(double rC, double zC) const ;
  

 private:
  
  double m_d0; 
  double m_phi0;
  double m_Rcurv;
  double m_z0;
  double m_q;
  double m_eta;
  double m_theta;
  double m_pz;
  double m_pT;


};
}
#endif
