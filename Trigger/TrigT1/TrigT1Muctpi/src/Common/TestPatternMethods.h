// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TestPatternMethods.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_TESTPATTERNMETHODS_H
#define TRIGT1MUCTPI_TESTPATTERNMETHODS_H

/*
*******************************************************************
*
*    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
*
*    @short Header file to define the Methods for TestPattern Genneration
*
*           In this header file
*
*      @see
*   @author  Author: Thorsten Wengler
*  @version $Revision: 362102 $
*
*******************************************************************
*/

namespace LVL1MUCTPI {

  enum PatternMethod { 
    MANUAL,        // Manually fill the sectors booked
    RANDOM,        // Random subset of the booked sectors is filled
    FIXED          // all booked sectors are filled randomly
  };

}

#endif // TRIGT1MUCTPI_TESTPATTERNMETHODS_H
