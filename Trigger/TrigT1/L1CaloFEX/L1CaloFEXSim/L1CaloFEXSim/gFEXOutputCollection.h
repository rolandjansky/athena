/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXOutputCollection
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************


#ifndef gFEXOutputCollection_H
#define gFEXOutputCollection_H

#include "AthenaKernel/CLASS_DEF.h"
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

namespace LVL1 {
  class gFEXOutputCollection
  {
   //gFEXOutputCollection class stores the output of gFEX simulation for the use of gFEXNtupleWriter.

  public:
    //constructor
    gFEXOutputCollection() {};

    //Destructor
    ~gFEXOutputCollection();


    void clear();

    /**
     * @brief add a value related to the jet finder algorithm for a TOB
     *
     * @param key name of the value
     * @param value the value
     *
     */
    void addValueJet(std::string key, float value);

    //Save all jet values. Use only after finishing defining all jet values for one TOB.
    void fillJet();

    //Get total number of TOBs saved
    int size();

    //Get all jeta related values the ith TOB
    std::unordered_map<std::string, float> getJet(int) const;

    //Add a 32-bit jet TOB word
    void addJetTob(uint32_t);

    //Get all jet TOB words of an event
    std::vector<uint32_t> getJetTob() const;

  private:
    //vector of TOB words
    std::vector<uint32_t> m_jettob;
    // /// jet related values of a TOB
    std::unordered_map<std::string, float> m_values_gFEXJet;
    /// jet related values of all TOBs in an event
    std::vector<std::unordered_map<std::string, float>> m_allvalues_gFEXjet;

  };
}
CLASS_DEF( LVL1::gFEXOutputCollection, 2459353 , 1 )

#endif