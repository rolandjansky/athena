// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: XMLMultStrategy.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_MIOCT_XMLMULTSTRATEGY_H
#define TRIGT1MUCTPI_MIOCT_XMLMULTSTRATEGY_H

// STL include(s):
#include <string>
#include <map>
#include <list>

// Local include(s):
#include "MultiplicityStrategy.h"
#include "ExclusiveMultStrategy.h"
#include "SectorID.h"
#include "XMLMultCalculator.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   bool operator< ( const std::pair< SectorID, bool >& id1,
                    const std::pair< SectorID, bool >& id2 );

   class XMLMultStrategy : public MultiplicityStrategy {

   public:
      /// Constructor
      XMLMultStrategy();
      XMLMultStrategy( const std::string& xmlMultFile );
      XMLMultStrategy( const XMLMultStrategy& strategy );
      XMLMultStrategy  & operator = ( const XMLMultStrategy  & ) = delete;
      /// Destructor
      ~XMLMultStrategy();

      void initialize( const std::string& xmlMultFile );

      /// Multiplicity handling function
      virtual PtMultiplicitySet calculateMultiplicity( unsigned int pt,
                                                       const SectorID& id,
                                                       bool firstCandidate ) const;

   private:
      ExclusiveMultStrategy                          m_exStrategy;
      std::map< std::pair< SectorID, bool>, const XMLMultCalculator* > m_calculatorMap;
      std::list< XMLMultCalculator >                 m_calculatorList;
      MsgLogger                                      m_logger;

   }; // class XMLMultStrategy

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCT_XMLMULTSTRATEGY_H
