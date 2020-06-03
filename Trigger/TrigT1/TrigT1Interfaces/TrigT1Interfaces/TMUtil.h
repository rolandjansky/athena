// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_TMUTIL_H
#define TRIGT1INTERFACES_TMUTIL_H

// WARNING This is a legacy class used by many of the old configuration classes.
// WARNING It will likely be removed soon. Any needed functionality should be put into
// WARNING other places.

// STL include(s):
#include <string>

// External include(s):
#include "xercesc/dom/DOMNode.hpp"

namespace LVL1CTP {

   /**
    *  @short Legacy class lingering around hopefully unused...
    *
    *         TMUtil is a helper class which does things like conversion from
    *         XML DOM strings to normal strings etc. I want to get rid of that
    *         class in the long run, and some parts of it are really nastily
    *         written ....
    *
    * @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class TMUtil {

   public:
      /** Method returning a string representing a bit pattern for a given
          integer argument. The second argument is the number of bits to use */
      static std::string intToBits( unsigned int, int );

      /* ???? */
      static int powerOfInt( const int );

      /** Translates a XML string to an integer */
      static int DOMToInt( const XMLCh* );

      /** Converts a XML string to a normal std::string */
      static std::string utilTranscode( const XMLCh * );

      /// comparison between Nodename and string
      static bool compareNodeNameString( const xercesc::DOMNode*, const std::string& );

      /// get GeV value from a XML string
      static float getGevValue( const XMLCh* );

      /** Calculates an uint from a string containing only numbers */
      static unsigned int uintFromString( std::string );

      /* ???? */
      static int getMuonThresholdMult( const unsigned int, const int, const int );
      static int getEMThresholdMult( const unsigned int, const int, const int );
      static int getEnergyThreshold( const unsigned int, const int,
                                     const int, const int );

   }; // class TMUtil

} // namespace LVL1CTP

#endif // TRIGT1INTERFACES_TMUTIL_H
