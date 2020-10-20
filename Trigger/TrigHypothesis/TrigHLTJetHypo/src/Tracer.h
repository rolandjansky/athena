/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_TRACER_H
#define TRIGJETHYPO_TRACER_H

#include <iostream>
class Tracer {
public:
  Tracer(const std::string& msg): m_msg(msg){
    std::cerr<< "+ " << m_msg << '\n';} 
  ~Tracer() {
    std::cerr << "- " <<m_msg << '\n';
  }
private:
  std::string m_msg;
};

#endif
