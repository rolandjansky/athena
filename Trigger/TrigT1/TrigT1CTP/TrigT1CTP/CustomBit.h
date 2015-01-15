/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CUSTOMBIT_H
#define TRIGT1CTP_CUSTOMBIT_H

// STL includes:
#include <bitset>

// monitoring from HLT
#include "TrigInterfaces/IMonitoredAlgo.h"

// Local includes:
#include "TrigT1CTP/ResultBuilder.h"

namespace LVL1CTP {

  // Forward include(s) in local namespace:
  class CTPSimulation;

  /**
   *
   *   @short Helper class for monitoring CTP results
   *
   * Helper classs for monitoring CTP results (PIT, TBP, TAP, TAV).
   * Data access is via the LVL1::ResultBuilder class.
   *
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: CustomBit.h,v 1.2 2008-09-24 15:12:12 efeld Exp $
   *
   */

  class CustomBit : public IMonitoredAlgo::IGetter {

  public:
    
    //! default constructor, not implemented!
    CustomBit()
      : m_algo(0), m_func(0), m_resultBuilder(0), m_result(0) {}
    //! constructor
    CustomBit(const CTPSimulation* algo, Result func)
      : m_algo(algo), m_func(func), m_resultBuilder(0), m_result(0) {}

    //! return size of data
    /* function performs caching of result bit vector and must be
       called always before CustomBit::get() function. This is the
       case for the usage in TrigGenericMonitoringTool class.  */
    virtual unsigned int size() const;
    //! indexed access to data
    inline virtual double get(unsigned pos) const;
    
  private:

    const CTPSimulation* m_algo;            //!< pointer to algorithm
    const Result m_func;                    //!< pointer to data access function

    mutable const ResultBuilder* m_resultBuilder;   //!< cache for pointer to current ResultBuilder object
    mutable std::bitset<512> m_result;              //!< cache for result bitset

  };

  // inline member functions

  double CustomBit::get(unsigned pos) const { return m_result[pos] ? pos : -1.; }

}

#endif // TRIGT1CTP_CUSTOMBIT_H
