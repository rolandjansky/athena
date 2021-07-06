/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ARGSTRINGTODOUBLE_H
#define TRIGHLTJETHYPO_ARGSTRINGTODOUBLE_H

#include <string>
#include <vector>
#include <algorithm>
#include <limits>

class ArgStrToDouble {
 public:

  double operator() (const std::string& s){
    
    double val{0};
    if(std::find(m_posinf.begin(), m_posinf.end(), s) != m_posinf.end()){
      val =  std::numeric_limits<double>::max();}
    else if (std::find(m_neginf.begin(), m_neginf.end(), s) != m_neginf.end()){
      val = std::numeric_limits<double>::lowest();}
    else {
      val = std::stod(s);
    }
    
    return val;
  }

  
 private:
  std::vector<std::string> m_posinf{"inf", "+inf", "pinf", "INF"};
  std::vector<std::string> m_neginf{"-inf", "ninf", "-INF"};
};

#endif
