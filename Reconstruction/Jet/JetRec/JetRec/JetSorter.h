// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETSORTER_H
#define JETREC_JETSORTER_H

#include <string>

#include "AsgTools/AsgTool.h"


#include "JetInterface/IJetModifier.h"



class JetSorter
  :  public asg::AsgTool ,
     virtual public IJetModifier {

ASG_TOOL_CLASS(JetSorter, IJetModifier)
public:
    
  JetSorter(const std::string& t);
  
  
  virtual int modify(xAOD::JetContainer& jets) const ;

protected:
  std::string m_sortType;
    
};

#endif
