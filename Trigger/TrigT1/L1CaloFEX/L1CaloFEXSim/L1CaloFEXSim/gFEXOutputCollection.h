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
    gFEXOutputCollection();

    //Destructor
    ~gFEXOutputCollection();


    void clearJets();
    void clearGlobals();

    /**
     * @brief add a value related to the jet finder algorithm for a TOB
     *
     * @param key name of the value
     * @param value the value
     *
     */
    void addValueJet(std::string key, float value);
    void addValueGlobal(std::string key, float value);

    //Save all jet values. Use only after finishing defining all jet values for one TOB.
    void fillJet();
    void fillGlobal();

    //Get total number of TOBs saved
    int jetsSize() const; 
    int globalsSize() const; 

    //Get all jets related values
    std::unordered_map<std::string, float> getJet(int) const;
    std::unordered_map<std::string, float> getGlobal(int) const;

    //Add a 32-bit jet TOB word
    void addJetTob(uint32_t);
    void addGlobalTob(uint32_t);

    //Get all jet TOB words of an event
    std::vector<uint32_t> getJetTob() const;
    std::vector<uint32_t> getGlobalTob() const;

    //setting true if ntuple output is needed
    void setdooutput(bool);

    //return true if ntuple output is needed
    bool getdooutput() const;

  private:
    bool m_dooutput; //if write Ntuple
    //vector of TOB words
    std::vector<uint32_t> m_jettob;
    std::vector<uint32_t> m_globaltob;
    // /// jet related values of a TOB
    std::unordered_map<std::string, float> m_values_gFEXJet;
    std::unordered_map<std::string, float> m_values_gFEXGlobal;
    /// jet related values of all TOBs in an event
    std::vector<std::unordered_map<std::string, float>> m_allvalues_gFEXjet;
    std::vector<std::unordered_map<std::string, float>> m_allvalues_gFEXglobal;

  };
}
CLASS_DEF( LVL1::gFEXOutputCollection, 2459353 , 1 )

#endif
