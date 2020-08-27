/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************
*
* JetEnergyRoI contains to a RoI delivered by the jet/E trigger.
* Contained in the JetEnergyResult class.
* Author: Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
*
********************************************************** */
#ifndef TRIGT1RESULT_JETENERGYROI_H
#define TRIGT1RESULT_JETENERGYROI_H

#include <stdint.h>
#include <vector>

namespace ROIB {

  class JetEnergyRoI {

  public:
    /* A RoI is a uint32_t */
    JetEnergyRoI( uint32_t );
    JetEnergyRoI();
    ~JetEnergyRoI() = default;
                                                                                
    /* Method to update the RoI word format  Obselete*/
    void update();
                                                                                
    /** Method returning the RoI word */
    uint32_t roIWord() const;
    /** Method returning the RoI type (jet, energy, jet-Et)*/
    unsigned int roIType() const;
    /** Method returning electronics address*/
    unsigned int electronicsID() const;
    /** Method returning the list of passed thresholds */
    std::vector<unsigned int> thresholds() const;
    /* Method returning the jet Et Sum*/
    unsigned int jetEt() const;
    /** Methods returning the jet ET cluster values for Run 2 RoIs */
    unsigned int etLarge() const;
    unsigned int etSmall() const;
    /** Method returning Ex, Ey, and Esum */
    unsigned int energyX() const;
    unsigned int energyY() const;
    unsigned int energySum() const;
    /** Eta range for MET/SumET RoIs (0 = full, 1 = restricted) */
    unsigned int etSumType() const;
    /** Method returning patterns for passed etsum and etmiss threhsolds */
    std::vector<unsigned int> etSumThresholds() const;
    std::vector<unsigned int> etMissThresholds() const;
    std::vector<unsigned int> metSigThresholds() const;
    /** Jet RoI version (Run 1 or Run 2) */
    unsigned int jetRoIVersion() const;

  private:
    /* The data member - a uint32_t */
    uint32_t m_roIWord;

  }; // class JetEnergyRoI

} // namespace ROIB

#endif // TRIGT1RESULT_JETENERGYROI_H
