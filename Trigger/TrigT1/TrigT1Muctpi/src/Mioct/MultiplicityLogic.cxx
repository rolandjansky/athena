/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MultiplicityLogic.cxx 472835 2011-12-06 21:05:16Z stelzer $

// Local include(s):
#include "MultiplicityLogic.h"
#include "SectorID.h"
#include "MultiplicityStrategy.h"
#include "InclusiveMultStrategy.h"
#include "ExclusiveMultStrategy.h"
#include "XMLMultStrategy.h"

namespace LVL1MUCTPI {

   /**
    * The constructor creates all the possible multiplicity handling
    * strategies, and puts them in its central "catalogue". By default
    * it assigns the INCLUSIVE strategy as active.
    */
   MultiplicityLogic::MultiplicityLogic()
      : m_logger( "MultiplicityLogic" ), m_currentStrategy( 0 ),
        m_strategyMap() {

      // Create the strategies:
      m_strategyMap[ INCLUSIVE ] = new InclusiveMultStrategy();
      m_strategyMap[ EXCLUSIVE ] = new ExclusiveMultStrategy();
      m_strategyMap[ XML ]       = new XMLMultStrategy();

      // "Activate" the inclusive strategy:
      m_currentStrategy = m_strategyMap[ INCLUSIVE ];
      std::string infoString ;
      infoString = "Multiplicity Logic with default strategy " +
         m_currentStrategy->getName() + " installed.";
      REPORT_MSG(DEBUG, infoString); 
   }

   /**
    * The destructor has to be careful to delete all the strategies that were
    * created in the constructor.
    */
   MultiplicityLogic::~MultiplicityLogic() {

      for( std::map< MultiplicityCalcType, MultiplicityStrategy* >::iterator it =
              m_strategyMap.begin(); it != m_strategyMap.end(); ++it ) {
         delete it->second;
      }
      m_strategyMap.clear();

   }

   /**
    * The strategy can be any one of the strategies defined in the
    * MultiplicityCalcType enumerator.
    *
    * @param type Type of the strategy that should be used
    */
   void MultiplicityLogic::changeStrategy( MultiplicityCalcType type ) {

      m_currentStrategy = m_strategyMap[ type ];
      std::string infoString = "Current Strategy changed to " +
         m_currentStrategy->getName();
      m_logger.send( INFO, infoString );

      return;

   }

   void MultiplicityLogic::initializeXML( const std::string& xmlFile ) {

      XMLMultStrategy* xmlstrat =
         dynamic_cast< XMLMultStrategy* >( m_strategyMap[ XML ] );
      if( ! xmlstrat ) {
         m_logger.send( FATAL, "Couldn't cast XMLMultStrategy in initializeXML()!" );
         return;
      }

      xmlstrat->initialize( xmlFile );
      return;

   }

   /**
    * This function should be called by all parts of the simulation when
    * creating a PtMultiplicitySet object.
    *
    * @param pt The p<sub>T</sub> threshold of the candidate
    * @param id The sector that produced the muon candidate
    * @param firstCandidate <code>true</code> if the candidate is the first one in
    *                       the sector, <code>false</code> otherwise
    * @returns The multiplicity word from one muon candidate
    */
   PtMultiplicitySet
   MultiplicityLogic::calculateMultiplicity( unsigned int pt,
                                             const SectorID& id,
                                             bool firstCandidate ) const {

      //
      // Use the active strategy to create the multiplicity object:
      //
      PtMultiplicitySet result;
      if ( m_currentStrategy ){
         result = m_currentStrategy->calculateMultiplicity( pt, id, firstCandidate );
      } else {
         m_logger.send( ERROR,
                        "No overlap strategy set, cannot calculate multiplicity!" );
      }

      return result;
   }

} // namespace LVL1MUCTPI
