/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1RESULT_CTPROI_H
#define TRIGT1RESULT_CTPROI_H

// std include(s):
#include <stdint.h>

namespace ROIB {

  /**
   * 
   *   @short ROIB::CTPRoI contains a RoI delivered by the CTP
   *
   *          The CTPRoI class is mainly used in the CTPResult class.
   *
   *     @see ROIB::CTPResult
   *
   *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
   *
   * @version \$Id: CTPRoI.h,v 1.5 2008-12-08 21:43:16 efeld Exp $
   *
   */

  class CTPRoI {

  public:
    //! constructor: setting the RoI
    CTPRoI( uint32_t r)
      : m_roIWord( r ) {}

    //! default constructor: init RoI to 0
    CTPRoI()
      : m_roIWord( 0 ) {}

    //! empty default destructor
    ~CTPRoI() = default;

    /* Methods: */

    //! returning the RoI
    uint32_t roIWord() const { return m_roIWord; }

  private:
    //! The uint32_t data member of the RoI
    uint32_t m_roIWord;

  }; // class CTPRoI

} // namespace ROIB

#endif // TRIGT1RESULT_CTPROI_H
