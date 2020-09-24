/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************
*
* EMTauRoI contains to a RoI delivered by the egamma. 
* Contained in the EMTauResult class.
* Author: Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
*
********************************************************** */
#ifndef TRIGT1RESULT_EMTAUROI_H
#define TRIGT1RESULT_EMTAUROI_H

#include <stdint.h>
#include <vector>
 
namespace ROIB {

  class EMTauRoI {

  public:
    /* A RoI is a uint32_t */
    EMTauRoI( uint32_t );
    /* Init to 0 */
    EMTauRoI();
    ~EMTauRoI() = default;
    
    /* Method to update the RoI word format */
    void update();

    /* Method returning the RoI */
    uint32_t roIWord() const;
    /* Checks the first few bits of the RoI, to see if this really is an
        EM/tau RoI */
    unsigned int roIType() const;
    /* Return the bit pattern for the electronics address */
    unsigned int electronicsID() const;
    /* Returns a vector containing the thresholds that were passsed (Run 1 only)*/
    std::vector<unsigned int> thresholds() const;
    /* Returns cluster ET value (Run 2 only) */
    unsigned int et() const;
    /* Returns isolation results (Run 2 only) */
    unsigned int isolation() const;

  private:
    /* The uint32_t data member of the RoI */
    uint32_t m_roIWord;

  }; // class RMTauRoI

} // namespace ROIB

#endif // TRIGT1RESULT_EMTAUROI_H
