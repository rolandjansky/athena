/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXOutputCollection.h  -  
//                              -------------------
//     begin                : 09 12 2020
//     email                : varsiha.sothilingam@cern.ch
//  **************************************************************************


#ifndef JFEX_OUTPUT_COLLECTION_H
#define JFEX_OUTPUT_COLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>

namespace LVL1 {
  class jFEXOutputCollection
  {
  public:
    jFEXOutputCollection() {};
    ~jFEXOutputCollection();
    void clear();
    void addValue_smallRJet(std::string, float);
    void fill_smallRJet();
//    void addValue_largeRJet(std::string, float);
//    void fill_largeRJet();
    int size();
    std::map<std::string, float>* get_smallRJet(int);
//    std::map<std::string, float>* get_largeRJet(int);
  private:
    std::map<std::string, float> m_values_tem_smallRJet;
    std::vector<std::map<std::string, float>*> m_allvalues_smallRJet;
//    std::map<std::string, float> m_values_tem_largeRJet;
//    std::vector<std::map<std::string, float>*> m_allvalues_largeRJet;
  };
}

CLASS_DEF(LVL1::jFEXOutputCollection, 1317184196 , 1 )

#endif
