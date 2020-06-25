/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKEROI_H
#define FAKEROI_H
#include <string> 

class FakeRoI {
 public:

  FakeRoI(std::string type, double phi, double eta) : 
    m_type(type), m_phi(phi), m_eta(eta) {};

  ~FakeRoI() {};
  
 public:
  std::string type () const { return m_type; } ;
  double phi () const { return m_phi ; } ;
  double eta () const { return m_eta ; } ;
  
 private:
  std::string m_type;
  double m_phi;
  double m_eta;
};
#endif
