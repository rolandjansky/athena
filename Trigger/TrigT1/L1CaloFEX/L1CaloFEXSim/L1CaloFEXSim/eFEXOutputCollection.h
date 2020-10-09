/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXOutputCollection.h  -  
//                              -------------------
//     begin                : 28 02 2020
//     email                : tong.qiu@cern.ch
//  **************************************************************************

#pragma once
#include "AthenaKernel/CLASS_DEF.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>

namespace LVL1 {
  class eFEXOutputCollection
  {
  public:
    eFEXOutputCollection() {};
    ~eFEXOutputCollection();
    void clear();
    void addValue_eg(std::string, float);
    void fill_eg();
    void addValue_tau(std::string, float);
    void fill_tau();
    int size();
    std::map<std::string, float>* get_eg(int);
    std::map<std::string, float>* get_tau(int);
  private:
    std::map<std::string, float> m_values_tem_eg;
    std::vector<std::map<std::string, float>*> m_allvalues_eg;
    std::map<std::string, float> m_values_tem_tau;
    std::vector<std::map<std::string, float>*> m_allvalues_tau;
  };
}
CLASS_DEF( LVL1::eFEXOutputCollection, 32202262 , 1 )

