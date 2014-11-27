/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigT2MbtsBits_V1_H
#define xAODTrigMinBias_TrigT2MbtsBits_V1_H

#include "AthContainers/AuxElement.h"

class MsgStream;

namespace xAOD {

  class TrigT2MbtsBits_v1 : public SG::AuxElement {
  public:
    ///default ctor
    TrigT2MbtsBits_v1();

    //destructor
    ~TrigT2MbtsBits_v1();

    /** Return the trigger energies of each counter */
    const std::vector<float> &triggerEnergies() const;
    
    void setTriggerEnergies(const std::vector<float> &trigEg);
    
    /** Return the relative times of the triggers */
    const std::vector<float> &triggerTimes() const;
    
    void setTriggerTimes(const std::vector<float> &trigTm);

    /**  Prints out data members to std::cout */
    void print(void) const;
   
    /**  Prints out data members to MsgStream */
//    void print(MsgStream& log) const;
 
    /** A data member to contain the number of MBTS counters */
    static const unsigned int NUM_MBTS = 32;
  };

  /// Helper function for printing the object
//   std::string str(const TrigT2MbtsBits_v1& trigT2MbtsBits);
 
  ///Helper operator for printing the object
//  MsgStream& operator<< (MsgStream& m, const TrigT2MbtsBits_v1& trigT2MbtsBits);

}

#endif
