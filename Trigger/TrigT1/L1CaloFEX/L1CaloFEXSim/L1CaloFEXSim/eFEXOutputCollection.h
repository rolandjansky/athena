/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file eFEXOutputCollection.h
 * @brief create ntuples output
 *
 * File contains an class which stores the output of eFEX simulation
 *
 * @author Tong Qiu (tong.qiu@cern.ch)
 * @date 04 Jun 2021
 */

#pragma once
#include "AthenaKernel/CLASS_DEF.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>

namespace LVL1 {
  class eFEXOutputCollection
  {
   /**
   * @class eFEXOutputCollection
   * @brief an class which stores the output of eFEX simulation for the use of eFEXNtupleWriter.
   */
  public:
    /// @brief constructor
    eFEXOutputCollection();

    /// @brief Destructor
    ~eFEXOutputCollection();

    /// @brief Removes all values from the vectors for the next event
    void clear();

    /**
     * @brief add a value related to the e-gamma algorithm for a TOB
     *
     * @param key name of the value
     * @param value the value
     *
     */
    void addValue_eg(std::string key, float value);

    /// @brief Save all e-gamma values. Use only after finishing defining all e-gamma values for one TOB.
    void fill_eg();

    /**
     * @brief define a value related to the tau algorithm for a TOB
     *
     * @param key name of the value
     * @param value value
     *
     */
    void addValue_tau(std::string key, float value);

    /// @brief Save all tau values. Use only after finishing defining all tau values for one TOB.
    void fill_tau();

    /// @brief get total number of tau TOBs saved
    int tau_size() const;

    /// @brief get total number of eg TOBs saved
    int size() const;

    /// @brief get all e-gamma related values the ith TOB
    std::map<std::string, float>* get_eg(int) const;

    /// @brief get all tau related values the ith TOB
    std::map<std::string, float>* get_tau(int) const;

    /// @brief add the eEFX number of a TOB
    void addeFexNumber(int);

    /// @brief get the eFEX numbers of all TOBs
    std::vector<int> geteFexNumber() const;

    /// @brief add a 32-bit e-gamma TOB word
    void addEMtob(uint32_t);
    
    /// @brief get all e-gamma TOB words of an event
    std::vector<uint32_t> getEMtob() const;

    /// @brief setting to true if ntuple output is needed
    void setdooutput(bool);

    /// @brief return to true if ntuple output is needed
    bool getdooutput() const;
  
  private:
    bool m_dooutput; ///< if write Ntuple
    std::vector<int> m_eFexNumber; ///< vector of eFEX numbers
    std::vector<uint32_t> m_emtob; ///< vector of TOB words
    /// e-gamma related values of a TOB
    std::map<std::string, float> m_values_tem_eg;
    /// e-gamma related values of all TOBs in an event
    std::vector<std::map<std::string, float>*> m_allvalues_eg;
    /// tau related values of a TOB
    std::map<std::string, float> m_values_tem_tau;
    /// rau related values of all TOBs in an event
    std::vector<std::map<std::string, float>*> m_allvalues_tau;
  };
}
CLASS_DEF( LVL1::eFEXOutputCollection, 32202262 , 1 )

