// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: XMLMultCalculator.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MIOCT_XMLMULTCALCULATOR_H
#define TRIGT1MUCTPI_MIOCT_XMLMULTCALCULATOR_H

// STL include(s):
#include <vector>

// XercesC include(s):
#include <xercesc/dom/DOMNode.hpp>

// Local include(s):
#include "../Common/PtMultiplicitySet.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   class XMLMultCalculator;
   bool operator== ( const XMLMultCalculator& calc1,
                     const XMLMultCalculator& calc2 );

   class XMLMultCalculator {

      friend bool operator== ( const XMLMultCalculator& calc1,
                               const XMLMultCalculator& calc2 );

   public:
      XMLMultCalculator( const xercesc::DOMNode* node = 0 );
      XMLMultCalculator( const XMLMultCalculator& parent );
      ~XMLMultCalculator();

      void initialize( const xercesc::DOMNode* node );

      PtMultiplicitySet calculateMultiplicity( unsigned int pt ) const;

   private:
      std::vector< std::vector< bool > > m_lut;

      MsgLogger m_logger;

   }; // class XMLMultCalculator

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCT_XMLMULTCALCULATOR_H
